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
// FirstTimeSetupWizardChatPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "FirstTimeSetupWizardChatPage.h"
#include ".\firsttimesetupwizardchatpage.h"
#include "FirstTimeSetupWizard.h"

namespace Wizards
{
	// CFirstTimeSetupWizardChatPage dialog

	IMPLEMENT_DYNAMIC(CFirstTimeSetupWizardChatPage, CPropertyPage)
		CFirstTimeSetupWizardChatPage::CFirstTimeSetupWizardChatPage()
		: CPropertyPage(CFirstTimeSetupWizardChatPage::IDD)
		, chatName_(_T(""))
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		m_psp.pszHeaderTitle = _T("Chat Server Configuration");
		m_psp.pszHeaderSubTitle = _T("Set your chatname and your chat server port for the Mud Master peer to peer chat network");
	}

	CFirstTimeSetupWizardChatPage::~CFirstTimeSetupWizardChatPage()
	{
	}

	void CFirstTimeSetupWizardChatPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_EDIT1, chatPort_);
		DDX_Text(pDX, IDC_CHATNAME, chatName_);
		DDX_Control(pDX, IDC_CHATNAME, chatNameEdit_);
	}


	BEGIN_MESSAGE_MAP(CFirstTimeSetupWizardChatPage, CPropertyPage)
	END_MESSAGE_MAP()

	BOOL CFirstTimeSetupWizardChatPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		chatPort_ = 4050;

		UpdateData(FALSE);

		return TRUE;  
	}

	BOOL CFirstTimeSetupWizardChatPage::OnSetActive()
	{
		CPropertySheet* psheet = (CPropertySheet*) GetParent();   
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		return CPropertyPage::OnSetActive();
	}

	LRESULT CFirstTimeSetupWizardChatPage::OnWizardNext()
	{
		UpdateData(TRUE);

		if(chatName_.IsEmpty())
		{
			AfxMessageBox(IDS_MISSING_CHATNAME);
			chatNameEdit_.SetFocus();
			return -1;
		}

		return CPropertyPage::OnWizardNext();
	}


}