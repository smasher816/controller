/* Copyright (C) 2014-2017 by Jacob Alexander
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// ----- Includes -----

// Project Includes
#include <Lib/mcu_compat.h>
#include <Lib/chip_version.h>
#include <Lib/entropy.h>
#include <Lib/periodic.h>
#include <Lib/time.h>

// General Includes
#include <buildvars.h>
#include <latency.h>
#include <led.h>
#include <print.h>

// KLL Includes
#include <kll_defs.h>

// Local Includes
#include "cli.h"


// ----- Defines -----

#define CLI_PROMPT VT_BOLD_BLUE ":" VT_NORMAL " "

/* VT100 Escape Sequences */
// Screen Control
#define VT_RESET       "\033r"
#define VT_CLEAR_BELOW "\033[0J"
#define VT_CLEAR_ALL   "\033[2J"
#define VT_CLEAR_LINE  "\033[2K"

// Cursor Control
#define VT_CURSOR_SAVE    "\033[s"
#define VT_CURSOR_RESTORE "\033[u"
#define VT_CURSOR_HOME    "\033[H"

// Arrow Keys
#define VT_UP    "\033[A"
#define VT_DOWN  "\033[B"
#define VT_RIGHT "\033[C"
#define VT_LEFT  "\033[D"

// Special Keys
// Note: Backspace and delete swapped due to linux history
#define KC_DELETE 0x08    // \b
#define KC_BACKSPACE 0x7F // DEL
#define KC_ESC 0x1B

/* Readline bindings */
#define CTRL(x)  (x & 0b00011111)
#define CMD_BEGINNING_OF_LINE     CTRL('A')
#define CMD_BACKWARD_CHAR         CTRL('B')
#define CMD_DELETE_CHAR           KC_DELETE
#define CMD_END_OF_LINE           CTRL('E')
#define CMD_FORWARD_CHAR          CTRL('F')
#define CMD_BACKWARD_DELETE_CHAR  KC_BACKSPACE
#define CMD_COMPLETE              CTRL('I') // \t (tab) *
#define CMD_ACCEPT_LINE           CTRL('J') // \n (Linefeed)
#define CMD_KILL_LINE             CTRL('K')
#define CMD_CLEAR_SCREEN          CTRL('L')
#define CMD_ACCEPT_LINE2          CTRL('M') // \r (Carriage return)
#define CMD_NEXT_HISTORY          CTRL('N')
#define CMD_PREVIOUS_HISTORY      CTRL('P')
#define CMD_TRANSPOSE_CHARS       CTRL('T')
#define CMD_UNIX_LINE_DISCARD     CTRL('U')
#define CMD_UNIX_WORD_RUBOUT      CTRL('W')
#define CMD_BACKWARD_DELETE_CHAR2 CTRL('?')

typedef struct inputBinding {
	char *sequence;
	char c;
} inputBinding;

inputBinding bindingsDict[] = {
	// VT100
	{ VT_UP,    CMD_PREVIOUS_HISTORY },
	{ VT_DOWN,  CMD_NEXT_HISTORY },
	{ VT_RIGHT, CMD_FORWARD_CHAR },
	{ VT_LEFT,  CMD_BACKWARD_CHAR },

	// inputrc defaults
	{ "\033[1~", CMD_BEGINNING_OF_LINE },
	{ "\033[3~", CMD_DELETE_CHAR },
	{ "\033[4~", CMD_END_OF_LINE },
	{ "\033[5~", CMD_BEGINNING_OF_LINE },
	{ "\033[7~", CMD_BEGINNING_OF_LINE },
	{ "\033[8~", CMD_END_OF_LINE },
	{ "\033[H",  CMD_BEGINNING_OF_LINE },
	{ "\033[F",  CMD_END_OF_LINE }
};

// ----- Variables -----

