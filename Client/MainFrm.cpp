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
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Ifx.h"

#include "MainFrm.h"
#include "Messages.h"
#include "ChildFrm.h"
#include "MacroList.h"
#include "Macro.h"
#include "ChatServer.h"
#include "Sess.h"
#include <dsound.h>
#include "Globals.h"
#include ".\mainfrm.h"
#include "NewSessionWizard.h"
#include "DefaultsDialog.h"
#include "FileUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SYSTEMTRAY_USEW2K
#define NIIF_WARNING 0
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_HELP_SENDBUGREPORT, OnHelpSendbugreport)
	ON_COMMAND(ID_HELP_FINDER, CMainFrame::OnHelpFinder)
	ON_COMMAND(ID_HELP_SCRIPTFORUMFORMUDMASTER2K6, CMainFrame::OnHelpScriptForum)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_DEMO_MAXIMISE, OnDemoMaximise)
	ON_COMMAND(ID_DEMO_MINIMISE, OnDemoMinimise)
	ON_COMMAND(ID_FULL_SCREEN, OnFullScreen)
	ON_UPDATE_COMMAND_UI(ID_FULL_SCREEN, OnUpdateFullScreen)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_IP, OnUpdateIndicatorIP)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateIndicatorScrl)
	//ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	//ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ONLINE, OnUpdateIndicatorOnline)
	ON_COMMAND(ID_FILE_WIZARD, OnFileWizard)
	ON_COMMAND(ID_FILE_EDITDEFAULTS, OnFileEditdefaults)
	ON_COMMAND(ID_WINDOW_CASCADE,OnWindowCommand)
	ON_COMMAND(ID_WINDOW_TILE_HORZ,OnWindowCommand)
	ON_COMMAND(ID_WINDOW_TILE_VERT,OnWindowCommand)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ONLINE,
	ID_INDICATOR_IP,
	ID_INDICATOR_OVR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,	//hijack this for something else
	ID_INDICATOR_CMDS,	//use for commands processed 
	ID_INDICATOR_KBYTES,	//use for byte received
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_bFullScreenMode(FALSE)
{
	// TODO: add member initialization code here
	m_bToolBarWasVisible = FALSE;
	m_bMenuBarWasVisible = TRUE;
	m_bStatusBarWasVisible = FALSE;
	m_bWorkspaceWasVisible=FALSE;
	m_bOutputBarWasVisible=FALSE;
	memset(&m_WindowPlacement, 0, sizeof(m_WindowPlacement));
	m_bValidWindowPlacement = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	// Icon to use
	HICON hIcon;
	if(CGlobals::GetIconPath() == _T("NoIcon"))
	{
		hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)); 
	}
	else
	{
		TCHAR iconfile[90];
		_tcscpy(iconfile, CGlobals::GetIconPath());
		hIcon = ::ExtractIcon(AfxGetResourceHandle(),iconfile,0);
		CMDIFrameWnd::SetIcon(hIcon,FALSE);
	}

  
	if(CGlobals::GetMinimiseToTray() == _T("hide"))
	{
		if (!m_TrayIcon.Create(
			NULL,                           // Let icon deal with its own messages
			CMessages::WM_ICON_NOTIFY,      // Icon notify message to use
			CGlobals::GetMsgBoxTitle(),     //_T("MudMaster 2k6 v4.1"),		// tooltip
			hIcon,
			IDR_POPUP_MENU))				// ID of tray icon
		{
			return -1;
		}
		m_TrayIcon.SetMenuDefaultItem(3, TRUE);
	}
	//else
	//{
	//	if (!m_TrayIcon.Create(
	//		NULL,                           // Let icon deal with its own messages
	//		CMessages::WM_ICON_NOTIFY,      // Icon notify message to use
	//		CGlobals::GetMsgBoxTitle(),     //_T("MudMaster 2k6 v4.1"),		// tooltip
	//		hIcon,
	//		IDR_TRAY_MENU1))				// ID of tray icon
	//	{
	//		return -1;
	//	}

	//	m_TrayIcon.SetMenuDefaultItem(0, TRUE);
	//}

	// Load toolbar user images:
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD | 
		WS_VISIBLE | 
		CBRS_TOP| 
		CBRS_GRIPPER | 
		CBRS_TOOLTIPS | 
		CBRS_HIDE_INPLACE|
		CBRS_FLYBY | 
		CBRS_BORDER_3D |
		CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_IFXTYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_bToolbarFloating = FALSE;  //starts docked
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	CString strIP;
	strIP.Format("\t%s", CGlobals::GetIPAddress());
	m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH,150);
	m_wndStatusBar.SetPaneText(2, strIP, TRUE);
	
	CString strMainToolbarTitle;
	//strMainToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	strMainToolbarTitle = CGlobals::GetMsgBoxTitle();  // use the branding option
	m_wndToolBar.SetWindowText (strMainToolbarTitle);
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	

	// Read the saved window position information
	// and set the window placement accordingly
	CWinApp* pApp = ::AfxGetApp();
	UINT nSize;
	WINDOWPLACEMENT* pwp;
	if (pApp->GetProfileBinary(_T("Window"), _T("Position"), (LPBYTE*)&pwp, &nSize))
	{
		// Success
		::memcpy((void *)&m_WindowPlacement, pwp, sizeof(m_WindowPlacement));
		SetWindowPlacement(&m_WindowPlacement);

	}
		delete [] pwp; // free the buffer
