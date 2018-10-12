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

namespace SerializedOptions
{
	class CChatOptions 
	{
	public:
		HRESULT Read(LPCTSTR lpszPathName);
		HRESULT Save(LPCTSTR lpszPathName);

		CChatOptions();
		CChatOptions(const CChatOptions& src);
		CChatOptions &operator=(const CChatOptions &src);
		virtual ~CChatOptions();

		//ACCESSORS
		BOOL AutoAccept()		{return m_bAutoAccept;}
		LPCTSTR ChatName()		{return _chatName.c_str();}
		BOOL DoNotDisturb()		{return m_bDoNotDisturb;}
		UINT ListenPort()		{return m_nListenPort;}
		BOOL AutoServe()		{return m_bAutoServe;}

		//MUTATORS
		void ChatName(LPCTSTR szChatName)			{_chatName = szChatName;}
		void DoNotDisturb(BOOL bDoNotDisturb)		{m_bDoNotDisturb = bDoNotDisturb;}
		void AutoAccept(BOOL bAutoAccept)			{m_bAutoAccept = bAutoAccept;}
		void ListenPort(UINT nListenPort)			{m_nListenPort = nListenPort;}
		void AutoServe(BOOL bAutoServe)				{m_bAutoServe = bAutoServe;}

	private:
		void Swap(CChatOptions &src);

		// User's chat name.
		std::string _chatName;

		// When TRUE incoming chat calls are not accepted.
		BOOL m_bDoNotDisturb;

		// The port to use for listening to chat requests.
		UINT m_nListenPort;

		// When TRUE the user is not prompted to accept the call.
		BOOL m_bAutoAccept;
		// When TRUE the chat is chatserved when the call is accepted.
		BOOL m_bAutoServe;

	};
}