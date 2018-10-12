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

#include "ScriptEntityList.h"
#include "ChatServer.h"

namespace MMScript
{
	class CMMList;

	class CUserLists : public CScriptEntityList
	{
		static const DWORD DEFAULT_LIST_ARRAY_SIZE = 150;
		static const DWORD DEFAULT_LIST_GROW_SIZE = 20;

	public:
		virtual CScriptEntity *FindExactImpl(const CString& strSearch) const;
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity);
		virtual BOOL RemoveImpl(const CString& strSearch);

		CUserLists();
		~CUserLists();

		// Adds an item to an existing list.
		BOOL AddItem(LPCSTR pszListName, LPCSTR pszItem);
		BOOL AddItem(int nListIndex, LPCSTR pszItem);

		// Removes an item from a list.
		BOOL RemoveItem(LPCSTR pszListName, LPCSTR pszItem);
		BOOL RemoveItem(LPCSTR pszListName, int nItemIndex);
		BOOL RemoveItem(int nListIndex, LPCSTR pszItem);
		BOOL RemoveItem(int nListIndex, int nItemIndex);

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const CString &groupName, 
			const CString &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);

		// Pass in the name of a list to search for.  Returns a pointer
		// to that list if found, else returns NULL.
		CMMList* FindByName(LPCSTR pszListName);
	};

	extern BOOL InList(CString &strParams, CString &strResult);
	extern void CommandToList(CString &strParams);

	// Gets an item from a list.  @GetItem(<list name>,<index>)
	extern BOOL GetItem(CString &strParams, CString &strResult);

	// Returns the number of items in a list.  @GetCount(<list name>)
	extern BOOL GetCount(CString &strParams, CString &strResult);
}
