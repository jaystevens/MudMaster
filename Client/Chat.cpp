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
// Chat.cpp: implementation of the CChat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chat.h"
#include "ErrorHandling.h"
#include "ActionList.h"
#include "TriggerList.h"
#include "Trigger.h"
#include "Alias.h"
#include "AliasList.h"
#include "Macro.h"
#include "MacroList.h"
#include "VarList.h"
#include "Event.h"
#include "EventList.h"
#include "SubList.h"
#include "GagList.h"
#include "High.h"
#include "HighList.h"
#include "UserLists.h"
#include "MMList.h"
#include "Globals.h"
#include "Array.h"
#include "ArrayList.h"
#include "BarItem.h"
#include "StatusBar.h"
#include "IChatOutput.h"
#include "resource.h"
#include "Colors.h"
#include <sstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace Utilities;
using namespace MMScript;
using namespace std;

namespace MMChatServer
{
	CChat::CChat()
		: m_bAllowSnoop(false)
		, m_bCommands(false)
		, m_bIgnore(false)
		, m_bPrivate(false)
		, m_bServe(false)
		, m_bExcludeServe(false)
		, m_bSnooped(false)
		, m_bTransfers(false)
		, m_nBytesWritten(0)
		, m_ConnectionState(InvalidConnectionState)
		, m_nFileLength(0)
		, m_nPort(0)
		, m_TransferType(None)
		, m_strAddress(_T("Uninitialized"))
		, m_strGroup(_T("Uninitialized"))
		, m_strName(_T("Connecting..."))
		, m_strSearchGroup(_T("not grouped"))
		, m_strSearchName(_T("Uninitialized"))
		, m_strSockAddress(_T("Uninitialized"))
		, m_strVersion(_T("Uninitialized"))
		, m_tTransferStart(0)
	{
	}

	CChat::CChat(LPCTSTR pszAddress, UINT nPort)
		: m_bAllowSnoop(false)
		, m_bCommands(false)
		, m_bIgnore(false)
		, m_bPrivate(false)
		, m_bServe(false)
		, m_bExcludeServe(false)
		, m_bSnooped(false)
		, m_bTransfers(false)
		, m_nBytesWritten(0)
		, m_ConnectionState(Called)
		, m_nFileLength(0)
		, m_nPort(nPort)
		, m_TransferType(None)
		, m_strAddress(pszAddress)
		, m_strGroup(_T("Uninitialized"))
		, m_strName(_T("Connecting..."))
		, m_strSearchGroup(_T("not grouped"))
		, m_strSearchName(_T("Uninitialized"))
		, m_strSockAddress(pszAddress)
		, m_strVersion(_T("Uninitialized"))
		, m_tTransferStart(0)
	{
		m_strAddress.TrimRight();
		m_strSockAddress.TrimRight();
	}

	CChat::~CChat()
	{
	}

	bool CChat::GetAllowSnoop() const
	{
		return m_bAllowSnoop;
	}

	bool CChat::GetCommands() const
	{
		return m_bCommands;
	}

	bool CChat::GetIgnore() const
	{
		return m_bIgnore;
	}

	bool CChat::GetPrivate() const
	{
		return m_bPrivate;
	}

	bool CChat::GetServe() const
	{
		return m_bServe;
	}
	bool CChat::GetExcludeServe() const
	{
		return m_bExcludeServe;
	}

	bool CChat::GetSnooped() const
	{
		return m_bSnooped;
	}

	bool CChat::GetTransfers() const
	{
		return m_bTransfers;
	}

	int CChat::GetBytesWritten() const
	{
		return m_nBytesWritten;
	}

	CChat::ConnectionState CChat::GetConnectionState() const
	{
		return m_ConnectionState;
	}

	int CChat::GetFileLength() const
	{
		return m_nFileLength;
	}

	int CChat::GetPort() const
	{
		return m_nPort;
	}

	CChat::TransferType CChat::GetTransferType() const
	{
		return m_TransferType;
	}

	bool CChat::IsSending() const
	{
		return m_TransferType == TransferType::Send;
	}

	bool CChat::IsReceiving() const
	{
		return m_TransferType == TransferType::Receive;
	}

