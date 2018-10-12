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
// IfxDoc.cpp : implementation of the CIfxDoc class
//

#include "stdafx.h"
#include "Ifx.h"

#include "IfxDoc.h"
#include "MainFrm.h"

#include "ObString.h"
#include "Sess.h"
#include "Globals.h"
#include ".\ifxdoc.h"
#include "DefaultOptions.h"
#include "colors.h"
#include "NewWindowView.h"

using namespace MMScript;
using namespace MudmasterColors;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CIfxDoc, CDocument, 1);

/////////////////////////////////////////////////////////////////////////////
// CIfxDoc

BEGIN_MESSAGE_MAP(CIfxDoc, CDocument)
	//{{AFX_MSG_MAP(CIfxDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
	ON_COMMAND(ID_WINDOW_SAVE_POSITION, OnSaveSessionPosition)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIfxDoc construction/destruction

CIfxDoc::CIfxDoc()
: m_pSess(NULL)
{
	m_pSess = new CSession;
	m_pSess->SetHost(this);
}

CIfxDoc::~CIfxDoc()
{
	delete m_pSess;
}


/////////////////////////////////////////////////////////////////////////////
// CIfxDoc diagnostics

#ifdef _DEBUG
void CIfxDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIfxDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIfxDoc commands
	// ACCESSORS

// OPERATIONS
void CIfxDoc::SetEditText(const CString &strText) 
{
	CObString osText;

	CString strTemp = strText;
	osText.Value(strTemp);
	UpdateAllViews(NULL, UPDATE_SET_EDIT_TEXT, &osText);
}

void CIfxDoc::QueueMessage(CMessages::MsgType msgType, const CString& strMsg, BOOL bCR/*=TRUE*/) 
{
	ASSERT(m_pSess != NULL);
	BOOL bPrint = FALSE;
	switch(msgType)
	{
	case CMessages::MM_VARIABLE_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.VariableMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_ACTION_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.ActionMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_ALIAS_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.AliasMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_AUTO_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.AutoexecMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_CHAT_MESSAGE:
		bPrint = TRUE;
		break;
	case CMessages::MM_EVENT_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.EventMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_MACRO_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.MacroMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_GENERAL_MESSAGE:
			bPrint = TRUE;
		break;
	case CMessages::MM_TAB_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.TabMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_ARRAY_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.ArrayMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_MATH_MESSAGE:
		bPrint = TRUE;
		break;
	case CMessages::MM_DLL_MESSAGE:
		bPrint = TRUE;
		break;
	case CMessages::MM_PROCEDURE_MESSAGE:
		bPrint = TRUE;
		break;
	case CMessages::MM_LIST_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.ListMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_GAG_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.GagMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_HIGHLIGHT_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.HighMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_SUB_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.SubMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_ITEM_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.ItemMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_BAR_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.BarMessages())
			bPrint = TRUE;
		break;
	case CMessages::MM_ENABLE_MESSAGE:
		if(m_pSess->GetOptions().messageOptions_.EnableMessages())
			bPrint = TRUE;
	default:
		break;
	};

	if(!bPrint)	return;

	CString strText;
	if (bCR)
		strText.Format("# %s\n", strMsg);
	else
		strText.Format("# %s", strMsg);
	
	CObString osString;
	osString.Value(strText);
	UpdateAllViews(NULL, UPDATE_PRINT_MESSAGE, &osString);
}

void CIfxDoc::ExecuteCommand(LPCTSTR pszCommand) {
	try
	{
		CObString osString(pszCommand);
		UpdateAllViews(NULL, UPDATE_EXECUTE_COMMAND, &osString);
	}
	catch(...)
	{
		::OutputDebugString(_T("Error in CIfxDoc::ExecuteCommand\n"));
	}
}

void CIfxDoc::SaveScrollBack(CString &strFileName)
{
	CObString osString(strFileName);
	UpdateAllViews(NULL, UPDATE_SAVE_SCROLLBACK, &osString);
}

void CIfxDoc::ReadScript()
{
	ASSERT(m_pSess != NULL);

	CString scriptFilename = m_pSess->GetOptions().sessionInfo_.ScriptFilename();
	if(scriptFilename.IsEmpty())
		return;

	CString strCommand;
	strCommand.Format("%cread {%s}", CGlobals::GetCommandCharacter(), scriptFilename);
	ExecuteCommand(strCommand);
}

/*
 * Helper function for finding the window pointer.
 * Returns window inside the current document.
 */
CWnd* CIfxDoc::ThisWnd() {
	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	return pView->GetParent();
}

/*
 * Broadcasts a message to a target window, if the target window is a local window,
 * a message will be dispatched to the attached view, otherwise if the target window
 * is another window, external message will be dispatched.
 */
