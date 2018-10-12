/**************************************************************************
*  Copyright (c) 1997-2004, Kevin Cook and Aaron O'Neil
*  All rights reserved.

*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions are 
*  met:
*  
*      * Redistributions of source code must retain the above copyright 
*        notice, this list of conditions and the following disclaimer. 
*      * Redistributions in binary form must reproduce the above copyright 
*        notice, this list of conditions and the following disclaimer in 
*        the documentation and/or other materials provided with the 
*        distribution. 
*      * Neither the name of the MMGUI Project nor the names of its 
*        contributors may be used to endorse or promote products derived 
*        from this software without specific prior written permission. 
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
*  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
*  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#include "StdAfx.h"
#include "Colors.h"

namespace MudmasterColors
{
	const COLORREF DEFAULT_COLORS[] =
	{
		WINDOWS_BLACK,
			WINDOWS_BLUE,
			WINDOWS_GREEN,
			WINDOWS_CYAN,
			WINDOWS_RED,
			WINDOWS_MAGENTA,
			WINDOWS_BROWN,
			WINDOWS_GRAY,
			WINDOWS_DARKGRAY,
			WINDOWS_LIGHTBLUE,
			WINDOWS_LIGHTGREEN,
			WINDOWS_LIGHTCYAN,
			WINDOWS_LIGHTRED,
			WINDOWS_LIGHTMAGENTA,
			WINDOWS_YELLOW,
			WINDOWS_WHITE
	};

	const char *ANSI_FORE_COLORS[] = 
	{
		ANSI_F_BLACK,
			ANSI_F_BLUE,
			ANSI_F_GREEN,
			ANSI_F_CYAN,
			ANSI_F_RED,
			ANSI_F_MAGENTA,
			ANSI_F_YELLOW,
			ANSI_F_WHITE,
			ANSI_F_BOLDGRAY,
			ANSI_F_BOLDBLUE,
			ANSI_F_BOLDGREEN,
			ANSI_F_BOLDCYAN,
			ANSI_F_BOLDRED,
			ANSI_F_BOLDMAGENTA,
			ANSI_F_BOLDYELLOW,
			ANSI_F_BOLDWHITE
	};

	const char *ANSI_BACK_COLORS[] =
	{
		ANSI_B_BLACK,
			ANSI_B_BLUE,
			ANSI_B_GREEN,
			ANSI_B_CYAN,
			ANSI_B_RED,
			ANSI_B_MAGENTA,
			ANSI_B_YELLOW,
			ANSI_B_WHITE
	};

	const char *HTML_BACK_COLORS[] =
	{
			"BBK",
			"BB",
			"BG",
			"BC",
			"BR",
			"BM",
			"BY",
			"BW"
	};

	const char *HTML_FORE_COLORS[] = 
	{
			"FBK",
			"FB",
			"FG",
			"FC",
			"FR",
			"FM",
			"FY",
			"FW",
			"FGY",
			"FBB",
			"FBG",
			"FBC",
			"FBR",
			"FBM",
			"FBY",
			"FBW"
	};


	const char *MESSAGE_COLORS[] =
	{
		ANSI_RESET,
			ANSI_F_BOLDWHITE,
			ANSI_F_WHITE
	};

	const char *COLOR_STRINGS[] =
	{
		"black",
			"blue",
			"green",
			"cyan",
			"red",
			"magenta",
			"brown",
			"light grey",
			"dark greay",
			"light blue",
			"light green",
			"light cyan",
			"light red",
			"light magenta",
			"yellow",
			"white"
	};
}