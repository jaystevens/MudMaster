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
#include "afxwin.h"

class COptions;

class CPathOptionsDlg : public CPropertyPage
{
	COptions &_options;
public:
	CPathOptionsDlg(bool isDefault, COptions &options);
	~CPathOptionsDlg();

// Dialog Data
	//{{AFX_DATA(CPathOptionsDlg)
	enum { IDD = IDD_PATH_OPTIONS };
	CString	m_strDownloadPath;
	CString	m_strLogPath;
	CString	m_strHelpPath;
	CString	m_strSoundPath;
	CString	m_strUploadPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPathOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPathOptionsDlg)
	afx_msg void OnPickDownload();
	afx_msg void OnPickLog();
	afx_msg void OnPickSession();
	afx_msg void OnPickSound();
	afx_msg void OnPickUpload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool _isDefault;

private:
	BOOL PickDirectory(char *pszPath, int nId);
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CStatic _helpLabel;
	CStatic _helpDesc;
	CButton _helpBrowse;
	CEdit _helpEdit;
};
