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
#include "GagEditorDlg.h"
#include "Gag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CGagEditorDlg::CGagEditorDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CGagEditorDlg::IDD, pParent)
	{
		m_pGag = NULL;
		//{{AFX_DATA_INIT(CGagEditorDlg)
		m_bDisabled = FALSE;
		m_strGroup = _T("");
		m_strMask = _T("");
		//}}AFX_DATA_INIT
	}


	void CGagEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CGagEditorDlg)
		DDX_Check(pDX, IDC_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_GAG_GROUP, m_strGroup);
		DDX_Text(pDX, IDC_GAG_MASK, m_strMask);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CGagEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CGagEditorDlg)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CGagEditorDlg message handlers

	void CGagEditorDlg::OnOK() 
	{
		UpdateData(TRUE);
		m_strGroup.TrimLeft();
		m_strMask.TrimLeft();

		if(m_strMask.IsEmpty())
		{
			AfxMessageBox("You must supply a mask", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strGroup.IsEmpty())
		{
			int nResult = AfxMessageBox("You have not supplied a group name for this gag, do you wish to continue?", MB_YESNO | MB_ICONQUESTION);
			if(nResult == IDNO)
				return;
		}

		CDialog::OnOK();
	}

	BOOL CGagEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		if(m_pGag == NULL)
			return TRUE;

		m_bDisabled = !m_pGag->Enabled();
		m_strGroup = m_pGag->Group();
		m_strMask = m_pGag->GetTrigger();
		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
}