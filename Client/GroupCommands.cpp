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
#include "Action.h"
#include "Alias.h"
#include "Macro.h"
#include "Variable.h"
#include "Event.h"
#include "Sub.h"
#include "Gag.h"
#include "High.h"
#include "MMList.h"
#include "Array.h"
#include "BarItem.h"
#include "Tab.h"
#include "Dll.h"
#include "Sess.h"
#include "Colors.h"
#include "ActionList.h"
#include "ChatServer.h"
#include "AliasList.h"
#include "MacroList.h"
#include "VarList.h"
#include "EventList.h"
#include "SubList.h"
#include "GagList.h"
#include "HighList.h"
#include "UserLists.h"
#include "ArrayList.h"
#include "StatusBar.h"
#include "TabList.h"
#include "DllList.h"
#include "Globals.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MMSystem;
using namespace SerializedOptions;
using namespace Utilities;

BOOL CCommandTable::SendGroup(CSession *pSession, CString &strLine)
{
	CString strPerson, strGroup;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strGroup);

	if (strPerson.IsEmpty() || strGroup.IsEmpty())
		return TRUE;

	pSession->SendGroupToChat(
		static_cast<LPCTSTR>(strGroup), 
		static_cast<LPCTSTR>(strPerson));

	return TRUE;
}

BOOL CCommandTable::WriteTriggers(CStdioFile& file, CTriggerList& list)
{
	CTrigger *pTrigger = (CTrigger *)list.GetFirst();
	while(pTrigger != NULL)
	{
		CString strText;
		pTrigger->MapToCommand(strText);
		file.WriteString(strText+"\n");
		pTrigger = (CTrigger *)list.GetNext();
	}
	return TRUE;
}

