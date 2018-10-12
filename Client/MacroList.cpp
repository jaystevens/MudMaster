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
#include "MacroList.h"
#include "Macro.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMScript
{
	CMacroList::CMacroList()
	{
	}

	CMacroList::~CMacroList()
	{
	}

	BOOL CMacroList::RemoveImpl(const CString& strSearch)
	{
		WORD wVirtualKeyCode, wModifiers;
		if (!NameToKey(strSearch,wVirtualKeyCode,wModifiers))
			return FALSE;

		CMacro *pMacro;
		for (int i=0;i<m_nCount;i++)
		{
			pMacro = (CMacro *)m_list.GetAt(i);
			if (pMacro->VirtualKeyCode() == wVirtualKeyCode && pMacro->Modifiers() == wModifiers)
			{
				return Remove(i);
			}
		}
		return FALSE;
	}

	CMacro *CMacroList::Add(const CString& strKey, const CString& strAction, const CString& strGroup, int nDest)
	{
		WORD wVirtualKeyCode, wModifiers;
		if (!NameToKey(strKey,wVirtualKeyCode,wModifiers))
			return(FALSE);
		return(Add(wVirtualKeyCode, wModifiers, strAction, strGroup, nDest));
	}

	CMacro *CMacroList::Add(WORD wVirtualKeyCode, WORD wModifiers, const CString& strAction, const CString& strGroup, int nDest)
	{
		BOOL bResult = ValidateKey(wVirtualKeyCode, wModifiers);

		if(!bResult)
			return FALSE;

		CMacro *pMac;
		for (int i = 0; i < m_nCount; ++i)
		{
			pMac = (CMacro *)m_list.GetAt(i);

			// If it exists already, replace it.
			if (pMac->VirtualKeyCode() == wVirtualKeyCode && pMac->Modifiers() == wModifiers)
			{
				pMac->Action(strAction);
				pMac->Group(strGroup);
				pMac->Dest(nDest);
				return pMac;
			}

			// Insert a new macro.
			if (pMac->VirtualKeyCode() > wVirtualKeyCode)
			{
				CMacro *pNew = new CMacro;
				pNew->Action(strAction);
				pNew->Group(strGroup);
				pNew->Enabled(TRUE);
				pNew->VirtualKeyCode(wVirtualKeyCode);
				pNew->Modifiers(wModifiers);
				pNew->Dest(nDest);
				AddScriptEntity(pNew, i);
				return pNew;
			}
		}

		// Add it to the tail.
		CMacro *pNew = new CMacro;
		pNew->Action(strAction);
		pNew->Group(strGroup);
		pNew->Enabled(TRUE);
		pNew->VirtualKeyCode(wVirtualKeyCode);
		pNew->Modifiers(wModifiers);
		pNew->Dest(nDest);
		AddScriptEntity(pNew, m_nCount);
		return pNew;
	}

	// This verison of add does not check for an existing matching 
	// macro.  This can cause duplicates.
	CScriptEntity *CMacroList::AddToList(CScriptEntity *pMacro)
	{
		CMacro *ptr = dynamic_cast<CMacro *>(pMacro);
		CMacro *pMac;
		for (int i = 0; i < m_nCount; ++i)
		{
			pMac = (CMacro *)m_list.GetAt(i);

			if (pMac->VirtualKeyCode() > ptr->VirtualKeyCode())
			{
				AddScriptEntity(ptr, i);
				return ptr;
			}
		}

		AddScriptEntity(ptr, m_nCount);
		return ptr;
	}

	BOOL CMacroList::NameToKey(LPCSTR pszKey, WORD &wVirtualKeyCode, WORD &wModifiers)
	{
		CString strKey(pszKey);
		strKey.MakeLower();

		// Need to first pull the key name off the right side of 
		// the string.
		CString strName;

		// Shift+Alpha is an invalid combination.  Shift-a needs to
		// give you an 'A' not a macro.  When a key name is found that
		// is an ascii char this is set to TRUE.
		BOOL bAscii = FALSE;

		// If it ends with a digit it must be either a function key
		// or a keypad digit.
		int nIndex = strKey.GetLength()-1;
		if (isdigit(strKey.GetAt(nIndex)))
		{
			while(nIndex && isdigit(strKey.GetAt(nIndex)))
				nIndex--;

			// If the key isn't a p or f, then is probably a single
			// digit key.
			if (strKey.GetAt(nIndex) != 'p' && strKey.GetAt(nIndex) != 'f')
			{
				// If there was more than 1 digit, something went wrong.
				if (nIndex != strKey.GetLength()-2)
					return(FALSE);
				strName = strKey.Right(1);
				strKey = strKey.Left(strKey.GetLength()-1);
				bAscii = TRUE;
			}
			else
			{
				// After last digit has been found, it has to either
				// be an 'f' or a 'p'.  If it is a 'p' then need to subtract
				// 1 more from the index, key name will be "kp<num>" or
				// "f<num>"
				if (strKey.GetAt(nIndex) == 'p')
					if (nIndex)
						nIndex--;
					else
						return(FALSE);	// Would go past head of buffer.

				// Pull off the name.
				strName = strKey.Right(strKey.GetLength()-nIndex);
				strKey = strKey.Left(nIndex);
			}
		}
		else
		{
			CString strTemp;
			BOOL bFoundMatch = FALSE;
			int nLen = strKey.GetLength();
			int i = 7;
			for (; i > 2 && !bFoundMatch; --i)
			{
				if (nLen >= i)
				{
					strTemp = strKey.Right(i);
					switch(i)
					{
					case 7 :
						if (strTemp == "kpslash")
							bFoundMatch = TRUE;
						if (strTemp == "kpminus")
							bFoundMatch = TRUE;
						if (strTemp == "uparrow")
							bFoundMatch = TRUE;
						if (strTemp == "dnarrow")
							bFoundMatch = TRUE;
						if (strTemp == "ltarrow")
							bFoundMatch = TRUE;
						if (strTemp == "rtarrow")
							bFoundMatch = TRUE;
						break;

					case 6 :
						if (strTemp == "kpstar")
							bFoundMatch = TRUE;
						if (strTemp == "kpplus")
							bFoundMatch = TRUE;
						break;

					case 5 :
						if (strTemp == "kpdel")
							bFoundMatch = TRUE;
						break;

					case 4 :
						if (strTemp == "home")
							bFoundMatch = TRUE;
						if (strTemp == "pgup")
							bFoundMatch = TRUE;
						if (strTemp == "pgdn")
							bFoundMatch = TRUE;
						break;

					case 3 :
						if (strTemp == "del")
							bFoundMatch = TRUE;
						if (strTemp == "ins")
							bFoundMatch = TRUE;
						if (strTemp == "end")
							bFoundMatch = TRUE;
						break;
					} // switch(i)
				} // if (nLen > = i)
			} // for (int i=8;i>2,!bFoundMatch;i--)

			// If we didn't find a match, assume it was an alpha key.
			if (!bFoundMatch)
			{
				// Needs to have at least 1 modifier.  If the length is
				// 1, then there isn't one.
				if (nLen < 2)
					return(FALSE);
				strName = strKey.Right(1);
				strKey = strKey.Left(nLen-1);
				bAscii = TRUE;
			}
			else
			{
				strName = strTemp;
				strKey = strKey.Left(nLen-i);
			}
		}

		wVirtualKeyCode = VirtualKeyFromName(strName);
		if (!wVirtualKeyCode)
			return(FALSE);

		// strKey should now be equal to the modifiers if there were
		// any.
		BOOL bAlt = FALSE;
		BOOL bShift = FALSE;
		BOOL bCtrl = FALSE;
		int nLen = strKey.GetLength();
		for (int i=0;i<nLen;i++)
		{
			switch(strKey.GetAt(i))
			{
			case 'a' : 
				bAlt = TRUE;
				break;

			case 's' :
				bShift = TRUE;
				break;

			case 'c' :
				bCtrl = TRUE;
				break;
			}
		}

		wModifiers = 0;
		if (bAlt)
			wModifiers |= HOTKEYF_ALT;
		if (bShift)
			wModifiers |= HOTKEYF_SHIFT;
		if (bCtrl)
			wModifiers |= HOTKEYF_CONTROL;

		return(TRUE); 
	}

	WORD CMacroList::VirtualKeyFromName(LPCSTR pszKey)
	{
		int nLen = strlen(pszKey);
		if (nLen == 1 && toupper(*pszKey) >= 'A' && toupper(*pszKey) <= 'Z')
			return((WORD)toupper(*pszKey));
		if (nLen == 1 && *pszKey >= 48 && *pszKey <= 57)
			return(*pszKey);

		if (!strcmp(pszKey,"f1"))
			return(VK_F1);
		if (!strcmp(pszKey,"f2"))
			return(VK_F2);
		if (!strcmp(pszKey,"f3"))
			return(VK_F3);
		if (!strcmp(pszKey,"f4"))
			return(VK_F4);
		if (!strcmp(pszKey,"f5"))
			return(VK_F5);
		if (!strcmp(pszKey,"f6"))
			return(VK_F6);
		if (!strcmp(pszKey,"f7"))
			return(VK_F7);
		if (!strcmp(pszKey,"f8"))
			return(VK_F8);
		if (!strcmp(pszKey,"f9"))
			return(VK_F9);
		if (!strcmp(pszKey,"f10"))
			return(VK_F10);
		if (!strcmp(pszKey,"f11"))
			return(VK_F11);
		if (!strcmp(pszKey,"f12"))
			return(VK_F12);
		if (!strcmp(pszKey,"del"))
			return(VK_DELETE);
		if (!strcmp(pszKey,"kpdel"))
			return(VK_DECIMAL);
		if (!strcmp(pszKey,"kp1"))
			return(VK_NUMPAD1);
		if (!strcmp(pszKey,"kp2"))
			return(VK_NUMPAD2);
		if (!strcmp(pszKey,"kp3"))
			return(VK_NUMPAD3);
		if (!strcmp(pszKey,"kp4"))
			return(VK_NUMPAD4);
		if (!strcmp(pszKey,"kp5"))
			return(VK_NUMPAD5);
		if (!strcmp(pszKey,"kp6"))
			return(VK_NUMPAD6);
		if (!strcmp(pszKey,"kp7"))
			return(VK_NUMPAD7);
		if (!strcmp(pszKey,"kp8"))
			return(VK_NUMPAD8);
		if (!strcmp(pszKey,"kp9"))
			return(VK_NUMPAD9);
		if (!strcmp(pszKey,"kp0"))
			return(VK_NUMPAD0);
		if (!strcmp(pszKey,"ins"))
			return(VK_INSERT);
		if (!strcmp(pszKey,"home"))
			return(VK_HOME);
		if (!strcmp(pszKey,"pgup"))
			return(VK_PRIOR);
		if (!strcmp(pszKey,"pgdn"))
			return(VK_NEXT);
		if (!strcmp(pszKey,"end"))
			return(VK_END);
		if (!strcmp(pszKey,"uparrow"))
			return(VK_UP);
		if (!strcmp(pszKey,"dnarrow"))
			return(VK_DOWN);
		if (!strcmp(pszKey,"rtarrow"))
			return(VK_RIGHT);
		if (!strcmp(pszKey,"ltarrow"))
			return(VK_LEFT);
		if (!strcmp(pszKey,"kpstar"))
			return(VK_MULTIPLY);
		if (!strcmp(pszKey,"kpplus"))
			return(VK_ADD);
		if (!strcmp(pszKey,"kpminus"))
			return(VK_SUBTRACT);
		if (!strcmp(pszKey,"kpslash"))
			return(VK_DIVIDE);
		return(0);
	}

	BOOL CMacroList::DisableMacro(LPCSTR pszKey)
	{
		WORD wVirtualKeyCode, wModifiers;
		if (!NameToKey(pszKey,wVirtualKeyCode,wModifiers))
			return(FALSE);
		CMacro *pMac;
		for (int i=0;i<m_nCount;i++)
		{
			pMac = (CMacro *)m_list.GetAt(i);
			if (pMac->VirtualKeyCode() == wVirtualKeyCode && pMac->Modifiers() == wModifiers)
			{
				pMac->Enabled(FALSE);
				return(TRUE);
			} 
		}
		return(FALSE);
	}

	BOOL CMacroList::EnableMacro(LPCSTR pszKey)
	{
		WORD wVirtualKeyCode, wModifiers;
		if (!NameToKey(pszKey,wVirtualKeyCode,wModifiers))
			return(FALSE);
		CMacro *pMac;
		for (int i=0;i<m_nCount;i++)
		{
			pMac = (CMacro *)m_list.GetAt(i);
			if (pMac->VirtualKeyCode() == wVirtualKeyCode && pMac->Modifiers() == wModifiers)
			{
				pMac->Enabled(TRUE);
				return(TRUE);
			} 
		}
		return(FALSE);
	}

	int CMacroList::DisableGroup(LPCSTR pszGroup)
	{
		CMacro *pMac;
		int nCount = 0;
		for (int i=0;i<m_nCount;i++)
		{
			pMac = (CMacro *)m_list.GetAt(i);
			if (pMac->Group() == pszGroup)
			{
				pMac->Enabled(FALSE);
				nCount++;
			}
		}
		return(nCount); 
	}

	int CMacroList::EnableGroup(LPCSTR pszGroup)
	{
		CMacro *pMac;
		int nCount = 0;
		for (int i=0;i<m_nCount;i++)
		{
			pMac = (CMacro *)m_list.GetAt(i);
			if (pMac->Group() == pszGroup)
			{
				pMac->Enabled(TRUE);
				nCount++;
			}
		}
		return(nCount); 
	}

	CMacro* CMacroList::FindKeyByName(LPCSTR pszKey)
	{
		WORD wVirtualKeyCode, wModifiers;
		if (!NameToKey(pszKey,wVirtualKeyCode,wModifiers))
			return(NULL);
		return(FindByValue(wVirtualKeyCode, wModifiers));
	}

	CMacro* CMacroList::FindByValue(WORD wVirtualKeyCode, WORD wModifiers)
	{
		CMacro *pMac;
		for (int i=0;i<m_nCount;i++)
		{
			pMac = (CMacro *)m_list.GetAt(i);
			if (pMac->VirtualKeyCode() == wVirtualKeyCode && pMac->Modifiers() == wModifiers)
				return(pMac);
		}
		return(NULL);
	}

	void PASCAL CMacroList::GetVirtualKeyName(WORD wVirtualKeyCode, CString &strName)
	{
		if (wVirtualKeyCode >= 48 && wVirtualKeyCode <= 57)
		{
			strName = (char)wVirtualKeyCode;
			return;
		}

		if (wVirtualKeyCode >= 65 && wVirtualKeyCode <= 90)
		{
			strName = (char)wVirtualKeyCode;
			return;
		}

		if (wVirtualKeyCode >= VK_NUMPAD0 && wVirtualKeyCode <= VK_NUMPAD9)
		{
			strName = CString("Num ") + (char)(wVirtualKeyCode-VK_NUMPAD0+'0');
			return;
		}

		if (wVirtualKeyCode >= VK_F1 && wVirtualKeyCode <= VK_F24)
		{
			strName.Format("F%d",wVirtualKeyCode-VK_F1+1);
			return;
		}

		switch(wVirtualKeyCode)
		{
		case VK_BACK :
			strName = "Backspace";
			break;

		case VK_TAB :
			strName = "Tab";
			break;

		case VK_CLEAR :
			strName = "Clear";
			break;

		case VK_RETURN :
			strName = "Enter";
			break;

		case VK_ESCAPE :
			strName = "Escape";
			break;

		case VK_SPACE :
			strName = "Space";
			break;

		case VK_PRIOR :
			strName = "Page Up";
			break;

		case VK_NEXT :
			strName = "Page Down";
			break;

		case VK_END :
			strName = "End";
			break;

		case VK_HOME :
			strName = "Home";
			break;

		case VK_LEFT :
			strName = "Left Arrow";
			break;

		case VK_RIGHT :
			strName = "Right Arrow";
			break;

		case VK_UP :
			strName = "Up Arrow";
			break;

		case VK_DOWN :
			strName = "Down Arrow";
			break;

		case VK_SELECT :
			strName = "Select";
			break;

		case VK_EXECUTE :
			strName = "Execute";
			break;

		case VK_SNAPSHOT :
			strName = "Print Screen";
			break;

		case VK_INSERT :
			strName = "Insert";
			break;

		case VK_DELETE :
			strName = "Delete";
			break;

		case VK_HELP :
			strName = "Help";
			break;

		case VK_MULTIPLY :
			strName = "Multiply";
			break;

		case VK_ADD :
			strName = "Add";
			break;

		case VK_SEPARATOR :
			strName = "Separator";
			break;

		case VK_SUBTRACT :
			strName = "Subtract";
			break;

		case VK_DECIMAL :
			strName = "Decimal";
			break;

		case VK_DIVIDE :
			strName = "Divide";
			break;

		case VK_NUMLOCK :
			strName = "Num Lock";
			break;

		case VK_SCROLL :
			strName = "Scroll Lock";
			break;

		default :
			strName.Empty();
			break;
		}
	}

	void PASCAL CMacroList::GetModifierName(WORD wModifiers, CString &strModifier)
	{
		strModifier.Empty();
		if (wModifiers & HOTKEYF_ALT)
			strModifier = "Alt";

		if (wModifiers & HOTKEYF_CONTROL)
			if (strModifier.IsEmpty())
				strModifier = "Ctrl";
			else
				strModifier += "+Ctrl";

		if (wModifiers & HOTKEYF_SHIFT)
			if (strModifier.IsEmpty())
				strModifier = "Shift";
			else
				strModifier += "+Shift";
	}

	BOOL CMacroList::ValidateKey(WORD wVirtualKey, WORD wModifiers)
	{
		// a - z
		if(wVirtualKey > 0x40 && wVirtualKey < 0x5B)
		{
			// if its a letter, it must have an alt or a ctrl
			if(wModifiers & HOTKEYF_ALT)
				return TRUE;

			if(wModifiers & HOTKEYF_CONTROL)
				return TRUE;

			return FALSE;
		}

		// all keypad keys can be mapped
		// all arrow keys can be mapped

		if(wModifiers == 0)
		{
			if(wVirtualKey == VK_ESCAPE)
				return FALSE;

			if(wVirtualKey == VK_SPACE)
				return FALSE;

			if(wVirtualKey == VK_UP)
				return FALSE;

			if(wVirtualKey == VK_DOWN)
				return FALSE;

			if(wVirtualKey == VK_NUMLOCK)
				return FALSE;

			if(wVirtualKey == VK_SCROLL)
				return FALSE;

			if(wVirtualKey == VK_PRIOR)
				return FALSE;

			if(wVirtualKey == VK_NEXT)
				return FALSE;
		}


		if(wVirtualKey == VK_SELECT ||
			wVirtualKey == VK_PRINT ||
			wVirtualKey == VK_EXECUTE ||
			wVirtualKey == VK_SNAPSHOT ||
			wVirtualKey == VK_HELP)
			return FALSE;

		return TRUE;
	}

	CScriptEntity *CMacroList::FindExactImpl(const CString& /*strSearch*/) const
	{
		ASSERT(FALSE);
		return NULL;
	}

	HRESULT CMacroList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CMacro *pMac = (CMacro *)GetFirst();

			while(pMac != NULL)
			{
				if ( pMac->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					pMac->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						strText));

					++nSent;
				}

				pMac = (CMacro *)GetNext();
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return hr;
	}
}