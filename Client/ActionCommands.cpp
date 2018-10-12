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
#include "Ifx.h"
#include "Sess.h"
#include "Action.h"
#include "Globals.h"
#include "ObString.h"
#include "Colors.h"
#include "ActionList.h"
#include "GroupList.h"
#include "ChatServer.h"
#include "Array.h"
#include "High.h"
#include "Undo.h"
#include <sstream>
#include "GagList.h"
#include "ArrayList.h"
#include "HighList.h"
#include "ChatServer.h"
#include "Trigger.h"
#include "StatementParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace MMScript;

BOOL CCommandTable::GroupCommand(CSession *pSession, CString& strGroup, CScriptEntity *pEntity)
{
	pSession->GetGroups().Add(strGroup, pEntity);
	return TRUE;
}

BOOL CCommandTable::Action(CSession *pSession, CString &strLine)
{
	CString strTrigger, strAction, strGroup;

	if(!CParseUtils::FindStatement(pSession,strLine,strTrigger))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strAction))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strTrigger);
	CParseUtils::ReplaceVariables(pSession, strGroup);
	CParseUtils::PretranslateString(pSession, strAction);

	if (!strTrigger.IsEmpty() && !strAction.IsEmpty())
	{
		try
		{
			CActionList *list = pSession->GetActions();

			CAction::PtrType newAction;
			bool bAdded = list->Add( strTrigger, strAction, strGroup, newAction );

			CString strMessage;
			strMessage.Format( _T("Action %s added."), bAdded ? "was" : "was not" );
			pSession->QueueMessage( CMessages::MM_ACTION_MESSAGE, strMessage );
			return TRUE;
		}
		catch(...)
		{
			::OutputDebugString(_T("Error adding action\n"));
			return FALSE;
		}

	}

	if(strTrigger.IsEmpty() && strAction.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Actions.");
		string msg = pSession->GetActions()->PrintList();
		pSession->PrintAnsiNoProcess( msg );
		return TRUE;
	}

	if (strAction.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Action:");
		CString strText = pSession->GetActions()->Print( strAction );
	}
	return TRUE;
}

BOOL CCommandTable::DisableAction(CSession *pSession, CString &strLine)
{
	CString searchCriteria;
	if(!CParseUtils::FindStatement( pSession, strLine, searchCriteria ) )
		return FALSE;

	CParseUtils::ReplaceVariables( pSession, searchCriteria );

	if ( searchCriteria.IsEmpty() )
		return TRUE;

	CAction::PtrType foundAction;
	if( pSession->GetActions()->Find( searchCriteria, foundAction ) )
	{
		foundAction->SetEnabled( false );
		pSession->QueueMessage( CMessages::MM_ENABLE_MESSAGE, "Action disabled." );
	}
	else
		pSession->QueueMessage( CMessages::MM_ENABLE_MESSAGE, "Action not found." );

	return TRUE;
}

BOOL CCommandTable::DisableTrigger(CSession *pSession, CString& strLine, CTriggerList& list)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam))
		return FALSE;

	if (strParam.IsEmpty())
		return TRUE;

	CTrigger *pTrigger = list.Find(strParam);

	if (pTrigger != NULL)
	{
		pTrigger->Enabled(FALSE);
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Trigger disabled.");
	}
	return TRUE;
}

BOOL CCommandTable::EditTrigger(CSession *pSession, CString& strLine, CTriggerList& list)
{
	CString strType, strParam1;
	if(!CParseUtils::FindStatement(pSession,strLine,strType))
		return FALSE;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam1))
		return FALSE;

	if (strParam1.IsEmpty())
		return TRUE;

	CTrigger *pTrigger = list.Find(strParam1);

	if (pTrigger != NULL)
	{
		CString strText;
		pTrigger->MapToCommand(strText);
		pSession->SetEditText(strText);
	}
	else
		strType.Append(" not found.");
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strType );
	return TRUE;
}

