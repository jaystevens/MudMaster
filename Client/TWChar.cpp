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
#include "stdafx.h"
#include "TWChar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;

CTWChar::CTWChar()
{
	m_ch = 0; 
	m_attr = 0;
	m_nFore = TW_BLACK;
	m_nBack = TW_BLACK;
	m_bItalic = FALSE; 
	m_bBold = FALSE;
}

CTWChar::CTWChar(const CTWChar& src):
m_ch(src.m_ch),
m_attr(src.m_attr),
m_nFore(src.m_nFore),
m_nBack(src.m_nBack),
m_bBold(src.m_bBold),
m_bItalic(src.m_bItalic)
{
}

CTWChar& CTWChar::operator=(const CTWChar& src)
{
	m_ch = src.m_ch;
	m_attr = src.m_attr;
	m_nFore = src.m_nFore;
	m_nBack = src.m_nBack;
	m_bBold = src.m_bBold;
	m_bItalic = src.m_bItalic;
	return *this;
}

CTWChar::~CTWChar()
{

}

void CTWChar::Reset()
{
	m_nFore = 7;
	m_nBack = 0;
	m_bBold = FALSE;
	m_bItalic = FALSE;
}

void CTWChar::Attribute(BYTE nAttribute, BOOL bBold)
{
	m_nFore = (BYTE)(nAttribute & 0x0F);
	m_nBack = (BYTE)((nAttribute & 0xF0) >> 4);
	m_bBold = bBold;
}

BYTE CTWChar::Attribute()
{
	return MakeAttr(m_nFore, m_nBack);
}

BYTE CTWChar::MakeAttr(int nFore, int nBack)
{
	return(BYTE)(((nBack << 4) | nFore));
}

void CTWChar::ForeBlack()
{
	m_nFore = Bold() ? TW_LIGHTGRAY : TW_BLACK;
}

void CTWChar::ForeRed()
{
	m_nFore = Bold() ? TW_LIGHTRED : TW_RED;
}

void CTWChar::ForeGreen()
{
	m_nFore = Bold() ? TW_LIGHTGREEN : TW_GREEN;
}

void CTWChar::ForeYellow()
{
	m_nFore = Bold() ? TW_YELLOW : TW_BROWN;
}

void CTWChar::ForeBlue()
{
	m_nFore = Bold() ? TW_LIGHTBLUE : TW_BLUE;
}

void CTWChar::ForeMagenta()
{
	m_nFore = Bold() ? TW_LIGHTMAGENTA : TW_MAGENTA;
}

void CTWChar::ForeCyan()
{
	m_nFore = Bold() ? TW_LIGHTCYAN : TW_CYAN;
}

void CTWChar::ForeWhite()
{
	m_nFore = Bold() ? TW_WHITE : TW_LIGHTGRAY;
}

void CTWChar::BackBlack()
{
	m_nBack = TW_BLACK;
}

void CTWChar::BackRed()
{
	m_nBack = TW_RED;
}

void CTWChar::BackGreen()
{
	m_nBack = TW_GREEN;
}

void CTWChar::BackYellow()
{
	m_nBack = TW_YELLOW;
}

void CTWChar::BackBlue()
{
	m_nBack = TW_BLUE;
}

void CTWChar::BackMagenta()
{
	m_nBack = TW_MAGENTA;
}

void CTWChar::BackCyan()
{
	m_nBack = TW_CYAN;
}

void CTWChar::BackWhite()
{
	m_nBack = TW_WHITE;
}

void CTWChar::SetGraphicsRendition(LPSTR  argbuf, int arglen)
{
	LPSTR p,pp;

	if (*argbuf && arglen)
	{
		pp = argbuf;
		do
		{
			p = strchr(pp,';');
			if (p && *p)
			{
				*p = 0;
				p++;
			}

			switch (atoi(pp))
			{
			case 0: /* all attributes off */
				Reset();
				break;

			case 1: /* bright on */
				Bold(TRUE);
				break;

			case 2: /* faint on */
				Bold(FALSE);
				break;

			case 3: /* italic on */
				Italic(TRUE);
				break;

			case 5: /* blink on */
				break;

			case 6: /* rapid blink on */
				break;

			case 7: /* reverse video on */
				break;

			case 8: /* concealed on */
				break;

			case 30: /* black fg */
				ForeBlack();
				break;

			case 31: /* red fg */
				ForeRed();
				break;

			case 32: /* green fg */
				ForeGreen();
				break;

			case 33: /* yellow fg */
				ForeYellow();
				break;

			case 34: /* blue fg */
				ForeBlue();
				break;

			case 35: /* magenta fg */
				ForeMagenta();
				break;

			case 36: /* cyan fg */
				ForeCyan();
				break;

			case 37: /* white fg */
				ForeWhite();
				break;

			case 40: /* black bg */
				BackBlack();
				break;

			case 41: /* red bg */
				BackRed();
				break;

			case 42: /* green bg */
				BackGreen();
				break;

			case 43: /* yellow bg */
				BackYellow();
				break;

			case 44: /* blue bg */
				BackBlue();
				break;

			case 45: /* magenta bg */
				BackMagenta();
				break;

			case 46: /* cyan bg */
				BackCyan();
				break;

			case 47: /* white bg */
				BackWhite();
				break;

			case 48: /* subscript bg */
				SubScript();
				break;

			case 49: /* superscript bg */
				SuperScript();
				break;

			default: /* unsupported */
				break;
			}
			pp = p;
		} while (p);
	}
}
