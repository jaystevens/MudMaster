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
#include "Globals.h"
#include "CommandTable.h"
#include "ParseUtils.h"
#include "MMList.h"
#include "BarItem.h"
#include "Event.h"
#include "Gag.h"
#include "High.h"
#include "Sub.h"
#include "Dll.h"
#include "Macro.h"
#include "Variable.h"
#include "ObString.h"
#include "Sess.h"
#include "Colors.h"
#include "UserLists.h"
#include "StatusBar.h"
#include "EventList.h"
#include "GagList.h"
#include "ActionList.h"
#include "AliasList.h"
#include "HighList.h"
#include "SubList.h"
#include "DllList.h"
#include "VarList.h"
#include "ChatServer.h"
#include "MacroList.h"
#include "DefaultOptions.h"
#include "ConfigFileConstants.h"
#include "FileUtils.h"
#include "IfxDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MMSystem;

BOOL CCommandTable::DBConnect(CSession *pSession, CString &strLine)
{
	CString strConnectString,strDBUser;
	CParseUtils::FindStatement(pSession,strLine, strConnectString);
		if(strConnectString.IsEmpty())
			strConnectString ="C:\\mm2k6\\MudMaster\\builds\\debug\\mm2k6Arrays.mdb";
			//strConnectString ="C:\\mm2k6\\MudMaster\\builds\\debug\\db1.mdb";
		
	CParseUtils::FindStatement(pSession,strLine, strDBUser);
		if(strDBUser.IsEmpty())
			strDBUser = "Admin";
//pSession->GetMDB().Disconnect();
	 //if(pSession->GetMDB().Connect(strConnectString,strDBUser))
		//pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, CString(_T("Database Connection Made")));
	 //else
		//pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, CString(_T("Database Connection Failed")));

	return TRUE;
}

BOOL CCommandTable::DebugDepth(CSession *pSession, CString &strLine)
{
	CString strDebugDepth;
	if(!CParseUtils::FindStatement(pSession,strLine, strDebugDepth))
		return FALSE;


	if(strDebugDepth.IsEmpty())
	{
		CString strMessage;
		strMessage = "You must provide a depth. e.g. /debugdepth {5}";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	int nDepth = atoi(strDebugDepth);
	pSession->GetOptions().debugOptions_.DebugDepth(nDepth);
	CString strMessage;
	strMessage.Format("Debug depth set to: %d", nDepth);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);

	return TRUE;
}

