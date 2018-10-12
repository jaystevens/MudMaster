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
#include "GroupList.h"
#include "Group.h"
#include "Action.h"
#include "Array.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MMScript;

CGroupList::CGroupList()
{
	m_list.SetSize(DEFAULT_GROUP_ARRAY_SIZE,DEFAULT_GROUP_GROW_SIZE);
	m_nCount = 0;
	m_nGetIndex = 0;
}

CGroupList::~CGroupList()
{
	RemoveAll();
}

void CGroupList::RemoveAll()
{
	for (int i=0;i<m_nCount;i++)
	{
		CGroup *pGroup = (CGroup *)m_list.GetAt(i);
		delete pGroup;
		pGroup = NULL;
	}
	m_list.RemoveAll();
	m_nCount = 0;
	m_nGetIndex = 0;
}

CGroup * CGroupList::GetFirst()
{
	if (m_nCount == 0)
		return NULL;

	m_nGetIndex = 0;

	return (CGroup *)m_list.GetAt(m_nGetIndex);
}

CGroup * CGroupList::GetNext()
{
	if (m_nGetIndex+1 == m_nCount)
		return NULL;

	m_nGetIndex++;

	return (CGroup *)m_list.GetAt(m_nGetIndex);
}

CGroup * CGroupList::GetAt(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return NULL;

	return (CGroup *)m_list.GetAt(nIndex);
}

BOOL CGroupList::Remove(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nCount)
		return FALSE;

	CGroup *pGroup = (CGroup *)m_list.GetAt(nIndex);

	if (pGroup == NULL)
		return FALSE;

	m_list.RemoveAt(nIndex);
	m_nCount--;
	delete pGroup;
	pGroup = NULL;
	return TRUE;
}

BOOL CGroupList::Remove(CGroup *pGroup)
{
	CGroup *ptr = NULL;
	for(int i = 0; i < m_nCount; i++)
	{
		ptr = (CGroup *)m_list.GetAt(i);
		if(ptr == pGroup)
		{
			Remove(i);
			return TRUE;
		}
	}
	return FALSE;
}

CGroup *CGroupList::Add(const CString& strGroup)
{
	CGroup *pGrp = new CGroup(strGroup);

	CGroup *pSrcGrp;

	for (int i = 0; i < m_nCount; ++i)
	{
		pSrcGrp = (CGroup *)m_list.GetAt(i);

		if (pSrcGrp->Name() > pGrp->Name())
		{
			m_list.InsertAt(i,pGrp);
			m_nCount++;
			return pGrp;
		}
	}

	m_list.SetAtGrow(m_nCount, pGrp);

	m_nCount++;

	return pGrp;
}

CGroup * CGroupList::Add(const CString& strGroup, CScriptEntity *pEntity)
{
	CGroup *pGroup = FindByName(strGroup);

	if(pGroup == NULL)
	{
		pGroup = Add(strGroup);
	}

	pGroup->Add(pEntity);
	return pGroup;
}

CGroup * CGroupList::FindByName(const CString& strGroup)
{
	CGroup *pGroup;

	for(int i = 0; i < m_nCount; i++)
	{
		pGroup = (CGroup *)m_list.GetAt(i);
		if(pGroup->Name() == strGroup)
		{
			m_nGetIndex = i;
			return pGroup;
		}
	}

	return NULL;
}
