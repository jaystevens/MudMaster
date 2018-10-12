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
#include "stdafx.h"
#include "ChatSocket.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMChatServer
{
	CChatSocket::CChatSocket()
		: m_nBytesSent(0)
		, m_lID(0)
	{
	}

	CChatSocket::~CChatSocket()
	{
	}

	HRESULT CChatSocket::SendData(const CString& strText)
	{
		return SendData((LPCTSTR)strText, strText.GetLength());
	}

	HRESULT CChatSocket::SendData(const char *data, int nLength)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			_buffer.reserve(_buffer.size() + nLength);
			_buffer.insert(_buffer.end(), data, data + nLength);

			OnSend(0);

			hr = S_OK;
		}
		catch (_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}

		return hr;
	}

	void CChatSocket::OnAccept(int nErrorCode) 
	{
		AcceptEvent(this, nErrorCode);
	}

	void CChatSocket::OnClose(int nErrorCode) 
	{
		CloseEvent(this, nErrorCode);
	}

	void CChatSocket::OnConnect(int nErrorCode) 
	{
        BOOL fOptval = TRUE;
        this->SetSockOpt(SO_KEEPALIVE, &fOptval, sizeof(BOOL));
		ConnectEvent(this, nErrorCode);
	}

	void CChatSocket::OnReceive(int nErrorCode) 
	{
		ReceiveEvent(this, nErrorCode);
	}

	void CChatSocket::OnSend(int nErrorCode) 
	{
		while(m_nBytesSent < _buffer.size())
		{
			int dwBytes = 0;

			dwBytes = Send(&_buffer[0] + m_nBytesSent, _buffer.size() -m_nBytesSent);
			if(dwBytes == SOCKET_ERROR)
			{
				if(GetLastError() == WSAEWOULDBLOCK) break;
				else
				{
					Close();
					return;
				}
			}
			else
			{
				m_nBytesSent += dwBytes;
			}
		}

		if(m_nBytesSent == _buffer.size())
		{
			m_nBytesSent = 0;
			std::vector<char>().swap(_buffer);
		}

		CAsyncSocket::OnSend(nErrorCode);
	}
}