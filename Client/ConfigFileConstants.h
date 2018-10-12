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

static LPCTSTR INI_FILE_NAME = _T("MudMaster2k6.ini");
static LPCTSTR DEFAULT_SECTION_NAME = _T("Defaults");
static LPCTSTR DEFAULT_SESSION_KEY = _T("Default Session");
static LPCTSTR DEFAULT_IP_ADDRESS = _T("Default IP");
static LPCTSTR INI_WITH_EXE = _T("Ini With Exe");

#define PUT_ENTRY_INT(section, key, val) CGlobals::WriteIniInt((section), (key), (val), lpszPathName)
#define PUT_ENTRY_STR(section, key, val) ::WritePrivateProfileString((section), (key), (val), lpszPathName)
#define GET_ENTRY_INT(section, key, def) ::GetPrivateProfileInt((section), (key), (def), lpszPathName)
#define GET_ENTRY_STR(section, key, def) CGlobals::GetIniString((section), (key), (def), lpszPathName)

#define TERM_OPTIONS_SECTION _T("Terminal Options")
#define CHARSET_KEY			_T("CharSet")
#define CLIPPRES_KEY		_T("Clip Precision")
#define ESCAPE_KEY			_T("Escapement")
#define HEIGHT_KEY			_T("Height")
#define ITALIC_KEY			_T("Italic")
#define ORIENTATION_KEY		_T("Orientation")
#define OUTPREC_KEY			_T("Out Precision")
#define PITCHFAM_KEY		_T("Pitch and Fam")
#define QUALITY_KEY			_T("Quality")
#define STRIKEOUT_KEY		_T("Strikeout")
#define UNDERLINE_KEY		_T("Underline")
#define WEIGHT_KEY			_T("Weight")
#define WIDTH_KEY			_T("Width")
#define FACE_KEY			_T("Font Face")
#define SB_WIDTH_KEY		_T("Characters Wide")
#define LINES_KEY			_T("Lines High")
#define LINE_LENGTH_KEY		_T("Max Line Length")

#define TELNET_GA_KEY		_T("Telnet GA")
#define UNZAP_KEY			_T("Unzap")
#define LOCAL_ECHO_KEY		_T("Local Echo")
#define BUFFER_SIZE_KEY		_T("Buffer Size")
#define DEFAULT_FORE_KEY	_T("Default Fore Color")
#define DEFAULT_BACK_KEY	_T("Default Back Color")
#define HIGH_FORE_KEY		_T("Highlight Fore Color")
#define HIGH_BACK_KEY		_T("Highlight Back Color")
#define FULL_SCREEN_SB		_T("Full Screen Scrollback")

#define FORE_ONE_KEY		_T("Fore Color One")
#define FORE_TWO_KEY		_T("Fore Color Two")
#define FORE_THREE_KEY		_T("Fore Color Three")
#define FORE_FOUR_KEY		_T("Fore Color Four")
#define FORE_FIVE_KEY		_T("Fore Color Five")
#define FORE_SIX_KEY		_T("Fore Color Six")
#define FORE_SEVEN_KEY		_T("Fore Color Seven")
#define FORE_EIGHT_KEY		_T("Fore Color Eight")
#define FORE_NINE_KEY		_T("Fore Color Nine")
#define FORE_TEN_KEY		_T("Fore Color Ten")
#define FORE_ELEVEN_KEY		_T("Fore Color Eleven")
#define FORE_TWELVE_KEY		_T("Fore Color Twelve")
#define FORE_THIRTEEN_KEY	_T("Fore Color Thirteen")
#define FORE_FOURTEEN_KEY	_T("Fore Color Fourteen")
#define FORE_FIFTEEN_KEY	_T("Fore Color Fifteen")
#define FORE_SIXTEEN_KEY	_T("Fore Color Sixteen")
#define BACK_ONE_KEY		_T("Back Color One")
#define BACK_TWO_KEY		_T("Back Color Two")
#define BACK_THREE_KEY		_T("Back Color Three")
#define BACK_FOUR_KEY		_T("Back Color Four")
#define BACK_FIVE_KEY		_T("Back Color Five")
#define BACK_SIX_KEY		_T("Back Color Six")
#define BACK_SEVEN_KEY		_T("Back Color Seven")
#define BACK_EIGHT_KEY		_T("Back Color Eight")

#define SB_OPTIONS_SECTION	"Status Bar"
#define BACK_COLOR_KEY		"Back Color"
#define FORE_COLOR_KEY		"Fore Color"
