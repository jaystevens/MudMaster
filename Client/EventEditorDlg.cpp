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
#include "EventEditorDlg.h"
#include "Event.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CEventEditorDlg::CEventEditorDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CEventEditorDlg::IDD, pParent)
	{
		m_pEvent = NULL;
		//{{AFX_DATA_INIT(CEventEditorDlg)
		m_strAction = _T("");
		m_bDisabled = FALSE;
		m_strGroup = _T("");
		m_strName = _T("");
		m_nSeconds = 0;
		//}}AFX_DATA_INIT
	}


	void CEventEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CEventEditorDlg)
		DDX_Text(pDX, IDC_EVENT_ACTION, m_strAction);
		DDX_Check(pDX, IDC_EVENT_DISABLED, m_bDisabled);
		DDX_Text(pDX, IDC_EVENT_GROUP, m_strGroup);
		DDX_Text(pDX, IDC_EVENT_NAME, m_strName);
		DDX_Text(pDX, IDC_EVENT_SECONDS, m_nSeconds);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CEventEditorDlg, CDialog)
		//{{AFX_MSG_MAP(CEventEditorDlg)
		//}}AFX_MSG_MAP
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CEventEditorDlg message handlers

	BOOL CEventEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		SetupLayout();
		if(m_pEvent == NULL)
			return TRUE;

		

		m_bDisabled = !m_pEvent->Enabled();
		m_nSeconds = m_pEvent->Seconds();
		m_strAction = m_pEvent->Event();
		m_strGroup = m_pEvent->Group();
		m_strName = m_pEvent->Name();
		UpdateData(FALSE);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CEventEditorDlg::OnOK() 
	{
		UpdateData(TRUE);
		m_strAction.TrimLeft();
		m_strName.TrimLeft();
		m_strGroup.TrimLeft();

		if(m_nSeconds < 1)
		{
			AfxMessageBox("Seconds must be greater than zero", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strAction.IsEmpty())
		{
			AfxMessageBox("You need to supply an action for this event", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strName.IsEmpty())
		{
			AfxMessageBox("You need to supply a name for this event", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if(m_strGroup.IsEmpty())
		{
			int nResult = AfxMessageBox("You have not supplied a group name for this event, do you wish to continue?", MB_YESNO | MB_ICONQUESTION);
			if(nResult == IDNO)
				return;
		}

		CDialog::OnOK();
	}

	void CEventEditorDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE);

		CPane bottomPane = pane(HORIZONTAL)
			<< item(IDC_EVENT_DISABLED, ABSOLUTE_VERT)
			<< item(IDC_SECONDS_LABEL, ABSOLUTE_VERT)
			<< item(IDC_EVENT_SECONDS, ABSOLUTE_VERT);

		CPane leftPane = pane(VERTICAL)
			<< item(IDC_NAME_LABEL, ABSOLUTE_VERT)
			<< item(IDC_EVENT_NAME, ABSOLUTE_VERT)
			<< item(IDC_ACTION_LABEL, ABSOLUTE_VERT)
			<< item(IDC_EVENT_ACTION, ALIGN_FILL)
			<< item(IDC_GROUP_LABEL, ABSOLUTE_VERT)
			<< item(IDC_EVENT_GROUP, ABSOLUTE_VERT)
			<< bottomPane;

		CreateRoot(HORIZONTAL)
			<< leftPane
			<< rightPane;

		UpdateLayout();
	}

	void CEventEditorDlg::OnSize(UINT nType, int cx, int cy)
	{
		ETSLayoutDialog::OnSize(nType, cx, cy);

		UpdateLayout();
	}
}