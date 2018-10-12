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
#include "Sub.h"
#include "SubEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSubEditorDlg::CSubEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubEditorDlg::IDD, pParent)
{
	m_pSub = NULL;
	//{{AFX_DATA_INIT(CSubEditorDlg)
	m_bDisabled = FALSE;
	m_strGroup = _T("");
	m_strMask = _T("");
	m_strSub = _T("");
	//}}AFX_DATA_INIT
}


void CSubEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubEditorDlg)
	DDX_Check(pDX, IDC_SUB_DISABLED, m_bDisabled);
	DDX_Text(pDX, IDC_SUB_GROUP, m_strGroup);
	DDX_Text(pDX, IDC_SUB_MASK, m_strMask);
	DDX_Text(pDX, IDC_SUB_SUB, m_strSub);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CSubEditorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubEditorDlg message handlers

BOOL CSubEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pSub == NULL)
		return TRUE;

	m_strGroup = m_pSub->Group();
	m_strMask = m_pSub->GetTrigger();
	m_strSub = m_pSub->Sub();
	m_bDisabled = !m_pSub->Enabled();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubEditorDlg::OnOK() 
{
	UpdateData(TRUE);
	
	m_strMask.TrimLeft();
	m_strSub.TrimLeft();
	m_strGroup.TrimLeft();

	if(m_strMask.IsEmpty())
	{
		AfxMessageBox("You must supply a mask for this substitution", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if(m_strSub.IsEmpty())
	{
		AfxMessageBox("You must supply a substitution for this sub", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if(m_strGroup.IsEmpty())
	{
		int nResult = AfxMessageBox("You have not supplied a group for this substitition, do you wish to continue?", MB_YESNO | MB_ICONQUESTION);
		if(nResult == IDNO)
			return;
	}

	CDialog::OnOK();
}
