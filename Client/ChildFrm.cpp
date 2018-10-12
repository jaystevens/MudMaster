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
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Ifx.h"
#include "IfxDoc.h"
#include "ChildFrm.h"
#include "Messages.h"
#include "ParseUtils.h"
#include "LogDialog.h"
#include "ActionListDlg.h"
#include "ActionOnlyListDlg.h"
#include "AliasListDlg.h"
#include "EventListDlg.h"
#include "MacroListDlg.h"

#include "Globals.h"

#include "Sess.h"
#include "DefaultOptions.h"

#include ".\childfrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_IP, OnUpdateIndicatorIP)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateIndicatorScrl)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ONLINE, OnUpdateIndicatorOnline)
	ON_WM_SIZE()

	//ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CChildFrame::OnUpdateIndicatorIP(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
void CChildFrame::OnUpdateIndicatorScrl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CChildFrame::OnUpdateIndicatorOnline(CCmdUI *pCmdUI)
{
	CStatusBar *pBar = reinterpret_cast<CStatusBar *>(pCmdUI->m_pOther);
	CIfxDoc *pDoc = dynamic_cast<CIfxDoc *>(GetActiveDocument());
	if(pDoc->Session()->GetConnected())
	{
		pBar->SetPaneText(1, "ONLINE", TRUE);
	}
	else
	{
		pBar->SetPaneText(1, "OFFLINE", TRUE);
	}
	CString strStatsDisplay;
	pBar->SetPaneText(6, pDoc->Session()->GetReturnTripMsecs(), TRUE);	//ReturnTripMsecs
	strStatsDisplay.Format("%d" ,pDoc->Session()->GetCmdsProcessed());
	CParseUtils::Commatize( strStatsDisplay);
	strStatsDisplay = "Cmds: " + strStatsDisplay;
	pBar->SetPaneText(7, strStatsDisplay, TRUE);	//commands
	strStatsDisplay.Format("%d" ,pDoc->Session()->GetBytesReceived() / 1024);
	CParseUtils::Commatize( strStatsDisplay);
	strStatsDisplay = "KBytes: " + strStatsDisplay;

	pBar->SetPaneText(8, strStatsDisplay, TRUE);	//Bytes Received


	pCmdUI->Enable();
}
void CChildFrame::OnUpdateIndicatorOvr(CCmdUI *pCmdUI)
{
	UINT nVK;
	UINT flag = 0x0001;
	nVK = VK_INSERT;
	
	pCmdUI->Enable(::GetKeyState(nVK) & flag);
	ASSERT(pCmdUI->m_bEnableChanged);
}


CMacroList * CChildFrame::GetMacros()
{
	CIfxDoc *pDoc = dynamic_cast<CIfxDoc *>(GetActiveDocument());

	if(pDoc == NULL)
		return NULL;

	return pDoc->Session()->GetMacros();
}


BOOL CChildFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	CIfxDoc *pDoc = (CIfxDoc *)GetActiveDocument();

	// Catch external commands and translate them into local commands for the window.
	if (wParam == ID_EXT_WINDOW_COMMAND) {		
		pDoc->ReceiveBroadcast(UPDATE_EXECUTE_COMMAND, (LPCTSTR)lParam);
		return (TRUE);
	} else if (wParam == UPDATE_PRINT_INCOMING_CHAT || wParam == UPDATE_PRINT_ANSI_NO_TRIGGER) {
		pDoc->ReceiveBroadcast(wParam, (LPCTSTR)lParam);
		return (TRUE);
	}
	return CMDIChildWnd::OnCommand(wParam, lParam);
}

