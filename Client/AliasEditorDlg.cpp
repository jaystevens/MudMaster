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
#include "AliasEditorDlg.h"
#include "Alias.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CAliasEditorDlg::CAliasEditorDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CAliasEditorDlg::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CAliasEditorDlg)
		m_strAction = _T("");
		m_bDisabled = FALSE;
		m_strGroup = _T("");
		m_strText = _T("");
		//}}AFX_DATA_INIT
	}


	void CAliasEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CAliasEditorDlg)
		DDX_Text(pDX, IDC_ALIAS_ACTION, m_strAction);
		DDX_Check(pDX, IDC_ALIAS_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_ALIAS_GROUP, m_strGroup);
		DDX_Text(pDX, IDC_ALIAS_TEXT, m_strText);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CAliasEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CAliasEditorDlg)
		//}}AFX_MSG_MAP
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CAliasEditorDlg message handlers

	void CAliasEditorDlg::OnOK() 
	{
		// TODO: Add extra validation here

		UpdateData(TRUE);
		m_strText.TrimLeft();
		if(m_strText.IsEmpty())
		{
			AfxMessageBox("You must supply an alias command.");
			return;
		}

		m_strAction.TrimLeft();
		if(m_strAction.IsEmpty())
		{
			AfxMessageBox("You must supply an action for this command.");
			return;
		}

		if(m_strGroup.IsEmpty())
		{
			int nResult = AfxMessageBox("You don't have a group specified for this alias, are you sure?", MB_YESNO | MB_ICONQUESTION);
			if(nResult == IDNO)
			{
				return;
			}
		}

		CDialog::OnOK();
	}

	BOOL CAliasEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		SetupLayout();

		if(_alias.get() == NULL)
			return TRUE;


		m_strText = _alias->DisplayName();
		m_strAction = _alias->action();
		m_strGroup = _alias->group().c_str();
		m_bDisabled = !_alias->enabled();

		UpdateData(FALSE);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CAliasEditorDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE);

		CPane leftPane = pane(VERTICAL,GREEDY, nDefaultBorder, 10, 10)
			<< item(IDC_ALIAS_TEXT_LABEL, ABSOLUTE_VERT)
			<< item(IDC_ALIAS_TEXT, ABSOLUTE_VERT)
			<< item(IDC_ACTION_LABEL, ABSOLUTE_VERT)
			<< item(IDC_ALIAS_ACTION, ALIGN_FILL)
			<< item(IDC_GROUP_LABEL, ABSOLUTE_VERT)
			<< item(IDC_ALIAS_GROUP, ABSOLUTE_VERT)
			<< item(IDC_ALIAS_DISABLED, ABSOLUTE_VERT);

		CreateRoot(HORIZONTAL)
			<< leftPane
			<< rightPane;
UpdateLayout();
		
	}

	void CAliasEditorDlg::OnSize(UINT nType, int cx, int cy)
	{
		ETSLayoutDialog::OnSize(nType, cx, cy);
		UpdateLayout();

	}


	void CAliasEditorDlg::SetIsDisabled( bool disabled )
	{
		m_bDisabled = disabled;
	}
	void CAliasEditorDlg::SetAction( CString  strAction )
	{
		m_strAction = strAction ;
	}
	void CAliasEditorDlg::SetName( CString  strName )
	{
		m_strText = strName;
	}
	void CAliasEditorDlg::SetGroup(CString  strGroup)
	{
		m_strGroup = strGroup;
	}
	const CString CAliasEditorDlg::GetAction() const
	{
		return static_cast<LPCTSTR>(m_strAction);
	}

	const CString CAliasEditorDlg::GetName() const
	{
		return static_cast<LPCTSTR>(m_strText);
	}

	const CString CAliasEditorDlg::GetGroup() const
	{
		return static_cast<LPCTSTR>(m_strGroup);
	}

	bool CAliasEditorDlg::IsEnabled() const
	{
		return m_bDisabled ? false : true;
	}

}