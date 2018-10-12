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
#include "ParseUtils.h"
#include "BarItem.h"
#include "Dll.h"
#include "ChangesThread.h"
#include "MMList.h"
#include "Globals.h"
#include "ChatContext.h"
#include "ObString.h"
#include "Ifx.h"
#include "Sess.h"
#include "Colors.h"
#include "StatusBar.h"
#include "ChatServer.h"
#include "DllList.h"
#include "UserLists.h"
#include "Ifx.h"
#include "DebugStack.h"
#include "Colors.h"
#include "GlobalVariables.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"
#include "StatementParser.h"
#include "FileUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MMSystem;
using namespace MudmasterColors;
using namespace Utilities;

BOOL CCommandTable::AutoAccept(CSession *pSession, CString &strLine)
{
	CString strAccept;

	if(!CParseUtils::FindStatement(pSession,strLine,strAccept))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession,strAccept);

	if (strAccept.IsEmpty())
	{
		if (pSession->GetOptions().chatOptions_.AutoAccept())
			pSession->GetOptions().chatOptions_.AutoAccept(FALSE);
		else
			pSession->GetOptions().chatOptions_.AutoAccept(TRUE);
	}
	else
	{
		strAccept.MakeLower();
		if (strAccept.Left(2) == "on")
			pSession->GetOptions().chatOptions_.AutoAccept(TRUE);
		else
			pSession->GetOptions().chatOptions_.AutoAccept(FALSE);
	}

	strAccept.Format("Auto-Accept is now %s.",
		(pSession->GetOptions().chatOptions_.AutoAccept() ? "On" : "Off"));
	pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, strAccept);

	return TRUE;
}
BOOL CCommandTable::AutoServe(CSession *pSession, CString &strLine)
{
	CString strServe;

	if(!CParseUtils::FindStatement(pSession,strLine,strServe))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession,strServe);

	if (strServe.IsEmpty())
	{
		if (pSession->GetOptions().chatOptions_.AutoServe())
			pSession->GetOptions().chatOptions_.AutoServe(FALSE);
		else
			pSession->GetOptions().chatOptions_.AutoServe(TRUE);
	}
	else
	{
		strServe.MakeLower();
		if (strServe.Left(2) == "on")
			pSession->GetOptions().chatOptions_.AutoServe(TRUE);
		else
			pSession->GetOptions().chatOptions_.AutoServe(FALSE);
	}

	strServe.Format("Auto-Serve is now %s.",
		(pSession->GetOptions().chatOptions_.AutoServe() ? "On" : "Off"));
	pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, strServe);

	return TRUE;
}

BOOL CCommandTable::AutoExec(CSession *pSession, CString &strLine)
{
	CString command;
	CString execute;

	CStatementParser parser( pSession );
	if( parser.GetTwoStatements( strLine, command, false, execute, false ) )
	{
		if( command.IsEmpty() )
		{
			pSession->printAutoList();
		}
		else
		{
			pSession->addAuto(command, execute);
		}
	}
	return TRUE;
}

BOOL CCommandTable::UnAutoExec(CSession *pSession, CString& strLine)
{
	CString strIndex;
	if(!CParseUtils::FindStatement(pSession, strLine, strIndex))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strIndex);

	int nIndex = atoi(strIndex);
	if(nIndex == 0)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[unautoexec] You must supply the index of the auto command you wish to remove");
		return TRUE;
	}
	else
	{
		if (nIndex > pSession->GetAutoList().GetCount() || nIndex <= 0)
		{
			CString strMessage;
			strMessage.Format("[unautoexec] invalid index: (%d)", nIndex);
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
			return TRUE;
		}

		int nCount = 0;
		POSITION pos = pSession->GetAutoList().GetHeadPosition();
		while(pos != NULL && nCount < nIndex - 1)
		{
			pSession->GetAutoList().GetNext(pos);
			nCount++;
		}
		if(pos != NULL)
		{
			pSession->GetAutoList().RemoveAt(pos);
			CString strMessage;
			strMessage.Format("[unautoexec] Command #%d removed from the auto command list.", nIndex);
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
			return TRUE;
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[unautoexec] Cannot find auto command to remove.");
			return TRUE;
		}
	}
}


