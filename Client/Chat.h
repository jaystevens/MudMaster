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
#pragma warning ( disable : 4482)

#include "ChatSocket.h"

namespace MMScript
{
	class CActionList;
	class CAliasList;
	class CTriggerList;
	class CMacroList;
	class CVarList;
	class CEventList;
	class CSubList;
	class CGagList;
	class CHighList;
	class CUserLists;
	class CArrayList;
	class CMMStatusBar;
}

namespace MMChatServer
{
	class IChatServerClient;

	class CChat  
	{
	private:
		CChat(const CChat &);
		CChat &operator=(const CChat &);

	public:
		CChat();
		CChat(LPCTSTR pszAddress, UINT nPort);
		virtual ~CChat();

		typedef enum ConnectionState
		{
			Called,
			Connected,
			WaitForResponse,
			Negotiate,
			InvalidConnectionState,
		} ConnectionState;

		typedef enum TransferType
		{
			None,
			Send,
			Receive,
		} TransferType;

		bool GetAllowSnoop() const;
		bool GetCommands() const;
		bool GetIgnore() const;
		bool GetPrivate() const;
		bool GetServe() const;
		bool GetExcludeServe() const;
		bool GetSnooped() const;
		bool GetTransfers() const;
		int GetBytesWritten() const;
		ConnectionState GetConnectionState() const;
		int GetFileLength() const;
		int GetPort() const;
		TransferType GetTransferType() const;
		bool IsSending() const;
		bool IsReceiving() const;
		const CString &GetAddress() const;
		const CString &GetGroup() const;
		const CString &GetName() const;
		const CString &GetSearchGroup() const;
		const CString &GetSearchName() const;
		const CString &GetReportedAddress() const;
		const CString &GetVersion() const;
		time_t GetTransferStartTime() const;
		CFile &GetFile();
		CString GetInfoString(int index);

		void SetAllowSnoop(bool bAllowSnoop);
		void SetCommands(bool bCommands);
		void SetIgnore(bool bIgnore);
		void SetPrivate(bool bPrivate);
		void SetServe(bool bServe);
		void SetExcludeServe(bool bExcludeServe);
		void SetSnooped(bool bSnooped);
		void SetTransfers(bool bTransfers);
		void SetBytesWritten(int nBytesWritten);
		void SetConnectionState(ConnectionState state);
		void SetFileLength(int nFileLength);
		void SetPort(UINT nPort);
		void SetTransferType(TransferType type);
		void SetAddress(const CString &strAddress);
		void SetGroup(const CString &strGroup);
		void SetName(const CString &strName);
		void SetReportedAddress(const CString &strAddress);
		void SetVersion(const CString &strVersion);
		void SetTransferStartTime(time_t time);
		void SetId(int id);


		int ReceiveData(char *buffer, int bufferSize);
		HRESULT SendMessage(PCTSTR szMessage);
		HRESULT Close(const std::string &message);
		bool MatchesSearchName(LPCTSTR szName);
		bool MatchesAddress(PCTSTR szAddress, int nPort);
		bool MatchesSocketId(unsigned long id);

		HRESULT SendData(const std::string &data);
		HRESULT SendData(PCTSTR szData, int length);
		HRESULT SendDataToGroup(PCTSTR szData, PCTSTR szGroupName);

		HRESULT GetFlagsString(CString &strFlags);
		HRESULT CancelTransfer();

		HRESULT SendCommand(byte cmd, const std::string &command);

		HRESULT Snoop();
		HRESULT SendFile(
			PCTSTR szFilename, 
			IChatServerClient *pClient, 
			const CString &uploadPath);

		std::string ToString();

		static HRESULT CreateChat(PCTSTR szAddress, int nPort, CChat **ppChat, int id);
		CChatSocket &GetSocket();

	private:

		HRESULT SetupSocket(int id);
//		HRESULT SendGroupTriggers(const CString &groupName, MMScript::CTriggerList *pList, byte cmd, int &nSent);

		CString m_strName;			// Name of the person connected.
		CString m_strSearchName;	// Name to use when searching list.  This one is all lower case.
		CString m_strAddress;		// Address of person connected.  This is sent from the other machine.
		CString m_strSockAddress;	// This is the address reported by the socket.

		int m_nPort;				// Port connected to.
		CString m_strGroup;
		CString m_strSearchGroup;	// Name to user when searching list.

		// Status flags.
		bool    m_bPrivate;			// Flagged TRUE when other users should not be able to retrieve this address.
		bool	m_bIgnore;			// TRUE to ignore incoming chats.	
		bool	m_bCommands;		// TRUE to allow others to add commands to your command file.
		bool	m_bTransfers;		// TRUE to accept files from the person.
		bool	m_bExcludeServe;	// True when excluding this user from being served.

		bool	m_bServe;			// True when serving this user.
		bool	m_bAllowSnoop;		// TRUE when others can snoop my mud connection
		bool	m_bSnooped;			// TRUE when others are snooping you

		CString m_strVersion;			// Version of MM the person is using.

		ConnectionState m_ConnectionState;	// Defines above.  This keep track of
		// where we are at with the connection
		// negotiation.

		TransferType m_TransferType;	// See above for types.
		unsigned int m_nFileLength;		// Length of the file.
		unsigned int m_nBytesWritten;	// Bytes written so far.
		time_t  m_tTransferStart;

		CChatSocket m_Socket;				// Socket descriptor.
		CFile		m_file;					// the handle to the file that is being transferred
	};
}