// Basic command dictionary
CLIDict_Entry( clear,     "Clear the screen.");
CLIDict_Entry( cliDebug,  "Enables/Disables hex output of the most recent cli input." );
CLIDict_Entry( colorTest, "Displays a True Color ANSI test sequence to test terminal. If it displays in color, you're good." );
#if defined(_host_)
CLIDict_Entry( exit,      "Host KLL Only - Exits cli." );
#endif
CLIDict_Entry( help,      "You're looking at it :P" );
CLIDict_Entry( latency,   "Show latency of specific modules and routiines. Specify index for a single item" );
CLIDict_Entry( led,       "Enables/Disables indicator LED. Try a couple times just in case the LED is in an odd state." NL "\t\t" COLOR_FG(YELLOW, "Warning") ": May adversely affect some modules..." );
CLIDict_Entry( periodic,  "Set the number of clock cycles between periodic scans." );
CLIDict_Entry( rand,      "If entropy available, print a random 32-bit number." );
CLIDict_Entry( reload,    "Signals microcontroller to reflash/reload." );
CLIDict_Entry( reset,     "Resets the terminal back to initial settings." );
CLIDict_Entry( restart,   "Sends a software restart, should be similar to powering on the device." );
CLIDict_Entry( tick,      "Displays the fundamental tick size, and current ticks since last systick." );
CLIDict_Entry( version,   "Version information about this firmware." );

CLIDict_Def( basicCLIDict, "General Commands" ) = {
	CLIDict_Item( clear ),
	CLIDict_Item( cliDebug ),
	CLIDict_Item( colorTest ),
#if defined(_host_)
	CLIDict_Item( exit ),
#endif
	CLIDict_Item( help ),
	CLIDict_Item( latency ),
	CLIDict_Item( led ),
	CLIDict_Item( periodic ),
	CLIDict_Item( rand ),
	CLIDict_Item( reload ),
	CLIDict_Item( reset ),
	CLIDict_Item( restart ),
	CLIDict_Item( tick ),
	CLIDict_Item( version ),
	{ 0, 0, 0 } // Null entry for dictionary end
};

#if defined(_host_)
int CLI_exit = 0; // When 1, cli signals library to exit (Host-side KLL only)
#endif



// ----- Functions -----

void CLI_clearInput()
{
	CLILineBufferCurrent = 0;
	CLILineBufferLength = 0;
}

// Initialize the CLI
inline void CLI_init()
{
	// History starts empty
	CLIHistoryHead = 0;
	CLIHistoryCurrent = 0;
	CLIHistoryTail = 0;

	// Line starts empty
	CLI_clearInput();

	// Register first dictionary
	CLIDictionariesUsed = 0;
	CLI_registerDictionary( basicCLIDict, basicCLIDictName );

	// Initialize main LED
	init_errorLED();
	CLILEDState = 0;

	// Hex debug mode is off by default
	CLIHexDebugMode = 0;

#if defined(_host_)
	// Make sure we're not exiting right away in Host-side KLL mode
	CLI_exit = 0;
#endif
}

