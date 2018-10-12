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
#pragma warning(disable: 4267)

namespace MMChatServer
{
	[event_source(native)]
	class CChatSocket : public CAsyncSocket
	{
	public:
		CChatSocket();
		virtual ~CChatSocket();

		// The event messages pass back this is in the LPARAM.  This is
		// how I identifiy which socket is sending them message.
		void SetID(unsigned long lID)		{m_lID = lID; }
		unsigned long GetID()				{ return(m_lID); }

		virtual HRESULT SendData(const CString& strText);
		virtual HRESULT SendData(const char *data, int nLength);

		__event void AcceptEvent(CChatSocket *pSocket, int nErrorCode);
		__event void CloseEvent(CChatSocket *pSocket, int nErrorCode);
		__event void ConnectEvent(CChatSocket *pSocket, int nErrorCode);
		__event void ReceiveEvent(CChatSocket *pSocket, int nErrorCode);
		__event void SendEvent(CChatSocket *pSocket, int nErrorCode);

	public:
		virtual void OnAccept(int nErrorCode);
		virtual void OnClose(int nErrorCode);
		virtual void OnConnect(int nErrorCode);
		virtual void OnReceive(int nErrorCode);
		virtual void OnSend(int nErrorCode);

	protected:
		unsigned long m_lID;

	private:
		std::vector<char> _buffer;
		unsigned int m_nBytesSent;
	};
}