BOOL CCommandTable::DefaultScript(CSession *pSession, CString &strLine)
{
	CString strFile;
	if(!CParseUtils::FindStatement(pSession, strLine, strFile))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strFile);
	if(strFile.IsEmpty())
	{
		//show the default script setting
		CString strMessage;
		strMessage.Format("Default script file for this session is set to:  %s",
			pSession->GetOptions().sessionInfo_.ScriptFilename());
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	CFileStatus fs;
	if(!CFile::GetStatus(strFile, fs))
	{
		CString strMessage;
		strMessage.Format("File [%s] not found...please check the file name you tried to set or use /write {%s}to create it first.",
			strFile,strFile);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	// set the default script setting
	pSession->GetOptions().sessionInfo_.ScriptFilename(strFile);
	CString strText;
	strText.Format("Default script file set to:  %s", (LPCSTR)strFile);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
	return TRUE;
}

BOOL CCommandTable::DefaultSession(CSession *pSession, CString &strLine)
{
	CString strFile;
	if(!CParseUtils::FindStatement(pSession,strLine,strFile))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strFile);
	if (strFile.IsEmpty())
	{
		// show the default session setting
		CString strMessage;
		strMessage.Format("Current default character session file is:  [%s]",
			((CIfxApp*)AfxGetApp())->GetDefaultSessionFileName());
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	// make sure the file exists.
	CFileStatus fs;
	if(!CFile::GetStatus(strFile, fs))
	{
		CString strMessage;
		strMessage.Format("File [%s] not found...please check the file name you tried to set or File..Save Character As.",
			strFile);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	// set the default session setting
	((CIfxApp*)AfxGetApp())->SetDefaultSessionFileName(strFile);

	CString dir;
	dir = MMFileUtils::fixPath(INI_FILE_NAME);

	::WritePrivateProfileString(
			DEFAULT_SECTION_NAME,
			DEFAULT_SESSION_KEY,
			strFile,
			dir );

	CString strText;
	strText.Format("Default command file set: %s",(LPCSTR)strFile);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

	return TRUE;
}

BOOL CCommandTable::Dir(CSession *pSession, CString &strLine)
{
	CString strMask, strList;
	if(!CParseUtils::FindStatement(pSession,strLine,strMask))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strList))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strMask);
	CParseUtils::ReplaceVariables(pSession, strList);

	if (strMask.IsEmpty() || strList.IsEmpty())
		return TRUE;

	// Remove the current list if it exists and re-add it.
	pSession->GetLists()->Remove(strList);
	CMMList *pList = new CMMList;
	pList->ListName(strList);
	pSession->GetLists()->AddToList(pList);

	CString strFileData;
	CString strName;
	CString strFileSize;
	CString strDate;
	CString strTime;
	int nHour;
	FILETIME ftLocal;
	SYSTEMTIME stCreated;
	int nFileCount = 0;
	unsigned long nTotalBytes = 0;
	unsigned long nFileSize;
	WIN32_FIND_DATA fd;
	HANDLE hSearch = FindFirstFile(strMask,&fd);
	BOOL bResult = (hSearch != INVALID_HANDLE_VALUE);
	while(bResult)
	{
		nFileCount++;
		nFileSize = (fd.nFileSizeHigh * MAXDWORD) + fd.nFileSizeLow;
		nTotalBytes += nFileSize;

		// Conver file size.
		strFileSize.Format("%lu",nFileSize);
		CParseUtils::Commatize(strFileSize);

		// Filename.
		strName = fd.cFileName;
		if (strName.GetLength() > 20)
			strName = strName.Left(20);
		strFileData.Format("%-20s %11s ",
			(LPCSTR)strName,
			(LPCSTR)strFileSize);

		// File date and time.
		FileTimeToLocalFileTime(&fd.ftCreationTime,&ftLocal);
		FileTimeToSystemTime(&ftLocal,&stCreated);

		strDate.Format("%02d-%02d-%4d",
			stCreated.wMonth,
			stCreated.wDay,
			stCreated.wYear);

		nHour = stCreated.wHour;
		if (nHour > 12)
			nHour -= 12;
		if (!nHour)
			nHour++;
		strTime.Format(" %02d:%02d%c",
			nHour,
			stCreated.wMinute,
			(nHour > 11 ? 'p' : 'a'));

		strFileData += strDate + strTime;
		pList->Items().Add(strFileData);

		bResult = FindNextFile(hSearch,&fd);
	}
	strFileData.Format("ÿ %d file(s)   %lu bytes",
		nFileCount,
		nTotalBytes);
	pList->Items().Add(strFileData);

	if (hSearch != INVALID_HANDLE_VALUE)
		FindClose(hSearch);
	return TRUE;
}


BOOL CCommandTable::DisableBarItem(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam))
		return FALSE;


	if (strParam.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strParam);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strParam);

	if (pItem != NULL)
	{
		pItem->Enabled(FALSE);
		if (pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Bar Item Disabled");
	}
	return TRUE;
}

BOOL CCommandTable::DisableEvent(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam1))
		return FALSE;


	if (strParam1.IsEmpty())
		return TRUE;

	CMMEvent *pEvent;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strParam1);

	if (pEvent)
	{
		pEvent->Enabled(FALSE);
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Event disabled.");
	}
	return TRUE;
}

BOOL CCommandTable::DisableGag(CSession *pSession, CString &strLine)
{
	DisableTrigger(pSession, strLine, *(pSession->GetGags()));
	return TRUE;
}

