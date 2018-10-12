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
#include "BarItem.h"
#include "Colors.h"
#include "Globals.h"
#include "Ifx.h"
#include "Sess.h"
#include "StatusBar.h"
#include "ParseUtils.h"
#include "TWChar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;

namespace MMScript
{
	CBarItem::CBarItem()
	{
		m_bEnabled		= TRUE;
		m_bSeparator	= FALSE;
		m_nBack			= COLOR_BLACK;
		m_nFore			= COLOR_GRAY;
		m_nLength		= 0;
		m_nPosition		= 0;
	}

	CBarItem::CBarItem(const CBarItem & src):
	m_bEnabled(src.m_bEnabled),
		m_bSeparator(src.m_bSeparator),
		m_nBack(src.m_nBack),
		m_nFore(src.m_nFore),
		m_nLength(src.m_nLength),
		m_nPosition(src.m_nPosition),
		m_strGroup(src.m_strGroup),
		m_strItem(src.m_strItem),
		m_strText(src.m_strText)
	{

	}

	CBarItem::~CBarItem()
	{

	}


	void CBarItem::MapToCommand(CString &strCommand)
	{
		CString strText;
		MapToText(strText, TRUE);

		if(Separator())
		{
			strCommand.Format("%cbarseparator %s",
				CGlobals::GetCommandCharacter(),
				strText);
			return;
		}
		else
		{
			strCommand.Format("%cbaritem %s",
				CGlobals::GetCommandCharacter(),
				strText);
			return;
		}
	}

	void CBarItem::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		if(bIncludeGroup)
		{
			if (Separator())
			{
				strText.Format("{%s} {%d} {%s}",
					Item(),
					Position(),
					Group());
				return;
			}
			else
			{
				strText.Format("{%s} {%s} {%d} {%d} {%d} {%d} {%s}",
					Item(),
					Text(),
					Position(),
					Length(),
					ForeColor(),
					BackColor(),
					Group());
				return;
			}
		}
		else
		{
			if (Separator())
			{
				strText.Format("{%s} {%d}",
					Item(),
					Position());
				return;
			}
			else
			{
				strText.Format("{%s} {%s} {%d} {%d} {%d} {%d}",
					Item(),
					Text(),
					Position(),
					Length(),
					ForeColor(),
					BackColor());
				return;
			}
		}
	}

	void CBarItem::MapToDisplayText(CString &strText, CSession *pSession)
	{
		if (Separator())
		{
			CMMStatusBar *pStatusBar = pSession->GetStatusBar();
			if(NULL == pStatusBar)
			{
				TRACE(_T("status bar is null!"));
			}

			strText.Format("%s%03d:%s%c{Pos:%2d} {Len: 1} {%s} {SEPARATOR} {%s}",
				ANSI_F_BOLDWHITE,
				pStatusBar->GetFindIndex()+1,
				ANSI_RESET,
				Enabled() ? ' ' : '*',
				Position(),
				Item(),
				Group());
		}
		else
		{
			strText.Format("%s%03d:%s%c{Pos:%2d} {Len:%2d} {%s} {%s} {Fore:%d} {Back:%d} {%s}",
				ANSI_F_BOLDWHITE,
				pSession->GetStatusBar()->GetFindIndex()+1,
				ANSI_RESET,
				Enabled() ? ' ' : '*',
				Position(),
				Length(),
				Item(),
				Text(),
				ForeColor(),
				BackColor(),
				Group());
		}

	}

	void CBarItem::DrawItem(CSession *pSession, CPtrArray &bar)
	{
		CString strText(Text());
		CParseUtils::ReplaceVariables(pSession, strText);

		if (strText.GetLength() > Length())
			strText = strText.Left(Length());

		int x = Position()-1;
		if(x<0)
		{
			x=0;
			Position(1);
		}

		int len = Length();
		int y = 0;
		for(; y < strText.GetLength(); y++)
		{
			((CTWChar *)bar[y + x])->Character(strText[y]);
			((CTWChar *)bar[y + x])->Attribute(
				CTWChar::MakeAttr(ForeColor(), BackColor()), FALSE);
		}

		for(;y<len;y++)
		{
			((CTWChar *)bar[y + x])->Character(' ');
			((CTWChar *)bar[y + x])->Attribute(
				CTWChar::MakeAttr(ForeColor(), BackColor()), FALSE);
		}
	}
}