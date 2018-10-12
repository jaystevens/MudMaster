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
#include "Macro.h"
#include "MacroEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CMacroEditorDlg::CMacroEditorDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CMacroEditorDlg::IDD, pParent)
	{
		m_pMacro = NULL;
		//{{AFX_DATA_INIT(CMacroEditorDlg)
		m_strAction = _T("");
		m_strGroup = _T("");
		m_bDisabled = FALSE;
		m_strKey = _T("");
		m_nDest = -1;
		//}}AFX_DATA_INIT
	}


	void CMacroEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CMacroEditorDlg)
		DDX_Text(pDX, IDC_ACTION, m_strAction);
		DDX_Text(pDX, IDC_GROUP, m_strGroup);
		DDX_Check(pDX, IDC_MACRO_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_MACRO_KEY, m_strKey);
		DDX_Radio(pDX, IDC_SEND_TO_MUD, m_nDest);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CMacroEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CMacroEditorDlg)
		//}}AFX_MSG_MAP
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CMacroEditorDlg message handlers

	BOOL CMacroEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		SetupLayout();
		if(m_pMacro == NULL)
		{
			m_nDest = 0;
			UpdateData(FALSE);
			return TRUE;
		}

		

		m_strAction = m_pMacro->Action();
		m_strGroup = m_pMacro->Group();
		m_pMacro->KeyToName(m_strKey);
		m_nDest = m_pMacro->Dest();
		UpdateData(FALSE);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CMacroEditorDlg::OnOK() 
	{
		UpdateData(TRUE);

		m_strKey.TrimLeft();
		m_strAction.TrimLeft();
		m_strGroup.TrimLeft();

		if(m_strKey.IsEmpty())
		{
			AfxMessageBox("You have not supplied a key for this macro.", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strAction.IsEmpty())
		{
			AfxMessageBox("You have not supplied an action for this macro.", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strGroup.IsEmpty())
		{
			int nResult = AfxMessageBox("You have not supplied a group for this macro, do you wish to continue?", MB_YESNO | MB_ICONQUESTION);
			if(nResult == IDNO)
				return;
		}

		CDialog::OnOK();
	}

	void CMacroEditorDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE);

		CPane destGroup = paneCtrl(IDC_DEST_GROUP, HORIZONTAL, GREEDY, nDefaultBorder, 10, 10)
			<< item(IDC_SEND_TO_MUD, ABSOLUTE_VERT)
			<< item(IDC_INPUT_CONTROL, ABSOLUTE_VERT)
			<< item(IDC_INPUT_CONTROLCR, ABSOLUTE_VERT);

		CPane leftPane = pane(VERTICAL,GREEDY, nDefaultBorder, 10, 10)
			<< item(IDC_KEY_LABEL, ABSOLUTE_VERT)
			<< item(IDC_MACRO_KEY, ABSOLUTE_VERT)
			<< destGroup
			<< item(IDC_ACTION_LABEL, ABSOLUTE_VERT)
			<< item(IDC_ACTION, ALIGN_FILL)
			<< item(IDC_GROUP_LABEL, ABSOLUTE_VERT)
			<< item(IDC_GROUP, ABSOLUTE_VERT)
			<< item(IDC_MACRO_DISABLED, ABSOLUTE_VERT);

		CreateRoot(HORIZONTAL)
			<< leftPane
			<< rightPane;

		UpdateLayout();
	}

	void CMacroEditorDlg::OnSize(UINT nType, int cx, int cy)
	{
		ETSLayoutDialog::OnSize(nType, cx, cy);
		UpdateLayout();
	}
}