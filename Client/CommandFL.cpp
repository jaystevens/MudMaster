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
#include "CommandTable.h"
#include "ParseUtils.h"
#include "Event.h"
#include "Gag.h"
#include "BarItem.h"
#include "MMList.h"
#include "Macro.h"
#include "Sub.h"
#include "Tab.h"
#include "Variable.h"
#include "High.h"
#include "Globals.h"
#include "mmsystem.h"
#include "ObString.h"
#include "Sess.h"
#include "Colors.h"
#include "ChatServer.h"
#include "EventList.h"
#include "DllList.h"
#include "GagList.h"
#include "StatusBar.h"
#include "UserLists.h"
#include "SubList.h"
#include "TabList.h"
#include "VarList.h"
#include "HighList.h"
#include "ActionList.h"
#include "AliasList.h"
#include "ArrayList.h"
#include "DebugStack.h"
#include "MacroList.h"
#include "ProcedureTable.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"
#include "StatementParser.h"
#include <sstream>
#include "math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MudmasterColors;
using namespace Utilities;

BOOL CCommandTable::FileCancel(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession, strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().CancelTransfer(strName);

	return TRUE;
}

BOOL CCommandTable::FileDelete(CSession *pSession, CString &strLine)
{
#pragma message ("FileDelete should use the same path rules that /read and /write uses")
	CString strFilename;
	if(!CParseUtils::FindStatement(pSession, strLine,strFilename))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strFilename.IsEmpty())
		return TRUE;

	CFileStatus fs;
	if (CFile::GetStatus(strFilename,fs))
	{
		CString strMessage;
		strMessage.Format("File [%s] deleted.", strFilename);
		CFile::Remove(strFilename);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, FALSE);
	}
	else
	{
		CString strMessage;
		strMessage.Format("File [%s] not found.", strFilename);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, FALSE);
	}
	return TRUE;
}

BOOL CCommandTable::FileStatus(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CString strName;
		if(!CParseUtils::FindStatement(pSession, strLine,strName))
			return FALSE;

		CParseUtils::ReplaceVariables(pSession, strName);

		CString status;
		ErrorHandlingUtils::TESTHR(pSession->GetChat().FileStatus(strName, status));
		pSession->PrintAnsiNoProcess(status);

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, _T("Failed to retrieve file status"));
		hr = e.Error();
	}

	return TRUE;
}

char *CCommandTable::FindCommandName(CSession *pSession, const CString& strCommand)
{
	COMMANDLIST *pCommand = FindCommandCommon(pSession, strCommand);
	if(pCommand != NULL)
		return pCommand->szCommand;
	else
		return NULL;
}

CCommandTable::COMMANDLIST *CCommandTable::FindCommandCommon(CSession *pSession, const CString& strCommand)
{
	ASSERT(NULL != pSession);
	CHashTable::HASHITEM *phi = m_HashTable[strCommand[0]];
	if (!phi || 
		phi->nStart == CHashTable::HASHITEM_EMPTY || 
		phi->nEnd == CHashTable::HASHITEM_EMPTY)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "No command found.");
		CGlobals::PrintDebugText(pSession, "Invalid Command.");
		pSession->DebugStack().DumpStack(
			pSession, 
			pSession->GetOptions().debugOptions_.DebugDepth());
		return NULL;
	}
	
	for (int i=phi->nStart;i<=phi->nEnd && m_List[i].proc;i++)
	{
		if (!m_List[i].bWholeMatch)
		{
			if (CParseUtils::IsPartial(strCommand,m_List[i].szCommand))
			{
				return &m_List[i];
			}
		}
		else
		{
			// Complete text match only.  Used to protect sensitive commands
			// from being accidentally triggered.
			if (!strcmp(strCommand,m_List[i].szCommand))
			{
				return &m_List[i];
			}
		}
	}
	
	return NULL;
}

CCommandTable::COMMANDPROC CCommandTable::FindCommand(CSession *pSession, LPCSTR pszCommand)
{
	ASSERT(NULL != pSession);
	ASSERT(NULL != pszCommand);

	COMMANDLIST *pCommand = FindCommandCommon(pSession, pszCommand);
	if(pCommand != NULL)
		return pCommand->proc;
	else
		return NULL;
}

BOOL CCommandTable::FireEvent(CSession *pSession, CString &strLine)
{
	CString strIndex;
	if(!CParseUtils::FindStatement(pSession, strLine,strIndex))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strIndex);

	if (strIndex.IsEmpty())
		return TRUE;

	CMMEvent *pEvent;
	int nIndex = atoi(strIndex);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strIndex);

	if (pEvent != NULL)
	{
		pEvent->Started(time(NULL));
		CString strTemp(pEvent->Event());
		pSession->DebugStack().Push(pSession->GetEvents()->GetFindIndex(), STACK_TYPE_EVENT, strTemp);

		pSession->ExecuteCommand( strTemp );

		pSession->DebugStack().Pop();
	}
	return TRUE;
}

BOOL CCommandTable::FreeLibrary(CSession *pSession, CString &strLine)
{
	CString lib;
	std::stringstream message;

	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, lib, false ) )
	{
		if( pSession->GetDlls().remove(pSession, lib) )
		{
			message << "FreeLibrary: removed dll named [" << lib << "]";
		}
		else
		{
			message << "FreeLibrary: failed to remove dll named [" << lib << "]";
		}
	}
	else
	{
		message << "FreeLibrary: failed to parse arguments";
	}

	pSession->QueueMessage(CMessages::MM_DLL_MESSAGE, message.str().c_str());
	return TRUE;
}