// Query the serial input buffer for any new characters
int CLI_process()
{
	// Current buffer position
	uint8_t prev_buf_pos = 0;

	// Buffer for the new input
	char CLIInputBuffer[CLIInputBufferMaxSize + 1]; // +1 for an additional null
	uint8_t CLIInputBufferLength = 0;

	// Process each character while available
	while ( Output_availablechar() != 0 )
	{
		// Retrieve from output module
		char cur_char = (char)Output_getchar();

		// Make sure buffer isn't full
		if ( CLIInputBufferLength >= CLIInputBufferMaxSize )
		{
			print( NL );
			erro_msg("Serial line buffer is full, dropping character and resetting...");

			// Reset the prompt
			CLI_clearInput();

			return 0;
		}

		// Place into line buffer
		CLIInputBuffer[CLIInputBufferLength++] = cur_char;
	}

	// Display Hex Key Input if enabled
	if ( CLIHexDebugMode && CLIInputBufferLength )
	{
		print(VT_CURSOR_SAVE NL);
		print(VT_CLEAR_LINE);

		uint8_t pos = prev_buf_pos;
		while ( CLIInputBufferLength > pos )
		{
			printHex( CLIInputBuffer[pos++] );
			print(" ");
		}

		print(VT_CURSOR_RESTORE);
	}

	// If buffer has changed, output to screen while there are still characters in the buffer not displayed
	while ( CLIInputBufferLength > prev_buf_pos )
	{
		char c = CLIInputBuffer[prev_buf_pos];

		// Handle multi-character combos such as the
		// \e[ escape code in vt100 compatible terminals
		if ( CLIInputBufferLength >= prev_buf_pos + 2
			&& CLIInputBuffer[ prev_buf_pos ] == KC_ESC
			&& CLIInputBuffer[ prev_buf_pos + 1] == '[' )
		{
			// Null terminate string for comparing
			CLIInputBuffer[CLIInputBufferLength] = '\0';

			for (uint8_t i = 0; i < sizeof(bindingsDict)/sizeof(bindingsDict[0]); i++)
			{
				if (eqStr(&CLIInputBuffer[prev_buf_pos], bindingsDict[i].sequence) == -1)
				{
					c = bindingsDict[i].c;
					prev_buf_pos += 3; //strlen(defaultBindings[i].cmd);
					break;
				}

			}
		}

		// Handle single characters
		switch ( c )
		{
		case CMD_ACCEPT_LINE:
		case CMD_ACCEPT_LINE2:
			CLI_saveHistory( CLILineBuffer );

			// Process the current line buffer
			CLI_commandLookup();

			// Keep the array circular, discarding the older entries
			if ( CLIHistoryTail < CLIHistoryHead )
				CLIHistoryHead = ( CLIHistoryHead + 1 ) % CLIMaxHistorySize;
			CLIHistoryTail++;
			if ( CLIHistoryTail == CLIMaxHistorySize )
			{
				CLIHistoryTail = 0;
				CLIHistoryHead = 1;
			}

			CLIHistoryCurrent = CLIHistoryTail; // 'Up' starts at the last item
			CLI_saveHistory( NULL ); // delete the old temp buffer

			CLI_clearInput();

			// Check if we need to exit right away
#if defined(_host_)
			if ( CLI_exit )
			{
				CLI_exit = 0;
				return 1;
			}
#endif

			// XXX There is a potential bug here when resetting the buffer (losing valid keypresses)
			//     Doesn't look like it will happen *that* often, so not handling it for now -HaaTa
			break;

		case CMD_COMPLETE:
			// Tab completion for the current command
			CLI_tabCompletion();

			//CLILineBufferCurrent--; // Remove the Tab

			// XXX There is a potential bug here when resetting the buffer (losing valid keypresses)
			//     Doesn't look like it will happen *that* often, so not handling it for now -HaaTa
			break;

		case CMD_PREVIOUS_HISTORY:
			if ( CLIHistoryCurrent == CLIHistoryTail )
			{
				// Is first time pressing arrow. Save the current buffer
				//CLILineBuffer[ CLILineBufferCurrent ] = '\0';
				CLI_saveHistory( CLILineBuffer );
			}

			// Grab the previus item from the history if there is one
			if ( RING_PREV( CLIHistoryCurrent ) != RING_PREV( CLIHistoryHead ) )
				CLIHistoryCurrent = RING_PREV( CLIHistoryCurrent );
			CLI_retreiveHistory( CLIHistoryCurrent );
			break;

		case CMD_NEXT_HISTORY:
			// Grab the next item from the history if it exists
			if ( RING_NEXT( CLIHistoryCurrent ) != RING_NEXT( CLIHistoryTail ) )
				CLIHistoryCurrent = RING_NEXT( CLIHistoryCurrent );
			CLI_retreiveHistory( CLIHistoryCurrent );
			break;

		case CMD_BACKWARD_CHAR:
			if ( CLILineBufferCurrent > 0 ) {
				CLILineBufferCurrent--;
			}
			break;

		case CMD_FORWARD_CHAR:
			if ( CLILineBufferCurrent < CLILineBufferLength ) {
				CLILineBufferCurrent++;
			}
			break;

		case CMD_BEGINNING_OF_LINE:
			CLILineBufferCurrent = 0;
			break;

		case CMD_END_OF_LINE:
			CLILineBufferCurrent = CLILineBufferLength;
			break;

		case CMD_BACKWARD_DELETE_CHAR:
		case CMD_BACKWARD_DELETE_CHAR2:
			// If there are characters in the buffer
			if ( CLILineBufferCurrent > 0 )
			{
				// Remove character from current position in the line buffer
				for (uint8_t i = CLILineBufferCurrent; i <= CLILineBufferLength; i++)
				{
					CLILineBuffer[i-1] = CLILineBuffer[i];
				}

				CLILineBufferCurrent--;
				CLILineBufferLength--;
			}
			break;

		case CMD_DELETE_CHAR:
			if ( CLILineBufferCurrent < CLILineBufferLength )
			{
				for (uint8_t i = CLILineBufferCurrent; i < CLILineBufferLength; i++)
				{
					CLILineBuffer[i] = CLILineBuffer[i+1];
				}
				CLILineBufferLength--;
			}
			break;

		// Delete to beginning of line
		case CMD_UNIX_LINE_DISCARD:
			for (uint8_t i = 0; i < CLILineBufferCurrent; i++)
			{
				CLILineBuffer[i] = CLILineBuffer[CLILineBufferCurrent+i];
			}
			break;

		// Delete to end of line
		case CMD_KILL_LINE:
			CLILineBufferLength = CLILineBufferCurrent;
			break;

		// Delete previous word
		case CMD_UNIX_WORD_RUBOUT:
			for (uint8_t i = CLILineBufferCurrent-1; i >= 0; i--) {
				if (CLILineBuffer[i] == ' ')
				{
					CLILineBufferCurrent = i+1;
					CLILineBufferLength = i+1;
					break;
				}
			}
			break;

		case CMD_TRANSPOSE_CHARS:
			if ( CLILineBufferLength >= 2)
			{
				// Some quick edge case checks
				int i = CLILineBufferCurrent > 0 ? CLILineBufferCurrent : 1;
				if ( i >= CLILineBufferLength )
					i = CLILineBufferLength - 1;

				// Perform the swap
				char temp = CLILineBuffer[i-1];
				CLILineBuffer[i-1] = CLILineBuffer[i];
				CLILineBuffer[i] = temp;
				if ( CLILineBufferCurrent < CLILineBufferLength) {
					CLILineBufferCurrent++;
				}
			}
			break;

		case CMD_CLEAR_SCREEN:
			cliFunc_clear( NULL );
			CLI_clearInput();
			break;

		case CTRL('C'):
			print(NL);
			CLI_clearInput();
			break;

		default:
			// Add non-special characters to the line if there is room
			if ( (c > 31) && (CLILineBufferCurrent < CLILineBufferMaxSize) )
			{
				if (CLILineBufferCurrent < CLILineBufferLength)
				{
					for (uint8_t i = CLILineBufferLength; i > CLILineBufferCurrent; i--)
					{
						CLILineBuffer[i] = CLILineBuffer[i-1];
					}
				}

				CLILineBuffer[CLILineBufferCurrent++] = c;
				CLILineBufferLength++;
			}

			break;
		}

		prev_buf_pos++;

		// Place a null on the end (to use with string print)
		CLILineBuffer[CLILineBufferLength] = '\0';

		// Output line to screen
		print(VT_CLEAR_LINE "\r" CLI_PROMPT);
		dPrint( CLILineBuffer );
		print(VT_NORMAL);

		if (CLILineBufferCurrent < CLILineBufferLength) {
			//Move cursor to correct position
			print("\033[");
			printInt8(CLILineBufferLength - CLILineBufferCurrent);
			print("D");
		}
	}

	return 0;
}

