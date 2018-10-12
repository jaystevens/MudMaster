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
	class CTrigger;

	class CTriggerList : public CScriptEntityList
	{
	public:
		virtual BOOL RemoveImpl(const CString& strSearch);
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity);
		virtual CScriptEntity *FindExactImpl(const CString& strTrigger) const;

		CTriggerList();
		virtual ~CTriggerList();

		virtual CTrigger *Add(const CString& strTrigger, const CString& strAction, const CString& strGroup) = 0;

		void PrintList(CString& strBuffer) const;
		void PrintTrigger(int nIndex, CString& strBuffer) const;
		void PrintTrigger(CTrigger *pTrigger, CString& strBuffer) const;
		void PrintTrigger(const CString& strTrigger, CString& strBuffer) const;
		void PrintGroup(const CString& strGroup, CString& strBuffer) const;

		int DisableGroup(const CString& strGroup);
		int EnableGroup(const CString& strGroup);
		int ToggleGroup(const CString& strGroup, BOOL bEnabled);

		virtual CTrigger *CheckTrigger(const CString& strLine, int& nStartIndex);
		CTrigger *Find(const CString& strParam) const;

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const CString &groupName, 
			const CString &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);
	};
}