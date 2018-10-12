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

#include "MyInputControl.h"
#include "TermWindow.h"
#include "StatusWindow.h"
#include "SplitterBar.h"
#include "AnsiParser.h"
#include "TaskbarNotifier.h"

namespace MMScript
{
	class CMMEvent;
}

class CIfxDoc;
class CMudSocket;
class CGlobalSoundManager; // We use the Same Object wether we're using SFML or directmusic

[event_receiver(native)]
class CNewWindowView : public CView
{
	static const UINT SCROLL_DIRECTION_NOTSCROLLING	= 0;
	static const UINT SCROLL_DIRECTION_SCROLLUP		= 1;
	static const UINT SCROLL_DIRECTION_SCROLLDOWN	= 2;
	static const UINT MAX_SOCK_RECEIVE				= 5000;

protected:
	CNewWindowView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNewWindowView)

// Attributes
public:
	void handleScrollGrep(ULONG nLines, const CString &searchCriteria);
	void handleTermWindowErrorMessage(const CString &errorMessage);
	void handleScrollGrepMatch(const CStringList &grepMatch);
	void HandleFullScreen();
	BOOL HandleOneLine(const CString &line);

	void hookEvent(CSession *pSession);
	void unhookEvent(CSession *pSession);
	void hookEvent(CSplitterBar *pSession);
	void unhookEvent(CSplitterBar *pSession);