// Takes a string, returns two pointers
//  One to the first non-space character
//  The second to the next argument (first NULL if there isn't an argument). delimited by a space
//  Places a NULL at the first space after the first argument
void CLI_argumentIsolation( char* string, char** first, char** second )
{
	// Mark out the first argument
	// This is done by finding the first space after a list of non-spaces and setting it NULL
	char* cmdPtr = string - 1;
	while ( *++cmdPtr == ' ' ); // Skips leading spaces, and points to first character of cmd

	// Locates first space delimiter
	char* argPtr = cmdPtr + 1;
	while ( *argPtr != ' ' && *argPtr != '\0' )
		argPtr++;

	// Point to the first character of args or a NULL (no args) and set the space delimiter as a NULL
	(++argPtr)[-1] = '\0';

	// Set return variables
	*first = cmdPtr;
	*second = argPtr;
}

// Scans the CLILineBuffer for any valid commands
void CLI_commandLookup()
{
	// Ignore command if buffer is 0 length
	if ( CLILineBufferCurrent == 0 )
		return;

	// Set the last+1 character of the buffer to NULL for string processing
	CLILineBuffer[CLILineBufferCurrent] = '\0';

	// Retrieve pointers to command and beginning of arguments
	// Places a NULL at the first space after the command
	char* cmdPtr;
	char* argPtr;
	CLI_argumentIsolation( CLILineBuffer, &cmdPtr, &argPtr );

	// Scan array of dictionaries for a valid command match
	for ( uint8_t dict = 0; dict < CLIDictionariesUsed; dict++ )
	{
		// Parse each cmd until a null command entry is found, or an argument match
		for ( uint8_t cmd = 0; CLIDict[dict][cmd].name != 0; cmd++ )
		{
			// Compare the first argument and each command entry
			if ( eqStr( cmdPtr, (char*)CLIDict[dict][cmd].name ) == -1 )
			{
				// Run the specified command function pointer
				//   argPtr is already pointing at the first character of the arguments
				(*(void (*)(char*))CLIDict[dict][cmd].function)( argPtr );

				return;
			}
		}
	}

	// No match for the command...
	print( NL );
	erro_dPrint("\"", CLILineBuffer, "\" is not a valid command...type " COLOR_FG(PURPLE, "help"));
}

