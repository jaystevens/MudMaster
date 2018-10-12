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
// CommandTable.cpp: implementation of the CCommandTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ifx.h"
#include "CommandTable.h"
#include "ParseUtils.h"
#include "ActionList.h"
#include "ArrayList.h"
#include "DllList.h"
#include "ChangesThread.h"
#include "GagList.h"
#include "Globals.h"
#include "MMList.h"
#include "Variable.h"
#include "Event.h"
#include "Dll.h"
#include "Gag.h"
#include "Sub.h"
#include "Tab.h"
#include "Macro.h"
#include "High.h"
#include "BarItem.h"
#include "direct.h"
#include "ObString.h"
#include "Sess.h"
#include "Colors.h"
#include "TabList.h"
#include "Alias.h"
#include "AliasList.h"
#include "Undo.h"
#include "StatusBar.h"
#include "ChatServer.h"
#include "EventList.h"
#include "HighList.h"
#include "SubList.h"
#include "VarList.h"
#include "MacroList.h"
#include "Action.h"
#include "Array.h"
#include "UserLists.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"
#include "StatementParser.h"

using namespace std;
using namespace MMScript;
using namespace Utilities;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHashTable CCommandTable::m_HashTable;
CCommandTable::COMMANDLIST CCommandTable::m_List[] =
{
	{"action",			CCommandTable::Action,		FALSE},
	{"addcaption",      CCommandTable::AddCaption,  FALSE},
	{"alias",			CCommandTable::Alias,		FALSE},
	{"allcommands",		CCommandTable::AllCommands,	FALSE},
	{"array",			CCommandTable::Array,		FALSE},
	{"arraycopy",		CCommandTable::ArrayCopy,	FALSE},
	{"arraydbread",		CCommandTable::ArrayDBRead,	FALSE},
	{"arraydbsave",		CCommandTable::ArrayDBSave,	FALSE},
	{"assign",			CCommandTable::Assign,		FALSE},
	{"autoaccept",		CCommandTable::AutoAccept,	FALSE},
	{"autoexec",		CCommandTable::AutoExec,	FALSE},
	{"autoserve",		CCommandTable::AutoServe,	FALSE},
	{"background",      CCommandTable::Background,  FALSE},
	{"barback",			CCommandTable::BarBack,		FALSE},
	{"barfore",			CCommandTable::BarFore,		FALSE},
	{"baritem",			CCommandTable::BarItem,		FALSE},
	{"baritemback",		CCommandTable::BarItemBack,	FALSE},
	{"baritemfore",		CCommandTable::BarItemFore,	FALSE},
	{"baritemshift",	CCommandTable::BarItemShift,FALSE},
	{"barseparator",	CCommandTable::BarSeparator,FALSE},
	{"bell",			CCommandTable::Bell,		FALSE},
	{"call",			CCommandTable::Call,		FALSE},
	{"calldll",			CCommandTable::CallDLL,		FALSE},
	{"cg",				CCommandTable::GroupChat,	FALSE},
	{"changes",			CCommandTable::Changes,		TRUE},
	{"char",			CCommandTable::Char,		TRUE},
	{"chat",			CCommandTable::Chat,		FALSE},
	{"chatall",			CCommandTable::ChatAll,		FALSE},
	{"chatback",		CCommandTable::ChatBack,	FALSE},
	{"chatexcludeserve",	CCommandTable::ChatExcludeServe,	FALSE},
	{"chatcommands",	CCommandTable::ChatCommands,FALSE},
	{"chatfore",		CCommandTable::ChatFore,	FALSE},
	{"chatgroup",		CCommandTable::GroupChat,	FALSE},
	{"chatgroupremove",	CCommandTable::ChatGroupRemove,FALSE},
	{"chatgroupset",	CCommandTable::ChatGroupSet,FALSE},
	{"chatignore",		CCommandTable::ChatIgnore,	FALSE},
	{"chatname",		CCommandTable::ChatName,	FALSE},
	{"chatport",		CCommandTable::ChatPort,	FALSE},
	{"chatprivate",		CCommandTable::ChatPrivate,	FALSE},
	{"chatserve",		CCommandTable::ChatServe,	FALSE},
	{"chatsnoop",		CCommandTable::ChatSnoop,	FALSE},
	{"chattransfers",	CCommandTable::ChatTransfers,FALSE},
	{"chatversions",	CCommandTable::ChatVersions,FALSE},
	{"clearlist",		CCommandTable::ClearList,	FALSE},
	{"clearscreen",		CCommandTable::ClearScreen,	FALSE},
	{"cr",				CCommandTable::CR,			FALSE},
	{"dbconnect",		CCommandTable::DBConnect,	FALSE},
	{"debugdepth",		CCommandTable::DebugDepth,	FALSE},
	{"defaultsession",	CCommandTable::DefaultSession, FALSE},
	{"defaultscript",	CCommandTable::DefaultScript, FALSE},
	{"dir",				CCommandTable::Dir,			FALSE},
	{"disableaction",	CCommandTable::DisableAction,FALSE},
	{"disablealias",	CCommandTable::DisableAlias,FALSE},
	{"disablebaritem",	CCommandTable::DisableBarItem,FALSE},
	{"disableevent",	CCommandTable::DisableEvent,FALSE},
	{"disablegag",		CCommandTable::DisableGag,	FALSE},
	{"disablegroup",	CCommandTable::DisableGroup,FALSE},
	{"disablehighlight",CCommandTable::DisableHighlight,FALSE},
	{"disablemacro",	CCommandTable::DisableMacro,FALSE},
	{"disablesubstitute",CCommandTable::DisableSub,	FALSE},
	{"dll",				CCommandTable::Dll,			FALSE},
	{"dnd",				CCommandTable::Dnd,			FALSE},
	{"doevents",		CCommandTable::DoEvents,	FALSE},
	{"downloadpath",	CCommandTable::DownloadPath,FALSE},
	{"echo",			CCommandTable::Echo,		FALSE},
	{"editaction",		CCommandTable::EditAction,	FALSE},
	{"editalias",		CCommandTable::EditAlias,	FALSE},
	{"editbaritem",		CCommandTable::EditBarItem,	FALSE},
	{"editcurrentsession",		CCommandTable::EditCurrentSession,	FALSE},
	{"editevent",		CCommandTable::EditEvent,	FALSE},
	{"editgag",			CCommandTable::EditGag,		FALSE},
	{"edithighlight",	CCommandTable::EditHighlight,FALSE},
	{"editmacro",		CCommandTable::EditMacro,	FALSE},
	{"editsubstitute",	CCommandTable::EditSub,		FALSE},
	{"editvariable",	CCommandTable::EditVariable,FALSE},
	{"eg",				CCommandTable::GroupEmote,	FALSE},
	{"emote",			CCommandTable::Emote,		FALSE},
	{"emoteall",		CCommandTable::EmoteAll,	FALSE},
	{"emotegroup",		CCommandTable::GroupEmote,	FALSE},
	{"empty",			CCommandTable::Empty,		FALSE},
	{"enableaction",	CCommandTable::EnableAction,FALSE},
	{"enablealias",		CCommandTable::EnableAlias,	FALSE},
	{"enablebaritem",	CCommandTable::EnableBarItem,FALSE},
	{"enableevent",		CCommandTable::EnableEvent,	FALSE},
	{"enablegag",		CCommandTable::EnableGag,	FALSE},
	{"enablegroup",		CCommandTable::EnableGroup,	FALSE},
	{"enablehighlight",	CCommandTable::EnableHighlight,FALSE},
	{"enablemacro",		CCommandTable::EnableMacro,	FALSE},
	{"enablesubstitute",CCommandTable::EnableSub,	FALSE},
	{"event",			CCommandTable::Event,		FALSE},
	{"filecancel",		CCommandTable::FileCancel,	FALSE},
	{"filedelete",		CCommandTable::FileDelete,	TRUE},
	{"filestatus",		CCommandTable::FileStatus,	FALSE},
	{"fireevent",		CCommandTable::FireEvent,	FALSE},
	{"freelibrary",		CCommandTable::FreeLibrary,	FALSE},
	{"fullscreen",		CCommandTable::FullScreen,	FALSE},
	{"gag",				CCommandTable::Gag,			FALSE},
	{"grep",			CCommandTable::Grep,		FALSE},
	{"groupactions",	CCommandTable::GroupActions,FALSE},
	{"groupaliases",	CCommandTable::GroupAliases,FALSE},
	{"grouparrays",		CCommandTable::GroupArrays, FALSE},
	{"groupbaritems",	CCommandTable::GroupBarItems,FALSE},
	{"groupevents",		CCommandTable::GroupEvents,	FALSE},
	{"groupgag",		CCommandTable::GroupGags,	FALSE},
	{"grouphighlights",	CCommandTable::GroupHighs,	FALSE},
	{"grouplists",		CCommandTable::GroupLists,	FALSE},
	{"groupmacros",		CCommandTable::GroupMacros,	FALSE},
	{"groupsubstitutes",CCommandTable::GroupSubs,	FALSE},
	{"grouptablist",	CCommandTable::GroupTablist,FALSE},
	{"groupvariables",	CCommandTable::GroupVariables,FALSE},
	{"handledbydll",	CCommandTable::HandledByDll,FALSE},
	{"help",			CCommandTable::Help,		FALSE},
	{"highlight",		CCommandTable::Highlight,	FALSE},
//	{"icqsendfile",		CCommandTable::ICQSendFile, FALSE},
	{"if",				CCommandTable::If,			FALSE},
	{"ignore",			CCommandTable::Ignore,		FALSE},
	{"ignorealiases",	CCommandTable::IgnoreAliases,FALSE},
	{"information",		CCommandTable::Information,	FALSE},
	{"intersection",	CCommandTable::Intersection,FALSE},
	{"itemadd",			CCommandTable::ItemAdd,		FALSE},
	{"itemdelete",		CCommandTable::ItemDelete,	FALSE},
	{"killall",			CCommandTable::KillAll,		TRUE},
	{"killgroup",		CCommandTable::KillGroup,	TRUE},
	{"listadd",			CCommandTable::ListAdd,		FALSE},
	{"listcopy",		CCommandTable::ListCopy,	FALSE},
	{"listdelete",		CCommandTable::ListDelete,	FALSE},
	{"listitems",		CCommandTable::ListItems,	FALSE},
	{"lists",			CCommandTable::Lists,		FALSE},
	{"listsort",		CCommandTable::ListSort,	FALSE},
	{"loadlibrary",		CCommandTable::MMLoadLibrary,FALSE},
	{"loadsession",		CCommandTable::LoadSession,FALSE},
	{"logclose",		CCommandTable::CloseLog,	FALSE},
	{"logopen",			CCommandTable::OpenLog,		FALSE},
	{"logopenappend",	CCommandTable::OpenLogAppend, FALSE},
	{"logpath",			CCommandTable::LogPath,		FALSE},
	{"loop",			CCommandTable::Loop,		FALSE},
	{"macro",			CCommandTable::Macro,		FALSE},
	{"math",			CCommandTable::Math,		FALSE},
	{"maxonopen",		CCommandTable::MaxOnOpen,	FALSE},
	{"maxrestore",		CCommandTable::MaxRestore,	FALSE},
	{"message",			CCommandTable::Message,		FALSE},
	{"msp",				CCommandTable::Msp,			FALSE},
	{"ontop",			CCommandTable::OnTop,		FALSE},
	{"pace",			CCommandTable::Pace,		FALSE},
	{"peekconnections",	CCommandTable::PeekConnections,FALSE},
	{"ping",			CCommandTable::Ping,		FALSE},
	{"playmidi",		CCommandTable::PlayMidi,	FALSE},
	{"playwave",		CCommandTable::PlayWav,		FALSE},
	{"presubstitute",	CCommandTable::Presubstitute,FALSE},
	{"processfx",		CCommandTable::ProcessFX,	FALSE},
	{"processmidi",		CCommandTable::ProcessMidi,	FALSE},
	{"promptoverwrite",	CCommandTable::PromptOverwrite,FALSE},
	{"rawmudlogclose",	CCommandTable::CloseRawMudLog,	FALSE},
	{"rawmudlogopen",	CCommandTable::OpenRawMudLog,	FALSE},
	{"read",			CCommandTable::Read,		FALSE},
	{"readviaweb",		CCommandTable::ReadViaWeb,		TRUE},
	{"remark",			CCommandTable::Remark,		FALSE},
	{"removecaption",   CCommandTable::RemoveCaption,  FALSE},
	{"requestconnects",	CCommandTable::RequestConnects,FALSE},
	{"resetevent",		CCommandTable::ResetEvent,	FALSE},
	{"savescrollback",	CCommandTable::SaveScrollback, FALSE},
	{"screenshot",		CCommandTable::ScreenShot,	FALSE},
	{"scrollback",		CCommandTable::ScrollBack,	FALSE},
	{"scrollgrep",		CCommandTable::ScrollGrep,	FALSE},
	{"sendaction",		CCommandTable::SendAction,	FALSE},
	{"sendalias",		CCommandTable::SendAlias,	FALSE},
	{"sendarray",		CCommandTable::SendArray,	FALSE},
	{"sendbaritem",		CCommandTable::SendBarItem,	FALSE},
	{"sendevent",		CCommandTable::SendEvent,	FALSE},
	{"sendfile",		CCommandTable::SendFile,	FALSE},
	{"sendgag",			CCommandTable::SendGag,		FALSE},
	{"sendgroup",		CCommandTable::SendGroup,	FALSE},
	{"sendhighlight",	CCommandTable::SendHighlight,FALSE},
	{"sendlist",		CCommandTable::SendList,	FALSE},
	{"sendmacro",		CCommandTable::SendMacro,	FALSE},
	{"sendsubstitute",	CCommandTable::SendSub,		FALSE},
	{"sendvariable",	CCommandTable::SendVariable,FALSE},
	{"session",			CCommandTable::Session,		FALSE},
	{"sessionaddress",	CCommandTable::SessionAddress, FALSE},
	{"sessionname",		CCommandTable::SessionName,	FALSE},
	{"sessionpath",		CCommandTable::SessionPath,	FALSE},
	{"sessionport",		CCommandTable::SessionPort,	FALSE},
	{"seteventtime",	CCommandTable::SetEventTime,FALSE},
//	{"setfocus",		CCommandTable::SetFocus,	FALSE},
	{"setip",			CCommandTable::SetIP,		FALSE},
	{"showme",			CCommandTable::ShowMe,		FALSE},
	{"showmeviaweb",	CCommandTable::ShowMeViaWeb,FALSE},
	{"showother",		CCommandTable::ShowOther,	FALSE},
	{"showtoast",		CCommandTable::ShowToast,	FALSE},
	{"showmsptriggers",	CCommandTable::ShowMSPTriggers,		FALSE},
	{"snoop",			CCommandTable::Snoop,		FALSE},
	{"soundpath",		CCommandTable::SoundPath,	FALSE},
	{"spam",			CCommandTable::Spam,		FALSE},
	{"speedwalk",		CCommandTable::SpeedWalk,	FALSE},
	{"sqlexecute",		CCommandTable::SQLExecute,	FALSE},
	{"sqlquery",		CCommandTable::SQLQuery,	FALSE},
	{"startchatserver",	CCommandTable::StartChatServer, FALSE},
	{"statusbar",		CCommandTable::StatusBar,	FALSE},
	{"statusbarsize",	CCommandTable::StatusBarSize,	FALSE},
	{"stopchatserver",	CCommandTable::StopChatServer,	FALSE},
	{"substitute",		CCommandTable::Substitute,	FALSE},
	{"substitutedepth",	CCommandTable::SubDepth,	FALSE},
	{"tabadd",			CCommandTable::TabAdd,		FALSE},
	{"tabdelete",		CCommandTable::TabDelete,	FALSE},
	{"tablist",			CCommandTable::TabList,		FALSE},
	{"textin",			CCommandTable::TextIn,		FALSE},
	{"textout",			CCommandTable::TextOut,		FALSE},
	{"timestamplog",	CCommandTable::TimestampLog, FALSE},
	{"unaction",		CCommandTable::UnAction,	FALSE},
	{"unalias",			CCommandTable::UnAlias,		FALSE},
	{"unarray",			CCommandTable::UnArray,		FALSE},
	{"unautoexec",		CCommandTable::UnAutoExec,	FALSE},
	{"unbaritem",		CCommandTable::UnBarItem,	FALSE},
	{"unchat",			CCommandTable::UnChat,		FALSE},
	{"undo",			CCommandTable::UnDo,		FALSE},
	{"unevent",			CCommandTable::UnEvent,		FALSE},
	{"ungag",			CCommandTable::UnGag,		FALSE},
	{"unhighlight",		CCommandTable::UnHighlight,	FALSE},
	{"unmacro",			CCommandTable::UnMacro,		FALSE},
	{"unsubstitute",	CCommandTable::UnSubstitute,FALSE},
	{"unvariable",		CCommandTable::UnVariable,	FALSE},
	{"unzap",			CCommandTable::UnZap,		TRUE},
	{"updatebaritem",	CCommandTable::UpdateBarItem,FALSE},
	{"updatestatusbar",	CCommandTable::UpdateStatusBar,FALSE},
	{"uploadpath",		CCommandTable::UploadPath,	FALSE},
	{"useplaysound",	CCommandTable::UsePlaySound,	FALSE},
	{"variable",		CCommandTable::Variable,	FALSE},
	{"verbatim",		CCommandTable::Verbatim,	FALSE},
	{"version",			CCommandTable::Version,		FALSE},
	{"viewlog",			CCommandTable::ViewLog,		FALSE},
	{"walkstop",		CCommandTable::WalkStop,	FALSE},
	{"while",			CCommandTable::While,		FALSE},
	{"window",          CCommandTable::Window,      TRUE},
	{"write",			CCommandTable::Write,		TRUE},
	{"writegroup",		CCommandTable::WriteGroup,	TRUE},
	{"zap",				CCommandTable::Zap,			TRUE},
	{NULL,				NULL,						FALSE}
};

