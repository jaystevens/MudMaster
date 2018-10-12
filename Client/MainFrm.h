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
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__337A98CB_20BC_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_MAINFRM_H__337A98CB_20BC_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SystemTray.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnWindowCommand();

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHelpSendbugreport();
	afx_msg void OnHelpFinder();
	afx_msg void OnHelpScriptForum();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDemoMaximise();
	afx_msg void OnDemoMinimise();
	afx_msg void OnViewCaptions();
	afx_msg void OnUpdateViewCaptions(CCmdUI* pCmdUI);
	afx_msg void OnForceGradient();
	afx_msg void OnUpdateForceGradient(CCmdUI* pCmdUI);
	afx_msg void OnWindowManager();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnFullScreen();

protected:
	afx_msg void OnUpdateFullScreen(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	//TODO: KW so I can catch toolbar notifications
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult); 
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg LRESULT OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp);
	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorIP(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorScrl(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorOnline(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorOvr(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
 virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
private:
	void RestoreFromFullScreen();
	void ConvertToFullScreen();
	CSystemTray m_TrayIcon;

	// Added for Full Screen Mode
public:
	BOOL m_bFullScreenMode;
	BOOL m_bToolbarFloating;

private:
	BOOL m_bToolBarWasVisible;
	BOOL m_bStatusBarWasVisible;
	BOOL m_bMenuBarWasVisible;
	BOOL m_bWorkspaceWasVisible;
	BOOL m_bOutputBarWasVisible;
	CRect m_mainRect;
	BOOL m_bChildMax;

	WINDOWPLACEMENT m_WindowPlacement;
	BOOL m_bValidWindowPlacement;

	CMenu m_Menu;

public:
	afx_msg void OnFileWizard();
	afx_msg void OnFileEditdefaults();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__337A98CB_20BC_11D4_BCD6_00E029482496__INCLUDED_)
