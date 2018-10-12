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
// FirstTimeSetupWizard.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "FirstTimeSetupWizardWelcomePage.h"
#include ".\firsttimesetupwizardwelcomepage.h"

namespace Wizards
{
	// CFirstTimeSetupWizard dialog

	IMPLEMENT_DYNAMIC(CFirstTimeSetupWizardWelcomePage, CPropertyPage)
		CFirstTimeSetupWizardWelcomePage::CFirstTimeSetupWizardWelcomePage()
		: CPropertyPage(CFirstTimeSetupWizardWelcomePage::IDD)
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_HIDEHEADER;
	}

	CFirstTimeSetupWizardWelcomePage::~CFirstTimeSetupWizardWelcomePage()
	{
	}

	void CFirstTimeSetupWizardWelcomePage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_TITLE, title_);
	}


	BEGIN_MESSAGE_MAP(CFirstTimeSetupWizardWelcomePage, CPropertyPage)
	END_MESSAGE_MAP()


	// CFirstTimeSetupWizard message handlers

	BOOL CFirstTimeSetupWizardWelcomePage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		CString strFaceName;
		if(!strFaceName.LoadString(IDS_WIZARD_HEAD_FACE_NAME))
			strFaceName = _T("Verdana");

		CClientDC dc(this);
		if(!titleFont_.CreatePointFont(120, strFaceName, &dc))
		{
			TRACE(_T("Failed to create point font!"));
		}
		else
		{
			title_.SetFont(&titleFont_);
		}

		return TRUE;
	}

	BOOL CFirstTimeSetupWizardWelcomePage::OnSetActive()
	{
		CPropertySheet* psheet = (CPropertySheet*) GetParent();   
		psheet->SetWizardButtons(PSWIZB_NEXT);

		return CPropertyPage::OnSetActive();
	}

}