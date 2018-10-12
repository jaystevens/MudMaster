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
#include "AliasList.h"
#include "colors.h"
#include <boost/lexical_cast.hpp>
#include <iomanip>

#pragma warning(disable:4267)
#pragma warning(disable:4244)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;

namespace MMScript
{
	struct CAliasList::AliasListImpl
	{
		AliasListImpl()
			: m_pGlobalVars(NULL)
		{
		}

		void AddToList(CAlias::PtrType &alias);
		bool isValidIndex(unsigned int index);
		bool GetByIndex(const CString &index, CAlias::PtrType &alias);
		const ListType &GetAliases();
		void SetGlobalVars(CString *ptr) { m_pGlobalVars = ptr; }
		CString *m_pGlobalVars;
		ListType _list;
	};

	class MatchesGroupPred
		: public std::unary_function<CAlias::PtrType, bool>
	{
	public:
		MatchesGroupPred( const CString &group )
			: _group( group )
		{
		}

		bool operator()( CAlias::PtrType alias )
		{
			return alias->group().compare( _group ) == 0;
		}

	private:
		const CString &_group;
	};

	class WriteToFileFunc
	{
	public:
		WriteToFileFunc(char commandChar, CStdioFile &file)
			: _file(file)
			, _commandChar(commandChar)
		{}

		void operator()(CAlias::PtrType alias)
		{
			std::string command = alias->Command(_commandChar);
			command += "\n";
			_file.WriteString( command.c_str() );
		}

	private:
		CStdioFile &_file;
		char _commandChar;
	};

	class SendChatToGroupFunc
	{
	public:
		SendChatToGroupFunc(
			char commandChar,
			const std::string &group,
			BYTE cmd,
			const std::string &person, 
			MMChatServer::CChatServer &server)
			: _commandChar(commandChar)
			, _group(group)
			, _cmd(cmd)
			, _person(person)
			, _server(server)
			, _count(0)
		{
		}

		void operator()( CAlias::PtrType alias )
		{
			if( alias->group().compare( _group ) == 0 )
			{
				std::string command = alias->Command( _commandChar );
				_server.SendCommand( _cmd, _person, command );

				++_count;
			}
		}

		int count() { return _count; }
	private:
		char _commandChar;
		std::string _group;
		BYTE _cmd;
		std::string _person;
		MMChatServer::CChatServer &_server;
		int _count;
	};

	class WriteGroupAlias
	{
	public:
		WriteGroupAlias(
			char commandChar,
			const CString &group, 
			CStdioFile &file)
			: _commandChar(commandChar)
			, _group(group)
			, _file(file)
			, _count(0)
		{
		}

		void operator()(CAlias::PtrType alias)
		{
			if( alias->group().compare( _group ) == 0 )
			{
				std::string command = alias->Command( _commandChar );
				command += "\n";
				_file.WriteString( command.c_str() );
				++_count;
			}
		}

		int count() { return _count; }

	private:
		char _commandChar;
		const CString &_group;
		CStdioFile &_file;
		int _count;
	};

	class MatchesNamePred
		: public std::unary_function<CString, bool>
	{
	public:
		MatchesNamePred(const CString &name)
			: _name(name)
		{
		}

		bool operator()(CAlias::PtrType alias)
		{
			return alias->name().Compare( _name ) == 0;
		}

	private:
		const CString &_name;
	};

	class EnableGroupFunc
	{
	public:
		EnableGroupFunc( const CString &group, bool enable )
		: _group( group )
		, _enable( enable )
		, _count( 0 )
		{}

		void operator()( CAlias::PtrType pAlias )
		{
			if( pAlias->group().compare( _group ) == 0 )
			{
				pAlias->enabled( _enable );
				++_count;
			}
		}

		int count()
		{
			return _count;
		}

	private:
		const CString &_group;
		const bool &_enable;
		int _count;
	};

	CAliasList::CAliasList()
		: _pImpl(new AliasListImpl)
	{
	}

	CAliasList::~CAliasList()
	{
	}

	unsigned int CAliasList::GetCount() const
	{
		return _pImpl->_list.size();
	}

	int CAliasList::EnableGroup(const CString &group, bool enabled)
	{
		EnableGroupFunc g = std::for_each(
			_pImpl->_list.begin(), _pImpl->_list.end(), 
			EnableGroupFunc(group, enabled));

		return g.count();
	}

	void CAliasList::AliasListImpl::AddToList(CAlias::PtrType &alias)
	{
		ListType::iterator it = std::find_if(
			_list.begin(), _list.end(), MatchesNamePred(alias->name()));

		if(it != _list.end())
		{
			CAlias::PtrType current = *it;
			alias->enabled(current->enabled());
			*it = alias;
		}
		else
		{
			it = std::lower_bound(_list.begin(), _list.end(), alias->name());
			_list.insert(it, alias);
		}
	}

