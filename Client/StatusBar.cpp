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
#include "StatusBar.h"
#include "BarItem.h"
#include "Colors.h"
#include "Sess.h"
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
	CMMStatusBar::CMMStatusBar()
	{
		m_nFore = DEFAULT_COLORS[COLOR_YELLOW];
		m_nBack = DEFAULT_COLORS[COLOR_MAGENTA];
		m_bShowing = FALSE;
	}

	CMMStatusBar::~CMMStatusBar()
	{
	}


	int CMMStatusBar::DisableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CBarItem *pItem = (CBarItem *)GetFirst();
		while(pItem != NULL)
		{
			if (pItem->Group() == pszGroup)
			{
				pItem->Enabled(FALSE);
				nCount++;
			}
			pItem = (CBarItem *)GetNext();
		}
		return(nCount);
	}

	int CMMStatusBar::EnableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CBarItem *pItem = (CBarItem *)GetFirst();
		while(pItem != NULL)
		{
			if (pItem->Group() == pszGroup)
			{
				pItem->Enabled(TRUE);
				nCount++;
			}
			pItem = (CBarItem *)GetNext();
		}
		return(nCount);
	}

	void CMMStatusBar::SetBarFore(CSession *pSession, int nColor)
	{
		m_nFore = nColor;
		if (m_bShowing)
			pSession->RedrawStatusBar();
	}

	void CMMStatusBar::SetBarBack(CSession *pSession, int nColor)
	{
		m_nBack = nColor;
		if (m_bShowing)
			pSession->RedrawStatusBar();
	}

	void CMMStatusBar::SetBarItemFore(CSession *pSession, CBarItem *pItem, BYTE nColor)
	{
		pItem->ForeColor(nColor);
		DrawItem(pSession, pItem);
	}

	void CMMStatusBar::SetBarItemBack(CSession *pSession, CBarItem *pItem, BYTE nColor)
	{
		pItem->BackColor(nColor);
		DrawItem(pSession, pItem);
	}

	void CMMStatusBar::ShiftItems(int nStart, int nEnd, int nNum)
	{
		if (nStart < 0 || nStart >= m_nCount || nStart > nEnd)
			return;
		if (nEnd < 0 || nEnd >= m_nCount)
			return;

		int nNewPos;
		CBarItem *pItem;
		for (int i=nStart;i<=nEnd;i++)
		{
			pItem = (CBarItem *)GetAt(i);
			nNewPos = pItem->Position() + nNum;
			if (nNewPos > 0 && nNewPos < 481)
				pItem->Position(nNewPos);
		}

		int j;
		CBarItem *pItemI, *pItemJ;
		for (int i = 0; i < m_nCount; ++i)
			for (j=0;j<m_nCount-1;j++)
			{
				pItemI = (CBarItem *)GetAt(i);
				pItemJ = (CBarItem *)GetAt(j);
				if (pItemI->Position() < pItemJ->Position())
				{
					m_list.SetAt(i,pItemJ);
					m_list.SetAt(j,pItemI);
				}
			}
	}

	void CMMStatusBar::ShowBar(CSession *pSession)
	{
		pSession->ShowStatusBar();
	}

	void CMMStatusBar::HideBar(CSession *pSession)
	{
		pSession->HideStatusBar();
	}

	void CMMStatusBar::DrawItem(CSession *pSession, CBarItem *pItem)
	{
		pSession->UpdateBarItem(pItem);
	}

	BOOL CMMStatusBar::RemoveImpl(const CString& strSearch)
	{
		CBarItem *pItem;
		for (int i=0;i<m_nCount;i++)
		{
			pItem = (CBarItem *)m_list.GetAt(i);
			if (pItem->Item() == strSearch)
			{
				return Remove(i);
			}
		}
		return(FALSE);
	}

	int CMMStatusBar::AddSeparator(LPCSTR pszItem, int nPos,LPCSTR pszGroup)
	{
		int i;

		CBarItem *pItem = FindItem(pszItem);
		if (pItem != NULL)
		{
			pItem->Item(pszItem);
			pItem->Text('|');
			pItem->Position(nPos);
			pItem->Length(1);
			pItem->ForeColor(TW_YELLOW);
			pItem->BackColor(TW_BLACK);
			pItem->Separator(TRUE);
			pItem->Group(pszGroup);
			for (i=0;i<m_nCount;i++)
				if ((CBarItem*)m_list.GetAt(i) == pItem)
					return(i+1);

			// Shouldn't actually get to this one.
			return(1);
		}

		for (i=0;i<m_nCount;i++)
		{
			pItem = (CBarItem *)m_list.GetAt(i);

			if (pItem->Position() > nPos)
			{
				CBarItem *pNew = new CBarItem;
				pNew->Item(pszItem);
				pNew->Text('|');
				pNew->Position(nPos);
				pNew->Length(1);
				pNew->ForeColor(TW_YELLOW);
				pNew->BackColor(TW_BLACK);
				pNew->Separator(TRUE);
				pNew->Group(pszGroup);
				pNew->Enabled(TRUE);
				m_list.InsertAt(i,pNew);
				m_nCount++;
				return(i+1);
			}
		}

		CBarItem *pNew = new CBarItem;
		pNew->Item(pszItem);
		pNew->Text('|');
		pNew->Position(nPos);
		pNew->Length(1);
		pNew->ForeColor(TW_YELLOW);
		pNew->BackColor(TW_BLACK);
		pNew->Separator(TRUE);
		pNew->Group(pszGroup);
		pNew->Enabled(TRUE);
		m_list.SetAtGrow(i,pNew);
		m_nCount++;
		return(m_nCount);
	}

	int CMMStatusBar::AddBarItem(LPCSTR pszItem, LPCSTR pszText, 
		int nPos, int nLen, BYTE nFore, BYTE nBack, LPCSTR pszGroup)
	{
		int i;
		//TODO: KW make 80 160
		if (nPos + nLen > 480)
			nLen = 480 - nPos;

		// If the item is already in the list we need to update that
		// item.
		CBarItem *pItem = FindItem(pszItem);
		if (pItem != NULL)
		{
			pItem->Item(pszItem);
			pItem->Text(pszText);
			pItem->Position(nPos);
			pItem->Length(nLen);
			pItem->ForeColor(nFore);
			pItem->BackColor(nBack);
			pItem->Separator(FALSE);
			pItem->Group(pszGroup);
			for (i=0;i<m_nCount;i++)
				if ((CBarItem*)m_list.GetAt(i) == pItem)
					return(i+1);
			return(1);
		}

		// If not already in the list we need to insert it into the
		// list sorted by position.

		for (i=0;i<m_nCount;i++)
		{
			pItem = (CBarItem *)m_list.GetAt(i);

			if (pItem->Position() > nPos)
			{
				CBarItem *pNew = new CBarItem;
				pNew->Item(pszItem);
				pNew->Text(pszText);
				pNew->Position(nPos);
				pNew->Length(nLen);
				pNew->ForeColor(nFore);
				pNew->BackColor(nBack);
				pNew->Separator(FALSE);
				pNew->Group(pszGroup);
				pNew->Enabled(TRUE);
				m_list.InsertAt(i,pNew);
				m_nCount++;
				return(i+1);
			}
		}

		CBarItem *pNew = new CBarItem;
		pNew->Item(pszItem);
		pNew->Text(pszText);
		pNew->Position(nPos);
		pNew->Length(nLen);
		pNew->ForeColor(nFore);
		pNew->BackColor(nBack);
		pNew->Separator(FALSE);
		pNew->Group(pszGroup);
		pNew->Enabled(TRUE);
		m_list.SetAtGrow(i,pNew);
		m_nCount++;
		return(i+1);
	}


	CBarItem* CMMStatusBar::FindItem(LPCSTR pszItem)
	{
		CBarItem *pItem;
		for (int i=0;i<m_nCount;i++)
		{
			pItem = (CBarItem *)m_list.GetAt(i);
			if (pItem && pItem->Item() == pszItem)
				return(pItem);
		}
		return(NULL);
	}
	CBarItem* CMMStatusBar::FindItemByPosition(int iPos)
	{
		CBarItem *pItem;
		for (int i=0;i<m_nCount;i++)
		{
			pItem = (CBarItem *)m_list.GetAt(i);
			if (pItem && iPos >= pItem->Position()  &&  iPos < pItem->Position() + pItem->Length())
				return(pItem);
		}
		return(NULL);
	}

	CScriptEntity *CMMStatusBar::AddToList(CScriptEntity * /*pEntity*/)
	{
		ASSERT(FALSE);
		return NULL;
	}

	CScriptEntity *CMMStatusBar::FindExactImpl(const CString& /*strSearch*/) const
	{
		ASSERT(FALSE);
		return NULL;
	}

	HRESULT CMMStatusBar::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CBarItem *pItem = (CBarItem *)GetFirst();
			while(pItem != NULL)
			{
				if( pItem->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					pItem->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd), 
						personName, 
						strText ));

					++nSent;
				}
				pItem = (CBarItem *)GetNext();
			}
			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString("Error in Statusbar SendGroup\n");
		}

		return hr;
	}

}