BOOL CCommandTable::DisableGroup(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement( pSession, strLine, strGroup ) )
		return FALSE;

	CParseUtils::ReplaceVariables( pSession, strGroup );
	if( strGroup.IsEmpty() )
		return TRUE;

	int nActions = pSession->GetActions()->EnableGroup( strGroup, false );
	int nAliases = pSession->GetAliases()->EnableGroup( strGroup, false );
	int nEvents = pSession->GetEvents()->DisableGroup(strGroup);
	int nMacros = pSession->GetMacros()->DisableGroup(strGroup);
	int nBarItems = pSession->GetStatusBar()->DisableGroup(strGroup);
	int nGags = pSession->GetGags()->DisableGroup(strGroup);
	int nHighs = pSession->GetHighlights()->DisableGroup(strGroup);
	int nSubs = pSession->GetSubs()->DisableGroup(strGroup);

	if (nBarItems && pSession->GetOptions().sessionInfo_.ShowStatusBar())
		pSession->RedrawStatusBar();

	if (pSession->GetOptions().messageOptions_.EnableMessages())
	{
		CString strText;
		strText.Format(	_T("Commands Disabled: Actions(%d), Aliases(%d), Bar Items(%d), Events(%d)\n")
						_T("                   Gags(%d), Highlights(%d), Macros(%d), Substitutes(%d)\n"),
						nActions,nAliases,nBarItems,nEvents,nGags,nHighs,nMacros,nSubs);
		pSession->PrintAnsiNoProcess(strText);
	}
	return TRUE;
}

BOOL CCommandTable::DisableHighlight(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;


	if (strParam.IsEmpty())
		return TRUE;

	CHigh *pHigh;
	int nIndex = atoi(strParam);
	if (nIndex)
		pHigh = (CHigh *)pSession->GetHighlights()->GetAt(nIndex-1);
	else
		pHigh = (CHigh *)pSession->GetHighlights()->FindExact(strParam);

	if (pHigh != NULL)
	{
		pHigh->Enabled(FALSE);
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Highlight Disabled");
	}
	return TRUE;
}

BOOL CCommandTable::DisableMacro(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;


	if (strParam.IsEmpty())
		return TRUE;

	BOOL bResult = pSession->GetMacros()->DisableMacro(strParam);
	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Macro disabled.");
	}
	return TRUE;
}

BOOL CCommandTable::DisableSub(CSession *pSession, CString &strLine)
{
	DisableTrigger(pSession, strLine, *(pSession->GetSubs()));
	return TRUE;
}

BOOL CCommandTable::Dll(CSession *pSession, CString &/*strLine*/)
{
	pSession->GetDlls().print(pSession);
	return TRUE;
}