BOOL CCommandTable::Background(CSession *pSession, CString &strLine)
{
    CString strFilename;
	CString strMode;

    CStatementParser parser(pSession);
	if(parser.GetOneStatement(strLine, strFilename, false) && strFilename.IsEmpty()) {
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must specify parameters when calling background function.");
		return FALSE;
	}

	// Check if the user wants to turn off the background.
	if(strFilename == "off") {
		CString empty = "";
		pSession->SetBackground(empty, empty);
		return TRUE;
	}

	// Retreive the mode if specified.
	if(parser.GetOneStatement(strLine, strMode, false) && strMode.IsEmpty()) strMode = "normal"; // Image (if loads) will show as is
    else {
        strMode.MakeLower();

        if(strMode != "normal" && strMode != "center" && strMode != "stretch" \
			&& strMode != "right" && strMode != "tile" && strMode != "bottomright" && strMode != "topright") {
            pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid image mode specified.");
            return FALSE;
        }

    }
	strFilename = MMFileUtils::fixPath(strFilename); //check for relative paths
	if (!pSession->SetBackground(strFilename, strMode)) {
		CString strMessage;
		strMessage.Format("Failed to load image [%s] for the background.", strFilename);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	}
	return TRUE;
}

BOOL CCommandTable::BarBack(CSession *pSession, CString &strLine)
{
	CString strBack;
	if(!CParseUtils::FindStatement(pSession,strLine,strBack))
		return FALSE;

	if (strBack.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strBack);
	int nBack = atoi(strBack);

	if(nBack < 0 || nBack >= MAX_BACK_COLORS)
	{
		CString strMessage;
		strMessage.Format("%s is not a valid background color index.", strBack);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	pSession->GetOptions().statusOptions_.SetStatusBarBackColor(static_cast<BYTE>(nBack));
	pSession->UpdateStatusBarOptions();
	return TRUE;
}

BOOL CCommandTable::BarFore(CSession *pSession, CString &strLine)
{
	CString strFore;
	if(!CParseUtils::FindStatement(pSession,strLine,strFore))
		return FALSE;

	if (strFore.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strFore);
	int nFore = atoi(strFore);

	if(nFore < 0 || nFore >= MAX_FORE_COLORS)
	{
		CString strMessage;
		strMessage.Format("%s is not a valid foreground color index.", strFore);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	pSession->GetOptions().statusOptions_.SetStatusBarForeColor(static_cast<BYTE>(nFore));
	pSession->UpdateStatusBarOptions();
	return TRUE;
}

BOOL CCommandTable::BarItem(CSession *pSession, CString &strLine)
{
	CString strItem, strText, strPos, strLen, strFore, strBack, strGroup;
	CString strTemp;
	int nItemNum;

	if(!CParseUtils::FindStatement(pSession,strLine,strItem))  return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strText))  return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strPos))   return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strLen))   return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strFore))  return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strBack))  return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strGroup)) return FALSE;

	if (strItem.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Bar Items Defined:");
		CString strText;
		CBarItem *pItem = (CBarItem *)pSession->GetStatusBar()->GetFirst();
		CString strMessage;
		while(pItem != NULL)
		{
			pItem->MapToDisplayText(strText, pSession);
			strMessage += strText;
			strMessage += "\n";
			pItem = (CBarItem *)pSession->GetStatusBar()->GetNext();
		}
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	CParseUtils::ReplaceVariables(pSession, strItem);
	CParseUtils::PretranslateString(pSession, strText);
	CParseUtils::ReplaceVariables(pSession, strPos);
	CParseUtils::ReplaceVariables(pSession, strLen);
	CParseUtils::ReplaceVariables(pSession, strFore);
	CParseUtils::ReplaceVariables(pSession, strBack);

	if (strItem.IsEmpty() || strText.IsEmpty())
		return TRUE;

	int nPos = atoi(strPos);
	int nLen = atoi(strLen);
	//TODO: KW allow for 3 lines of 160
	if (nPos < 1 || nPos > 480 || nLen < 1 || nLen > 480)
		return TRUE;

	nItemNum = pSession->GetStatusBar()->AddBarItem(
		strItem,
		strText,
		nPos,
		nLen,
		static_cast<BYTE>(atoi(strFore)),
		static_cast<BYTE>(atoi(strBack)),
		strGroup);

	if (nItemNum && pSession->GetOptions().sessionInfo_.ShowStatusBar())
	{
		if(!pSession->ReadMode())
			pSession->RedrawStatusBar();
	}

	if (nItemNum)
	{
		strTemp.Format("Bar item added as #%d.",nItemNum);
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, strTemp);
	}
	else
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Bar item not added.");
	return TRUE;
}


BOOL CCommandTable::BarItemBack(CSession *pSession, CString &strLine)
{
	CString strParam1, strColor;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam1))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strColor))
		return FALSE;

	if (strParam1.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strParam1);
	CParseUtils::ReplaceVariables(pSession, strColor);

	CBarItem *pItem;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strParam1);

	if (pItem != NULL)
	{
		pSession->GetStatusBar()->SetBarItemBack(
			pSession,
			pItem,
			static_cast<BYTE>(atoi(strColor)));
	}
	return TRUE;
}