// Registers a command dictionary with the CLI
void CLI_registerDictionary( const CLIDictItem *cmdDict, const char* dictName )
{
	// Make sure this max limit of dictionaries hasn't been reached
	if ( CLIDictionariesUsed >= CLIMaxDictionaries )
	{
		erro_msg("Max number of dictionaries defined already...");
		return;
	}

	// Add dictionary
	CLIDictNames[CLIDictionariesUsed] = (char*)dictName;
	CLIDict[CLIDictionariesUsed++] = (CLIDictItem*)cmdDict;
}

inline void CLI_tabCompletion()
{
	// Ignore command if buffer is 0 length
	if ( CLILineBufferLength == 0 )
		return;

	// Retrieve pointers to command and beginning of arguments
	// Places a NULL at the first space after the command
	char* cmdPtr;
	char* argPtr;
	CLI_argumentIsolation( CLILineBuffer, &cmdPtr, &argPtr );

	// Tab match pointer
	char* tabMatch = 0;
	uint8_t matches = 0;

	print(VT_CURSOR_SAVE NL VT_CLEAR_BELOW VT_PURPLE);

	// Scan array of dictionaries for a valid command match
	for ( uint8_t dict = 0; dict < CLIDictionariesUsed; dict++ )
	{
		// Parse each cmd until a null command entry is found, or an argument match
		for ( uint8_t cmd = 0; CLIDict[dict][cmd].name != 0; cmd++ )
		{
			// Compare the first argument piece to each command entry to see if it is "like"
			// NOTE: To save on processing, we only care about the commands and ignore the arguments
			//       If there are arguments, and a valid tab match is found, buffer is cleared (args lost)
			//       Also ignores full matches
			if ( eqStr( cmdPtr, (char*)CLIDict[dict][cmd].name ) == 0 )
			{
				matches++;
				tabMatch = (char*)CLIDict[dict][cmd].name;
				print("  ");
				print(tabMatch);
				print(NL);
			}
		}
	}

	print(VT_CURSOR_RESTORE VT_NORMAL);

	if ( matches == 1 )
	{
		print(VT_CLEAR_BELOW);

		// Reset the buffer
		CLILineBufferLength = 0;

		// There are no index counts, so just copy the whole string to the input buffer
		while ( *tabMatch != '\0' )
		{
			CLILineBuffer[CLILineBufferLength++] = *tabMatch++;
			CLILineBufferCurrent = CLILineBufferLength;
		}
	}
}

