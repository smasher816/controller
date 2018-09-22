/* Copyright (C) 2018 by Jacob Alexander
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

#if defined(SYSTEMVIEW_ENABLED) && !defined(_bootloader_)
#include "SEGGER_SYSVIEW.h"

//#define SEGGER_SYSVIEW_RecordVoid(x) SEGGER_SYSVIEW_Print(" >>> " __FILENAME__ ":" _STR(__LINE__) "()")
//#define SEGGER_SYSVIEW_RecordEndCall(x) SEGGER_SYSVIEW_Print(" <<< " __FILENAME__ ":" _STR(__LINE__))

/*
#define SEGGER_SYSVIEW_RecordVoid(x) SEGGER_SYSVIEW_Print(__FUNCTION__)
#define SEGGER_SYSVIEW_RecordU32(x, y) SEGGER_SYSVIEW_RecordVoid
#define SEGGER_SYSVIEW_RecordU32x2(x, y, z) SEGGER_SYSVIEW_RecordVoid
#define SEGGER_SYSVIEW_RecordU32x3(x, y, z, a) SEGGER_SYSVIEW_RecordVoid
#define SEGGER_SYSVIEW_RecordU32x4(x, y, z, a, b) SEGGER_SYSVIEW_RecordVoid
#define SEGGER_SYSVIEW_RecordU32x5(x, y, z, a, b, c) SEGGER_SYSVIEW_RecordVoid
#define SEGGER_SYSVIEW_RecordString(x, y) SEGGER_SYSVIEW_Print(y)
#define SEGGER_SYSVIEW_RecordEndCall(x)
#define SEGGER_SYSVIEW_RecordEndCallU32(x, y) SEGGER_SYSVIEW_RecordEndCall
*/

/*
#define SEGGER_SYSVIEW_RecordVoid(x) SEGGER_RTT_printf(0, " >>> %s:%s %s()\n", __FILE__, __LINE__, __func__);
#define SEGGER_SYSVIEW_RecordU32(x, y) SEGGER_RTT_printf(0, " >>> %s:%s %s(%u)\n", __FILE__, __LINE__, __func__, y);
#define SEGGER_SYSVIEW_RecordU32x2(x, y, z) SEGGER_RTT_printf(0, " >>> %s:%s %s(%u, %u)\n", __FILE__, __LINE__, __func__, y, z);
#define SEGGER_SYSVIEW_RecordU32x3(x, y, z, a) SEGGER_RTT_printf(0, " >>> %s:%s %s(%u, %u, %u)\n", __FILE__, __LINE__, __func__, y, z, a);
#define SEGGER_SYSVIEW_RecordU32x4(x, y, z, a, b) SEGGER_RTT_printf(0, " >>> %s:%s %s(%u, %u, %u, %u)\n", __FILE__, __LINE__, __func__, y, z, a, b);
#define SEGGER_SYSVIEW_RecordU32x5(x, y, z, a, b, c) SEGGER_RTT_printf(0, " >>> %s:%s %s(%u, %u, %u, %u, %u)\n", __FILE__, __LINE__, __func__, y, z, a, b, c);
#define SEGGER_SYSVIEW_RecordString(x, y) SEGGER_RTT_printf(0, " >>> %s:%s %s\n", __FILE__, __LINE__, y);
#define SEGGER_SYSVIEW_RecordEndCall(x)
#define SEGGER_SYSVIEW_RecordEndCallU32(x, y)
*/

#define SEGGER_SYSVIEW_Print(s) SEGGER_RTT_printf(0, "%s\n", s)
#define SEGGER_SYSVIEW_PrintfHost(format, ...) SEGGER_RTT_printf(0, format, __VA_ARGS__)

#define SEGGER_SYSVIEW_RecordVoid(x) SEGGER_RTT_printf(0, " >>> %s()\n", __func__);
#define SEGGER_SYSVIEW_RecordU32(x, y) SEGGER_RTT_printf(0, " >>> %s(%u)\n", __func__, y);
#define SEGGER_SYSVIEW_RecordU32x2(x, y, z) SEGGER_RTT_printf(0, " >>> %s(%u, %u)\n", __func__, y, z);
#define SEGGER_SYSVIEW_RecordU32x3(x, y, z, a) SEGGER_RTT_printf(0, " >>> %s(%u, %u, %u)\n", __func__, y, z, a);
#define SEGGER_SYSVIEW_RecordU32x4(x, y, z, a, b) SEGGER_RTT_printf(0, " >>> %s(%u, %u, %u, %u)\n", __func__, y, z, a, b);
#define SEGGER_SYSVIEW_RecordU32x5(x, y, z, a, b, c) SEGGER_RTT_printf(0, " >>> %s(%u, %u, %u, %u, %u)\n", __func__, y, z, a, b, c);
#define SEGGER_SYSVIEW_RecordString(x, y) SEGGER_RTT_printf(0, "%s\n", __FILE__, __LINE__, y);
#define SEGGER_SYSVIEW_RecordEndCall(x)
#define SEGGER_SYSVIEW_RecordEndCallU32(x, y)


#else
#define SEGGER_SYSVIEW_MODULE void*

#define SEGGER_SYSVIEW_RecordVoid(x)
#define SEGGER_SYSVIEW_RecordU32(x, y)
#define SEGGER_SYSVIEW_RecordU32x2(x, y, z)
#define SEGGER_SYSVIEW_RecordU32x3(x, y, z, a)
#define SEGGER_SYSVIEW_RecordU32x4(x, y, z, a, b)
#define SEGGER_SYSVIEW_RecordU32x5(x, y, z, a, b, c)
#define SEGGER_SYSVIEW_RecordString(x, y)
#define SEGGER_SYSVIEW_RecordSystime()
#define SEGGER_SYSVIEW_RecordEnterISR()
#define SEGGER_SYSVIEW_RecordExitISR()
#define SEGGER_SYSVIEW_RecordExitISRToScheduler()
#define SEGGER_SYSVIEW_RecordEnterTimer(x)
#define SEGGER_SYSVIEW_RecordExitTimer()
#define SEGGER_SYSVIEW_RecordEndCall(x)
#define SEGGER_SYSVIEW_RecordEndCallU32(x, y)

#define SEGGER_SYSVIEW_Print(x)
#define SEGGER_SYSVIEW_PrintfHost(x, ...)
#define SEGGER_SYSVIEW_RegisterModule(x)
#define SEGGER_SYSVIEW_Start()
#define SEGGER_SYSVIEW_RecordModuleDescription(x, y)

#define SEGGER_SYSVIEW_OnIdle()
#define SEGGER_SYSVIEW_OnTaskCreate(x)
#define SEGGER_SYSVIEW_OnTaskTerminate(x)
#define SEGGER_SYSVIEW_OnTaskStartExec(x)
#define SEGGER_SYSVIEW_OnTaskStopExec()
#define SEGGER_SYSVIEW_OnTaskStartReady(x)
#define SEGGER_SYSVIEW_OnTaskStopReady(x, y)
#define SEGGER_SYSVIEW_OnUserStart(x)
#define SEGGER_SYSVIEW_OnUserStop(x)

#endif
