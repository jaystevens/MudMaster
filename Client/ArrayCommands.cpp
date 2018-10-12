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
#include "CommandTable.h"
#include "Ifx.h"
#include "Sess.h"
#include "ParseUtils.h"
#include "Array.h"
#include "ObString.h"
#include "Colors.h"
#include "ArrayList.h"
#include "DefaultOptions.h"
#include "StatementParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

BOOL CCommandTable::Array(CSession *pSession, CString &strLine)
{
	CString strName, strDim, strGroup;
	CString strTemp;

	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strDim))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession,strName);
	CParseUtils::ReplaceVariables(pSession,strDim);
	CParseUtils::ReplaceVariables(pSession,strGroup);

	if (strName.IsEmpty())
	{
		// List the arrays.
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Arrays:");
		CString strText;
		CMMArray *ptr = (CMMArray *)pSession->GetArrays()->GetFirst();
		CString strMessage;
		while(ptr != NULL)
		{
			ptr->MapToDisplayText(strText, pSession);
			strMessage += strText;
			strMessage += "\n";
			ptr = (CMMArray *)pSession->GetArrays()->GetNext();
		}
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	if (strDim.IsEmpty())
		return TRUE;

	int nRows = atoi(strDim);
	if (nRows < 1)
		return TRUE;
	int nIndex = strDim.Find(',');
	BOOL bSingleDim;
	int nCols = 0;
	if (nIndex == -1)
		bSingleDim = TRUE;
	else
	{
		bSingleDim = FALSE;
		nCols = atoi(strDim.Right(strDim.GetLength()-nIndex-1));
		if (nCols < 1)
			return TRUE;
	}

	CMMArray *pArray = pSession->GetArrays()->Add(strName,bSingleDim,nRows,nCols,strGroup);
	if (pSession->GetOptions().messageOptions_.ArrayMessages())
		if(pArray != NULL)
		{
			strTemp.Format("Array added.");
			pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, strTemp);
		}
		else
			pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array not added.");

	return TRUE;
}

BOOL CCommandTable::Assign(CSession *pSession, CString &strLine)
{
	CString strName, strIndex, strValue;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strIndex))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strValue))
		return FALSE;

	if (strName.IsEmpty() || strIndex.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession,strName);
	CParseUtils::ReplaceVariables(pSession,strIndex);
	CParseUtils::ReplaceVariables(pSession,strValue);

	int nRow = atoi(strIndex);
	int nCol;
	int nIndex = strIndex.Find(',');
	if (nIndex == -1)
		nCol = 0;
	else
		nCol = atoi(strIndex.Right(strIndex.GetLength()-nIndex-1));

	CMMArray *ptr;
	nIndex = atoi(strName);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strName);

	if (ptr == NULL)
		return TRUE;

	BOOL bResult = pSession->GetArrays()->Assign(ptr,nRow,nCol,strValue);

	if (pSession->GetOptions().messageOptions_.ArrayMessages())
	{
		if (bResult)
		{
			pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Item assigned.");
		}
		else
			pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Item not assigned.");
	}
	return TRUE;
}

