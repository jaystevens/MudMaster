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
#include "Action.h"
#include "Globals.h"
#include "ParseUtils.h"
#include "Ifx.h"
#include "Sess.h"
#include <sstream>

using namespace std;
using namespace MMScript;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAction::CAction(const CAction & src)
: _strTrigger( src._strTrigger )
, _strAction( src._strAction )
, _strGroup( src._strGroup )
, _enabled( src._enabled )
{
	ProcessTrigger( src.GetTrigger().c_str() );
}

CAction::~CAction()
{
}

CAction& CAction::operator =(const CAction& src)
{
	CAction action(src);
	Swap( action );
	return *this;
}

const string CAction::MapToCommand()
{
	std::stringstream buffer;

	buffer
		<< CGlobals::GetCommandCharacter()
		<< "action "
		<< MapToText( true );

	return buffer.str();
}

const string CAction::MapToText( BOOL bIncludeGroup )
{
	std::stringstream buffer;
	buffer 
		<< CGlobals::GetBlockStartCharacter()
		<< GetTrigger()
		<< CGlobals::GetBlockEndCharacter()
		<< " "
		<< CGlobals::GetBlockStartCharacter()
		<< GetAction()
		<< CGlobals::GetBlockEndCharacter();

	if(bIncludeGroup)
	{
		buffer
			<< " "
			<< CGlobals::GetBlockStartCharacter()
			<< GetGroup()
			<< CGlobals::GetBlockEndCharacter();
	}

	return buffer.str();
}

const std::string &CAction::GetAction()	const
{
	return _strAction;
}

void CAction::SetAction( LPCTSTR strAction )
{
	_strAction = strAction;
}

const std::string &CAction::GetGroup() const
{
	return _strGroup;
}

const std::string &CAction::GetTrigger() const
{
	return _strTrigger;
}

bool CAction::IsEnabled(void) const
{
	return _enabled;
}

void CAction::SetEnabled( bool enabled )
{
	_enabled = enabled;
}

void CAction::Swap( CAction &action )
{
	std::swap( _strAction, action._strAction );
	std::swap( _strTrigger, action._strTrigger );
	std::swap( _strGroup, action._strGroup );
	std::swap( _enabled, action._enabled );
}

bool CAction::CreateAction(
	const std::string &trigger, 
	const std::string &strAction,
	const std::string &group, 
	PtrType &action )
{
	if( strAction.empty() || trigger.empty() )
	{
		return false;
	}

	action.reset( new CAction( trigger, strAction, group ) );

	return true;
}

CAction::CAction( 
	const std::string &trigger, 
	const std::string &strAction, 
	const std::string &group )
	: _strTrigger( trigger )
	, _strAction( strAction )
	, _strGroup( group )
	, _enabled( true )
{
	ProcessTrigger( trigger.c_str() );
}

void CAction::SetTrigger( LPCTSTR trigger )
{
	_strTrigger = trigger;
}