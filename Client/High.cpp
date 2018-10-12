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
#include "Ifx.h"
#include "High.h"
#include "Globals.h"
#include "Colors.h"
#include "Sess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;

namespace MMScript
{
	CHigh::CHigh()
	{
		m_fore = 0;
		m_back = 0;
	}

	CHigh::CHigh(const CHigh& src):
	m_bAnchored(src.m_bAnchored),
		m_nTokens(src.m_nTokens),
		m_nVars(src.m_nVars),
		m_bStartVar(src.m_bStartVar),
		m_strGroup(src.m_strGroup),
		m_bEnabled(src.m_bEnabled),
		m_fore(src.m_fore),
		m_back(src.m_back)
	{
		for(int i = 0; i < 10; i++)
		{
			m_strToken[i] = src.m_strToken[i];
		}
	}

	CHigh& CHigh::operator =(const CHigh& src)
	{
		if(this == &src)
			return *this;

		m_bAnchored = src.m_bAnchored;
		m_nTokens = src.m_nTokens;
		m_nVars = src.m_nVars;
		m_bStartVar = src.m_bStartVar;
		m_strGroup = src.m_strGroup;
		m_bEnabled = src.m_bEnabled;
		m_fore = src.m_fore;
		m_back = src.m_back;

		for(int i = 0; i < 10; i++)
		{
			m_strToken[i] = src.m_strToken[i];
		}

		return *this;
	}

	CHigh::~CHigh()
	{

	}

	void CHigh::ToMask(CString &strMask)
	{
		strMask.Empty();
		if (Anchored())
			strMask += "^";

		int nVarIndex = 0;
		CString strTemp;
		if (StartVar())
		{
			strTemp.Format("%%%c",(char)(nVarIndex+'0'));
			strMask += strTemp;
			nVarIndex++;
		}

		for (int i=0;i<Tokens();i++)
		{
			strMask += Token(i);
			if (nVarIndex < Vars())
			{
				strTemp.Format("%%%c",(char)(nVarIndex+'0'));
				strMask += strTemp;
				nVarIndex++;
			}
		}
	}

	void CHigh::MapToCommand(CString &strCommand)
	{
		CString strText;
		MapToText(strText, TRUE);
		strCommand.Format("%chighlight %s", CGlobals::GetCommandCharacter(), strText);
	}

	void CHigh::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		CString strMask, strColor;
		ToMask(strMask);
		ColorToString(strColor);
		if(bIncludeGroup)
		{
			strText.Format("{%s} {%s} {%s}", strMask, strColor, Group());
		}
		else
		{
			strText.Format("{%s} {%s}", strMask, strColor);
		}
	}

	BYTE CHigh::fore()
	{
		return m_fore;
	}

	BYTE CHigh::back()
	{
		return m_back;
	}

	void CHigh::fore(BYTE nFore)
	{
		m_fore = nFore;
	}

	void CHigh::back(BYTE nBack)
	{
		m_back = nBack;
	}

	void CHigh::ColorToString(CString &strResult)
	{
		strResult.Empty();

		if(m_fore == COLOR_BLACK)
		{ 
			strResult = "black"; 
		}
		else if(m_fore == COLOR_BLUE)
		{
			strResult = "blue"; 
		}
		else if(m_fore == COLOR_GREEN)
		{
			strResult = "green"; 
		}
		else if(m_fore == COLOR_CYAN)
		{
			strResult = "cyan"; 
		}
		else if(m_fore == COLOR_RED)
		{
			strResult = "red"; 
		}
		else if(m_fore == COLOR_MAGENTA)
		{
			strResult = "magenta"; 
		}
		else if(m_fore == COLOR_BROWN)
		{
			strResult = "brown"; 
		}
		else if(m_fore == COLOR_GRAY)
		{
			strResult = "light grey"; 
		}
		else if(m_fore == COLOR_DARKGRAY)
		{
			strResult = "dark grey"; 
		}
		else if(m_fore == COLOR_LIGHTBLUE)
		{
			strResult = "light blue"; 
		}
		else if(m_fore == COLOR_LIGHTGREEN)
		{
			strResult = "light green"; 
		}
		else if(m_fore == COLOR_LIGHTCYAN)
		{
			strResult = "light cyan"; 
		}
		else if(m_fore == COLOR_LIGHTRED)
		{
			strResult = "light red"; 
		}
		else if(m_fore == COLOR_LIGHTMAGENTA)
		{
			strResult = "light magenta"; 
		}
		else if(m_fore == COLOR_YELLOW)
		{
			strResult = "yellow"; 
		}
		else if(m_fore == COLOR_WHITE)
		{
			strResult = "white"; 
		}

		strResult += ",";

		if(m_back == COLOR_BLACK)
		{
			strResult += "back black"; 
		}
		else if(m_back == COLOR_BLUE)
		{
			strResult += "back blue"; 
		}
		else if(m_back == COLOR_GREEN)
		{
			strResult += "back green"; 
		}
		else if(m_back == COLOR_CYAN)
		{
			strResult += "back cyan"; 
		}
		else if(m_back == COLOR_RED)
		{
			strResult += "back red"; 
		}
		else if(m_back == COLOR_MAGENTA)
		{
			strResult += "back magenta"; 
		}
		else if(m_back == COLOR_BROWN)
		{
			strResult += "back brown"; 
		}
		else if(m_back == COLOR_GRAY)
		{
			strResult += "back light grey"; 
		}
	}
}