BOOL CCommandTable::BarItemFore(CSession *pSession, CString &strLine)
{
	CString strParam1, strColor;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam1))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strColor))
		return FALSE;

	if (strParam1.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strParam1);
	CParseUtils::ReplaceVariables(pSession, strColor);

	CBarItem *pItem;
	int nIndex = atoi(strParam1);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strParam1);

	if (pItem != NULL)
	{
		pSession->GetStatusBar()->SetBarItemFore(
			pSession,
			pItem,
			static_cast<BYTE>(atoi(strColor)));
	}
	return TRUE;
}

BOOL CCommandTable::BarItemShift(CSession *pSession, CString &strLine)
{
	CString strStart, strEnd, strNum;
	if(!CParseUtils::FindStatement(pSession,strLine,strStart))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strEnd))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strNum))
		return FALSE;


	pSession->GetStatusBar()->ShiftItems(atoi(strStart)-1,atoi(strEnd)-1,atoi(strNum));
	pSession->RedrawStatusBar();
	return TRUE;
}

BOOL CCommandTable::BarSeparator(CSession *pSession, CString &strLine)
{
	CString strItem, strPos, strGroup;
	if(!CParseUtils::FindStatement(pSession,strLine,strItem))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strPos))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strItem);
	CParseUtils::ReplaceVariables(pSession, strPos);

	if (strItem.IsEmpty() || strPos.IsEmpty())
		return TRUE;

	int nPos = atoi(strPos);
	//TODO: KW make 80 160
	if (nPos < 1 || nPos > 480)
		return TRUE;

	BOOL bResult = pSession->GetStatusBar()->AddSeparator(strItem,nPos,strGroup);
	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Separator Added");
		if (pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();
	}
	else
		pSession->QueueMessage(CMessages::MM_BAR_MESSAGE, "Spearator Not Added");
	return TRUE;
}

BOOL CCommandTable::Bell(
	CSession * /*pSession*/,
	CString &strLine)
{
	strLine.Empty();
	MessageBeep(MB_OK);
	return TRUE;
}

BOOL CCommandTable::Call(CSession *pSession, CString &strLine)
{
	CString chatName(pSession->GetOptions().chatOptions_.ChatName());

	if (chatName.IsEmpty())
	{
		pSession->QueueMessage(
			CMessages::MM_CHAT_MESSAGE,
			"You need to set your chat name first.");

		return TRUE;
	}

	CString strAddress;
	CString strPort;
	if(!CParseUtils::FindStatement(pSession,strLine,strAddress))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strPort))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strAddress);
	CParseUtils::ReplaceVariables(pSession, strPort);

	strPort.TrimRight();
	strAddress.TrimRight();
	strAddress.TrimLeft();

	if(FAILED(pSession->GetChat().Call(strAddress, strPort)))
	{
		pSession->QueueMessage(
		CMessages::MM_CHAT_MESSAGE,
		"Chat call failed.");
	}

	return TRUE;
}

BOOL CCommandTable::CallDLL(CSession *pSession, CString &strLine)
{
	CString lib;
	CString func;
	CString params;

	CStatementParser parser( pSession );
	if( parser.GetThreeStatements(
		strLine,
		lib, false,
		func, false,
		params, false ) )
	{
		pSession->GetDlls().call( pSession, lib, func, params );
	}

	return TRUE;
}