BOOL CChildFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint /*pt*/) 
{
	// TODO: Add your message handler code here and/or call default
	CIfxDoc *pDoc = (CIfxDoc *)GetActiveDocument();

	if(zDelta > 0)
	{
		// Go Up
		if(nFlags & MK_CONTROL)
		{
			pDoc->Session()->ScrollLineUp(/*GetActiveView()*/);
		}
		else
		{
			pDoc->Session()->ScrollPageUp(/*GetActiveView()*/);
		}
	}
	else
	{
		// Go Down
		if(nFlags & MK_CONTROL)
		{
			pDoc->Session()->ScrollLineDown(/*GetActiveView()*/);
		}
		else
		{
			pDoc->Session()->ScrollPageDown(/*GetActiveView()*/);
		}
	}
	return TRUE;
}

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMainFrame *pFrame = (CMainFrame *)GetParentFrame();
	if(pFrame->m_bFullScreenMode)
	{
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
	}
	else
	{
		CIfxDoc *pDoc = (CIfxDoc *)GetActiveDocument();
		
if(pDoc != NULL)
{
		CRect mainRect, childRect, posRect;
		GetWindowRect(&childRect);
		//GetWindowRect(&posRect);
		int toolbarHeight = 22;
		pFrame->GetWindowRect(&mainRect);
		if(pFrame->m_bToolbarFloating) toolbarHeight = -2;
		//TRACE(_T("Main = %d %d, Child = %d %d %d %d \n"), mainRect.top,mainRect.left, childRect.top, childRect.left, childRect.bottom, childRect.right);
		//pDoc->Session()->GetOptions().sessionInfo_.Maximize()
		if(pDoc->Session()->GetMaximize() == "width")
		{
			if(cx==0 && cy==0  && childRect.top >0 &&  childRect.left - mainRect.left >=0)
			{

			//Set positioning rectangle to maintain the top and bottom positions
			//while making the left and right border extend just below the parent
			//frame edges. For some reason the top seems to jump under some
			//circumstances so check to see if it has disappeared under the top
			//of the mainframe and if so bring it back to the edge.
			posRect.left = -4;  //slide under main border by 2 pixels
			posRect.top = childRect.top - (mainRect.top + 54 + toolbarHeight);  //76 the height of title, menu, and toobar?
			if(posRect.top < 0)
				posRect.top = 0;
			posRect.right = posRect.left + mainRect.Width() - 4;  //subtract half the width of main frame border
			posRect.bottom = posRect.top + childRect.Height();
			//TRACE(_T("Mainbottom = %d maintop %d, childbottom %d  diff %d \n"), mainRect.bottom,mainRect.top, childRect.bottom,mainRect.bottom - childRect.bottom );
			// Why is the main frame bottom always 100 pixels more than I expect it to be?
			if(posRect.bottom > ((mainRect.bottom - mainRect.top) - (78 + toolbarHeight)))
				posRect.bottom = (mainRect.bottom - mainRect.top)  - (78 + toolbarHeight) ;
			//Maximaze width to fit main frame
			MoveWindow(posRect.left,
				posRect.top,
				posRect.Width(),
				posRect.Height(),TRUE);
			}
		}
		if(pDoc->Session()->GetMaximize() == "right")
		{
			if(cx==0 && cy==0 && childRect.top >0 &&  childRect.left - mainRect.left >=0)
			{
			posRect.left =	childRect.left - mainRect.left - 6; 
			posRect.top = childRect.top - (mainRect.top + 54 + toolbarHeight);  //76 the height of title, menu, and toobar?
			if(posRect.top < 0)
				posRect.top = 0;
			posRect.right = mainRect.Width() - 6;
			posRect.bottom = posRect.top + childRect.Height();
			if(posRect.bottom > ((mainRect.bottom - mainRect.top) - (78 + toolbarHeight)))
				posRect.bottom = (mainRect.bottom - mainRect.top)  - (78 + toolbarHeight) ;
			//Dock right side to main frame
			MoveWindow(
				posRect.left,
				posRect.top ,
				posRect.Width() ,
				posRect.Height(),
				TRUE );
			}
		}

		//Maximize height to fit main frame 
		if(pDoc->Session()->GetMaximize() == "height")
		{
			SetWindowPos(
				NULL,
				mainRect.left - childRect.left,
				0,
				childRect.Width(),
				mainRect.Height() - 100,
				SWP_NOZORDER );
		}
}
		CMDIChildWnd::OnSize(nType, cx, cy);
	

	}
}




//void CChildFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
//{
//	//CIfxDoc *pDoc = (CIfxDoc *)GetActiveDocument();
//	//if(pDoc->Session()->OnTop()) 
//		//GetSafeHwnd() SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
//	CMDIChildWnd::OnWindowPosChanged(lpwndpos);
//
//	// TODO: Add your message handler code here
//}
