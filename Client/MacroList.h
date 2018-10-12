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

// Key names are created as follows:
// prefix 'a' = alt
// prefix 'c' = ctrl
// prefix 's' = shift
// name of the key
// af1 = Alt-F1
// cf1 = Ctrl-F1
// sf1 = Shift-F1
// asf1 = Alt-Shift-F1
// aa = Alt-A
// acsa = Alt-Shift-Control-A

#include "ScriptEntityList.h"
#include "ChatServer.h"

namespace MMScript
{
	class CMacro;

	class CMacroList : public CScriptEntityList
	{
		static const DWORD DEFAULT_MACRO_ARRAY_SIZE	= 150;
		static const DWORD DEFAULT_MACRO_GROW_SIZE = 20;

		static const DWORD MACRO_FILE_VERSION = 1;
	public:
		virtual CScriptEntity *FindExactImpl(const CString& strSearch) const;
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity);
		virtual BOOL RemoveImpl(const CString& strSearch);

		BOOL ValidateKey(WORD wVirtualKey, WORD wModifiers);

		CMacroList();
		~CMacroList();

		// Adds a macro to the list.  If it exists the definition of that
		// key will be replaced.
		CMacro *Add(WORD wVirtualKeyCode, WORD wModifiers, const CString& strAction, const CString& strGroup, int nDest);
		CMacro *Add(const CString& strKey, const CString& strAction, const CString& strGroup, int nDest);

		BOOL DisableMacro(LPCSTR pszKey);
		BOOL EnableMacro(LPCSTR pszKey);

		int DisableGroup(LPCSTR pszGroup);
		int EnableGroup(LPCSTR pszGroup);

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const CString &groupName, 
			const CString &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);

		// Pass in a key code, it returns a text representation of 
		// that key code.  VK_END returns "END"
		static void PASCAL GetVirtualKeyName(WORD wVirtualKeyCode, CString &strName);
		static void PASCAL GetModifierName(WORD wModifiers, CString &strModifier);

		// Converts the text name to a virtual key code.  Returns 0 if the name
		// is an invalid key name.
		BOOL NameToKey(LPCSTR pszKey, WORD &wVirtualKeyCode, WORD &wModifiers);
		WORD VirtualKeyFromName(LPCSTR pszKey);

		// Pass in a key name, returns TRUE and fill in macro
		// if it finds it.
		CMacro *FindByValue(WORD wVirtualKeyCode, WORD wModifiers);
		CMacro *FindKeyByName(LPCSTR pszKey);
	};
}