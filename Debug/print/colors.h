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

#pragma once

// Convenience macros to wrap a string in a color
#define COLOR_FG(color, msg) VT_##color msg VT_NORMAL
#define COLOR_BG(color, msg) VT_BG_##color msg VT_NORMAL

// Truecolor (terminal support varies)
#define TRUECOLOR_FG "\033[38;2"
#define TRUECOLOR_BG "\033[48;2"


/* These color codes are for VT100 compatible terminals */

// Modes
#define VT_NORMAL    "\033[0m"
#define VT_BOLD      "\033[1m"
#define VT_UNDERLINE "\033[4m"
#define VT_BLINK     "\033[5m"
#define VT_REVERSE   "\033[1m"

// Colors
#define VT_BLACK     "\033[30m"
#define VT_RED       "\033[31m"
#define VT_GREEN     "\033[32m"
#define VT_YELLOW    "\033[33m"
#define VT_BLUE      "\033[34m"
#define VT_PURPLE    "\033[35m"
#define VT_CYAN      "\033[36m"
#define VT_WHITE     "\033[37m"
#define VT_BG_BLACK  "\033[40m"
#define VT_BG_RED    "\033[41m"
#define VT_BG_GREEN  "\033[42m"
#define VT_BG_YELLOW "\033[43m"
#define VT_BG_BLUE   "\033[44m"
#define VT_BG_PURPLE "\033[45m"
#define VT_BG_CYAN   "\033[46m"
#define VT_BG_WHITE  "\033[47m"

#define VT_BOLD_BLACK  "\033[1;30m"
#define VT_BOLD_RED    "\033[1;31m"
#define VT_BOLD_GREEN  "\033[1;32m"
#define VT_BOLD_YELLOW "\033[1;33m"
#define VT_BOLD_BLUE   "\033[1;34m"
#define VT_BOLD_PURPLE "\033[1;35m"
#define VT_BOLD_CYAN   "\033[1;36m"
#define VT_BOLD_WHITE  "\033[1;37m"