BOOL CCommandTable::Write(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString strFilename;
		CParseUtils::FindStatement(pSession, strLine, strFilename);
		CParseUtils::ReplaceVariables(pSession, strFilename);

		CPath path;
		ErrorHandlingUtils::TESTHR(CGlobals::FixPath(path, strFilename));

		CString strPath = path.m_strPath;
		if(!CGlobals::ProtectFile(
			strPath, 
			pSession->GetOptions().sessionInfo_.PromptOverwrite()))
		{
			return TRUE;
		}


		CStdioFile file;
		CFileException fe;
		if (!file.Open(
			strPath,
			CFile::modeWrite|CFile::modeCreate|CFile::typeText,
			&fe))
		{
			CGlobals::OpenFileError(pSession, fe);
			return TRUE;
		}

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,CString("Writing script file: ")+strPath);

		CString strText;
		CString strTemp;
		CString strKey;
		CString strPrepare;

		pSession->GetActions()->WriteToFile( file );
		//WriteTriggers(file, *(pSession->GetActions()));

		pSession->GetAliases()->WriteToFile(
			CGlobals::GetCommandCharacter(), file);

		int nMacros = 0;
		CMacro *pMac = (CMacro *)pSession->GetMacros()->GetFirst();
		while(pMac != NULL)
		{
			CString strText;
			pMac->MapToCommand(strText);
			file.WriteString(strText+"\n");
			nMacros++;
			pMac = (CMacro *)pSession->GetMacros()->GetNext();
		}

		CTab *pTab = (CTab *)pSession->GetTabList()->GetFirst();
		while(pTab != NULL)
		{
			pTab->MapToCommand(strText);
			file.WriteString(strText+"\n");
			pTab = (CTab *)pSession->GetTabList()->GetNext();
		}

		for(int i = 0; i < pSession->GetVariables()->GetSize(); ++i)
		{
			CVariable &v = pSession->GetVariables()->GetAt(i);
			v.MapToCommand(strTemp);
			file.WriteString(strTemp+"\n");
		}

		CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
		while(pEvent != NULL)
		{
			pEvent->MapToCommand(strText);
			file.WriteString(strText+"\n");
			pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
		}

		WriteTriggers(file, *(pSession->GetGags()));
		WriteTriggers(file, *(pSession->GetSubs()));

		CString strColor;
		CHigh *pHigh = (CHigh *)pSession->GetHighlights()->GetFirst();
		while(pHigh != NULL)
		{
			pHigh->MapToCommand(strText);
			file.WriteString(strText+"\n");
			pHigh = (CHigh *)pSession->GetHighlights()->GetNext();
		}

		CMMList *pList = (CMMList *)pSession->GetLists()->GetFirst();
		while(pList != NULL)
		{
			pList->MapToCommand(strText);
			file.WriteString(strText+"\n");

			// Write the items in the list out.
			int nCount = pList->Items().GetCount();
			for(int i = 0; i < nCount; ++i)
			{
				CString listItem = pList->Items().GetAt(i);
				CGlobals::PrepareForWrite(listItem, strPrepare);
				strText.Format(_T("%citemadd {%s} {%s}\n"),
					CGlobals::GetCommandCharacter(),
					(LPCSTR)pList->ListName(),
					(LPCSTR)strPrepare);
				file.WriteString(strText);
			}
			pList = (CMMList *)pSession->GetLists()->GetNext();
		}

		pSession->GetDlls().write(file);

		CMMArray *ptr = (CMMArray *)pSession->GetArrays()->GetFirst();
		while(ptr != NULL)
		{
			ptr->MapToCommand(strText);
			file.WriteString(strText+"\n");

			// Write out the items.
			if (ptr->SingleDim())
			{
				for (int i=1;i<=ptr->Dim(0);i++)
				{
					pSession->GetArrays()->GetItem(ptr,i,0,strTemp);
					CGlobals::PrepareForWrite(strTemp,strPrepare);
					if (!strPrepare.IsEmpty())
					{
						strText.Format(_T("%cassign {%s} {%d} {%s}\n"),
							CGlobals::GetCommandCharacter(),
							(LPCSTR)ptr->Name(),
							i,
							(LPCSTR)strPrepare);
						file.WriteString(strText);
					}
					else
					{
						/*strText.Format("%cassign {%s} {%d} {}\n",
							CGlobals::GetCommandCharacter(),
							(LPCSTR)ptr->Name(),
							i);
						file.WriteString(strText);*/
					}
				}
			}
			else
			{
				int j;
				for (int i=1;i<=ptr->Dim(0);i++)
					for (j=1;j<=ptr->Dim(1);j++)
					{
						pSession->GetArrays()->GetItem(ptr,i,j,strTemp);
						CGlobals::PrepareForWrite(strTemp,strPrepare);
						if (!strPrepare.IsEmpty())
						{
							strText.Format(_T("%cassign {%s} {%d,%d} {%s}\n"),
								CGlobals::GetCommandCharacter(),
								(LPCSTR)ptr->Name(),
								i,j,
								(LPCSTR)strPrepare);
							file.WriteString(strText);
						}
						else
						{
							/*strText.Format("%cassign {%s} {%d,%d} {}\n",
								CGlobals::GetCommandCharacter(),
								(LPCSTR)ptr->Name(),
								i, j);
							file.WriteString(strText);*/
						}
					}
			}

			ptr = (CMMArray *)pSession->GetArrays()->GetNext();
		}

		CBarItem *pItem = (CBarItem *)pSession->GetStatusBar()->GetFirst();
		while(pItem != NULL)
		{
			pItem->MapToCommand(strText);
			file.WriteString(strText+"\n");
			pItem = (CBarItem *)pSession->GetStatusBar()->GetNext();
		}

		strText.Format("%csubstitutedepth %d\n", CGlobals::GetCommandCharacter(), pSession->GetOptions().sessionInfo_.SubDepth());
		file.WriteString(strText);

		for(POSITION pos = pSession->GetAutoList().GetHeadPosition();pos;)
		{
			CString str = pSession->GetAutoList().GetNext(pos);
			CString strCommand;
			strCommand.Format("%cautoexec {%s}\n", CGlobals::GetCommandCharacter(), (LPCSTR)str);
			file.WriteString(strCommand);
		}

		if(pSession->GetOptions().mspOptions_.On())
		{
			//TODO: correct command is now /msp gather not /msp on
			strText.Format("%cmsp gather\n", CGlobals::GetCommandCharacter());
			file.WriteString(strText);
			if(pSession->GetOptions().mspOptions_.ProcessFX())
			{
				strText.Format("%cprocessfx on\n", CGlobals::GetCommandCharacter());
				file.WriteString(strText);
			}

			if(pSession->GetOptions().mspOptions_.ProcessMidi())
			{
				strText.Format("%cprocessmidi on\n", CGlobals::GetCommandCharacter());
				file.WriteString(strText);
			}

			if(pSession->GetOptions().mspOptions_.ShowMSPTriggers())
			{
				strText.Format("%cshowmsptriggers on\n", CGlobals::GetCommandCharacter());
				file.WriteString(strText);
			}
		}

		if(pSession->GetOptions().mspOptions_.UsePlaySound())
		{
			strText.Format("%cuseplaysound on\n", CGlobals::GetCommandCharacter());
			file.WriteString(strText);
		}

		// The status bar defaults to ON.
		// /statusbar is a toggle command
		// so if the statusbar is off...then add the toggle
		// to the command file so it will be toggled
		// the next time the script is read
		if(!pSession->GetOptions().statusOptions_.IsVisible())
		{
			strText.Format("%cstatusbar", CGlobals::GetCommandCharacter());
			file.WriteString(strText);
		}

		// the status bar defaults to a position below the input bar
		// if the status bar position is above the input bar we'll
		// write out a command to the command file to put it above
		// the next time the script file is read
		if(pSession->GetOptions().statusOptions_.GetStatusbarPos() == 
			CStatusBarOptions::STATUS_POS_ABOVE)
		{
			strText.Format("%cstatusbar {above}", CGlobals::GetCommandCharacter());
			file.WriteString(strText);
		}

		file.Close();
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Script file written."); 
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return TRUE;
}