BOOL CCommandTable::Dnd(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine, strParam))
		return FALSE;


	if(strParam.IsEmpty())
	{
		pSession->GetOptions().chatOptions_.DoNotDisturb(!pSession->GetOptions().chatOptions_.DoNotDisturb());

		CString strMessage;
		strMessage.Format("DoNotDisturb is now %s.", pSession->GetOptions().chatOptions_.DoNotDisturb() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().chatOptions_.DoNotDisturb(TRUE);
		CString strMessage;
		strMessage.Format("DoNotDisturb is now %s.", pSession->GetOptions().chatOptions_.DoNotDisturb() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().chatOptions_.DoNotDisturb(FALSE);
		CString strMessage;
		strMessage.Format("DoNotDisturb is now %s.", pSession->GetOptions().chatOptions_.DoNotDisturb() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /dnd.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	return TRUE;
}

BOOL CCommandTable::DoEvents(CSession *pSession, CString& strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine, strParam))
		return FALSE;


	if(strParam.IsEmpty())
	{
		pSession->GetOptions().sessionInfo_.ProcessEvents(!pSession->GetOptions().sessionInfo_.ProcessEvents());

		CString strMessage;
		strMessage.Format("Events are now %s.", pSession->GetOptions().sessionInfo_.ProcessEvents() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().sessionInfo_.ProcessEvents(TRUE);
		CString strMessage;
		strMessage.Format("Events are now %s.", pSession->GetOptions().sessionInfo_.ProcessEvents() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().sessionInfo_.ProcessEvents(FALSE);
		CString strMessage;
		strMessage.Format("Events are now %s.", pSession->GetOptions().sessionInfo_.ProcessEvents() ? "enabled" : "disabled");

		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /doevents.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strMessage);

	return TRUE;
}

BOOL CCommandTable::DownloadPath(CSession *pSession, CString &strLine)
{
#pragma message("download path should follow the same rules for paths that /read and /write does")
	CString strPath;
	if(!CParseUtils::FindStatement(pSession, strLine,strPath))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPath);

	if(strPath.IsEmpty())
	{
		std::string path;
		pSession->GetOptions().pathOptions_.Get_DownloadPath(path);

		CString strMessage;
		strMessage.Format(
			_T("Download path is currently set to %s"),
			path.c_str());

		pSession->QueueMessage(
			CMessages::MM_GENERAL_MESSAGE,
			strMessage );

		return TRUE;
	}

	if (strPath.Right(1) != "\\")
		strPath += "\\";
	pSession->GetOptions().pathOptions_.DownloadPath(strPath);

	CString strMessage;
	strMessage.Format(
		_T("Download path changed to: %s"),
		strPath);

	pSession->QueueMessage(
		CMessages::MM_GENERAL_MESSAGE,
		strMessage);

	return TRUE;
}

BOOL CCommandTable::Echo(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine, strParam))
		return FALSE;


	if(strParam.IsEmpty())
	{
		pSession->GetOptions().terminalOptions_.LocalEcho(!pSession->GetOptions().terminalOptions_.LocalEcho());

		CString strMessage;
		strMessage.Format("You will %s echo commands to the screen.", pSession->GetOptions().terminalOptions_.LocalEcho() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().terminalOptions_.LocalEcho(TRUE);
		CString strMessage;
		strMessage.Format("You will %s echo commands to the screen.", pSession->GetOptions().terminalOptions_.LocalEcho() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().terminalOptions_.LocalEcho(FALSE);
		CString strMessage;
		strMessage.Format("You will %s echo commands to the screen.", pSession->GetOptions().terminalOptions_.LocalEcho() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /echo.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	return TRUE;
}

BOOL CCommandTable::EditBarItem(CSession *pSession, CString &strLine)
{
	CString strItem;
	if(!CParseUtils::FindStatement(pSession, strLine,strItem))
		return FALSE;


	if (strItem.IsEmpty())
	{
		pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_CONFIGURATION_STATUSBARITEMS,0 );
		return TRUE;
	}


	CBarItem *pItem;
	int nIndex = atoi(strItem);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strItem);

	if (pItem != NULL)
	{
		CString strText;
		pItem->MapToCommand(strText);
		pSession->SetEditText(strText);
	}
	return TRUE;
}

BOOL CCommandTable::EditEvent(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam1))
		return FALSE;


	if (strParam1.IsEmpty())
		return TRUE;

	CMMEvent *pEvent;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strParam1);

	if (pEvent != NULL)
	{
		CString strText;
		pEvent->MapToCommand(strText);
		pSession->SetEditText(strText);
	}
	else
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Event not found.");
	return TRUE;
}

BOOL CCommandTable::EditGag(CSession *pSession, CString &strLine)
{
	if (strLine.IsEmpty())
	{
		pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_SCRIPT_GAGS,0 );
		return TRUE;
	}

	CString strType;
	strType = "{gag} ";
	strType.Append(strLine);
	EditTrigger(pSession, strType, *(pSession->GetGags()));
	return TRUE;
}
BOOL CCommandTable::EditCurrentSession(CSession *pSession, CString &strFileName)
{
	CString strPath;
	CString dir, editor;
	dir = MMFileUtils::fixPath(INI_FILE_NAME);
	LPCTSTR lpszPathName = dir;
	editor = GET_ENTRY_STR(DEFAULT_SECTION_NAME,_T("Default Editor"),_T("notepad"));
	

	if(strFileName.IsEmpty())
	strFileName.Format("%s", pSession->GetOptions().sessionInfo_.ScriptFilename());
	MMFileUtils::getPersonalFolder(strPath);
	ShellExecute(NULL, NULL, editor, strFileName, strPath,SW_SHOW);;
	return TRUE;
}
BOOL CCommandTable::EditHighlight(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam1))
		return FALSE;


	if (strParam1.IsEmpty())
	{
		pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_SCRIPT_HIGHLIGHTS,0 );
		return TRUE;
	}

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
		pSession->SetEditText(strText);
	}
	else
		pSession->QueueMessage(CMessages::MM_HIGHLIGHT_MESSAGE, "Highlight not found.");
	return TRUE;
}