BOOL CCommandTable::Gag(CSession *pSession, CString &strLine)
{
	CString strMask, strGroup;
	CString strTemp;

	if(!CParseUtils::FindStatement(pSession, strLine,strMask))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strMask);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (!strMask.IsEmpty())
	{
		CTrigger *pTrigger = pSession->GetGags()->Add(strMask, "", strGroup);
		if (pTrigger != NULL)
		{
			strTemp.Format("Gag added.");
			pSession->QueueMessage(CMessages::MM_GAG_MESSAGE, strTemp);
		}
		else
			pSession->QueueMessage(CMessages::MM_GAG_MESSAGE, "Gag not added.");
		return TRUE;
	}

	pSession->QueueMessage(CMessages::MM_GAG_MESSAGE, "Defined Gags:");

	CString strMessage;
	pSession->GetGags()->PrintList(strMessage);
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::Grep(CSession *pSession, CString &strLine)
{
#pragma message("Grep should use the regular expression stuff like /scrollgrep")
	CString strSearch;
	if(!CParseUtils::FindStatement(pSession, strLine, strSearch))
		return FALSE;


	if(strSearch.IsEmpty())
	{
		CString strMessage = "No search text provided.";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	CString strText;
	strText.Format("%sSearching for [%s%s%s]...%s\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		strSearch,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Actions..." ANSI_RESET);
	GrepActions(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Aliases..." ANSI_RESET);
	GrepAliases(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Variables..." ANSI_RESET);
	GrepVariables(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Arrays..." ANSI_RESET);
	GrepArrays(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Lists..." ANSI_RESET);
	GrepLists(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Highlights..." ANSI_RESET);
	GrepHighs(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Macros..." ANSI_RESET);
	GrepMacros(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Subsitutions..." ANSI_RESET);
	GrepSubs(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Events..." ANSI_RESET);
	GrepEvents(pSession, strSearch);
	pSession->PrintAnsiNoProcess(ANSI_RESET ANSI_F_BOLDWHITE "Gags..." ANSI_RESET);
	GrepGags(pSession, strSearch);
	return TRUE;
}

BOOL CCommandTable::GroupBarItems(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined Bar Items(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	int nCount = 0;
	CBarItem *pItem = (CBarItem *)pSession->GetStatusBar()->GetFirst();
	CString strMessage;
	while(pItem != NULL)
	{
		if (pItem->Group() == strGroup)
		{
			nCount++;
			pItem->MapToDisplayText(strText, pSession);
			strMessage += strText + "\n";
		}
		pItem = (CBarItem *)pSession->GetStatusBar()->GetNext();
	}
	strText.Format("%s[%s%d%s bar items listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GroupChat(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strGroup);
	CParseUtils::ReplaceVariables(pSession, strLine);
	if(strLine.IsEmpty())
		return TRUE;

	HRESULT hr = pSession->GetChat().SendChatToGroup(strGroup,strLine);
	return SUCCEEDED(hr);
}

BOOL CCommandTable::GroupEmote(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strGroup);
	CParseUtils::ReplaceVariables(pSession, strLine);

	if (strLine.IsEmpty())
		return TRUE;

	pSession->GetChat().SendEmoteToGroup(strGroup,strLine);
	return TRUE;
}

BOOL CCommandTable::GroupEvents(CSession *pSession, CString &strLine) 
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined Events(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	int nCount = 0;
	CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
	CString strMessage;
	while(pEvent != NULL)
	{
		if (pEvent->Group() == strGroup)
		{
			nCount++;
			CString strTemp;
			pEvent->MapToDisplayText(strTemp);
			strText.Format("%s%03d:%s%c%s",
				ANSI_F_BOLDWHITE,
				pSession->GetEvents()->GetFindIndex()+1,
				ANSI_RESET,
				pEvent->Enabled() ? ' ' : '*',
				strTemp);
			strMessage += strText;
		}
		pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
	}
	strText.Format("%s[%s%s%d%s%s events listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GroupLists(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined Lists(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	int nCount = 0;
	CMMList *pList = (CMMList *)pSession->GetLists()->GetFirst();
	CString strMessage;
	while(pList != NULL)
	{
		if (pList->Group() == strGroup)
		{
			nCount++;
			strText.Format("%s%03d:%s %s(%d)\n",
				ANSI_F_BOLDWHITE,
				pSession->GetLists()->GetFindIndex()+1,
				ANSI_RESET,
				(LPCSTR)pList->ListName(),
				pList->Items().GetCount());
			strMessage += strText;
		}
		pList = (CMMList *)pSession->GetLists()->GetNext();
	}
	strText.Format("%s[%s%s%d%s%s lists listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GroupMacros(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined Macros(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	CString strKeyName;
	int nCount = 0;
	CMacro *pMac = (CMacro *)pSession->GetMacros()->GetFirst();
	CString strMessage;
	while(pMac != NULL)
	{
		if (pMac->Group() == strGroup)
		{
			nCount++;
			pSession->GetMacros()->GetVirtualKeyName(pMac->VirtualKeyCode(), strKeyName);

			strText.Format(_T("%c{%s%s%s} {%s}\n"),
				(pMac->Enabled() ? ' ' : '*'),
				ANSI_F_BOLDWHITE,
				(LPCSTR)strKeyName,
				ANSI_RESET,
				(LPCSTR)pMac->Action());
			strMessage += strText;
		}
		pMac = (CMacro *)pSession->GetMacros()->GetNext();
	}
	strText.Format("%s[%s%s%d%s%s macros listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GroupSubs(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText,strTemp;
	strText.Format("Defined Subs(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
	pSession->GetSubs()->PrintGroup(strGroup, strText);
	pSession->PrintAnsiNoProcess(strText);
	return TRUE;
}

BOOL CCommandTable::GroupTablist(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined Tab Words(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	int nCount = 0;
	CTab *pTab = (CTab *)pSession->GetTabList()->GetFirst();
	CString strMessage;
	while(pTab != NULL)
	{
		if (pTab->Group() == strGroup)
		{
			pTab->MapToText(strText, TRUE);
			strMessage += strText;
			strMessage += "\n";
			nCount++;
		}
		pTab = (CTab *)pSession->GetTabList()->GetNext();
	}
	strText.Format("%s[%s%s%d%s%s tab words listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GroupVariables(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText,strTemp;
	strText.Format("Defined Variables(%s):",(LPCSTR)strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	CString strMessage;
	int nCount = 0;
	for(int i = 0; i < pSession->GetVariables()->GetSize(); ++i)
	{
		CVariable &v = pSession->GetVariables()->GetAt(i);
		if( v.GetGroup() == static_cast<LPCTSTR>( strGroup ) )
		{
			v.MapToText(strTemp, TRUE);
			strMessage += strTemp;
			strMessage += _T("\n");
			nCount++;
		}
	}

	strText.Format("%s[%s%s%d%s%s variables listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);
	strMessage += strText;
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::Hash()
{
	UINT nCurEntry = 0;		// Current hash table entry.
	UINT nEntry;			// Entry of the item being checked.
	int i;
	int nStart;
	int nEnd;

 	m_HashTable.Clear();

	nStart = CHashTable::HASHITEM_EMPTY;
	nEnd = CHashTable::HASHITEM_EMPTY;
	for (i=0;m_List[i].proc;i++)
	{
		nEntry = m_List[i].szCommand[0];

		// Will only be this value first time thru.
		if (!i)
		{
			nCurEntry = nEntry;
			nStart = i;
		}

		// If the entry we just found is not equal the to the entry we are working
		// with, need to finish off the last entry and start a new one.
		if (nEntry != nCurEntry)
		{
			m_HashTable.SetAt(nCurEntry,nStart,nEnd);
			nStart = i;
			nCurEntry = nEntry;
		}

		nEnd = i;
	}

	// Last time thru loop need to add last hash entry.
	m_HashTable.SetAt(nCurEntry,nStart,nEnd);
	return TRUE;
}

BOOL CCommandTable::ShowHelpFile(CSession *pSession, const CString& strHelpFileName)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		std::string path;
		ErrorHandlingUtils::TESTHR(CGlobals::GetModuleDirectory(path));

		std::stringstream buffer;

		buffer 
			<< "{" 
			<< path 
			<< "Help\\" 
			<< static_cast<LPCTSTR>(strHelpFileName) 
			<< ".hlp" 
			<< "}";

		CString temp(buffer.str().c_str());
		Spam(pSession, temp);

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}


	return TRUE;
}
BOOL CCommandTable::HandledByDll(CSession *pSession, CString &strLine)
{
// must be exactly "TRUE" otherwise set to false
	CString strHandled;
	CParseUtils::FindStatement(pSession, strLine,strHandled);
	try
	{
		pSession->SetHandledByDll(strHandled == "TRUE");

	}
	catch(...)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error in /HandledByDll");
	}


	return TRUE;
}
BOOL CCommandTable::Help(CSession *pSession, CString& strLine)
{
	if(strLine.IsEmpty())
	{
		ShowHelpFile(pSession, "topics");
		return TRUE;
	}
	//make lower case to eliminate case sensitivity
	strLine = strLine.MakeLower();
	if(strLine[0] == CGlobals::GetProcedureCharacter())
	{
		if(strLine.GetLength() < 2)
		{
			ShowHelpFile(pSession, "procedures");
			return TRUE;
		}
		//find a procedure name and show the help file for it
		const char *strProcName = CProcedureTable::FindProcedureName(pSession, strLine.Mid(1));
		if(strProcName == NULL)
		{
			if(!strLine.Mid(1).Compare(_T("commandtolist")))
			{
				strProcName = _T("CommandToList");
			}
			else
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid procedure name.");
				return TRUE;
			}
		}

		CString strFileName = "p_";
		strFileName += strProcName;
		ShowHelpFile(pSession, strFileName);
		return TRUE;
	}

	if(strLine[0] == CGlobals::GetCommandCharacter())
	{
		if(strLine.GetLength() < 2)
		{
			ShowHelpFile(pSession, "commands");
			return TRUE;
		}
		//find a command name and show the help file for it
		const char *strCommandName = FindCommandName(pSession, strLine.Mid(1));
		if(strCommandName == NULL)
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid command name.");
			return TRUE;
		}

		ShowHelpFile(pSession, strCommandName);
		return TRUE;
	}

	// if it wasn't a command or a procedure...we'll just try to load
	// and show what ever file it was they specified.
	ShowHelpFile(pSession, strLine);
	return TRUE;
}

BOOL CCommandTable::Highlight(CSession *pSession, CString &strLine)
{
	CString strMask, strColor, strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strMask))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strColor))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strMask);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (!strMask.IsEmpty() && !strColor.IsEmpty())
	{
		CHigh *pHigh = pSession->GetHighlights()->Add(strMask,strColor,strGroup);
		if (pHigh >= 0)
		{
			CString strMessage;
			strMessage.Format("Highlight added.");
			pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, strMessage);
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, "Highlight not added.");
		}
		return TRUE;
	}

	if(!strMask.IsEmpty() && strColor.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Ok, but what would you like the colors to be??");
		return TRUE;
	}

	pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, "Defined Highlights.");

	CString strTemp, strText;
	CHigh *pHigh = (CHigh *)pSession->GetHighlights()->GetFirst();
	CString strMessage;
	while(pHigh != NULL)
	{
		pHigh->MapToText(strTemp, TRUE);
		strText.Format("%s%03d:%s%c%s\n",
			ANSI_F_BOLDWHITE,
			pSession->GetHighlights()->GetFindIndex()+1,
			ANSI_RESET,
			(pHigh->Enabled() ? ' ' : '*'),
			strTemp);
		strMessage += strText;
		pHigh = (CHigh *)pSession->GetHighlights()->GetNext();
	}
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::If(CSession *pSession, CString &strLine)
{
	CString strCondition, strParam, strElse;
	if(!CParseUtils::FindStatement(pSession, strLine,strCondition))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strElse))
		return FALSE;


	strElse.TrimLeft();
	strElse.TrimRight();

	if (strCondition.IsEmpty() || strParam.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strCondition,TRUE);

	if ( CParseUtils::EvaluateLine( pSession, strCondition ) )
	{
		pSession->ExecuteCommand( strParam );
	}
	else
	{
		if (!strElse.IsEmpty())
		{
			pSession->ExecuteCommand( strElse );
		}
	}
	return TRUE;
}

BOOL CCommandTable::Ignore(CSession *pSession, CString &strLine)
{
	CString strActionType;
	if(!CParseUtils::FindStatement(pSession, strLine,strActionType))
		return FALSE;

	CString strMessage;
	strActionType.MakeLower();

	if(!strActionType.Compare("all"))
		strActionType = "";

	if(strActionType.IsEmpty())
	{
		pSession->GetOptions().triggerOptions_.IgnoreTriggers(!pSession->GetOptions().triggerOptions_.IgnoreTriggers());
		pSession->GetOptions().triggerOptions_.IgnoreActions(pSession->GetOptions().triggerOptions_.IgnoreTriggers());
		pSession->GetOptions().triggerOptions_.IgnoreGags(pSession->GetOptions().triggerOptions_.IgnoreTriggers());
		pSession->GetOptions().triggerOptions_.IgnoreHighs(pSession->GetOptions().triggerOptions_.IgnoreTriggers());
		pSession->GetOptions().triggerOptions_.IgnoreSubs(pSession->GetOptions().triggerOptions_.IgnoreTriggers());

		if(pSession->GetOptions().triggerOptions_.IgnoreTriggers())
			strMessage = "You will now ignore all actions, gags, subs and highlights";
		else
			strMessage = "You will now process all enabled actions, gags, subs and highlights";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		return TRUE;
	}

	strActionType.MakeLower();
	if(CParseUtils::IsPartial("substitutions", strActionType))
	{
		pSession->GetOptions().triggerOptions_.IgnoreSubs(!pSession->GetOptions().triggerOptions_.IgnoreSubs());
		if(pSession->GetOptions().triggerOptions_.IgnoreSubs())
			strMessage = "You will now ignore all substitutions";
		else
			strMessage = "You will now process all enabled substitutions";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		return TRUE;
	}

	if(CParseUtils::IsPartial("highlights", strActionType))
	{
		pSession->GetOptions().triggerOptions_.IgnoreHighs(!pSession->GetOptions().triggerOptions_.IgnoreHighs());
		if(pSession->GetOptions().triggerOptions_.IgnoreHighs())
			strMessage = "You will now ignore all highlights";
		else
			strMessage = "You will now process all enabled highlights";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		return TRUE;
	}

	if(CParseUtils::IsPartial("gags", strActionType))
	{
		pSession->GetOptions().triggerOptions_.IgnoreGags(!pSession->GetOptions().triggerOptions_.IgnoreGags());
		if(pSession->GetOptions().triggerOptions_.IgnoreGags())
			strMessage = "You will now ignore all gags";
		else
			strMessage = "You will now process all enabled gags";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		return TRUE;
	}

	if(CParseUtils::IsPartial("actions", strActionType))
	{
		pSession->GetOptions().triggerOptions_.IgnoreActions(!pSession->GetOptions().triggerOptions_.IgnoreActions());
		if(pSession->GetOptions().triggerOptions_.IgnoreActions())
			strMessage = "You will now ignore all actions";
		else
			strMessage = "You will now process all enabled actions";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		return TRUE;
	}

	strMessage="Invalid parameter passed to /ignore.  Valid arguments are [all, actions, gags, highlights, substitutions, or no parameter at all]";
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
	return TRUE;
}

BOOL CCommandTable::IgnoreAliases(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine, strParam))
		return FALSE;


	if(strParam.IsEmpty())
	{
		pSession->GetOptions().sessionInfo_.IgnoreAliases(!pSession->GetOptions().sessionInfo_.IgnoreAliases());

		CString strMessage;
		strMessage.Format("Aliases will now be %s.", pSession->GetOptions().sessionInfo_.IgnoreAliases() ? "ignored" : "processed");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().sessionInfo_.IgnoreAliases(TRUE);
		CString strMessage;
		strMessage.Format("Aliases will now be %s.", pSession->GetOptions().sessionInfo_.IgnoreAliases() ? "ignored" : "processed");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().sessionInfo_.IgnoreAliases(FALSE);
		CString strMessage;
		strMessage.Format("Aliases will now be %s.", pSession->GetOptions().sessionInfo_.IgnoreAliases() ? "ignored" : "processed");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /ignorealiases.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	return TRUE;
}

BOOL CCommandTable::Information(CSession *pSession, CString &strLine)
{
	CString strCommand;
	if(!CParseUtils::FindStatement(pSession, strLine, strCommand))
		return FALSE;

	strCommand.MakeLower();
	strCommand.TrimRight();

	CString strText;

	if(strCommand.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Program Information:");

		CString strMessage;
		strText.Format("    Host Name: %s\n",(LPCSTR)CGlobals::GetHostName());
		strMessage += strText;
		strText.Format("   IP Address: %s\n",(LPCSTR)CGlobals::GetIPAddress());
		strMessage += strText;

		strText.Format("  Listen Port: %d\n",pSession->GetOptions().chatOptions_.ListenPort());
		strMessage += strText;

		strText.Format("    Chat Name: %s\n",(LPCSTR)pSession->GetOptions().chatOptions_.ChatName());
		strMessage += strText;

		strText.Format(" Active Chats: %d\n",pSession->GetChat().GetCount());
		strMessage += strText;

		strText.Format("  Auto-Accept: %s\n",
			(pSession->GetOptions().chatOptions_.AutoAccept() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("          DnD: %s\n",
			(pSession->GetOptions().chatOptions_.DoNotDisturb() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("       Ignore: %s\n",
			(pSession->GetOptions().triggerOptions_.IgnoreTriggers() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("IgnoreActions: %s\n",
			(pSession->GetOptions().triggerOptions_.IgnoreActions() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("   IgnoreGags: %s\n",
			(pSession->GetOptions().triggerOptions_.IgnoreGags() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("  IgnoreHighs: %s\n",
			(pSession->GetOptions().triggerOptions_.IgnoreHighs() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("   IgnoreSubs: %s\n",
			(pSession->GetOptions().triggerOptions_.IgnoreSubs() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("   Status Bar: %s\n\n",
			(pSession->GetOptions().sessionInfo_.ShowStatusBar() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("      Actions: %d\n",pSession->GetActions()->GetCount());
		strMessage += strText;

		strText.Format("      Aliases: %d\n",pSession->GetAliases()->GetCount());
		strMessage += strText;

		strText.Format("       Arrays: %d\n",pSession->GetArrays()->GetCount());
		strMessage += strText;

		strText.Format("    Bar Items: %d\n",pSession->GetStatusBar()->GetCount());
		strMessage += strText;

		strText.Format("         DLLS: %d\n", pSession->GetDlls().count());
		strMessage += strText;

		strText.Format("       Events: %d\n",pSession->GetEvents()->GetCount());
		strMessage += strText;

		strText.Format("         Gags: %d\n",pSession->GetGags()->GetCount());
		strMessage += strText;

		strText.Format("   Highlights: %d\n",pSession->GetHighlights()->GetCount());
		strMessage += strText;

		strText.Format("        Lists: %d\n",pSession->GetLists()->GetCount());
		strMessage += strText;

		strText.Format("       Macros: %d\n",pSession->GetMacros()->GetCount());
		strMessage += strText;

		strText.Format("  Substitutes: %d\n", pSession->GetSubs()->GetCount());
		strMessage += strText;

		strText.Format("     Tab List: %d\n",pSession->GetTabList()->GetCount());
		strMessage += strText;

		strText.Format("    Variables: %d\n",pSession->GetVariables()->GetCount());
		strMessage += strText;
		strMessage += "\n";
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strCommand,"paths"))
	{
		CString strMessage;
		strText.Format("\n#%s%s%s Paths%s:\n\n",
			ANSI_RESET,
			ANSI_BOLD,
			ANSI_F_WHITE,
			ANSI_RESET);
		strMessage += strText;
		
		SerializedOptions::CPathOptions &pathOptions = pSession->GetOptions().pathOptions_;

		std::string path;
		pathOptions.Get_DownloadPath(path);

		strText.Format(_T("   Downloads: %s\n"), path.c_str());
		strMessage += strText;

		pathOptions.Get_UploadPath(path);
		strText.Format(_T("     Uploads: %s\n"), path.c_str());
		strMessage += strText;

		if(FAILED(pathOptions.Get_SoundPath(path)))
		{
			ATLASSERT(FALSE);
		}
		strText.Format(_T("      Sounds: %s\n"), path.c_str());
		strMessage += strText;

		strText.Format(_T("    Log Path: %s\n"),
			pathOptions.LogPath());
		strMessage += strText;

		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strCommand,"chat"))
	{
		CString strMessage;
		strText.Format("\n#%s%s%s Chat%s:\n\n",
			ANSI_RESET,
			ANSI_BOLD,
			ANSI_F_WHITE,
			ANSI_RESET);
		strMessage += strText;

		strText.Format("   Host Name: %s\n",(LPCSTR)CGlobals::GetHostName());
		strMessage += strText;

		strText.Format("  IP Address: %s\n",
			(LPCSTR)CGlobals::GetIPAddress());
		strMessage += strText;

		strText.Format(" Listen Port: %d\n",pSession->GetOptions().chatOptions_.ListenPort());
		strMessage += strText;

		strText.Format("   Chat Name: %s\n",(LPCSTR)pSession->GetOptions().chatOptions_.ChatName());
		strMessage += strText;

		strText.Format("Active Chats: %d\n",pSession->GetChat().GetCount());
		strMessage += strText;

		strText.Format(" Auto-Accept: %s\n",
			(pSession->GetOptions().chatOptions_.AutoAccept() ? "On" : "Off"));
		strMessage += strText;

		strText.Format("         DnD: %s\n\n",
			(pSession->GetOptions().chatOptions_.DoNotDisturb() ? "On" : "Off"));
		strMessage += strText;
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	return TRUE;
}
BOOL CCommandTable::Intersection(CSession *pSession, CString &strLine)
{
	//Need to receive X and Y for center and radius of 1st circle
    //                X and Y for center and radius of 2nd circle
	//				  and the name of return variable
	CString strXOne, strYOne, strRadiusOne,strXTwo, strYTwo, strRadiusTwo, strVar;
	// X one
	if(!CParseUtils::FindStatement(pSession, strLine,strXOne))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strXOne);
	// Y one
	if(!CParseUtils::FindStatement(pSession, strLine,strYOne))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strYOne);
	// Radius one
	if(!CParseUtils::FindStatement(pSession, strLine,strRadiusOne))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strRadiusOne);
	// X two
	if(!CParseUtils::FindStatement(pSession, strLine,strXTwo))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strXTwo);
	// Y two
	if(!CParseUtils::FindStatement(pSession, strLine,strYTwo))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strYTwo);
	// Radius two
	if(!CParseUtils::FindStatement(pSession, strLine,strRadiusTwo))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strRadiusTwo);

	if(!CParseUtils::FindStatement(pSession, strLine,strVar))
		return FALSE;

	if (strVar.IsEmpty())
		return TRUE;

	
	double deltaX, deltaY, distanceOneTwo, distanceOneP2, XP2, YP2, distanceIntersectP2;
	double XOne, YOne, XTwo, YTwo, RadiusOne, RadiusTwo, RX, RY;
	double interXOne, interYOne, interXTwo, interYTwo;
	XOne = atof(strXOne);
	YOne = atof(strYOne);
	XTwo = atof(strXTwo);
	YTwo = atof(strYTwo);
	RadiusOne = atof(strRadiusOne);
	RadiusTwo = atof(strRadiusTwo);
	// X and Y distances between the centers
	deltaX = XTwo - XOne;
	deltaY = YTwo - YOne;
	double sumSquares = (deltaY * deltaY) + (deltaX * deltaX);
	distanceOneTwo = sqrt(sumSquares);

	// circles don't intersect
	if(distanceOneTwo > (RadiusOne + RadiusTwo))
		return TRUE;
	// circle inside  other
	double Diff = (RadiusOne - RadiusTwo);
	if(distanceOneTwo < abs(Diff))
		return TRUE;
  /* 'point 2' is the point where the line through the circle
   * intersection points crosses the line between the circle
   * centers.  
   */
	distanceOneP2 = ((RadiusOne * RadiusOne) - (RadiusTwo * RadiusTwo) + (distanceOneTwo * distanceOneTwo)) / (2.0 * distanceOneTwo);
	XP2 =  XOne + (deltaX * distanceOneP2 / distanceOneTwo);
	YP2 =  YOne + (deltaY * distanceOneP2 / distanceOneTwo);

	distanceIntersectP2 = sqrt ((RadiusOne * RadiusOne) - (distanceOneP2 * distanceOneP2));
	RX = - deltaY * (distanceIntersectP2 / distanceOneTwo);
	RY =  deltaX * (distanceIntersectP2 / distanceOneTwo);
	interXOne = XP2 + RX;
    interYOne = YP2 + RY;
	interXTwo = XP2 - RX;
	interYTwo = YP2 - RY;

	CString strValue;
	strValue.Format(_T("(%4.0f,%4.0f) (%4.0f,%4.0f)"),interXOne,interYOne,interXTwo,interYTwo);

	std::string name = static_cast<LPCTSTR>( strVar );
	std::string value = static_cast<LPCTSTR>( strValue );

	pSession->GetVariables()->Add( name, value, "" );
	return TRUE;
}

BOOL CCommandTable::ItemAdd(CSession *pSession, CString &strLine)
{
	CString strListName, strItem;
	if(!CParseUtils::FindStatement(pSession, strLine,strListName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);

	if(!CParseUtils::FindStatement(pSession, strLine,strItem))
		return FALSE;

	if (strListName.IsEmpty() || strItem.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strItem);
	BOOL bResult;
	int nIndex = atoi(strListName);
	if (!nIndex)
		bResult = pSession->GetLists()->AddItem(strListName,strItem);
	else
		bResult = pSession->GetLists()->AddItem(nIndex-1,strItem);

	pSession->QueueMessage(CMessages::MM_ITEM_MESSAGE, "Item added.");
	return TRUE;
}

BOOL CCommandTable::ItemDelete(CSession *pSession, CString &strLine)
{
	CString strListName, strItem;
	if(!CParseUtils::FindStatement(pSession, strLine,strListName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);

	if(!CParseUtils::FindStatement(pSession, strLine,strItem))
		return FALSE;


	if (strListName.IsEmpty() || strItem.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strItem);
	int nListIndex = atoi(strListName);
	int nItemIndex = atoi(strItem);

	if (!nListIndex)
		if(nItemIndex > 0)
			pSession->GetLists()->RemoveItem(strListName, nItemIndex-1);
		else
			pSession->GetLists()->RemoveItem(strListName, strItem);
	else
		if(nItemIndex > 0)
			pSession->GetLists()->RemoveItem(nListIndex-1, nItemIndex-1);
		else
			pSession->GetLists()->RemoveItem(nListIndex-1, strItem);

	pSession->QueueMessage(CMessages::MM_ITEM_MESSAGE, "Item deleted.");
	return TRUE;
}

BOOL CCommandTable::KillAll(CSession *pSession, CString &strLine)
{
	strLine.Empty();
	pSession->KillAll();
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Cleared..");
	return TRUE;
}

BOOL CCommandTable::KillGroup(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement( pSession, strLine, strGroup ) )
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	std::string group = static_cast<LPCTSTR>( strGroup );
	try
	{
		int nActions = pSession->GetActions()->RemoveGroup(strGroup);
		int nAliases = pSession->GetAliases()->RemoveGroup(strGroup);
		int nArrays = pSession->GetArrays()->RemoveGroup(strGroup);
		int nEvents = pSession->GetEvents()->RemoveGroup(strGroup);
		int nLists = pSession->GetLists()->RemoveGroup(strGroup);
		int nMacros = pSession->GetMacros()->RemoveGroup(strGroup);
//Must update macros since the accelerator table must be updated to match
		pSession->UpdateMacros();	

		int nTabLists = pSession->GetTabList()->RemoveGroup(strGroup);
		int nVariables = pSession->GetVariables()->RemoveGroup( group );
		int nBarItems = pSession->GetStatusBar()->RemoveGroup(strGroup);
		int nGags = pSession->GetGags()->RemoveGroup(strGroup);
		int nHighs = pSession->GetHighlights()->RemoveGroup(strGroup);
		int nSubs = pSession->GetSubs()->RemoveGroup(strGroup);

		if (nBarItems && pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();

		CString strText;
		strText.Format(	_T("Memory cleared: Actions(%d), Aliases(%d), Arrays(%d), Bar Items(%d)\n")
			_T("                  Events(%d), Gags(%d), Highlights(%d), Lists(%d)\n")
			_T("                  Macros(%d), Tab Words(%d), Variables(%d), Subs(%d)"),
			nActions,nAliases,nArrays,nBarItems,nEvents,nGags,nHighs,
			nLists,nMacros,nTabLists,nVariables, nSubs);

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText, TRUE);
		//pSession->PrintAnsiNoProcess(strText);
		return TRUE;
	}
	catch(...)
	{
		::OutputDebugString(_T("Error executing KillGroup RemoveGroup section\n"));
		return FALSE;
	}

}

BOOL CCommandTable::ListAdd(CSession *pSession, CString &strLine)
{
	CString strListName, strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strListName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (strListName.IsEmpty())
		return TRUE;

	// Make sure it doesn't already exist.
	CMMList *pList = pSession->GetLists()->FindByName(strListName);
	if (pList != NULL)
	{
		pList->Group(strGroup);
		return TRUE;
	}

	pList = new CMMList;
	pList->ListName(strListName);
	pList->Group(strGroup);
	pSession->GetLists()->AddToList(pList);

	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List added.");
	return TRUE;
}
BOOL CCommandTable::ListCopy(CSession *pSession, CString &strLine)
{
	CString strOriginalListName, strNewListName, strSort;
	if(!CParseUtils::FindStatement(pSession, strLine,strOriginalListName))
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "listcopy Usage: /listcopy {original name}{new name (optional)}{a or d (sort optional)}");
		return FALSE;
	}

	if(!CParseUtils::FindStatement(pSession, strLine,strNewListName))
	{
		return FALSE;
	}
	if(!CParseUtils::FindStatement(pSession, strLine,strSort))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strOriginalListName);
	CParseUtils::ReplaceVariables(pSession, strNewListName);
	//if no new name is passed but sort indicator is
	if(strSort.IsEmpty() && strNewListName.GetLength() == 1)
	{
		strSort = strNewListName.MakeLower();
		strNewListName.Empty();
	}
	if (strOriginalListName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Usage: /listcopy {original name}{new name (optional)}{a or d (sort optional)}");
		return TRUE;
	}
	if (strNewListName.IsEmpty() )
		strNewListName.Format("%s%s",strOriginalListName,"Copy");
	// Make sure original already exists.
	CMMList *pOriginalList = pSession->GetLists()->FindByName(strOriginalListName);
CMMList OriginalList = *pOriginalList;
	if (pOriginalList == NULL)
	{
		pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "No list to copy found.");
		return TRUE;
	}

	// Make sure new one doesn't already exist.
	CMMList *pList = pSession->GetLists()->FindByName(strNewListName);
	if (pList != NULL)
	{
		pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List already exists, not added.");
		return TRUE;
	}

	if(strSort == "a" || strSort == "d")
	{
	pList = new CMMList(OriginalList,strSort);
	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "Sorting.");

	}
	else
	{
		pList = new CMMList(OriginalList);
	}
	pList->ListName(strNewListName);
	pSession->GetLists()->AddToList(pList);

	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List added.");
	return TRUE;
}
BOOL CCommandTable::ListSort(CSession *pSession, CString &strLine)
{
	CString strOriginalListName, strSort;
	if(!CParseUtils::FindStatement(pSession, strLine,strOriginalListName))
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "listcopy Usage: /listcopy {original name}{new name (optional)}{a or d (sort optional)}");
		return FALSE;
	}

	if(!CParseUtils::FindStatement(pSession, strLine,strSort))
		strSort = "a";

	CParseUtils::ReplaceVariables(pSession, strOriginalListName);
	
	if (strOriginalListName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Usage: /listcopy {original name}{new name (optional)}{a or d (sort optional)}");
		return TRUE;
	}
	// Make sure original already exists.
	CMMList *pOriginalList = pSession->GetLists()->FindByName(strOriginalListName);
	CMMList OriginalList = *pOriginalList;
	if (pOriginalList == NULL)
	{
		pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "No list to sort found.");
		return TRUE;
	}
	CMMList *pList;
	if(strSort == "a" || strSort == "d")
	{
	pList = new CMMList(OriginalList,strSort);
	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "Sorting.");
	}
	else
	{
		pList = new CMMList(OriginalList);
	}

	pSession->GetLists()->Remove(pOriginalList);
	pList->ListName(strOriginalListName);
	pSession->GetLists()->AddToList(pList);

	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List sorted.");
	return TRUE;
}

BOOL CCommandTable::ListDelete(CSession *pSession, CString &strLine)
{
	CString strListName;
	if(!CParseUtils::FindStatement(pSession, strLine,strListName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);

	if (strListName.IsEmpty())
		return TRUE;

	BOOL bResult;
	int nIndex = atoi(strListName);
	if (!nIndex)
	{
		//pSession->GetLists()->FindByName(strListName)->Items().RemoveAll();
		bResult = pSession->GetLists()->Remove(strListName);
	}
	else
		bResult = pSession->GetLists()->Remove(nIndex-1);

	if(bResult)
		pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List removed.");
	return TRUE;
}

BOOL CCommandTable::ListItems(CSession *pSession, CString &strLine)
{
	CString strListName;
	if(!CParseUtils::FindStatement(pSession, strLine,strListName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);

	if (strListName.IsEmpty())
		return TRUE;

	CMMList *pList;
	int nIndex = atoi(strListName);
	if (!nIndex)
		pList = pSession->GetLists()->FindByName(strListName);
	else
		pList = (CMMList *)pSession->GetLists()->GetAt(nIndex-1);

	if (pList == NULL)
		return TRUE;

	CString strText;
	strText.Format("%sItems in list [%s%s%s](%s%d%s):%s",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDYELLOW,
		(LPCSTR)pList->ListName(),
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		pList->Items().GetCount(),
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	pSession->PrintAnsiNoProcess(strText);

	int numItem = 0;
	CString strMessage;
	int nCount = pList->Items().GetCount();
	for(int i = 0; i < nCount; ++i)
	{
		CString listItem = pList->Items().GetAt(i);
		strText.Format(_T("%s%03d:%s %s\n"),
			ANSI_F_BOLDWHITE,
			numItem + 1,
			ANSI_RESET,
			(LPCSTR)listItem);
		strMessage += strText;
		numItem++;
	}
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::Lists(CSession *pSession, CString &strText)
{
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Lists:");

	CMMList *pList = (CMMList *)pSession->GetLists()->GetFirst();
	CString strMessage;
	while(pList != NULL)
	{
		strText.Format("%s%03d:%s %s(%d) {%s}\n",
			ANSI_F_BOLDWHITE,
			pSession->GetLists()->GetFindIndex()+1,
			ANSI_RESET,
			(LPCSTR)pList->ListName(),
			pList->Items().GetCount(),
			(LPCSTR)pList->Group());
		strMessage += strText;

		pList = (CMMList *)pSession->GetLists()->GetNext();
	}
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::LogPath(CSession *pSession, CString &strLine)
{
	CString strPath;
	if(!CParseUtils::FindStatement(pSession, strLine, strPath))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strPath);

	if(strPath.IsEmpty())
	{
		strPath = pSession->GetOptions().pathOptions_.LogPath();

		CString strMessage;
		strMessage.Format(
			_T("Your log path is currently set to: %s"),
			strPath);

		pSession->QueueMessage(
			CMessages::MM_GENERAL_MESSAGE, 
			strMessage, 
			TRUE);

		return TRUE;
	}

	pSession->GetOptions().pathOptions_.LogPath(strPath);
	CString strMessage;
	strMessage.Format("Log path set to [%s]", strPath);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
	return TRUE;
}

BOOL CCommandTable::LoadSession(CSession *pSession, CString &strLine)
{
	CString strFile;
	CParseUtils::FindStatement(pSession, strLine,strFile);
	CParseUtils::ReplaceVariables(pSession,strFile);
	CFileStatus fs;
	if(CFile::GetStatus(strFile, fs))
			{
				if (!AfxGetApp()->m_pDocManager->OpenDocumentFile(strFile))
				{
					::OutputDebugString(strFile);  //if we are here debug output is appropriate
					return FALSE;
				}
			}
	else
	{
			CString strMessage;
			strMessage.Format("Couldn't  open  [%s]", strFile);
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
	}
	return TRUE;
}


BOOL CCommandTable::Loop(CSession *pSession, CString &strLine)
{
	CString strRange, strCommand;
	if(!CParseUtils::FindStatement(pSession, strLine,strRange))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strCommand))
		return FALSE;


	if (strRange.IsEmpty() || strCommand.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strRange);

	LPSTR pBuf = new char[strRange.GetLength()+1];
	strcpy(pBuf,strRange);
	LPSTR ptr = strtok(pBuf,",");
	if (ptr == NULL)
	{
		delete [] pBuf;
		pBuf = NULL;
		return TRUE;
	}
	int nStart = atoi(ptr);
	ptr = strtok(NULL,",");
	if (ptr == NULL)
	{
		delete [] pBuf;
		pBuf = NULL;
		return TRUE;
	}
	int nEnd = atoi(ptr);
	ptr = strtok(NULL,",");

	CString strVar(ptr);
	char szNum[20];

	if (nEnd >= nStart)
	{
		for (int i=nStart;i<=nEnd;i++)
		{
			if (strVar.IsEmpty())
				pSession->SetLoopCount(i);
			else
			{
				_itoa( i, szNum, 10 );
				std::string name = static_cast<LPCTSTR>( strVar );
				std::string value = szNum;
				pSession->GetVariables()->Add( name, value, "" );
			}

			pSession->ExecuteCommand( strCommand );
		}
	}
	else
	{
		for (int i=nStart;i>=nEnd;i--)
		{
			if (strVar.IsEmpty())
				pSession->SetLoopCount(i);
			else
			{
				_itoa(i,szNum,10);

				std::string name = static_cast<LPCTSTR>( strVar );
				std::string value = szNum;
				pSession->GetVariables()->Add( name, value, "" );
			}

			pSession->ExecuteCommand( strCommand );
		}
	}

	if (strVar.IsEmpty())
	{
		pSession->GetVariables()->RemoveByName( static_cast<LPCTSTR>( strVar ) );
	}

	delete [] pBuf;
	pBuf = NULL;
	return TRUE;
}

