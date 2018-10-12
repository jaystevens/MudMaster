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
#include "MudSocket.h"
#include "Messages.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

CMudSocket::CMudSocket()
{
	m_pView = NULL;
}

CMudSocket::~CMudSocket()
{
}


#if 0
BEGIN_MESSAGE_MAP(CMudSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CMudSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

void CMudSocket::OnConnect(int nErrorCode) 
{
	if (0 != nErrorCode)
	{
		std::string errorString;
		ErrorHandlingUtils::GetErrorString(nErrorCode, errorString);

		m_pView->SendMessage(CMessages::PRINT_MESSAGE, 0, (LPARAM)errorString.c_str());
		m_pView->PostMessage(CMessages::MSG_SOCKET_CLOSE, 0, 0);
	}
    else 
    {
        BOOL fOptval = TRUE;
        this->SetSockOpt(SO_KEEPALIVE, &fOptval, sizeof(BOOL));
		m_pView->SendMessage(CMessages::MSG_SOCKET_CONNECT, 0, 0);
    }
}

void CMudSocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	m_pView->PostMessage(CMessages::MSG_SOCKET_INCOMING, 0, nErrorCode);
}

void CMudSocket::OnClose(int nErrorCode) 
{
	if (0 != nErrorCode)
	{
		std::string errorString;
		ErrorHandlingUtils::GetErrorString(nErrorCode, errorString);
		m_pView->SendMessage(CMessages::PRINT_MESSAGE, 0, (LPARAM)errorString.c_str());
	}

	m_pView->PostMessage(CMessages::MSG_SOCKET_CLOSE, 0, nErrorCode);

}

void CMudSocket::SetView(CView *pView)
{
	m_pView = pView;
}

std::string CMudSocket::GetErrorString(int nError)
{
	std::string message;
	switch(nError)
	{
	case WSANOTINITIALISED:
		message = "A successful AfxSocketInit must occur before using this API.";
		break;
	case WSAENETDOWN:
		message = "The Windows Sockets implementation detected that the network subsystem failed.";
		break;
	case WSAEADDRINUSE:
		message = "The specified address is already in use.";
		break;
	case WSAEINPROGRESS:
		message = "A blocking Windows Sockets call is in progress.";
		break;
	case WSAEADDRNOTAVAIL:
		message = "The specified address is not available from the local machine.";
		break;
	case WSAEAFNOSUPPORT:
		message = "Addresses in the specified family cannot be used with this socket.";
		break;
	case WSAECONNREFUSED:
		message = "The attempt to connect was rejected.";
		break;
	case WSAEDESTADDRREQ:
		message = "A destination address is required.";
		break;
	case WSAEFAULT:
		message = "The nSockAddrLen argument is incorrect.";
		break;
	case WSAEINVAL:
		message = "Invalid host address.";
		break;
	case WSAEISCONN:
		message = "The socket is already connected.";
		break;
	case WSAEMFILE:
		message = "No more file descriptors are available.";
		break;
	case WSAENETUNREACH:
		message = "The network cannot be reached from this host at this time.";
		break;
	case WSAENOBUFS:
		message = "No buffer space is available. The socket cannot be connected.";
		break;
	case WSAENOTSOCK:
		message = "The descriptor is not a socket.";
		break;
	case WSAETIMEDOUT:
		message = "Attempt to connect timed out without establishing a connection.";
		break;
	default:
		message = "An unknown error occurred";			
	}

	return message;
}