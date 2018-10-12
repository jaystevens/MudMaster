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
#include "UserLists.h"
#include "MMList.h"
#include "ErrorHandling.h"
#include "Globals.h"

#pragma warning(disable:4244)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMScript
{
	CUserLists::CUserLists()
	{
	}

	CUserLists::~CUserLists()
	{
	}

	BOOL CUserLists::AddItem(LPCSTR pszListName, LPCSTR pszItem)
	{
		// Empty strings are not valid.
		if (	pszListName		== NULL	|| 
			*pszListName	== '\0'	|| 
			pszItem			== NULL	|| 
			*pszItem		== '\x0')
			return(FALSE);

		CMMList *pList = FindByName(pszListName);
		if (pList == NULL)
			return(FALSE);

		return pList->Add(pszItem);
	}

	BOOL CUserLists::AddItem(int nListIndex, LPCSTR pszItem)
	{
		if (pszItem == NULL || *pszItem == '\x0')
			return(FALSE);

		if (nListIndex < 0 || nListIndex >= m_nCount)
			return(FALSE);

		CMMList *pList = (CMMList *)m_list.GetAt(nListIndex);
		if (pList == NULL)
			return(FALSE);

		return pList->Add(pszItem);
	}

	CMMList* CUserLists::FindByName(LPCSTR pszListName)
	{
		CMMList *pList;
		for (int i=0;i<m_nCount;i++)
		{
			pList = (CMMList *)m_list.GetAt(i);
			if (pList->ListName() == pszListName)
			{
				m_nGetIndex = i;
				return(pList);
			}
		}
		return(NULL);
	}

	CScriptEntity *CUserLists::AddToList(CScriptEntity *pNewList)
	{
		// TODO Add a list sorting procedure
		CMMList *ptr = dynamic_cast<CMMList *>(pNewList);
		CMMList *pList;
		for (int i = 0; i < m_nCount; ++i)
		{
			pList = (CMMList *)m_list.GetAt(i);

			// Don't add it if the list already exists.
			if (pList->ListName() == ptr->ListName())
				return(FALSE);

			if (pList->ListName() > ptr->ListName())
			{
				AddScriptEntity(ptr, i);
				return ptr;
			}
		}

		AddScriptEntity(ptr, m_nCount);
		return ptr;
	}

	BOOL CUserLists::RemoveImpl(const CString& strSearch)
	{
		CMMList *pList = FindByName(strSearch);
		if (pList == NULL)
			return(FALSE);
		return Remove(m_nGetIndex);
	}


	BOOL CUserLists::RemoveItem(int nListIndex, LPCSTR pszItem)
	{
		if (pszItem == NULL || *pszItem == '\x0')
			return(FALSE);

		if (nListIndex < 0 || nListIndex >= m_nCount)
			return(FALSE);

		CMMList *pList = (CMMList *)m_list.GetAt(nListIndex);
		if (pList == NULL)
			return(FALSE);

		return pList->Remove(pszItem);
	}

	BOOL CUserLists::RemoveItem(LPCSTR pszListName, LPCSTR pszItem)
	{
		// Empty strings are not valid.
		if (pszListName == NULL || *pszListName == '\x0' || pszItem == NULL || *pszItem == '\x0')
			return FALSE;

		CMMList *pList = FindByName(pszListName);
		if (NULL == pList)
			return FALSE;

		return pList->Remove(pszItem);
	}

	BOOL CUserLists::RemoveItem(LPCSTR pszListName, int nItemIndex)
	{
		if (pszListName == NULL || *pszListName == '\x0')
			return FALSE;

		CMMList *pList = FindByName(pszListName);
		if (NULL == pList)
			return FALSE;

		return pList->Remove(nItemIndex);
	}

	BOOL CUserLists::RemoveItem(int nListIndex, int nItemIndex)
	{
		if (nListIndex < 0 || nListIndex >= m_nCount)
			return FALSE;

		CMMList *pList = (CMMList *)m_list.GetAt(nListIndex);
		if (NULL == pList)
			return FALSE;

		return pList->Remove(nItemIndex);
	}

	CScriptEntity *CUserLists::FindExactImpl(const CString& /*strSearch*/) const
	{
		ASSERT(FALSE);
		return NULL;
	}

	HRESULT CUserLists::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CMMList *pList = (CMMList *)GetFirst();
			while(pList != NULL)
			{
				if( pList->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					pList->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						strText));

					int nCount = pList->Items().GetCount();
					for(int i = 0; i < nCount; ++i)
					{
						CString listItem = pList->Items().GetAt(i);
						strText.Format("%citemadd {%s} {%s}",
							CGlobals::GetCommandCharacter(),
							(LPCSTR)pList->ListName(),
							(LPCSTR)listItem);

						ErrorHandlingUtils::TESTHR(server.SendCommand(
							static_cast<BYTE>(cmd),
							personName,
							strText ));
					}
					++nSent;
				}

				pList = (CMMList *)GetNext();
			}		

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString("Error in UserLists SendGroup\n");
		}
		return hr;
	}

}