CCommandTable::CCommandTable()
{

}

CCommandTable::~CCommandTable()
{

}


int CCommandTable::Count()
{
	int i = 0;
	while(m_List[i].proc != NULL)
	{
		i++;
	}
	return ++i;
}

BOOL CCommandTable::Verbatim(CSession *pSession, CString &/*strLine*/)
{
	pSession->ToggleVerbatim();
	return TRUE;
}


BOOL CCommandTable::TabAdd(CSession *pSession, CString &strLine)
{
	CString strParam1, strGroup;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strGroup))
		return FALSE;

	if (strParam1.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_TAB_MESSAGE, "Defined Tabadds:");
		CString strMessage, strText;
		CTab *pTab = (CTab *)pSession->GetTabList()->GetFirst();
		while(pTab != NULL)
		{
			pTab->MapToCommand(strText);
			strMessage.Append(strText+"\n");
			pTab = (CTab *)pSession->GetTabList()->GetNext();
		}
		pSession->PrintAnsiNoProcess(strMessage);

		return TRUE;
	}
	CParseUtils::ReplaceVariables(pSession, strParam1);

	if (strParam1.Find(' ') != -1)
	{
		pSession->QueueMessage(CMessages::MM_TAB_MESSAGE, "You cannot have spaces in the tablist.");
		return TRUE;
	}

	if (pSession->GetTabList()->Add(strParam1,strGroup))
	{
		if (pSession->GetOptions().messageOptions_.TabMessages())
		{
			CString strText;
			strText.Format(_T("\"%s\" added to tab list."),
				(LPCSTR)strParam1);
			pSession->QueueMessage(CMessages::MM_TAB_MESSAGE, strText);
		}
	}

	return TRUE;
}