	bool CAliasList::Add(
		const CString &name,
		const CString &action, 
		const CString &group,
		CAlias::PtrType &alias)
	{
		bool result = false;
		CAlias::PtrType newAlias;

		if( CAlias::CreateAlias( name, action, group, newAlias ) )
		{
			_pImpl->AddToList(newAlias);
			alias = newAlias;
			result = true;
		}
		 
		return result;
	}
//TODO: KW added Remove
	//bool CAliasList::Remove( CAlias::PtrType &alias )
	//{
	//	bool result = false;
	//	ListIter it = std::remove( (_pImpl->_list.begin(), _pImpl->_list.end(), alias);

	//	if(it != _pImpl->_list.end())
	//	{
	//		_pImpl->_list.erase( it	);
	//		result = true;
	//	}

	//	return result;

	//}
	bool CAliasList::Remove(CAlias::PtrType &alias)
	{
		bool result = false;

		ListIter it = std::find_if(
			_pImpl->_list.begin(), 
			_pImpl->_list.end(), 
			MatchesNamePred(alias->name()));

		if(it != _pImpl->_list.end())
		{
			_pImpl->_list.erase( it	);
			result = true;
		}

		return result;
	}
	bool CAliasList::RemoveByDisplayName(const CString &search)
	{
		bool result = false;

		ListIter it = std::find_if(
			_pImpl->_list.begin(), 
			_pImpl->_list.end(), 
			MatchesNamePred(search));

		if(it != _pImpl->_list.end())
		{
			_pImpl->_list.erase( it	);
			result = true;
		}

		return result;
	}

	bool CAliasList::FindAlias(
		const CString &alias, 
		CAlias::PtrType &matchedAlias)
	{
		bool retval = false;

		// Separate the key portion and the variable portion.
		CString name;
		CString var;
		int pos = alias.Find(' ');
		if(-1 != pos)
		{
			name = alias.Left( pos );
			var = alias.Mid( pos + 1 );
		}
		else
		{
			name = alias;
			// FRUITMAN another exception bug
			var = "";
		}

		typedef std::pair<ListType::iterator, ListType::iterator> ResultType;

		ResultType result = std::equal_range(
			_pImpl->_list.begin(), _pImpl->_list.end(), name);

		if(result.first != result.second)
		{
			matchedAlias = *result.first;
			retval = true;
			//if(a->hasVar())
			//	_pImpl->m_pGlobalVars[a->varNum()] = var;

			//m._command = a->action();

			//m._index = std::distance(
			//_pImpl->_list.begin(), result.first) + 1;

		}

		return retval;
	}

	bool CAliasList::FindEnabledAlias(
		const CString &alias, 
		MatchData &m)
	{
		bool retval = false;

		// Separate the key portion and the variable portion.
		CString name;
		CString var;
		int pos = alias.Find(' ');
		if(-1 != pos)
		{
			name = alias.Left( pos );
			var = alias.Mid( pos + 1 );
		}
		else
		{
			name = alias;
// FRUITMAN another exception bug
			var = "";
		}

		typedef std::pair<ListType::iterator, ListType::iterator> ResultType;

		ResultType result = std::equal_range(
			_pImpl->_list.begin(), _pImpl->_list.end(), name);

		if(result.first != result.second)
		{
			CAlias::PtrType a = *result.first;
// FRUITMAN if enabled...
			if(a->enabled())
			{
				retval = true;
				if(a->hasVar())
					_pImpl->m_pGlobalVars[a->varNum()] = var;

				m._command = a->action();

				m._index = std::distance(
				_pImpl->_list.begin(), result.first) + 1;
			}
		}

		return retval;
	}

	unsigned int CAliasList::SendGroup(
		char commandChar,
		MMChatServer::ScriptCommandType cmd, 
		const std::string &groupName, 
		const std::string &personName, 
		MMChatServer::CChatServer &server )
	{
		SendChatToGroupFunc s = std::for_each( 
			_pImpl->_list.begin(), _pImpl->_list.end(),  
			SendChatToGroupFunc(
				commandChar,
				groupName,
				static_cast<BYTE>( cmd ), 
				personName, 
				server ) );

		return s.count();
	}

	unsigned int CAliasList::WriteToFile(
		char commandChar,
		CStdioFile &file)
	{
		WriteToFileFunc f = std::for_each(
			_pImpl->_list.begin(), 
			_pImpl->_list.end(), WriteToFileFunc( commandChar, file ) );

		return _pImpl->_list.size();
	}

	unsigned int CAliasList::WriteGroupToFile(
		char commandChar,
		CStdioFile &file, 
		const CString &group )
	{
		WriteGroupAlias g = std::for_each(
			_pImpl->_list.begin(), 
			_pImpl->_list.end(), WriteGroupAlias( commandChar, group, file ));

		return g.count();
	}

	unsigned int CAliasList::RemoveGroup(const CString &group)
	{
		ListType::size_type before = _pImpl->_list.size();

		_pImpl->_list.erase(
			remove_if(
			_pImpl->_list.begin(), 
			_pImpl->_list.end(), MatchesGroupPred(group)),
			_pImpl->_list.end() );

		return before - _pImpl->_list.size();
	}

