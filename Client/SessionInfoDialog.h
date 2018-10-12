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

class COptions;

class CSessionInfoDialog 
	: public CPropertyPage
{
	COptions &_options;
public:
	CSessionInfoDialog(COptions &options);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSessionInfoDialog)
	enum { IDD = IDD_SESSION_INFO };
	BOOL	m_bChatServer;
	BOOL	m_bEvents;
	BOOL	m_bIgnoreAliases;
	BOOL	m_bMaximize;
	BOOL	m_bMaxWidth;
	BOOL	m_bMaxHeight;
	BOOL	m_bDockRight;
	CString m_strMaximize;
	BOOL	m_bPreSub;
	BOOL	m_bPromptOverwrite;
	CString	m_strAddress;
	CString	m_strName;
	BOOL	m_bSpeedWalk;
	BOOL	m_bStatusBar;
	CString	m_strSessionFile;
	BOOL	m_bAutoLoadScript;
	BOOL	m_bAppendLog;
	BOOL	m_bAutoLog;
	CString	m_strLogFileName;
	int		m_nPace;
	UINT	m_nPort;
	short	m_nSubDepth;
	BOOL	m_bShowInfo;
	CString	m_strCommandChar;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSessionInfoDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSessionInfoDialog)
	afx_msg void OnNotes();
	afx_msg void OnScriptBrowse();
	afx_msg void OnLogfileBrowse();
	afx_msg void OnMaxChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