BOOL CCommandTable::TabDelete(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	if (strParam1.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strParam1);

	BOOL bResult;
	int nIndex = atoi(strParam1);
	if (nIndex)
		bResult = pSession->GetTabList()->Remove(nIndex-1);
	else
		bResult = pSession->GetTabList()->Remove(strParam1);

	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_TAB_MESSAGE, "Word removed from tab list.");
	}
	return TRUE;
}

BOOL CCommandTable::TabList(
	CSession *pSession, 
	CString &/*strLine*/)
{
	pSession->QueueMessage(CMessages::MM_TAB_MESSAGE, "Tab list:");
	CString strText;
	CTab *pTab = (CTab *)pSession->GetTabList()->GetFirst();
	CString strMessage;
	while(pTab != NULL)
	{
		pTab->MapToText(strText, TRUE);
		strMessage += strText;		
		strMessage += "\n";
		pTab = (CTab *)pSession->GetTabList()->GetNext();
	}
	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::Spam(CSession *pSession, CString &strLine)
{
	CString strFilename;
	if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strFilename.IsEmpty())
		return TRUE;

	CStdioFile file;
	if (!file.Open(strFilename,CFile::modeRead))
	{
		strFilename += " file not found.";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strFilename);
		return TRUE;
	}

	CString strText;
	while(file.ReadString(strText))
	{
		strText += "\n";
		pSession->PrintAnsiNoProcess(strText);
	}
	file.Close();
	return TRUE;
}

