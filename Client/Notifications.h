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

// Don't want to have to search the DLL list every time there is a possible
// notification so I'm storing the handle of the DLL in each of the 
// notification lists. 

class CSession;

namespace MMSystem
{
	struct NOTIFICATION
	{
		CString strFunction;			// Name of the function to call.
		HINSTANCE hDll;				// Instance handle.
	};

	typedef VOID (*MYPROC)(LPCSTR pszParams, LPSTR pszResult);
	typedef VOID (*CONNECTPROC)(SOCKET hSockt);
	typedef VOID (*DISCONNECTPROC)(void);
	typedef VOID (*CHATCONNECTPROC)(SOCKET hSocket, LPCSTR pszChatName);
	typedef VOID (*CHATDISCONNECTPROC)(SOCKET hSocket, LPCSTR pszChatName);
	typedef BOOL (*INPUTPROC)(LPCSTR pszInput);
	typedef BOOL (*RAWPROC)(LPSTR pszBuf);

	typedef VOID (*DLLADDRESSFUNC)(LPCSTR);
	typedef VOID (*DLLCALLBACK)(DLLADDRESSFUNC);

	class CNotifications
	{
		static const DWORD DEFAULT_NOT_ARRAY_SIZE = 150;
		static const DWORD DEFAULT_NOT_GROW_SIZE = 20;
	public:
		CNotifications(LPCSTR pszTypeName);
		~CNotifications();

		BOOL Add(HINSTANCE hDll,LPCSTR pszFunction);
		BOOL Remove(HINSTANCE hDll);
		BOOL Remove(int nIndex);
		void RemoveAll();

		NOTIFICATION* GetFirst();
		NOTIFICATION* GetNext();

		NOTIFICATION* FindExact(HINSTANCE hDll);

		// Prints all the items in the list.
		void Print(CSession *pSession);

		int GetCount()			{ return(m_nCount); }
		int GetFindIndex()	{ return(m_nGetIndex); }

	protected:
		CString m_strTypeName;

		CPtrArray m_ptrList;
		int m_nCount;

		int m_nGetIndex;
	};

	class CNotifyConnect : public CNotifications
	{
	public:
		CNotifyConnect();
		void Notify(SOCKET hSocket);
	};

	class CNotifyDisconnect : public CNotifications
	{
	public:
		CNotifyDisconnect();
		void Notify();
	};

	class CNotifyChatConnect : public CNotifications
	{
	public:
		CNotifyChatConnect();
		void Notify(SOCKET hSocket, LPCSTR pszChatName);
	};

	class CNotifyChatDisconnect : public CNotifications
	{
	public:
		CNotifyChatDisconnect();
		void Notify(SOCKET hSocket, LPCSTR pszChatName);
	};

	class CNotifyInput : public CNotifications
	{
	public:
		CNotifyInput();
		BOOL Notify(LPCSTR pszInput);
	};

	class CNotifyRaw : public CNotifications
	{
	public:
		CNotifyRaw();
		BOOL Notify(LPSTR pszBuf);
	};

}