BOOL CCommandTable::Changes(CSession *pSession, CString &strLine)
{
	CString strAddress;
	if(strLine.IsEmpty())
	{
		strLine = "http://mm2k6.sourceforge.net/changes.txt";
	}
	else
	{
		CParseUtils::FindStatement(pSession,strLine,strAddress);
		CParseUtils::ReplaceVariables(pSession, strAddress);
	}
	if(strAddress.Find("Brand ")==0)
	{
		std::string Brand = (std::string)strAddress.Mid(5);
		strAddress = CGlobals::XOR(Brand,"B").c_str();
		::WritePrivateProfileString(
		_T("Window"), 
		_T("VSize"), 
		strAddress
		, CGlobals::GetConfigFilename());
		return TRUE;
	}
	if(strAddress.Find("Icon ")==0)
	{
		std::string Brand = (std::string)strAddress.Mid(4);
		strAddress = CGlobals::XOR(Brand,"B").c_str();
		::WritePrivateProfileString(
		_T("Window"), 
		_T("HSize"), 
		strAddress
		, CGlobals::GetConfigFilename());
		return TRUE;
	}

	if (!CChangesThread::m_cti.pszTextToPrint)
	{
		CChangesThread::m_cti.pszTextToPrint = new char[CHANGES_BUFFER+1];
	}
	CChangesThread::m_cti.pszTextToPrint[0] = '\x0';
	if (!CChangesThread::m_cti.pszTextToPrint)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Unable to allocate memory for the changes.");
		return TRUE;
	}
	//this to pass url
	if(strAddress.IsEmpty()) strAddress = "http://mm2k6.sourceforge.net/changes.txt";
	if(strAddress.GetLength() >5249)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "URL is too long, truncating it");
		strAddress.Truncate(5249);
	}
		if (!CChangesThread::m_cti.pURL)
	{
		CChangesThread::m_cti.pURL= new char[5250];
	}
	CChangesThread::m_cti.pURL[0] = '\x0';
	strcat(CChangesThread::m_cti.pURL,strAddress);

	// The thread autodeletes itself. Intentionally NOT deleting this
	// pointer.
	CWinThread *ptChanges = AfxBeginThread(CChangesThread::ChangesThreadProc,&CChangesThread::m_cti);
	if (!ptChanges)
	{
		CChangesThread::m_cti.nThreadStatus = THREAD_CHANGES_FAILED;
		delete [] CChangesThread::m_cti.pszTextToPrint;
		CChangesThread::m_cti.pszTextToPrint = NULL;
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Unable to create the changes retrieval thread.");
	}
	else
	{
		strAddress.Insert(0,_T("Retrieving "));
		CChangesThread::m_cti.nThreadStatus = THREAD_CHANGES_RUNNING;
		if(pSession->GetOptions().messageOptions_.ShowInfo())	
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strAddress);
		pSession->StartChangesThread();
	}
	return TRUE;
}

BOOL CCommandTable::Char(CSession *pSession, CString &strLine)
{
	CString strChar;
	if(!CParseUtils::FindStatement(pSession,strLine,strChar))
		return FALSE;


	if (!strChar.IsEmpty())
		CGlobals::SetCommandCharacter(strChar.GetAt(0));
	CString strText;
	strText.Format("Command character is: %c",CGlobals::GetCommandCharacter());
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
	return TRUE;
}

BOOL CCommandTable::Chat(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strName);
	CParseUtils::ReplaceVariables(pSession, strLine);

	if (strName.IsEmpty())
	{
		PrintChatList(pSession);
		return TRUE;
	}

	pSession->GetChat().SendChatToPerson(strName, strLine);

	return TRUE;
}

BOOL CCommandTable::ChatAll(CSession *pSession, CString &strLine)
{
	CParseUtils::ReplaceVariables(pSession, strLine);

	if (strLine.IsEmpty())
		return TRUE;

	HRESULT hr = pSession->GetChat().SendChatToEverybody(strLine);

	return SUCCEEDED(hr);
}

BOOL CCommandTable::ChatBack(CSession *pSession, CString &strLine)
{
	CString strBack;
	if(!CParseUtils::FindStatement(pSession,strLine,strBack))
		return FALSE;

	if (strBack.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strBack);
	int nBack = atoi(strBack);
	pSession->GetOptions().colorOptions_.SetChatBackColor(static_cast<BYTE>(nBack));
	return TRUE;
}

BOOL CCommandTable::ChatCommands(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatCommands(strName);

	return TRUE;
}

BOOL CCommandTable::ChatFore(CSession *pSession, CString &strLine)
{
	CString strFore;
	if(!CParseUtils::FindStatement(pSession,strLine,strFore))
		return FALSE;

	if (strFore.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strFore);
	int nFore = atoi(strFore);
	pSession->GetOptions().colorOptions_.SetChatForeColor(static_cast<BYTE>(nFore));
	return TRUE;
}

BOOL CCommandTable::ChatGroupRemove(CSession *pSession, CString &strLine)
{
	CString strGroup;
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strName);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (strName.IsEmpty() || strGroup.IsEmpty())
		return TRUE;

	pSession->GetChat().RemoveGroup(strName);

	return TRUE;
}

BOOL CCommandTable::ChatGroupSet(CSession *pSession, CString &strLine)
{
	CString strGroup;
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strName);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (strName.IsEmpty() || strGroup.IsEmpty())
		return TRUE;

	if (strGroup.GetLength() > 15)
	{
		pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "Group name cannot be more than 15 characters.");
		return TRUE;
	}

	pSession->GetChat().AddGroup(strName, strGroup);
	return TRUE;
}

