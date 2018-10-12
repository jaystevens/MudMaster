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
// LogDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "LogDialog.h"
#include "NewWindowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogDialog dialog


CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{
	m_bLogging = FALSE;
	m_pView = NULL;
	//{{AFX_DATA_INIT(CLogDialog)
	m_bAppend = FALSE;
	m_bIncludeScrollback = FALSE;
	m_strLogFileName = _T("");
	//}}AFX_DATA_INIT
}


void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogDialog)
	DDX_Control(pDX, IDC_LOG_FILE_NAME, m_editFileName);
	DDX_Control(pDX, IDC_INCLUDE_SCROLLBACK, m_chkIncludeScrollBack);
	DDX_Control(pDX, IDC_CLOSE_LOG, m_btnClose);
	DDX_Control(pDX, IDC_BROWSE_LOGFILE, m_btnBrowse);
	DDX_Control(pDX, IDC_APPEND_LOG, m_chkAppend);
	DDX_Check(pDX, IDC_APPEND_LOG, m_bAppend);
	DDX_Check(pDX, IDC_INCLUDE_SCROLLBACK, m_bIncludeScrollback);
	DDX_Text(pDX, IDC_LOG_FILE_NAME, m_strLogFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	//{{AFX_MSG_MAP(CLogDialog)
	ON_BN_CLICKED(IDC_BROWSE_LOGFILE, OnBrowseLogfile)
	ON_BN_CLICKED(IDC_CLOSE_LOG, OnCloseLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogDialog message handlers

void CLogDialog::OnBrowseLogfile() 
{
	CTime timeNow;
	timeNow = CTime::GetCurrentTime();

	CString strLogFileName;
	strLogFileName.Format("log%d", timeNow);
	CFileDialog dlg(TRUE, "log", strLogFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Log Files (*.log)|*.log|All Files (*.*)|*.*||");

	if(dlg.DoModal() != IDOK)
		return;

	m_strLogFileName = dlg.GetPathName();
	UpdateData(FALSE);
}

void CLogDialog::OnCloseLog() 
{
	ASSERT(m_pView != NULL);

	m_strLogFileName = "";
	m_bLogging = FALSE;
	m_pView->CloseLog(TRUE);
	CDialog::OnCancel();
}

BOOL CLogDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_bLogging)
	{
		m_btnClose.EnableWindow(TRUE);
		m_btnBrowse.EnableWindow(FALSE);
		m_chkAppend.EnableWindow(FALSE);
		m_chkIncludeScrollBack.EnableWindow(FALSE);
		m_editFileName.SetReadOnly(TRUE);
	}
	else
	{
		m_btnClose.EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_strLogFileName.IsEmpty())
	{
		AfxMessageBox("You don't have a file name selected.\n", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CDialog::OnOK();
}
