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
#pragma	once

/**
  * \file
  * \brief Contains the interface for the CActionList class
  */

#include "ChatServer.h"
#include "Action.h"
#include "TriggerListImpl.h"

class CSession;

/**
  * \brief Contains all of the scripting entities 
  */
namespace MMScript
{
	class CActionList
		: public TriggerListImpl<CActionList, CAction::PtrType>
	{
	public:
		typedef CAction::PtrType ElementType;
		typedef std::vector<ElementType> ListType;
		typedef ListType::iterator ListIter;

		CActionList();
		~CActionList();

		void RemoveAll();

		ListType::size_type GetCount() const;

		void WriteToFile( CStdioFile &file );

		bool GetAt( unsigned int index, CAction::PtrType &foundAction );

		const std::string Grep( LPCTSTR searchText );
		CString Print( LPCTSTR index ) const;
		std::string PrintList() const;
		bool Remove( CAction::PtrType &action );
		//TODO: KW add overload of remove which uses strTrigger
		bool Remove( const std::string  strTrigger );

		int RemoveGroup( LPCTSTR groupName );
		int EnableGroup( LPCTSTR groupName, bool enable );
		int WriteGroupToFile( CStdioFile &file, LPCTSTR groupName );
		int SendGroup( 
			LPCTSTR groupName, 
			LPCTSTR personName, 
			MMChatServer::CChatServer &server );
		std::string GroupActionList( LPCTSTR groupName );

		// Adds an action to the list.  The action is inserted into the
		// list in sorted order.
		bool Add(
			LPCTSTR strTrigger, 
			LPCTSTR strAction, 
			LPCTSTR strGroup,
			CAction::PtrType &action );

		bool Add(
			const std::string &strTrigger,
			const std::string &strAction,
			const std::string &strGroup,
			CAction::PtrType &action);

		void SetGlobalVars(CString *ptr)	{ m_pGlobalVars = ptr; }

		const ListType &GetList() const;
		bool Find( LPCTSTR search, CAction::PtrType &action ) const;
		bool Find( LPCTSTR search, CAction::PtrType &action, int &whereFound ) const;
	private:
		void AddToList( CAction::PtrType &action );
		bool FindByName( LPCTSTR search, CAction::PtrType &action, int &whereFound ) const;
		ListType _list;
		CString *m_pGlobalVars;
	};
}