LoadBarState("mm2k6Toolbars");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
//cs.style &= ~FWS_ADDTOTITLE;
return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
	CString strTitle, strCustom;
	CMDIFrameWnd::GetWindowText(strTitle);
	int iTitlePos = strTitle.Find(" - ");
	strCustom = CString(CGlobals::GetMsgBoxTitle());
	if(strTitle.Find(" - [") == -1 && iTitlePos > 0)
	{
		strCustom += strTitle.Mid(iTitlePos);
		
	}
	CMDIFrameWnd::SetWindowText(strCustom);
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd* pChild =(CMDIChildWnd *)MDIGetActive();
	CMDIChildWnd* pStartingChild =(CMDIChildWnd *)MDIGetActive(); ;
	//pStartingChild = pChild;
	
	if(pChild != NULL && pChild->IsWindowEnabled())
	{
		//step through all children
		while (pChild)
		{
			pChild->PostMessage(WM_SIZE);
			// Get the next child window in the list.
			pChild =
				(CMDIChildWnd*)pChild->GetWindow(GW_HWNDNEXT);
			
		}

		//    CWnd *target          = windowContainer->GetWindow(GW_CHILD);

		pStartingChild->SetFocus();
	}	
	CMDIFrameWnd::OnSize(nType,cx,cy);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (HIWORD(wParam) == 1)
	{
		// Low word of wParam is the message that the accelerator
		// table has associated with that key.
		WORD wMessage = LOWORD(wParam);
		if (wMessage >= ID_MACRO_START && wMessage <= ID_MACRO_END)
		{
			// Let the view know about the macro and pass in the
			// macro index so the view can look it up.
			CChildFrame *pFrame = (CChildFrame *)GetActiveFrame();
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame)));
			if(pFrame)
			{
				WPARAM param = MAKEWPARAM(wMessage, 0);
				pFrame->PostMessage(WM_COMMAND, param, NULL);
			}
			return(TRUE);
		}
	}
	return CMDIFrameWnd::OnCommand(wParam, lParam);
}
void CMainFrame::OnWindowCommand() 
{   
   // Get the message the window is currently processing. Since we are
   // in the OnMessage handler, the message we get back must be a 
   // WM_COMMAND message, and its low-order word of wParam is the ID of 
   // menu item being selected.
   const MSG* msg = GetCurrentMessage();
	CMDIChildWnd* pChild =(CMDIChildWnd *)MDIGetActive();

   switch (LOWORD(msg->wParam))
   {
   case ID_WINDOW_ARRANGE:   // For Window\Arrange Icons menu item, arrange
      MDIIconArrange();     // all minimized document child windows.
      break;

   case ID_WINDOW_CASCADE:  // For Window\Cascade menu item, arrange
	   //Must make sure child windows have title caption when tiling or cascading
	   if(pChild != NULL && pChild->IsWindowEnabled())
	   {
		   //step through all children
		   while (pChild)
		   {
			   CWnd *target          = pChild->GetWindow(GW_CHILD)->GetParent();
			   target->ModifyStyle(0,WS_CAPTION,SWP_FRAMECHANGED);
			   // Get the next child window in the list.
			   pChild =
				   (CMDIChildWnd*)pChild->GetWindow(GW_HWNDNEXT);
		   }
	   }	

	   MDICascade();         // all the MDI child windows in a cascade format.

	   break;

   case ID_WINDOW_TILE_HORZ:       // For Window\Tile Horizontal menu item, 
	   //Must make sure child windows have title caption when tiling or cascading
	   if(pChild != NULL && pChild->IsWindowEnabled())
	   {
		   //step through all children
		   while (pChild)
		   {
			   CWnd *target          = pChild->GetWindow(GW_CHILD)->GetParent();
			   target->ModifyStyle(0,WS_CAPTION,SWP_FRAMECHANGED);
			   // Get the next child window in the list.
			   pChild =
				   (CMDIChildWnd*)pChild->GetWindow(GW_HWNDNEXT);
		   }
	   }	
      MDITile(MDITILE_HORIZONTAL); // tile MDI child windows so that
      break;                       // one window appears above another.
      case ID_WINDOW_TILE_VERT:       // For Window\Tile Vertical menu item, 
	   //Must make sure child windows have title caption when tiling or cascading
	   if(pChild != NULL && pChild->IsWindowEnabled())
	   {
		   //step through all children
		   while (pChild)
		   {
			   CWnd *target          = pChild->GetWindow(GW_CHILD)->GetParent();
			   target->ModifyStyle(0,WS_CAPTION,SWP_FRAMECHANGED);
			   // Get the next child window in the list.
			   pChild =
				   (CMDIChildWnd*)pChild->GetWindow(GW_HWNDNEXT);
		   }
	   }	
      MDITile(MDITILE_VERTICAL); // tile MDI child windows so that
      break;                       // one window appears beside another.
 
   }
}

