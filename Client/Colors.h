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
#pragma	once
namespace MudmasterColors
{
	const int MAX_FORE_COLORS		= 16;
	const int MAX_BACK_COLORS		= 8;

	// Mud Master Color indices. Note
	// these do not correspond with their
	// ansi counterparts.
	const BYTE COLOR_BLACK			= 0;
	const BYTE COLOR_BLUE			= 1;
	const BYTE COLOR_GREEN			= 2;
	const BYTE COLOR_CYAN			= 3;
	const BYTE COLOR_RED			= 4;
	const BYTE COLOR_MAGENTA		= 5;
	const BYTE COLOR_BROWN			= 6;
	const BYTE COLOR_GRAY			= 7;
	const BYTE COLOR_DARKGRAY		= 8;
	const BYTE COLOR_LIGHTBLUE		= 9;
	const BYTE COLOR_LIGHTGREEN		= 10;
	const BYTE COLOR_LIGHTCYAN		= 11;
	const BYTE COLOR_LIGHTRED		= 12;
	const BYTE COLOR_LIGHTMAGENTA	= 13;
	const BYTE COLOR_YELLOW			= 14;
	const BYTE COLOR_WHITE			= 15;

	const COLORREF WINDOWS_BLACK	= RGB(0,0,0);
	const COLORREF WINDOWS_BLUE		= RGB(0,0,128);
	const COLORREF WINDOWS_GREEN	= RGB(0,128,0);
	const COLORREF WINDOWS_CYAN		= RGB(0,128,128);
	const COLORREF WINDOWS_RED		= RGB(128,0,0);
	const COLORREF WINDOWS_MAGENTA	= RGB(128,0,128);
	const COLORREF WINDOWS_BROWN	= RGB(128,128,0);
	const COLORREF WINDOWS_GRAY		= RGB(192,192,192);
	const COLORREF WINDOWS_DARKGRAY	= RGB(64,64,64);
	const COLORREF WINDOWS_LIGHTBLUE	= RGB(0,0,255);
	const COLORREF WINDOWS_LIGHTGREEN	= RGB(0,255,0);
	const COLORREF WINDOWS_LIGHTCYAN	= RGB(0,255,255);
	const COLORREF WINDOWS_LIGHTRED		= RGB(255,0,0);
	const COLORREF WINDOWS_LIGHTMAGENTA	= RGB(255,0,255);
	const COLORREF WINDOWS_YELLOW		= RGB(255,255,0);
	const COLORREF WINDOWS_WHITE		= RGB(255,255,255);

#define ANSI_RESET				"\x1b[0m"
#define ANSI_BOLD				"\x1b[1m"
#define ANSI_REVERSE			"\x1b[7m"
#define ANSI_F_BLACK			"\x1b[30m"
#define ANSI_F_RED				"\x1b[31m"
#define ANSI_F_GREEN			"\x1b[32m"
#define ANSI_F_YELLOW			"\x1b[33m"
#define ANSI_F_BLUE				"\x1b[34m"
#define ANSI_F_MAGENTA			"\x1b[35m"
#define ANSI_F_CYAN				"\x1b[36m"
#define ANSI_F_WHITE			"\x1b[37m"
#define ANSI_F_BOLDGRAY			"\x1b[1;30m"
#define ANSI_F_BOLDRED			"\x1b[1;31m"
#define ANSI_F_BOLDGREEN		"\x1b[1;32m"
#define ANSI_F_BOLDYELLOW		"\x1b[1;33m"
#define ANSI_F_BOLDBLUE			"\x1b[1;34m"
#define ANSI_F_BOLDMAGENTA		"\x1b[1;35m"
#define ANSI_F_BOLDCYAN			"\x1b[1;36m"
#define ANSI_F_BOLDWHITE		"\x1b[1;37m"
#define ANSI_B_BLACK			"\x1b[40m"
#define ANSI_B_RED				"\x1b[41m"
#define ANSI_B_GREEN			"\x1b[42m"
#define ANSI_B_YELLOW			"\x1b[43m"
#define ANSI_B_BLUE				"\x1b[44m"
#define ANSI_B_MAGENTA			"\x1b[45m"
#define ANSI_B_CYAN				"\x1b[46m"
#define ANSI_B_WHITE			"\x1b[47m"

	extern const COLORREF DEFAULT_COLORS[];
	extern const char *ANSI_FORE_COLORS[];
	extern const char *ANSI_BACK_COLORS[];
	extern const char *COLOR_STRINGS[];
	extern const char *HTML_FORE_COLORS[];
	extern const char *HTML_BACK_COLORS[];

	const int MM_COLOR_RESET	= 0;
	const int MM_COLOR_INFO		= 1;
	const int MM_COLOR_COMMAND	= 2;

	extern const char *MESSAGE_COLORS[];

	static const unsigned char TW_BLACK		= 0;
	static const unsigned char TW_RED		= 1;
	static const unsigned char TW_GREEN		= 2;
	static const unsigned char TW_BROWN		= 3;
	static const unsigned char TW_BLUE		= 4;
	static const unsigned char TW_MAGENTA	= 5;
	static const unsigned char TW_CYAN		= 6;
	static const unsigned char TW_LIGHTGRAY	= 7;

	static const unsigned char TW_DARKGRAY		= 8;
	static const unsigned char TW_LIGHTRED		= 9;
	static const unsigned char TW_LIGHTGREEN	= 10;
	static const unsigned char TW_YELLOW		= 11;
	static const unsigned char TW_LIGHTBLUE		= 12;
	static const unsigned char TW_LIGHTMAGENTA	= 13;
	static const unsigned char TW_LIGHTCYAN		= 14;
	static const unsigned char TW_WHITE			= 15;
}