inline int CLI_wrap( int kX, int const kLowerBound, int const kUpperBound )
{
	int range_size = kUpperBound - kLowerBound + 1;

	if ( kX < kLowerBound )
		kX += range_size * ((kLowerBound - kX) / range_size + 1);

	return kLowerBound + (kX - kLowerBound) % range_size;
}

inline void CLI_saveHistory( char *buff )
{
	if ( buff == NULL )
	{
		//clear the item
		CLIHistoryBuffer[ CLIHistoryTail ][ 0 ] = '\0';
		return;
	}

        // Don't write empty lines to the history
        const char *cursor = buff;
        while (*cursor == ' ') { cursor++; } // advance past the leading whitespace
        if (*cursor == '\0') { return ; }

	// Copy the line to the history
	int i;
	for (i = 0; i < CLILineBufferCurrent; i++)
	{
		CLIHistoryBuffer[ CLIHistoryTail ][ i ] = CLILineBuffer[ i ];
	}
}

void CLI_retreiveHistory( int index )
{
	char *histMatch = CLIHistoryBuffer[ index ];

	// Reset the buffer
	CLILineBufferCurrent = 0;

	// There are no index counts, so just copy the whole string to the input buffe
	CLILineBufferLength = 0;
	while ( *histMatch != '\0' )
	{
		CLILineBuffer[ CLILineBufferLength++ ] = *histMatch++;
		CLILineBufferCurrent = CLILineBufferLength;
	}
}



// ----- CLI Command Functions -----

void cliFunc_clear( char* args)
{
	print(VT_CLEAR_ALL VT_CURSOR_HOME);
	print(CLI_PROMPT);
}

void cliFunc_cliDebug( char* args )
{
	// Toggle Hex Debug Mode
	if ( CLIHexDebugMode )
	{
		print( NL );
		info_msg("Hex debug mode disabled...");
		CLIHexDebugMode = 0;
	}
	else
	{
		print( NL );
		info_msg("Hex debug mode enabled...");
		CLIHexDebugMode = 1;
	}
}

void cliFunc_colorTest( char* args )
{
	print( NL );
	print(TRUECOLOR_FG ";255;100;0mTRUECOLOR" VT_NORMAL NL);
}

#if defined(_host_)
void cliFunc_exit( char* args )
{
	CLI_exit = 1;
}
#endif

void cliFunc_help( char* args )
{
	// Scan array of dictionaries and print every description
	//  (no alphabetical here, too much processing/memory to sort...)
	for ( uint8_t dict = 0; dict < CLIDictionariesUsed; dict++ )
	{
		// Print the name of each dictionary as a title
		print( NL VT_BOLD_GREEN );
		_print( CLIDictNames[dict] ); // This print is requride by AVR (flash)
		print( VT_NORMAL NL );

		// Parse each cmd/description until a null command entry is found
		for ( uint8_t cmd = 0; CLIDict[dict][cmd].name != 0; cmd++ )
		{
			dPrintStrs(" " VT_PURPLE, CLIDict[dict][cmd].name, VT_NORMAL);

			// Determine number of spaces to tab by the length of the command and TabAlign
			uint8_t padLength = CLIEntryTabAlign - lenStr( (char*)CLIDict[dict][cmd].name );
			while ( padLength-- > 0 )
				print(" ");

			_print( CLIDict[dict][cmd].description ); // This print is required by AVR (flash)
			print( NL );
		}
	}
}