BOOL CCommandTable::EditMacro(CSession *pSession, CString &strLine)
{
	CString strMacro;
	if(!CParseUtils::FindStatement(pSession, strLine,strMacro))
		return FALSE;


	if (strMacro.IsEmpty())
		return TRUE;

	CMacro *pMac;
	int nIndex = atoi(strMacro);
	if (nIndex)
		pMac = (CMacro *)pSession->GetMacros()->GetAt(nIndex-1);
	else
		pMac = pSession->GetMacros()->FindKeyByName(strMacro);

	if (pMac != NULL)
	{
		CString strText;
		pMac->MapToCommand(strText);
		pSession->SetEditText(strText);
	}
	else
		pSession->QueueMessage(CMessages::MM_MACRO_MESSAGE, "Macro not found.");
	return TRUE;
}

BOOL CCommandTable::EditSub(CSession *pSession, CString &strLine)
{
	if (strLine.IsEmpty())
	{
		pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_SCRIPT_SUBSTITUTIONS,0 );
		return TRUE;
	}

	CString strType;
	strType = "{Subs} ";
	strType.Append(strLine);

	EditTrigger(pSession, strType, *(pSession->GetSubs()));
	return TRUE;
}

BOOL CCommandTable::EditVariable(CSession *pSession, CString &strLine)
{
	CString strVarName;
	if(!CParseUtils::FindStatement(pSession, strLine, strVarName))
		return FALSE;


	if (strVarName.IsEmpty())
		return TRUE;

	int nIndex = atoi(strVarName);
	if (nIndex)
	{
		if(nIndex < 0 || nIndex > pSession->GetVariables()->GetSize())
		{
			CString strFormat;
			if(!strFormat.LoadString(IDS_INVALID_VARIABLE_INDEX))
				strFormat = _T("Invalid variable index [%d]");

			CString strMessage;
			strMessage.Format(strFormat, nIndex);

			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, strMessage);
		}
		else
		{
			CVariable &v = pSession->GetVariables()->GetAt(nIndex - 1);
			CString strTemp;
			v.MapToCommand(strTemp);
			pSession->SetEditText(strTemp);
		}
	}
	else
	{
		CVariable v;

		std::string varName = static_cast<LPCTSTR>( strVarName );
		if( !pSession->GetVariables()->FindByName( varName, v ) )
		{
			CString strFormat;
			if(!strFormat.LoadString(IDS_VARNAME_NOT_FOUND))
				strFormat = _T("Variable name not found [%s]");

			CString strMessage;
			strMessage.Format(strFormat, strVarName);

			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, strMessage);
		}
		else
		{
			CString strTemp;
			v.MapToCommand(strTemp);
			pSession->SetEditText(strTemp);
		}
	}

	return TRUE;
}

BOOL CCommandTable::Emote(CSession *pSession, CString &strLine)
{
	CString strName;
	if(CParseUtils::FindStatement(pSession, strLine,strName))
	{
		CParseUtils::ReplaceVariables(pSession, strName);
		CParseUtils::ReplaceVariables(pSession, strLine);

		if(!strLine.IsEmpty() && !strName.IsEmpty())
		{
			pSession->GetChat().SendEmoteToPerson(strName, strLine);
		}
	}

	return TRUE;
}

BOOL CCommandTable::EmoteAll(CSession *pSession, CString &strLine)
{
	CParseUtils::ReplaceVariables(pSession, strLine);

	if (strLine.IsEmpty())
		return TRUE;

	pSession->GetChat().SendEmoteToEverybody(strLine);
	return TRUE;
}

BOOL CCommandTable::Empty(CSession *pSession, CString &strLine)
{
	CString variableName, groupName;
	if(!CParseUtils::FindStatement( pSession, strLine, variableName ) )
		return FALSE;

	if(!CParseUtils::FindStatement( pSession, strLine, groupName) )
		return FALSE;

	CParseUtils::ReplaceVariables( pSession, variableName );
	CParseUtils::ReplaceVariables( pSession, groupName );

	std::string name = static_cast<LPCTSTR>( variableName );
	std::string group = static_cast<LPCTSTR>( groupName );
	if( pSession->GetVariables()->Add( name, "", group ) )
	{
		pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, "Variable added.");
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, "Variable not added.");
	}

	return TRUE;
}


BOOL CCommandTable::EnableBarItem(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam);

	if (strParam.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strParam);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strParam);

	if (pItem != NULL)
	{
		pItem->Enabled(TRUE);
		if (pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Bar Item Enabled");
	}
	return TRUE;
}

