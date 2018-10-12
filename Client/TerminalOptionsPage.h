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

#include "ColorButton.h"
#include "Colors.h"
#include "BtnSt.h"

class COptions;

class CTerminalOptionsPage 
	: public CPropertyPage
{
	COptions &_options;
public:
	CTerminalOptionsPage(COptions &options);
	~CTerminalOptionsPage();

	CButtonST m_btnTermFore;
	CButtonST m_btnTermBack;
	CButtonST m_btnInputBackColor;
	CButtonST m_btnHighFore;
	CButtonST m_btnHighBack;

	BYTE m_term_fore;
	BYTE m_term_back;

	COLORREF m_crInputBack;
	COLORREF m_crHighFore;
	COLORREF m_crHighBack;

	LOGFONT m_lfTerm;
	CHARFORMAT m_cfInputTemp;

	BYTE m_nMessBack;
	BYTE m_nMessFore;
	BYTE m_nStatBack;
	BYTE m_nStatFore;

	CButtonST m_btnMessBack;
	CButtonST m_btnMessFore;
	CButtonST m_btnStatBack;
	CButtonST m_btnStatFore;

	LOGFONT m_lfStatusBar;

// Dialog Data
	//{{AFX_DATA(CTerminalOptionsPage)
	enum { IDD = IDD_TERMINAL_OPTIONS };
	CSpinButtonCtrl	m_spinCommandHistory;
	CSpinButtonCtrl	m_spinScrollBack;
	BOOL	m_bLocalEcho;
	BOOL	m_bTelnetGA;
	BOOL	m_bZmudRetain;
	BOOL	m_bShowBorder;
	int		m_nSBBSize;
	UINT	m_nCommHistBuffSize;
	UINT	m_nLineLength;
	BOOL	m_bFullScreen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTerminalOptionsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTerminalOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTermForecolor();
	afx_msg void OnTermBackcolor();
	afx_msg void OnInputBackcolor();
	afx_msg void OnTermFont();
	afx_msg void OnInputFont();
	afx_msg void OnHighlight();
	afx_msg void OnHighlighttext();
	afx_msg void OnStatForecolor();
	afx_msg void OnStatBackcolor();
	afx_msg void OnMessageForecolor();
	afx_msg void OnMessageBackcolor();
	afx_msg void OnStatFont();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bTimestampLog;
};
