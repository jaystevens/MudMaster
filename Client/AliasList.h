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
#pragma once

////////////////////////////////////////////////////
// Sameple ALIAS might look like this.
// strKey		targ
// bHasVar		TRUE
// nVarNum		0
// strText		/var Target $0
//
// The /alias command that would represent is:
// "/alias {targ %0} {/var Target $0}"

#include "ChatServer.h"
#include "Alias.h"
#include <boost/shared_ptr.hpp>

namespace MMScript
{
	class CAliasList
	{
	public:
//TODO: KW add element type and use it in listType def
		typedef CAlias::PtrType ElementType;
		typedef std::vector<ElementType> ListType;
		typedef ListType::iterator ListIter;

		struct MatchData
		{
			CString _command;
			unsigned int _index;
		};

		CAliasList();
		~CAliasList();

		void RemoveAll();
		unsigned int GetCount() const;

		//TODO: KW add remove which uses pointer
		bool Remove( CAlias::PtrType &alias );

		bool RemoveByDisplayName(const CString &search);
		unsigned int RemoveGroup(const CString &group);

		bool UnAlias(
			char commandChar,
			const CString &index,
			std::string &undo);

		bool DisableAlias(const CString &index);

		bool SendAlias(
			char commandChar,
			const CString &index, 
			const std::string &person,
			MMChatServer::CChatServer &server);

		CString Grep( const CString &search );

		CString AliasList();
		CString GroupAliasList(const std::string &group);

		// This function will break the key down to fill in the
		// ALIAS_MAP.  It also places the alias in the array in
		// alphabetical order.
		bool Add(
			const CString &name, 
			const CString &action, 
			const CString &group,
			CAlias::PtrType &alias);

		// Need to set this to point to the view's global variable
		// structure.
		void SetGlobalVars(CString *ptr);

		int EnableGroup(const CString &group, bool enabled);
		unsigned int SendGroup(
			char commandChar,
			MMChatServer::ScriptCommandType cmd, 
			const std::string &groupName, 
			const std::string &personName, 
			MMChatServer::CChatServer &server);

		unsigned int WriteToFile(
			char commandChar,
			CStdioFile &file);

		unsigned int WriteGroupToFile(
			char commandChar,
			CStdioFile &file, 
			const CString &group);

		// Takes some text and determine whether there is a match
		// in the alias list.  If the alias does NOT have a variable
		// then the matched is equal if pszText == pMap->strName.
		// If the alias has a variable then the first word of
		// pszText must equal  pMap->strName.
		bool FindAlias(
			const CString &alias, 
			CAlias::PtrType &matchedalias);
		bool FindEnabledAlias(
			const CString &alias, 
			MatchData &m);

		const ListType &GetAliases();
		bool GetByIndex( const CString &index, CAlias::PtrType &alias );
	private:
		struct AliasListImpl;
		std::auto_ptr<AliasListImpl> _pImpl;

	};
}