BOOL CCommandTable::TextIn(CSession *pSession, CString &strLine)
{
	CString strFilename;
	if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strFilename.IsEmpty())
		return TRUE;

	CStdioFile file;
	if (!file.Open(strFilename,CFile::modeRead))
		return TRUE;

	CString strText;
	while(file.ReadString( strText ))
	{
		pSession->PrintAnsiNoProcess( strText );
		pSession->ExecuteCommand( strText );
	}
	file.Close();
	return TRUE;
}
BOOL CCommandTable::TextOut(CSession *pSession, CString &strLine)
{
	CString strFilename;
	CString strTextToOutput;
	CString strAppendInd;
	BOOL bAppend = FALSE;

	//next the filename - manditory
	if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strFilename.IsEmpty())
		return TRUE;

	//next the text - manditory
	if (!CParseUtils::FindStatement(pSession,  strLine,strTextToOutput))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strTextToOutput);
	if (strTextToOutput.IsEmpty())
		return TRUE;

	//next the Append flag - optional
	if (CParseUtils::FindStatement(pSession,  strLine,strAppendInd))
	if (!strAppendInd.IsEmpty())
		bAppend = TRUE;


	CStdioFile file;
	if (bAppend)
	{
		if (!file.Open(strFilename,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
		return TRUE;
		file.SeekToEnd();
	}
	else
	{
		if (!file.Open(strFilename,CFile::modeCreate | CFile::modeWrite))
		return TRUE;
	}

	file.WriteString ( strTextToOutput );
	file.Close();
	return TRUE;
}

BOOL CCommandTable::TimestampLog(CSession *pSession, CString &/*strLine*/)
{
	BOOL bTimeStamp = pSession->GetOptions().terminalOptions_.TimeStampLog();
	bTimeStamp = !bTimeStamp;
	pSession->GetOptions().terminalOptions_.TimeStampLog(bTimeStamp);
	CString strMessage;
	strMessage.Format(_T("Log Timestamp is now [%s]"), bTimeStamp ? _T("ON") : _T("OFF"));
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	pSession->TimeStampLog();
	return TRUE;
}

BOOL CCommandTable::UnAlias(CSession *pSession, CString &strLine)
{
	CString index;
	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, index, false ) )
	{
		if( !index.IsEmpty() )
		{
			std::string undo;
			if( pSession->GetAliases()->UnAlias( 
				CGlobals::GetCommandCharacter(), index, undo ) )
			{
				pSession->Undo().Add( undo );
				pSession->QueueMessage( CMessages::MM_ALIAS_MESSAGE, "Alias removed." );
			}
			else
			{
				pSession->QueueMessage( CMessages::MM_ALIAS_MESSAGE, "Alias not found." );
			}
		}
		else
		{
			pSession->QueueMessage( CMessages::MM_ALIAS_MESSAGE, "You must supply an index for the alias." );
		}
	}
	else
	{
		pSession->QueueMessage( CMessages::MM_ALIAS_MESSAGE, "" );
	}

	return TRUE;
}

BOOL CCommandTable::UnBarItem(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam1);
	
	if (strParam1.IsEmpty())
		return TRUE;

	BOOL bResult;
	CBarItem *pItem;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strParam1);
	if (pItem != NULL)
	{
		CString strText;

		pItem->MapToCommand(strText);
		pSession->Undo().Add(strText);

		if (nIndex)
			bResult = pSession->GetStatusBar()->Remove(nIndex-1);
		else
			bResult = pSession->GetStatusBar()->Remove(strParam1);
	}
	else
		bResult = FALSE;
	
	if (bResult)
	{
		if (pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Bar item removed.");
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Bar item not found.");
	}
	return TRUE;
}

