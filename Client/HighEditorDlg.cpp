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
#include "High.h"
#include "ColorButton.h"
#include "HighEditorDlg.h"
#include "BackColors.h"
#include "ForeColorsDlg.h"
#include "DefaultOptions.h"
#include "Colors.h"
#include "BtnST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CHighEditorDlg::CHighEditorDlg(COptions &options, CWnd* pParent /*=NULL*/)
		: CDialog(CHighEditorDlg::IDD, pParent)
		, _options(options)
	{
		m_pHigh = NULL;
		m_nFore = 0;
		m_nBack = 0;

		//{{AFX_DATA_INIT(CHighEditorDlg)
		m_bDisabled = FALSE;
		m_strGroup = _T("");
		m_strMask = _T("");
		//}}AFX_DATA_INIT
	}


	void CHighEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CHighEditorDlg)
		DDX_Check(pDX, IDC_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_HIGH_GROUP, m_strGroup);
		DDX_Text(pDX, IDC_HIGH_MASK, m_strMask);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CHighEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CHighEditorDlg)
		ON_BN_CLICKED(IDC_BACK_COLOR, OnBackColor)
		ON_BN_CLICKED(IDC_FORE_COLOR, OnForeColor)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CHighEditorDlg message handlers

	void CHighEditorDlg::OnOK() 
	{
		UpdateData(TRUE);
		m_strMask.TrimLeft();
		m_strGroup.TrimLeft();
		if(m_strMask.IsEmpty())
		{
			AfxMessageBox("You must supply a mask for this highlight", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strGroup.IsEmpty())
		{
			int nResult = AfxMessageBox("You have not supplied a group name for this highlight, do you wish to continue?", MB_YESNO | MB_ICONQUESTION);
			if(nResult == IDNO)
				return;
		}

		CDialog::OnOK();
	}

	BOOL CHighEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		m_btnBackColor.SubclassDlgItem(IDC_BACK_COLOR, this);
		m_btnForeColor.SubclassDlgItem(IDC_FORE_COLOR, this);
		m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBack]);
		m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nFore]);

		//m_btnBackColor[0].SubclassDlgItem(IDC_BACK_COLOR, this);
		//m_btnForeColor[0].SubclassDlgItem(IDC_FORE_COLOR, this);

		//COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nBack];
		//m_btnBackColor[0].SetColor(CButtonST::BTNST_COLOR_BK_IN,cr);
		//cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nFore];
		//m_btnForeColor[0].SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);

		

		if(m_pHigh == NULL)
		{
			UpdateData(FALSE);
			return TRUE;
		}

		m_pHigh->ToMask(m_strMask);
		m_nBack = m_pHigh->back();
		m_nFore = m_pHigh->fore();
		m_strGroup=m_pHigh->Group();
		m_bDisabled = !m_pHigh->Enabled();
		m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBack]);
		m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nFore]);
		UpdateData(FALSE);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CHighEditorDlg::OnBackColor() 
	{
		CBackColorsDlg dlg(_options, this);

		dlg.m_nCurrent = m_nBack;

		dlg.DoModal();

		m_nBack = dlg.m_nCurrent;

		m_btnBackColor.SetColor(_options.terminalOptions_.TerminalWindowBackColors()[m_nBack]);	
	}

	void CHighEditorDlg::OnForeColor() 
	{
		CForeColorsDlg dlg(_options, this);

		dlg.m_nCurrent = m_nFore;

		dlg.DoModal();

		m_nFore = dlg.m_nCurrent;
		m_btnForeColor.SetColor(_options.terminalOptions_.TerminalWindowForeColors()[m_nFore]);


	}
}