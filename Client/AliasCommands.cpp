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
#include "Alias.h"
#include "ObString.h"
#include "Colors.h"
#include "AliasList.h"
#include "Globals.h"
#include "StatementParser.h"
#include <sstream>
#include "ProcedureTable.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

BOOL CCommandTable::Alias(CSession *pSession, CString &strLine)
{
	CString name;
	CString action;
	CString group;

	CStatementParser parser( pSession );
	if(! parser.GetThreeStatements( 
		strLine, 
		name, false,
		action, true,
		group, false ))
		return FALSE;

	// If have 2 params then add the alias.
	if( !name.IsEmpty() && !action.IsEmpty() )
	{
		CAlias::PtrType alias;

		std::stringstream msg;

		if( pSession->GetAliases()->Add( name, action, group, alias ) )
			msg << "Alias added.";
		else
			msg << "Failed to add the alias.";

		pSession->QueueMessage( CMessages::MM_ALIAS_MESSAGE, msg.str().c_str() );
	}
	else if( name.IsEmpty() && action.IsEmpty() )
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Aliases:");
		CString aliasList = pSession->GetAliases()->AliasList();
		pSession->PrintAnsiNoProcess( aliasList );
	}
	else if( action.IsEmpty() )
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Alias:");
		CAlias::PtrType alias;
		if(pSession->GetAliases()->GetByIndex(name, alias))
		{
			std::stringstream buffer;
			if(alias->enabled())
				buffer << " ";
			else
				buffer << "*";

			buffer 
				<< alias->Command(CGlobals::GetCommandCharacter()) 
				<< std::endl;

			pSession->PrintAnsiNoProcess(buffer.str());
		}
	}

	return TRUE;
}
BOOL CCommandTable::AllCommands(CSession *pSession, CString &strLine)
{
	CString allCommands;
	int i = 0;
		while(CCommandTable::m_List[i].proc != NULL )
		{
			strLine.Format("%s%03d: %s%s\n",
				ANSI_F_BOLDWHITE,
				i + 1,
				ANSI_RESET,
				_T(CCommandTable::m_List[i].szCommand ));
			allCommands += strLine;
			i++;
		}

	pSession->PrintAnsiNoProcess(allCommands);

	return TRUE;
}

BOOL CCommandTable::DisableAlias(CSession *pSession, CString &strLine)
{
	CString index;

	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, index, false ) ) 
	{
		if( !index.IsEmpty() )
		{
			if( pSession->GetAliases()->DisableAlias( index ) )
			{
				pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Alias disabled.");
			}
		}
	}

	return TRUE;
}

BOOL CCommandTable::EditAlias(CSession *pSession, CString &strLine)
{
	CString index;
	if(strLine.IsEmpty())
	{
        pSession->GetHost()->ThisWnd()->SendMessage(WM_COMMAND,ID_CONFIGURATION_ALIASES,0 );
		return TRUE;
	}
	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, index, false ) )
	{
		if( !index.IsEmpty() )
		{
			CAlias::PtrType alias;
			if(pSession->GetAliases()->GetByIndex(index, alias))
				pSession->SetEditText( 
					alias->Command( CGlobals::GetCommandCharacter() ) );
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_ALIAS_MESSAGE, "Alias not found.");
		}
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_ALIAS_MESSAGE, "Invalid argument to EditAlias command.");
	}

	return TRUE;
}

BOOL CCommandTable::EnableAlias(CSession *pSession, CString &strLine)
{
	CString index;

	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, index, false ) )
	{
		if( !index.IsEmpty() )
		{
			CAlias::PtrType alias;
			if( pSession->GetAliases()->GetByIndex(index, alias) )
			{
				alias->enabled(true);
				pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Alias enabled.");
			}
			else
			{
				pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "Alias not found.");
			}
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "You must supply an alias to enable.");
		}
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, "No arguments supplied to EnableAlias.");
	}

	return TRUE;
}

BOOL CCommandTable::GroupAliases(CSession *pSession, CString &strLine)
{
	CString group;
	CStatementParser parser( pSession );
	if( parser.GetOneStatement( strLine, group, false ) )
	{
		if( !group.IsEmpty() )
		{
			std::string groupList = 
				pSession->GetAliases()->GroupAliasList( static_cast<LPCTSTR>( group ) );

			CString strText;
			strText.Format( "Defined Aliases(%s):", group );
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText);

			pSession->PrintAnsiNoProcess(groupList);
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
				"You must supply a group for the GroupAliases command");
		}
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,
			"Not enough arguments to GroupAliases command");
	}

	return TRUE;
}
