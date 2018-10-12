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
#include "StdAfx.h"
#include "ChatServer.h"
#include "Chat.h"
#include "ErrorHandling.h"
#include "resource.h"
#include "StringUtils.h"
#include "colors.h"
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <iterator>

#pragma warning(disable:4244)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int CHAT_BUF_SIZE = 5000;

using namespace std;
using namespace Utilities;
using namespace MudmasterColors;

namespace MMChatServer
{
	typedef boost::shared_ptr<CChat> CChatPtr;
	typedef std::vector<CChatPtr> ChatConnectionList;
	typedef ChatConnectionList::iterator ChatConnectionListIterator;
	typedef boost::shared_ptr<CChatSocket> ChatServerSocket;
	typedef std::vector<char> BufferType;
	typedef BufferType::size_type BufferIndex;

	namespace Functors
	{
		class ChatFilter
		{
		public:
			TCHAR operator()(TCHAR ch)
			{
				if(ch == 7 || ch == 8)
					return _T('*');

				return ch;
			}
		};

		class SortByName
		{
		public:
			bool operator()(CChatPtr p1, CChatPtr p2)
			{
				if(p1->GetSearchName().Compare(p2->GetSearchName()) < 0)
					return true;
				return false;
			}
		};



		class SameAddress
		{
		public:
			SameAddress(const string &address, const int &port)
				: _address(address)
				, _port(port)
			{}

			bool operator()(CChatPtr pChat)
			{
				TRACE(("%s"), pChat->ToString().c_str());
				return pChat->MatchesAddress(_address.c_str(), _port);
			}

		protected:
			const string &_address;
			const int &_port;
		};

		class SameSearchName
		{
		public:
			SameSearchName(const string &name)
				:_name(name){}

				bool operator()(CChatPtr pChat)
				{
					TRACE(_T("%s"), pChat->ToString().c_str());
					return pChat->MatchesSearchName(_name.c_str());
				}

		private:
			const string &_name;
		};

		class SameId
		{
		public:
			SameId(int &id)
				: _id(id)
			{
			}

			bool operator()(CChatPtr pChat)
			{
				TRACE(_T("%s"), pChat->ToString().c_str());
				return pChat->MatchesSocketId(_id);
			}

		private:
			const int &_id;
		};

		class SendData
		{
		public:
			SendData(const string &data)
				: _data(data)
			{
			}

			void operator()(CChatPtr pChat)
			{
				pChat->SendData(_data.c_str());
			}

		private:
			const string &_data;
		};

		class SetGroup
		{
		public:
			SetGroup(const string &groupName)
				: _groupName(groupName)
			{
			}

			void operator()(CChatPtr pChat)
			{
				pChat->SetGroup(_groupName.c_str());
			}

		private:
			const string &_groupName;
		};

		class SameGroup
		{
		public:
			SameGroup(const string &groupName)
				: _groupName(groupName)
			{
			}

			bool operator()(CChatPtr pChat) const
			{
				TRACE(_T("%s"), pChat->ToString().c_str());
				return pChat->GetSearchGroup().Compare(_groupName.c_str()) == 0;
			}

		private:
			const string &_groupName;
		};
		class SameGroupServed
		{
		public:
			SameGroupServed(const string &groupName, const string &chatFrom)
				: _groupNameServed(groupName),_chatFrom(chatFrom)
			{
			}

			bool operator()(CChatPtr pChat) const
			{
				TRACE(_T("%s samegroupserved\n"), pChat->ToString().c_str());
				return pChat->GetSearchGroup().Compare(_groupNameServed.c_str()) == 0 && pChat->GetServe()
						&& pChat->GetSearchName().Compare(_chatFrom.c_str()) != 0;
			}

		private:
			const string &_groupNameServed;
			const string &_chatFrom;
		};

		class IsServed
		{
		public:
			bool operator()(CChatPtr pChat) const
			{
				return pChat->GetServe();
			}
		};
		class IsExcludeServed
		{
		public:
			bool operator()(CChatPtr pChat) const
			{
				return pChat->GetExcludeServe();
			}
		};

		class NotSameSearchName
		{
		public:
			NotSameSearchName(const string &chatName)
				: _chatName(chatName)
			{
			}

			bool operator()(CChatPtr pChat) const
			{
				return pChat->GetSearchName().Compare(_chatName.c_str()) != 0;
			}

		private:
			const string &_chatName;
		};
		class NotSameSearchNameNotExcluded
		{
		public:
			NotSameSearchNameNotExcluded(const string &chatName)
				: _chatName(chatName)
			{
			}

			bool operator()(CChatPtr pChat) const
			{
				return pChat->GetSearchName().Compare(_chatName.c_str()) != 0 && !pChat->GetExcludeServe();
			}

		private:
			const string &_chatName;
		};
	}

	struct CChatServer::ChatServerImpl
	{
		ChatServerImpl()
			: _snoopCount(0)
			, _chatCount(1)
			, _chatParseIndex(0)
			, _pClient(NULL)
		{
			_chatBuffer.resize(CHAT_BUF_SIZE + 1);
			_sendBuffer.resize(CHAT_BLOCKSIZE + 2);
			_receiveBuffer.resize(CHAT_BUF_SIZE + 1);
		}

		int GetCount() const;
		HRESULT Connect( PCTSTR pszAddress, int nPort );
		HRESULT BroadcastNameChange();
		HRESULT GetIpAddress(std::string &address);
		HRESULT GetFileVersion(std::string &version);
		int GetPort() const;
		HRESULT GetSoundPath(std::string &strPath) const;
		bool GetAutoAccept() const;
		bool GetAutoServe() const;
		bool GetDoNotDisturb() const;
		BYTE GetChatForeColor() const;
		BYTE GetChatBackColor() const;
		HRESULT GetUploadPath(std::string &path) const;
		HRESULT GetDownloadPath(std::string &path) const;

		void Accept(CChatSocket *pSocket);
		HRESULT RemoveAll();
		HRESULT GetCloseMessage(std::string &out);
		HRESULT Remove(PCTSTR pszName);
		HRESULT Find(const CString& strParam, CChatPtr &pChat);
		HRESULT FindName(PCTSTR pszName, CChatPtr &pChat);
		HRESULT CloseChatConnection(CChatPtr pChat, PCTSTR szMessage);
		HRESULT FindAddress(PCTSTR pszAddress, UINT nPort, CChatPtr &pChat);
		HRESULT FindID(int id, CChatPtr &pChat);

		HRESULT PrintChatMessage(PCTSTR pszMsg);
		HRESULT PrintChatMessage(UINT nMsgId);
		HRESULT PrintSnoop(PCTSTR szSnoopData) const;
		HRESULT ExecuteCommand(PCTSTR szCommand);

		void AddChatConnection(CChatPtr pChat);

		HRESULT ChatPrint(PCTSTR pszText);
		HRESULT ChatPrint(UINT nStringId);

		void DoCommands(CChatPtr pChat, BufferType &buffer, int nLen);
		void GetPeekParam(CString &strText, CString &strParam);
		void Close(unsigned long lID);
		HRESULT Connection(WPARAM wParam, CChatSocket *pSocket);
		HRESULT Receive(unsigned long lID);

		HRESULT HandleWaitForResponseState(CChatPtr pChat, int length);
		HRESULT HandleNegotiate(CChatPtr pChat, int length);
		HRESULT CheckAccept(CChatPtr pChat, const CString &strName, bool &accept);

		int GetSnoopCount() const	{ return _snoopCount; }
		int GetChatCount() const	{ return _chatCount; }
		IChatServerClient *GetClient() const { return _pClient; }
		const CString &GetChatName() const { return _chatName; }
		bool isValidIndex(int index) const;

		void SetSnoopCount(int count) { _snoopCount = count; }
		void IncrementChatCount() { ++_chatCount; }
		void DecrementChatCount() { --_chatCount; }
		void DecrementSnoopCount() { --_snoopCount; }
		void IncrementSnoopCount() { ++_snoopCount; }

		void AcceptChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode);
		void CloseChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode);
		void ConnectChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode);