void CMainFrame::ActivateFrame(int /*nCmdShow*/) 
{
	CFrameWnd *pFrame = GetActiveFrame();
	pFrame->PostMessage(WM_SETFOCUS, NULL, NULL);
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CWnd* pWndFocus = GetFocus ();

	if	(pWndFocus != NULL)
	{
		//see if the window can handle this message
		if (pWndFocus->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo))
		{
			return TRUE;
		}
	}
	return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnHelpSendbugreport() 
{
	std::string v;
	CGlobals::GetFileVersion(v);
//TODO: KW update email address
	CString strSubject;
	strSubject.Format("mailto:ghoti_20@users.sourceforge.net?subject=Bug Report v%s", v.c_str());
	//strSubject = "notepad.exe";
	ShellExecute(NULL, NULL, strSubject, NULL, NULL,SW_SHOW);
}

void CMainFrame::OnHelpFinder() 
{
       CString helppath;
	   
	   helppath = "http://mm2k6.sourceforge.net/mm2k6/";
			//"file://" + MMFileUtils::fixPath("helphtml\\index.html");

		ShellExecute(NULL, NULL, helppath, NULL, NULL,SW_SHOW);
}
void CMainFrame::OnHelpScriptForum() 
{
       CString helppath;
	   
	   helppath = "https://sourceforge.net/projects/mm2k6/forums/forum/704930/";
			//"file://" + MMFileUtils::fixPath("helphtml\\index.html");

		ShellExecute(NULL, NULL, helppath, NULL, NULL,SW_SHOW);
}

void CMainFrame::OnClose() 
{
	CWinApp* pApp = ::AfxGetApp();
	GetWindowPlacement(&m_WindowPlacement);
	pApp->WriteProfileBinary(_T("Window"), _T("Position"), (LPBYTE)&m_WindowPlacement, sizeof(m_WindowPlacement));
    SaveBarState("mm2k6Toolbars");
	m_TrayIcon.RemoveIcon();	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);
}

void CMainFrame::OnDemoMaximise() 
{
	if(!m_bValidWindowPlacement)
		return;
	if(CGlobals::GetMinimiseToTray() == _T("hide"))
	{
		CSystemTray::MaximiseFromTray(this);
		m_TrayIcon.SetMenuDefaultItem(ID_DEMO_MINIMISE, FALSE);
	}
	
	SetWindowPlacement(&m_WindowPlacement);

}

void CMainFrame::OnDemoMinimise() 
{
	m_WindowPlacement.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&m_WindowPlacement);
	m_bValidWindowPlacement = TRUE;
	if(CGlobals::GetMinimiseToTray() == _T("hide"))
	{
		CSystemTray::MinimiseToTray(this);
		m_TrayIcon.SetMenuDefaultItem(ID_DEMO_MAXIMISE, FALSE);	
	}
}


