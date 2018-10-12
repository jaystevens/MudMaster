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
// NewSessionMudInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "NewSessionMudInfoPage.h"
#include ".\newsessionmudinfopage.h"
#include "NewSessionWizard.h"


namespace Wizards
{
	// CNewSessionMudInfoPage dialog

	IMPLEMENT_DYNCREATE(CNewSessionMudInfoPage, CPropertyPage)

		// Message map

		BEGIN_MESSAGE_MAP(CNewSessionMudInfoPage, CPropertyPage)
			ON_BN_CLICKED(IDC_BROWSE_FOR_MUD, OnBrowseForMud)
		END_MESSAGE_MAP()


		CNewSessionMudInfoPage::CNewSessionMudInfoPage() 
		:CPropertyPage(CNewSessionMudInfoPage::IDD)
		, _characterName(_T(""))
		, _mudAddress(_T(""))
		, _mudPort(0)
		, _useTelnetGA(TRUE)
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		m_psp.pszHeaderTitle = _T("Mud/Character Information");
		m_psp.pszHeaderSubTitle = _T("What is the name of your character and on what MUD does it exist?");
	}

	void CNewSessionMudInfoPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_CHARACTER_NAME, _characterName);
		DDX_Text(pDX, IDC_MUD_ADDRESS, _mudAddress);
		DDX_Text(pDX, IDC_IDC_MUD_PORT, _mudPort);
		DDV_MinMaxUInt(pDX, _mudPort, 0, 65535);
		DDX_Check(pDX, IDC_END_OF_LINE_DETECT, _useTelnetGA);
		DDX_Control(pDX, IDC_MUD_ADDRESS, _mudAddressEdit);
		DDX_Control(pDX, IDC_CHARACTER_NAME, _characterNameEdit);
	}

	// CNewSessionMudInfoPage message handlers

	BOOL CNewSessionMudInfoPage::OnSetActive()
	{
		return CPropertyPage::OnSetActive();
	}

	BOOL CNewSessionMudInfoPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		_characterName = psheet->defaultOptions_.chatOptions_.ChatName();
		_mudPort = 4000;
		UpdateData(FALSE);

		return TRUE;  
	}

	LRESULT CNewSessionMudInfoPage::OnWizardNext()
	{
		UpdateData();
		_characterName.Trim();
		_mudAddress.Trim();

		if(_characterName.IsEmpty())
		{
			AfxMessageBox(IDS_EMPTYCHARACTERNAME);
			_characterNameEdit.SetFocus();
			return -1;
		}

		if(_mudAddress.IsEmpty())
		{
			AfxMessageBox(IDS_EMPTYMUDADDRESS);
			_mudAddressEdit.SetFocus();
			return -1;
		}

		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
		psheet->_characterName = _characterName;
		psheet->_mudAddress = _mudAddress;
		psheet->_mudPort = static_cast<USHORT>(_mudPort);
		psheet->_detectEndOfRecord = _useTelnetGA ? true : false;

		return CPropertyPage::OnWizardNext();
	}

	void CNewSessionMudInfoPage::OnBrowseForMud()
	{
		// TODO: Add your control notification handler code here
	}

}