//TODO: KW typo for RecieveChatSocketEventHandler??
		void ReceiveChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode);
		void SendChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode);

		// DoCommands
		void ChangeName(CChatPtr pChat, PCTSTR pszBuf);
		void SendPublicPeek(CChatPtr pChat);
		void IncomingSnoop(CChatPtr pChat);
		void IncomingPeekList(PCTSTR pszBuf);
		HRESULT SendPublicConnections(CChatPtr pChat);
		void ProcessConnectionList(PCTSTR pszBuf);
		void IncomingChatEverybody(const CChatPtr pChat, PCTSTR pszBuf);
		void IncomingChatPersonal(const CChatPtr pChat, PCTSTR pszBuf);
		void IncomingChatGroup(const CChatPtr pChat, PCTSTR pszBuf);
		void IncomingMessage(PCTSTR pszBuf);
		void IncomingDnd(PCTSTR pszBuf);
		void IncomingVersion(CChatPtr pChat, PCTSTR pszBuf);
		void IncomingCommand(CChatPtr pChat, BYTE chCommand, PCTSTR pszBuf);
		void IncomingSnoopData(PCTSTR pszBuf);
		void IncomingPingRequest(CChatPtr pChat, PCTSTR pszBuf);
		void IncomingPingResponse(const CChatPtr pChat, PCTSTR pszBuf);
		void IncomingFileStart(CChatPtr pChat, PCTSTR pszBuf);
		void IncomingFileDeny(CChatPtr pChat, PCTSTR pszBuf);
		void IncomingFileBlockRequest(CChatPtr pChat);
		void IncomingFileBlock(CChatPtr pChat, PCTSTR pszBuf);
		void IncomingFileEnd(CChatPtr pChat);
		void IncomingFileCancel(CChatPtr pChat);

		void hookEvent(CChatPtr pChat);
		void unhookEvent(CChatPtr pChat);

		void serverHookEvent(ChatServerSocket pServerSocket);
		void serverUnhookEvents(ChatServerSocket pServerSocket);

		template<class T>
			void MultiSendData(const std::string &data, const T &pfn);

		HRESULT AnnounceConnection();

		ChatConnectionList _ptrList;

		int _snoopCount;		// the number of snooped connections
		int _chatCount;			// the current number of clients
		BufferIndex _chatParseIndex;	// the current index of the parse pointer
		void SetChatParseIndex(BufferIndex index) { _chatParseIndex = index; }
		BufferIndex GetParseIndex() const	{ return _chatParseIndex; }

		BufferType _receiveBuffer;
		BufferType _chatBuffer;
		BufferType _sendBuffer;

		IChatServerClient *_pClient;
		CString _chatName;

		ChatServerSocket _pSockListen;

		CString _errorString;
		void SetError(const CString &errorString);
	};

	class CloseChat
	{
	public:
		CloseChat(CChatServer::ChatServerImpl &server, const string &msg)
			: _server(server)
			, _message(msg)
		{
		}

		void operator()(CChatPtr pChat)
		{
			_server.unhookEvent(pChat);
			pChat->Close(_message);
		}

	protected:
		const string &_message;
		CChatServer::ChatServerImpl &_server;
	};

	CChatServer::CChatServer()
		: _pImpl(new ChatServerImpl)
	{
	}

	CChatServer::~CChatServer()
	{
		try
		{
			_pImpl->RemoveAll();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in chatserver destructor\n"));
		}
	}

	HRESULT CChatServer::ChatServerImpl::RemoveAll()
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			string closeMessage;
			ErrorHandlingUtils::TESTHR(GetCloseMessage(closeMessage));

			std::for_each(
				_ptrList.begin(), 
				_ptrList.end(), 
				CloseChat(*this, closeMessage));

			_ptrList.clear();
			SetSnoopCount(0);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Removeall\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::Remove(PCTSTR pszName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			string closeMessage;
			ErrorHandlingUtils::TESTHR(GetCloseMessage(closeMessage));

			CChatPtr pChat;
			HRESULT hr = Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);

			if(S_FALSE == hr)
			{
				ChatPrint(IDS_CHATSERVER_CHAT_CONNECTION_NOT_FOUND);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			ErrorHandlingUtils::TESTHR(CloseChatConnection(pChat, closeMessage.c_str()));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Remove\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::FindAddress(PCTSTR pszAddress, UINT nPort, CChatPtr &pChat)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			ChatConnectionListIterator it = 
				std::find_if(
					_ptrList.begin(), 
					_ptrList.end(), 
					Functors::SameAddress(pszAddress, nPort));
			//::OutputDebugString(_T("CChatServer::ChatServerImpl::FindAddress\n"));

			if(it != _ptrList.end())
			{
				pChat = *it;
				hr = S_OK;
				//::OutputDebugString(_T(" found connection in CChatServer::ChatServerImpl::FindAddress\n"));
			}
			else
				hr = S_FALSE;
				//::OutputDebugString(_T(" Did not find connection in CChatServer::ChatServerImpl::FindAddress\n"));
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::FindAddress\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::FindAddress\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::FindName(PCTSTR pszName, CChatPtr &pChat)
	{
		HRESULT hr = S_FALSE;

		try
		{
			string name;
			StringUtils::LowerString(pszName, name);

			ChatConnectionListIterator it = 
				std::find_if(
					_ptrList.begin(), 
					_ptrList.end(), 
					Functors::SameSearchName(name));
			//::OutputDebugString(_T(" looking for connection in CChatServer::ChatServerImpl::FindName\n"));
			if(it != _ptrList.end())
			{
				pChat = *it;
				hr = S_OK;
			//	::OutputDebugString(_T(" found connection in CChatServer::ChatServerImpl::FindName\n"));
			}
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::FindName\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::FindName\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::Find(const CString& strParam, CChatPtr &pChat)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			try
			{
				int nIndex = boost::lexical_cast<int>(static_cast<PCTSTR>(strParam));
				//TODO: KW if successful then param is numeric so try to find it in chat list
				//but make sure the nIndex is not bigger than the number of connections
				int vectorSize = _ptrList.size();
				if(nIndex < vectorSize +1)
				{
					pChat = _ptrList[nIndex-1];
					hr = S_OK;
				}
				else
					hr = S_FALSE;
			}
			catch(boost::bad_lexical_cast ble)
			{
				//::OutputDebugString(_T("not a number try FindName CChatServer::ChatServerImpl::Find\n"));
				hr = FindName(strParam, pChat);
			}
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::Find\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Find\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::FindID(int id, CChatPtr &pChat)
	{
		HRESULT hr = S_FALSE;
		try
		{
			ChatConnectionListIterator it = 
				std::find_if(
					_ptrList.begin(), 
					_ptrList.end(), 
					Functors::SameId(id));
			//::OutputDebugString(_T("looking for connection in CChatServer::ChatServerImpl::FindID\n"));

			if(it != _ptrList.end())
			{
				pChat = *it;
				hr = S_OK;
			//	::OutputDebugString(_T("found connection in CChatServer::ChatServerImpl::FindID\n"));
			}
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::FindID\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::FindID\n"));
		}


		return	hr;
	}

	HRESULT CChatServer::ChatServerImpl::Connect(PCTSTR pszAddress, int nPort)
	{
		HRESULT hr = E_UNEXPECTED;
		CString strMessage;
		try
		{
			if (!nPort)
				nPort = 4050;

			strMessage.Format(IDS_CHATSERVER_CALLING, pszAddress, nPort);
			PrintChatMessage(strMessage);

			CChat *pRawChat = NULL;
			strMessage = "Attempting to call CreateChat\n";
			//::OutputDebugString(_T("calling CreateChat in CChatServer::ChatServerImpl::Connect\n"));
			ErrorHandlingUtils::TESTHR(CChat::CreateChat(pszAddress, nPort, &pRawChat, GetChatCount()));
			
			strMessage = "Attempting to assign pChat as CChatPtr\n";
			CChatPtr pChat(pRawChat);
			
			strMessage = "HookingEvent to pChat\n";
			hookEvent(pChat);

			strMessage.Format("Incrementing Chat Count was %d before increment",GetChatCount());
			IncrementChatCount();

			strMessage.Format("Adding Chat Connection\n");
			AddChatConnection(pChat);
			
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			PrintChatMessage(_T("Error trying to connect, likely a bad address"));
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Connect\n"));
			::OutputDebugString(strMessage);
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			PrintChatMessage(_T("Unknown Error trying to connect"));
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Connect\n"));
			::OutputDebugString(strMessage);
		}


		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::Connection(WPARAM wParam, CChatSocket *pChatSocket)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			HRESULT hr = FindID(pChatSocket->GetID(), pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				PrintChatMessage(IDS_LOST_SOCKET_HANDLE);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			CString strText;
			if (wParam != 0)
			{
				std::string errorString;
				ErrorHandlingUtils::GetErrorString(wParam, errorString);
				strText.Format(IDS_CHATSERVER_CONNECTION_FAILED,
					pChat->GetAddress(),
					pChat->GetPort(), 
					errorString.c_str());

				PrintChatMessage(strText);

				CloseChatConnection(pChat, NULL);
			}
			else
			{
				std::string ipAddress;
				ErrorHandlingUtils::TESTHR(GetIpAddress(ipAddress));

				strText.Format(
					"CHAT:%s\n%s%-5u",
					GetChatName(),
					ipAddress.c_str(),
					GetPort());

				//TODO encapsulate the Chat Connection's socket
				ErrorHandlingUtils::TESTHR(pChat->GetSocket().SendData(strText));
				pChat->SetConnectionState(CChat::WaitForResponse);

				strText.Format(IDS_CHATSERVER_WAITING_FOR_RESPONSE,
					(LPCSTR)pChat->GetAddress(),
					pChat->GetPort());

				PrintChatMessage(strText);
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Connection\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Connection\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::HandleWaitForResponseState(CChatPtr pChat, int length)
	{
		length; // TODO fix this method!
		// fix by adding check for version after newline and handle it
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CString strText(&_receiveBuffer[0]);
			CString strMessage;
			if (strText.Left(2) == "NO")
			{
				strMessage.Format(IDS_CHATSERVER_CHAT_WAS_DENIED,(LPCSTR)pChat->GetAddress(),pChat->GetPort());
				PrintChatMessage(strMessage);
				CloseChatConnection(pChat, NULL);
			}
			else
			{
				pChat->SetConnectionState(CChat::Connected);

				CString strName;
				int nLen = strText.GetLength();
				int nIndex = 4;
				while(nIndex < nLen && strText.GetAt(nIndex) != '\n')
				{
					strName += strText.GetAt(nIndex);
					nIndex++;
				}
				pChat->SetName(strName);
		//now check if version was included in the response.
				nIndex++;
				if((BYTE)strText.GetAt(nIndex) == (BYTE)CommandVersion)
				{
					//assume the last character is end of command character
					_receiveBuffer[nLen - 1] = '\x0';
					nIndex++;
					IncomingVersion(pChat, &_receiveBuffer[nIndex]);
				}

				std::string version;
				ErrorHandlingUtils::TESTHR(GetFileVersion(version));

				std::stringstream buffer;
				buffer
					<< (BYTE)CommandVersion
					<< APP_NAME
					<< version
					<< (BYTE)CHAT_END_OF_COMMAND;

				// Send out the version number.
				pChat->SendData(buffer.str());
//finally announce accepted connection
				strMessage.Format(IDS_CHATSERVER_CONNECTION_ACCEPTED,
					(LPCSTR)pChat->GetName(),
					(LPCSTR)pChat->GetAddress(),
					pChat->GetPort());

				ChatPrint(strMessage);

			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::HandleWaitForResponseState\n"));
			::OutputDebugString(_T("Attempting to close failed chat connection\n"));
			CloseChatConnection(pChat, NULL);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::HandleWaitForResponseState\n"));
			::OutputDebugString(_T("Attempting to close failed chat connection\n"));
			CloseChatConnection(pChat, NULL);
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::CheckAccept(CChatPtr pChat, const CString &strName, bool &accept)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			accept = false;

			CString strText;
			strText.Format(IDS_CHATSERVER_ACCEPT_REQUEST_QUESTION,(LPCSTR)strName);
			CString strCaption;
			strCaption.LoadString(IDS_CHATSERVER_CHATREQUEST);

			int nRet = ::MessageBox(NULL, strText, strCaption,MB_ICONQUESTION|MB_YESNO);

			if (IDNO == nRet)
			{
				pChat->SendData("NO");
				CloseChatConnection(pChat, NULL);
			}
			else
			{
				std::stringstream buffer;
				buffer
					<< "YES:"
					<< static_cast<LPCTSTR>(GetChatName())
					<< "\n";

				pChat->SendData(buffer.str());
				accept = true;
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CheckAccept\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CheckAccept\n"));
		}

		return hr;
	}
	HRESULT CChatServer::ChatServerImpl::HandleNegotiate(CChatPtr pChat, int length)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			static const std::string NEGOTIATE_HEADER = "CHAT:";

			std::string buffer;
			buffer.assign(_receiveBuffer.begin(), _receiveBuffer.begin() + length);
			if(buffer.length() < NEGOTIATE_HEADER.length() + 1)
			{
				PrintChatMessage(IDS_CHATSERVER_NEGOTIATION_FAILED);
				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			std::string header = buffer.substr(0, NEGOTIATE_HEADER.length());
			if (header != NEGOTIATE_HEADER)
			{
				PrintChatMessage(IDS_CHATSERVER_NEGOTIATION_FAILED);
				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			buffer = buffer.substr(NEGOTIATE_HEADER.length());

			std::string name;
			std::string::size_type newLineIndex = buffer.find('\n');
			if(std::string::npos != newLineIndex)
				name = buffer.substr(0, newLineIndex);

			name = trim(name);
			if (name.empty())
			{
				CString temp1;
				temp1.LoadString(IDS_CHATSERVER_NO_NAME_PROVIDED);

				CString temp2;
				temp2.Format(IDS_CHATSERVER_NEGOTIATION_FAILED_S, temp1);
				PrintChatMessage(temp2);

				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			// Real machine address.  This is the actual address, not the
			// address that a firewall would nomally scramble.  Should fix
			// the problems I've been having with firewalls giving me a
			// different address than the machine really has.
			std::string address = buffer.substr(newLineIndex + 1);

			static const int PORT_LENGTH = 5;
			if(address.length() < PORT_LENGTH)
			{
				CString temp1;
				temp1.Format(IDS_CHATSERVER_ADDRESS_SEGMENT_CORRUPT);

				CString s;
				s.Format(IDS_CHATSERVER_NEGOTIATION_FAILED_S, temp1);
				PrintChatMessage(s);
				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			std::string portBuffer = address.substr(address.length() - PORT_LENGTH);
			address = address.substr(0, address.length()-PORT_LENGTH);

			portBuffer = trim(portBuffer);
			address = trim(address);

			if(portBuffer.empty() || address.empty())
			{
				CString temp1;
				temp1.Format(IDS_CHATSERVER_ADDRESS_SEGMENT_CORRUPT);

				CString s;
				s.Format(IDS_CHATSERVER_NEGOTIATION_FAILED_S, temp1);
				PrintChatMessage(s);
				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			pChat->SetAddress(address.c_str());

			int port = 0;
			try
			{
				port = boost::lexical_cast<int>(portBuffer);
			}
			catch(boost::bad_lexical_cast ble)
			{
				CString temp1;
				temp1.Format(IDS_CHATSERVER_ADDRESS_SEGMENT_CORRUPT);

				CString s;
				s.Format(IDS_CHATSERVER_NEGOTIATION_FAILED_S, temp1);
				PrintChatMessage(s);
				CloseChatConnection(pChat, NULL);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);			
			}

			pChat->SetPort(port);

			AnnounceConnection();

			bool accept = false;
			if (!GetAutoAccept())
			{
				ErrorHandlingUtils::TESTHR(CheckAccept(pChat, name.c_str(), accept));
			}
			else
			{
				CString strText;
				//strText.Format(IDS_CHATSERVER_CHAT_REQUEST_ACCEPTED, 
				//	(LPCSTR)name.c_str(), 
				//	pChat->GetAddress(), 
				//	pChat->GetPort());
				//ChatPrint(strText);
				strText.Format("YES:%s\n",(LPCSTR)GetChatName());
				pChat->GetSocket().SendData(strText);
				accept = true;
			}

			if(accept)
			{
				pChat->SetName(name.c_str());
				pChat->SetConnectionState(CChat::Connected);

				std::string version;
				ErrorHandlingUtils::TESTHR(GetFileVersion(version));

				// Send out the version number.
				
				std::stringstream buffer;
				buffer
					<< (BYTE)CommandVersion
					<< APP_NAME
					<< version
					<< (BYTE)CHAT_END_OF_COMMAND;

				pChat->SendData(buffer.str());
				//announce accepted connection after it is finished connecting			
				CString strText,strMessage;
				strText.Format(IDS_CHATSERVER_CHAT_REQUEST_ACCEPTED, 
					(LPCSTR)name.c_str(), 
					pChat->GetAddress(), 
					pChat->GetPort());
				ChatPrint(strText);
//				 announce the connection in taskbarnotifier
//				strText.Format("/showtoast {%s connected to chat}",(LPCSTR)name.c_str());
//				ExecuteCommand(strText);
				if (GetAutoServe())
					{
						pChat->SetServe(true);
						strText.Format(IDS_CHATSERVER_S_IS_BEING_SERVED, pChat->GetName());
						ChatPrint(strText);
						strMessage.Format(
							IDS_CHATSERVER_YOU_ARE_BEING_SERVED_BY_S, 
							(LPCSTR)GetChatName());
						pChat->SendMessage(strMessage);
					}

			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::HandleNegotiate\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::HandleNegotiate\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::Receive(unsigned long lID)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = FindID(lID, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				PrintChatMessage(IDS_LOST_SOCKET_HANDLE);
				ErrorHandlingUtils::TESTHR(E_UNEXPECTED);
			}

			//TODO I believe that I have a bug here in regards to the receive buffer
			// it appears that I'm assuming that I get a full message for every read
			// and I'm not certain that this is always true...
			int nLen = pChat->ReceiveData(&_receiveBuffer[0], CHAT_BUF_SIZE);
			if (nLen == SOCKET_ERROR)
			{
				//Probably should clean up the chat connection if there is a socket error

				::OutputDebugString(_T("Socket Error in CChatServer::ChatServerImpl::Receive\n"));
				::OutputDebugString(_T("Will try to close this connection\n"));
				CloseChatConnection(pChat, NULL);
			}
			else
			{
				if (nLen == CHAT_BUF_SIZE) 
					_receiveBuffer[nLen - 1] = (BYTE)CHAT_END_OF_COMMAND;
				_receiveBuffer[nLen] = '\x0';

				switch(pChat->GetConnectionState())
				{
				case CChat::WaitForResponse:
					HandleWaitForResponseState(pChat, nLen);
					break;
				case CChat::Negotiate:
					HandleNegotiate(pChat, nLen);
					break;
				case CChat::Connected:
					DoCommands(pChat, _receiveBuffer, nLen);
					break;
				default:
					ATLASSERT(!"Invalid connection state!");
				}
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::Receive\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Receive\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::Close(unsigned long lID)
	{
		try
		{
			CString strText;
			CChatPtr pChat;
			strText.Format("CChatServer::ChatServerImpl::Close ID= %d\n",lID);
			::OutputDebugString(strText);
			HRESULT hr = FindID(lID, pChat);
			if(S_FALSE == hr)
			{
				PrintChatMessage(IDS_CHATSERVER_LOST_SOCKET_HANDLE);
				return;
			}

			
			strText.Format(IDS_CHATSERVER_CHAT_CONNECTION_LOST,
				(LPCSTR)pChat->GetName());
			ChatPrint(strText);
			CloseChatConnection(pChat, NULL);
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Close\n"));
		}

	}

	void CChatServer::ChatServerImpl::Accept(CChatSocket *pSocket)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChat *pRawChat = new CChat;
			if(NULL == pRawChat)
				ErrorHandlingUtils::TESTHR(E_OUTOFMEMORY);

			::OutputDebugString(_T("using pRawChat to create pChat CChatServer::ChatServerImpl::Accept\n"));
			CChatPtr pChat(pRawChat);

			hookEvent(pChat);

			pChat->SetId(GetChatCount());
			::OutputDebugString(_T("Incrementing chat count in CChatServer::ChatServerImpl::Accept\n"));
			IncrementChatCount();


			SOCKADDR_IN sockAddr;
			int nAddressLen = sizeof(sockAddr);
			if (!pSocket->Accept(pChat->GetSocket(),(SOCKADDR *)&sockAddr, &nAddressLen))
			{
				CString strMessage;
				strMessage.LoadString(IDS_CHATSERVER_FAILED_TO_ACCEPT_SOCKET);
				AfxGetMainWnd()->MessageBox(
					strMessage,
					APP_NAME,
					MB_ICONINFORMATION);
				if(pChat->GetSnooped())
					DecrementSnoopCount();
				//TODO: KW since chatcount was incremented before the failure to connect
				//      decrement it here
				::OutputDebugString(_T("decrementing chat count in CChatServer::ChatServerImpl::Accept\n"));
				DecrementChatCount();
			}
			else 
			{
				if (!pChat->GetSocket().AsyncSelect(FD_READ|FD_CONNECT|FD_CLOSE|FD_WRITE))
				{
					CString strMessage;
					strMessage.LoadString(IDS_CHATSERVER_FAILED_TO_SET_NONBLOCK);

					AfxGetMainWnd()->MessageBox(
						strMessage,
						APP_NAME,
						MB_ICONINFORMATION);

					if(pChat->GetSnooped())
						DecrementSnoopCount();
				//TODO: KW since chatcount was incremented before the failure to connect
				//      decrement it here
				::OutputDebugString(_T("decrementing chat count in CChatServer::ChatServerImpl::Accept\n"));
				DecrementChatCount();

				}
				else
				{
					if (GetDoNotDisturb())
					{
						pChat->GetSocket().Close();
					}
					else
					{
						pChat->SetReportedAddress(inet_ntoa(sockAddr.sin_addr));
						pChat->SetConnectionState(CChat::Negotiate);

						CString strMessage;
						strMessage.LoadString(IDS_CHATSERVER_NEGOTIATING_INCOMING_CHAT);
						ChatPrint(strMessage);
//TODO: KW should this be new pChat??
						AddChatConnection(CChatPtr(pChat));
					}
				}
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::Accept\n"));
			//Wonder if this decrement is handled by the if even with this error
			::OutputDebugString(_T("decrementing chat count in CChatServer::ChatServerImpl::Accept\n"));
			DecrementChatCount();
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Accept\n"));
			//Wonder if this decrement is handled by the if even with this error
			::OutputDebugString(_T("decrementing chat count in CChatServer::ChatServerImpl::Accept\n"));
			DecrementChatCount();
		}

	}

	HRESULT CChatServer::AddGroup(PCTSTR pszName, PCTSTR pszGroup)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CString strName(pszName);
			strName.MakeLower();

			bool setAll = strName.Compare(_T("*")) == 0;
			if(setAll)
			{
				std::for_each(
					_pImpl->_ptrList.begin(), 
					_pImpl->_ptrList.end(), 
					Functors::SetGroup(pszGroup));
			}
			else
			{
				CChatPtr pChat;
				HRESULT hr = _pImpl->Find(pszName, pChat);
				ErrorHandlingUtils::TESTHR(hr);
				if(S_FALSE == hr)
				{
					CString s;
					s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
					_pImpl->PrintChatMessage(s);
					ErrorHandlingUtils::TESTHR(E_INVALIDARG);
				}

				pChat->SetGroup(pszGroup);
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::AddGroup\n"));
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::AddGroup\n"));
		}

		return hr;
	}

	HRESULT CChatServer::RemoveGroup(PCTSTR pszName)
	{
		return AddGroup(pszName, _T(""));
	}

	HRESULT CChatServer::ChatServerImpl::ChatPrint(PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CString strTemp;
			strTemp.Format("%s%s%s%s%s", 
				ANSI_RESET,
				ANSI_FORE_COLORS[GetChatForeColor()], 
				ANSI_BACK_COLORS[GetChatBackColor()],
				pszText, 
				ANSI_RESET);

			ErrorHandlingUtils::TESTHR(GetClient()->PrintChatText(strTemp));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::ChatPrint\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatPrint\n"));
		}

		return hr;
	}

	HRESULT CChatServer::SendChatToEverybody(PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if (_pImpl->_ptrList.empty())
			{
				CString temp;
				temp.LoadString(IDS_CHATSERVER_YOU_DO_NOT_HAVE_CONNECTIONS);
				_pImpl->PrintChatMessage(temp);
			}
			else
			{
				string strLine(pszText);
				std::transform(
					strLine.begin(), 
					strLine.end(), 
					strLine.begin(), 
					Functors::ChatFilter());

				CString strText;
				strText.Format(
					IDS_CHATSERVER_YOU_CHAT_TO_EVERYONE, 
					strLine.c_str());

				_pImpl->ChatPrint(strText);

				strText.Format(
					IDS_CHATSERVER_CHATS_TO_EVERYONE,
					(BYTE)CommandTextEveryone,
					(LPCSTR)_pImpl->GetChatName(),
					ANSI_RESET,
					strLine.c_str(),
					ANSI_BOLD ANSI_F_RED,
					(BYTE)CHAT_END_OF_COMMAND);

				std::for_each(
					_pImpl->_ptrList.begin(), 
					_pImpl->_ptrList.end(), 
					Functors::SendData(static_cast<PCTSTR>(strText)));
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::SendToEverybody\n"));
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendToEverybody\n"));
		}

		return hr;
	}

	HRESULT CChatServer::SendChatToGroup(PCTSTR pszGroup, PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			string strLine(pszText);
			std::transform(
				strLine.begin(), 
				strLine.end(), 
				strLine.begin(), 
				Functors::ChatFilter());

			CString strGroup(pszGroup);
			strGroup.MakeLower();

			CString strText;
			strText.Format(IDS_CHATSERVER_YOU_CHAT_TO_GROUP,
				ANSI_BOLD ANSI_F_CYAN,
				pszGroup, 
				ANSI_BOLD ANSI_F_RED,
				ANSI_RESET,
				strLine.c_str(),
				ANSI_BOLD ANSI_F_RED);

			_pImpl->ChatPrint(strText);
//(LPCSTR)strGroup,"%c%s              %s%s%c"
			//      %c%-14s%s chats to the group, '%s'%c\n
			strText.Format(IDS_CHATSERVER_CHATS_TO_GROUP,
				(BYTE)CommandTextGroup,
				strGroup,
				_pImpl->GetChatName(),
				strLine.c_str(),
				(BYTE)CHAT_END_OF_COMMAND);

			const string groupName = static_cast<PCTSTR>(strGroup);
			const string data = static_cast<PCTSTR>(strText);

			_pImpl->MultiSendData(data, Functors::SameGroup(groupName));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendChatToGroup\n"));
		}

		return hr;
	}

	HRESULT CChatServer::SendChatToPerson(PCTSTR pszName, PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				::OutputDebugString(_T("connection not found in CChatServer::ChatServerImpl::SendChatToPerson\n"));
			}
			else
			{
				CString strLine(pszText);
				CString strText;
				strText.Format(IDS_CHATSERVER_YOU_CHAT_TO_PERSON,
					(LPCSTR)pChat->GetName(),
					(LPCSTR)strLine);
				_pImpl->ChatPrint(strText);

				strText.Format(IDS_CHATSERVER_CHATS_TO_YOU,
					(BYTE)CommandTextPersonal,
					_pImpl->GetChatName(),
					(LPCSTR)strLine,
					(BYTE)CHAT_END_OF_COMMAND);

				std::string data = static_cast<LPCTSTR>( strText );
				hr = pChat->SendData(data);

			}
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::SendChatToPerson\n"));
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendChatToPerson\n"));
		}

		return hr;
	}

	HRESULT CChatServer::SendEmoteToEverybody(PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if (_pImpl->_ptrList.empty())
			{
				CString temp;
				temp.LoadString(IDS_CHATSERVER_YOU_DO_NOT_HAVE_CONNECTIONS);
				_pImpl->PrintChatMessage(temp);
			}
			else
			{
				string strLine(pszText);
				std::transform(
					strLine.begin(), 
					strLine.end(), 
					strLine.begin(), 
					Functors::ChatFilter());

				CString strText;
				strText.Format(IDS_CHATSERVER_YOU_EMOTE_TO_EVERYONE,
					_pImpl->GetChatName(),
					strLine.c_str());
				_pImpl->ChatPrint(strText);

				strText.Format("%c%s %s\n%c",
					(BYTE)CommandTextEveryone,
					_pImpl->GetChatName(),
					strLine.c_str(),
					(BYTE)CHAT_END_OF_COMMAND);

				std::for_each(
					_pImpl->_ptrList.begin(), 
					_pImpl->_ptrList.end(), 
					Functors::SendData(static_cast<PCTSTR>(strText)));
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::SendEmoteToEverybody\n"));
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendEmoteToEverybody\n"));
		}

		return	hr;
	}

	HRESULT CChatServer::SendEmoteToGroup(PCTSTR pszGroup, PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			string strLine(pszText);
			std::transform(
				strLine.begin(), 
				strLine.end(), 
				strLine.begin(), 
				Functors::ChatFilter());

			CString strGroup(pszGroup);
			strGroup.MakeLower();

			CString strText;
			strText.Format(IDS_CHATSERVER_YOU_EMOTE_TO_GROUP,
				pszGroup, 
				_pImpl->GetChatName(),
				strLine.c_str());
			_pImpl->ChatPrint(strText);

			strText.Format(IDS_CHATSERVER_EMOTES_TO_GROUP,
				(BYTE)CommandTextGroup,
				(LPCSTR)strGroup,
				_pImpl->GetChatName(),
				strLine.c_str(),
				(BYTE)CHAT_END_OF_COMMAND);

			string groupName = static_cast<PCTSTR>(strGroup);
			string data = static_cast<PCTSTR>(strText);

			_pImpl->MultiSendData(data, Functors::SameGroup(groupName));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendEmoteToGroup\n"));
		}

		return	hr;
	}

	HRESULT CChatServer::SendEmoteToPerson(PCTSTR pszName, PCTSTR pszText)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			string strLine(pszText);
			std::transform(
				strLine.begin(), 
				strLine.end(), 
				strLine.begin(), 
				Functors::ChatFilter());

			CString strText;
			strText.Format(IDS_CHATSERVER_YOU_EMOTE_TO_PERSON,
				(LPCSTR)pChat->GetName(),
				_pImpl->GetChatName(),
				strLine.c_str());
			_pImpl->ChatPrint(strText);

			strText.Format(IDS_CHATSERVER_EMOTES_TO_YOU,
				(BYTE)CommandTextPersonal,
				_pImpl->GetChatName(),
				strLine.c_str(),
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			ErrorHandlingUtils::TESTHR( pChat->SendData( data ) );

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendEmoteToPerson\n"));
		}

		return	hr;
	}

	HRESULT CChatServer::ChatServerImpl::BroadcastNameChange()
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CString strText;
			strText.Format("%c%s%c",
				(BYTE)CommandNameChange,
				GetChatName(),
				(BYTE)CHAT_END_OF_COMMAND);

			std::for_each(
				_ptrList.begin(), 
				_ptrList.end(), 
				Functors::SendData(static_cast<PCTSTR>(strText)));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::BroadCastNameChange\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::DoCommands(CChatPtr pChat, BufferType &buffer, int nLen)
	{
		BYTE chCommand;
		int nIndex = 0;
		while(GetParseIndex() < CHAT_BUF_SIZE && nIndex < nLen)
		{
			if ((BYTE)(buffer[nIndex]) == CHAT_END_OF_COMMAND)
			{
				_chatBuffer[GetParseIndex()] = '\x0';

				chCommand = _chatBuffer[0];
				switch(chCommand)
				{
				case CommandNameChange :
					ChangeName(
						pChat, 
						&_chatBuffer[1]);
					break;

				case CommandPeekConnections:
					SendPublicPeek(pChat);
					break;

				case CommandSnoop:
					IncomingSnoop(pChat);
					break;

				case CommandSnoopData:
					IncomingSnoopData(&_chatBuffer[1]);
					break;

				case CommandPeekList:
					IncomingPeekList(&_chatBuffer[1]);
					break;

				case CommandRequestConnections:
					SendPublicConnections(pChat);
					break;

				case CommandConnectionList:
					ProcessConnectionList(&_chatBuffer[1]);
					break;

				case CommandTextEveryone:
					IncomingChatEverybody(pChat, &_chatBuffer[1]);
					break;

				case CommandTextPersonal:
					IncomingChatPersonal(pChat, &_chatBuffer[1]);
					break;

				case CommandTextGroup:
					IncomingChatGroup(pChat, &_chatBuffer[1]);
					break;

				case CommandMessage:
					IncomingMessage(&_chatBuffer[1]);
					break;

				case CommandDoNotDisturb:
					IncomingDnd(&_chatBuffer[1]);
					break;

				case CommandVersion:
					IncomingVersion(pChat, &_chatBuffer[1]);
					break;

				case CommandPingRequest:
					IncomingPingRequest(pChat, &_chatBuffer[1]);
					break;

				case CommandPingResponse:
					IncomingPingResponse(pChat, &_chatBuffer[1]);
					break;

				case CommandFileStart:
					IncomingFileStart(pChat, &_chatBuffer[1]);
					break;

				case CommandFileDeny:
					IncomingFileDeny(pChat, &_chatBuffer[1]);
					break;

				case CommandFileBlockRequest:
					IncomingFileBlockRequest(pChat);
					break;

				case CommandFileBlock:
					IncomingFileBlock(pChat, &_chatBuffer[1]);
					break;

				case CommandFileEnd:
					IncomingFileEnd(pChat);
					break;

				case CommandFileCancel:
					IncomingFileCancel(pChat);
					break;

				case CommandTypeAction:
				case CommandTypeAlias:
				case CommandTypeMacro: 
				case CommandTypeVariable:
				case CommandTypeEvent:
				case CommandTypeGag:
				case CommandTypeHighlight:
				case CommandTypeList:
				case CommandTypeArray:
				case CommandTypeBarItem:
				case CommandTypeSubstitute:
					IncomingCommand(pChat,  chCommand, &_chatBuffer[1]);
					break;

				} // switch(chCommand)

				SetChatParseIndex(0);
				nIndex++;
				continue;

			} // if ((BYTE)(*(pszBuf+nIndex)) == CHAT_END_OF_COMMAND)

			_chatBuffer[GetParseIndex()] = buffer[nIndex];

			// If we are receiving a block, jump to the end of the block size.
			// There may be data in the block that looks like th end of command
			// character and need to skip over it.
			if (!GetParseIndex() && _chatBuffer[GetParseIndex()] == CommandFileBlock)
			{
				// Copy the next blocksize chars into the chat buf.
				ASSERT(GetParseIndex() + 1 < _chatBuffer.size());

				_chatBuffer.insert(_chatBuffer.begin() + GetParseIndex() + 1,
					buffer.begin() + nIndex + 1, buffer.begin() + nIndex + 1 + CHAT_BLOCKSIZE);

				SetChatParseIndex(GetParseIndex() + CHAT_BLOCKSIZE);
				nIndex += CHAT_BLOCKSIZE;

				// The normal increment below should then point the buffer
				// at the end of command char.
			}

			SetChatParseIndex(GetParseIndex() + 1);
			nIndex++;

		} // while(m_nChatIndex < CHAT_BUF_SIZE && nIndex < nLen)

		if (GetParseIndex() >= CHAT_BUF_SIZE)
			SetChatParseIndex(0);
	}

	void CChatServer::ChatServerImpl::ChangeName(CChatPtr pChat, PCTSTR pszBuf)
	{
		if (!pChat->GetIgnore())
		{
			CString strText;
			strText.Format(
				IDS_CHATSERVER_NAME_CHANGE,
				static_cast<LPCTSTR>(pChat->GetName()),
				pszBuf);

			ChatPrint(strText);
		}

		pChat->SetName(pszBuf);
	}

	HRESULT CChatServer::RequestConnects(PCTSTR pszName)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			std::stringstream buffer;

			buffer
				<< (BYTE)CommandRequestConnections
				<< CHAT_END_OF_COMMAND;

			ErrorHandlingUtils::TESTHR( pChat->SendData( buffer.str() ) );

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::RequestConnects\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::SendPublicConnections(CChatPtr pChat)
	{
		HRESULT hr =E_UNEXPECTED;
		try
		{
			if(!pChat->GetIgnore())
			{
				CString strText;
				strText.Format(IDS_CHATSERVER_REQUESTED_YOUR_PUBLIC_CONNECTIONS,
					(LPCSTR)pChat->GetName());
				ChatPrint(strText);

				CString strConnects;
				CString strTemp;
				CString strAddress;

				ChatConnectionListIterator it = _ptrList.begin();
				for(; it != _ptrList.end(); ++it)
				{
					CChatPtr pTemp = *it;
					if(!pChat->MatchesSearchName(pTemp->GetSearchName()) && !pTemp->GetPrivate())
					{
						if (!strConnects.IsEmpty())
							strConnects += ",";

						if (pTemp->GetAddress().GetLength() < 8)
							strAddress = "<Unknown>";
						else
							strAddress = pTemp->GetAddress();

						strTemp.Format("%s,%u",
							(LPCSTR)strAddress,
							pTemp->GetPort());

						strConnects += strTemp;
					}
				}

				if (!strConnects.IsEmpty())
				{
					std::stringstream buffer;
					buffer
						<< (BYTE)CommandConnectionList
						<< static_cast<LPCTSTR>(strConnects)
						<< (BYTE)CHAT_END_OF_COMMAND;

					ErrorHandlingUtils::TESTHR( pChat->SendData( buffer.str() ) );
				}
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
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendPublicConnections\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::ProcessConnectionList(PCTSTR pszBuf)
	{
		string buffer(pszBuf);

		std::vector<string> tokens;
		StringUtils::split(buffer, ",", std::back_inserter(tokens));

		std::vector<string>::size_type numConnections = tokens.size() / 2;
		if(numConnections == 0 && !tokens.empty())
			++numConnections;

		CString strText;
		strText.Format(IDS_CHATSERVER_CONNECTIONS_RETURNED, numConnections);
		ChatPrint(strText);

		for( std::vector<string>::size_type i = 0; i < numConnections; ++i)
		{
			std::string address = tokens[i];
			std::string portString;

			if(tokens.size() > i+1)
				portString = tokens[i+1];

			int port = 0;
			try
			{
				port = boost::lexical_cast<int>(portString);
			}
			catch(boost::bad_lexical_cast ble)
			{
				::OutputDebugString(_T("Invalid port value!\n"));
			}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ProcessConnectList\n"));
		}

			CChatPtr pChat;
			HRESULT hr = FindAddress(address.c_str(), port, pChat);
			if(SUCCEEDED(hr) && S_FALSE != hr)
			{
				strText.Format(
					IDS_CHATSERVER_ALREADY_CONNECTED_WITH,
					address.c_str(),
					portString.c_str());

				PrintChatMessage(strText);
			}
			else if(SUCCEEDED(hr))
			{
				string myAddress;
				ErrorHandlingUtils::TESTHR(GetIpAddress(myAddress));
				if(address != myAddress || port != GetPort())
				{
					Connect(address.c_str(), port);
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}

	void CChatServer::ChatServerImpl::IncomingChatGroup(const CChatPtr pChat, PCTSTR pszBuf)
	{	
		CString strChat;
		CString strTemp;
		strTemp = pszBuf+15;
		strTemp.TrimLeft();
		strChat += strTemp;

		if (!pChat->GetIgnore())
			ChatPrint(strChat);
		//test chatting to group ifserved
		strTemp.Format("%c%s%c",
			(BYTE)CommandTextEveryone,
			strChat,
			(BYTE)CHAT_END_OF_COMMAND);

			string data = static_cast<PCTSTR>(strTemp);
			string chatGroupName = static_cast<PCTSTR>(pChat->GetSearchGroup());
			string chatName = static_cast<PCTSTR>(pChat->GetSearchName());

			MultiSendData(data, Functors::SameGroupServed(chatGroupName, chatName));

	}

	void CChatServer::ChatServerImpl::IncomingChatPersonal(const CChatPtr pChat, PCTSTR pszBuf)
	{
		if (!pChat->GetIgnore())
			ChatPrint(pszBuf);
	}

	void CChatServer::ChatServerImpl::IncomingChatEverybody(const CChatPtr pChat, PCTSTR pszBuf)
	{
		CString strTemp;
		strTemp = pszBuf;
		strTemp.TrimLeft();

		if (!pChat->GetIgnore())
		{
			ChatPrint(pszBuf);
		}

		// If we are serving this chat connection, need to echo
		// the chat to everybody.  
		if (pChat->GetServe())
		{
			CString strText;
			strText.Format("%c%s%c", 			
				(BYTE)CommandTextEveryone,
				pszBuf,
				(BYTE)CHAT_END_OF_COMMAND);

			string data = static_cast<PCTSTR>(strText);
			string chatName = static_cast<PCTSTR>(pChat->GetSearchName());
			MultiSendData(data, Functors::NotSameSearchNameNotExcluded(chatName));
		}
		else
		{
			// If not serving see if there are any others we ARE serving
			// and need to echo this too.
			CString strText;
			strText.Format("%c%s%c",
				(BYTE)CommandTextEveryone,
				pszBuf,
				(BYTE)CHAT_END_OF_COMMAND);

			string data = static_cast<PCTSTR>(strText);
			MultiSendData(data, Functors::IsServed());
		}
	}

	void CChatServer::ChatServerImpl::IncomingMessage(PCTSTR pszBuf)
	{
		ChatPrint(pszBuf);
	}

	void CChatServer::ChatServerImpl::IncomingDnd(PCTSTR pszBuf)
	{
		CString strText;
		strText.Format(IDS_CHATSERVER_DND_IS_ON,pszBuf);
		ChatPrint(strText); 
	}

	void CChatServer::ChatServerImpl::IncomingVersion(CChatPtr pChat, PCTSTR pszBuf)
	{
		pChat->SetVersion(pszBuf);
	}

	HRESULT CChatServer::SendFile(
		PCTSTR pszName, 
		PCTSTR pszFilename)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			std::string uploadPath;
			ErrorHandlingUtils::TESTHR(_pImpl->GetUploadPath(uploadPath));
			ErrorHandlingUtils::TESTHR(pChat->SendFile(
				pszFilename, 
				_pImpl->GetClient(), 
				uploadPath.c_str()));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendFile\n"));
		}

		return hr;
	}

	HRESULT CChatServer::CancelTransfer( PCTSTR pszName )
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CString strName(pszName);
			strName.MakeLower();

			CChatPtr pChat;
			HRESULT hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{ 
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			pChat->CancelTransfer();

			_pImpl->ChatPrint(IDS_CHATSERVER_FILE_TRANSFER_CANCELED);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CancelTransfer\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::IncomingFileStart(CChatPtr pChat, PCTSTR pszBuf)
	{
		CString strText;
		// Make sure we are accepting transfers.
		if (!pChat->GetTransfers())
		{
			strText.Format(IDS_CHATSERVER_NOT_ALLOWING_TRANSFERS,
				(BYTE)CommandFileDeny,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			pChat->SendData(data);
		}
		else if (pChat->GetTransferType() != CChat::TransferType::None)
		{
			strText.Format(IDS_CHATSERVER_CURRENTLY_TRANSFERRING,
				(BYTE)CommandFileDeny,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			pChat->SendData(data);
		}
		else
		{
			bool packet_good = false;

			string fileName;
			UINT fileSize = 0;
			string buffer(pszBuf);
			string::size_type pos = buffer.find(",");
			if(pos != string::npos)
			{
				fileName = buffer.substr(0, pos);
				buffer = buffer.substr(pos+1);
				try
				{
					fileSize = boost::lexical_cast<UINT>(buffer);
					packet_good = true;
				}
				catch(bad_cast bce)
				{
					::OutputDebugString(_T("bad cast on file size"));
				}
			}

			if (!packet_good)
			{
				strText.Format(IDS_CHATSERVER_INVALID_PACKET,
					(BYTE)CommandFileDeny,
					(BYTE)CHAT_END_OF_COMMAND);

				std::string data = static_cast<LPCTSTR>( strText );
				pChat->SendData( data );
			}
			else
			{
				// Make sure the file does not already exist.
				std::string downloadPath;
				ErrorHandlingUtils::TESTHR(GetDownloadPath(downloadPath));

				CString strFileName;
				strFileName = downloadPath.c_str();
				strFileName += "\\";
				strFileName += fileName.c_str();
				CFileException fe;
				CFileStatus fs;
				if (CFile::GetStatus(strFileName,fs))
				{
					strText.Format(IDS_CHATSERVER_FILE_ALREADY_EXISTS,
						(BYTE)CommandFileDeny,
						(BYTE)CHAT_END_OF_COMMAND);

					std::string data = static_cast<LPCTSTR>( strText );
					pChat->SendData( data );

					strText = "";
					strText.Format(IDS_CHATSERVER_FILE_BEING_SENT_ALREADY_EXISTS, 
						(LPCSTR)strFileName, 
						pChat->GetName());
					ChatPrint(strText);
				}
				else if (!pChat->GetFile().Open(
					strFileName,
					CFile::modeCreate|CFile::modeWrite|CFile::typeBinary,
					&fe))
				{
					strText.Format(IDS_CHATSERVER_UNABLE_TO_CREATE_FILE,
						(BYTE)CommandFileDeny,
						(BYTE)CHAT_END_OF_COMMAND);

					std::string data = static_cast<LPCTSTR>( strText );
					pChat->SendData( data );

					CString errorMessage;
					TCHAR error_buffer[1024+1] = {0};
					if(!fe.GetErrorMessage(error_buffer, 1024))
					{
						errorMessage = _T("<UNKNOWN ERROR>");
					}
					else
					{
						errorMessage = error_buffer;
					}

					strText = "";
					strText.Format(
						IDS_CHATSERVER_FAILED_TO_CREATE_FILE,
						strFileName,
						errorMessage);
					ChatPrint(strText);
				}
				else
				{
					strText.Format(IDS_CHATSERVER_RECEIVING_FILE_FROM,
						(LPCSTR)pChat->GetName(),
						(LPCSTR)strFileName,
						fileSize);
					ChatPrint(strText);

					pChat->SetTransferType(CChat::TransferType::Receive);
					pChat->SetFileLength(fileSize);
					pChat->SetBytesWritten(0);
					pChat->SetTransferStartTime(time(NULL));

					// Request the first block.
					strText.Format("%c%c",
						(BYTE)CommandFileBlockRequest,
						(BYTE)CHAT_END_OF_COMMAND);

					std::string data = static_cast<LPCTSTR>( strText );
					pChat->SendData( data ); 
				}
			}
		}
	}

	void CChatServer::ChatServerImpl::IncomingFileDeny(CChatPtr pChat, PCTSTR pszBuf)
	{
		if (pChat->GetTransferType() != CChat::TransferType::Send)
			return;

		CString strText;
		strText.Format(IDS_CHATSERVER_FILE_START_DENIED,
			(LPCSTR)pChat->GetName(),
			pszBuf);
		ChatPrint(strText);

		pChat->SetTransferType(CChat::TransferType::None);;
		pChat->GetFile().Close();
	}

	void CChatServer::ChatServerImpl::IncomingFileBlockRequest(CChatPtr pChat)
	{
		if (!pChat->IsSending())
			return;

		_sendBuffer[0] = (BYTE)CommandFileBlock;
		int nBytes = pChat->GetFile().Read(&_sendBuffer[1],CHAT_BLOCKSIZE);
		_sendBuffer[CHAT_BLOCKSIZE+1] = (BYTE)CHAT_END_OF_COMMAND;
		pChat->SendData(&_sendBuffer[0], CHAT_BLOCKSIZE + 2);

		// Check to see if we are at the end of the file.
		if (nBytes < CHAT_BLOCKSIZE)
		{
			CString strText;
			strText.Format("%c%c",
				(BYTE)CommandFileEnd,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			pChat->SendData( data );

			time_t tNow = time(NULL);
			time_t tDif = tNow - pChat->GetTransferStartTime();
			if(tDif == 0)
				tDif = 1;
			strText.Format(IDS_CHATSERVER_FILE_TRANSFER_COMPLETE,
				(LPCSTR)pChat->GetFile().GetFileName(),
				pChat->GetFileLength(),
				tDif / 60,
				tDif % 60,
				pChat->GetFileLength() / tDif);

			ChatPrint(strText);
			pChat->SetTransferType(CChat::TransferType::None);
			pChat->GetFile().Close();
		}
	}

	void CChatServer::ChatServerImpl::IncomingFileBlock(CChatPtr pChat, PCTSTR pszBuf)
	{
		if (!pChat->IsReceiving())
			return;

		// Figure out how many bytes the block should have in it.
		int nBytes = pChat->GetFileLength() - pChat->GetBytesWritten();
		if (nBytes > CHAT_BLOCKSIZE)
			nBytes = CHAT_BLOCKSIZE;

		// Write it out and keep track of bytes written.
		if (nBytes)
		{
			pChat->GetFile().Write(pszBuf,nBytes);
			pChat->SetBytesWritten(pChat->GetBytesWritten() + nBytes);
		}

		// Request another block.  Need to keep requesting blocks even
		// if this side thinks it is done with the file.  This will
		// cause the sender to eventually give us the end of file
		// command.
		char pBuf[2];
		pBuf[0] = (BYTE)CommandFileBlockRequest;
		pBuf[1] = (BYTE)CHAT_END_OF_COMMAND;
		pChat->SendData(pBuf, 2);
	}

	void CChatServer::ChatServerImpl::IncomingFileEnd(CChatPtr pChat)
	{
		if (!pChat->IsReceiving())
			return;

		CString strText;
		time_t tNow = time(NULL);
		time_t tDif = tNow - pChat->GetTransferStartTime();
		if(tDif == 0)
			tDif=1;

		strText.Format(IDS_CHATSERVER_FILE_TRANSFER_COMPLETE,
			(LPCSTR)pChat->GetFile().GetFileName(),
			pChat->GetFileLength(),
			tDif / 60,
			tDif % 60,
			pChat->GetFileLength()/tDif);
		ChatPrint(strText);

		pChat->SetTransferType(CChat::TransferType::None);
		pChat->GetFile().Close();
	}

	void CChatServer::ChatServerImpl::IncomingFileCancel(CChatPtr pChat)
	{
		if (!pChat->IsSending() && !pChat->IsReceiving())
			return;

		pChat->SetTransferType(CChat::TransferType::None);
		pChat->GetFile().Close();

		ChatPrint(IDS_CHATSERVER_FILE_TRANSFER_CANCELED);
	}

	HRESULT CChatServer::Ping(PCTSTR pszName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			int nClock = clock();

			CString strText;
			strText.Format("%c%d%c",
				(BYTE)CommandPingRequest,
				nClock,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			ErrorHandlingUtils::TESTHR(pChat->SendData( data ));

			_pImpl->ChatPrint(IDS_CHATSERVER_PING_REQUEST_SENT);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Ping\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::IncomingPingRequest(CChatPtr pChat, PCTSTR pszBuf)
	{
		// pszBuf will have the clock ticks elapsed sent from the person
		// requesting the ping.  Send it back to them and I don't have 
		// to keep track of the ping requests on the sending machine.
		CString strText;
		strText.Format("%c%s%c",
			(BYTE)CommandPingResponse,
			pszBuf,
			(BYTE)CHAT_END_OF_COMMAND);

		std::string data = static_cast<LPCTSTR>( strText );
		pChat->SendData( data );
	}

	void CChatServer::ChatServerImpl::IncomingPingResponse(const CChatPtr pChat, PCTSTR pszBuf)
	{
		try
		{
			
			clock_t nClockNow = clock();
			long nStart = atoi(pszBuf);
			long nDif = nClockNow - nStart;
			double dElapsed = nDif / (double)CLOCKS_PER_SEC;

			CString strText;
			strText.Format(IDS_CHATSERVER_PING_RETURNED,
				(LPCSTR)pChat->GetName(),
				dElapsed);
			ChatPrint(strText);
		}
		catch(...)
		{
			ChatPrint(_T("Ping response error"));
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::IncomingPingResponse\n"));
		}

	}

	HRESULT CChatServer::PeekConnections(PCTSTR pszName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			strText.Format("%c%c",
				(BYTE)CommandPeekConnections,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			ErrorHandlingUtils::TESTHR(pChat->SendData( data ));

			hr = S_OK;
		}
		catch (_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::PeekConnections\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::SendPublicPeek(CChatPtr pChat)
	{
		CString strConnects;
		CString strTemp;
		CString strAddress;

		if(!pChat->GetIgnore())
		{
			CString strMessage;
			strMessage.Format(IDS_CHATSERVER_IS_PEEKING_AT_YOUR_CONNECTIONS, pChat->GetName());
			ChatPrint(strMessage);
		}

		ChatConnectionListIterator it = _ptrList.begin();
		for(; it != _ptrList.end(); ++it)
		{
			CChatPtr pTemp = *it;
			if (!pChat->MatchesSearchName(pTemp->GetSearchName()) && !pTemp->GetPrivate())
			{
				if (pTemp->GetAddress().GetLength() < 8)
					strAddress = "<Unknown>";
				else
					strAddress = pTemp->GetAddress();

				strTemp.Format("%s~%u~%s~",
					(LPCSTR)strAddress,
					pTemp->GetPort(),
					(LPCSTR)pTemp->GetName());
				strConnects += strTemp;
			}
		}

		if (!strConnects.IsEmpty())
		{
			CString strText;
			strText.Format(
				_T("%c%s%c"),
				(BYTE)CommandPeekList,
				strConnects,
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			pChat->SendData( data );
		}
		else
		{
			CString strText;
			strText.Format(IDS_CHATSERVER_DOESNT_HAVE_OTHER_CONNECTIONS,
				(PCTSTR)GetChatName());
			pChat->SendMessage(strText);
		}
	}

	void CChatServer::ChatServerImpl::GetPeekParam(CString &strText, CString &strParam)
	{
		int nIndex = strText.Find('~');
		if (nIndex == -1)
		{
			strText.Empty();
			strParam.Empty();
			return;
		}

		strParam = strText.Left(nIndex);
		strText = strText.Right(strText.GetLength()-nIndex-1);
	}

	void CChatServer::ChatServerImpl::IncomingPeekList(PCTSTR pszBuf)
	{
		int nCount = 0;
		int nIndex = 0;
		while(*(pszBuf+nIndex))
		{
			if (*(pszBuf+nIndex) == '~')
				nCount++;
			nIndex++;
		}

		nCount /= 3;

		CString strBuf(pszBuf);

		CString strText;
		strText.Format(IDS_CHATSERVER_CHAT_PEEK_FOUND_CONNECTIONS, nCount);
		ChatPrint(strText);

		CString strMessage;
		strMessage =  _T("     Name                 Address         Port\n");
		strMessage += _T("     ==================== =============== =====\n");

		CString strIP, strPort, strName;
		nCount = 1;
		GetPeekParam(strBuf,strIP);
		GetPeekParam(strBuf,strPort);
		GetPeekParam(strBuf,strName);
		while(!strIP.IsEmpty() && !strPort.IsEmpty() && !strName.IsEmpty())
		{

			if (strName.GetLength() > 20)
				strName = strName.Left(20);
			strText.Format(
				_T("%s%03d:%s %-20s %-15s %s\n"),
				ANSI_F_BOLDWHITE,
				nCount,
				ANSI_RESET,
				strName,
				strIP,
				strPort);
			strMessage+= strText;

			GetPeekParam(strBuf,strIP);
			GetPeekParam(strBuf,strPort);
			GetPeekParam(strBuf,strName);
			nCount++;
		}

		ChatPrint(strMessage);
	}

	void CChatServer::SendSnoop( const std::string &line, byte fore, byte back )
	{
		CString strText;
		CString strColor;

		std::stringstream buffer;
		buffer 
			<< (BYTE)CommandSnoopData
			<< std::setw(2) << std::setfill('0') << (int)fore
			<< std::setw(2) << std::setfill('0') << (int)back
			<< '\n'
			<< line
			<< (BYTE)CHAT_END_OF_COMMAND;

		ChatConnectionListIterator it = _pImpl->_ptrList.begin();
		for(; it != _pImpl->_ptrList.end(); ++it)
		{
			CChatPtr pTemp = *it;
			if (pTemp->GetSnooped())
				pTemp->SendData(buffer.str().c_str());
		}
	}

	void CChatServer::ChatServerImpl::IncomingSnoopData(PCTSTR pszBuf)
	{
		static const int MAX_SNOOP_LEN = 200;

		TRACE(_T("%s"), pszBuf);
		CString strText;
		char szLine[MAX_SNOOP_LEN+1];
		char szColor[3];
		int  nIndex;
		int  nFore, nBack;

		szColor[2] = '\x0';

		// Foreground color.
		szColor[0] = *pszBuf++;
		szColor[1] = *pszBuf++;
		nFore = atoi(szColor);

		// Background color.
		szColor[0] = *pszBuf++;
		szColor[1] = *pszBuf++;
		nBack = atoi(szColor);

		if (*pszBuf == '\n')
		{
			pszBuf++;
		}

		nIndex = 0;
		CString strMessage;
		while(*pszBuf)
		{
			if (*pszBuf == '\n' || nIndex == MAX_SNOOP_LEN)
			{
				szLine[nIndex] = '\x0';
				PrintSnoop(szLine);
				nIndex = 0;
				pszBuf++;
				continue;
			}


			if (*pszBuf == '\r')
			{
				pszBuf++;
				continue;
			}

			szLine[nIndex] = *pszBuf;
			nIndex++;
			pszBuf++;
		}

		// Print the left overs.
		if (nIndex)
		{
			szLine[nIndex] = '\x0';
			PrintSnoop(szLine);
		}

	}

	void CChatServer::ChatServerImpl::IncomingSnoop(CChatPtr pChat)
	{
		CString strText;

		if (!pChat->GetAllowSnoop())
		{
			strText.Format(IDS_CHATSERVER_HAS_NOT_GIVEN_PERM_TO_SNOOP,
				(LPCSTR)GetChatName());
			pChat->SendMessage(strText);
			return;
		}

		if (pChat->GetSnooped())
		{
			strText.Format(IDS_CHATSERVER_HAS_STOPPED_SNOOPING_YOU,
				(LPCSTR)pChat->GetName());
			pChat->SetSnooped(false);
			DecrementSnoopCount();

			CString strMessage;
			strMessage.Format(IDS_CHATSERVER_YOU_STOPPED_SNOOPING, (LPCSTR)GetChatName());
			pChat->SendMessage(strMessage);
		}
		else
		{
			strText.Format(IDS_CHATSERVER_YOU_ARE_BEING_SNOOPED_BY,
				(LPCSTR)pChat->GetName());

			pChat->SetSnooped(true);
			IncrementSnoopCount();

			CString strMessage;
			strMessage.Format(IDS_CHATSERVER_YOU_HAVE_BEGUN_SNOOPING, 
				(LPCSTR)GetChatName());
			pChat->SendMessage(strMessage);
		}

		ChatPrint(strText);
	}

	void CChatServer::ChatServerImpl::IncomingCommand(CChatPtr pChat, BYTE chCommand, PCTSTR pszBuf)
	{
		// Make sure we are allowing commands form this user.
		if (!pChat->GetCommands())
		{
			// Send a message back letting the person know we are not accepting.
			CString strText;
			strText.Format(IDS_CHATSERVER_IS_NOT_ALLOWING_COMMANDS,
				(BYTE)CommandMessage,
				(LPCSTR)GetChatName(),
				(BYTE)CHAT_END_OF_COMMAND);

			std::string data = static_cast<LPCTSTR>( strText );
			pChat->SendData( data );
			return;
		}

		CString strType;
		switch(chCommand)
		{
		case CommandTypeAction:
			strType = "Action";
			break;

		case CommandTypeAlias:
			strType = "Alias";
			break;

		case CommandTypeMacro: 
			strType = "Macro";
			break;

		case CommandTypeVariable:
			strType = "Variable";
			break;

		case CommandTypeEvent:
			strType = "Event";
			break;

		case CommandTypeGag:
			strType = "Gag";
			break;

		case CommandTypeHighlight:
			strType = "Highlight";
			break;

		case CommandTypeList:
			strType = "List";
			break;

		case CommandTypeArray:
			strType = "Array";
			break;

		case CommandTypeBarItem:
			strType = "Bar Item";
			break;

		case CommandTypeSubstitute:
			strType = "Substitute";
			break;

		default :
			return;
		}

		CString strText;
		strText.Format(IDS_CHATSERVER_RECEIVED,(LPCSTR)strType);
		ChatPrint(strText);

		strText = pszBuf;
		ExecuteCommand(strText);
	}

	HRESULT CChatServer::SendCommand( byte chCommand, PCTSTR pszName, PCTSTR pszCommand )
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			ErrorHandlingUtils::TESTHR(pChat->SendCommand(chCommand, pszCommand));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::SendCommand\n"));
		}

		return hr;
	}

	void CChatServer::SetClient(IChatServerClient *pClient)
	{
		_pImpl->_pClient = pClient;
	}

	void CChatServer::SetChatName(PCTSTR szChatName)
	{
		if(_pImpl->_chatName.Compare(szChatName) != 0)
		{
			_pImpl->_chatName = szChatName;
			_pImpl->BroadcastNameChange();
		}
	}

	HRESULT CChatServer::CloseChatSocket(PCTSTR pszIndex)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(pszIndex, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s(pszIndex);
				if(s.MakeLower() = "all")
				{
					ErrorHandlingUtils::TESTHR(_pImpl->RemoveAll());
					hr = S_OK;
					return hr;
				}
				else
				{
					s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, pszIndex);
					_pImpl->PrintChatMessage(s);
					ErrorHandlingUtils::TESTHR(E_INVALIDARG);
				}
			}

			ErrorHandlingUtils::TESTHR(_pImpl->CloseChatConnection(pChat, NULL));

			CString strMessage;
			strMessage.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_DISCONNECTED, pszIndex);
			_pImpl->PrintChatMessage(strMessage);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CliseChatSocket\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::CloseChatConnection(CChatPtr pChat, PCTSTR szMessage)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{

			ChatConnectionListIterator it = std::find(_ptrList.begin(), _ptrList.end(), pChat);
			if(pChat->GetSnooped())
				DecrementSnoopCount();

			unhookEvent(*it);
			if(NULL != szMessage)
			{
				::OutputDebugString(_T("CChatServer::ChatServerImpl::CloseChatConnection calling close with message\n"));
				ErrorHandlingUtils::TESTHR(pChat->Close(szMessage));
			}
			else
			{
			::OutputDebugString(_T("CChatServer::ChatServerImpl::CloseChatConnection no message closing socket directly\n"));
			pChat->GetSocket().Close();
			}
			// don't detach if closed pChat->GetSocket().Detach();
			pChat.reset();
			_ptrList.erase(it);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CloseChatConnection\n"));
		}

		return hr;
	}

	bool CChatServer::ChatServerImpl::isValidIndex(int index) const
	{
		if(index > 0 && index <= GetCount())
			return true;
		return false;
	}

	HRESULT CChatServer::ChatServerImpl::ChatPrint(UINT nStringId)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			string str;
			ErrorHandlingUtils::TESTHR(StringUtils::LoadString(nStringId, str));
			ErrorHandlingUtils::TESTHR(ChatPrint(str.c_str()));
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			ErrorHandlingUtils::ReportException(e);
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatPrint\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::AddChatConnection(CChatPtr pChat)
	{
		_ptrList.push_back(pChat);
	}

	HRESULT CChatServer::ChatServerImpl::GetCloseMessage(string &out)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			string closeString;
			ErrorHandlingUtils::TESTHR(StringUtils::LoadString(IDS_CHATSERVER_REMOTE_CLOSED_CONNECTION, closeString));

			CString strMessage;
			strMessage.Format(closeString.c_str(), static_cast<LPCTSTR>(GetChatName()));

			out = static_cast<LPCTSTR>(strMessage);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetCloseMessage\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetCloseMessage\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::AcceptChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode)
	{
		// Incoming chat connection request
		if(ERROR_SUCCESS == nErrorCode)
			Accept(pSocket);
		else
		{
			::OutputDebugString(_T("Error code not success in CChatServer::ChatServerImpl::AcceptChatSocketEventHandler\n"));
			ATLASSERT(FALSE);
		}

	}

	void CChatServer::ChatServerImpl::CloseChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode)
	{
		try
		{
		if(ERROR_SUCCESS == nErrorCode)
			nErrorCode = 0 ;
		CChatServer::ChatServerImpl::Close(pSocket->GetID());
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::CloseChatSocketEventHandler\n"));
		}

	}

	void CChatServer::ChatServerImpl::ConnectChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode)
	{
		Connection(nErrorCode, pSocket);
	}

	//TODO: KW typo ReceiveChatSocketEventHandler(
	void CChatServer::ChatServerImpl::ReceiveChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode)
	{
		if(ERROR_SUCCESS == nErrorCode)
			Receive(pSocket->GetID());
		else
			::OutputDebugString(_T("ERROR_SUCCESS != nErrorCode in ReceiveChatSocketEventHandler\n"));

	}


	void CChatServer::ChatServerImpl::SendChatSocketEventHandler(CChatSocket *pSocket, int nErrorCode)
	{
		pSocket;nErrorCode;
	}

	HRESULT CChatServer::GetChatName(int index, CString &name)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if(!_pImpl->isValidIndex(index))
			{
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			name = _pImpl->_ptrList[index]->GetName();

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetChatName\n"));
		}

		return hr;
	}

	HRESULT CChatServer::GetConnectName(const CString &index, CString &name)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(index, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString strMessage;
				strMessage.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, index);
				_pImpl->SetError(strMessage);
				name == "-1";
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			name = pChat->GetName();

			hr = S_OK;
		}
		catch (_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetConnectName\n"));
		}

		return hr;
	}

	HRESULT CChatServer::GetConnectIP(const CString &name, CString &ip)
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(name, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString temp;
				temp.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, name);
				_pImpl->SetError(temp);
				ip == "-1";
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			ip.Format("%s:%d", pChat->GetReportedAddress(), pChat->GetPort());

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetConnectIP\n"));
		}

		return hr;
	}

	HRESULT CChatServer::GetChatVersion(const CString &index, CString &version)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(index, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString temp;
				temp.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, index);
				_pImpl->SetError(temp);
				version == "-1";
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			version = pChat->GetVersion();

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetChatVersion\n"));
		}

		return hr;
	}

	HRESULT CChatServer::GetChatFlags(const CString &index, CString &flags)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(index, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString temp;
				temp.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, index);
				_pImpl->SetError(temp);
				flags == "-1";
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			flags = ":";

			if(pChat->GetCommands())
				flags += "A";

			if(pChat->GetTransfers())
				flags += "T";

			if(pChat->GetPrivate())
				flags += "P";

			if(pChat->GetIgnore())
				flags += "I";

			if(pChat->GetServe())
				flags += "S";

			if(pChat->GetAddress() != pChat->GetReportedAddress())
				flags += "F";

			if(pChat->GetSnooped())
				flags += "N";

			if(pChat->GetAllowSnoop())
				flags += "n";

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetChatFlags\n"));
		}

		return hr;
	}

	HRESULT CChatServer::Snoop(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			pChat->Snoop();

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Snoop\n"));
		}

		return hr;
	}

	HRESULT CChatServer::FileStatus(PCTSTR szName, CString &strStatus)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			if (pChat->GetTransferType() == CChat::TransferType::None)
			{
				CString s;
				s.Format(IDS_CHATSERVER_NO_FILE_TRANSFER_IN_PROGRESS);
				_pImpl->PrintChatMessage(s);
			}
			else
			{
				CString strText;
				if (pChat->GetTransferType() == CChat::TransferType::Receive)
				{
					strText.Format(IDS_CHATSERVER_RECEIVING_S_FROM_S,
					(LPCSTR)pChat->GetFile().GetFileName(),
					(LPCSTR)pChat->GetName());
				}
				else
				{
					strText.Format(IDS_CHATSERVER_SENDING_S_TO_S,
					(LPCSTR)pChat->GetFile().GetFileName(),
					(LPCSTR)pChat->GetName());
				}

				strStatus = strText;

				strText.Format(IDS_CHATSERVER_FILE_LENGTH_U,pChat->GetFileLength());

				strStatus += strText;
				if (pChat->GetTransferType() == CChat::TransferType::Receive)
				{
					int nBytesWritten = pChat->GetBytesWritten();

					strText.Format(IDS_CHATSERVER_BYTES_RECEIVED_U,nBytesWritten);
					strStatus += strText;

					strText.Format(IDS_CHATSERVER_BYTES_REMAINING,pChat->GetFileLength() - nBytesWritten);
					strStatus += strText;
					time_t tNow = time(NULL);
					time_t tStart = pChat->GetTransferStartTime();
					time_t tDif = tNow - tStart;

					strText.Format(IDS_CHATSERVER_BYTES_PER_SECOND_U,nBytesWritten/tDif);
					strStatus += strText;
				}
				else
				{
					strText.Format(IDS_CHATSERVER_BYTES_SENT, pChat->GetFile().GetPosition());

					strStatus += strText;

					strText.Format(IDS_CHATSERVER_BYTES_REMAINING,
						pChat->GetFileLength() - pChat->GetFile().GetPosition());

					strStatus += strText;
					time_t tNow = time(NULL);
					time_t tStart = pChat->GetTransferStartTime();
					time_t tDif = tNow - tStart;
					strText.Format(IDS_CHATSERVER_BYTES_PER_SECOND_U,pChat->GetFile().GetPosition()/tDif);
					strStatus += strText;
				}
			}
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::FileStatus\n"));
		}

		return hr;
	}

	const CString &CChatServer::GetError() const
	{
		return _pImpl->_errorString;
	}

	void CChatServer::ChatServerImpl::SetError(const CString &errorString)
	{
		_errorString = errorString;
	}

	HRESULT CChatServer::Call(const CString &address, const CString &port)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			int nPort = atoi(port);
			if (!nPort)
				nPort = 4050;

			CChatPtr pChat;
			hr = _pImpl->FindAddress(address, nPort, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE != hr)
			{
				CString strText;
				strText.Format(IDS_CHATSERVER_ALREADY_CONNECTED_WITH, address, port);

				_pImpl->PrintChatMessage(strText);

				::OutputDebugString(_T("already connected CChatServer::ChatServerImpl::Call\n"));
			}
			else
				hr = _pImpl->Connect(address, nPort);
		}
		catch(_com_error &e)
		{
			::OutputDebugString("com_Error in CChatServer::ChatServerImpl::Call\n");
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Call\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatCommands(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;
			bool commands = false;

			if (pChat->GetCommands())
			{
				strText.Format(IDS_CHATSERVER_NO_LONGER_ACCEPTING_COMMANDS_FROM_S,pChat->GetName());

				strMessage.Format(
					IDS_CHATSERVER_S_IS_NO_LONGER_ACCEPTING_COMMANDS_FROM_YOU,
					_pImpl->GetChatName());
			}
			else
			{
				commands = true;
				strText.Format(IDS_CHATSERVER_NOW_ACCEPTING_COMMANDS_FROM_S, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_S_IS_NOW_ACCEPTING_COMMANDS_FROM_YOU,
					_pImpl->GetChatName());
			}

			_pImpl->PrintChatMessage(strText);
			pChat->SetCommands(commands);
			ErrorHandlingUtils::TESTHR(pChat->SendMessage(strMessage));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatCommands\n"));
		}

		return hr;
	}

	HRESULT CChatServer::Ignore(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			bool ignore = false;
			if (pChat->GetIgnore())
			{
				strText.Format(IDS_CHATSERVER_NO_LONGER_IGNORING_S,(LPCSTR)pChat->GetName());
			}
			else
			{
				strText.Format(IDS_CHATSERVER_NOW_IGNORING_S,(LPCSTR)pChat->GetName());
				ignore = true;
			}

			_pImpl->PrintChatMessage(strText);
			pChat->SetIgnore(ignore);

			hr = S_OK;

		}
		catch (_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::Ignore\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatPrivate(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;
			if (pChat->GetPrivate())
			{
				pChat->SetPrivate(false);

				strText.Format(IDS_CHATSERVER_S_MARKED_AS_PUBLIC, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_S_HAS_MARKED_YOUR_CONNECTION_PUBLIC, 
					_pImpl->GetChatName());
			}
			else
			{
				pChat->SetPrivate(true);

				strText.Format(IDS_CHATSERVER_S_MARKED_AS_PRIVATE, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_S_HAS_MARKED_YOUR_CONNECTION_PRIVATE, 
					_pImpl->GetChatName());
			}

			_pImpl->PrintChatMessage(strText);
			ErrorHandlingUtils::TESTHR(pChat->SendMessage(strText));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::ChatPrivate\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatPrivate\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServe(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;

			if (pChat->GetServe())
			{
				pChat->SetServe(false);

				strText.Format(IDS_CHATSERVER_S_IS_NO_LONGER_BEING_SERVED, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_YOU_ARE_NO_LONGER_SERVED_BY_S, 
					_pImpl->GetChatName());
			}
			else
			{
				pChat->SetServe(true);

				strText.Format(IDS_CHATSERVER_S_IS_BEING_SERVED, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_YOU_ARE_BEING_SERVED_BY_S, 
					_pImpl->GetChatName());
			}

			_pImpl->PrintChatMessage(strText);
			ErrorHandlingUtils::TESTHR(pChat->SendMessage(strText));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			::OutputDebugString(_T("_com_Error in CChatServer::ChatServerImpl::ChatServe\n"));
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatServe\n"));
		}

		return hr;
	}
	HRESULT CChatServer::ChatExcludeServe(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;

			if (pChat->GetExcludeServe())
			{
				pChat->SetExcludeServe(false);

				strText.Format("%s is not excluded from getting served chat", pChat->GetName());
			}
			else
			{
				pChat->SetExcludeServe(true);

				strText.Format(IDS_CHATSERVER_S_IS_NO_LONGER_BEING_SERVE_EXCLUDED, pChat->GetName());
			}

			_pImpl->PrintChatMessage(strText);

			hr = S_OK;
		}

		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatExcludeServe\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatSnoop(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;

			if (pChat->GetAllowSnoop())
			{
				pChat->SetAllowSnoop(false);
				pChat->SetSnooped(false);

				strText.Format(IDS_CHATSERVER_S_IS_NO_LONGER_ALLOWED_TO_SNOOP_YOU, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_YOU_ARE_NO_LONGER_ALLOWED_TO_SNOOP_S, 
					_pImpl->GetChatName());
			}
			else
			{
				pChat->SetAllowSnoop(true);

				strText.Format(IDS_CHATSERVER_S_IS_NOW_ALLOWED_TO_SNOOP_YOU, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_YOU_ARE_NOW_ALLOWED_TO_SNOOP_S, 
					_pImpl->GetChatName());
			}

			_pImpl->PrintChatMessage(strText);
			ErrorHandlingUtils::TESTHR(pChat->SendMessage(strMessage));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatSnoop\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatTransfers(PCTSTR szName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CChatPtr pChat;
			hr = _pImpl->Find(szName, pChat);
			ErrorHandlingUtils::TESTHR(hr);
			if(S_FALSE == hr)
			{
				CString s;
				s.Format(IDS_CHATSERVER_CHAT_CONNECTION_S_NOT_FOUND, szName);
				_pImpl->PrintChatMessage(s);
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			CString strText;
			CString strMessage;

			if (pChat->GetTransfers())
			{
				pChat->SetTransfers(false);

				strText.Format(IDS_CHATSERVER_S_IS_NO_LONGER_ALLOWED_TO_SEND_YOU_FILES, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_S_IS_NO_LONGER_ACCEPTING_FILE_TRANSFERS_FROM_YOU, 
					_pImpl->GetChatName());
			}
			else
			{
				pChat->SetTransfers(true);

				strText.Format(IDS_CHATSERVER_S_IS_ALLOWED_TO_SEND_YOU_FILES, pChat->GetName());
				strMessage.Format(
					IDS_CHATSERVER_S_IS_ACCEPTING_FILE_TRANSFERS_FROM_YOU, 
					_pImpl->GetChatName());
			}

			_pImpl->PrintChatMessage(strText);
			ErrorHandlingUtils::TESTHR(pChat->SendMessage(strMessage));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::ChatTransfers\n"));
		}

		return hr;
	}

	HRESULT CChatServer::GetVersionInfo(CString &strData)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CString strText;

			for(ChatConnectionList::size_type i = 0; i < _pImpl->_ptrList.size(); ++i)
			{
				strText.Format("%s%03d:%s %-30s %s\n",
					ANSI_F_BOLDWHITE,
					i+1,
					ANSI_RESET,
					_pImpl->_ptrList[i]->GetName(),
					_pImpl->_ptrList[i]->GetVersion());

				strData += strText;
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return hr;
	}

	HRESULT CChatServer::GetChatListInfo(CString &strData)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			for(ChatConnectionList::size_type i = 0; i < _pImpl->_ptrList.size(); ++i)
				strData += _pImpl->_ptrList[i]->GetInfoString(i);
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::GetChatListInfo\n"));
		}

		return hr;
	}

	void CChatServer::ChatServerImpl::serverHookEvent(ChatServerSocket pServerSocket)
	{
		__hook(
			&CChatSocket::AcceptEvent, 
			pServerSocket.get(), 
			&CChatServer::ChatServerImpl::AcceptChatSocketEventHandler);
	}

	void CChatServer::ChatServerImpl::serverUnhookEvents(ChatServerSocket pServerSocket)
	{
		__unhook(
			&CChatSocket::AcceptEvent, 
			pServerSocket.get(), 
			&CChatServer::ChatServerImpl::AcceptChatSocketEventHandler);
	}

	void CChatServer::ChatServerImpl::hookEvent(CChatPtr pChat)
	{

		__hook(&CChatSocket::CloseEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::CloseChatSocketEventHandler);

		__hook(&CChatSocket::ConnectEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::ConnectChatSocketEventHandler);
//TODO: KW fix receive typo
		__hook(&CChatSocket::ReceiveEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::ReceiveChatSocketEventHandler);

		__hook(&CChatSocket::SendEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::SendChatSocketEventHandler);
	}

	void CChatServer::ChatServerImpl::unhookEvent(CChatPtr pChat)
	{

		__unhook(&CChatSocket::CloseEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::CloseChatSocketEventHandler);

		__unhook(&CChatSocket::ConnectEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::ConnectChatSocketEventHandler);
//TODO: KW fix receive typo
		__unhook(&CChatSocket::ReceiveEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::ReceiveChatSocketEventHandler);

		__unhook(&CChatSocket::SendEvent, 
			&(pChat->GetSocket()), 
			&CChatServer::ChatServerImpl::SendChatSocketEventHandler);
	}

	template<class T>
		void CChatServer::ChatServerImpl::MultiSendData(const string &data, const T &pfn)
	{
		try
		{
			ChatConnectionListIterator it = _ptrList.begin();
			for(ChatConnectionListIterator i = _ptrList.begin();i != _ptrList.end(); ++i)
			{
				if(pfn(*i))
				{
					CChatPtr pChat = *i;
					pChat->SendData(data.c_str());
				}
			}
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::MultiSendData\n"));
		}

	}

	HRESULT CChatServer::InitializeChatServer(int port)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if(_pImpl->_pSockListen.get() != NULL)
			{
				CString temp;
				temp.LoadString(IDS_CHATSERVER_ALREADY_STARTED);
				_pImpl->PrintChatMessage(temp);
			}
			else
			{
				ChatServerSocket pSockListen = ChatServerSocket(new CChatSocket);
				if(pSockListen.get() == NULL)
					ErrorHandlingUtils::TESTHR(E_OUTOFMEMORY);

				CString strMessage;
				strMessage.Format(IDS_CHATSERVER_STARTING_CHAT_SERVER_ON_PORT_D, port);
				_pImpl->PrintChatMessage(strMessage);

				if (!pSockListen->Create(port))
				{
					DWORD dwError = pSockListen->GetLastError();
					std::string errorString;
					ErrorHandlingUtils::GetErrorString(dwError, errorString);

					CString s;
					s.Format(IDS_CHATSERVER_FAILED_TO_CREATE_SOCKET, errorString.c_str());
					_pImpl->PrintChatMessage(s);

					ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
				}

				if (!pSockListen->Listen())
				{
					DWORD dwError = pSockListen->GetLastError();
					std::string errorString;
					ErrorHandlingUtils::GetErrorString(dwError, errorString);
					_pImpl->PrintChatMessage(errorString.c_str());
					ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
				}

				_pImpl->serverHookEvent(pSockListen);

				_pImpl->_pSockListen = pSockListen;
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::InitializeChatServer\n"));
		}

		return hr;
	}

	HRESULT CChatServer::StopChatServer()
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if(_pImpl->_pSockListen.get() != NULL)
			{
			_pImpl->serverUnhookEvents(_pImpl->_pSockListen);

			_pImpl->_pSockListen->Close();
			_pImpl->_pSockListen.reset();
			}

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString(_T("com_error in CChatServer::ChatServerImpl::StopChatServer\n"));
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::StopChatServer\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerIsStarted(bool &started)
	{
		started = _pImpl->_pSockListen.get() != NULL;
		return S_OK;
	}
	HRESULT CChatServer::AreSnoops(bool &snoops)
	{
		snoops = _pImpl->GetSnoopCount() > 0;
		return S_OK;
	}

	HRESULT CChatServer::ChatServerImpl::PrintChatMessage(PCTSTR pszMsg)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			ErrorHandlingUtils::TESTHR(GetClient()->PrintChatMessage(pszMsg));
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::PrintChatMessage(pszMsg)\n"));
		}

		return hr;
	}

	HRESULT CChatServer::ChatServerImpl::PrintChatMessage(UINT nMsgId)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			string msg;
			ErrorHandlingUtils::TESTHR(StringUtils::LoadString(nMsgId, msg));
			ErrorHandlingUtils::TESTHR(PrintChatMessage(msg.c_str()));
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::PrintChatMessage(nMsgID)\n"));
		}

		return hr;
	}

	int CChatServer::ChatServerImpl::GetPort() const
	{
		return GetClient()->GetChatServerPort();
	}

	HRESULT CChatServer::ChatServerImpl::GetSoundPath(std::string &path) const
	{
		return GetClient()->GetSoundPath(path);
	}

	bool CChatServer::ChatServerImpl::GetAutoAccept() const
	{
		return GetClient()->GetAutoAccept();
	}
	bool CChatServer::ChatServerImpl::GetAutoServe() const
	{
		return GetClient()->GetAutoServe();
	}

	bool CChatServer::ChatServerImpl::GetDoNotDisturb() const
	{
		return GetClient()->GetDoNotDisturb();
	}

	BYTE CChatServer::ChatServerImpl::GetChatForeColor() const
	{
		return GetClient()->GetChatForeColor();
	}

	BYTE CChatServer::ChatServerImpl::GetChatBackColor() const
	{
		return GetClient()->GetChatBackColor();
	}

	HRESULT CChatServer::ChatServerImpl::GetUploadPath(std::string &path) const
	{
		return GetClient()->GetUploadPath(path);
	}

	HRESULT CChatServer::ChatServerImpl::GetDownloadPath(std::string &path) const
	{
		return GetClient()->GetDownloadPath(path);
	}

	HRESULT CChatServer::ChatServerImpl::PrintSnoop(PCTSTR szSnoopData) const
	{
		return GetClient()->PrintSnoop(szSnoopData);
	}

	HRESULT CChatServer::ChatServerImpl::ExecuteCommand(PCTSTR szCommand)
	{
		return GetClient()->ExecuteCommand(szCommand);
	}

	HRESULT CChatServer::ChatServerImpl::GetFileVersion(std::string &version)
	{
		return GetClient()->GetFileVersion(version);
	}

	HRESULT CChatServer::ChatServerImpl::GetIpAddress(std::string &address)
	{
		return GetClient()->GetIpAddress(address);
	}

	int CChatServer::ChatServerImpl::GetCount() const
	{
		return _ptrList.size();
	}

	int CChatServer::GetCount() const
	{
		 return _pImpl->GetCount();
	}

	HRESULT CChatServer::ChatServerImpl::AnnounceConnection()
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			std::string ringWav;
			TCHAR pszFilename[MAX_PATH] = {0};
			DWORD dwNumChars = GetModuleFileName(NULL, pszFilename, MAX_PATH);
			if(0 == dwNumChars)
			{
				DWORD dwError = ::GetLastError();
				//MMERR::TRACE(dwError, _T("Failed to get the module filename"));
				ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
			}

			std::string directory = pszFilename;
			string::size_type pos = directory.find_last_of('\\');
			directory = directory.substr(0, pos + 1);

			std::string ringWavPath = directory + "\\ring.wav";

			if(!PlaySound(ringWavPath.c_str(), NULL, SND_ASYNC|SND_FILENAME|SND_NOWAIT))
			{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::AnnounceConnection PlaySound\n"));
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
			::OutputDebugString(_T("com_Error in CChatServer::ChatServerImpl::AnnounceConnection\n"));
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CChatServer::ChatServerImpl::AnnounceConnection\n"));
		}

		return hr;
	}

	HRESULT CChatServer::SendCommand(
		BYTE chCommand, 
		const std::string &name, 
		const std::string &command)
	{
		return SendCommand(chCommand, name.c_str(), command.c_str());
	}
}