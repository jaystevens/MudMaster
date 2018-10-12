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
#include "stdafx.h"
#include "ifx.h"
#include "PathOptionsDlg.h"
#include ".\pathoptionsdlg.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPathOptionsDlg::CPathOptionsDlg(bool isDefault, COptions &options) 
: CPropertyPage(CPathOptionsDlg::IDD)
, _options(options)
, _isDefault(isDefault)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	//{{AFX_DATA_INIT(CPathOptionsDlg)
	m_strDownloadPath = _T("");
	m_strLogPath = _T("");
	m_strHelpPath = _T("");
	m_strSoundPath = _T("");
	m_strUploadPath = _T("");
	//}}AFX_DATA_INIT
}

CPathOptionsDlg::~CPathOptionsDlg()
{
}

void CPathOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPathOptionsDlg)
	DDX_Text(pDX, IDC_DOWNLOAD_PATH, m_strDownloadPath);
	DDX_Text(pDX, IDC_LOG_PATH, m_strLogPath);
	DDX_Text(pDX, IDC_SESSION_PATH, m_strHelpPath);
	DDX_Text(pDX, IDC_SOUND_PATH, m_strSoundPath);
	DDX_Text(pDX, IDC_UPLOAD_PATH, m_strUploadPath);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_HELP_LABEL, _helpLabel);
	DDX_Control(pDX, IDC_HELP_DESC, _helpDesc);
	DDX_Control(pDX, IDC_PICK_SESSION, _helpBrowse);
	DDX_Control(pDX, IDC_SESSION_PATH, _helpEdit);
}


BEGIN_MESSAGE_MAP(CPathOptionsDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CPathOptionsDlg)
	ON_BN_CLICKED(IDC_PICK_DOWNLOAD, OnPickDownload)
	ON_BN_CLICKED(IDC_PICK_LOG, OnPickLog)
	ON_BN_CLICKED(IDC_PICK_SESSION, OnPickSession)
	ON_BN_CLICKED(IDC_PICK_SOUND, OnPickSound)
	ON_BN_CLICKED(IDC_PICK_UPLOAD, OnPickUpload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathOptionsDlg message handlers

void CPathOptionsDlg::OnPickDownload() 
{
	char pszPath[MAX_PATH];
	ZeroMemory(pszPath, MAX_PATH);

	BOOL bRet = PickDirectory(pszPath, IDS_DOWNLOAD_TITLE);
	if(!bRet)
		return;

	m_strDownloadPath = pszPath;

	UpdateData(FALSE);
}

BOOL CPathOptionsDlg::PickDirectory(char *pszPath, int nID)
{
	ASSERT(pszPath != NULL);

	CString strDialogTitle;
	if(!strDialogTitle.LoadString(nID))
		return FALSE;

	BROWSEINFO bi;
	LPITEMIDLIST ilist;
	char displayName[MAX_PATH];
	ZeroMemory(displayName, MAX_PATH);

	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = displayName;
	bi.lpszTitle = strDialogTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_VALIDATE;
	bi.lpfn = NULL;
	bi.lParam = 0;

	ilist = SHBrowseForFolder(&bi);

	if(ilist == NULL)
		return FALSE;

	BOOL bRet = SHGetPathFromIDList(ilist, pszPath);
	LPMALLOC pMalloc;
	HRESULT hRes = SHGetMalloc(&pMalloc);
	if(hRes != NO_ERROR)
	{
		AfxMessageBox("Error in SHGetMalloc!");
		return FALSE;
	}
	pMalloc->Free(ilist);
	return bRet;
}

void CPathOptionsDlg::OnPickLog() 
{
	char pszPath[MAX_PATH];
	ZeroMemory(pszPath, MAX_PATH);

	BOOL bRet = PickDirectory(pszPath, IDS_LOG_TITLE);
	if(!bRet)
		return;

	m_strLogPath = pszPath;

	UpdateData(FALSE);
	
}

void CPathOptionsDlg::OnPickSession() 
{
	char pszPath[MAX_PATH];
	ZeroMemory(pszPath, MAX_PATH);

	BOOL bRet = PickDirectory(pszPath, IDS_SESSION_TITLE);
	if(!bRet)
		return;

	m_strHelpPath = pszPath;

	UpdateData(FALSE);
	
}

void CPathOptionsDlg::OnPickSound() 
{
	char pszPath[MAX_PATH];
	ZeroMemory(pszPath, MAX_PATH);

	BOOL bRet = PickDirectory(pszPath, IDS_SOUND_TITLE);
	if(!bRet)
		return;

	m_strSoundPath = pszPath;

	UpdateData(FALSE);
	
}

void CPathOptionsDlg::OnPickUpload() 
{
	char pszPath[MAX_PATH];
	ZeroMemory(pszPath, MAX_PATH);

	BOOL bRet = PickDirectory(pszPath, IDS_UPLOAD_TITLE);
	if(!bRet)
		return;

	m_strUploadPath = pszPath;

	UpdateData(FALSE);
	
}

BOOL CPathOptionsDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	std::string path;
	_options.pathOptions_.Get_DownloadPath(path);
	m_strDownloadPath	= path.c_str();

	m_strLogPath		= _options.pathOptions_.LogPath();

	_helpLabel.ShowWindow(SW_HIDE);
	_helpDesc.ShowWindow(SW_HIDE);
	_helpBrowse.ShowWindow(SW_HIDE);
	_helpEdit.ShowWindow(SW_HIDE);

	_options.pathOptions_.Get_SoundPath(path);
	m_strSoundPath = path.c_str();

	_options.pathOptions_.Get_UploadPath(path);
	m_strUploadPath	= path.c_str();

	UpdateData(FALSE);
	return TRUE;
}

void CPathOptionsDlg::OnOK()
{
	UpdateData(TRUE);

	_options.pathOptions_.DownloadPath(m_strDownloadPath);
	_options.pathOptions_.LogPath(m_strLogPath);
	_options.pathOptions_.Put_SoundPath(m_strSoundPath);
	_options.pathOptions_.UploadPath(m_strUploadPath);

	CPropertyPage::OnOK();
}
