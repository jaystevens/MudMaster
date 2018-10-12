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
#include "Ifx.h"
#include "HighList.h"
#include "High.h"
#include "CharFormat.h"
#include "Globals.h"
#include "ColorLine.h"
#include "Sess.h"
#include "Colors.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;
using namespace Utilities;

namespace MMScript
{
	CHighList::CHighList()
	{
	}

	CHighList::~CHighList()
	{
	}

	CHigh *CHighList::Add(LPCSTR pszMask, LPCSTR pszColor, LPCSTR pszGroup)
	{
		int nLen = strlen(pszMask);
		if (!nLen)
			return(FALSE);

		// Parese the colors.
		CCharFormat cfFormat(COLOR_GRAY, COLOR_BLACK);
		BYTE nFore=0, nBack=0;

		BOOL bResult = ColorFromString(pszColor, &nFore, &nBack);
		if(!bResult)
			return NULL;

		CHigh high;
		high.Anchored(FALSE);
		high.Tokens(0);
		high.Vars(0);
		high.StartVar(FALSE);
		high.Group(pszGroup);
		high.Enabled(TRUE);
		high.fore(nFore);
		high.back(nBack);

		if (*pszMask == '^')
		{
			high.Anchored(TRUE);
			nLen--;
			pszMask++;
		}

		int nIndex = 0;
		while(nIndex < nLen)
		{
			// Need to check for the escape char to see if they
			// are just trying to use a % instead of indicating
			// a variable.
			if (*(pszMask+nIndex) == CGlobals::GetEscapeCharacter() && nIndex+1 < nLen && *(pszMask+nIndex+1) == '%')
			{
				high.Token(high.Tokens(), high.Tokens() + "%");
				nIndex += 2;				
				continue;
			}

			// Found a variable.
			if (*(pszMask+nIndex) == '%' && nIndex+1 < nLen && isdigit(*(pszMask+nIndex+1)))
			{
				// Need to know if the trigger starts with a variable.
				if (!nIndex)
					high.StartVar(TRUE);

				// Should not ever have an empty token.  This would be caused
				// by creating a trigger like this: "%0%1 stuff" which would
				// be impossible to evaluate.
				if (high.Token(high.Tokens()).IsEmpty() && (nIndex != 0 && high.StartVar() != TRUE))
					return(FALSE);

				// Advance the token counter only if we are not starting with a var.
				if (!nIndex && high.StartVar())
				{
					// Don't advance the token count.
				}
				else
					high.Tokens(high.Tokens()+1);

				// Should have more than 10 vars either.
				if (high.Vars() > 9)
					return(FALSE);
				high.Vars(high.Vars()+1);
				nIndex += 2;
				continue;
			}

			// Need to make sure we don't try to store more thant 10
			// tokens.
			if (high.Tokens() > 9)
				return(FALSE);
			high.Token(high.Tokens(), high.Token(high.Tokens()) + *(pszMask+nIndex));
			nIndex++;
		}
		// If dropped out of loop because of reaching the end of line
		// should have to increment the token counter unless it is
		// empty.
		if (!high.Token(high.Tokens()).IsEmpty())
			high.Tokens(high.Tokens()+1);

		// A highlight declared as anchored, but having a variable at the
		// front of it is pointless.  Make sure it doesn't happen.
		if (high.Anchored() && high.StartVar())
			high.Anchored(FALSE);

		// Put the pointer back to how it used to be for comparisons.
		if (high.Anchored())
		{
			nLen++;
			pszMask--;
		}

		CString strTemp;
		CHigh *pHigh;
		for (int i = 0; i < m_nCount; ++i)
		{
			pHigh = (CHigh *)m_list.GetAt(i);
			pHigh->ToMask(strTemp);

			// high already exists.
			if (strTemp == pszMask)
			{
				*pHigh = high;
				return pHigh;
			}

			if (_stricmp((pHigh->Anchored() ? strTemp.Right(strTemp.GetLength()-1) : strTemp), 
				(*pszMask == '^' ? pszMask+1 : pszMask)) > 0)
			{
				CHigh *pNew = new CHigh;
				*pNew = high;
				AddScriptEntity(pNew, i);
				return pNew;
			}
		}

		// Fell thru the loop, must go at the end.
		CHigh *pNew = new CHigh;
		*pNew = high;
		AddScriptEntity(pNew, m_nCount);
		return pNew;
	}