void printLatency( uint8_t resource )
{
	printInt8( resource );
	print(":");
	print( Latency_query_name( resource ) );
	print("\t");
	printInt32( Latency_query( LatencyQuery_Count, resource ) );
	print("\t");
	printInt32( Latency_query( LatencyQuery_Min, resource ) );
	print("\t");
	printInt32( Latency_query( LatencyQuery_Average, resource ) );
	print("\t");
	printInt32( Latency_query( LatencyQuery_Last, resource ) );
	print("\t");
	printInt32( Latency_query( LatencyQuery_Max, resource ) );
}

void cliFunc_latency( char* args )
{
	// Parse number from argument
	//  NOTE: Only first argument is used
	char* arg1Ptr;
	char* arg2Ptr;
	CLI_argumentIsolation( args, &arg1Ptr, &arg2Ptr );

	print( NL );
	print("Latency" NL );
	print("<i>:<module>\t<count>\t<min>\t<avg>\t<last>\t<max>");

	// If no arguments print all
	if ( arg1Ptr[0] == '\0' )
	{
		// Iterate through all the latency resources
		for ( uint8_t c = 0; c < Latency_resources(); c++ )
		{
			print( NL );
			printLatency( c );
		}
	}
	else
	{
		print( NL );
		if ( arg1Ptr[0] < Latency_resources() )
		{
			printLatency( arg1Ptr[0] );
		}
	}
}

void cliFunc_led( char* args )
{
	CLILEDState ^= 1 << 1; // Toggle between 0 and 1
	errorLED( CLILEDState ); // Enable/Disable error LED
}

void cliFunc_periodic( char* args )
{
	// Parse number from argument
	//  NOTE: Only first argument is used
	char* arg1Ptr;
	char* arg2Ptr;
	CLI_argumentIsolation( args, &arg1Ptr, &arg2Ptr );
	print( NL );

	// Set clock cycles if an argument is given
	if ( arg1Ptr[0] != '\0' )
	{
		uint32_t cycles = (uint32_t)numToInt( arg1Ptr );

		Periodic_init( cycles );
	}

	// Show number of clock cycles between periods
	info_print("Period Clock Cycles: ");
	printInt32( Periodic_cycles() );
}

void cliFunc_rand( char* args )
{
	print( NL );

	// Check if entropy available
	if ( !rand_available() )
	{
		info_msg("No entropy available!");
		return;
	}

	info_print("Rand: ");
	printHex32( rand_value32() );
}

void cliFunc_reload( char* args )
{
	if ( flashModeEnabled_define == 0 )
	{
		print( NL );
		warn_msg("flashModeEnabled not set, cancelling firmware reload...");
		info_msg("Set flashModeEnabled to 1 in your kll configuration.");
		return;
	}

	// Request to output module to be set into firmware reload mode
	Output_firmwareReload();
}

void cliFunc_reset( char* args )
{
	print(VT_RESET);
}

void cliFunc_restart( char* args )
{
	// Trigger an overall software reset
	Output_softReset();
}

void cliFunc_tick( char* args )
{
	print( NL );

	// Get current time
	Time now = Time_now();

	// Display <systick>:<cycleticks since systick>
	info_print("ns per cycletick: ");
	print( Time_ticksPer_ns_str );
	print( NL );
	info_msg("<systick ms>:<cycleticks since systick>");
	printInt32( now.ms );
	print(":");
	printInt32( now.ticks );
	print( NL );
}

