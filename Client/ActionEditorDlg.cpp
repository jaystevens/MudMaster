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
#include "ActionEditorDlg.h"
#include "Action.h"
#include "ActionList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	using namespace std;

	void CActionEditorDlg::SetAction( MMScript::CAction::PtrType &action )
	{
		_action = action;
	}

	const string CActionEditorDlg::GetAction() const
	{
		return static_cast<LPCTSTR>(_strAction);
	}

	const string CActionEditorDlg::GetTrigger() const
	{
		return static_cast<LPCTSTR>(_strTrigger);
	}

	const string CActionEditorDlg::GetGroup() const
	{
		return static_cast<LPCTSTR>(_strGroup);
	}

	bool CActionEditorDlg::IsEnabled() const
	{
		return _bDisabled ? false : true;
	}
	void CActionEditorDlg::SetEnabled( bool enabled )
	{
		_bDisabled = !enabled;
	}
	void CActionEditorDlg::SetAction( std::string  strAction )
	{
		_strAction = strAction.c_str() ;
	}
	void CActionEditorDlg::SetTrigger( std::string  strTrigger )
	{
		_strTrigger = strTrigger.c_str();
	}
	void CActionEditorDlg::SetGroup(std::string  strGroup)
	{
		_strGroup = strGroup.c_str();
	}


	CActionEditorDlg::CActionEditorDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CActionEditorDlg::IDD, pParent)
	{
		_strAction = _T("");
		_bDisabled = FALSE;
		_strGroup = _T("");
		_strTrigger = _T("");
	}


	void CActionEditorDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_ACTION, _strAction);
		DDX_Check(pDX, IDC_DISABLED, _bDisabled);
		DDX_Text(pDX, IDC_GROUP, _strGroup);
		DDX_Text(pDX, IDC_TRIGGER, _strTrigger);
	}


	BEGIN_MESSAGE_MAP(CActionEditorDlg, CDialog)
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CActionEditorDlg message handlers

	BOOL CActionEditorDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		SetupLayout();

		if(_action.get() == NULL)
			return TRUE;


		_strAction = _action->GetAction().c_str();
		_strGroup = _action->GetGroup().c_str();
		_strTrigger = _action->GetTrigger().c_str();
		_bDisabled = !_action->IsEnabled();

		UpdateData(FALSE);

		return TRUE;
	}

	void CActionEditorDlg::OnOK() 
	{
		UpdateData();

//TODO: KW should be trigger
//		if(_strAction.IsEmpty())
		if(_strTrigger.IsEmpty())
		{
			AfxMessageBox("Please enter the trigger text.");
			return;
		}

		if(_strAction.IsEmpty())
		{
			AfxMessageBox("Please enter the action to take.");
			return;
		}

		CDialog::OnOK();
	}

	void CActionEditorDlg::OnSize(UINT nType, int cx, int cy) 
	{
			
		ETSLayoutDialog::OnSize(nType, cx, cy);
UpdateLayout();

	}

	void CActionEditorDlg::SetupLayout(void)
	{
		
		CPane leftPane = pane(VERTICAL,GREEDY, nDefaultBorder, 10, 10)
			<< item(IDC_TRIGGER_STATIC, ABSOLUTE_VERT)
			<< item(IDC_TRIGGER, ABSOLUTE_VERT)
			<< item(IDC_ACTION_STATIC, ABSOLUTE_VERT)
			<< item(IDC_ACTION, ALIGN_FILL)
			<< item(IDC_GROUP_STATIC, ABSOLUTE_VERT)
			<< item(IDC_GROUP, ABSOLUTE_VERT)
			<< item(IDC_DISABLED, ABSOLUTE_VERT);
		

		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< item(IDCANCEL, NORESIZE);

		CreateRoot(HORIZONTAL)
			<< leftPane
			<< rightPane;

		UpdateLayout();
	}
}