	const CString &CChat::GetAddress() const
	{
		return m_strAddress;
	}

	const CString &CChat::GetGroup() const
	{
		return m_strGroup;
	}

	const CString &CChat::GetName() const
	{
		return m_strName;
	}

	const CString &CChat::GetSearchGroup() const
	{
		return m_strSearchGroup;
	}

	const CString &CChat::GetSearchName() const
	{
		return m_strSearchName;
	}

	const CString &CChat::GetReportedAddress() const
	{
		return m_strSockAddress;
	}

	const CString &CChat::GetVersion() const
	{
		return m_strVersion;
	}

	time_t CChat::GetTransferStartTime() const
	{
		return m_tTransferStart;
	}

	CFile &CChat::GetFile()
	{
		return m_file;
	}

	CChatSocket &CChat::GetSocket()
	{
		return m_Socket;
	}

	void CChat::SetAllowSnoop(bool bAllowSnoop)
	{
		m_bAllowSnoop = bAllowSnoop;
	}

	void CChat::SetCommands(bool bCommands)
	{
		m_bCommands = bCommands;
	}

	void CChat::SetIgnore(bool bIgnore)
	{
		m_bIgnore = bIgnore;
	}

	void CChat::SetPrivate(bool bPrivate)
	{
		m_bPrivate = bPrivate;
	}

	void CChat::SetServe(bool bServe)
	{
		m_bServe = bServe;
	}
	void CChat::SetExcludeServe(bool bExcludeServe)
	{
		m_bExcludeServe = bExcludeServe;
	}

	void CChat::SetSnooped(bool bSnooped)
	{
		m_bSnooped = bSnooped;
	}

	void CChat::SetTransfers(bool bTransfers)
	{
		m_bTransfers = bTransfers;
	}

	void CChat::SetBytesWritten(int nBytesWritten)
	{
		m_nBytesWritten = nBytesWritten;
	}

	void CChat::SetConnectionState(ConnectionState state)
	{
		m_ConnectionState = state;
	}

	void CChat::SetFileLength(int nFileLength)
	{
		m_nFileLength = nFileLength;
	}

	void CChat::SetPort(UINT nPort)
	{
		m_nPort = nPort;
	}

	void CChat::SetTransferType(TransferType type)
	{
		m_TransferType = type;
	}

	void CChat::SetAddress(const CString &strAddress)
	{
		m_strAddress = strAddress;
		m_strAddress.TrimRight();
	}

	void CChat::SetGroup(const CString &strGroup)
	{
		m_strGroup = strGroup;
		m_strSearchGroup = m_strGroup;
		m_strSearchGroup.MakeLower();
	}

	void CChat::SetName(const CString &strName)
	{
		m_strName = strName;
		m_strSearchName = m_strName;
		m_strSearchName.MakeLower();
	}

	void CChat::SetReportedAddress(const CString &strAddress)
	{
		m_strSockAddress = strAddress;
		m_strSockAddress.TrimRight();
	}

	void CChat::SetVersion(const CString &strVersion)
	{
		m_strVersion = strVersion;
	}

	void CChat::SetTransferStartTime(time_t time)
	{
		m_tTransferStart = time;
	}

	int CChat::ReceiveData(char *buffer, int bufferSize)
	{
		return GetSocket().Receive(buffer, bufferSize);
	}

	HRESULT CChat::Close(const string &message)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			ErrorHandlingUtils::TESTHR(SendMessage(message.c_str()));