BOOL CCommandTable::UnChat(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString strIndex;
		if(CParseUtils::FindStatement(pSession, strLine, strIndex))
		{
			CParseUtils::ReplaceVariables(pSession, strIndex);

			if(!strIndex.IsEmpty())
			{
				ErrorHandlingUtils::TESTHR(pSession->CloseChatSocket(strIndex));
			}
		}

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		//ErrorHandlingUtils::ReportException(e);
		pSession->QueueMessage( CMessages::MM_GENERAL_MESSAGE, "Error attempting to UnChat" );
		hr = e.Error();
	}
	catch(...)
	{
		pSession->QueueMessage( CMessages::MM_GENERAL_MESSAGE, "Error attempting to UnChat (not _com_error)" );
	}

	return SUCCEEDED(hr);
}

BOOL CCommandTable::UnDo(CSession *pSession, CString &strLine)
{
	CString strIndex;
	if(!CParseUtils::FindStatement(pSession,  strLine,strIndex))
		return FALSE;

	
	// List all the items in the undo buffer.
	if (strIndex.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Undo items:");

		CString strText;
		CUndo::UNDO *pUndo = pSession->Undo().GetFirst();
		CString strMessage;
		while(pUndo != NULL)
		{
			strText.Format(_T("%s%03d:%s %s\n"),
				ANSI_F_BOLDWHITE,
				pSession->Undo().GetFindIndex()+1,
				ANSI_RESET,
				(LPCSTR)pUndo->strText);
			strMessage += strText;
			pUndo = pSession->Undo().GetNext();
		}
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	int nIndex = atoi(strIndex)-1;
	CUndo::UNDO *pUndo = pSession->Undo().GetAt(nIndex);
	if (pUndo != NULL)
	{
		pSession->ExecuteCommand( pUndo->strText );
	}
	return TRUE;
}

BOOL CCommandTable::UnEvent(CSession *pSession, CString &strLine)
{
	CString strIndex;
	if(!CParseUtils::FindStatement(pSession,  strLine,strIndex))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strIndex);

	if (strIndex.IsEmpty())
		return TRUE;

	BOOL bResult;
	CMMEvent *pEvent;
	int nIndex = atoi(strIndex);

	if(nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strIndex);

	if (pEvent != NULL)
	{
		CString strText;
		pEvent->MapToCommand(strText);
		pSession->Undo().Add(strText);
		
		bResult = pSession->GetEvents()->Remove(pEvent->Name());
	}
	else
		bResult = FALSE;

	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Event Removed.");
	}
	else
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Event not found.");
	return TRUE;
}

BOOL CCommandTable::UnGag(CSession *pSession, CString &strLine)
{
	KillTrigger(pSession, strLine, *(pSession->GetGags()));
	return TRUE;
}

BOOL CCommandTable::UnHighlight(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam1);

	if (strParam1.IsEmpty())
		return TRUE;

	BOOL bResult;
	CHigh *pHigh;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pHigh = (CHigh *)pSession->GetHighlights()->GetAt(nIndex-1);
	else
		pHigh = (CHigh *)pSession->GetHighlights()->FindExact(strParam1);

	if (pHigh != NULL)
	{
		CString strText;
		pHigh->MapToCommand(strText);
		pSession->Undo().Add(strText);

		if (nIndex)
			bResult = pSession->GetHighlights()->Remove(nIndex-1);
		else
			bResult = pSession->GetHighlights()->Remove(strParam1);
	}
	else
		bResult = FALSE;

	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, "Highlight removed.");
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, "Highlight not found.");
	}
	return TRUE;
}

BOOL CCommandTable::UnMacro(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam1);

	if (strParam1.IsEmpty())
		return TRUE;

	BOOL bResult;
	CMacro *pMac;
	int nIndex = atoi(strParam1);
	if(nIndex)
		pMac = (CMacro *)pSession->GetMacros()->GetAt(nIndex - 1);
	else
		pMac = pSession->GetMacros()->FindKeyByName(strParam1);

	if (pMac != NULL)
	{
		CString strText;
		CString strName;
		CString strMods;
		CString strAction = pMac->Action();
		pMac->KeyToName(strName);
		switch(pMac->Dest())
		{
		case MMScript::CMacro::MacroDestBar:
			strAction+=";";
			break;
		case MMScript::CMacro::MacroDestBarCr:
			strAction+=":";
			break;
		case MMScript::CMacro::MacroDestMud:
			break;
		}

		strText.Format(_T("%cmacro {%s %s} {%s} {%s}\n"),
			CGlobals::GetCommandCharacter(),
			(LPCSTR)strMods,
			(LPCSTR)strName,
			(LPCSTR)strAction,
			(LPCSTR)pMac->Group());

		pSession->Undo().Add(strText);
		bResult = pSession->GetMacros()->Remove(pMac);
	}
	else
		bResult = FALSE;

	if (bResult)
	{
		pSession->UpdateMacros();
		pSession->QueueMessage(CMessages::MM_MACRO_MESSAGE, "Macro cleared.");
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_MACRO_MESSAGE, "Macro not defined.");
	}
	return TRUE;
}

BOOL CCommandTable::UnSubstitute(CSession *pSession, CString &strLine)
{
	KillTrigger(pSession, strLine, *(pSession->GetSubs()));
	return TRUE;
}

