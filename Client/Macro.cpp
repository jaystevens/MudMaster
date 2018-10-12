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
// Macro.cpp: implementation of the CMacro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ifx.h"
#include "Macro.h"
#include "Sess.h"
#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace MMScript
{
	CMacro::CMacro()
	{
		m_bEnabled = TRUE;
		m_nDest = MacroDestBar;
		m_wModifiers = 0;
		m_wVirtualKeyCode = 0;
	}

	CMacro::~CMacro()
	{

	}

	void CMacro::MapToCommand(CString &strCommand)
	{
		CString strText;

		MapToText(strText, TRUE);

		strCommand.Format("%cmacro %s", 
			CGlobals::GetCommandCharacter(),
			strText);
	}

	void CMacro::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		CString strKeyName;
		KeyToName(strKeyName);

		strText.Format("{%s} {", strKeyName);

		strText += Action();

		switch(Dest())
		{
		case MacroDestBar:
			strText+=";";
			break;
		case MacroDestBarCr:
			strText+=":";
			break;
		case MacroDestMud:
			break;
		}

		strText += "}";
		if (bIncludeGroup)
		{
			strText += " {";
			strText += Group();
			strText += "}";
		}

	}

	void CMacro::KeyToName(CString &strName)
	{
		strName.Empty();
		if (m_wModifiers & HOTKEYF_ALT)
			strName = "a";

		if (m_wModifiers & HOTKEYF_CONTROL)
			if (strName.IsEmpty())
				strName = "c";
			else
				strName += "c";

		if (m_wModifiers & HOTKEYF_SHIFT)
			if (strName.IsEmpty())
				strName = "s";
			else
				strName += "s";

		if (m_wVirtualKeyCode >= 48 && m_wVirtualKeyCode <= 57)
		{
			if(strName.IsEmpty())
				strName = (char)m_wVirtualKeyCode;
			else
				strName += (char)m_wVirtualKeyCode;
			return;
		}

		if (m_wVirtualKeyCode >= 65 && m_wVirtualKeyCode <= 90)
		{
			if(strName.IsEmpty())
				strName = (char)m_wVirtualKeyCode;
			else
				strName += (char)m_wVirtualKeyCode;
			return;
		}

		if (m_wVirtualKeyCode >= VK_NUMPAD0 && m_wVirtualKeyCode <= VK_NUMPAD9)
		{
			if(strName.IsEmpty())
			{
				strName = "kp";
				strName += (char)(m_wVirtualKeyCode - VK_NUMPAD0 + '0');
			}
			else
			{
				strName += "kp";
				strName += (char)(m_wVirtualKeyCode - VK_NUMPAD0 + '0');
			}
			return;
		}

		if (m_wVirtualKeyCode >= VK_F1 && m_wVirtualKeyCode <= VK_F24)
		{
			if(strName.IsEmpty())
			{
				strName.Format("F%d",m_wVirtualKeyCode-VK_F1+1);
			}
			else
			{
				CString strKey;
				strKey.Format("F%d", m_wVirtualKeyCode-VK_F1+1);
				strName += strKey;
			}
			return;
		}

		CString strText;
		switch(m_wVirtualKeyCode)
		{
		case VK_PRIOR :
			strText = "pgup";
			break;

		case VK_NEXT :
			strText = "pgdn";
			break;

		case VK_END :
			strText = "end";
			break;

		case VK_HOME :
			strText = "home";
			break;

		case VK_LEFT :
			strText = "ltarrow";
			break;

		case VK_RIGHT :
			strText = "rtarrow";
			break;

		case VK_UP :
			strText = "uparrow";
			break;

		case VK_DOWN :
			strText = "dnarrow";
			break;

		case VK_INSERT :
			strText = "ins";
			break;

		case VK_DELETE :
			strText = "del";
			break;

		case VK_MULTIPLY:
			strText = "kpstar";
			break;

		case VK_ADD:
			strText = "kpplus";
			break;

		case VK_SUBTRACT:
			strText = "kpminus";
			break;

		case VK_DIVIDE:
			strText = "kpslash";
			break;

		case VK_DECIMAL:
			strText = "kpdel";
			break;

		default :
			strText.Empty();
			break;
		}

		if(strName.IsEmpty())
			strName = strText;
		else
			strName += strText;
	}
}