int CCommandTable::WriteTriggersGroup(CStdioFile& file, const CString& strGroup, CTriggerList& list)
{
	CTrigger *pTrigger = (CTrigger *)list.GetFirst();
	int nTriggers = 0;
	while(pTrigger != NULL)
	{
		if (pTrigger->Group() == strGroup)
		{
			CString strText;
			pTrigger->MapToCommand(strText);
			file.WriteString(strText+"\n");
			nTriggers++;
		}
		pTrigger = (CTrigger *)list.GetNext();
	}
	return nTriggers;
}

BOOL CCommandTable::WriteGroup(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CString strFilename, strGroup;
		if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
			return FALSE;

		if(!CParseUtils::FindStatement(pSession,  strLine,strGroup))
			return FALSE;

		CParseUtils::ReplaceVariables(pSession, strFilename);
		CParseUtils::ReplaceVariables(pSession, strGroup);

		CPath path;
		ErrorHandlingUtils::TESTHR(CGlobals::FixPath(path, strFilename));

		strFilename = path.m_strPath;
		CStdioFile file;
		if(!CGlobals::ProtectFile(
			strFilename, 
			pSession->GetOptions().sessionInfo_.PromptOverwrite()))
		{
			return TRUE;
		}

		CFileException fe;
		if (!file.Open(
			strFilename,
			CFile::modeWrite|CFile::modeCreate|CFile::typeText,
			&fe))
		{
			CGlobals::OpenFileError(pSession, fe);
			return TRUE;
		}

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, CString("Writing command file: ") + strFilename);

		CString strText;
		CString strTemp;
		CString strKey;
		CString strPrepare;

		int nActions = pSession->GetActions()->WriteGroupToFile( file, strGroup );
		//int nActions = WriteTriggersGroup(file, strGroup, *(pSession->GetActions()));

		int nAliases = pSession->GetAliases()->WriteGroupToFile(
			CGlobals::GetCommandCharacter(), file, strGroup);

		int nMacros = 0;
		CMacro *pMac = (CMacro *)pSession->GetMacros()->GetFirst();
		while(pMac != NULL)
		{
			if (pMac->Group() == strGroup)
			{
				CString strText;
				pMac->MapToCommand(strText);
				file.WriteString(strText+"\n");
				nMacros++;
			}
			pMac = (CMacro *)pSession->GetMacros()->GetNext();
		}

		int nTabList = 0;
		CTab *pTab = (CTab *)pSession->GetTabList()->GetFirst();
		while(pTab != NULL)
		{
			if (pTab->Group() == strGroup)
			{
				pTab->MapToCommand(strText);
				file.WriteString(strText+"\n");
				nTabList++;
			}
			pTab = (CTab *)pSession->GetTabList()->GetNext();
		}

		int nVarsWritten = 0;
		for(int i = 0; i < pSession->GetVariables()->GetSize(); ++i)
		{
			CVariable v = pSession->GetVariables()->GetAt(i);
			if( v.GetGroup() == static_cast<LPCTSTR>( strGroup ) )
			{
				v.MapToCommand(strTemp);
				file.WriteString(strTemp+"\n");
				++nVarsWritten;
			}
		}

		int nEvents = 0;
		CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
		while(pEvent != NULL)
		{
			if (pEvent->Group() == strGroup)
			{
				pEvent->MapToCommand(strText);
				file.WriteString(strText+"\n");
				nEvents++;
			}
			pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
		}

		int nSubs = WriteTriggersGroup(file, strGroup, *(pSession->GetSubs()));
		int nGags = WriteTriggersGroup(file, strGroup, *(pSession->GetGags()));

		CString strColor;
		int nHighs = 0;
		CHigh *pHigh = (CHigh *)pSession->GetHighlights()->GetFirst();
		while(pHigh != NULL)
		{
			if (pHigh->Group() == strGroup)
			{
				pHigh->MapToCommand(strText);
				file.WriteString(strText+"\n");
				nHighs++;
			}
			pHigh = (CHigh *)pSession->GetHighlights()->GetNext();
		}

		int nLists = 0;
		CMMList *pList = (CMMList *)pSession->GetLists()->GetFirst();
		while(pList != NULL)
		{
			if (pList->Group() == strGroup)
			{
				pList->MapToCommand(strText);
				file.WriteString(strText+"\n");


				int nCount = pList->Items().GetCount();
				for(int i = 0; i < nCount; ++i)
				{
					CString listItem = pList->Items().GetAt(i);
					CGlobals::PrepareForWrite(listItem, strPrepare);
					strText.Format("%citemadd {%s} {%s}\n",
						CGlobals::GetCommandCharacter(),
						(LPCSTR)pList->ListName(),
						(LPCSTR)strPrepare);
					file.WriteString(strText);
				}
				nLists++;
			}

			pList = (CMMList *)pSession->GetLists()->GetNext();
		}

		int nArrays = 0;
		CMMArray *ptr = (CMMArray *)pSession->GetArrays()->GetFirst();
		while(ptr != NULL)
		{
			if (ptr->Group() == strGroup)
			{
				ptr->MapToCommand(strText);
				file.WriteString(strText+"\n");

				// Write out the items.
				if (ptr->SingleDim())
				{
					for (int i=1;i<=ptr->Dim(0);i++)
					{
						pSession->GetArrays()->GetItem(ptr,i,0,strTemp);
						CGlobals::PrepareForWrite(strTemp,strPrepare);
						if (!strPrepare.IsEmpty())
						{
							strText.Format(_T("%cassign {%s} {%d} {%s}\n"),
								CGlobals::GetCommandCharacter(),
								(LPCSTR)ptr->Name(),
								i,
								(LPCSTR)strPrepare);

							file.WriteString(strText);
						}
					}
				}
				else
				{
					int j;
					for (int i=1;i<=ptr->Dim(0);i++)
						for (j=1;j<=ptr->Dim(1);j++)
						{
							pSession->GetArrays()->GetItem(ptr,i,j,strTemp);
							CGlobals::PrepareForWrite(strTemp,strPrepare);
							if (!strPrepare.IsEmpty())
							{
								strText.Format(_T("%cassign {%s} {%d,%d} {%s}\n"),
									CGlobals::GetCommandCharacter(),
									(LPCSTR)ptr->Name(),
									i,j,
									(LPCSTR)strPrepare);
								file.WriteString(strText);
							}
						}
				}
				nArrays++;
			}

			ptr = (CMMArray *)pSession->GetArrays()->GetNext();
		}

		int nBarItems = 0;
		CBarItem *pItem = (CBarItem *)pSession->GetStatusBar()->GetFirst();
		while(pItem != NULL)
		{
			if (pItem->Group() == strGroup)
			{
				pItem->MapToCommand(strText);
				file.WriteString(strText+"\n");
				nBarItems++;
			}
			pItem = (CBarItem *)pSession->GetStatusBar()->GetNext();
		}

		file.Close();
		strText.Format(	_T("Commands Written: Actions(%d), Aliases(%d), Arrays(%d), Bar Items(%d)\n")
						_T("                    Events(%d), Subs(%d), Gags(%d), Highlights(%d), Lists(%d)\n")
						_T("                    Macros(%d), Tab Words(%d), Variables(%d)"),
						nActions,nAliases,nArrays,nBarItems,nEvents,nSubs, nGags,nHighs,
						nLists,nMacros,nTabList,
						nVarsWritten);

		pSession->PrintAnsiNoProcess(strText);
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return TRUE;
}