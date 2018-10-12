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
#include "ArrayList.h"
#include "Array.h"
#include "ErrorHandling.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMScript
{
	CArrayList::CArrayList()
	{
	}

	CArrayList::~CArrayList()
	{
	}

	CMMArray *CArrayList::Add(const CString& strName, BOOL bSingleDim, int nRows, int nCols, const CString& strGroup)
	{
		CMMArray *ptr;
		for (int i = 0; i < m_nCount; ++i)
		{
			ptr = (CMMArray *)m_list.GetAt(i);

			// Can be in the list only once.
			if (ptr->Name() == strName)
			{
				Remove(i);

				CMMArray *pNew = new CMMArray;
				pNew->Name(strName);
				pNew->Group(strGroup);

				pNew->SingleDim(bSingleDim);
				if(!pNew->CreateArray(nRows, nCols))
				{
					delete pNew;
					pNew = NULL;
					return FALSE;
				}
				AddScriptEntity(pNew, i);
				return pNew;
			}

			if (ptr->Name() > strName)
			{
				CMMArray *pNew = new CMMArray;
				pNew->Name(strName);
				pNew->Group(strGroup);

				pNew->SingleDim(bSingleDim);
				if(!pNew->CreateArray(nRows, nCols))
				{
					delete pNew;
					pNew = NULL;
					return FALSE;
				}

				AddScriptEntity(pNew, i);
				return pNew;
			}
		}

		CMMArray *pNew = new CMMArray;

		pNew->Name(strName);
		pNew->Group(strGroup);
		pNew->SingleDim(bSingleDim);
		if(!pNew->CreateArray(nRows, nCols))
		{
			delete pNew;
			pNew = NULL;
			return FALSE;
		}

		AddScriptEntity(pNew, m_nCount);
		return pNew;
	}


	BOOL CArrayList::RemoveImpl(const CString& strSearch)
	{
		CMMArray *ptr;
		for (int i=0;i<m_nCount;i++)
		{
			ptr = (CMMArray *)m_list.GetAt(i);
			if (ptr->Name() == strSearch)
			{
				return Remove(i);
			}
		}
		return FALSE;
	}

	CScriptEntity* CArrayList::FindExactImpl(const CString& strSearch) const
	{
		if(strSearch.IsEmpty())
			return NULL;

		CMMArray *ptr;
		for (int i=0;i<m_nCount;i++)
		{
			ptr = (CMMArray*)m_list.GetAt(i);
			if (ptr->Name() == strSearch)
				return ptr;
		}
		return NULL;
	}

	BOOL CArrayList::Assign(CMMArray *ptr, int nRow, int nCol, LPCSTR pszValue)
	{
		BOOL bSuccess = ptr->SetValue(nRow, nCol, pszValue);
		if(bSuccess)
			return TRUE;
		else
			return FALSE;
	}

	BOOL CArrayList::GetItem(CMMArray *ptr, int nRow, int nCol, CString &strItem)
	{
		BOOL bSuccess = ptr->GetValue(nRow, nCol, strItem);
		if(!bSuccess)
			return FALSE;
		else
			return(TRUE);
	}

	CScriptEntity *CArrayList::AddToList(CScriptEntity * ptr)
	{
		AddScriptEntity(ptr, m_nCount);

		return ptr;
	}

	HRESULT CArrayList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const CString &groupName, 
		const CString &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CMMArray *ptr = (CMMArray *)GetFirst();
			while(ptr != NULL)
			{
				if( ptr->Group().Compare( groupName ) == 0 )
				{
					CString strText;
					ptr->MapToCommand(strText);

					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						strText ));

					++nSent;
					// Write out the items.
					if (ptr->SingleDim())
					{
						for (int i=1;i<=ptr->Dim(0);i++)
						{
							CString strTemp;
							GetItem(ptr,i,0,strTemp);
							if (!strTemp.IsEmpty())
							{
								strText.Format(_T("%cassign {%s} {%d} {%s}"),
									CGlobals::GetCommandCharacter(),
									(LPCSTR)ptr->Name(),
									i,
									(LPCSTR)strTemp);

								ErrorHandlingUtils::TESTHR(server.SendCommand(
									static_cast<BYTE>(cmd),
									personName,
									strText ));
							}
						}
					}
					else
					{
						int j;
						for (int i=1;i<=ptr->Dim(0);i++)
							for (j=1;j<=ptr->Dim(1);j++)
							{
								CString strTemp;
								GetItem(ptr,i,j,strTemp);
								if (!strTemp.IsEmpty())
								{
									strText.Format(_T("%cassign {%s} {%d,%d} {%s}"),
										CGlobals::GetCommandCharacter(),
										(LPCSTR)ptr->Name(),
										i,j,
										(LPCSTR)strTemp);

									ErrorHandlingUtils::TESTHR(server.SendCommand(
										static_cast<BYTE>(cmd),
										personName,
										strText ));
								}
							}
					}
				}

				ptr = (CMMArray *)GetNext();
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