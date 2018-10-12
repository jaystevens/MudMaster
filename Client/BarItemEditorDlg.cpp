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
#include "BarItemEditorDlg.h"
#include "BarItem.h"
#include "ForeColorsDlg.h"
#include "BackColors.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;
namespace EditorDialogs
{
	CBarItemEditorDlg::CBarItemEditorDlg(COptions &options, CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CBarItemEditorDlg::IDD, pParent)
		, _options(options)
	{
		m_pItem = NULL;
		m_nForeColor = 0;
		m_nBackColor = 0;
		//{{AFX_DATA_INIT(CBarItemEditorDlg)
		m_bDisabled = FALSE;
		m_strGroup = _T("");
		m_strName = _T("");
		m_bSeparator = FALSE;
		m_strText = _T("");
		m_nLength = 0;
		m_nPosition = 1;
		//}}AFX_DATA_INIT
	}


	void CBarItemEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CBarItemEditorDlg)
		DDX_Control(pDX, IDC_SEPARATOR, m_btnSeparator);
		DDX_Control(pDX, IDC_DISABLED, m_btnDisabled);
		DDX_Control(pDX, IDC_TEXT, m_editText);
		DDX_Control(pDX, IDC_POSITION, m_editPosition);
		DDX_Control(pDX, IDC_NAME, m_editName);
		DDX_Control(pDX, IDC_LENGTH, m_editLength);
		DDX_Control(pDX, IDC_GROUP, m_editGroup);
		DDX_Check(pDX, IDC_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_GROUP, m_strGroup);
		DDX_Text(pDX, IDC_NAME, m_strName);
		DDX_Check(pDX, IDC_SEPARATOR, m_bSeparator);
		DDX_Text(pDX, IDC_TEXT, m_strText);
		DDX_Text(pDX, IDC_LENGTH, m_nLength);
		DDX_Text(pDX, IDC_POSITION, m_nPosition);
		DDV_MinMaxInt(pDX, m_nPosition, 1, 480);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CBarItemEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CBarItemEditorDlg)
		ON_BN_CLICKED(IDC_BAR_FORECOLOR, OnBarForecolor)
		ON_BN_CLICKED(IDC_BAR_BACKCOLOR, OnBarBackcolor)
		ON_BN_CLICKED(IDC_DISABLED, OnDisabled)
		ON_BN_CLICKED(IDC_SEPARATOR, OnSeparator)
		//}}AFX_MSG_MAP
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CBarItemEditorDlg message handlers

	BOOL CBarItemEditorDlg::OnInitDialog() 
	{
		ASSERT(m_nForeColor < MAX_FORE_COLORS);
		ASSERT(m_nBackColor < MAX_BACK_COLORS);

		CDialog::OnInitDialog();

		SetupLayout();

		m_btnBackColor.SubclassDlgItem(IDC_BAR_BACKCOLOR, this);
		m_btnForeColor.SubclassDlgItem(IDC_BAR_FORECOLOR, this);
		m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBackColor]);
		m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nForeColor]);

		if(m_pItem == NULL)
			return TRUE;

		if(m_pItem->Separator())
		{
			m_bSeparator = TRUE;
			m_editLength.EnableWindow(FALSE);
			m_editName.EnableWindow(FALSE);
			m_editText.EnableWindow(FALSE);
			m_btnBackColor.EnableWindow(FALSE);
			m_btnForeColor.EnableWindow(FALSE);
			UpdateData(FALSE);
			return TRUE;
		}
		else
		{
			m_nBackColor = m_pItem->BackColor();
			m_nForeColor = m_pItem->ForeColor();
			m_nLength = m_pItem->Length();
			m_nPosition = m_pItem->Position();
			m_strGroup = m_pItem->Group();
			m_strName = m_pItem->Item();
			m_strText = m_pItem->Text();
			m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBackColor]);
			m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nForeColor]);
			UpdateData(FALSE);
		}

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CBarItemEditorDlg::OnBarForecolor() 
	{
		CForeColorsDlg dlg(_options, this);

		dlg.m_nCurrent = m_nForeColor;

		if(dlg.DoModal()!=IDOK)
			return;

		m_nForeColor = dlg.m_nCurrent;

		m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nForeColor]);
	}

	void CBarItemEditorDlg::OnBarBackcolor() 
	{
		CBackColorsDlg dlg(_options, this);

		dlg.m_nCurrent = m_nBackColor;

		if(dlg.DoModal()!=IDOK)
			return;

		m_nBackColor = dlg.m_nCurrent;

		m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBackColor]);	
	}

	void CBarItemEditorDlg::OnDisabled() 
	{

	}

	void CBarItemEditorDlg::OnSeparator() 
	{
		int btnState = m_btnSeparator.GetCheck();
		if(btnState == BST_CHECKED)
		{
			m_editLength.EnableWindow(FALSE);
			m_editName.EnableWindow(FALSE);
			m_editText.EnableWindow(FALSE);
			m_btnBackColor.EnableWindow(FALSE);
			m_btnForeColor.EnableWindow(FALSE);
		}
		else
		{
			m_editLength.EnableWindow(TRUE);
			m_editName.EnableWindow(TRUE);
			m_editText.EnableWindow(TRUE);
			m_btnBackColor.EnableWindow(TRUE);
			m_btnForeColor.EnableWindow(TRUE);
		}

	}

	void CBarItemEditorDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE);

		CPane posPane = pane(HORIZONTAL)
			<< item(IDC_POSITION_LABEL, NORESIZE)
			<< item(IDC_POSITION, ABSOLUTE_VERT)
			<< item(IDC_LENGTH_LABEL, NORESIZE)
			<< item(IDC_LENGTH, ABSOLUTE_VERT);

		CPane colorPane = pane(HORIZONTAL)
			<< item(IDC_BAR_FORECOLOR, NORESIZE)
			<< item(IDC_FORE_LABEL, ABSOLUTE_VERT)
			<< item(IDC_BAR_BACKCOLOR, NORESIZE)
			<< item(IDC_BACK_LABEL, ABSOLUTE_VERT);

		CPane leftPane = pane(VERTICAL)
			<< item(IDC_SEPARATOR, ABSOLUTE_VERT)
			<< item(IDC_NAME_LABEL, ABSOLUTE_VERT)
			<< item(IDC_NAME, ABSOLUTE_VERT)
			<< item(IDC_TEXT_LABEL, ABSOLUTE_VERT)
			<< item(IDC_TEXT, ABSOLUTE_VERT)
			<< posPane
			<< colorPane
			<< item(IDC_GROUP_LABEL, ABSOLUTE_VERT)
			<< item(IDC_GROUP, ABSOLUTE_VERT)
			<< item(IDC_DISABLED, ABSOLUTE_VERT);

		CreateRoot(HORIZONTAL)
			<< leftPane
			<< rightPane;

		UpdateLayout();
	}

	void CBarItemEditorDlg::OnSize(UINT nType, int cx, int cy)
	{
		ETSLayoutDialog::OnSize(nType, cx, cy);

		UpdateLayout();
	}
}