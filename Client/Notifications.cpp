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
#include "Ifx.h"
#include "Globals.h"
#include "Notifications.h"
#include "DllList.h"
#include "Dll.h"
#include "Sess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMSystem
{
	CNotifications::CNotifications(LPCSTR pszTypeName)
	{
		m_ptrList.SetSize(DEFAULT_NOT_ARRAY_SIZE,DEFAULT_NOT_GROW_SIZE);
		m_nCount = 0;
		m_nGetIndex = 0;

		m_strTypeName = pszTypeName;
	}

	CNotifications::~CNotifications()
	{
		RemoveAll();
	}

	void CNotifications::RemoveAll()
	{
		NOTIFICATION *pNot;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			delete pNot;
			pNot = NULL;
		}
		m_ptrList.RemoveAll();
		m_nCount = 0;
		m_nGetIndex = 0;
	}

	BOOL CNotifications::Add(HINSTANCE hDll,LPCSTR pszFunction)
	{
		NOTIFICATION *pNot;

		// If this dll is already in the list, just change the name of
		// the function to call.
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (pNot->hDll == hDll)
			{
				pNot->strFunction = pszFunction;
				return(TRUE);
			}
		}

		NOTIFICATION *pNew = new NOTIFICATION;
		pNew->strFunction = pszFunction;
		pNew->hDll = hDll;
		m_ptrList.SetAtGrow(m_nCount,pNew);
		m_nCount++;
		return(TRUE);
	}

	BOOL CNotifications::Remove(HINSTANCE hDll)
	{
		NOTIFICATION *pNot;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (pNot->hDll == hDll)
			{
				m_ptrList.RemoveAt(i);
				m_nCount--;
				delete pNot;
				pNot = NULL;
				return(TRUE);
			}
		}
		return(FALSE);
	}

	BOOL CNotifications::Remove(int nIndex)
	{
		if (nIndex < 0 || nIndex >= m_nCount)
			return(FALSE);

		NOTIFICATION *pNot = (NOTIFICATION *)m_ptrList.GetAt(nIndex);
		if (pNot == NULL)
			return(FALSE);

		m_ptrList.RemoveAt(nIndex);
		m_nCount--;
		delete pNot;
		pNot = NULL;
		return(TRUE);
	}

	NOTIFICATION* CNotifications::GetFirst()
	{
		if (!m_nCount)
			return(NULL);
		m_nGetIndex = 0;
		return((NOTIFICATION *)m_ptrList.GetAt(m_nGetIndex));
	}

	NOTIFICATION* CNotifications::GetNext()
	{
		if (m_nGetIndex+1 == m_nCount)
			return(NULL);
		m_nGetIndex++;
		return((NOTIFICATION *)m_ptrList.GetAt(m_nGetIndex));
	}

	NOTIFICATION* CNotifications::FindExact(HINSTANCE hDll)
	{
		NOTIFICATION *pNot;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (pNot != NULL && pNot->hDll == hDll)
				return(pNot);
		}
		return(NULL);
	}

	void CNotifications::Print(CSession *pSession)
	{
		try
		{
			for (int i = 0; i < m_nCount; i++)
			{
				NOTIFICATION *pNot = 
					reinterpret_cast<NOTIFICATION *>(m_ptrList.GetAt(i));
				if (0 == pNot)
					continue;

				CDll::PtrType dll;
				if(pSession->GetDlls().getByHMOD( pNot->hDll, dll ))
				{
					CString strText;
					strText.Format("%-15s %-20s %s\n",
						m_strTypeName,
						dll->name(),
						pNot->strFunction);

					CGlobals::PrintDebugText(pSession, strText);
				}
			}
		}
		catch(_com_error &)
		{
		}
	}

	///////////////////////////////////////////////////////////////////
	// Connection notifications
	//

	CNotifyConnect::CNotifyConnect() : CNotifications("Connect")
	{

	}

	void CNotifyConnect::Notify(SOCKET hSocket)
	{
		NOTIFICATION *pNot;
		CONNECTPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;
			pProc = (CONNECTPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc)
				pProc(hSocket);
		}
	}

	//
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Connection notifications
	//

	CNotifyDisconnect::CNotifyDisconnect() : CNotifications("Disconnect")
	{

	}

	void CNotifyDisconnect::Notify()
	{
		NOTIFICATION *pNot;
		DISCONNECTPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;
			pProc = (DISCONNECTPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc)
				pProc();
		}
	}

	//
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Chat Connection notifications
	//

	CNotifyChatConnect::CNotifyChatConnect() : CNotifications("ChatConnect")
	{

	}

	void CNotifyChatConnect::Notify(SOCKET hSocket, LPCSTR pszChatName)
	{
		NOTIFICATION *pNot;
		CHATCONNECTPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;
			pProc = (CHATCONNECTPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc)
				pProc(hSocket,pszChatName);
		}
	}

	//
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Chat Disconnection notifications
	//

	CNotifyChatDisconnect::CNotifyChatDisconnect() : CNotifications("ChatDisconnect")
	{

	}

	void CNotifyChatDisconnect::Notify(SOCKET hSocket, LPCSTR pszChatName)
	{
		NOTIFICATION *pNot;
		CHATDISCONNECTPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;
			pProc = (CHATDISCONNECTPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc)
				pProc(hSocket,pszChatName);
		}
	}

	//
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Input Notifications
	//

	CNotifyInput::CNotifyInput() : CNotifications("Input")
	{

	}

	// Returns TRUE if any one of the dlls in the list says it processed it.
	BOOL CNotifyInput::Notify(LPCSTR pszInput)
	{
		BOOL bProcessed = FALSE;

		NOTIFICATION *pNot;
		INPUTPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;

			pProc = (INPUTPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc && pProc(pszInput))
				bProcessed = TRUE;
		}

		return(bProcessed);
	}

	//
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// Raw input (from socket) Notifications
	//

	CNotifyRaw::CNotifyRaw() : CNotifications("Raw")
	{

	}

	// Returns TRUE if any one of the dlls in the list says it processed it.
	BOOL CNotifyRaw::Notify(LPSTR pszBuf)
	{
		BOOL bProcessed = FALSE;

		NOTIFICATION *pNot;
		RAWPROC pProc;
		for (int i=0;i<m_nCount;i++)
		{
			pNot = (NOTIFICATION *)m_ptrList.GetAt(i);
			if (!pNot)
				continue;

			pProc = (RAWPROC)GetProcAddress(pNot->hDll,pNot->strFunction);
			if (pProc && pProc(pszBuf))
				bProcessed = TRUE;
		}

		return(bProcessed);
	}
}