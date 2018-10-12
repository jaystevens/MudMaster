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
#include "ActionList.h"
#include "Action.h"
#include "Ifx.h"
#include "Sess.h"
#include "Globals.h"
#include "DebugStack.h"
#include <boost/lexical_cast.hpp>
#include "Colors.h"
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	using namespace std;

	static LPCTSTR ACTION_FORMAT_STRING = _T("%s%-3d%s%c%s\n");

	CActionList::CActionList()
	{
	}

	CActionList::~CActionList()
	{
	}

	bool CActionList::Add(
		const string &trigger,
		const string &actionString,
		const string &group,
		CAction::PtrType &action)
	{
		bool result = false;
		CAction::PtrType newAction;

		if( CAction::CreateAction( trigger, actionString, group, newAction ) )
		{
			action = newAction;
			AddToList( action );
			result = true;
		}

		return result;
	}

	bool CActionList::Add(
		LPCTSTR trigger, 
		LPCTSTR strAction, 
		LPCTSTR group,
		CAction::PtrType &action )
	{
		std::string triggerString(trigger);
		std::string actionString(strAction);
		std::string groupString(group);
		return Add( triggerString, actionString, groupString, action );
	}

	class MatchesTrigger
		: public std::unary_function<string, bool>
	{
	public:
		MatchesTrigger(const string &trigger)
			: _trigger(trigger)
		{
		}

		bool operator()(CAction::PtrType action)
		{
			return action->GetTrigger().compare( _trigger ) == 0;
		}

	private:
		const string &_trigger;
	};

	void CActionList::AddToList( CAction::PtrType &action )
	{
		ListType::iterator it = std::find_if(
			_list.begin(), _list.end(), MatchesTrigger(action->GetTrigger()));

		if(it != _list.end())
		{
			CAction::PtrType current = *it;
			action->SetEnabled(current->IsEnabled());
			*it = action;
		}
		else
		{
			it = std::lower_bound(_list.begin(), _list.end(), action->GetTrigger());
			_list.insert(it, action);
		}
	}

	class SameTrigger
		: public std::unary_function<string, bool>
	{
	public:
		SameTrigger(const string &trigger)
			:_trigger(trigger){}

			bool operator()(const CAction::PtrType &action )
			{
				return action->GetTrigger().compare(_trigger) == 0;
			}

	private:
		const string &_trigger;
	};

	bool CActionList::FindByName( 
		LPCTSTR search, 
		CAction::PtrType &action, 
		int &whereFound ) const
	{
		bool found = false;
		ListType::const_iterator it = std::find_if( 
			_list.begin(), _list.end(), SameTrigger(search) );

		if( it != _list.end() )
		{
			action = *it;
			whereFound = std::distance( _list.begin(), it );
			found = true;
		}

		return found;
	}

	bool CActionList::Find( LPCTSTR search, CAction::PtrType &action ) const
	{
		int whereFound = -1;
		return Find( search, action, whereFound );
	}

	bool CActionList::Find( 
		LPCTSTR search, 
		CAction::PtrType &action, 
		int &whereFound ) const
	{
		bool found = false;
		try
		{
			unsigned int index = boost::lexical_cast<int>( search );
			if( index > 0 && index <= _list.size() )
			{
				--index;
				action = _list[index];
				whereFound = index;
				found = true;
			}
		}
		catch( boost::bad_lexical_cast ble )
		{
			found = FindByName( search, action, whereFound );
		}

		return found;
	}

	CString CActionList::Print( LPCTSTR index ) const
	{
		CString result;
		CAction::PtrType action;
		int whereFound = -1;
		if( Find( index, action, whereFound ) )
		{
			result.Format(ACTION_FORMAT_STRING,
				ANSI_F_BOLDWHITE,
				whereFound,
				ANSI_RESET,
				action->IsEnabled() ? ' ' : '*',
				action->MapToText( TRUE ));
		}
		return result;
	}

	string CActionList::PrintList() const
	{
		stringstream result;

		ListType::const_iterator it = _list.begin();
		ListType::const_iterator end = _list.end();
		while( it != end )
		{
			result
				<< ANSI_F_BOLDWHITE
				<< setw(3) << distance( _list.begin(), it ) + 1 
				<< ANSI_RESET
				<< ((*it)->IsEnabled() ? ' ' : '*')
				<< (*it)->MapToText( true )
				<< endl;

			++it;
		}

		return result.str();
	}

	string CActionList::GroupActionList( LPCTSTR groupName )
	{
		ListType::iterator it = _list.begin();
		ListType::iterator end = _list.end();

		int count = 0;
		std::stringstream buffer;

		while(it != end)
		{
			CAction::PtrType action = *it;
			if(action->GetGroup() == groupName)
			{
				buffer
					<< ANSI_F_BOLDWHITE
					<< std::setw(3) << std::distance(_list.begin(), it) + 1 << ":"
					<< ANSI_RESET;

				if(action->IsEnabled())
					buffer << " ";
				else
					buffer << "*";

				buffer << action->MapToText(true) << std::endl;

				++count;
			}
			++it;
		}

		buffer 
			<< ANSI_F_BOLDWHITE 
			<< "[" 
			<< ANSI_RESET 
			<< ANSI_F_BOLDRED
			<< count
			<< ANSI_RESET
			<< ANSI_F_BOLDWHITE
			<< " actions listed.]"
			<< ANSI_RESET
			<< std::endl << std::endl;

		return buffer.str().c_str();
	}

	bool CActionList::Remove( CAction::PtrType &action )
	{
		bool result = false;

	//TODO: KW original code
	//	ListIter it = std::remove(_list.begin(), _list.end(), action->GetTrigger());
	//New code
        ListIter it = std::find_if(
			_list.begin(), _list.end(), MatchesTrigger(action->GetTrigger()));
	//End New code
	//TODO: KW changed != to <= because it looked like if last action in the 
	// list was being deleted it missed. Should check again. 
	// Is _list.last < _list.end?
		if(it <= _list.end())
		{ 
			_list.erase( it	);
			result = true;
		}

		return result;
	}
	//TODO: KW remove based on trigger string
	bool CActionList::Remove( const std::string  strTrigger )
	{
		bool result = false;
        ListIter it = std::find_if(
			_list.begin(), _list.end(), MatchesTrigger(strTrigger));

		if(it <= _list.end())
		{ 
			_list.erase( it	);
			result = true;
		}

		return result;

	}

	const CActionList::ListType &CActionList::GetList() const
	{
		return _list;
	}

	CActionList::ListType::size_type CActionList::GetCount() const
	{
		return _list.size();
	}

	class EnableGroupFunc
	{
	public:
		EnableGroupFunc( const string &group, bool enable )
			: _group( group )
			, _enable( enable )
			, _count( 0 )
		{}

		void operator()( CAction::PtrType action )
		{
			if( action->GetGroup().compare( _group ) == 0 )
			{
				action->SetEnabled( _enable );
				++_count;
			}
		}

		int count()
		{
			return _count;
		}

	private:
		const string &_group;
		const bool &_enable;
		int _count;
	};

	int CActionList::EnableGroup( LPCTSTR group, bool enable )
	{
		EnableGroupFunc g = std::for_each(_list.begin(), _list.end(), 
			EnableGroupFunc(group, enable));

		return g.count();
	}

	class MatchesGroupPred
		: public std::unary_function<CAction::PtrType, bool>
	{
	public:
		MatchesGroupPred( const string &group )
			: _group( group )
		{
		}

		bool operator()( CAction::PtrType action )
		{
			return action->GetGroup().compare( _group ) == 0;
		}

	private:
		const string &_group;
	};

	int CActionList::RemoveGroup( LPCTSTR groupName )
	{
		ListType::size_type before = _list.size();

		ListType::iterator it = remove_if(
			_list.begin(), 
			_list.end(), MatchesGroupPred( groupName ));

		if( it != _list.end() )
			_list.erase(it, _list.end());

		return before - _list.size();
	}

	const std::string CActionList::Grep( LPCTSTR searchCriteria )
	{
		ListIter it = _list.begin();
		ListIter end = _list.end();

		std::stringstream out;
		int count = 0;

		while(it != end)
		{
			CAction::PtrType action = *it;
			string text = action->MapToText(true);
			if( text.find( searchCriteria ) != string::npos )
			{
				out
					<< ANSI_F_BOLDWHITE
					<< std::distance( _list.begin(), it ) + 1 
					<< ANSI_RESET;

				if(action->IsEnabled())
					out << " ";
				else
					out << "*";

				out << action->MapToText( true ) << std::endl;
				++count;
			}
			++it;
		}

		std::stringstream buffer;
		buffer
			<< ANSI_F_BOLDWHITE
			<< '['
			<< ANSI_F_BOLDRED
			<< count
			<< ANSI_F_BOLDWHITE
			<< "] actions found"
			<< ANSI_RESET
			<< std::endl
			<< out.str();

		return buffer.str();
	}

	bool CActionList::GetAt( unsigned int index, CAction::PtrType &foundAction )
	{
		if( index > _list.size() )
			return false;

		foundAction = _list[index];
		return true;
	}

	void CActionList::WriteToFile( CStdioFile &file )
	{
		stringstream result;

		ListType::const_iterator it = _list.begin();
		ListType::const_iterator end = _list.end();
		while( it != end )
		{
			result 
				<< (*it)->MapToCommand()
				<< std::endl;
			++it;
		}
		
		file.WriteString( result.str().c_str() );
	}

	class WriteGroup
	{
	public:
		WriteGroup(
			const string &group, 
			CStdioFile &file)
			: _group(group)
			, _file(file)
			, _count(0)
		{
		}

		void operator()(CAction::PtrType action)
		{
			if( action->GetGroup().compare( _group ) == 0 )
			{
				std::string actionCommand = action->MapToCommand();
				actionCommand += "\n";
				_file.WriteString( actionCommand.c_str() );
				++_count;
			}
		}

		int count() { return _count; }

	private:
		const string &_group;
		CStdioFile &_file;
		int _count;
	};

	int CActionList::WriteGroupToFile( CStdioFile &file, LPCTSTR groupName )
	{
		WriteGroup g = std::for_each(
			_list.begin(), _list.end(), WriteGroup( groupName, file ));

		return g.count();
	}

	void CActionList::RemoveAll()
	{
		_list.clear();
	}

	class SendGroupHelper
	{
	public:
		SendGroupHelper( 
			const string &group, 
			const string &person, 
			MMChatServer::CChatServer &server )
			: _group( group	)
			, _person( person )
			, _server( server )
			, _count( 0 )
		{
		}

		void operator()( CAction::PtrType action )
		{
			if( action->GetGroup().compare( _group ) == 0 )
			{
				_server.SendCommand(
					MMChatServer::CommandTypeAction,
					_person,
					action->MapToCommand() );
				++_count;
			}
		}

		int count() { return _count; }

	private:
		const string _group;
		const string _person;
		MMChatServer::CChatServer &_server;
		int _count;
	};

	int CActionList::SendGroup( 
		LPCTSTR groupName, 
		LPCTSTR personName, 
		MMChatServer::CChatServer &server )
	{
		SendGroupHelper g = std::for_each(
			_list.begin(), 
			_list.end(), 
			SendGroupHelper( groupName, personName, server ) );

		return g.count();
	}
}

