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
#include "ProcedureTable.h"
#include "ParseUtils.h"
#include "MMList.h"
#include "Sess.h"
#include "UserLists.h"
#include "Globals.h"
#include "DebugStack.h"
#include "DefaultOptions.h"
#include "CommandTable.h"
#include "GlobalVariables.h"
#include "CharFormat.h"
#include "ChatServer.h"
#include "EventList.h"
#include "Event.h"
#include <direct.h>
#include "Variable.h"
#include "VarList.h"
#include "Array.h"
#include "ArrayList.h"
#include "ActionList.h"
#include "AliasList.h"
#include "Statusbar.h"
#include "GagList.h"
#include "HighList.h"
#include "MacroList.h"
#include "SubList.h"
#include "TabList.h"
#include "ErrorHandling.h"
#include "FileUtils.h"
#include "BarItem.h"
#include "boost/regex.hpp"
#include "boost/regex/mfc.hpp"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MMScript;
using namespace Utilities;
using namespace std;
//using namespace MudmasterColors;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHashTable CProcedureTable::m_HashTable;
CProcedureTable::PROCLIST CProcedureTable::m_List[] = 
{
	{"abs",				CProcedureTable::Abs},
	{"alias",			CProcedureTable::Alias},
	{"ansibold",		CProcedureTable::AnsiBold},
	{"ansireset",		CProcedureTable::AnsiReset},
	{"ansirev",			CProcedureTable::AnsiRev},
	{"arr",				CProcedureTable::GetArray},
	{"a",				CProcedureTable::GetArray},
	{"asc",				CProcedureTable::Asc},
	{"backblack",		CProcedureTable::BackBlack},
	{"backblue",		CProcedureTable::BackBlue},
	{"backcolor",		CProcedureTable::BackColor},
	{"backcyan",		CProcedureTable::BackCyan},
	{"backgreen",		CProcedureTable::BackGreen},
	{"backmagenta",		CProcedureTable::BackMagenta},
	{"backred",			CProcedureTable::BackRed},
	{"backyellow",		CProcedureTable::BackYellow},
	{"backward",		CProcedureTable::Backward},
	{"backwhite",		CProcedureTable::BackWhite},
	{"baritemback",		CProcedureTable::BaritemBack},
	{"baritemfore",		CProcedureTable::BaritemFore},
	{"charbackcolor",	CProcedureTable::CharBackColor},
	{"charcolor",		CProcedureTable::CharColor},
	{"chatconnectip",	CProcedureTable::ChatConnectIP},
	{"chatconnectname",	CProcedureTable::ChatConnectName},
	{"chatconnects",	CProcedureTable::ChatConnects},
	{"chatflags",		CProcedureTable::ChatFlags},
	{"chatname",		CProcedureTable::ChatName},
	{"chatversion",		CProcedureTable::ChatVersion},
	{"chr",				CProcedureTable::Chr},
	{"commandchar",		CProcedureTable::CommandChar},
	{"commandcount",	CProcedureTable::CommandCount},
	{"concat",			CProcedureTable::ConCat},
	{"comma",			CProcedureTable::Comma},
	{"connected",		CProcedureTable::Connected},
	{"day",				CProcedureTable::Day},
	{"decomma",			CProcedureTable::DeComma},
	{"enum",			CProcedureTable::EnumList},
	{"enumlist",		CProcedureTable::EnumList},
	{"eventtime",		CProcedureTable::EventTime},
	{"exists",			CProcedureTable::Exists},
	{"fileexists",		CProcedureTable::FileExists},
	{"foreblack",		CProcedureTable::ForeBlack},
	{"foreblue",		CProcedureTable::ForeBlue},
	{"forecolor",		CProcedureTable::ForeColor},
	{"forecyan",		CProcedureTable::ForeCyan},
	{"foregreen",		CProcedureTable::ForeGreen},
	{"foremagenta",		CProcedureTable::ForeMagenta},
	{"forered",			CProcedureTable::ForeRed},
	{"foreyellow",		CProcedureTable::ForeYellow},
	{"forewhite",		CProcedureTable::ForeWhite},
	{"getarray",		CProcedureTable::GetArray},
	{"getarraycols",	CProcedureTable::GetArrayCols},
	{"getarrayrows",	CProcedureTable::GetArrayRows},
	{"getcount",		CProcedureTable::GetCount},
	{"getitem",			CProcedureTable::GetItem},
	{"hour",			CProcedureTable::Hour},
	{"if",				CProcedureTable::If},
	{"inlist",			CProcedureTable::InList},
	{"ip",				CProcedureTable::IP},
	{"ipp",				CProcedureTable::IPP},
	{"isempty",			CProcedureTable::IsEmpty},
	{"isnumber",		CProcedureTable::IsNumber},
	{"left",			CProcedureTable::Left},
	{"leftpad",			CProcedureTable::LeftPad},
	{"len",				CProcedureTable::Len},
	{"lower",			CProcedureTable::Lower},
	{"ltrim",			CProcedureTable::LTrim},
	{"math",			CProcedureTable::Math},
	{"mid",				CProcedureTable::Mid},
	{"microsecond",		CProcedureTable::Microsecond},
	{"minute",			CProcedureTable::Minute},
	{"month",			CProcedureTable::Month},
	{"numactions",		CProcedureTable::NumActions},
	{"numaliases",		CProcedureTable::NumAliases},
	{"numarrays",		CProcedureTable::NumArrays},
	{"numbaritems",		CProcedureTable::NumBarItems},
	{"numevents",		CProcedureTable::NumEvents},
	{"numgags",			CProcedureTable::NumGags},
	{"numhighlights",	CProcedureTable::NumHighlights},
	{"numlists",		CProcedureTable::NumLists},
	{"nummacros",		CProcedureTable::NumMacros},
	{"numsubstitutes",	CProcedureTable::NumSubstitutes},
	{"numtablist",		CProcedureTable::NumTabList},
	{"numvariables",	CProcedureTable::NumVariables},
	{"padleft",			CProcedureTable::PadLeft},
	{"padright",		CProcedureTable::PadRight},
	{"pretrans",		CProcedureTable::PreTrans},
	{"procedurecount",	CProcedureTable::ProcedureCount},
	{"random",			CProcedureTable::Random},
	{"readabletime",	CProcedureTable::ReadableTime},
	{"regex",			CProcedureTable::RegEx},
	{"regexarray",		CProcedureTable::RegExArray},
	{"regexmatch",		CProcedureTable::RegExMatch},
	{"replace",			CProcedureTable::Replace},
	{"right",			CProcedureTable::Right},
	{"rightpad",		CProcedureTable::RightPad},
	{"rtrim",			CProcedureTable::RTrim},
	{"second",			CProcedureTable::Second},
	{"sessionname",		CProcedureTable::SessionName},
	{"sessionpath",		CProcedureTable::SessionPath},
	{"statusbarsize",	CProcedureTable::StatusBarSize},
	{"stripansi",		CProcedureTable::StripAnsi},
	{"strstr",			CProcedureTable::StrStr},
	{"strstrrev",		CProcedureTable::StrStrRev},
	{"substr",			CProcedureTable::SubStr},
	{"textcolor",		CProcedureTable::TextColor},
	{"time",			CProcedureTable::Time},
	{"timetoday",		CProcedureTable::TimeToDay},
	{"timetodayofweek",	CProcedureTable::TimeToDayOfWeek},
	{"timetohour",		CProcedureTable::TimeToHour},
	{"timetominute",	CProcedureTable::TimeToMinute},
	{"timetomonth",		CProcedureTable::TimeToMonth},
	{"timetosecond",	CProcedureTable::TimeToSecond},
	{"timetoyear",		CProcedureTable::TimeToYear},
	{"timezone",		CProcedureTable::TimeZone},
	{"upper",			CProcedureTable::Upper},
	{"val",				CProcedureTable::Val},
	{"var",				CProcedureTable::Var},
	{"version",			CProcedureTable::VersionProc},
	{"word",			CProcedureTable::Word},
	{"wordcolor",		CProcedureTable::WordColor},
	{"wordcount",		CProcedureTable::WordCount},
	{"year",			CProcedureTable::Year},
	{NULL,NULL},
};

CProcedureTable::CProcedureTable()
{

}

CProcedureTable::~CProcedureTable()
{

}

int CProcedureTable::Count()
{
	int i = 0;
	while(m_List[i].proc != NULL)
	{
		i++;
	}
	return ++i;
}