void CIfxDoc::BroadCastToWindow(CString &window, WPARAM external_cmd, WPARAM local_cmd, const char *message) {
	CWnd *originator      = ThisWnd(); // Get current window
	CWnd *windowContainer = originator->GetParent();
    CWnd *target          = windowContainer->GetWindow(GW_CHILD);
	CString strMessage(message);
	target = windowContainer->FindWindowEx(windowContainer->GetSafeHwnd(), NULL, NULL, window);	
	if(target != NULL)	 
	{
		if (target != originator)
			if(external_cmd == ID_EXT_WINDOW_TEXT)
				target->SendMessage(WM_COMMAND, local_cmd, (LPARAM)message); // Send text to another window (Caught in ChildFrm.cpp)
			else
				if(strMessage.MakeLower() == "close")
					target->PostMessage(WM_CLOSE); //close the session window
				else
					target->SendMessage(WM_COMMAND, external_cmd, (LPARAM)message); //Send command to another window
				 
		else 			
			ReceiveBroadcast(local_cmd, message);                           // Execute command locally	
	}   
}

void CIfxDoc::ReceiveBroadcast(WPARAM wm_command, const char *message) {
	CObString osString(message);
	UpdateAllViews(NULL, wm_command, &osString);
}

// Sends a command to another window.
void CIfxDoc::SendCommandToAnotherWindow(CString &window, const char *command) {
	BroadCastToWindow(window, ID_EXT_WINDOW_COMMAND, UPDATE_EXECUTE_COMMAND, command);
}

// Sends a text message to be printed in another window.
void CIfxDoc::SendTextToAnotherWindow(CString &window, const char *text, bool dotriggers) {
	if(dotriggers)
		BroadCastToWindow(window, ID_EXT_WINDOW_TEXT, UPDATE_PRINT_INCOMING_CHAT, text);
	else
		BroadCastToWindow(window, ID_EXT_WINDOW_TEXT, UPDATE_PRINT_ANSI_NO_TRIGGER, text);

}

// Removes caption from the current window
void CIfxDoc::RemoveWindowCaption() 
{
	CWnd *this_window = ThisWnd();
	this_window->ModifyStyle(WS_CAPTION,0,SWP_FRAMECHANGED);
	this_window->SetFocus();
}

void CIfxDoc::AddWindowCaption() 
{
	CWnd *this_window = ThisWnd();
	this_window->ModifyStyle(0,WS_CAPTION,SWP_FRAMECHANGED);
	this_window->SetFocus();
}

void CIfxDoc::MaxRestoreSession()
{
	BOOL m_bChildMax;
	
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pChild=(CMDIChildWnd *)pFrame->MDIGetActive();
	
	LONG style;
	style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);
	m_bChildMax=(style & WS_MAXIMIZE)?TRUE:FALSE;

	if(pChild){
		if(m_bChildMax)
			pChild->MDIRestore();
		else pChild->MDIMaximize();
	}
}

HRESULT CIfxDoc::PrintSnoop(const char *pszLine)
{
	CObString osString(pszLine);
	UpdateAllViews(NULL, UPDATE_PRINT_SNOOP, &osString);
	return S_OK;
}

void CIfxDoc::PlayASound(CString& strFilename)
{
	CObString osString(strFilename);
	UpdateAllViews(NULL, UPDATE_PLAY_SOUND, &osString);
}

void CIfxDoc::UsePlaySound(BOOL bUsePlaySound)
{
	if(bUsePlaySound)
		UpdateAllViews(NULL, UPDATE_USE_PLAYSOUND, NULL);
	else
		UpdateAllViews(NULL, UPDATE_USE_DX, NULL);
}

void CIfxDoc::OpenLog(CString &strFileName, bool bAppend)
{
	CObString osString(strFileName);

	if(bAppend)
		UpdateAllViews(NULL, UPDATE_OPEN_LOG_APPEND, &osString);
	else
		UpdateAllViews(NULL, UPDATE_OPEN_LOG, &osString);
	
}

void CIfxDoc::OpenRawMudLog(CString &strFileName, bool bAppend)
{
	CObString osString(strFileName);

	if(bAppend)
	{
		UpdateAllViews(NULL, UPDATE_OPEN_LOG_APPEND, &osString);
	}
	else
	{
		UpdateAllViews(NULL, UPDATE_OPEN_RAWMUDLOG, &osString);
	}
}
void CIfxDoc::TaskbarNotification(CString &strMessage)
{
		CObString osString(strMessage);
		UpdateAllViews(NULL, UPDATE_TASKBARNOTIFICATION, &osString);
}

BOOL CIfxDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL bRead = m_pSess->Read(lpszPathName);
	UpdateAllViews(NULL, UPDATE_ALL_OPTIONS, NULL);
	return bRead;
}

BOOL CIfxDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return m_pSess->Save(lpszPathName);
}
void CIfxDoc::OnSaveSessionPosition()
{
	m_pSess->SaveSessionPosition();
}