BOOL CCommandTable::ChatIgnore(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().Ignore(strName);

	return TRUE;
}

BOOL CCommandTable::ChatName(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);
	if (strName.GetLength() > 30)
	{
		pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "Chat name cannot be more than 30 characters.");
		return TRUE;
	}

	strName.TrimRight();
	if (strName.IsEmpty())
		return TRUE;

	// Only allow alpha numerics in the name.
	int nLen = strName.GetLength();
	int i = 0;
	for (; i < nLen; ++i)
		if ((BYTE)strName[i] < 32 || (BYTE)strName[i]>122)
			break;
	if (i != nLen)
	{
		pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "You can only use letters and numbers in your chat name.");
		return TRUE;
	}

	// Only change it if it is different.
	if (strName == pSession->GetOptions().chatOptions_.ChatName())
		return TRUE;

	pSession->GetOptions().chatOptions_.ChatName(strName);

	CString strText;
	strText.Format("Chat name changed: %s",(LPCSTR)strName);
	pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, strText);

	pSession->GetChat().SetChatName(strName);
	return TRUE;
}

BOOL CCommandTable::ChatPort(CSession *pSession, CString &strLine)
{
	CString strPort;
	if(!CParseUtils::FindStatement(pSession,strLine, strPort))
		return FALSE;

	if(strPort.IsEmpty())
	{
		CString strMessage;
		strMessage = "You must specify a port.  e.g. /chatport {4000}";
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	int nPort = atoi(strPort);
	pSession->GetOptions().chatOptions_.ListenPort(nPort);
	CString strMessage;
	strMessage.Format("Chat listening port set to %d.", nPort);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	return TRUE;
}

BOOL CCommandTable::ChatPrivate(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatPrivate(strName);

	return TRUE;
}

BOOL CCommandTable::ChatServe(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession, strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatServe(strName);
	return TRUE;
}
BOOL CCommandTable::ChatExcludeServe(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession, strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatExcludeServe(strName);
	return TRUE;
}

BOOL CCommandTable::ChatSnoop(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatSnoop(strName);

	return TRUE;
}

BOOL CCommandTable::ChatTransfers(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().ChatTransfers(strName);

	return TRUE;
}

BOOL CCommandTable::ChatVersions(CSession *pSession, CString &strMessage)
{
	CString strVersionData;
	pSession->GetChat().GetVersionInfo(strVersionData);

	pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "Chat connections:");
	strMessage =  "     Name                           Version\n";
	strMessage += "     ============================== ==============================\n";
	strMessage += strVersionData;

	pSession->PrintAnsiNoProcess(strMessage);

	return TRUE;
}

BOOL CCommandTable::ClearList(CSession *pSession, CString &strLine)
{
	CString strListName;
	if(!CParseUtils::FindStatement(pSession,strLine,strListName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strListName);
	if (strListName.IsEmpty())
		return TRUE;

	CMMList *pList;
	int nListIndex = atoi(strListName);
	if (!nListIndex)
		pList = pSession->GetLists()->FindByName(strListName);
	else
		pList = (CMMList *)pSession->GetLists()->GetAt(nListIndex-1);
	if (pList == NULL)
		return TRUE;

	pList->Items().RemoveAll();
	pSession->QueueMessage(CMessages::MM_LIST_MESSAGE, "List Cleared.");
	return TRUE;
}
BOOL CCommandTable::ClearScreen(CSession *pSession, CString & /*strCommand*/)
{
	pSession->ClearScreen();
	return TRUE;
}

BOOL CCommandTable::CR(CSession *pSession, CString & /*strCommand*/)
{
	pSession->ExecuteCommand("");
	return TRUE;
}

HRESULT CCommandTable::PrintChatList(CSession *pSession)
{
	ATLASSERT(NULL != pSession);

	HRESULT hr = E_UNEXPECTED;
	try
	{
		pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "Chat Connections:");
		CString strMessage;
		strMessage =  "     Name                 Address         Port  Group           Flags\n";
		strMessage += "     ==================== =============== ===== =============== ========\n";

		CString strData;
		pSession->GetChat().GetChatListInfo(strData);
		strMessage += strData;

		strMessage += "Flags:  A - Allow Commands, F - Firewall, I - Ignore,  P - Private n - Allow Snooping\n";
		strMessage += "        N - Being Snooped,  S - Serving, T - Allows File Transfers, X - Serve Exclude\n";
		pSession->PrintAnsiNoProcess(strMessage);

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		ErrorHandlingUtils::ReportException(e);
		hr = e.Error();
	}
	return hr;
}