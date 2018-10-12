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
// VarEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "VarEditDlg.h"
#include "Variable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

CVarEditDlg::CVarEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVarEditDlg::IDD, pParent)
{
	m_pVar = NULL;
	//{{AFX_DATA_INIT(CVarEditDlg)
	m_strGroup = _T("");
	m_strName = _T("");
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}


void CVarEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarEditDlg)
	DDX_Text(pDX, IDC_VAR_GROUP, m_strGroup);
	DDX_Text(pDX, IDC_VAR_NAME, m_strName);
	DDX_Text(pDX, IDC_VAR_VALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVarEditDlg, CDialog)
	//{{AFX_MSG_MAP(CVarEditDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarEditDlg message handlers

BOOL CVarEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pVar == NULL)
		return TRUE;

	m_strGroup = m_pVar->GetGroup().c_str();
	m_strName = m_pVar->GetName().c_str();
	m_strValue = m_pVar->GetValue().c_str();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVarEditDlg::OnOK() 
{
	UpdateData(TRUE);
	m_strName.TrimLeft();
	if(m_strName.IsEmpty())
	{
		AfxMessageBox("You must supply a name for a variable.");
		return;
	}
	
	m_strValue.TrimLeft();
	if(m_strValue.IsEmpty())
	{
		int nResult = AfxMessageBox("You haven't supplied a value for this variable.  Store without a value?", MB_YESNO | MB_ICONQUESTION);
		if(nResult == IDNO)
			return;
	}

	m_strGroup.TrimLeft();
	if(m_strGroup.IsEmpty())
	{
		int nResult = AfxMessageBox("You haven't supplied a group for this variable.  Store without a group?", MB_YESNO | MB_ICONQUESTION);
		if(nResult == IDNO)
			return;
	}

	CDialog::OnOK();
}
