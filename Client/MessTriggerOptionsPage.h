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

class CMessTriggerOptionsPage 
	: public CPropertyPage
{
public:
	CMessTriggerOptionsPage(COptions &options);
	~CMessTriggerOptionsPage();

// Dialog Data
	//{{AFX_DATA(CMessTriggerOptionsPage)
	enum { IDD = IDD_MESSAGE_OPTIONS };
	CButton	m_chkSub;
	CButton	m_chkHigh;
	CButton	m_chkGag;
	CButton	m_chkAll;
	CButton	m_chkAction;
	BOOL	m_bAction;
	BOOL	m_bAlias;
	BOOL	m_bArray;
	BOOL	m_bAuto;
	BOOL	m_bEnable;
	BOOL	m_bEvent;
	BOOL	m_bGag;
	BOOL	m_bHigh;
	BOOL	m_bIgnoreAction;
	BOOL	m_bIgnoreAll;
	BOOL	m_bIgnoreGag;
	BOOL	m_bIgnoreHigh;
	BOOL	m_bIgnoreSub;
	BOOL	m_bList;
	BOOL	m_bListItem;
	BOOL	m_bMacro;
	BOOL	m_bStatusBarItem;
	BOOL	m_bSub;
	BOOL	m_bTab;
	BOOL	m_bVar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMessTriggerOptionsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMessTriggerOptionsPage)
	afx_msg void OnIgnoreAll();
	afx_msg void OnIgnoreActions();
	afx_msg void OnIgnoreGags();
	afx_msg void OnIgnoreHighs();
	afx_msg void OnIgnoreSubs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COptions &_options;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