	bool CAliasList::UnAlias(
		char commandChar,
		const CString &index, 
		std::string &undo)
	{
		bool result = false;

		CAlias::PtrType alias;
		if(FindAlias(index, alias))
		{
			undo = alias->Command( commandChar );
			CAliasList::Remove( alias );
			result = true;
		}
		else
		{
			if( _pImpl->GetByIndex( index, alias ) )
			{
				undo = alias->Command( commandChar );
				CAliasList::Remove( alias );
				result = true;
			}
		}

		return result;
	}

	bool CAliasList::AliasListImpl::GetByIndex(
		const CString &index, 
		CAlias::PtrType &alias )
	{
		bool result = false;

		try
		{
			int n = boost::lexical_cast<int>(index);
			if(isValidIndex(n))
			{
				alias = _list[n - 1];
				result = true;
			}
		}
		catch(boost::bad_lexical_cast ble)
		{
			typedef std::pair<ListType::iterator, ListType::iterator> PairType;
			PairType res = 
				std::equal_range( _list.begin(), _list.end(), index );

			if( res.first != res.second )
			{
				alias = *res.first;
				result = true;
			}
		}

		return result;
	}
	//bool CAliasList::FindByName( 
	//	LPCTSTR search, 
	//	CAlias::PtrType &alias, 
	//	int &whereFound ) const
	//{
	//	bool found = false;
	//	ListType::const_iterator it = std::find_if( 
	//		_list.begin(), _list.end(), SameTrigger(search) );

	//	if( it != _list.end() )
	//	{
	//		alias = *it;
	//		whereFound = std::distance( _list.begin(), it );
	//		found = true;
	//	}

	//	return found;
	//}

	CString CAliasList::Grep( const CString &search )
	{
		ListIter it = _pImpl->_list.begin();
		ListIter end = _pImpl->_list.end();

		std::stringstream out;
		int count = 0;

		while(it != end)
		{
			CAlias::PtrType alias = *it;
			if( alias->Text( true ).Find( search ) != -1 )
			{
				out
					<< ANSI_F_BOLDWHITE
					<< std::distance( _pImpl->_list.begin(), it ) + 1
					<< ANSI_RESET;

				if(alias->enabled())
					out << " ";
				else
					out << "*";

				out << alias->Text( true ) << std::endl;
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
			<< "] aliases found"
			<< ANSI_RESET
			<< std::endl
			<< out.str();

		return buffer.str().c_str();
	}

	bool CAliasList::SendAlias(
		char commandChar,
		const CString &index,
		const std::string &person,
		MMChatServer::CChatServer &server )
	{
		bool result = false;

		CAlias::PtrType alias;
		if( _pImpl->GetByIndex( index, alias ) )
		{
			server.SendCommand(
				MMChatServer::CommandTypeAlias, 
				person, 
				alias->Command(commandChar) );
			result = true;
		}

		return result;
	}

	bool CAliasList::AliasListImpl::isValidIndex(unsigned int index)
	{
		return (index > 0 && index <= _list.size());
	}

	const CAliasList::ListType &CAliasList::AliasListImpl::GetAliases()
	{
		return _list;
	}

	CString CAliasList::AliasList()
	{
		std::stringstream buffer;
		ListType::iterator it = _pImpl->_list.begin();
		ListType::iterator end = _pImpl->_list.end();
		while(it != end)
		{
			CAlias::PtrType alias = *it;

			buffer
				<< ANSI_F_BOLDWHITE
				<< std::setw(3) 
				<< std::distance(_pImpl->_list.begin(), it) + 1 
				<< ":"
				<< ANSI_RESET;

			if(alias->enabled())
				buffer << " ";
			else
				buffer << "*";

			buffer << alias->Text(true) << std::endl;
	
			++it;
		}

		return buffer.str().c_str();
	}

	bool CAliasList::DisableAlias( const CString &index )
	{
		bool retval = false;

		CAlias::PtrType alias;

		if( _pImpl->GetByIndex( index, alias ) )
		{
			alias->enabled( false );
			retval = true;
		}

		return retval;
	}

	CString CAliasList::GroupAliasList( const std::string &group )
	{
		ListType::iterator it = _pImpl->_list.begin();
		ListType::iterator end = _pImpl->_list.end();

		int count = 0;
		std::stringstream buffer;

		while(it != end)
		{
			CAlias::PtrType alias = *it;
			if(alias->group() == group)
			{
				buffer
					<< ANSI_F_BOLDWHITE
					<< std::setw(3) << std::distance(_pImpl->_list.begin(), it) + 1 << ":"
					<< ANSI_RESET;

				if(alias->enabled())
					buffer << " ";
				else
					buffer << "*";

				buffer << alias->Text(true) << std::endl;

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
			<< " aliases listed.]"
			<< ANSI_RESET
			<< std::endl << std::endl;

		return buffer.str().c_str();
	}

	void CAliasList::RemoveAll()
	{
		_pImpl->_list.clear();
	}

	const CAliasList::ListType &CAliasList::GetAliases()
	{
		return _pImpl->_list;
	}

	bool CAliasList::GetByIndex( const CString &index, CAlias::PtrType &alias )
	{
		return _pImpl->GetByIndex( index, alias );
	}

	void CAliasList::SetGlobalVars( CString *ptr )
	{
		_pImpl->SetGlobalVars( ptr );
	}
}