BOOL CCommandTable::EditAction(CSession *pSession, CString &strLine)
{
	CString searchCriteria;
	if( !CParseUtils::FindStatement( pSession, strLine, searchCriteria ) )
		return FALSE;

	if( searchCriteria.IsEmpty() )
	{
		pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_CONFIGURATION_ACTIONS,0 );
		return TRUE;
	}
	CAction::PtrType foundAction;
	if( pSession->GetActions()->Find( searchCriteria, foundAction ) )
		pSession->SetEditText( foundAction->MapToCommand() );
	else
		pSession->QueueMessage( CMessages::MM_ACTION_MESSAGE, "Action not found." );

	return TRUE;
}

BOOL CCommandTable::ToggleTrigger(CSession *pSession, CString& strLine, CTriggerList& list, BOOL bEnabled)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strParam);

	if (strParam.IsEmpty())
		return TRUE;

	CTrigger *pTrigger = list.Find(strParam);

	if (pTrigger != NULL)
	{
		pTrigger->Enabled(bEnabled);
	}
	if(bEnabled)
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Trigger enabled.");
	else
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Trigger disabled..");
	return TRUE;
}

BOOL CCommandTable::EnableTrigger(CSession *pSession, CString& strLine, CTriggerList& list)
{
	ToggleTrigger(pSession, strLine, list, TRUE);
	return TRUE;
}

BOOL CCommandTable::EnableAction(CSession *pSession, CString &strLine)
{
	CString searchCriteria;
	if(!CParseUtils::FindStatement( pSession, strLine, searchCriteria ) )
		return FALSE;

	CParseUtils::ReplaceVariables( pSession, searchCriteria );

	if ( searchCriteria.IsEmpty() )
		return TRUE;

	CAction::PtrType foundAction;
	if( pSession->GetActions()->Find( searchCriteria, foundAction ) )
	{
		foundAction->SetEnabled( true );
		pSession->QueueMessage( CMessages::MM_ENABLE_MESSAGE, "Action enabled." );
	}
	else
		pSession->QueueMessage( CMessages::MM_ENABLE_MESSAGE, "Action not found." );

	return TRUE;
}

BOOL CCommandTable::GroupActions(CSession *pSession, CString &strLine)
{
	CString group;

	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, group, false ) )
	{
		if( !group.IsEmpty() )
		{
			std::string groupList = pSession->GetActions()->GroupActionList(group);

			CString strText;
			strText.Format( "Defined actions for group [%s]:", group );
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
			pSession->PrintAnsiNoProcess(groupList);
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
				"You must supply a group for the GroupActions command");
		}
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
			"Not enough arguments to GroupActions command");
	}

	return TRUE;
}

BOOL CCommandTable::GroupGags(CSession *pSession, CString& strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strText;
	strText.Format("Defined gags(%s):",strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);
	pSession->GetGags()->PrintGroup(strGroup, strText);
	pSession->PrintAnsiNoProcess(strText);
	return TRUE;
}

BOOL CCommandTable::GroupArrays(CSession *pSession, CString& strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strHeader;
	strHeader.Format("Defined arrays(%s):",strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strHeader);
	CMMArray *pArray = (CMMArray *)pSession->GetArrays()->GetFirst();
	int nCount = 0;
	CString strText;
	while(pArray != NULL)
	{

		if(pArray->Group() == strGroup)
		{
			CString strTemp;
			pArray->MapToText(strTemp, TRUE);
			CString strMessage;
			strMessage.Format("%s%-3d%s %s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetArrays()->GetFindIndex() + 1,
				ANSI_RESET,
				strTemp);
			strText += strMessage;
			nCount++;
		}
		pArray = (CMMArray *)pSession->GetArrays()->GetNext();
	}

	CString strCount;
	strCount.Format("%s[%s%s%d%s%s arrays listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strText += strCount;

	pSession->PrintAnsiNoProcess(strText);
	return TRUE;
}
BOOL CCommandTable::GroupHighs(CSession *pSession, CString& strLine)
{
	CString strGroup;
	if(!CParseUtils::FindStatement(pSession,strLine,strGroup))
		return FALSE;

	if (strGroup.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strGroup);

	CString strHeader;
	strHeader.Format("Defined highlights(%s):",strGroup);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strHeader);
	CHigh *pHigh = (CHigh *)pSession->GetHighlights()->GetFirst();
	int nCount = 0;
	CString strText;
	while(pHigh != NULL)
	{

		if(pHigh->Group() == strGroup)
		{
			CString strTemp;
			pHigh->MapToText(strTemp, TRUE);
			CString strMessage;
			strMessage.Format("%s%-3d%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetHighlights()->GetFindIndex() + 1,
				ANSI_RESET,
				pHigh->Enabled() ? ' ' : '*',
				strTemp);
			strText += strMessage;
			nCount++;
		}
		pHigh = (CHigh *)pSession->GetHighlights()->GetNext();
	}

	CString strCount;
	strCount.Format("%s[%s%s%d%s%s highlights listed.]%s\n\n",
		ANSI_F_BOLDWHITE,
		ANSI_RESET,
		ANSI_F_BOLDRED,
		nCount,
		ANSI_RESET,
		ANSI_F_BOLDWHITE,
		ANSI_RESET);

	strText += strCount;

	pSession->PrintAnsiNoProcess(strText);
	return TRUE;
}

