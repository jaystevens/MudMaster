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
#include "MMList.h"
#include "Globals.h"
#include "Sess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	CMMList::CMMList()
	{
	}

	CMMList::CMMList(const CMMList& src) : 
	m_strListName(src.m_strListName),
		m_strGroup(src.m_strGroup)
	{
		int nCount = src.m_slItems.GetCount();
		for(int i = 0; i < nCount; ++i)
		{
			m_slItems.SetAtGrow(i, src.m_slItems.GetAt(i));
		}
	}
	CMMList::CMMList(const CMMList& src, CString strSort) : 
	m_strListName(src.m_strListName),
		m_strGroup(src.m_strGroup)
	{
		BOOL retVal;
		int nSrcCount = src.m_slItems.GetCount();
		CString srcItem;
		strSort.MakeLower();
		for(int j = 0; j < nSrcCount; ++ j)
		{
			retVal = FALSE;
			srcItem = src.m_slItems.GetAt(j);
			int nCount = Items().GetCount();
			for(int i = 0; i < nCount; ++ i)
			{
				CString strItem = Items().GetAt(i);
				int nComp = strItem.Compare(srcItem);
				if(0 == nComp)
				{
					retVal = TRUE;
					break;
				}
				if(0 < nComp && strSort.Left(1) == "a")
				{
					Items().InsertAt(i, srcItem);
					retVal = TRUE;
					break;
				}
				if(0 > nComp && strSort.Left(1) == "d")
				{
					Items().InsertAt(i, srcItem);
					retVal = TRUE;
					break;
				}

			}

			if (!retVal)
			{
				Items().Add(srcItem);
				retVal = TRUE;
			}
		}
	}

	CMMList::~CMMList()
	{
	}

	void CMMList::MapToCommand(CString &strCommand)
	{
		CString strTemp;
		MapToText(strTemp, TRUE);
		strCommand.Format("%clistadd %s", CGlobals::GetCommandCharacter(), strTemp);
	}

	void CMMList::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		if(bIncludeGroup)
			strText.Format("{%s} {%s}", ListName(), Group());
		else
			strText.Format("{%s}", ListName());
	}

	BOOL CMMList::Add(LPCTSTR pszItem)
	{
		BOOL retVal = FALSE;

		int nCount = Items().GetCount();
		for(int i = 0; i < nCount; ++ i)
		{
			CString strItem = Items().GetAt(i);
			int nComp = strItem.Compare(pszItem);
			if(0 == nComp)
			{
				retVal = TRUE;
				break;
			}
//TODO: KW don't do comparison so it doesn't sort lists
			//if(0 < nComp)
			//{
			//	Items().InsertAt(i, pszItem);
			//	retVal = TRUE;
			//	break;
			//}
		}

		if (!retVal)
		{
			Items().Add(pszItem);
			retVal = TRUE;
		}

		return retVal;
	}

	BOOL CMMList::Remove(LPCTSTR pszItem)
	{
		int nCount = Items().GetCount();
		for(int i = 0; i < nCount; ++i)
		{
			CString strVal = Items().GetAt(i);
			if(!strVal.Compare(pszItem))
			{
				Items().RemoveAt(i);
				break;
			}
		}

		return TRUE;
	}

	BOOL CMMList::Remove(int nIndex)
	{
		BOOL bRet = FALSE;

		if (nIndex >= 0 && nIndex < Items().GetCount())
		{
			Items().RemoveAt(nIndex);
			bRet = TRUE;
		}

		return bRet;
	}
}