BOOL CCommandTable::UnVariable(CSession *pSession, CString &strLine)
{
	CString strVar;
	if(!CParseUtils::FindStatement(pSession,  strLine,strVar))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strVar);

	if (strVar.IsEmpty())
		return TRUE;

	CVarList *pVarList = pSession->GetVariables();
	if(NULL == pVarList)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strMessage;
	CString strFormat;

	int nIndex = _ttoi(strVar);
	if(0 != nIndex)
	{
		if(nIndex > 0 || nIndex < pVarList->GetSize())
		{
			CVariable &v = pVarList->GetAt(nIndex - 1);

			CString strTemp;
			v.MapToCommand(strTemp);
			pSession->Undo().Add(strTemp);
			pVarList->RemoveByName( v.GetName() );

			if(!strMessage.LoadString(IDS_VARIABLE_REMOVED))
				strMessage = _T("Variable Removed.");

			pSession->QueueMessage(
				CMessages::MM_VARIABLE_MESSAGE, 
				strMessage);
		}
		else
		{
			if(!strFormat.LoadString(IDS_INVALID_VARIABLE_INDEX))
				strFormat = _T("Invalid Variable Index.");

			strMessage.Format(strFormat, nIndex);

			pSession->QueueMessage(
				CMessages::MM_VARIABLE_MESSAGE, 
				strMessage);
		}
		return TRUE;
	}

	CVariable v;
	std::string name = static_cast<LPCTSTR>( strVar );
	if( pVarList->FindByName( name, v ) )
	{
		CString strTemp;
		v.MapToCommand(strTemp);
		pSession->Undo().Add(strTemp);
		if(pVarList->RemoveByName( v.GetName() ) )
		{
			if(!strMessage.LoadString(IDS_VARIABLE_REMOVED))
				strMessage = _T("Variable Removed.");

			pSession->QueueMessage(
				CMessages::MM_VARIABLE_MESSAGE, 
				strMessage);
		}
	}
	else
	{
		if(!strFormat.LoadString(IDS_VARNAME_NOT_FOUND))
			strFormat = _T("A variable by that name could not be found [%s]");

		strMessage.Format(strFormat, strVar);

		pSession->QueueMessage(
			CMessages::MM_VARIABLE_MESSAGE,
			strMessage);
	}

	return TRUE;
}

BOOL CCommandTable::UpdateBarItem(CSession *pSession, CString &strLine)
{
	if (!pSession->GetOptions().sessionInfo_.ShowStatusBar())
		return TRUE;

	CString strItem, strText;
	if(!CParseUtils::FindStatement(pSession,  strLine,strItem))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strText))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strItem);

	if (strItem.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strItem);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strItem);
	
	if (pItem == NULL)
		return TRUE;

	if (!strText.IsEmpty())
		pItem->Text(strText);

	pSession->GetStatusBar()->DrawItem(pSession, pItem);
	if(pSession->GetOptions().sessionInfo_.ShowStatusBar())
		pSession->RedrawStatusBar();
	return TRUE;
}

BOOL CCommandTable::UpdateStatusBar(
	CSession *pSession, 
	CString &/*strLine*/)
{
	if (!pSession->GetOptions().sessionInfo_.ShowStatusBar())
	return TRUE;

	pSession->RedrawStatusBar();
	return TRUE;
}

BOOL CCommandTable::UploadPath(CSession *pSession, CString &strLine)
{
	CString strPath;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPath))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPath);

	if (!strPath.IsEmpty())
		if (strPath.Right(1) != "\\")
			strPath += "\\";
	pSession->GetOptions().pathOptions_.UploadPath(strPath);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, CString("Upload path changed to: ")+strPath);
	return TRUE;
}

BOOL CCommandTable::Variable(CSession *pSession, CString &strLine)
{
	CString strVarName, strVarValue, strGroup;
	if(!CParseUtils::FindStatement(pSession,  strLine, strVarName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine, strVarValue))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine, strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strVarName);
	CParseUtils::ReplaceVariables(pSession, strVarValue);

	CVarList *pVarList = pSession->GetVariables();
	if(NULL == pVarList)
	{
		ASSERT(FALSE);
	}

	if(!strVarName.IsEmpty() && !strVarValue.IsEmpty())
	{
		std::string name  = static_cast<LPCTSTR>( strVarName );
		std::string value = static_cast<LPCTSTR>( strVarValue );
		std::string group = static_cast<LPCTSTR>( strGroup );

		int nIndex = pVarList->Add( name, value, group );
		if (-1 == nIndex)
		{
			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, "Variable not added.");
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, "Variable added.");
		}
		return TRUE;
	}

	if(strVarName.IsEmpty() && strVarValue.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, "Defined Variables:");

		CString strCommand;
		CString strLine;
		CString strMessage;

		for(int i = 0; i < pSession->GetVariables()->GetSize(); ++i)
		{
			CVariable &v = pSession->GetVariables()->GetAt(i);

			v.MapToText(strCommand, TRUE);

			strLine.Format("%s%03d:%s%s\n",
				ANSI_F_BOLDWHITE,
				i + 1,
				ANSI_RESET,
				strCommand);

			strMessage += strLine;
		}

		pSession->PrintAnsiNoProcess(strMessage);
	}

	return TRUE;
}

BOOL CCommandTable::Version(
	CSession *pSession, 
	CString &/*strLine*/)
{
	CString strText;
	CString strAppName;
	strAppName.LoadString(IDS_MAIN_TOOLBAR);

	strText = _T("Version: ");
	strText += strAppName;

	std::string v;
	CGlobals::GetFileVersion(v);
	strText += v.c_str();
	//TODO: KW  add build number
	CGlobals::GetBuildNumber(v);
    strText += _T(" Build ");
	strText += v.c_str();
	int screenx=GetSystemMetrics(SM_CXSCREEN);
	int screeny=GetSystemMetrics(SM_CYSCREEN);
	strText.AppendFormat(" Display res %d by %d",screenx,screeny);

	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
	return TRUE;
}
BOOL CCommandTable::ViewLog(CSession *pSession, CString &strLine)
{
	CString strFilename,strProcessScript;
	
	if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
		return FALSE;
	CParseUtils::FindStatement(pSession,  strLine,strProcessScript);
	//if(!strProcessScript.IsEmpty()) bProcessScript = true 
	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strFilename.IsEmpty())
		return TRUE;

	CStdioFile file;
	if (!file.Open(strFilename,CFile::modeRead))
		return TRUE;
	CTime t = CTime::GetCurrentTime();
	CString strText;

	while(file.ReadString(strText))
	{
	//	strText += "\n";
	//	pSession->PrintAnsiNoProcess(strText);
	//	pSession->PrintAnsi(strText, false, false);
		pSession->ProcessLineNoChat(strText, false, !strProcessScript.IsEmpty());
	}
	// first write the start load time
	strText.Format("# %s :: Log read start time.", t.Format("%A, %B %d, %Y::%H:%M:%S"));
	pSession->PrintAnsiNoProcess(strText);
	// now get end time and calculate elapsed time
	CTime endTime = CTime::GetCurrentTime();
	strText.Format("# %s :: Log read end time.", endTime.Format("%A, %B %d, %Y::%H:%M:%S"));
	pSession->PrintAnsiNoProcess(strText);

	CTimeSpan elapsedTime = endTime - t;
	strText.Format("# %s :: Total log read time.", elapsedTime.Format("%S"));
	pSession->PrintAnsiNoProcess(strText);

	file.Close();
	return TRUE;
}
BOOL CCommandTable::WalkStop(CSession *pSession, CString &/*strLine*/)
{
	pSession->KillSpeedwalk();
	return TRUE;
}

