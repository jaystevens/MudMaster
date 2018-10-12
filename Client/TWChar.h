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
// TWChar.h: interface for the CTWChar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TWCHAR_H__B6819842_7F1A_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_TWCHAR_H__B6819842_7F1A_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Colors.h"

// Color indices.  All of the are valid for the foreground color.
// Only TW_BLACK through TW_LIGHTGRAY are valid for background.
class CTWChar  
{
public:
	CTWChar();
	CTWChar(const CTWChar& src);
	virtual ~CTWChar();

	CTWChar& operator=(const CTWChar& src);


	//GENERAL FUNCTIONS
	static BYTE MakeAttr(int nFore, int nBack);
	void SetGraphicsRendition(LPSTR  argbuf, int arglen);

	//ACCESSORS
	char Character()			{return m_ch;}
	BYTE Attribute();
	BOOL Bold()					{return m_bBold;}
	BOOL Italic()				{return m_bItalic;}
	BYTE Fore()					{return m_nFore;}
	BYTE Back()					{return m_nBack;}

	//MUTATORS
	void Character(char ch)		{m_ch = ch;}
	void Attribute(BYTE attr, BOOL bBold);

	void Reset();
	void Bold(BOOL bBold)
	{ 
		if(m_bBold) 
			m_nFore = MudmasterColors::TW_WHITE;
		m_bBold = bBold;
	}
	void Italic(BOOL bItalic)	{m_bItalic = bItalic;}

	void ForeBlack();
	void ForeRed();
	void ForeGreen();
	void ForeYellow();
	void ForeBlue();
	void ForeMagenta();
	void ForeCyan();
	void ForeWhite();
	void BackBlack();
	void BackRed();
	void BackGreen();
	void BackYellow();
	void BackBlue();
	void BackMagenta();
	void BackCyan();
	void BackWhite();
	void SubScript(){}
	void SuperScript(){}

private:
	BYTE m_ch;		// Printed character.
	BYTE m_attr;	// Attribute.  This is the combined foreground and background index.

	BYTE m_nFore;
	BYTE m_nBack;
	BOOL m_bItalic;
	BOOL m_bBold;
};

#endif // !defined(AFX_TWCHAR_H__B6819842_7F1A_11D3_BCD6_00E029482496__INCLUDED_)
