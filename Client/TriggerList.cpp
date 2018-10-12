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
#include "ifx.h"
#include "TriggerList.h"
#include "Trigger.h"
#include "Colors.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;
using namespace Utilities;

namespace MMScript
{
	CTriggerList::CTriggerList()
	{
	}

	CTriggerList::~CTriggerList()
	{
	}

	CTrigger *CTriggerList::CheckTrigger(const CString& strLine, int& nStartIndex)
	{
		CTrigger *pTrigger;
		for (int i=nStartIndex;i<m_nCount;i++)
		{
			pTrigger = (CTrigger *)m_list.GetAt(i);
			if(pTrigger->Match(strLine))
			{
				nStartIndex = i + 1;
				return pTrigger;
			}
		} // for (int i=nStartIndex;i<m_nCount;i++)
		return NULL;
	}

	CScriptEntity *CTriggerList::AddToList(CScriptEntity *trigger)
	{
		CTrigger *pAdd = dynamic_cast<CTrigger *>(trigger);
		if(m_nCount == 0)
		{
			m_list.SetAtGrow(0, trigger);
			m_nCount++;
			return trigger;
		}

		int l = 0;
		int r = m_nCount;
		int i = 0;
		while(r >= l)
		{
			i = (l + r) >> 1;
			if(i == m_nCount)
				break;

			CTrigger *pTrigger = static_cast<CTrigger *>(m_list.GetAt(i));
			if(pAdd->GetTrigger() < pTrigger->GetTrigger())
				r = i - 1;
			else
				l = i + 1;

			if(pAdd->GetTrigger() == pTrigger->GetTrigger())
			{
				delete pTrigger;
				pTrigger = NULL;

				m_list.RemoveAt(i);
				m_list.InsertAt(i, pAdd);

				return pAdd;
			}
		}

		if(i == m_nCount)
			m_list.SetAtGrow(i, pAdd);
		else
			m_list.InsertAt(i, pAdd);

		m_nCount++;
		return trigger;
	}

	int CTriggerList::ToggleGroup(const CString& strGroup, BOOL bEnabled)
	{
		int nCount = 0;
		CTrigger *pTrigger = (CTrigger *)GetFirst();
		while(pTrigger != NULL)
		{
			if (pTrigger->Group() == strGroup)
			{
				pTrigger->Enabled(bEnabled);
				nCount++;
			}
			pTrigger = (CTrigger *)GetNext();
		}
		return nCount;
	}

	int CTriggerList::DisableGroup(const CString& strGroup)
	{
		return ToggleGroup(strGroup, FALSE);
	}

	int CTriggerList::EnableGroup(const CString& strGroup)
	{
		return ToggleGroup(strGroup, TRUE);
	}

	CTrigger *CTriggerList::Find(const CString& strParam) const
	{
		int nIndex = atoi(strParam);
		if (nIndex != 0)
			return (CTrigger *)GetAt(nIndex-1);
		else
			return (CTrigger *)FindExact(strParam);
	}

	CScriptEntity *CTriggerList::FindExactImpl(const CString& strTrigger) const
	{
		CTrigger *pTrigger;
		for (int i=0;i<m_nCount;i++)
		{
			pTrigger = (CTrigger *)m_list.GetAt(i);
			if (strTrigger == pTrigger->GetTrigger())
			{
				m_nGetIndex = i;
				return pTrigger;
			}
		}
		return NULL;
	}

	BOOL CTriggerList::RemoveImpl(const CString &strTrigger)
	{
		CTrigger *pTrigger = (CTrigger *)FindExact(strTrigger);

		if (pTrigger == NULL)
			return FALSE;

		return Remove(m_nGetIndex);
	}

	void CTriggerList::PrintTrigger(CTrigger *pTrigger, CString& strBuffer) const
	{
		strBuffer.Empty();

		CString strTemp;
		pTrigger->MapToText(strTemp, TRUE);

		strBuffer.Format("%s%-3d%s%c%s\n",
			ANSI_F_BOLDWHITE,
			GetFindIndex() + 1,
			ANSI_RESET,
			pTrigger->Enabled() ? ' ' : '*',
			strTemp);
	}

	void CTriggerList::PrintGroup(const CString& strGroup, CString& strBuffer) const
	{
		strBuffer.Empty();

		CString strText;
		CTrigger *pTrigger = (CTrigger *)GetFirst();
		int nCount = 0;
		while(pTrigger != NULL)
		{
			if(pTrigger->Group() == strGroup)
			{
				PrintTrigger(pTrigger, strText);
				strBuffer += strText;
				nCount++;
			}
			pTrigger = (CTrigger *)GetNext();
		}

		strText.Format("%s[%s%s%d%s%s triggers listed.]%s\n\n",
			ANSI_F_BOLDWHITE,
			ANSI_RESET,
			ANSI_F_BOLDRED,
			nCount,
			ANSI_RESET,
			ANSI_F_BOLDWHITE,
			ANSI_RESET);

		strBuffer += strText;
	}

	void CTriggerList::PrintList(CString& strBuffer) const
	{
		strBuffer.Empty();

		CString strText;
		CTrigger *pTrigger = (CTrigger *)GetFirst();
		while(pTrigger != NULL)
		{
			PrintTrigger(pTrigger, strText);
			strBuffer += strText;
			pTrigger = (CTrigger *)GetNext();
		}
	}

	void CTriggerList::PrintTrigger(const CString& strTrigger, CString& strBuffer) const
	{
		strBuffer.Empty();

		CTrigger *pTrigger = (CTrigger *)FindExact(strTrigger);

		if(pTrigger != NULL)
		{
			PrintTrigger(pTrigger, strBuffer);
		}
	}

	void CTriggerList::PrintTrigger(int nIndex, CString& strBuffer) const
	{
		strBuffer.Empty();

		if(nIndex < 0 || nIndex >= m_nCount)
			return;

		CTrigger *pTrigger = (CTrigger *)GetAt(nIndex);

		if(pTrigger == NULL)
			return;

		PrintTrigger(pTrigger, strBuffer);
	}

	HRESULT CTriggerList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			nSent = 0;
			CTrigger *pTrigger = (CTrigger *)GetFirst();
			while(pTrigger != NULL)
			{
				if ( pTrigger->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					pTrigger->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						static_cast<LPCTSTR>(strText)));

					++nSent;
				}

				pTrigger = (CTrigger *)GetNext();
			}	

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString("Error in Triggerlist SendGroup\n");
		}

		return hr;
	}

}