BOOL CCommandTable::EnableEvent(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam1))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam1);

	if (strParam1.IsEmpty())
		return TRUE;

	CMMEvent *pEvent;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strParam1);

	if (pEvent)
	{
		pEvent->Enabled(TRUE);
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Event enabled.");
	}
	return TRUE;
}

BOOL CCommandTable::EnableGag(CSession *pSession, CString &strLine)
{
	EnableTrigger(pSession, strLine, *(pSession->GetGags()));
	return TRUE;
}

BOOL CCommandTable::EnableGroup(CSession *pSession, CString &strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (strGroup.IsEmpty())
		return TRUE;

	int nActions = pSession->GetActions()->EnableGroup( strGroup, true );
	int nAliases = pSession->GetAliases()->EnableGroup( strGroup, true );
	int nEvents = pSession->GetEvents()->EnableGroup(strGroup);
	int nMacros = pSession->GetMacros()->EnableGroup(strGroup);
	int nBarItems = pSession->GetStatusBar()->EnableGroup(strGroup);
	int nGags = pSession->GetGags()->EnableGroup(strGroup);
	int nHighs = pSession->GetHighlights()->EnableGroup(strGroup);
	int nSubs = pSession->GetSubs()->EnableGroup(strGroup);

	if(nBarItems > 0)
		pSession->RedrawStatusBar();

	if (pSession->GetOptions().messageOptions_.EnableMessages())
	{
		CString strText;
		strText.Format(	_T("Commands Enabled: Actions(%d), Aliases(%d), Bar Items(%d)\n")
						_T("                    Events(%d), Gags(%d), Highlights(%d), Macros(%d), Substitutions(%d)\n"),
						nActions,nAliases,0,nEvents,nGags,nHighs,nMacros, nSubs);

		pSession->PrintAnsiNoProcess(strText);
	}
	return TRUE;
}

BOOL CCommandTable::EnableHighlight(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;


	if (strParam.IsEmpty())
		return TRUE;

	CHigh *pHigh;
	int nIndex = atoi(strParam);
	if (nIndex)
		pHigh = (CHigh *)pSession->GetHighlights()->GetAt(nIndex-1);
	else
		pHigh = (CHigh *)pSession->GetHighlights()->FindExact(strParam);

	if (pHigh != NULL)
	{
		pHigh->Enabled(TRUE);
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Highlight Enabled");
	}
	return TRUE;
}

BOOL CCommandTable::EnableMacro(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession, strLine,strParam))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam);

	if (strParam.IsEmpty())
		return TRUE;

	BOOL bResult = pSession->GetMacros()->EnableMacro(strParam);
	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Macro enabled.");
	}
	return TRUE;
}

BOOL CCommandTable::EnableSub(CSession *pSession, CString &strLine)
{
	EnableTrigger(pSession, strLine, *(pSession->GetSubs()));
	return TRUE;
}

BOOL CCommandTable::Event(CSession *pSession, CString &strLine)
{
	CString strName, strSeconds, strEvent, strGroup;
	if(!CParseUtils::FindStatement(pSession, strLine,strName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strSeconds))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strEvent))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession, strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strName);
	CParseUtils::ReplaceVariables(pSession, strSeconds);
	CParseUtils::PretranslateString(pSession, strEvent);

	if (strName.IsEmpty() || strSeconds.IsEmpty() || strEvent.IsEmpty())
	{
		//list events
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Events:");
		CString strText;
		CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
		CString strMessage;
		while(pEvent != NULL)
		{
			CString strTemp;
			pEvent->MapToDisplayText(strTemp);
			strText.Format("%s%03d:%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetEvents()->GetFindIndex()+1,
				ANSI_RESET,
				pEvent->Enabled() ? ' ' : '*',
				strTemp);
			strMessage += strText;
			pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
		}
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	int nSeconds = atoi(strSeconds);
	if (nSeconds < 1)
		return TRUE;

	CMMEvent *pEvent = pSession->GetEvents()->Add(strName,nSeconds,strEvent,strGroup);
	if (pEvent != NULL)
	{
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Event added.");
	}
	else
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Unable to add event.");
	return TRUE;
}