BOOL CCommandTable::SendTrigger(CSession *pSession, CString& strLine, CTriggerList& list, BYTE chCommand)
{
	CString person, trigger;

	CStatementParser parser( pSession );
	if( !parser.GetTwoStatements( strLine, person, false, trigger, false ) )
	{
		if (person.IsEmpty() || trigger.IsEmpty())
			return TRUE;

		CTrigger *pTrigger = list.Find(trigger);

		if (pTrigger == NULL)
			return TRUE;

		pTrigger->MapToCommand(trigger);

		pSession->GetChat().SendCommand(chCommand, person, trigger);
	}

	return TRUE;
}

BOOL CCommandTable::SendAction(CSession *pSession, CString &strLine)
{
	CString person, trigger;

	CStatementParser parser( pSession );
	if(parser.GetTwoStatements( strLine, person, false, trigger, false ) )
	{
		if( person.IsEmpty() )
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, 
				_T("You must supply a receiver for the command") );

			return TRUE;
		}	

		if( trigger.IsEmpty() )
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, 
				_T("You must supply an action to send") );

			return TRUE;
		}

		CAction::PtrType foundAction;
		if( pSession->GetActions()->Find( trigger, foundAction ) )
		{
			string p = static_cast<LPCTSTR>(person);
			pSession->GetChat().SendCommand(
				MMChatServer::CommandTypeAction, 
				p, 
				foundAction->MapToCommand());
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
				_T("Action sent.") );
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
				_T("Action not found.") );
		}
	}

	return TRUE;
}

BOOL CCommandTable::KillTrigger(CSession *pSession, CString &strLine, CTriggerList& list)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,strLine,strParam1))
		return FALSE;
	if (strParam1.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strParam1);
	BOOL bResult;

	CTrigger *pTrigger = list.Find(strParam1);

	if (pTrigger != NULL)
	{
		CString strTemp, strText;
		pTrigger->MapToCommand(strText);
		pSession->Undo().Add(strText);
		bResult = list.Remove(pTrigger);
	}
	else
		bResult = FALSE;

	if (bResult)
	{
		pSession->QueueMessage(CMessages::MM_ACTION_MESSAGE, "Trigger removed.");
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_ACTION_MESSAGE, "Trigger not found.");
	}
	return TRUE;
}

BOOL CCommandTable::UnAction(CSession *pSession, CString &strLine)
{
	CString searchCriteria;

	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, searchCriteria, false ) )
	{
		CAction::PtrType foundAction;
		if( pSession->GetActions()->Find( searchCriteria, foundAction ) )
		{
			if( pSession->GetActions()->Remove( foundAction ) )
			{
				pSession->Undo().Add( foundAction->MapToCommand() );
				pSession->QueueMessage( CMessages::MM_ACTION_MESSAGE,
					_T("Action removed." ) );
			}
			else
			{
				pSession->QueueMessage( CMessages::MM_ACTION_MESSAGE,
					_T("Failed to remove the action.") );
			}
		}
		else
		{
			pSession->QueueMessage( CMessages::MM_ACTION_MESSAGE,
				_T("Action not found." ) );
		}
	}
	else
	{
		pSession->QueueMessage( CMessages::MM_GENERAL_MESSAGE,
			_T("You must supply an action to remove") );
	}
	return TRUE;
}