BOOL CProcedureTable::Hash()
{
	UINT nCurEntry=0;		// Current hash table entry.
	UINT nEntry;			// Entry of the item being checked.
	int i;
	int nStart;
	int nEnd;

 	m_HashTable.Clear();

	nStart = CHashTable::HASHITEM_EMPTY;
	nEnd = CHashTable::HASHITEM_EMPTY;
	for (i=0;m_List[i].proc;i++)
	{
		nEntry = m_List[i].szProcedure[0];

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

void CProcedureTable::CommandToList(CSession *pSession, CString &strParams)
{
	CString strListName, strCommand;
	string	remainder = CParseUtils::FindParam(strParams,strListName);
	remainder = CParseUtils::FindParam( remainder.c_str(), strCommand );

	if (strListName.IsEmpty() || strCommand.IsEmpty())
		return;

	CParseUtils::ReplaceVariables(pSession, strListName);
	CMMList *pList = pSession->GetLists()->FindByName(strListName);

	if (pList == NULL)
		return;

	CString strText;
	for(int i = 0; i < pList->Items().GetCount(); ++i)
	{
		pSession->SetListItem(pList->Items().GetAt(i));
		strText = strCommand;
//		CParseUtils::ReplaceVariables(pSession, strText);
		pSession->ExecuteCommand(strText);
	}
}

bool CProcedureTable::GetParameters( const std::string &paramText, std::string &params )
{
	using std::string;

	bool retval = false;

	int nParenCount = 1;

	string::const_iterator cur = paramText.begin();

	while( cur != paramText.end() )
	{
		if( *cur == CGlobals::PROC_OPEN_PAREN )
		{
			params += *cur;
			++nParenCount;
		}
		else if( CGlobals::GetEscapeCharacter() == *cur )
		{
			string::const_iterator next = cur+1;
			if( next != paramText.end() && CGlobals::PROC_CLOSE_PAREN == *next )
			{
				params += *cur;
				params += *next;
			}
			else
			{
				params += *cur;
			}

			cur++; //one for the escape
		}
		else if( *cur == CGlobals::PROC_CLOSE_PAREN )
		{
			--nParenCount;
			if( 0 == nParenCount )
			{
				retval = true;
				break;
			}
			else
			{
				params += *cur;
			}
		}
		else
		{
			params += *cur;
		}
		cur++;
	}

	return retval;
}

BOOL CProcedureTable::ProcedureVar( 
	CSession *pSession, 
	const std::string &procedure, 
	std::string &strResult )
{
	using std::string;

	BOOL retVal = FALSE;

	CString lowerProcName;
	string procName;
	string::size_type pos = procedure.find( CGlobals::PROC_OPEN_PAREN);
	if( string::npos != pos )
	{
		procName = left_string( procedure, pos );
		lowerProcName = procName.c_str();
		PROCLIST *pProc = FindProcedure( pSession, lowerProcName.MakeLower() );
		if( NULL != pProc )
		{
			string paramText = procedure.substr( pos + 1 );
			if( !paramText.empty() )
			{
				std::string params;
				if( GetParameters( paramText, params ) )
				{
					CString procResult;
					CString parameters = params.c_str();
					retVal = pProc->proc( pSession, parameters, procResult );
		
					//TODO:KW count procedures processed
					pSession->SetCmdsProcessed(pSession->GetCmdsProcessed() + 1);

					if( retVal )
						strResult = static_cast<LPCTSTR>( procResult );
				}
			}			
		}
	}


	return retVal;
}

char *CProcedureTable::FindProcedureName(CSession *pSession, const CString& strProcName)
{
	PROCLIST *pProc = FindProcedure(pSession, strProcName);
	if(pProc == NULL)
		return NULL;

	return pProc->szProcedure;
}

CProcedureTable::PROCLIST *CProcedureTable::FindProcedure(CSession *pSession, const CString& strProcName)
{
	CHashTable::HASHITEM *phi = m_HashTable[strProcName[0]];
	if (!phi || 
		phi->nStart == CHashTable::HASHITEM_EMPTY || 
		phi->nEnd == CHashTable::HASHITEM_EMPTY)
	{
		CGlobals::PrintDebugText(pSession, "Invalid procedure.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	for (int i=phi->nStart;i<=phi->nEnd && m_List[i].proc;i++)
		if (strProcName == m_List[i].szProcedure)
			return &m_List[i];

	return NULL;
}

BOOL CProcedureTable::DoProcedure(CSession *pSession, LPCSTR pszProc)
{
	CString strProc(pszProc);
	if(strProc.Find("(") == -1 || strProc.Find(")") == -1) // this is not really a procedure so avoid infinite looping
		return TRUE;  
	strProc = ""; 
	// Get past the '@'.
	pszProc++;

	// Pull the procedure name off.
	while(*pszProc)
	{
		if (*pszProc != '(')
			strProc += *pszProc;
		else
			break;
		pszProc++;
	}
	
	// Skip over the paren.
	pszProc++;

	// Collect everything between the parens.
	int nParenCount = 1;
	BOOL bInQuote = FALSE;
	CString strParams;
	while(*pszProc)
	{		
		switch(*pszProc)
		{
			case '"':
				if (bInQuote)
					bInQuote = FALSE;
				else
					bInQuote = TRUE;
				break;

			case '(':
				if (!bInQuote)
					nParenCount++;
				break;
			case ')':
				if (!bInQuote)
					nParenCount--;
				break;
		}
		if (!nParenCount)
			break;

		strParams += *pszProc;
		pszProc++;

	}

	if (strProc == "CommandToList")
	{
		try
		{
			CommandToList(pSession, strParams);
			return(TRUE);
		}
		catch(...)
		{
			::OutputDebugString(_T("Error executing command in CommandToList: "));
			::OutputDebugString(strParams + (_T("\n")));
		}
	}
	return(FALSE);
}

BOOL CProcedureTable::CommandCount(
	CSession * /*pSession*/, 
	CString & /*strParams*/, 
	CString & strResult)
{
	strResult.Format("%d commands found", CCommandTable::Count()-1);
	return TRUE;
}
BOOL CProcedureTable::CommandChar(
	CSession * /*pSession*/, 
	CString & /*strParams*/, 
	CString & strResult)
{
	strResult = "The command character is ";
	strResult += _T(CGlobals::GetCommandCharacter());
	strResult += ". You can change it with the ";
	strResult += _T(CGlobals::GetCommandCharacter());
	strResult += "char command";
	return TRUE;
}

BOOL CProcedureTable::ProcedureCount(
	CSession * /*pSession*/, 
	CString & /*strParams*/, 
	CString & strResult)
{
	CString strLine;
	strResult.Format("%d procedures found\n", CProcedureTable::Count()-1);
	int	i = 0;
		while(CProcedureTable::m_List[i].proc != NULL )
		{
			strLine.Format("%s%03d: %s%s\n",
				ANSI_F_BOLDWHITE,
				i + 1,
				ANSI_RESET,
				_T(CProcedureTable::m_List[i].szProcedure ));
			strResult += strLine;
			i++;
		}

	return TRUE;
}

BOOL CProcedureTable::Abs(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strNum;
	CParseUtils::FindParam(strParams,strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);
	if(!atoi(strNum) && strNum != "0")
	{
		CGlobals::PrintDebugText(pSession, "Invalid value passed to @Abs");
	}
	strResult.Format("%d",abs(atoi(strNum)));
	return(TRUE);
}
BOOL CProcedureTable::Alias(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strAlias;
	CAliasList::MatchData m;

	CParseUtils::FindParam(strParams,strAlias);
	CParseUtils::ReplaceVariables(pSession, strAlias);
	if( pSession->GetAliases()->FindEnabledAlias( strAlias, m ) )
	{
		strResult = m._command;
		CParseUtils::ReplaceVariables(pSession, strResult);
	}
	else
	{
        strResult = strAlias;
	}
	return(TRUE);
}

BOOL CProcedureTable::AnsiBold(
	CSession * /*pSession*/, 
	CString & /*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_BOLD;
	return(TRUE);
}

BOOL CProcedureTable::AnsiReset(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_RESET;
	return(TRUE);
}
//TODO: KW add reverse
BOOL CProcedureTable::AnsiRev(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_REVERSE;
	return(TRUE);
}


BOOL CProcedureTable::BackBlack(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_B_BLACK;
	return(TRUE);
}

BOOL CProcedureTable::BackBlue(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_BLUE;
	return(TRUE);
}

BOOL CProcedureTable::BackCyan(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_CYAN;
	return(TRUE);
}

BOOL CProcedureTable::BackGreen(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_GREEN;
	return(TRUE);
}

BOOL CProcedureTable::BackMagenta(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_MAGENTA;
	return(TRUE);
}

BOOL CProcedureTable::BackRed(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_RED;
	return(TRUE);
}

BOOL CProcedureTable::BackYellow(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_YELLOW;
	return(TRUE);
}

BOOL CProcedureTable::BackWhite(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString & strResult)
{
	strResult = ANSI_B_WHITE;
	return(TRUE);
}
BOOL CProcedureTable::BaritemBack(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strItem;
	if(!CParseUtils::FindStatement(pSession, strParams,strItem))
		return FALSE;


	if (strItem.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strItem);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strItem);

	if (pItem != NULL)
	{
		strResult.Format("%d",pItem->BackColor());
	}
	return TRUE;
}
BOOL CProcedureTable::BaritemFore(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strItem;
	if(!CParseUtils::FindStatement(pSession, strParams,strItem))
		return FALSE;


	if (strItem.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strItem);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strItem);

	if (pItem != NULL)
	{
		strResult.Format("%d",pItem->ForeColor());
	}
	return TRUE;
}

BOOL CProcedureTable::CharColor(CSession *pSession, CString & strParams, CString & strResult)
{
	CCharFormat *pColorBuffer = GlobalVariables::g_pszCurrentColorBuffer;
	CString strText = GlobalVariables::g_pszCurrentLineBuffer;

	if(pColorBuffer  == NULL)
	{
		CGlobals::PrintDebugText(pSession, "Null pColorBuffer ");
		strResult = "-1";
		return TRUE;
	}

	CString strNum;
	CParseUtils::FindParam(strParams, strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);

	int nIndex = atoi(strNum)-1;
	if (nIndex < 0 || nIndex >= strText.GetLength())
	{
		strResult = "-1";
		return(TRUE);
	}

	strResult.Format("%d", pColorBuffer[nIndex].Fore());
	return(TRUE);
}
BOOL CProcedureTable::CharBackColor(CSession *pSession, CString & strParams, CString & strResult)
{
	CCharFormat *pColorBuffer = GlobalVariables::g_pszCurrentColorBuffer;
	CString strText = GlobalVariables::g_pszCurrentLineBuffer;

	if(pColorBuffer  == NULL)
	{
		CGlobals::PrintDebugText(pSession, "Null pColorBuffer ");
		strResult = "-1";
		return TRUE;
	}

	CString strNum;
	CParseUtils::FindParam(strParams, strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);

	int nIndex = atoi(strNum)-1;
	if (nIndex < 0 || nIndex >= strText.GetLength())
	{
		strResult = "-1";
		return(TRUE);
	}

	strResult.Format("%d", pColorBuffer[nIndex].Back());
	return(TRUE);
}

BOOL CProcedureTable::ChatConnects(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString & strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format(
		_T("%d"),
		pSession->GetChat().GetCount());

	return TRUE;
}

BOOL CProcedureTable::ChatName(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strNum;
	int index = 0;

	CParseUtils::FindParam(strParams,strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);

	COptions &options = pSession->GetOptions();

	index = atoi(strNum);
	if (0 == index)
		strResult = options.chatOptions_.ChatName();
	else
	{
		HRESULT hr = pSession->GetChat().GetChatName(index, strResult);
		if(FAILED(hr))
		{
			CString strTemp;
			strTemp.Format("@ChatName(%d): Chat index out of range.", index);
			CGlobals::PrintDebugText(pSession, strTemp);
			pSession->DebugStack().DumpStack(pSession, options.debugOptions_.DebugDepth());
		}

	}

	return(TRUE);
}

BOOL CProcedureTable::Chr(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strNum;
	CParseUtils::FindParam(strParams,strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);
	int nNum = atoi(strNum);
	if (nNum < 0 || nNum > 254)
	{
		CString strTemp;
		strTemp.Format("@Chr(%d): Character values must be between 0 and 254.",nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	if(nNum == 8)
	{
		CString strTemp;
		strTemp.Format("@Chr(%d): The MM2K police have intercepted your attempt to create a backspace character.", nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}
	strResult.Format("%c",nNum);
	return(TRUE);
}

BOOL CProcedureTable::Asc(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strChar;
	CParseUtils::FindParam(strParams,strChar);
	CParseUtils::ReplaceVariables(pSession, strChar);
	if(strChar.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Asc(): You must provide a character to examine.");
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}

	char c = strChar.GetAt(0);

	int nNum = (int)c;
	if(nNum < 0) nNum = nNum +256;
	strResult.Format("%d",nNum);
	return(TRUE);
}
BOOL CProcedureTable::Comma(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CParseUtils::FindParam( strParams, strText );

	CParseUtils::ReplaceVariables( pSession, strText );
	CParseUtils::Commatize( strText );
	strResult = strText;
	return(TRUE);
}

BOOL CProcedureTable::ConCat(CSession *pSession, CString & strParams, CString & strResult)
{
	CString strText;
	string remainder = CParseUtils::FindParam(strParams,strResult);
	remainder = CParseUtils::FindParam( remainder.c_str(), strText );

	CParseUtils::ReplaceVariables(pSession, strResult);
	CParseUtils::ReplaceVariables(pSession, strText);
	strResult += strText;
	return(TRUE);
}

BOOL CProcedureTable::Connected(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	if(pSession->GetConnected())
	{
		strResult = "1";
	}
	else
	{
		strResult = "0";
	}
	return TRUE;
}

BOOL CProcedureTable::Day(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();
	
	strResult.Format(
		_T("%d"),
		tNow.GetDay());

	return TRUE;
}

BOOL CProcedureTable::EventTime(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName;
	CParseUtils::FindParam(strParams,strName);
	CParseUtils::ReplaceVariables(pSession, strName);
	CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strName);
	if (pEvent == NULL)
	{
		CString strTemp;
		strTemp.Format("@EventTime(%s): Event not found.",(LPCSTR )strName);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	time_t tNow = time(NULL);
	int nDif = pEvent->Seconds() - (tNow - pEvent->Started());
	if (nDif < 0)
		nDif = 0;
	strResult.Format("%d",nDif);
	return(TRUE);
}

BOOL CProcedureTable::Exists(CSession *pSession, CString &strParams, CString &strResult)
{
	BOOL retval = FALSE;

	CString strVarName;
	CParseUtils::FindParam( strParams, strVarName );
	CParseUtils::ReplaceVariables( pSession, strVarName );
	if( strVarName.IsEmpty() )
	{
		CGlobals::PrintDebugText(pSession, "@Exists(): Variable name is missing.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
	}
	else
	{
		CVariable v;
		std::string name = static_cast<LPCTSTR>( strVarName );
		if ( pSession->GetVariables()->FindByName( name, v ) )
			strResult = '1';
		else
			strResult = '0';

		retval = TRUE;
	}

	return retval;
}

BOOL CProcedureTable::FileExists(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName;
	CParseUtils::FindParam(strParams,strName);
	CParseUtils::ReplaceVariables(pSession, strName);

	CFileStatus fs;
	strResult.Format("%d",CFile::GetStatus(strName,fs));
	return(TRUE);
}

BOOL CProcedureTable::ForeBlack(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_BLACK;
	return(TRUE);
}

BOOL CProcedureTable::ForeBlue(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_BLUE;
	return(TRUE);
}

BOOL CProcedureTable::ForeCyan(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_CYAN;
	return(TRUE);
}

BOOL CProcedureTable::ForeGreen(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_GREEN;
	return(TRUE);
}

BOOL CProcedureTable::ForeMagenta(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_MAGENTA;
	return(TRUE);
}

BOOL CProcedureTable::ForeRed(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_RED;
	return(TRUE);
}

BOOL CProcedureTable::ForeYellow(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_YELLOW;
	return(TRUE);
}

BOOL CProcedureTable::ForeWhite(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = ANSI_F_WHITE;
	return(TRUE);
}

BOOL CProcedureTable::GetArray(
	CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName, strRow, strCol;
	CParseUtils::ReplaceVariables(pSession, strParams);

	string remainder = CParseUtils::FindParam( strParams, strName );
	remainder = CParseUtils::FindParam( remainder.c_str(), strRow );
	remainder = CParseUtils::FindParam( remainder.c_str(), strCol );

	CParseUtils::ReplaceVariables(pSession, strName);
	CParseUtils::ReplaceVariables(pSession, strRow);
	CParseUtils::ReplaceVariables(pSession, strCol);

	CMMArray *ptr;
	int nIndex = atoi(strName);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strName);

	if (ptr == NULL)
	{
		CString strTemp;
		strTemp.Format("@GetArray(%s,%s,%s): Array not found.",
			(LPCSTR )strName,
			(LPCSTR )strRow,
			(LPCSTR )strCol);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	if (!pSession->GetArrays()->GetItem(ptr,atoi(strRow),atoi(strCol),strResult))
	{
		CString strTemp;
		strTemp.Format("@GetArray(%s,%s,%s): Index out of bounds.",
			(LPCSTR )strName,
			(LPCSTR )strRow,
			(LPCSTR )strCol);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	return(TRUE);
}
BOOL CProcedureTable::GetArrayRows(
	CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName;
	CParseUtils::ReplaceVariables(pSession, strParams);

	string remainder = CParseUtils::FindParam( strParams, strName );

	CParseUtils::ReplaceVariables(pSession, strName);

	CMMArray *ptr;
	int nIndex = atoi(strName);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strName);

	if (ptr == NULL)
	{
		CString strTemp;
		strTemp.Format("@GetArrayRows(%s): Array not found.",
			(LPCSTR )strName);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		strResult = "0";
	}
	else
	{
		strResult.Format("%u",ptr->Dim(0));
	}
	return(TRUE);
}
BOOL CProcedureTable::GetArrayCols(
	CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName;
	CParseUtils::ReplaceVariables(pSession, strParams);

	string remainder = CParseUtils::FindParam( strParams, strName );

	CParseUtils::ReplaceVariables(pSession, strName);

	CMMArray *ptr;
	int nIndex = atoi(strName);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strName);

	if (ptr == NULL)
	{
		CString strTemp;
		strTemp.Format("@GetArrayRows(%s): Array not found.",
			(LPCSTR )strName);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		strResult = "0";
	}
	else
	{
		strResult.Format("%u",ptr->Dim(1));
	}
	return(TRUE);
}

BOOL CProcedureTable::GetCount(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strListName;
	CParseUtils::FindParam( strParams, strListName );
	CParseUtils::ReplaceVariables(pSession, strListName);
	if (strListName.IsEmpty())
	{
		CGlobals::PrintDebugText(pSession, "@GetCount(): Missing list name.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CMMList *pList = pSession->GetLists()->FindByName(strListName);
	if (pList == NULL)
	{
		CString strTemp;
		strTemp.Format("@GetCount(%s): Cannot find list.",(LPCSTR )strListName);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	
	strResult.Format("%u",pList->Items().GetCount()); 
	return(TRUE);
}

BOOL CProcedureTable::GetItem(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strListName, strIndex;
	//TODO: see what happens  if variable replacement is done early
	CParseUtils::ReplaceVariables(pSession, strParams);
	std::string remainder = CParseUtils::FindParam( strParams, strListName );
	remainder = CParseUtils::FindParam( remainder.c_str(), strIndex );

	CParseUtils::ReplaceVariables(pSession, strListName);
	CParseUtils::ReplaceVariables(pSession, strIndex);

	if (strListName.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@GetItem(,%s): List name is empty.",
			(LPCSTR )strIndex);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	if (strIndex.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("GetItem(%s,): The item index is missing.",
			(LPCSTR )strListName);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CMMList *pList = pSession->GetLists()->FindByName(strListName);

	if (pList == NULL)
	{
		CString strTemp;
		strTemp.Format("@GetItem(%s,%s): Cannot find list.",
			(LPCSTR )strListName,
			(LPCSTR )strIndex);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nItemIndex = atoi(strIndex);
	nItemIndex--;
	if (nItemIndex < 0 || nItemIndex >= pList->Items().GetCount())
	{
		CString strTemp;
		strTemp.Format("@GetItem(%s,%s): Index out of bounds.",
			(LPCSTR )strListName,
			(LPCSTR )strIndex);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	strResult = pList->Items().GetAt(nItemIndex);

	return(TRUE); 
}

BOOL CProcedureTable::Hour(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();
	strResult.Format("%d",tNow.GetHour());
	return(TRUE);
}
BOOL CProcedureTable::If(CSession *pSession, CString &strCondition, CString &strResult)
{
	
	CParseUtils::ReplaceVariables(pSession, strCondition,TRUE);

	if ( CParseUtils::EvaluateLine( pSession, strCondition ) )
	{
		strResult = "1" ;
	}
	else
	{
		strResult = "0" ;
	}
	return TRUE;
}
BOOL CProcedureTable::RegEx(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strExpression, strString;
	string strRemainder = CParseUtils::FindParam( strParams, strExpression );
	strRemainder = CParseUtils::FindParam( strRemainder.c_str(), strString );

	CParseUtils::ReplaceVariables(pSession, strExpression,FALSE);
	CParseUtils::ReplaceVariables(pSession, strString,FALSE);
	
	if ( boost::regex_match(strString,boost::make_regex(strExpression)) )
	{
		strResult = "1" ;
	}
	else
	{
		strResult = "0" ;
	}
	return TRUE;
}
BOOL CProcedureTable::RegExArray(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strExpression, strArrayName,strValue,strSkip;
	string strRemainder = CParseUtils::FindParam( strParams, strExpression );
	strRemainder = CParseUtils::FindParam( strRemainder.c_str(), strArrayName );
	if(strArrayName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Syntax: RegexArray(RegularExpression,ArrayName)");
		return TRUE;
	}
	strRemainder = CParseUtils::FindParam( strRemainder.c_str(), strSkip );
	int iSkip;
	iSkip = atoi(strSkip);

	CParseUtils::ReplaceVariables(pSession, strExpression,FALSE);
	CParseUtils::ReplaceVariables(pSession, strArrayName,FALSE);
	CMMArray *ptr;
	int nIndex = atoi(strArrayName);
	if (nIndex)
		ptr = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		ptr = (CMMArray *)pSession->GetArrays()->FindExact(strArrayName);

	if (ptr == NULL)
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Array not found.");
	else
	{

		int j;

		for(int i=1; i < ptr->Dim(0) + 1; i++)
		{
			if(ptr->SingleDim())
			{
				ptr->GetValue(i,0,strValue);
				if( boost::regex_match(strValue,boost::make_regex(strExpression)))
				{
					if(iSkip == 0)
					{
						strResult.Format("%d",i);
						return TRUE;
					}
					iSkip--;
				}

			}
			else
			{
				for(j=1; j < ptr->Dim(1)+1; j++)
				{
					ptr->GetValue(i,j,strValue);
					if( boost::regex_match(strValue,boost::make_regex(strExpression)))
					{
						if(iSkip == 0)
						{
							strResult.Format("%d,%d",i,j);
							return TRUE;
						}
						iSkip--;
					}
				}
			}
		}
	}
		strResult = "0" ;
		return TRUE;
}
BOOL CProcedureTable::RegExMatch(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strExpression, strString;
	string strRemainder = CParseUtils::FindParam( strParams, strExpression );
	strRemainder = CParseUtils::FindParam( strRemainder.c_str(), strString );

	CParseUtils::ReplaceVariables(pSession, strExpression,FALSE);
	CParseUtils::ReplaceVariables(pSession, strString,FALSE);
	
	boost::tregex expression(strExpression);
	boost::tmatch matchText;
		CMMArray *ptr;
		ptr = (CMMArray *)pSession->GetArrays()->FindExact("RegexSubMatches"); //The regex submatch array
		for(int i = 1; i < 21;i++)
			ptr->SetValue(i,0,""); //clear all submatches

	if ( boost::regex_match(strString,matchText,expression) )
	{
		strResult =  CString(matchText[0].first,matchText.length(0)) ;
		int iMatchCount;
		iMatchCount = matchText.size();
		CMMArray *ptr;
		ptr = (CMMArray *)pSession->GetArrays()->FindExact("RegexSubMatches"); //the regex submatch array
		for(int i = 1; i < iMatchCount ;i++)
			ptr->SetValue(i,0,CString(matchText[i].first,matchText.length(i)));

	}
	else
	{
		strResult = "" ;
	}
	return TRUE;
}
BOOL CProcedureTable::InList(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strListName, strItem;
	string remainder = CParseUtils::FindParam( strParams, strListName );
	remainder = CParseUtils::FindParam( remainder.c_str(), strItem );
	
	CParseUtils::ReplaceVariables(pSession, strListName);
	CParseUtils::ReplaceVariables(pSession, strItem);

	if(strListName.IsEmpty())
	{
		CString strDebugMessage;
		strDebugMessage.Format("@InList(,%s): List name is empty.", strItem);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}

	if(strItem.IsEmpty())
	{
		CString strDebugMessage;
		strDebugMessage.Format("@InList(%s,): List name is empty.", strListName);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}


	CMMList *pList = NULL;
	int nIndex = atoi(LPCSTR(strListName));

	if(!nIndex)
		pList = pSession->GetLists()->FindByName(strListName);
	else
		pList = (CMMList *)pSession->GetLists()->GetAt(nIndex-1);


	if (pList == NULL)
	{
		CString strDebugMessage;
		strDebugMessage.Format("@InList(%s,%s): Cannot find list.", strListName, strItem);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}

	strResult = "0";
	int nCount = pList->Items().GetCount();
	for(int i = 0; i < nCount; ++i)
	{
		if(pList->Items().GetAt(i) == strItem)
		{
			strResult = "1";
			break;
		}
	}

	return(TRUE);
}
// TODO: KW added enum like procedure for lists
// Returns the 1 based index of the item in the list or 0 if it isn't found in th list
BOOL CProcedureTable::EnumList(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strListName, strItem;
	string remainder = CParseUtils::FindParam( strParams, strListName );
	remainder = CParseUtils::FindParam( remainder.c_str(), strItem );
	
	CParseUtils::ReplaceVariables(pSession, strListName);
	CParseUtils::ReplaceVariables(pSession, strItem);

	if(strListName.IsEmpty())
	{
		CString strDebugMessage;
		strDebugMessage.Format("@EnumList(,%s): List name is empty.", strItem);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}

	if(strItem.IsEmpty())
	{
		CString strDebugMessage;
		strDebugMessage.Format("@EnumList(%s,): List name is empty.", strListName);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}


	CMMList *pList = NULL;
	int nIndex = atoi(LPCSTR(strListName));

	if(!nIndex)
		pList = pSession->GetLists()->FindByName(strListName);
	else
		pList = (CMMList *)pSession->GetLists()->GetAt(nIndex-1);


	if (pList == NULL)
	{
		CString strDebugMessage;
		strDebugMessage.Format("@EnumList(%s,%s): Cannot find list.", strListName, strItem);
		CGlobals::PrintDebugText(pSession, strDebugMessage);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return FALSE;
	}

	strResult = "0";
	int nCount = pList->Items().GetCount();
	for(int i = 0; i < nCount; ++i)
	{
		if(pList->Items().GetAt(i) == strItem)
		{
			strResult.Format("%u",++i); 
			break;
		}
	}

	return(TRUE);
}

BOOL CProcedureTable::IP(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	strResult = CGlobals::GetIPAddress();
	return(TRUE);
}

BOOL CProcedureTable::IPP(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);

	strResult.Format(
		_T("%s %d"),
		CGlobals::GetIPAddress(),
		pSession->GetOptions().chatOptions_.ListenPort());

	return(TRUE);
}

BOOL CProcedureTable::IsEmpty(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CParseUtils::FindParam( strParams, strText );

	CParseUtils::ReplaceVariables( pSession, strText );

	strResult.Format("%d",strText.IsEmpty());
	return(TRUE);
}

BOOL CProcedureTable::IsNumber(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CParseUtils::FindParam( strParams, strText );

	CParseUtils::ReplaceVariables( pSession, strText );

	if (strText.IsEmpty())
	{
		strResult = "0";
		return(TRUE);
	}

	int nLen;
	int i;

	nLen = strText.GetLength();
	i = 0;
	while(i < nLen)
	{
		if (!isdigit(strText[i]))
		{
			strResult = "0";
			return(TRUE);
		}
		i++;
	}
	strResult = "1";
	return(TRUE);
}

BOOL CProcedureTable::Left(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strNum;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Left(,%s): Text string is empty.",
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Left(%s,): Number of characters is empty.",
			(LPCSTR )strText);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nNum = atoi(strNum);
	if (nNum < 1)
	{
		CString strTemp;
		strTemp.Format("@Left(%s,%d): Number of characters is invalid.",
			(LPCSTR )strText,nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (nNum > strText.GetLength())
		nNum = strText.GetLength();
	strResult = strText.Left(nNum);
	return(TRUE);
}

BOOL CProcedureTable::Len(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CParseUtils::FindParam( strParams, strText );

	CParseUtils::ReplaceVariables( pSession, strText );
	strResult.Format("%d",strText.GetLength());
	return(TRUE);
}

BOOL CProcedureTable::Lower(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::FindParam( strParams, strResult );

	CParseUtils::ReplaceVariables( pSession, strResult );

	strResult.MakeLower();
	return(TRUE);
}
BOOL CProcedureTable::Backward(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::FindParam( strParams, strResult );

	CParseUtils::ReplaceVariables( pSession, strResult );

	strResult.MakeReverse();
	return(TRUE);
}
BOOL CProcedureTable::LTrim(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::FindParam( strParams, strResult );

	CParseUtils::ReplaceVariables( pSession, strResult );
	strResult.TrimLeft();
	return(TRUE);
}

BOOL CProcedureTable::Math(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strMath;
	CParseUtils::FindParam( strParams, strMath );

	CParseUtils::ReplaceVariables( pSession, strMath );

	if (strMath.IsEmpty())
	{
		CGlobals::PrintDebugText(pSession, "@Math() : Formula is empty.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	pSession->DebugStack().Push(0, STACK_TYPE_MATH, strMath);
	if(strMath.Find(".") == -1)
	{

		long lResult;
		if (CParseUtils::CalculateMath(pSession, strMath,lResult))
		{
			strResult.Format("%ld",lResult);
		}
		else
		{
			CGlobals::PrintDebugText(pSession, "# Error in Math Formula!");
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		}
	}
	else
	{
		double lResult;
		if (CParseUtils::CalculateMath(pSession, strMath,lResult))
		{
			strResult.Format("%.4f",lResult);
		}
		else
		{
			CGlobals::PrintDebugText(pSession, "# Error in Math Formula!");
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		}
	}
	pSession->DebugStack().Pop();
	return TRUE;
}

BOOL CProcedureTable::Mid(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strStart, strNum;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strStart );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strStart);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Mid(,%s,%s): Text is empty.",
			(LPCSTR )strStart,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strStart.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Mid(%s,,%s): Start index is empty.",
			(LPCSTR )strText,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Mid(%s,%s,): Number of characters is empty.",
			(LPCSTR )strText,
			(LPCSTR )strStart);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nLen = strText.GetLength();
	int nStart = atoi(strStart);
	if (nStart < 0 || nStart >= nLen)
	{
		CString strTemp;
		strTemp.Format("@Mid(%s,%d,%s): Invalid start index.",
			(LPCSTR )strText,
			nStart,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nNum = atoi(strNum);
	if (nStart + nNum > nLen)
	{
		CString strTemp;
		strTemp.Format("@Mid(%s,%d,%d): Invalid number of characters.",
			(LPCSTR )strText,
			nStart,
			nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult = strText.Mid(nStart,nNum);
	return(TRUE);
}

BOOL CProcedureTable::Microsecond(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	LARGE_INTEGER rawTicks;
	LARGE_INTEGER freq;
	::QueryPerformanceCounter(&rawTicks);
	::QueryPerformanceFrequency(&freq);
	rawTicks.QuadPart *= 1000000;
	rawTicks.QuadPart /= freq.QuadPart;
	strResult.Format("%u",rawTicks.QuadPart);
	
	return(TRUE);
}
BOOL CProcedureTable::Minute(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();
	strResult.Format("%d",tNow.GetMinute());
	return(TRUE);
}
BOOL CProcedureTable::TimeZone(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	_tzset();
	strResult.Format("%s",_daylight?_tzname[1]:_tzname[0]);
	
	return(TRUE);
}

BOOL CProcedureTable::Month(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();

	strResult.Format("%d",tNow.GetMonth());
	return(TRUE);
}

BOOL CProcedureTable::NumActions(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetActions()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumAliases(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetAliases()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumArrays(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetArrays()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumBarItems(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetStatusBar()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumEvents(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetEvents()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumGags(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetGags()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumHighlights(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetHighlights()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumLists(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetLists()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumMacros(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetMacros()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumSubstitutes(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetSubs()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumTabList(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetTabList()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::NumVariables(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	ASSERT(pSession != NULL);
	strResult.Format("%d",pSession->GetVariables()->GetCount());
	return(TRUE);
}

BOOL CProcedureTable::PadLeft(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText, strChar, strNum;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strChar );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strChar);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strChar.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,,%s): Character to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,%s,): Number of characters to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strChar);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nNum = atoi(strNum);
	if (nNum < 0 || nNum > 200)
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,%s,%d): Number to pad must be between 0 and 200.",
			(LPCSTR )strText,
			(LPCSTR )strChar,
			nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CString strPad(strChar.GetAt(0),nNum);
	
	strResult = strPad.Right(nNum) + strText;;
	return(TRUE);
}
BOOL CProcedureTable::LeftPad(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText, strChar, strNum;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strChar );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strChar);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strChar.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,,%s): Character to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,%s,): Number of characters to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strChar);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nNum = atoi(strNum);
	if (nNum < 0 || nNum > 200)
	{
		CString strTemp;
		strTemp.Format("@PadLeft(%s,%s,%d): Number to pad must be between 0 and 200.",
			(LPCSTR )strText,
			(LPCSTR )strChar,
			nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CString strPad(strChar.GetAt(0),nNum);
	strPad += strText;
	strResult = strPad.Right(nNum);
	return(TRUE);
}

BOOL CProcedureTable::PadRight(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText, strChar, strNum;
	string remainder = CParseUtils::FindParam(strParams,strText);
	remainder = CParseUtils::FindParam( remainder.c_str(), strChar );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strChar);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strChar.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,,%s): Character to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,%s,): Number of characters to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strChar);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nNum = atoi(strNum);
	if (nNum < 0 || nNum > 200)
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,%s,%d): Number to pad must be between 0 and 200.",
			(LPCSTR )strText,
			(LPCSTR )strChar,
			nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CString strPad(strChar.GetAt(0),nNum);
	strResult = strText + strPad;
	return(TRUE);
}

BOOL CProcedureTable::RightPad(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText, strChar, strNum;
	string remainder = CParseUtils::FindParam(strParams,strText);
	remainder = CParseUtils::FindParam( remainder.c_str(), strChar );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strChar);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strChar.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,,%s): Character to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,%s,): Number of characters to pad is empty.",
			(LPCSTR )strText,
			(LPCSTR )strChar);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nNum = atoi(strNum);
	if (nNum < 0 || nNum > 200)
	{
		CString strTemp;
		strTemp.Format("@PadRight(%s,%s,%d): Number to pad must be between 0 and 200.",
			(LPCSTR )strText,
			(LPCSTR )strChar,
			nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	CString strPad(strChar.GetAt(0),nNum);
	strResult = strText + strPad;
	strResult = strResult.Left(nNum);
	return(TRUE);
}

BOOL CProcedureTable::PreTrans(CSession *pSession, CString &strParams, CString &strResult)
{
	strResult = strParams;
	CParseUtils::ReplaceVariables(pSession, strResult);
	return(TRUE);
}

BOOL CProcedureTable::Random(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strMax;
	CParseUtils::FindParam( strParams, strMax );

	CParseUtils::ReplaceVariables(pSession, strMax);

	if (strMax.IsEmpty())
	{
		CGlobals::PrintDebugText(pSession, "@Random(): No value given.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	int nNum = atoi(strMax);
	if (!nNum || nNum > RAND_MAX)
	{
		CString strTemp;
		strTemp.Format("@Random(%d): Invalid value.",nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult.Format("%u",rand()%nNum+1);
	return(TRUE);
}
BOOL CProcedureTable::Replace(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strNew;
	CString strOld;
	CString remainder;

	remainder = CParseUtils::FindParam( strParams, strText ).c_str();
	strNew = CParseUtils::FindParam(remainder , strOld ).c_str();
	

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strNew);
	CParseUtils::ReplaceVariables(pSession, strOld);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Replace(%s): Text string is empty.",
			(LPCSTR )strText);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strOld.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Replace(%s,): strOld is empty.",
			(LPCSTR )strText);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}


	strText.Replace(strOld,strNew);
	strResult = strText;
	return(TRUE);
}
BOOL CProcedureTable::DeComma(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strComma;
	strComma = ",";
	
	strText = strParams;
	CParseUtils::ReplaceVariables(pSession, strText);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@DeComma(%s): Text string is empty.",
			(LPCSTR )strText);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}

	strText.Replace(strComma,NULL);
	strResult = strText;
	return(TRUE);
}

BOOL CProcedureTable::Right(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strNum;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strNum);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Right(,%s): Text string is empty.",
			(LPCSTR )strNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strNum.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Right(%s,): Number of characters is empty.",
			(LPCSTR )strText);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nNum = atoi(strNum);
	if (nNum < 1)
	{
		CString strTemp;
		strTemp.Format("@Right(%s,%d): Number of characters is invalid.",
			(LPCSTR )strText,nNum);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}	
	if (nNum > strText.GetLength())
		nNum = strText.GetLength();
	strResult = strText.Right(nNum);
	return(TRUE);
}

BOOL CProcedureTable::RTrim(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::FindParam(strParams, strResult);
	CParseUtils::ReplaceVariables(pSession, strResult);
	strResult.TrimRight();
	return(TRUE);
}

BOOL CProcedureTable::Second(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();
	strResult.Format("%d",tNow.GetSecond());
	return(TRUE);
}

BOOL CProcedureTable::StripAnsi(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::ReplaceVariables(pSession, strParams);

	// Quick check to see if there are any codes.
	if (strParams.Find('\x1b') == -1)
	{
		strResult = strParams;
		return(TRUE);
	}

	strResult.Empty();
	BOOL bInAnsi = FALSE;
	int nIndex = 0;
	int nLen = strParams.GetLength();
	while(nIndex < nLen)
	{
		if (bInAnsi)
		{
			if (strParams.GetAt(nIndex) == 'm')
				bInAnsi = FALSE;
			nIndex++;
			continue;
		}

		if (strParams.GetAt(nIndex) == '\x1b' && nIndex < nLen-2 && strParams.GetAt(nIndex+1) == '[')
		{
			bInAnsi = TRUE;
			nIndex++;
			continue;
		}

		strResult += strParams.GetAt(nIndex);
		nIndex++;
	}

	return(TRUE);
}
BOOL CProcedureTable::StatusBarSize(CSession *pSession, CString& strParams, CString& strResult)
{
	CString strWhich;
	int iSize;
	CParseUtils::FindParam(strParams, strWhich);
	CParseUtils::ReplaceVariables(pSession, strWhich);
	
	if(strWhich.IsEmpty() || strWhich.MakeLower() == "height")
		iSize = pSession->GetOptions().statusOptions_.GetStatusBarLines();
	else
		iSize = pSession->GetOptions().statusOptions_.GetStatusBarWidth();
	strResult.Format("%d",iSize);
	return TRUE;
}

BOOL CProcedureTable::StrStr(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strSearchIn, strSearchFor;
	string remainder = CParseUtils::FindParam( strParams, strSearchIn );
	remainder = CParseUtils::FindParam( remainder.c_str(), strSearchFor );

	CParseUtils::ReplaceVariables(pSession, strSearchIn);
	CParseUtils::ReplaceVariables(pSession, strSearchFor);

	if (strSearchIn.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@StrStr(,%s): String to search is empty.",
			(LPCSTR )strSearchFor);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	if (strSearchFor.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@StrStr(%s,): String to search for is empty.",
			(LPCSTR )strSearchIn);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}

	int nIndex = strSearchIn.Find(strSearchFor);
	strResult.Format("%d",nIndex);

	return(TRUE);
}
BOOL CProcedureTable::StrStrRev(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strSearchIn, strSearchFor, strSearchInRev, strSearchForRev;
	string remainder = CParseUtils::FindParam( strParams, strSearchIn );
	remainder = CParseUtils::FindParam( remainder.c_str(), strSearchFor );

	CParseUtils::ReplaceVariables(pSession, strSearchIn);
	CParseUtils::ReplaceVariables(pSession, strSearchFor);

	if (strSearchIn.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@StrStrRev(,%s): String to search is empty.",
			(LPCSTR )strSearchFor);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	if (strSearchFor.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@StrStrRev(%s,): String to search for is empty.",
			(LPCSTR )strSearchIn);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	strSearchInRev = strSearchIn.MakeReverse();
	strSearchForRev = strSearchFor.MakeReverse();
	int nIndex = strSearchInRev.Find(strSearchForRev);
	if(nIndex > -1)
		nIndex = strSearchIn.GetLength() - nIndex - 2;
	strResult.Format("%d",nIndex);

	return(TRUE);
}

BOOL CProcedureTable::SubStr(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strStart, strEnd;
	string remainder = CParseUtils::FindParam( strParams, strText );
	remainder = CParseUtils::FindParam( remainder.c_str(), strStart );
	remainder = CParseUtils::FindParam( remainder.c_str(), strEnd );

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strStart);
	CParseUtils::ReplaceVariables(pSession, strEnd);

	if (strText.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Substr(,%s,%s): Text is empty.",
			(LPCSTR )strStart,
			(LPCSTR )strEnd);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strStart.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Substr(%s,,%s): Start index is empty.",
			(LPCSTR )strText,
			(LPCSTR )strEnd);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	if (strEnd.IsEmpty())
	{
		CString strTemp;
		strTemp.Format("@Substr(%s,%s,): End index is empty.",
			(LPCSTR )strText,
			(LPCSTR )strStart);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nLen = strText.GetLength();
	int nStart = atoi(strStart);
	if (nStart < 0 || nStart >= nLen)
	{
		CString strTemp;
		strTemp.Format("@Substr(%s,%d,%s): Invalid start index.",
			(LPCSTR )strText,
			nStart,
			(LPCSTR )strEnd);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	int nEnd = atoi(strEnd);
	if (nEnd >= nLen)
	{
		CString strTemp;
		strTemp.Format("@Substr(%s,%d,%d): Invalid end index.",
			(LPCSTR )strText,
			nStart,
			nEnd);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult = strText.Mid(nStart,(nEnd - nStart)+1);
	return(TRUE);
}


BOOL CProcedureTable::TextColor(CSession *pSession, CString &strParams, CString &strResult)
{
	ASSERT(GlobalVariables::g_pszCurrentLineBuffer != NULL);

	CString strLastLine = GlobalVariables::g_pszCurrentLineBuffer;

	CString strText;
	CParseUtils::FindParam(strParams, strText);
	CParseUtils::ReplaceVariables(pSession, strText);

	if (strText.IsEmpty())
	{
		strResult = "-1";
		return(FALSE);
	}

	int nIndex  = strLastLine.Find(strText);
	if (nIndex < 0)
	{
		strResult = "-1";
		return(TRUE);
	}
	CString strNum;
	strNum.Format("%d", nIndex+1);
	CharColor(pSession, strNum, strResult);
	return(TRUE);
}

BOOL CProcedureTable::Time(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	time_t tNow = time(NULL);
	strResult.Format("%ld",tNow);
	return(TRUE);
}

BOOL CProcedureTable::TimeToHour(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession, "@TimeToHour(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToHour(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult.Format("%d",pTime->tm_hour);
	return(TRUE);
}

BOOL CProcedureTable::TimeToMinute(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession, "@TimeToMinute(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToMinute(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession, strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult.Format("%d",pTime->tm_min);
	return(TRUE);
}

BOOL CProcedureTable::TimeToSecond(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession,"@TimeToSecond(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToSecond(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(FALSE);
	}
	strResult.Format("%d",pTime->tm_sec);
	return(TRUE);
}

BOOL CProcedureTable::TimeToDay(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession,"@TimeToDay(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToDay(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	strResult.Format("%d",pTime->tm_mday);
	return(TRUE);
}

BOOL CProcedureTable::TimeToDayOfWeek(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession,"@TimeToDayOfWeek(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToDayOfWeek(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	strResult.Format("%d",pTime->tm_wday);
	return(TRUE);
}

BOOL CProcedureTable::TimeToMonth(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession,"@TimeToMonth(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		//return true otherwise this will loop and try again forever
		return(TRUE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToMonth(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	//tm time structure returns month from 0 to 11 so add 1
	strResult.Format("%d",pTime->tm_mon +1);
	return(TRUE);
}

BOOL CProcedureTable::TimeToYear(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strTime;
	time_t nTime;
	struct tm *pTime;

	CParseUtils::FindParam(strParams,strTime);
	CParseUtils::ReplaceVariables(pSession, strTime);
	nTime = atol(strTime);
	if (strTime.IsEmpty() || !nTime)
	{
		CGlobals::PrintDebugText(pSession,"@TimeToYear(): Missing a time value.");
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	pTime = localtime(&nTime);
	if (!pTime)
	{
		CString strTemp;
		strTemp.Format("@TimeToYear(%ld): Invalid time value.",nTime);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
	strResult.Format("%d",pTime->tm_year+1900);
	return(TRUE);
}

BOOL CProcedureTable::Upper(CSession *pSession, CString &strParams, CString &strResult)
{
	CParseUtils::FindParam(strParams,strResult);
	CParseUtils::ReplaceVariables(pSession, strResult);
	strResult.MakeUpper();
	return(TRUE);
}

BOOL CProcedureTable::Val(
	CSession * /*pSession*/, 
	CString &strParams, 
	CString &strResult)
{
	CString strText;
	CParseUtils::FindParam(strParams,strText);
	strResult.Format("%d",atoi(strText));
	return(TRUE);
}

BOOL CProcedureTable::Var(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strName;
	CParseUtils::FindParam( strParams, strName );
	CParseUtils::ReplaceVariables( pSession, strName );

	CVariable v;

	std::string name = static_cast<LPCTSTR>( strName );
	if( pSession->GetVariables()->FindByName( name, v ) )
	{
		strResult = v.GetValue().c_str();
		return(TRUE);
	}
	else
	{
		CString strTemp;
		strTemp.Format("@Var(%s): Cannot find the variable.",
			(LPCSTR )strName);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}
}

BOOL CProcedureTable::VersionProc(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CString strAppName;
	strAppName.LoadString(IDS_MAIN_TOOLBAR);
	strResult = strAppName;

	std::string v;
	CGlobals::GetFileVersion(v);
	strResult += v.c_str();
	//TODO: KW  add build number
	CGlobals::GetBuildNumber(v);
    strResult += _T(" Build ");
	strResult += v.c_str();

	return(TRUE);
}

BOOL CProcedureTable::Word(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strText;
	CString strNum;
	string remainder = CParseUtils::FindParam(strParams, strText);
	remainder = CParseUtils::FindParam( remainder.c_str(), strNum);

	CParseUtils::ReplaceVariables(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strNum);
	
	// If the text is empty just return and empty string.  Don't
	// want the function to fail just because you might have an
	// emtpy string.
	if (strText.IsEmpty())
		return(TRUE);

	// Numbers less than 1 are invalid.
	int nNum = atoi(strNum);
	if (strNum.IsEmpty() || nNum < 1)
	{
		CString strTemp;
		strTemp.Format("@Word(%s,%d): Numbers less than 1 are invalid.",
			(LPCSTR )strText,nNum);
		CGlobals::PrintDebugText(pSession,strTemp);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		return(TRUE);
	}

	// Pull the words off and count them.
	int nTokenCount = 1;
	LPSTR pszText = strText.GetBuffer(strText.GetLength()+1);
	LPSTR ptr = strtok(pszText," ");
	while(pszText != NULL && nTokenCount < nNum)
	{
		ptr = strtok(NULL," ");
		nTokenCount++;
	}

	// If the token count is the same as the number of the word
	// we are looking for, we fount it.  If not the result string
	// will just remain empty and the function still returns
	// TRUE.  Again, don't want the function to fail.
	if (nNum == nTokenCount)
		strResult = ptr;
	
	return(TRUE);
}

BOOL CProcedureTable::WordColor(CSession *pSession, CString &strParams, CString &strResult)
{
	CCharFormat *pColorBuffer = GlobalVariables::g_pszCurrentColorBuffer;
	CString strLine = GlobalVariables::g_pszCurrentLineBuffer;

	int nLen = strLine.GetLength();

	if(pColorBuffer == NULL)
	{
		strResult = "-1";
		return(TRUE);
	}

	CString strNum;
	CParseUtils::FindParam(strParams, strNum);
	CParseUtils::ReplaceVariables(pSession, strNum);
	
	// Numbers less than 1 are invalid.
	int nNum = atoi(strNum);
	if (strNum.IsEmpty() || nNum < 1)
	{
		strResult = "-1";
		return(TRUE);
	}

	if (nNum == 1)
	{
		CString strNum;
		strNum = "1";
		CharColor(pSession, strNum, strResult);
		return(TRUE);
	}

	int nIndex = 0;
	int nWordCount = 1;

	while(nIndex < nLen)
	{
		if (strLine[nIndex] == ' ')
		{
			nWordCount++;
			if (nWordCount == nNum)
			{
				nIndex++;
				if (nIndex == nLen)
				{
					strResult = "-1";
					return(TRUE);
				}
				CString strIndex;
				strIndex.Format("%d", nIndex);
				CharColor(pSession, strIndex, strResult);
				return(TRUE);
			}
		}
		nIndex++;
	}
	strResult = "-1";
	return(TRUE);
}

BOOL CProcedureTable::WordCount(CSession *pSession, CString &strParams, CString &strResult)
{
	int nIndex;
	int nLen;
	int nWordCount;
	CParseUtils::ReplaceVariables(pSession, strParams);

	if (strParams.IsEmpty())
	{
		strResult = "0";
		return(TRUE);
	}

	nIndex = 0;
	nWordCount = 0;
	nLen = strParams.GetLength();

	// Eat up leading spaces.
	while(strParams.GetAt(nIndex) == ' ' && nIndex < nLen)
		nIndex++;

	while(nIndex < nLen)
	{
		nWordCount++;

		// Each up all of the word.
		while(nIndex < nLen && strParams.GetAt(nIndex) != ' ')
			nIndex++;

		if (nIndex == nLen)
			break;

		// Eat up spaces again.
		while(nIndex < nLen && strParams.GetAt(nIndex) == ' ')
			nIndex++;
	}
	strResult.Format("%d",nWordCount);
	return(TRUE);
}

BOOL CProcedureTable::Year(
	CSession * /*pSession*/, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CTime tNow = CTime::GetCurrentTime();
	strResult.Format("%d",tNow.GetYear());
	return(TRUE);
}

BOOL CProcedureTable::SessionName(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	CWnd *this_window = pSession->GetHost()->ThisWnd();
	this_window->GetWindowText(strResult);
//	strResult = pSession->GetOptions().sessionInfo_.SessionName();
	return TRUE;
}
BOOL CProcedureTable::SessionPath(
	CSession *pSession, 
	CString &/*strParams*/, 
	CString &strResult)
{
	if( !MMFileUtils::getPersonalFolder(strResult) )
		CGlobals::PrintDebugText( pSession, "@SessionPath: failed to retrieve personal folder" );
		if(strResult.Right(1) != "\\")
			{
				strResult += "\\";
			}

	return TRUE;
}

BOOL CProcedureTable::ForeColor(CSession *pSession, CString& strParams, CString& strResult)
{
	CParseUtils::ReplaceVariables(pSession, strParams);
	int nColor = atoi(strParams);
	if(nColor == 0)
	{
		strResult = "";
		return TRUE;
	}

	if(nColor < 1 || nColor > 15)
	{
		CGlobals::PrintDebugText(pSession, "@ForeColor:  parameter must be a number from 1 to 15");
		return TRUE;
	}

	switch(nColor)
	{
	case 1:
		strResult = ANSI_F_BLUE;
		break;
	case 2:
		strResult = ANSI_F_GREEN;
		break;
	case 3:
		strResult = ANSI_F_CYAN;
		break;
	case 4:
		strResult = ANSI_F_RED;
		break;
	case 5:
		strResult = ANSI_F_MAGENTA;
		break;
	case 6:
		strResult = ANSI_F_YELLOW;
		break;
	case 7:
		strResult = ANSI_F_WHITE;
		break;
	case 8:
		strResult = ANSI_F_BLACK;
		break;
	case 9:
		strResult = ANSI_F_BOLDBLUE;
		break;
	case 10:
		strResult = ANSI_F_BOLDGREEN;
		break;
	case 11:
		strResult = ANSI_F_BOLDCYAN;
		break;
	case 12:
		strResult = ANSI_F_BOLDRED;
		break;
	case 13:
		strResult = ANSI_F_BOLDMAGENTA;
		break;
	case 14:
		strResult = ANSI_F_BOLDYELLOW;
		break;
	case 15:
		strResult = ANSI_F_BOLDWHITE;
		break;
	}
	return TRUE;
}

BOOL CProcedureTable::BackColor(CSession *pSession, CString& strParams, CString& strResult)
{
	int nColor = atoi(strParams);
	if(nColor == 0)
	{
		strResult = "";
		return TRUE;
	}

	if(nColor < 1 || nColor > 8)
	{
		CGlobals::PrintDebugText(pSession, "@ForeColor:  parameter must be a number from 1 to 15");
		return TRUE;
	}

	switch(nColor)
	{
	case 1:
		strResult = ANSI_B_BLUE;
		break;
	case 2:
		strResult = ANSI_B_GREEN;
		break;
	case 3:
		strResult = ANSI_B_CYAN;
		break;
	case 4:
		strResult = ANSI_B_RED;
		break;
	case 5:
		strResult = ANSI_B_MAGENTA;
		break;
	case 6:
		strResult = ANSI_B_YELLOW;
		break;
	case 7:
		strResult = ANSI_B_WHITE;
		break;
	case 8:
		strResult = ANSI_B_BLACK;
		break;
	}
	return TRUE;
}

BOOL CProcedureTable::ReadableTime(
	CSession * /*pSession*/, 
	CString &strParams, 
	CString &strResult)
{
	CTime timeNow = CTime::GetCurrentTime();
	int nType = atoi(strParams);

	if(nType)
	{
		switch(nType)
		{
		case 12:
			if(timeNow.GetHour() > 12)
			{
				strResult.Format("%d:%-0.2dPM", timeNow.GetHour()-12, timeNow.GetMinute());
				return TRUE;
			}
			if(timeNow.GetHour() == 0)
			{
				strResult.Format("%d:%-0.2dAM", 12, timeNow.GetMinute());
				return TRUE;
			}
			if(timeNow.GetHour() == 12)
			{
				strResult.Format("%d:%-0.2dPM", 12, timeNow.GetMinute());
				return TRUE;
			}
			strResult.Format("%d:%-0.2dAM", timeNow.GetHour(), timeNow.GetMinute());
			return TRUE;
			break;
		case 24:
			strResult = timeNow.Format("%H:%M");
			return TRUE;
			break;
		case 48:
			strResult = timeNow.Format("%H:%M:%S");
			return TRUE;
			break;
		}
	}
	strResult = "-1";
	return TRUE;
}

BOOL CProcedureTable::ChatConnectIP(CSession *pSession, CString& strParams, CString& strResult)
{
	CString strChatName;
	CParseUtils::FindParam(strParams, strChatName);
	CParseUtils::ReplaceVariables(pSession, strChatName);

	if(FAILED(pSession->GetChat().GetConnectIP(strChatName, strResult)))
	{
		const CString &error=pSession->GetChat().GetError();

		CString strMessage;
		strMessage.Format("@ChatConnectIP(%s): %s.", strChatName, error);

		CGlobals::PrintDebugText(pSession, error);
	}

	return TRUE;
}

BOOL CProcedureTable::ChatConnectName(CSession *pSession, CString& strParams, CString& strResult)
{
	CString strChatName;
	CParseUtils::FindParam(strParams, strChatName);
	CParseUtils::ReplaceVariables(pSession, strChatName);

	if(FAILED(pSession->GetChat().GetConnectName(strChatName, strResult)))
	{
		const CString & error = pSession->GetChat().GetError();
		CGlobals::PrintDebugText(pSession, error);
	}

	return TRUE;
}

BOOL CProcedureTable::ChatVersion(CSession *pSession, CString &strParams, CString &strResult)
{
	CString strChatName;
	CParseUtils::FindParam(strParams, strChatName);
	CParseUtils::ReplaceVariables(pSession, strChatName);

	if(FAILED(pSession->GetChat().GetChatVersion(strChatName, strResult)))
	{
		const CString &error = pSession->GetChat().GetError();
		CString strMessage;
		strMessage.Format("@ChatVersion(%s): %s.", strChatName, error);
		CGlobals::PrintDebugText(pSession, error);
	}

	return TRUE;
}

BOOL CProcedureTable::ChatFlags(CSession *pSession, CString& strParams, CString& strResult)
{
	CString strChatName;
	CParseUtils::FindParam(strParams, strChatName);
	CParseUtils::ReplaceVariables(pSession, strChatName);

	if(FAILED(pSession->GetChat().GetChatFlags(strChatName, strResult)))
	{
		const CString &error = pSession->GetChat().GetError();
		CString strMessage;
		strMessage.Format("@ChatFlags(%s): %s.", strChatName, error);
		CGlobals::PrintDebugText(pSession, error);
	}

	return TRUE;
}