BOOL CCommandTable::UnArray(CSession *pSession, CString &strLine)
{
	CString strArray;
	if(!CParseUtils::FindStatement(pSession,strLine,strArray))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strArray);

	if (strArray.IsEmpty())
		return TRUE;

	BOOL bResult;
	int nIndex = atoi(strArray);
	if (nIndex)
		bResult = pSession->GetArrays()->Remove(nIndex-1);
	else
		bResult = pSession->GetArrays()->Remove(strArray);

	if (bResult)
		pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array removed.");
	else
		pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array not removed.");
	return TRUE;
}
BOOL CCommandTable::ArrayCopy(CSession *pSession, CString &strLine)
{
	CString strArray, strCopyArray, strCopyArrayGroup, strValue;
	if(!CParseUtils::FindStatement(pSession,strLine,strArray))
		return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strCopyArray))
		return FALSE;
	CParseUtils::FindStatement(pSession,strLine,strCopyArrayGroup);
		if(strCopyArrayGroup.IsEmpty())
			strCopyArrayGroup = "";

	CParseUtils::ReplaceVariables(pSession, strArray);
	CParseUtils::ReplaceVariables(pSession, strCopyArray);
	CParseUtils::ReplaceVariables(pSession, strCopyArrayGroup);

	if (strArray.IsEmpty() || strCopyArray.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE , "Syntax /CopyArray {ArrayName}{CopyArrayName}{CopyArrayGroup}");
		return TRUE;
	}

	CMMArray *ptr;
	int nIndex = atoi(strArray);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strArray);

	if (ptr == NULL)
		pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array not found.");
	else
	{
		pSession->GetArrays()->Add(strCopyArray,ptr->SingleDim(),ptr->Dim(0),ptr->Dim(1),strCopyArrayGroup);
		CMMArray *pCopy;
		//pCopy->Name(strCopyArray);
		//pCopy->Group(strCopyArrayGroup);
		//pCopy->SingleDim(ptr->SingleDim());
		//pCopy->Dim(0,ptr->Dim(0));
		//pCopy->Dim(1,ptr->Dim(1));
		//pSession->GetArrays()->AddScriptEntity(pCopy,pSession->GetArrays()->GetCount());
		pCopy = (CMMArray *)pSession->GetArrays()->FindExact(strCopyArray);

		int j;
		BOOL bResult;
		for(int i=1; i < ptr->Dim(0) + 1; i++)
		{
			if(ptr->SingleDim())
			{
				ptr->GetValue(i,0,strValue);
				pCopy->SetValue(i,0,strValue);
			}
			else
			{
			for(j=1; j < ptr->Dim(1)+1; j++)
			{
				ptr->GetValue(i,j,strValue);
				bResult = pSession->GetArrays()->Assign(pCopy,i,j,strValue);
			}
			}
		}
		pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array Copied.");

	}
	return TRUE;
}
BOOL CCommandTable::ArrayDBSave(CSession *pSession, CString &strLine)
{
	CString strArray, strValue, strDBData;
	CString strConnectString,strDBUser;
	if(!CParseUtils::FindStatement(pSession,strLine,strArray))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strArray);

	if (strArray.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE , "Syntax /ArrayDBSave {ArrayName}{Database Connect String}{DB User}{DB Password}");
		return TRUE;
	}
	try
	{

// Take the rest of the input params and setup the database connection
			CStatementParser dbParser(pSession);
			ODBC::ODBCConnection DB;
			dbParser.GetDBConnection(strLine,DB);
		CMMArray *ptr;
		int nIndex = atoi(strArray);
		if (nIndex)
			ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
		else
			ptr = (CMMArray *)pSession->GetArrays()->FindExact(strArray);

		if (ptr == NULL)
			pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "Array not found.");
		else
		{
			ODBC::ODBCStmt Stmt(DB);
			if(Stmt == INVALID_HANDLE_VALUE)
			{
				pSession->QueueMessage(CMessages::MM_ARRAY_MESSAGE, "DB connection failed.");
				DB.Disconnect();
				return TRUE;
			}
			CString strQuery;
			strQuery.Format("SELECT ArrayName,SingleDimension,RowNumber,ColumnNumber,ElementText FROM Array where ArrayName like '%s'",ptr->Name());
			int nRet=Stmt.Query(strQuery);
			if(Stmt.FetchFirst())
			{
				strQuery.Format("DELETE  FROM [Array] where ArrayName like '%s'",ptr->Name());
				Stmt.Free();
				nRet=Stmt.Query(strQuery);
				strDBData.Format("Deleted %d existing rows",Stmt.GetChangedRowCount());
				if(pSession->GetOptions().messageOptions_.ShowInfo())
					pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strDBData);

				Stmt.Free();
			}
			else
				Stmt.Free();

			int j=0;
			int i=0;

			//		BOOL bResult;
			for(i=1; i < ptr->Dim(0) + 1; i++)
			{
				if(ptr->SingleDim())
				{
					ptr->GetValue(i,0,strValue);
					if(strValue !="")
					{
						strQuery.Format("INSERT into [Array]  (ArrayName,SingleDimension,RowNumber,ColumnNumber,ElementText) values('%s',TRUE,%d,%d,'%s');",ptr->Name(),i,j,strValue);
						nRet=Stmt.Query(strQuery);
						Stmt.Free();
					}
				}
				else
				{
					for(j=1; j < ptr->Dim(1)+1; j++)
					{
						ptr->GetValue(i,j,strValue);
						if(strValue !="")
						{
							strValue.Replace("'","''");
							strQuery.Format("INSERT into [Array]  (ArrayName,SingleDimension,RowNumber,ColumnNumber,ElementText) values('%s',FALSE,%d,%d,'%s');",ptr->Name(),i,j,strValue);
							nRet=Stmt.Query(strQuery);
							Stmt.Free();
						}
					}
				}
			}
			if(pSession->GetOptions().messageOptions_.ShowInfo())
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Array saved to db.");
		DB.Disconnect();
		}

	}
	catch(...)
	{
		//::OutputDebugString(_T("Error in arraydbsave."));
		strDBData.Empty();
	}
	return TRUE;
}
BOOL CCommandTable::ArrayDBRead(CSession *pSession, CString &strLine)
{
	CString strArray, strValue, strDBData;
	CString strConnectString,strDBUser;
	if(!CParseUtils::FindStatement(pSession,strLine,strArray))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strArray);

	if (strArray.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE , "Syntax /ArrayDBRead {ArrayName}{Database Connect String}{DB User}{DB Password}");
		return TRUE;
	}
	try
	{
// Take the rest of the input params and setup the database connection
		CStatementParser dbParser(pSession);
		ODBC::ODBCConnection DB;
		dbParser.GetDBConnection(strLine,DB);
		int nRet;
		CMMArray *ptr;
		int nIndex = atoi(strArray);
		if (nIndex)
			ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
		else
			ptr = (CMMArray *)pSession->GetArrays()->FindExact(strArray);

		if (ptr == NULL)
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Array not found. Create it with /array");
		else
		{
			ODBC::ODBCStmt Stmt(DB);
			if(Stmt == INVALID_HANDLE_VALUE)
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "DB connection failed.");
				return TRUE;
			}
			CString strQuery;
			strQuery.Format("SELECT RowNumber,ColumnNumber,ElementText FROM Array where ArrayName like '%s'",ptr->Name());
			nRet=Stmt.Query(strQuery);

			TCHAR ColContent[10255];
			//CString strMemo = "";
			//CString& strPointer = strMemo;
			//strPointer = &strMemo;
			long Row,Col;
			Row=0;
			Col=0;
			void* pvData = &Row;
            int iRecords=0,i=0,j=0;
			if(Stmt.FetchFirst())
			{
				ODBC::ODBCRecord rec(Stmt);
				do {
				pvData = &Row;
				rec.GetData(1,pvData,rec.GetColumnSize(1),0,rec.GetColumnType(1));
				i = Row;
				pvData = &Col;
				rec.GetData(2,pvData,rec.GetColumnSize(2),0,rec.GetColumnType(2));
				j = Col;
				//pvData = &strMemo;
				rec.GetData(3,ColContent,rec.GetColumnSize(3),0);
				strValue.Format("%s",ColContent);
				ptr->SetValue(i,j,strValue);
				iRecords++;


				}while(Stmt.FetchNext());

				strDBData.Format("%d array elements read",iRecords);
				if(pSession->GetOptions().messageOptions_.ShowInfo())
					pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strDBData);
				Stmt.Free();
			}
		}
		//DB.Disconnect();
	}
	catch(...)
	{
		//::OutputDebugString(_T("Error caught in ArrayDBRead"));
		strDBData.Empty();
	}
	return TRUE;
}
