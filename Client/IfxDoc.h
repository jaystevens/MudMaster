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
// IfxDoc.h : interface of the CIfxDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFXDOC_H__337A98CF_20BC_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_IFXDOC_H__337A98CF_20BC_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SessionHost.h"
#include "Colors.h"
#include "Updates.h"
#include "Sess.h"
#include "ObString.h"


namespace MMScript
{
	class CBarItem;
}

class CIfxDoc 
	: public CDocument
	, public CSessionHost
{
protected: // create from serialization only
	DECLARE_SERIAL(CIfxDoc)
	CIfxDoc();

// Attributes
public:
	// OPERATIONS
	CSession *Session() {return m_pSess;}

	virtual void SetEditText(const CString &strText);
	virtual void QueueMessage(CMessages::MsgType msgType, const CString& strMsg, BOOL bCR=TRUE);
	virtual void PlayASound(CString &strFilename);
    virtual void SaveScrollBack(CString &strFileName);
	virtual void TaskbarNotification(CString &strMessage);

	virtual void ConnectToMud()                    {UpdateAllViews(NULL, UPDATE_CONNECT_TO_MUD, NULL);}
	virtual void CloseMud()                        {UpdateAllViews(NULL, UPDATE_DISCONNECT_FROM_MUD, NULL);}
	virtual void CloseLog()                        {UpdateAllViews(NULL, UPDATE_CLOSE_LOG, NULL);}
	virtual void CloseRawMudLog()                  {UpdateAllViews(NULL, UPDATE_CLOSE_RAWMUDLOG, NULL);}
	virtual void ClearScreen(/*CView * pView*/)    {UpdateAllViews(NULL, UPDATE_CLEARSCREEN, NULL);}
	virtual void ScrollHome(/*CView * pView*/)     {UpdateAllViews(NULL, UPDATE_SCROLL_HOME, NULL);}
	virtual void ScrollLineUp(/*CView * pView*/)   {UpdateAllViews(NULL, UPDATE_SCROLL_LINEUP, NULL);}
	virtual void ScrollLineDown(/*CView * pView*/) {UpdateAllViews(NULL, UPDATE_SCROLL_LINEDOWN, NULL);}
	virtual void ScrollPageUp(/*CView * pView*/)   {UpdateAllViews(NULL, UPDATE_SCROLL_PAGEUP, NULL);}
	virtual void ScrollPageDown(/*CView * pView*/) {UpdateAllViews(NULL, UPDATE_SCROLL_PAGEDOWN, NULL);}
	virtual void EndScrollBack(/*CView * pView*/)  {UpdateAllViews(NULL, UPDATE_END_SCROLLBACK, NULL);}
	virtual void StartLog()                        {UpdateAllViews(NULL, UPDATE_AUTO_LOG, NULL);}
	virtual void StartChangesThread()              {UpdateAllViews(NULL, UPDATE_CHANGES_THREAD_START, NULL);}
	virtual void SubmitEditText()                  {UpdateAllViews(NULL, UPDATE_SUBMIT_EDITTEXT, NULL);}
	virtual void UpdateStatusBarOptions()          {UpdateAllViews(NULL, UPDATE_STATUS_BAR_OPTIONS, NULL);}
    virtual void TimeStampLog()                    {UpdateAllViews(NULL, UPDATE_TIMESTAMP_LOG, NULL);}
    virtual void LoadScript()                      {UpdateAllViews(NULL, UPDATE_AUTO_LOAD_SCRIPT, NULL);}
	virtual void UpdateMacros()                    {UpdateAllViews(NULL, UPDATE_MACROS_CHANGED);}
	virtual void HideStatusBar()                   {UpdateAllViews(NULL, UPDATE_BAR_HIDE, NULL);}
	virtual void ToggleVerbatim()                  {UpdateAllViews(NULL, UPDATE_TOGGLE_VERBATIM, NULL);}	
	virtual void UpdateBarItem(MMScript::CBarItem *pItem) {UpdateAllViews(NULL, UPDATE_BAR_DRAWITEM, (CObject *)pItem);}

	// Communications with other windows
    virtual void BroadCastToWindow(CString &window, WPARAM external_cmd, WPARAM local_cmd, const char *message);  // Sends message to other window or locally
	virtual void ReceiveBroadcast(WPARAM wm_command, const char *message); // Executes message from another window (must be caught in ChildFrm.cpp)
	virtual void SendCommandToAnotherWindow(CString &window, const char *command);
	virtual void SendTextToAnotherWindow(CString &window, const char *text, bool dotriggers);
	
	virtual void ExecuteCommand(LPCTSTR pszCommand);
	virtual void RemoveWindowCaption();
	virtual void AddWindowCaption();
	virtual void MaxRestoreSession();
	virtual HRESULT PrintSnoop(const char *pszLine);
	virtual void OpenLog(CString &strFileName, bool bAppend);
	virtual void OpenRawMudLog(CString &strFileName, bool bAppend);
	virtual void UsePlaySound(BOOL bUsePlaySound);

// Implementation
public:
	virtual void ReadScript();
	virtual ~CIfxDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CWnd* ThisWnd(); // Returns the session window pointer, that is used by this document.

// Generated message map functions
protected:
	//{{AFX_MSG(CIfxDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnWindowSavePosition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSession *m_pSess;
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnSaveSessionPosition();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IFXDOC_H__337A98CF_20BC_11D4_BCD6_00E029482496__INCLUDED_)
