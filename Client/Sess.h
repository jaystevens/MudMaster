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
#if !defined(AFX_SESS_H__3C3A6EC4_39A0_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_SESS_H__3C3A6EC4_39A0_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Sess.h : header file
//
#include "SessionHost.h"
#include "IChatOutput.h"
#include "ChatServer.h"
#include "ODBCWrapper.h"

class CSessionImpl;
class COptions;

namespace SerializedOptions
{
	class CMessageOptions;
	class CTriggerOptions;
	class CPathOptions;
	class CTerminalOptions;
	class CInputControlOptions;
	class CChatOptions;
	class CColorOptions;
	class CStatusBarOptions;
	class CDebugOptions;
	class CSessionInfo;
	class CMspOptions;
}

// Script Entities
namespace MMSystem
{
	class CDllList;
}

namespace MMScript
{
	class CSubList;
	class CHighList;
	class CAliasList;
	class CActionList;
	class CGagList;
	class CMMStatusBar;
	class CArrayList;
	class CTabList;
	class CEventList;
	class CUserLists;
	class CMacroList;
	class CVarList;
}

class CUndo;
class CStringList;
class CGroupList;
class CDebugStack;

/////////////////////////////////////////////////////////////////////////////
// CSession command target
[event_source(native)]
class CSession
	: public ISessionOutput
	, public MMChatServer::IChatServerClient
{
//	DECLARE_SERIAL(CSession)
public:
	CSession();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	CSession(CSessionHost *pHost);
	virtual ~CSession();

	virtual HRESULT PrintChatMessage(PCTSTR pszText);
	virtual HRESULT PrintChatMessage(UINT nStringId);

	BOOL Read(LPCTSTR lpszPathName);
	BOOL Save(LPCTSTR lpszPathName);
	BOOL SaveSessionPosition();
	void LoadScript();
	void SetHost(CSessionHost *pHost);
	CSessionHost *GetHost();

	bool printAutoList();
	bool addAuto( const CString &command, const CString &execute );
	int GetChatServerPort() const;
	bool GetAutoAccept() const;
	bool GetAutoServe() const;
	bool GetDoNotDisturb() const;
	CString GetMaximize() const;
	BYTE GetChatForeColor() const;
	BYTE GetChatBackColor() const;
	HRESULT GetSoundPath(std::string &path) const;
	HRESULT GetUploadPath(std::string &path) const;
	HRESULT GetDownloadPath(std::string &path) const;
	HRESULT GetFileVersion(std::string &version) const;
	HRESULT GetIpAddress(std::string &address) const;

	HRESULT SendGroupToChat(
	const std::string &groupName, 
	const std::string &personName);

	void ProcessEvents();
	void RedrawStatusBar();

protected:

public:
	BOOL SystemVar( const CString &strVarName, CString &strResult);
	bool SystemVar( const std::string &varName, std::string &result	);

	virtual HRESULT PrintChatText(const char *szMsg);
	virtual HRESULT PrintAnsiNoProcess(LPCTSTR strText);
	void PrintAnsiNoProcess(const std::string &text);
	void PrintNoChatLine(LPCTSTR strText);
	virtual HRESULT StartChatServer();
	virtual HRESULT StopChatServer();
	virtual HRESULT CloseChatSocket(LPCTSTR strChatID);

	virtual void ToggleVerbatim()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ToggleVerbatim();}

	virtual void OpenLog(CString &strFileName, bool bAppend)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->OpenLog(strFileName, bAppend);}

	virtual void CloseLog()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->CloseLog();}

	virtual void OpenRawMudLog(CString &strFileName, bool bAppend)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->OpenRawMudLog(strFileName, bAppend);}

	virtual void CloseRawMudLog()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->CloseRawMudLog();}

	virtual void UsePlaySound(BOOL bUsePlaySound)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->UsePlaySound(bUsePlaySound);}

	virtual void QueueMessage(CMessages::MsgType msgType, const CString& strMsg, BOOL bCR=FALSE) 
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->QueueMessage(msgType, strMsg, bCR);}

	virtual void UpdateBarItem(MMScript::CBarItem *pItem) 
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->UpdateBarItem(pItem);}

	virtual void SaveScrollBack(CString& strFilename)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->SaveScrollBack(strFilename);}

	virtual void ScrollHome(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ScrollHome(/*pView*/);}
	virtual void ClearScreen(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ClearScreen(/*pView*/);}

	virtual void ScrollLineUp(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ScrollLineUp(/*pView*/);}

	virtual void ScrollLineDown(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ScrollLineDown(/*pView*/);}

	virtual void ScrollPageUp(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ScrollPageUp(/*pView*/);}

	virtual void ScrollPageDown(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ScrollPageDown(/*pView*/);}

	void SetEditText(const std::string &text);

	virtual void SetEditText(LPCTSTR strEditText)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->SetEditText(strEditText);}

	virtual void TimeStampLog()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->TimeStampLog();}

	virtual void EndScrollBack(/*CView *pView*/)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->EndScrollBack(/*pView*/);}
	
	virtual void SubmitEditText()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->SubmitEditText();}

	virtual void PlayASound(CString &strFilename)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->PlayASound(strFilename);}

	virtual HRESULT PrintSnoop(const char *pszLine) 
		{ASSERT(m_pSessionHost != NULL); return m_pSessionHost->PrintSnoop(pszLine);}

	virtual void UpdateStatusBarOptions(void)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->UpdateStatusBarOptions();}

	virtual void StartChangesThread()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->StartChangesThread();}

	virtual void UpdateMacros()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->UpdateMacros();}

	virtual void ConnectToMud()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->ConnectToMud();}

	virtual void HideStatusBar()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->HideStatusBar();}

	virtual void CloseMud()
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->CloseMud();}

	virtual void TaskbarNotification(CString &strMessage)
		{ASSERT(m_pSessionHost != NULL); m_pSessionHost->TaskbarNotification(strMessage);}

	CStringList &GetCommandHistoryBuffer();
	CStringList &GetShowmeViaWebBuffer();

	COptions &GetOptions() const;

	// Script Entities
	MMSystem::CDllList        &GetDlls();
	MMScript::CSubList        *GetSubs();
	MMScript::CHighList       *GetHighlights();
	MMScript::CAliasList      *GetAliases();
	MMScript::CActionList     *GetActions();
	MMScript::CGagList        *GetGags();
	MMScript::CEventList      *GetEvents();
	MMScript::CMMStatusBar    *GetStatusBar();
	MMScript::CUserLists      *GetLists();
	MMChatServer::CChatServer &GetChat();
	CUndo                     &Undo();
	MMScript::CArrayList      *GetArrays();
	MMScript::CMacroList      *GetMacros();
	MMScript::CVarList        *GetVariables();
	MMScript::CTabList        *GetTabList();
	CStringList               &GetAutoList();
	CGroupList                &GetGroups();

	CDebugStack & DebugStack();

	void ReadMode(BOOL bReadMode);
	BOOL ReadMode();
	void OnTop(BOOL bonTop);
	BOOL OnTop();
	int GetCmdDepth();
	void SetCmdDepth(int nNewValue);
	long GetLoopCount();
	void SetLoopCount(long nNewValue);
	long GetLinesReceived();
	void SetLinesReceived(long nNewValue);
	long GetBytesReceived();
	void SetBytesReceived(long nNewValue);
	CString GetCurrentLogFile();
	void SetCurrentLogFile(CString strNewValue);
	CString GetReturnTripMsecs();
	void SetReturnTripMsecs();
	void SetANSICopy(CString sNewValue);
	long GetCmdsProcessed();
	void SetCmdsProcessed(long nNewValue);
	time_t GetTimeSinceLastInput();
	void SetTimeSinceLastInput();
	BOOL GetConnected();
	BOOL GetHandledByDll();
	void SetConnected(BOOL bValue);
	void SetHandledByDll(BOOL bValue);
	CString &GetListItem();
	void SetListItem(LPCTSTR lpszNewValue);
	void KillAll();
	void KillSpeedwalk()
	{
		m_strSpeedwalkQueue.Empty();
	}
	LPCSTR CSession::GetTelnetAuthkeyResponse();
	void CSession::SetTelnetAuthkeyResponse(LPCSTR newValue);					
	void CSession::SetTelnetNAWS(bool newValue);					
	bool CSession::GetTelnetNAWS();	
	void CSession::SetTelnetNAWSmessage(LPCSTR newValue);					
	LPCSTR CSession::GetTelnetNAWSmessage();	
	__event void ScrollGrep(ULONG nLines, const CString &searchCriteria);
	__event void PrintAnsi(const char *pszMessage, bool doTriggers);
	__event BOOL SetBackground(CString &strFilename, CString &strMode);
	__event BOOL ScreenShot(CString &strFilename);
	__event HRESULT ExecuteCommand(const CString &line);
	__event void FullScreen();
	__event void RedrawStatusBar(CSession *pSession);
	__event void ShowStatusBar();
	__event BOOL ProcessLineNoChat(const char *pszLine, BOOL bAppendNext=FALSE,  BOOL bDoTriggers = TRUE);

public:
	void AppendToSpeedwalkQueue(CString strAppend)
	{
		m_strSpeedwalkQueue += strAppend;
	}

	TCHAR GetNextSpeedwalkStep()
	{
		TCHAR ch = m_strSpeedwalkQueue.GetAt(0);
		m_strSpeedwalkQueue = m_strSpeedwalkQueue.Mid(1);
		return ch;
	}

	bool SpeedwalkDone()
	{
		return m_strSpeedwalkQueue.IsEmpty() ? true : false;
	}

private:
	CSessionImpl *m_pThis;
	CSessionHost *m_pSessionHost;
	CString m_strSpeedwalkQueue;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SESS_H__3C3A6EC4_39A0_11D4_BCD6_00E029482496__INCLUDED_)