BOOL CCommandTable::While(CSession *pSession, CString &strLine)
{
	long iLoops = 0;
	CString strCondition, strParam;
	CStatementParser parser( pSession );
	if( parser.GetTwoStatements(
		strLine, 
		strCondition, true, 
		strParam, true ))
	{
		if( !strCondition.IsEmpty() || !strParam.IsEmpty() )
		{
			CString strTemp(strCondition);
// FRUITMAN
			CParseUtils::ReplaceVariables(pSession, strTemp,TRUE);
			while( CParseUtils::EvaluateLine( pSession, strTemp ) && iLoops < 100000 )
			{
				pSession->ExecuteCommand( strParam );
				strTemp = strCondition;
				CParseUtils::ReplaceVariables( pSession, strTemp, TRUE );
				iLoops++;
			}
		}
	}
	return TRUE;
}

BOOL CCommandTable::RemoveCaption(CSession *pSession, CString &/*strLine*/) {
	pSession->GetHost()->RemoveWindowCaption();
	return TRUE;
}

BOOL CCommandTable::AddCaption(CSession *pSession, CString &/*strLine*/) {
	pSession->GetHost()->AddWindowCaption();
	return TRUE;
}

BOOL CCommandTable::Window(CSession *pSession, CString &strLine)
{
	CString window;
    CString params;
    CString lowerparams;
	CStatementParser parser(pSession);
	
	if(parser.GetTwoStatements(strLine, window, true, params, true))
	{		
		CParseUtils::ReplaceVariables(pSession, window);
		CParseUtils::ReplaceVariables(pSession, params);
		pSession->GetHost()->SendCommandToAnotherWindow(window, params);
	}
	return TRUE;
}


BOOL CCommandTable::Zap(
	CSession *pSession, 
	CString &/*strLine*/)
{
	pSession->CloseMud();
	return TRUE;
}
BOOL CCommandTable::UnZap(
	CSession *pSession, 
	CString &strLine)
{
	strLine = pSession->GetOptions().sessionInfo_.Address();

	if(!strLine.IsEmpty())
	{
	pSession->GetOptions().terminalOptions_.ReconnectOnZap(!pSession->GetOptions().terminalOptions_.ReconnectOnZap());
	strLine.Format("%sReconnect on zap is now %s%s%s",ANSI_F_BOLDWHITE,ANSI_F_BOLDRED,pSession->GetOptions().terminalOptions_.ReconnectOnZap() ? "ON" : "OFF",ANSI_RESET);
	}
	else
		strLine.Format("Reconnect on zap requires both session address and port option be set\nAddress is now (%s) and port is now (%d)",pSession->GetOptions().sessionInfo_.Address() , pSession->GetOptions().sessionInfo_.Port());


	pSession->PrintAnsiNoProcess(strLine);
	return TRUE;
}

BOOL CCommandTable::GrepTrigger(CSession *pSession, CString& strSearch, CTriggerList& list)
{
	CTrigger *pTrigger = (CTrigger *)list.GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pTrigger)
	{
		CString strMask;
		pTrigger->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			list.PrintTrigger(pTrigger, strText);
			strMessage += strText;
			nCount++;
		}
		pTrigger = (CTrigger *)list.GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] triggers found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepActions(CSession *pSession, CString &strSearch)
{
	string list = pSession->GetActions()->Grep(strSearch);
	pSession->PrintAnsiNoProcess(list);
	return TRUE;
}

BOOL CCommandTable::GrepAliases( CSession *pSession, CString &strSearch )
{
	CString list = pSession->GetAliases()->Grep( strSearch );
	pSession->PrintAnsiNoProcess( list );
	return TRUE;
}

BOOL CCommandTable::GrepVariables(CSession *pSession, CString &strSearch)
{
	CString strMessage;
	CString strMask;
	CString strText;

	int nMatches = 0;

	for(int i = 0; i < pSession->GetVariables()->GetSize(); ++i)
	{
		CVariable v = pSession->GetVariables()->GetAt(i);
		v.MapToText(strMask, TRUE);

		if(strMask.Find(strSearch) != -1)
		{
			strText.Format("%s%03d:%s%s\n",
				ANSI_F_BOLDWHITE,
				i + 1,
				ANSI_RESET,
				strMask);
			strMessage += strText;
			++nMatches;
		}
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] variables found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nMatches,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepArrays(CSession *pSession, CString &strSearch)
{
	CMMArray *pArray = (CMMArray *)pSession->GetArrays()->GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pArray)
	{
		CString strMask;
		pArray->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			strText.Format("%s%03d:%s%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetArrays()->GetFindIndex()+1,
				ANSI_RESET,
				strMask);
			strMessage += strText;
			nCount++;
		}
		pArray = (CMMArray *)pSession->GetArrays()->GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] arrays found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepLists(CSession *pSession, CString &strSearch)
{
	CMMList *pList = (CMMList *)pSession->GetLists()->GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pList)
	{
		CString strMask;
		pList->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			strText.Format("%s%03d:%s%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetLists()->GetFindIndex()+1,
				ANSI_RESET,
				strMask);
			strMessage += strText;
			nCount++;
		}
		pList = (CMMList *)pSession->GetLists()->GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] lists found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepHighs(CSession *pSession, CString &strSearch)
{
	CHigh *pHigh = (CHigh *)pSession->GetHighlights()->GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pHigh)
	{
		CString strMask;
		pHigh->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			strText.Format("%s%03d:%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetHighlights()->GetFindIndex()+1,
				ANSI_RESET,
				pHigh->Enabled() ? ' ' : '*',
				strMask);
			strMessage += strText;
			nCount++;
		}
		pHigh= (CHigh *)pSession->GetHighlights()->GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] highlights found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepMacros(CSession *pSession, CString &strSearch)
{
	CMacro *pMacro = (CMacro *)pSession->GetMacros()->GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pMacro)
	{
		CString strMask;
		pMacro->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			strText.Format("%s%03d:%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetMacros()->GetFindIndex()+1,
				ANSI_RESET,
				pMacro->Enabled() ? ' ' : '*',
				strMask);
			strMessage += strText;
			nCount++;
		}
		pMacro = (CMacro *)pSession->GetMacros()->GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] macros found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepSubs(CSession *pSession, CString &strSearch)
{
	GrepTrigger(pSession, strSearch, *(pSession->GetSubs()));
	return TRUE;
}

