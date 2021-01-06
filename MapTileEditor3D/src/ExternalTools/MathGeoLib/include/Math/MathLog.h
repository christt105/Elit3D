/* Copyright Jukka Jylänki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Log.h
	@author Jukka Jylänki
	@brief The MLOG and LOGUSER macros. Provides an unified mechanism for logging. */
#pragma once

#include "MathNamespace.h"
#include "MultiLineMacro.h"

MATH_BEGIN_NAMESPACE

/// A bitfield type that describes single or multiple log channels (each bit represents a channel).
typedef unsigned int MathLogChannel;

namespace
{
const MathLogChannel MathLogInfo = 1;
const MathLogChannel MathLogError = 2;
const MathLogChannel MathLogWarning = 4;
const MathLogChannel MathLogErrorNoCallstack = MathLogError|65536;
const MathLogChannel MathLogWarningNoCallstack = MathLogWarning|65536;
}

void PrintToConsoleVariadic(MathLogChannel channel, const char *format, ...);
void PrintToConsole(MathLogChannel channel, const char *str);

#define STRINGIZE_HELPER(x) #x
#define STRINGIZE(x) STRINGIZE_HELPER(x)
#define WARNING(desc) message(__FILE__ "(" STRINGIZE(__LINE__) ") : warning: " #desc)

#ifndef LOGGING_SUPPORT_DISABLED

#define MLOGI(...) PrintToConsoleVariadic(MathLogInfo, __VA_ARGS__)
#define MLOGW(...) PrintToConsoleVariadic(MathLogWarning, __VA_ARGS__)
#define MLOGW_NS(...) PrintToConsoleVariadic(MathLogWarningNoCallstack, __VA_ARGS__)
#define MLOGE(...) PrintToConsoleVariadic(MathLogError, __VA_ARGS__)
#define MLOGE_NS(...) PrintToConsoleVariadic(MathLogErrorNoCallstack, __VA_ARGS__)
#define MLOG(channel, ...) PrintToConsoleVariadic(channel, __VA_ARGS__)

#else

#define MLOG(...) ((void)0)
#define MLOGW(...) ((void)0)
#define MLOGW_NS(...) ((void)0)
#define MLOGE(...) ((void)0)
#define MLOGE_NS(...) ((void)0)
#define MLOGI(...) ((void)0)

#endif

MATH_END_NAMESPACE