void cliFunc_version( char* args )
{
	print( NL );
	print( COLOR_FG(BOLD, " Revision:      ") CLI_Revision          NL );
	print( COLOR_FG(BOLD, " Revision #:    ") CLI_RevisionNumberStr NL );
	print( COLOR_FG(BOLD, " Version:       ") CLI_Version " (+" );
#if CLI_RevisionNumber && CLI_VersionRevNumber
	printInt16( CLI_RevisionNumber - CLI_VersionRevNumber );
#endif
	print( ":" CLI_VersionRevNumberStr ")" NL );
	print( COLOR_FG(BOLD, " Branch:        ") CLI_Branch            NL );
	print( COLOR_FG(BOLD, " Tree Status:   ") CLI_ModifiedStatus CLI_ModifiedFiles NL );
	print( COLOR_FG(BOLD, " Repo Origin:   ") CLI_RepoOrigin        NL );
	print( COLOR_FG(BOLD, " Commit Date:   ") CLI_CommitDate        NL );
	print( COLOR_FG(BOLD, " Commit Author: ") CLI_CommitAuthor      NL );
	print( COLOR_FG(BOLD, " Build Date:    ") CLI_BuildDate         NL );
	print( COLOR_FG(BOLD, " Build OS:      ") CLI_BuildOS           NL );
	print( COLOR_FG(BOLD, " Compiler:      ") CLI_BuildCompiler     NL );
	print( COLOR_FG(BOLD, " Architecture:  ") CLI_Arch              NL );
	print( COLOR_FG(BOLD, " Chip Compiled: ") CLI_ChipShort " (" CLI_Chip ")" NL );
	print( COLOR_FG(BOLD, " CPU:           ") CLI_CPU               NL );
	print( COLOR_FG(BOLD, " Device:        ") CLI_Device            NL );
	print( COLOR_FG(BOLD, " Modules:       ") CLI_Modules           NL );
#if defined(_teensy_)
	print( COLOR_FG(BOLD, " Teensy:        ") "Yes"                 NL );
#endif
#if defined(_kinetis_)
	print( NL );
	printColor( BOLD, " CPU Detected:  " );
	print( ChipVersion_lookup() );
	print( NL);

	printColor( BOLD, " CPU Id:        " );
	printHex32( SCB_CPUID );
	print( NL "  (Implementor:");
	print( ChipVersion_cpuid_implementor() );
	print( ":" );
	printHex32( SCB_CPUID_IMPLEMENTOR );
	print( ")(Variant:" );
	printHex32( SCB_CPUID_VARIANT );
	print( ")(Arch:" );
	printHex32( SCB_CPUID_ARCH );
	print( ")(PartNo:" );
	print( ChipVersion_cpuid_partno() );
	print( ":" );
	printHex32( SCB_CPUID_PARTNO );
	print( ")(Revision:" );
	printHex32( SCB_CPUID_REVISION );
	print( ")" NL );

	printColor( BOLD, " Device Id:     " );
	printHex32( SIM_SDID );
	print( NL "  (Pincount:");
	print( ChipVersion_pincount[ SIM_SDID_PINID ] );
	print( ":" );
	printHex32( SIM_SDID_PINID );
	print( ")(Family:" );
	print( ChipVersion_familyid[ SIM_SDID_FAMID ] );
	print( ":" );
	printHex32( SIM_SDID_FAMID );
	print( ")(Die:" );
	printHex32( SIM_SDID_DIEID );
	print( ")(Rev:" );
	printHex32( SIM_SDID_REVID );
	print( ")" NL );

	printColor( BOLD, " Flash Cfg:     " );
	printHex32( SIM_FCFG1 & 0xFFFFFFF0 );
	print( NL "  (FlexNVM:" );
	printInt16( ChipVersion_nvmsize[ SIM_FCFG1_NVMSIZE ] );
	print( "kB)(PFlash:" );
	printInt16( ChipVersion_pflashsize[ SIM_FCFG1_PFSIZE ] );
	print( "kB)(EEPROM:" );
	printInt16( ChipVersion_eepromsize[ SIM_FCFG1_EESIZE ] );
	print( ")(DEPART:" );
	printHex32( SIM_FCFG1_DEPART );
	print( ")" NL );

	printColor( BOLD, " RAM:           " );
	printInt16( ChipVersion_ramsize[ SIM_SOPT1_RAMSIZE ] );
	print( " kB" NL );

	printColor( BOLD, " Unique Id:     " );
	printHex32_op( SIM_UIDH, 8 );
	printHex32_op( SIM_UIDMH, 8 );
	printHex32_op( SIM_UIDML, 8 );
	printHex32_op( SIM_UIDL, 8 );
#elif defined(_sam_)
#elif defined(_avr_at_)
#elif defined(_host_)
#else
#warning "No unique id defined."
#endif
}