// Operations
public:
	void CloseLog(BOOL bPrintMessage);
	void CloseRawMudLog(BOOL bPrintMessage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewWindowView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CNewWindowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewWindowView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
#ifndef _WIN64
	afx_msg void OnTimer(UINT nIDEvent);
#else
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#endif
	afx_msg void OnSessionConnect();
	afx_msg void OnUpdateSessionConnect(CCmdUI* pCmdUI);
	afx_msg void OnSessionZap();
	afx_msg void OnUpdateSessionZap(CCmdUI* pCmdUI);
	afx_msg void OnSessionReadscript();
	afx_msg void OnSessionLog();
	afx_msg void OnSessionRawMudLog();
	afx_msg void OnUpdateSessionLog(CCmdUI* pCmdUI);
	afx_msg void OnSessionSavescrollback();
	afx_msg void OnSessionStartchatserver();
	afx_msg void OnUpdateSessionStartchatserver(CCmdUI* pCmdUI);
	afx_msg void OnSessionStopchatserver();
	afx_msg void OnUpdateSessionStopchatserver(CCmdUI* pCmdUI);
	afx_msg void OnSessionConfiguration();
	afx_msg void OnViewOptions();
	afx_msg void OnConfigurationActionstriggers();
	afx_msg void OnAliasTree();
	afx_msg void OnConfigurationActions();
	afx_msg void OnConfigurationAliases();
	afx_msg void OnConfigurationEvents();
	afx_msg void OnConfigurationMacros();
	afx_msg void OnConfigurationGags();
	afx_msg void OnConfigurationHighs();
	afx_msg void OnConfigurationSubs();
	afx_msg void OnInputMsgFilter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRequestresizeInputCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnConfigurationStatusbaritems();
	afx_msg void OnDestroy();
	afx_msg void OnInputcontextCopy();
	afx_msg void OnInputcontextCut();
	afx_msg void OnInputcontextPaste();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
CTaskbarNotifier m_wndTaskbarNotifier;
public:
//afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
private:
	// variable used to track performance timing
	int elapsedTime;
   	CIfxDoc* GetDocument(){ return (CIfxDoc*)m_pDocument; }

	void UpdateStatusBarOptions();
	HRESULT SetSoundPath();
	void InitializeDirectSound();
	void RecalcLayout();
	void AddCommandToHistory(CString &strCommand);
	void SearchAndRetrieveCommandHistory(CString &strLookFor, CString &strResult);
	void HandleScreenScrollPageDown();
	void HandleScreenScrollPageUp();
	void HandleCommandScrollDown();
	void HandleScreenScrollLineDown();
	void HandleCommandScrollUp();
	void HandleScreenScrollLineUp();
	void HandleInput();
	void DoTabCompletion();
	void OnMacroKey(UINT nID);
	void PrintSnoop(const char *pszLine);
	void OpenLog(LPCTSTR szLogFilename, BOOL bAppend, BOOL bAddScrollBack);
	void OpenRawMudLog(LPCTSTR szLogFilename, BOOL bAppend, BOOL bAddScrollBack);
	void UpdateAcceleratorTable();
	BOOL ProcessChatLine(const char *pszLine, BOOL bAppendNext=FALSE,  BOOL bDoTriggers = TRUE);
	BOOL ProcessLineNoChat(const char *pszLine, BOOL bAppendNext=FALSE,  BOOL bDoTriggers = TRUE);

	void PrintAnsi(const char *pszText, bool doTriggers);
	BOOL SetBackground(CString &strFilename, CString &strMode);
	BOOL CNewWindowView::ScreenShot(CString &strFilename);
	BOOL CNewWindowView::ScreenShot(CString &strFilename, int nMilliSeconds);
	void OnMacro(UINT nID);
	LRESULT OnLoadScript(WPARAM wParam, LPARAM lParam);
	LRESULT OnDLLCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnConnectToMud(WPARAM wParam, LPARAM lParam);
	LRESULT OnPrintMessage(WPARAM wParam, LPARAM lParam);

	LRESULT ReceiveFromMud(WPARAM wParam, LPARAM lParam);
	void HandleIncomingSocketData(const char *pszData, int nBytes);
	BOOL HandleMusicString(CString &strMusic);
	BOOL HandleSoundString(CString &strSound);
	BOOL ProcessMSP(CColorLine::SpColorLine &line);
	BOOL ProcessLine(CColorLine::SpColorLine &line);
	BOOL ProcessMudLine(const char *pszLine, BOOL bAppendNext, BOOL bPrompt);
	BOOL ProcessStandardInput(const CString &line);
	BOOL ProcessMudmasterProcedure(const CString &line);
	bool ProcessMudmasterCommand(const CString &line);
	bool ProcessRepetitiveCommand(const CString &line);
	bool ProcessOneCommand(const CString &line);

	HRESULT HandleCommandLine(const CString &line);

	void SendTextToMud(const std::string &line);
	void PrintCommand(const std::string &line);
	void DoSpeedWalk(const std::string &line);
	void ConnectToMud();
	BOOL Connected();
	LRESULT CloseMud(WPARAM wParam, LPARAM lParam);
	BOOL ChatServerStarted();
	BOOL AreSnoops();
	void PrintMessage(const char *strText);
	void StartTimer();
	void StartFastTimer(int nMilliseconds = 250);
	void StopFastTimer();
	void UpdateAllOptions();
	void OnSpeedwalk();
	void queueLine(const CString &line);
	void unloadSpeedWalkQueue();
	void UpdateChatName(const CString &strOldChatname);
	void ShowStatusBar();
	bool LastLineGagged() const;
	void LastLineGagged(bool gagged);
	const std::string &LogFileName() const;
	void LogFileName(const std::string &name);
	bool Logging() const;
	void Logging(bool logging);
	const std::string &RawMudLogFileName() const;
	void RawMudLogFileName(const std::string &name);
	bool RawMudLogging() const;
	void RawMudLogging(bool RawMudlogging);
	bool ChangesTimer() const;
	void CheckShowmeViaWebQueue();
	void ChangesTimer(bool changes);
	POSITION &CommandScrollPos();
	void CommandScrollPos(POSITION pos);
	int ScrollDirection() const;
	void ScrollDirection(int direction);
	int InputHeight() const;
	void InputHeight(int height);
	const CString &LastCommand() const;
	void LastCommand(const CString &command);
	bool StatusBarVisible() const;
	void StatusBarVisible(bool visible);
	bool Verbatim() const;
	void Verbatim(bool verbatim);
	INT_PTR SpeedWalkTimer() const;
	void SpeedWalkTimer(INT_PTR timer);

	CStatusWindow   &StatusWindow();
	CSplitterBar    &Terminal();
	CMyInputControl &Input();

	bool _lastLineGagged;
	std::string _logFileName;
	std::string _RawMudlogFileName;
	bool _logging;
	bool _RawMudlogging;
	bool _changesTimer;
	bool _statusBarVisible;
	bool _verbatim;
	POSITION _commandScrollPos;
	int _scrollDirection;
	int _inputHeight;
	CString _lastCommand;
	INT_PTR _speedWalkTimer;

	//TODO encapsulate this var
	bool speedWalking_;

	std::vector<char> previousData_;
	std::auto_ptr<CMudSocket>          _pMudSocket;
	std::auto_ptr<CGlobalSoundManager> _pGlobalSoundManager; // We use the same SoundManager Object wether we're using SFML or DirectMusic
	LogFilePtr                         _pLogFile;
	LogFilePtr                         _pRawMudLogFile;

	CAnsiParser _apSnoopData;
	CAnsiParser _apMudData;
	CAnsiParser _apMessageData;

	CStatusWindow   _wndStatus;
	CMyInputControl _wndInput;
	CSplitterBar    _wndTerm;

	std::vector<CString> speedWalkQueue_;
};


#ifndef _DEBUG  // debug version in IfxView.cpp

#endif