	BOOL CHighList::RemoveImpl(const CString& strSearch)
	{
		CHigh *pHigh = (CHigh *)FindExact(strSearch);

		if (pHigh == NULL)
			return FALSE;

		return Remove(m_nGetIndex);
	}

	int CHighList::DisableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CHigh *pHigh = (CHigh *)GetFirst();
		while(pHigh != NULL)
		{
			if (pHigh->Group() == pszGroup)
			{
				pHigh->Enabled(FALSE);
				nCount++;
			}
			pHigh = (CHigh *)GetNext();
		}
		return(nCount);
	}

	int CHighList::EnableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CHigh *pHigh = (CHigh *)GetFirst();
		while(pHigh != NULL)
		{
			if (pHigh->Group() == pszGroup)
			{
				pHigh->Enabled(TRUE);
				nCount++;
			}
			pHigh = (CHigh *)GetNext();
		}
		return(nCount);
	}

	CScriptEntity* CHighList::FindExactImpl(const CString& strSearch) const
	{
		CHigh *pHigh;
		CString strMask;
		for (int i=0;i<m_nCount;i++)
		{
			pHigh = (CHigh *)m_list.GetAt(i);
			pHigh->ToMask(strMask);
			if (strMask == strSearch)
			{
				m_nGetIndex = i;
				return pHigh;
			}
		}
		return NULL;
	}

	void CHighList::CheckHighlight(CSession *pSession, CColorLine::SpColorLine &line)
	{
		CString strLine = line->GetTextBuffer();
		CCharFormat *pColorBuffer = line->GetColorBuffer();

		int nLineLen = strLine.GetLength();

		COLORREF *pFore, *pBack;

		pBack = pSession->GetOptions().terminalOptions_.TerminalWindowBackColors();
		pFore = pSession->GetOptions().terminalOptions_.TerminalWindowForeColors();

		CHigh *pHigh;
		for (int i=0;i<m_nCount;i++)
		{
			pHigh = (CHigh *)m_list.GetAt(i);
			if (!pHigh->Enabled())
				continue;
		//TODO:KW count commands processed - highlight counts as a command
			//    if we got this far count it as a processed command
		pSession->SetCmdsProcessed(pSession->GetCmdsProcessed() + 1);

			if (!pHigh->Vars())
			{
				int nIndex = strLine.Find(pHigh->Token(0));
				if ((pHigh->Anchored() && nIndex == 0) || 
					(!pHigh->Anchored() && nIndex != -1))
				{
					int nLen = pHigh->Token(0).GetLength();
					for (int j=0;j<nLen;j++)
						if (strLine[j + nIndex] != '\n')
						{
							pColorBuffer[j + nIndex].Fore(pHigh->fore());
							pColorBuffer[j + nIndex].Back(pHigh->back());
						}
				}
				continue;
			}

			int nVarCount = 0;
			int nOffset = 0;
			CString strTemp;

			int j = 0;
			for (; j < pHigh->Tokens(); ++j)
			{
				int nIndex = strLine.Find(pHigh->Token(j));
				if(nIndex == -1)
					break;

				// If the action starts with a variable and we are on the
				// very first token.
				if (!j)
				{
					// If it is anchored, need to make sure that ptr is at
					// the beginning of the text.
					if (pHigh->Anchored() && nIndex != 0)
						break;

					if (pHigh->StartVar())
					{
						// If the string we just found is pointing at the 
						// front of the line, then there is no var data to
						// be picked off the front.
						if (nIndex == 0)
							break;

						nVarCount++;
					}
					// Offset needs to point to 1 place after the first token
					// ptr-pszLine will skip over the var data + the lenght of the token
					// +1 because vars cannot be butted up against on another.
					nOffset += nIndex + pHigh->Token(j).GetLength() + 1;
					continue;
				}

				strTemp = strLine.Mid(nOffset-1,nIndex-nOffset+1);
				nOffset += strTemp.GetLength() + pHigh->Token(j).GetLength();
				nVarCount++;

			} // for (j=0;j<pAction->nTokens;j++)

			if (nVarCount < pHigh->Vars())
				nVarCount++;

			if (j == pHigh->Tokens())
			{
				for (int i = 0; i < nLineLen; ++i)
					if (strLine[i] != '\n')
					{
						pColorBuffer[i].Fore(pHigh->fore());
						pColorBuffer[i].Back(pHigh->back());
					}	
			}
		}
	}

	BOOL CHighList::ColorFromString(LPCSTR pszColor, BYTE *nFore, BYTE *nBack)
	{
		int nColorLen = strlen(pszColor);
		LPSTR pColor = new char[nColorLen+1];
		strcpy(pColor,pszColor);
		LPSTR ptr = strtok(pColor,",");
		if (ptr == NULL)
		{
			return TextToAttr(pszColor, nFore, nBack);
		}
		else
		{
			while(ptr != NULL)
			{
				BOOL bResult = TextToAttr(ptr, nFore, nBack);
				if(!bResult)
				{
					delete [] pColor;
					pColor = NULL;
					return FALSE;
				}
				ptr = strtok(NULL,",");
			}
		}
		delete [] pColor;
		pColor = NULL;
		return TRUE;
	}

	BOOL CHighList::TextToAttr(LPCSTR pszColor, BYTE *nFore, BYTE *nBack)
	{
		CString strColor(pszColor);
		strColor.MakeLower();
		strColor.TrimRight();
		strColor.TrimLeft();

		// Foreground colors.
		if (strColor == "black")
		{	
			*nFore = COLOR_BLACK;
			return TRUE;
		}
		else if (strColor == "blue")
		{
			*nFore = COLOR_BLUE;
			return TRUE;
		}
		else if (strColor == "green")
		{
			*nFore = COLOR_GREEN;
			return TRUE;
		}
		else if (strColor == "cyan")
		{
			*nFore = COLOR_CYAN;
			return TRUE;
		}
		else if (strColor == "red")
		{
			*nFore = COLOR_RED;
			return TRUE;
		}
		else if (strColor == "magenta")
		{
			*nFore = COLOR_MAGENTA;
			return TRUE;
		}
		else if (strColor == "brown")
		{
			*nFore = COLOR_BROWN;
			return TRUE;
		}
		else if (strColor == "light grey")
		{
			*nFore = COLOR_GRAY;
			return TRUE;
		}
		else if (strColor == "dark grey")
		{
			*nFore = COLOR_DARKGRAY;
			return TRUE;
		}
		else if (strColor == "light blue")
		{
			*nFore = COLOR_LIGHTBLUE;
			return TRUE;
		}
		else if (strColor == "light green")
		{
			*nFore = COLOR_LIGHTGREEN;
			return TRUE;
		}
		else if (strColor == "light cyan")
		{
			*nFore = COLOR_LIGHTCYAN;
			return TRUE;
		}
		else if (strColor == "light red")
		{
			*nFore = COLOR_LIGHTRED;
			return TRUE;
		}
		else if (strColor == "light magenta")
		{
			*nFore = COLOR_LIGHTMAGENTA;
			return TRUE;
		}
		else if (strColor == "yellow")
		{
			*nFore = COLOR_YELLOW;
			return TRUE;
		}
		else if (strColor == "white")
		{
			*nFore = COLOR_WHITE;
			return TRUE;
		}
		else if (strColor == "back black")
		{
			*nBack = COLOR_BLACK;
			return TRUE;
		}
		else if (strColor == "back blue")
		{
			*nBack = COLOR_BLUE;
			return TRUE;
		}
		else if (strColor == "back green")
		{
			*nBack = COLOR_GREEN;
			return TRUE;
		}
		else if (strColor == "back cyan")
		{
			*nBack = COLOR_CYAN;
			return TRUE;
		}
		else if (strColor == "back red")
		{
			*nBack = COLOR_RED;
			return TRUE;
		}
		else if (strColor == "back magenta")
		{
			*nBack = COLOR_MAGENTA;
			return TRUE;
		}
		else if (strColor == "back brown")
		{
			*nBack = COLOR_BROWN;
			return TRUE;
		}
		else if (strColor == "back light grey")
		{
			*nBack = COLOR_GRAY;
			return TRUE;
		}
		return FALSE;
	}

	CScriptEntity *CHighList::AddToList(CScriptEntity * /*pEntity*/)
	{
		ASSERT(FALSE);
		return NULL;
	}

	HRESULT CHighList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CString strColor;
			CHigh *pHigh = (CHigh *)GetFirst();
			while(pHigh != NULL)
			{
				if( pHigh->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					pHigh->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						strText));

					++nSent;
				}
				pHigh = (CHigh *)GetNext();
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