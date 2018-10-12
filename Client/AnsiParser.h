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
// AnsiParser.h: interface for the CAnsiParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANSIPARSER_H__71A294E1_17CA_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_ANSIPARSER_H__71A294E1_17CA_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorLine.h"
#include "MudSocket.h"
#include "Sess.h"
#include "TermWindow.h"


class CAnsiParser  
{
public:
	void ResetParser();
	enum { ANSI_ARG_LEN=100 };
	void ParseAnsi(const char *strLine, CColorLine::SpColorLine &line);
	void ParseAnsi(const char *strLine, CColorLine::SpColorLine &line, CMudSocket* pMudSocket, CSession*);
	CAnsiParser();
    	virtual ~CAnsiParser();

private:
	enum ParseState
	{
		PARSE_START = 0,
		PARSE_ANSI,
		PARSE_CODE,
		PARSE_IAC,
		PARSE_WILL,
		PARSE_WONT,
		PARSE_DO,
		PARSE_DONT,
		PARSE_SB,
		PARSE_SB_AUTH,
		PARSE_SE,
		PARSE_SB_TERM
	};

	ParseState m_nParseState;
	void SetGraphics();
	CCharFormat m_cfCurrentFormat;
	int m_nArgLen;
	char m_AnsiArg[ANSI_ARG_LEN];
	CCharFormat MakeBold(  CCharFormat _cfCurrentFormat);
	BYTE telnetCommand[100];
	BYTE telnetAuthkeyResponse[100];

};

#endif // !defined(AFX_ANSIPARSER_H__71A294E1_17CA_11D4_BCD6_00E029482496__INCLUDED_)
