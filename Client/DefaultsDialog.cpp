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
// DefaultsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "DefaultsDialog.h"
#include ".\defaultsdialog.h"
#include "DefaultOptions.h"
#include "OptionsSheet.h"
#include "ConfigFileConstants.h"
#include "FileUtils.h"
#include "Globals.h"

// CDefaultsDialog dialog

IMPLEMENT_DYNAMIC(CDefaultsDialog, CDialog)
CDefaultsDialog::CDefaultsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDefaultsDialog::IDD, pParent)
	, _defaultSessionFile(_T(""))
	, bMinimiseToTray(FALSE)
{
}

CDefaultsDialog::~CDefaultsDialog()
{
}

void CDefaultsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DEFAULT_SESSION_PATH, _defaultSessionFile);
	DDX_Check(pDX, IDC_CHECK1, bMinimiseToTray);
	DDX_Control(pDX, IDC_DEFAULT_SESSION_PATH, _sessionPathEdito);
}


BEGIN_MESSAGE_MAP(CDefaultsDialog, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_DEFAULT, OnBnClickedBrowseDefault)
	ON_BN_CLICKED(IDC_EDIT_DEFAULTS, OnBnClickedEditDefaults)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()


// CDefaultsDialog message handlers

void CDefaultsDialog::OnBnClickedBrowseDefault()
{
	CFileDialog dlg(
		TRUE, 
		"mm", 
		((CIfxApp*)AfxGetApp())->GetDefaultSessionFileName(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"Session Files (*.mms)|*.mms|All Files (*.*)|*.*||");	

	if(dlg.DoModal() != IDOK)
		return;

	_defaultSessionFile = dlg.GetPathName();

	UpdateData(FALSE);
}

BOOL CDefaultsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	_defaultSessionFile = ((CIfxApp*)AfxGetApp())->GetDefaultSessionFileName();
	if(CGlobals::GetMinimiseToTray() == "hide")
		bMinimiseToTray = TRUE;
	else
		bMinimiseToTray = FALSE;
	UpdateData(FALSE);

	return TRUE; 
}

void CDefaultsDialog::OnOK()
{
	UpdateData(TRUE);

	((CIfxApp*)AfxGetApp())->SetDefaultSessionFileName(_defaultSessionFile);

	CString dir;

	dir = MMFileUtils::fixPath(INI_FILE_NAME);

	::WritePrivateProfileString(
			DEFAULT_SECTION_NAME, 
			DEFAULT_SESSION_KEY, 
			_defaultSessionFile,
			dir );
	if(bMinimiseToTray)
	{
			::WritePrivateProfileString(
			_T("Window"), 
			_T("MinimiseToTray"), 
			_T("hide"),
			dir );
			AfxMessageBox("Minimise to tray requires you rerun MudMaster");
			//CGlobals::SetMinimiseToTray(_T("hide"));
	}
	else
	{		::WritePrivateProfileString(
			_T("Window"), 
			_T("MinimiseToTray"), 
			_T("show"),
			dir );
			CGlobals::SetMinimiseToTray(_T("show"));
	}


	CDialog::OnOK();
}

void CDefaultsDialog::OnBnClickedEditDefaults()
{
	COptions options;
	options.LoadDefaults();

	COptionsSheet sheet(true, options, IDS_EDIT_DEFAULTS);
	if(IDOK  == sheet.DoModal())
	{
		options.SaveDefaults();
	}
}

void CDefaultsDialog::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}