BOOL CCommandTable::GrepEvents(CSession *pSession, CString &strSearch)
{
	CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
	CString strMessage;
	int nCount = 0;
	while(pEvent)
	{
		CString strMask;
		pEvent->MapToText(strMask, TRUE);
		if(strMask.Find(strSearch) != -1)
		{
			CString strText;
			strText.Format("%s%03d:%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetEvents()->GetFindIndex()+1,
				ANSI_RESET,
				pEvent->Enabled() ? ' ' : '*',
				strMask);
			strMessage += strText;
			nCount++;
		}
		pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
	}

	CString strCount;

	strCount.Format("%s[%s%d%s] events found%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strMessage += strCount;

	pSession->PrintAnsiNoProcess(strMessage);
	return TRUE;
}

BOOL CCommandTable::GrepGags(CSession *pSession, CString &strSearch)
{
	GrepTrigger(pSession, strSearch, *(pSession->GetGags()));
	return TRUE;
}

BOOL CCommandTable::UsePlaySound(CSession *pSession, CString &strLine)
{
	CString strCommand;
	CParseUtils::FindStatement(pSession, strLine, strCommand);

	if(CParseUtils::IsPartial("on", (LPCSTR) strCommand))
	{
		pSession->UsePlaySound(TRUE);
	}
	else
	{
		pSession->UsePlaySound(FALSE);
	}
	return TRUE;
}

BOOL CCommandTable::ShowMSPTriggers(CSession *pSession, CString &strLine)
{
	CString strCommand;
	CParseUtils::FindStatement(pSession, strLine, strCommand);

	if(CParseUtils::IsPartial("on", (LPCSTR)strCommand))
	{
		pSession->GetOptions().mspOptions_.ShowMSPTriggers(TRUE);
	}
	else
	{
		pSession->GetOptions().mspOptions_.ShowMSPTriggers(FALSE);
	}
	return TRUE;
}

BOOL CCommandTable::ProcessFX(CSession *pSession, CString &strLine)
{
	CString strCommand;
	CParseUtils::FindStatement(pSession, strLine, strCommand);

	if(CParseUtils::IsPartial("on", (LPCSTR)strCommand))
	{
		pSession->GetOptions().mspOptions_.ProcessFX(TRUE);
	}
	else
	{
		pSession->GetOptions().mspOptions_.ProcessFX(FALSE);
	}
	return TRUE;
}

BOOL CCommandTable::ProcessMidi(CSession *pSession, CString &strLine)
{
	CString strCommand;
	CParseUtils::FindStatement(pSession, strLine, strCommand);

	if(CParseUtils::IsPartial("on", (LPCSTR)strCommand))
	{
		pSession->GetOptions().mspOptions_.ProcessMidi(TRUE);
	}
	else
	{
		pSession->GetOptions().mspOptions_.ProcessMidi(FALSE);
	}

	return TRUE;
}

BOOL CCommandTable::OpenLogAppend(CSession *pSession, CString &strLine)
{
	CString strFileName;
	CParseUtils::FindStatement(pSession, strLine, strFileName);
	CParseUtils::ReplaceVariables(pSession, strFileName);

	if(strFileName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[/openlog] : You must supply a filename");
		return TRUE;
	}
	pSession->OpenLog(strFileName, true);
	return TRUE;
}
BOOL CCommandTable::OnTop(CSession *pSession, CString &strLine)
{
	CString strOnTop;
	CParseUtils::FindStatement(pSession, strLine, strOnTop);
	CParseUtils::ReplaceVariables(pSession, strOnTop);

	if(!strOnTop.IsEmpty() && strOnTop.MakeLower() == "true")
	{
		pSession->OnTop(TRUE);
		strLine = "Set to Always on top";
	}
	else
	{
		if(strOnTop.MakeLower() == "now")
		{
			if(pSession->OnTop()) 
			pSession->GetHost()->ThisWnd()->BringWindowToTop();
		}
		else
		{
		pSession->OnTop(FALSE);
		strLine = "Set to Not always on top";
		}
	}
	pSession->PrintAnsiNoProcess(strLine);
	return TRUE;
}

BOOL CCommandTable::OpenLog(CSession *pSession, CString &strLine)
{
#pragma message("this should treat paths like /read and /write does")
	CString strFileName;
	CParseUtils::FindStatement(pSession, strLine, strFileName);
	CParseUtils::ReplaceVariables(pSession, strFileName);

	if(strFileName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[/openlog] : You must supply a filename");
		return TRUE;
	}
	pSession->OpenLog(strFileName, false);
	return TRUE;
}
BOOL CCommandTable::OpenRawMudLog(CSession *pSession, CString &strLine)
{
	CString strFileName;
	CParseUtils::FindStatement(pSession, strLine, strFileName);
	CParseUtils::ReplaceVariables(pSession, strFileName);

	if(strFileName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[/openlog] : You must supply a filename");
		return TRUE;
	}
	pSession->OpenRawMudLog(strFileName, false);
	return TRUE;
}

BOOL CCommandTable::CloseLog(CSession *pSession, CString &/*strLine*/)
{
	pSession->CloseLog();
	return TRUE;
}
BOOL CCommandTable::CloseRawMudLog(CSession *pSession, CString &/*strLine*/)
{
	pSession->CloseRawMudLog();
	return TRUE;
}

BOOL CCommandTable::FullScreen(CSession *pSession, CString &/*strLine*/)
{
	pSession->FullScreen();
	return TRUE;
}