void CMainFrame::OnUpdateIndicatorIP(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
void CMainFrame::OnUpdateIndicatorScrl(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateIndicatorOnline(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateIndicatorOvr(CCmdUI *pCmdUI)
{
	UINT nVK;
	UINT flag = 0x0001;
	nVK = VK_INSERT;

	pCmdUI->Enable(::GetKeyState(nVK) & flag);
	ASSERT(pCmdUI->m_bEnableChanged);
}


void CMainFrame::OnFullScreen() 
{
	if(m_bFullScreenMode)
		RestoreFromFullScreen();
	else
		ConvertToFullScreen();
}

void CMainFrame::OnUpdateFullScreen(CCmdUI* /*pCmdUI*/) 
{
}

void CMainFrame::ConvertToFullScreen()
{
	// available only if there is an active doc
	CMDIChildWnd* pChild=(CMDIChildWnd *)MDIGetActive();

	if(!pChild) 
		return;

	m_bToolBarWasVisible=(m_wndToolBar.IsWindowVisible()!=0);
	m_wndToolBar.ShowWindow(SW_HIDE);
	m_bStatusBarWasVisible=(m_wndStatusBar.IsWindowVisible()!=0);
	m_bToolbarFloating = FALSE;
	m_wndStatusBar.ShowWindow(SW_HIDE);

	// now save the old positions of the main and child windows
	GetWindowRect(&m_mainRect);
	// remove the caption of the mainWnd:
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style&=~WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);

	//SetMenu(NULL);

	CRect rectDesktop;
	//::GetWindowRect ( ::GetDesktopWindow(), &rectDesktop );
	//::AdjustWindowRectEx(&rectDesktop, GetStyle(), TRUE,	GetExStyle());
	int screenx=GetSystemMetrics(SM_CXSCREEN);
	int screeny=GetSystemMetrics(SM_CYSCREEN);
	TRACE(_T("screenx = %d, screeny = %d"), screenx, screeny);
	// resize:
	int cxBorder = GetSystemMetrics(SM_CXFRAME);
	int cyBorder = GetSystemMetrics(SM_CYFRAME);
	SetWindowPos(
		NULL,
		-cxBorder,
		-cyBorder,
		screenx+(cxBorder*2),
		screeny+(cxBorder*2),
		SWP_NOZORDER);
	style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);

	m_bChildMax=(style & WS_MAXIMIZE)?TRUE:FALSE;
	// note here: m_bMainMax is not needed since m_hWnd only
	// changed its caption...

	// and maximize the child window
	// it will remove its caption, too.
	pChild->ShowWindow(SW_SHOWMAXIMIZED);
	RecalcLayout();
	m_bFullScreenMode=TRUE;	
}

void CMainFrame::RestoreFromFullScreen()
{
	m_bFullScreenMode=FALSE;
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE);
	style|=WS_CAPTION;
	::SetWindowLong(m_hWnd,GWL_STYLE,style);
	if(m_bToolBarWasVisible)
		m_wndToolBar.ShowWindow(SW_SHOW);
	m_bToolbarFloating = !m_bStatusBarWasVisible;
	if(m_bStatusBarWasVisible)
		m_wndStatusBar.ShowWindow(SW_SHOW);

	//if(m_bWorkspaceWasVisible)
	//	m_wndWorkSpace.ShowWindow(SW_SHOW);
	//if(m_bOutputBarWasVisible)
	//	m_wndOutput.ShowWindow(SW_SHOW);

	MoveWindow(&m_mainRect);

	//m_Menu.DestroyMenu();
	//m_Menu.LoadMenu(IDR_IFXTYPE);
	//SetMenu(&m_Menu);

	RecalcLayout();
	CMDIChildWnd* pChild=(CMDIChildWnd *)MDIGetActive();
	// pchild can be NULL if the USER closed all the
	// childs during Full Screen Mode:
	if(pChild){
		if(m_bChildMax)
			MDIMaximize(pChild);
		else MDIRestore(pChild);
	}
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	if(nID == SC_MINIMIZE && CGlobals::GetMinimiseToTray() == _T("hide"))
	{
		OnDemoMinimise();
		return;
	}

	CMDIFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnFileWizard()
{
	Wizards::CNewSessionWizard dlg(_T("Create a new session"));
	INT_PTR nRes = dlg.DoModal();
	if(ID_WIZFINISH == nRes)
	{
		if(dlg.completePage_._connectNow)
		{
			AfxGetApp()->OpenDocumentFile(dlg.completePage_._sessionPath);
		}
	}
}

void CMainFrame::OnFileEditdefaults()
{
	CDefaultsDialog dlg;
	dlg.DoModal();
}
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    LPNMHDR pnmh = (LPNMHDR) lParam; 
    if(pnmh->hwndFrom == m_wndToolBar.m_hWnd)
    {
		m_bToolbarFloating = m_wndToolBar.IsFloating() ;  //
    }
    return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}
