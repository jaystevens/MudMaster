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
#include "Array.h"
#include "ArrayElement.h"
#include "Globals.h"
#include "Colors.h"
#include "Ifx.h"
#include "Sess.h"
#include "ArrayList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace MMScript
{
	CMMArray::CMMArray()
	{
		m_nDim[0] = 0;
		m_nDim[1] = 0;
		m_bSingleDim = TRUE;
	}


	CMMArray::~CMMArray()
	{
		if(!m_bSingleDim)
		{
			for(int i = 0; i < m_nDim[0]; i++)
			{
				CObArray *pArray = (CObArray *)m_Array.GetAt(i);
				for(int j = 0; j < m_nDim[1]; j++)
				{
					CArrayElement *pElement = (CArrayElement *)pArray->GetAt(j);
					delete pElement;
					pElement = NULL;
				}
				delete pArray;
				pArray = NULL;
			}
		}
		else
		{
			for(int i = 0; i < m_nDim[0]; i++)
			{
				CArrayElement *pElement = (CArrayElement *)m_Array.GetAt(i);
				delete pElement;
				pElement = NULL;
			}
		}
	}


	BOOL CMMArray::CreateArray(int nRows, int nCols)
	{
		m_nDim[0] = nRows;
		m_nDim[1] = nCols;
		m_Array.SetSize(nRows);
		if(!m_bSingleDim)
		{
			for(int i = 0; i < nRows; i++)
			{
				CObArray *pArray = new CObArray;
				if(!pArray)
				{
					m_Array.RemoveAll();
					return FALSE;
				}
				pArray->SetSize(nCols);
				for(int j = 0; j < nCols; j++)
				{
					CArrayElement *pElement = new CArrayElement;
					if(!pElement)
						return FALSE;
					pElement->Value("");
					pArray->SetAt(j, pElement);
				}

				m_Array.SetAt(i, pArray);
			}
		}
		else
		{
			for(int j = 0; j < nRows; j++)
			{
				CArrayElement *pElement = new CArrayElement;
				if(!pElement)
					return FALSE;
				pElement->Value("");
				m_Array.SetAt(j, pElement);
			}
		}
		return TRUE;
	}

	BOOL CMMArray::GetValue(int nRow, int nCol, CString& strValue)
	{
		if(m_bSingleDim)
		{
			if(nRow < 1 || nRow > m_nDim[0])
				return FALSE;

			CArrayElement *pElement = (CArrayElement *)m_Array.GetAt(nRow - 1);
			if(pElement)
				strValue = pElement->Value();
			else
				return FALSE;
		}
		else
		{
			if(nRow < 1 || nRow > m_nDim[0])
				return FALSE;

			if(nCol < 1 || nCol > m_nDim[1])
				return FALSE;

			CObArray * pArray = (CObArray *)m_Array.GetAt(nRow - 1);
			if(!pArray)
				return FALSE;

			CArrayElement *pElement = (CArrayElement *)pArray->GetAt(nCol - 1);
			if(pElement)
				strValue = pElement->Value();
			else
				return FALSE;
		}
		return TRUE;
	}

	BOOL CMMArray::SetValue(int nRow, int nCol, const CString& strValue)
	{
		if(m_bSingleDim)
		{
			if(nRow < 1 || nRow > m_nDim[0])
				return FALSE;

			CArrayElement *pTemp = (CArrayElement *)m_Array.GetAt(nRow -1);
			if(pTemp != NULL)
			{
				pTemp->Value(strValue);
			}
			else
			{
				CArrayElement *pElement = new CArrayElement;
				if(!pElement)
					return FALSE;
				pElement->Value(strValue);
				m_Array.SetAt(nRow - 1, pElement);
			}
		}
		else
		{
			if(nRow < 1 || nRow > m_nDim[0])
				return FALSE;

			if(nCol < 1 || nCol > m_nDim[1])
				return FALSE;

			CObArray *pArray = (CObArray *)m_Array.GetAt(nRow - 1);
			if(!pArray)
				return FALSE;

			CArrayElement *pElement = new CArrayElement;
			if(!pElement)
				return FALSE;

			pElement->Value(strValue);

			pArray->SetAt(nCol - 1, pElement);
		}
		return TRUE;
	}


	void CMMArray::MapToCommand(CString &strCommand)
	{
		CString strText;
		MapToText(strText, TRUE);
		strCommand.Format("%carray %s", CGlobals::GetCommandCharacter(), strText);
	}

	void CMMArray::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		if(bIncludeGroup)
		{
			if (SingleDim())
			{
				strText.Format("{%s} {%d} {%s}",
					Name(),
					Dim(0),
					Group());
			}
			else
			{
				strText.Format("{%s} {%d,%d} {%s}",
					Name(),
					Dim(0),
					Dim(1),
					Group());
			}
		}
		else
		{
			if (SingleDim())
			{
				strText.Format("{%s} {%d} {%s}",
					Name(),
					Dim(0));
			}
			else
			{
				strText.Format("{%s} {%d,%d} {%s}",
					Name(),
					Dim(0),
					Dim(1));
			}
		}
	}

	void CMMArray::MapToDisplayText(CString &strText, CSession *pSession)
	{
		CArrayList *pList = pSession->GetArrays();
		if(NULL == pList)
		{
			ATLASSERT(FALSE);
			return;
		}

		if (SingleDim())
		{
			strText.Format("%s%03d:%s {%s} {Rows:%d} {%s}",
				ANSI_F_BOLDWHITE,
				pList->GetFindIndex()+1,
				ANSI_RESET,
				Name(),
				Dim(0),
				Group());
		}
		else
		{
			strText.Format("%s%03d:%s {%s} {Rows:%d} {Cols:%d} {%s}",
				ANSI_F_BOLDWHITE,
				pList->GetFindIndex()+1,
				ANSI_RESET,
				Name(),
				Dim(0),
				Dim(1),
				Group());
		}

	}
}