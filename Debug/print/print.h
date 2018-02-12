/* Copyright (C) 2011-2017 by Jacob Alexander
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

#pragma once

// ----- Includes -----

#include <Lib/mcu_compat.h>

// Compiler Includes
#if defined(_avr_at_)
#include <avr/pgmspace.h>
#endif

// Project Includes
#include <output_com.h>

// Local Includes
#include "colors.h"



// ----- Defines -----
#define NL "\r\n"



// ----- Functions and Corresponding Function Aliases -----

/* XXX
 * Note that all the variadic functions below, take comma separated string lists, they are purposely not printf style (simplicity)
 */

// Function Aliases
#define dPrint(c)         Output_putstr(c)
#define dPrintStr(c)      Output_putstr(c)
#define dPrintStrs(...)   printstrs(__VA_ARGS__, "\0\0\0")      // Convenience Variadic Macro
#define dPrintStrNL(c)    dPrintStrs       (c, NL)              // Appends New Line Macro
#define dPrintStrsNL(...) printstrs(__VA_ARGS__, NL, "\0\0\0")  // Appends New Line Macro

// Special Msg Constructs (Uses VT100 tags)
#define dPrintMsg(color,msg,...) \
			  printstrs(COLOR_FG(color, msg) " - ", __VA_ARGS__, NL, "\0\0\0")
#define printMsgNL(color,msg,str) \
			  print(COLOR_FG(color, msg) " - " str NL)
#define printMsg(color,msg,str) \
			  print(COLOR_FG(color, msg) " - " str)
#define printColor(color, str) \
			  print(COLOR_FG(color, str))

// Info Messages
#define info_dPrint(...)  dPrintMsg        (BOLD_GREEN,   "INFO",    __VA_ARGS__) // Info Msg
#define info_print(str)   printMsgNL       (BOLD_GREEN,   "INFO",    str)         // Info Msg
#define info_msg(str)     printMsg         (BOLD_GREEN,   "INFO",    str)         // Info Msg

// Warning Messages
#define warn_dPrint(...)  dPrintMsg        (BOLD_YELLOW,   "WARNING", __VA_ARGS__) // Warning Msg
#define warn_print(str)   printMsgNL       (BOLD_YELLOW,   "WARNING", str)         // Warning Msg
#define warn_msg(str)     printMsg         (BOLD_YELLOW,   "WARNING", str)         // Warning Msg

// Error Messages
// TODO BLINK
#define erro_dPrint(...)  dPrintMsg        (BOLD_RED, "ERROR",   __VA_ARGS__) // Error Msg
#define erro_print(str)   printMsgNL       (BOLD_RED, "ERROR",   str)         // Error Msg
#define erro_msg(str)     printMsg         (BOLD_RED, "ERROR",   str)         // Error Msg

// Debug Messages
#define dbug_dPrint(...)  dPrintMsg        (BOLD_PURPLE,   "DEBUG",   __VA_ARGS__) // Debug Msg
#define dbug_print(str)   printMsgNL       (BOLD_PURPLE,   "DEBUG",   str)         // Debug Msg
#define dbug_msg(str)     printMsg         (BOLD_PURPLE,   "DEBUG",   str)         // Debug Msg

// Static String Printing
#if defined(_avr_at_)
#define print(s) _print(PSTR(s))
#else
#define print(s) _print(s)
#endif

void _print( const char *s );
void printstrs( char* first, ... );
void printChar( char c );


// Printing numbers
#define printHex(hex)   printHex_op(hex, 1)
#define printHex32(hex) printHex32_op(hex, 1)

void printInt8    ( uint8_t  in );
void printInt16   ( uint16_t in );
void printInt32   ( uint32_t in );
void printHex_op  ( uint16_t in, uint8_t op );
void printHex32_op( uint32_t in, uint8_t op );


// String Functions
#define hexToStr(hex, out) hexToStr_op(hex, out, 1)

void int8ToStr    ( uint8_t  in, char*  out );
void int16ToStr   ( uint16_t in, char*  out );
void int32ToStr   ( uint32_t in, char*  out );
void hexToStr_op  ( uint16_t in, char*  out, uint8_t op );
void hex32ToStr_op( uint32_t in, char*  out, uint8_t op );
void revsStr      ( char*  in );
uint16_t lenStr   ( char*  in );
int16_t eqStr     ( char*  str1, char* str2 ); // Returns -1 if identical, last character of str1 comparison (0 if str1 is like str2)
int numToInt      ( char* in ); // Returns the int representation of a string

void hex32ToStr16 ( uint32_t in, uint16_t* out, uint8_t op ); // Used for USB Descriptors

