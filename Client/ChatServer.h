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

namespace MMChatServer
{
	static const BYTE CHAT_END_OF_COMMAND = 255 ;

	enum ScriptCommandType
	{
		CommandTypeAction			= 9,
		CommandTypeAlias			= 10,
		CommandTypeMacro			= 11,
		CommandTypeVariable			= 12,
		CommandTypeEvent			= 13,
		CommandTypeGag				= 14,
		CommandTypeHighlight		= 15,
		CommandTypeList				= 16,
		CommandTypeArray			= 17,
		CommandTypeBarItem			= 18,
		CommandTypeSubstitute		= 33,
	};

	enum ChatCommandType
	{
		CommandNameChange			= 1,
		CommandRequestConnections	= 2,
		CommandConnectionList		= 3,
		CommandTextEveryone			= 4,
		CommandTextPersonal			= 5,
		CommandTextGroup			= 6,
		CommandMessage				= 7,
		CommandDoNotDisturb			= 8,
		CommandVersion				= 19,
		CommandFileStart			= 20,
		CommandFileDeny				= 21,
		CommandFileBlockRequest		= 22,
		CommandFileBlock			= 23,
		CommandFileEnd				= 24,
		CommandFileCancel			= 25,
		CommandPingRequest			= 26,
		CommandPingResponse			= 27,
		CommandPeekConnections		= 28,
		CommandPeekList				= 29,
		CommandSnoop				= 30,
		CommandSnoopData			= 31,
		CommandSnoopColor			= 32,
	};

	enum ChatAccept
	{
		AcceptFailed				= 0,
		AcceptSuccess				= 1,
		AcceptDnd					= 2,
	};

	class IChatServerClient
	{
	public:
		virtual int GetChatServerPort() const = 0;
		virtual bool GetAutoAccept() const = 0;
		virtual bool GetAutoServe() const = 0;
		virtual bool GetDoNotDisturb() const = 0;
		virtual BYTE GetChatForeColor() const = 0;
		virtual BYTE GetChatBackColor() const = 0;
		virtual HRESULT GetSoundPath(std::string &path) const = 0;
		virtual HRESULT GetUploadPath(std::string &path) const = 0;
		virtual HRESULT GetDownloadPath(std::string &path) const = 0;
		virtual HRESULT GetFileVersion(std::string &version) const = 0;
		virtual HRESULT GetIpAddress(std::string &ipAddress) const = 0;

		virtual HRESULT PrintChatText(PCTSTR szMsg) = 0;
		virtual HRESULT PrintSnoop(PCTSTR szMsg) = 0;
		virtual HRESULT PrintChatMessage(PCTSTR szMsg) = 0;
		virtual HRESULT ExecuteCommand(const CString &command) = 0;
	};

	class CChatSocket;
	class CChat;

	[event_receiver(native)]
	class CChatServer
	{
		static const int CHAT_BLOCKSIZE = 500;

	public:
		CChatServer();
		~CChatServer();

		void SetChatName( PCTSTR szChatName );
		void SetClient( IChatServerClient *pSession );

		HRESULT GetChatName(int index, CString &name);
		HRESULT GetConnectIP(const CString &name, CString &ip);
		HRESULT GetConnectName(const CString &index, CString &name);
		HRESULT GetChatVersion(const CString &index, CString &version);
		HRESULT GetChatFlags(const CString &index, CString &flags);
		HRESULT PeekConnections(PCTSTR pszName);
		HRESULT Ping(PCTSTR pszName);
		HRESULT RequestConnects(PCTSTR pszName);
		HRESULT SendCommand( byte chCommand, PCTSTR pszName, PCTSTR pszCommand );
		HRESULT SendCommand(BYTE chCommand, const std::string &name, const std::string &command);
		HRESULT SendFile(PCTSTR pszName, PCTSTR pszFilename);
		HRESULT Snoop(PCTSTR szName);

		HRESULT CancelTransfer( PCTSTR szIndex );

		HRESULT FileStatus(PCTSTR szName, CString &strStatus);
		HRESULT ChatCommands(PCTSTR szName);
		HRESULT AddGroup(PCTSTR pszName, PCTSTR pszGroup);	
		HRESULT RemoveGroup(PCTSTR pszName);
		HRESULT Ignore(PCTSTR szName);
		HRESULT ChatPrivate(PCTSTR szName);
		HRESULT ChatServe(PCTSTR szName);
		HRESULT ChatExcludeServe(PCTSTR szName);
		HRESULT ChatSnoop(PCTSTR szName);
		HRESULT ChatTransfers(PCTSTR szName);
		HRESULT CloseChatSocket( PCTSTR pszIndex );

		int GetCount() const;
		const CString &GetError() const;
		HRESULT StopChatServer();
		HRESULT InitializeChatServer(int port);
		HRESULT ChatServerIsStarted(bool &started);
		HRESULT AreSnoops(bool &snoops);
		void SendSnoop( const std::string &line, byte fore, byte back );

		HRESULT SendChatToGroup(PCTSTR pszGroup, PCTSTR pszText);
		HRESULT SendEmoteToGroup(PCTSTR pszGroup, PCTSTR pszText);

		HRESULT SendEmoteToPerson(PCTSTR pszName, PCTSTR pszText);
		HRESULT SendChatToPerson(PCTSTR pszName, PCTSTR pszText);
		
		HRESULT SendEmoteToEverybody(PCTSTR pszText);
		HRESULT SendChatToEverybody(PCTSTR pszText);

		HRESULT Call(const CString &address, const CString &port);
		HRESULT GetVersionInfo(CString &strData);
		HRESULT GetChatListInfo(CString &strData);

	private:
		friend class CloseChat;
		struct ChatServerImpl;
		std::auto_ptr<ChatServerImpl> _pImpl;
	};
}