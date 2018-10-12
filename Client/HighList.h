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

class CSession;
class CCharFormat;
class CColorLine;

#include "ScriptEntityList.h"
#include "ChatServer.h"
#include "ColorLine.h"

namespace MMScript
{
	class CHigh;
	class CHighList : public CScriptEntityList
	{
		static const DWORD DEFAULT_HIGHLIGHT_ARRAY_SIZE	= 150;
		static const DWORD DEFAULT_HIGHLIGHT_GROW_SIZE = 20;

	public:
		virtual CScriptEntity *FindExactImpl(const CString& strSearch) const;
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity);
		virtual BOOL RemoveImpl(const CString& strSearch);

		BOOL TextToAttr(LPCSTR szColor, BYTE *nFore, BYTE *nBack);
		BOOL ColorFromString(LPCSTR szColor, BYTE *nFore, BYTE *nBack);
		void CheckHighlight(CSession *pSession, CColorLine::SpColorLine &line);

		CHighList();
		~CHighList();

		// Adds a highlight to the list.  The highlight is inserted into the
		// list in sorted order.
		CHigh *Add(LPCSTR pszMask, LPCSTR pszColor, LPCSTR pszGroup);

		int DisableGroup(LPCSTR pszGroup);
		int EnableGroup(LPCSTR pszGroup);

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const CString &groupName, 
			const CString &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);

	};
}
