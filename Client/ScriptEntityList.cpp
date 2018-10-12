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
#include "ScriptEntityList.h"
#include "ScriptEntity.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace MMScript
{
	CScriptEntityList::CScriptEntityList()
	{
		m_list.SetSize(ARRAY_SIZE,GROW_SIZE);
		m_nCount = 0;
		m_nGetIndex = 0;
	}

	CScriptEntityList::~CScriptEntityList()
	{
		RemoveAll();
	}

	CScriptEntity *CScriptEntityList::FindExact(const CString& strSearch) const
	{
		return FindExactImpl(strSearch);
	}

	CScriptEntity *CScriptEntityList::GetAt(int nIndex) const
	{
		if (nIndex < 0 || nIndex >= m_nCount)
			return(NULL);

		m_nGetIndex = nIndex;
		return (CScriptEntity *)m_list.GetAt(nIndex);
	}

	CScriptEntity *CScriptEntityList::GetFirst() const
	{
		if (m_nCount==0)
			return NULL;

		m_nGetIndex = 0;

		return (CScriptEntity *)m_list.GetAt(m_nGetIndex);
	}

	CScriptEntity *CScriptEntityList::GetNext() const
	{
		if (m_nGetIndex + 1 >= m_nCount)
			return NULL;

		m_nGetIndex++;

		return (CScriptEntity *)m_list.GetAt(m_nGetIndex);
	}

	BOOL CScriptEntityList::Remove(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_nCount)
			return FALSE;

		CScriptEntity *ptr = (CScriptEntity *)m_list.GetAt(nIndex);
		if (ptr == NULL)
			return FALSE;

		m_list.RemoveAt(nIndex);
		m_nCount--;

		delete ptr;
		ptr = NULL;

		return(TRUE);
	}

	BOOL CScriptEntityList::Remove(CScriptEntity *pEntity)
	{
		CScriptEntity *ptr;

		for (int i = 0;i < m_nCount;i++)
		{
			ptr = (CScriptEntity *)m_list.GetAt(i);
			if (ptr == pEntity)
			{
				return Remove(i);
			}
		}

		return FALSE;
	}

	BOOL CScriptEntityList::RemoveGroup(const CString& strGroup)
	{
		CScriptEntity *pEntity;
		int nCount = 0;
		for (int i=m_nCount-1;i>-1;i--)
		{
			pEntity = (CScriptEntity *)m_list.GetAt(i);
			if (pEntity->Group() == strGroup)
			{
				Remove(i);
				nCount++;
			}
		}
		return nCount;
	}

	BOOL CScriptEntityList::AddScriptEntity(CScriptEntity *pEntity, int nIndex)
	{
		if(nIndex < m_nCount)
		{
			m_list.InsertAt(nIndex,pEntity);
		}
		else
		{
			m_list.SetAtGrow(nIndex,pEntity);
		}
		m_nCount++;
		return TRUE;
	}

	BOOL CScriptEntityList::Remove(const CString& strSearch)
	{
		return RemoveImpl(strSearch);
	}

	void CScriptEntityList::RemoveAll()
	{
		for(int i = 0; i < m_nCount; i++)
		{
			CScriptEntity *p = (CScriptEntity *)m_list.GetAt(i);
			delete p;
			p = NULL;
		}

		m_list.RemoveAll();
		m_nCount = 0;
		m_nGetIndex = 0;
	}
}