			GetSocket().Close();

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("com_Error in CChat::Close\n"));
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::Close\n"));
		}
		return hr;
	}
	HRESULT CChat::SendMessage(PCTSTR szMessage)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CString strText;
			strText.Format(
				_T("%c%s%c"),
				(BYTE)CommandMessage,
				szMessage,
				(BYTE)CHAT_END_OF_COMMAND);

			ErrorHandlingUtils::TESTHR(GetSocket().SendData(strText));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendMessage\n"));
		}
		return hr;
	}
	bool CChat::MatchesSearchName(LPCTSTR szName)
	{
		if(GetSearchName().Compare(szName) == 0)
			return true;
		return false;
	}

	bool CChat::MatchesAddress(PCTSTR szAddress, int nPort)
	{
		if (GetAddress().Compare(szAddress) == 0)
			if(GetPort() == nPort)
				return true;

		return false;
	}

	bool CChat::MatchesSocketId(unsigned long id)
	{
		if(GetSocket().GetID() == id)
			return true;
		return false;
	}

	HRESULT CChat::SendData(PCTSTR szData, int length)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if (!GetIgnore() && GetConnectionState() == CChat::Connected)
				ErrorHandlingUtils::TESTHR(GetSocket().SendData(szData, length));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendData\n"));
		}
		return hr;
	}

	HRESULT CChat::SendData(const std::string &data)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if (!GetIgnore() && GetConnectionState() == CChat::Connected)
				ErrorHandlingUtils::TESTHR(GetSocket().SendData(data.c_str()));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChat::SendData\n"));
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendData\n"));
		}
		return hr;
	}

	HRESULT CChat::SendDataToGroup(PCTSTR szData, PCTSTR szGroupName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if(GetGroup().Compare(szGroupName) == 0)
				ErrorHandlingUtils::TESTHR(SendData(szData));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendDataToGroup\n"));
		}
		return hr;
	}

	HRESULT CChat::GetFlagsString(CString &strFlags)
	{
		strFlags.Format("%c%c%c%c%c%c%c%c",
			(GetCommands() ? 'A' : ' '),
			(GetTransfers() ? 'T' : ' '),
			(GetPrivate() ? 'P' : ' '),
			(GetIgnore() ? 'I' : ' '),
			(GetServe() ? 'S' : ' '),
			(GetExcludeServe() ? 'X' : ' '),
			(GetAddress() != GetReportedAddress() ? 'F' : ' '),
			(GetSnooped() ? 'N' : (GetAllowSnoop() ? 'n' : ' ')));

		return S_OK;
	}

	HRESULT CChat::CancelTransfer()
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if (GetTransferType() != CChat::TransferType::None)
			{
				SetTransferType(CChat::TransferType::None);
				GetFile().Close();

				// Send the cancel notification.
				char pBuf[2];
				pBuf[0] = (BYTE)CommandFileCancel;
				pBuf[1] = (BYTE)CHAT_END_OF_COMMAND;
				GetSocket().SendData(pBuf, 2);
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::CancelTransfer\n"));
		}
		return hr;
	}

	string CChat::ToString()
	{
		stringstream buf;
		buf << _T("CChat(") << static_cast<LPCTSTR>(m_strName) << _T(", ")
			<< m_strAddress << _T(":") << m_nPort << _T(", ")
			<< m_strGroup << _T(")");
		return buf.str();
	}

	HRESULT CChat::SetupSocket(int id)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if (!GetSocket().Create())
				::OutputDebugString(_T("Error in CChat::SetupSocket GetSocket.Create\n"));

			if (!GetSocket().AsyncSelect(FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE))
				::OutputDebugString(_T("Error Setting the socket to non blocking\n"));
		//  TEST for successful connection!!
			int iSuccess;

			GetSocket().SetID(id);

			GetSocket().Connect(GetAddress(), GetPort());
			iSuccess = GetSocket().GetLastError();

			// no error or non blocking connection hasn't responded yet
			// 10035 is the usual response before connection is negotiated
			if(iSuccess == 0 || iSuccess == 10035)
			{
				//::OutputDebugString(_T("Success setting up socket\n"));
		                hr = S_OK;
			}
			else
			{
				std::string Message;
				ErrorHandlingUtils::GetErrorString((DWORD)iSuccess,Message);
				::OutputDebugString(Message.c_str());
			}

		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("com_Error in CChat::SetupSocket\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SetupSocket\n"));
		}
		return hr;
	}
	HRESULT CChat::CreateChat(PCTSTR szAddress, int nPort, CChat **ppChat, int id)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if(NULL == ppChat)
				ErrorHandlingUtils::TESTHR(E_POINTER);

			*ppChat = new CChat(szAddress, nPort);
			ErrorHandlingUtils::TESTHR((*ppChat)->SetupSocket(id));
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			if(E_POINTER != e.Error())
			{
				::OutputDebugString(_T("Error in CChat::CreateChat attempting safe delete\n"));
				::OutputDebugString(_T(e.ErrorMessage()));
				SAFE_DELETE(*ppChat);
			}

			ErrorHandlingUtils::ReportException(e);
			::OutputDebugString(_T("com_Error in CChat::CreateChat\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::CreateChat\n"));
		}
		return hr;
	}

	HRESULT CChat::SendCommand(byte cmd, const std::string &command)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			std::stringstream buffer;
			buffer
				<< cmd
				<< command
				<< CHAT_END_OF_COMMAND;

			ErrorHandlingUtils::TESTHR(SendData(buffer.str()));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendCommand\n"));
		}
		return hr;
	}

	HRESULT CChat::Snoop()
	{
		CString strText;
		strText.Format("%c%c",
			(BYTE)CommandSnoop,
			(BYTE)CHAT_END_OF_COMMAND);
		return GetSocket().SendData(strText);
	}

	HRESULT CChat::SendFile(
		PCTSTR szFilename, 
		IChatServerClient *pClient, 
		const CString &uploadPath)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if (IsSending() || IsReceiving())
			{
				CString s;
				s.LoadString(IDS_CHATSERVER_FILE_TRANSFER_ALREADY_IN_PROGRESS);

				pClient->PrintChatText(s);

				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CPath path(szFilename);
			if(path.IsRelative())
			{
				CPath realPath(uploadPath);
				realPath.Append(szFilename);
				path = realPath;
			}

			CFile &file = GetFile();
			CFileException fe;
			if (!file.Open(
				path.m_strPath,
				CFile::modeRead|CFile::typeBinary|CFile::shareDenyWrite,
				&fe))
			{
				CString strText;

				strText.Format(
					IDS_CHATSERVER_FILE_NOT_FOUND,
					path.m_strPath);

				pClient->PrintChatText(strText);

				ErrorHandlingUtils::TESTHR(E_FAIL);
			}

			SetTransferType(CChat::TransferType::Send);
			SetFileLength(static_cast<unsigned int>(GetFile().GetLength()));
			SetTransferStartTime(time(NULL));

			CString strText;
			strText.Format(_T("%c%s,%u%c"),
				(BYTE)CommandFileStart,
				GetFile().GetFileName(),
				GetFileLength(),		
				(BYTE)CHAT_END_OF_COMMAND);

			GetSocket().SendData(strText);

			CString s;
			s.LoadString(IDS_CHATSERVER_FILE_TRANSFER_STARTED);

			pClient->PrintChatText(s);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChat::SendFile\n"));
		}
		return SUCCEEDED(hr);
	}

	CString CChat::GetInfoString(int index)
	{
		CString strFlags;
		GetFlagsString(strFlags);

		CString strName;
		// Color the name to reflect the status of a transfer.
		switch(GetTransferType())
		{
		case CChat::TransferType::None:
			strName = (GetName().GetLength() > 20 ? GetName().Left(20) : GetName());
			break;

		case CChat::TransferType::Send:
			strName.Format("\x1b[1;32m%s\x1b[0;37m",
				(GetName().GetLength() > 20 ? (LPCSTR)GetName().Left(20) : (LPCSTR)GetName()));
			break;

		case CChat::TransferType::Receive:
			strName.Format("\x1b[1;34m%s\x1b[0;37m",
				(GetName().GetLength() > 20 ? (LPCSTR)GetName().Left(20) : (LPCSTR)GetName()));
			break;
		}

		// Figure out the padding of the name.
		int nNamePad = 20 - GetName().GetLength();
		if (nNamePad > 0)
			strName += CString(' ',nNamePad);

		CString strText;
		strText.Format(_T("%s%03d:%s %s %-15s %-5d %-15s %s\n"),
			ANSI_F_BOLDWHITE,
			index + 1,
			ANSI_RESET,
			(LPCSTR)strName,
			(LPCSTR)GetAddress(),
			GetPort(),
			(LPCSTR)GetGroup(),
			(LPCSTR)strFlags);
		return strText;
	}

	void CChat::SetId(int id)
	{
		GetSocket().SetID(id);
	}

}