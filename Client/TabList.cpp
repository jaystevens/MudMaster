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
#include "TabList.h"
#include "Tab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	CTabList::CTabList()
	{
	}

	CTabList::~CTabList()
	{
	}

	CScriptEntity *CTabList::AddToList(CScriptEntity *pEntity)
	{
		CTab *pAdd = dynamic_cast<CTab *>(pEntity);
		CTab *pTab = NULL;

		for (int i = 0;i < m_nCount; ++i)
		{
			pTab = (CTab *)m_list.GetAt(i);

			if(pTab->Text() == pAdd->Text())
			{
				pTab->Group(pAdd->Group());

				delete pAdd;
				return pTab;
			}

			if (pTab->Text() > pAdd->Text())
			{
				AddScriptEntity(pAdd, i);
				return pAdd;
			}
		}

		AddScriptEntity(pAdd, m_nCount);
		return pAdd;
	}

	CTab *CTabList::Add(const CString& strText, const CString& strGroup)
	{
		CTab *pTab;
		for (int i=0;i<m_nCount;i++)
		{
			pTab = (CTab *)m_list.GetAt(i);

			// Can be in the list only once.
			if (pTab->Text() == strText)
			{
				pTab->Group(strGroup);
				return pTab;
			}

			if (pTab->Text() > strText)
			{
				CTab *pNew = new CTab;
				pNew->Text(strText);
				pNew->Group(strGroup);
				AddScriptEntity(pNew, i);
				return pNew;
			}
		}

		CTab *pNew = new CTab;
		pNew->Text(strText);
		pNew->Group(strGroup);
		AddScriptEntity(pNew, m_nCount);
		return pNew;
	}

	BOOL CTabList::RemoveImpl(const CString& strText)
	{
		CTab *pTab;
		for (int i=0;i<m_nCount;i++)
		{
			pTab = (CTab *)m_list.GetAt(i);
			if (pTab->Text() == strText)
			{
				Remove(i);
				return TRUE;
			}
		}
		return FALSE;
	}

	CScriptEntity* CTabList::FindExactImpl(const CString& strText) const
	{
		CTab *pTab;
		for (int i=0;i<m_nCount;i++)
		{
			pTab = (CTab *)m_list.GetAt(i);
			if (pTab != NULL && pTab->Text() == strText)
				return pTab;
		}
		return NULL;
	}

	CTab* CTabList::Lookup(const CString& strText)
	{
		int nLen = strText.GetLength();

		CTab* pTab;

		for (int i=0;i<m_nCount;i++)
		{
			pTab = (CTab *)m_list.GetAt(i);

			if (pTab->Text().GetLength() >= nLen)
				if(memcmp((LPCSTR)strText,(LPCSTR )pTab->Text(), nLen)==0)
					return pTab;
		}
		return NULL;
	}
}