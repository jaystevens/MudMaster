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
#include "Globals.h"
#include "EventList.h"
#include "Event.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMScript
{
	CEventList::CEventList()
	{
	}

	CEventList::~CEventList()
	{
	}

	int CEventList::DisableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CMMEvent *pEvent = (CMMEvent *)GetFirst();
		while(pEvent != NULL)
		{
			if (pEvent->Group() == pszGroup)
			{
				pEvent->Enabled(FALSE);
				nCount++;
			}
			pEvent = (CMMEvent *)GetNext();
		}
		return(nCount);
	}

	int CEventList::EnableGroup(LPCSTR pszGroup)
	{
		int nCount = 0;
		CMMEvent *pEvent = (CMMEvent *)GetFirst();
		while(pEvent != NULL)
		{
			if (pEvent->Group() == pszGroup)
			{
				pEvent->Enabled(TRUE);
				nCount++;
			}
			pEvent = (CMMEvent *)GetNext();
		}
		return(nCount);
	}

	CScriptEntity *CEventList::AddToList(CScriptEntity *pAdd)
	{
		CMMEvent *ptr = dynamic_cast<CMMEvent *>(pAdd);
		CMMEvent *pEvent;
		for (int i = 0; i < m_nCount; ++i)
		{
			pEvent = (CMMEvent *)m_list.GetAt(i);

			if (pEvent->Name() == ptr->Name())
			{
				*pEvent = *ptr;
				delete ptr;
				ptr = NULL;
				return pEvent;
			}

			if (pEvent->Name() > ptr->Name())
			{
				AddScriptEntity(ptr, i);
				return ptr;
			}
		}

		AddScriptEntity(ptr, m_nCount);
		return ptr;
	}

	CMMEvent *CEventList::Add(const CString& strName, int nSeconds, const CString& strAction, const CString& strGroup)
	{

		CMMEvent *pEvent;
		for (int i = 0; i < m_nCount; ++i)
		{
			pEvent = (CMMEvent *)m_list.GetAt(i);

			// Replace the existing event.
			if (pEvent->Name() == strName)
			{
				pEvent->Seconds(nSeconds);
				pEvent->Started(time(NULL));
				pEvent->Event(strAction);
				pEvent->Group(strGroup);
				return pEvent;
			}

			// Insert a new event.
			if (pEvent->Name() > strName)
			{
				CMMEvent *pNew = new CMMEvent;
				pNew->Seconds(nSeconds);
				pNew->Event(strAction);
				pNew->Started(time(NULL));
				pNew->Name(strName);
				pNew->Group(strGroup);
				pNew->Enabled(TRUE);
				AddScriptEntity(pNew, i);
				return pNew;
			}
		}

		// Add it to the tail.
		CMMEvent *pNew = new CMMEvent;
		pNew->Seconds(nSeconds);
		pNew->Event(strAction);
		pNew->Started(time(NULL));
		pNew->Name(strName);
		pNew->Group(strGroup);
		pNew->Enabled(TRUE);
		AddScriptEntity(pNew, m_nCount);
		return pNew;
	}

	BOOL CEventList::RemoveImpl(const CString& strSearch)
	{
		CMMEvent *pEvent;
		for (int i=0;i<m_nCount;i++)
		{	
			pEvent = (CMMEvent *)m_list.GetAt(i);
			if (pEvent->Name() == strSearch)
			{
				return Remove(i);
			}
		}
		return FALSE;
	}

	CScriptEntity* CEventList::FindExactImpl(const CString& strSearch) const
	{
		CMMEvent *pEvent;
		for(int i = 0; i < m_nCount; i++)
		{
			pEvent = (CMMEvent *)m_list.GetAt(i);
			if(pEvent->Name() == strSearch)
			{
				m_nGetIndex = i;
				return pEvent;
			}
		}
		return NULL;
	}

	HRESULT CEventList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CMMEvent *pEvent = (CMMEvent *)GetFirst();
			while(pEvent != NULL)
			{
				if ( pEvent->Group().Compare( groupName ) )
				{
					CString strText;
					pEvent->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						strText));

					++nSent;
				}

				pEvent = (CMMEvent *)GetNext();
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