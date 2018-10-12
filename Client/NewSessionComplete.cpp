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
// NewSessionComplete.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "NewSessionComplete.h"
#include ".\newsessioncomplete.h"
#include "NewSessionWizard.h"
#include "ErrorHandling.h"
#include "Globals.h"
#include "FileUtils.h"
#include <sstream>

using namespace Utilities;

namespace Wizards
{
	// CNewSessionComplete dialog

	IMPLEMENT_DYNAMIC(CNewSessionComplete, CPropertyPage)
		CNewSessionComplete::CNewSessionComplete()
		: CPropertyPage(CNewSessionComplete::IDD)
		, _connectNow(FALSE)
		, _sessionPath(_T(""))
		, _defaultCharacter(FALSE)
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_HIDEHEADER;
	}

	CNewSessionComplete::~CNewSessionComplete()
	{
	}

	void CNewSessionComplete::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_COMPLETE_HEADER, _completeHeader);
		DDX_Check(pDX, IDC_CONNECT_NOW, _connectNow);
		DDX_Text(pDX, IDC_SESSION_PATH, _sessionPath);
		DDX_Check(pDX, IDC_DEFAULT_CHECK, _defaultCharacter);
		DDX_Control(pDX, IDC_CONNECT_NOW, _connectNowCheck);
		DDX_Control(pDX, IDC_DEFAULT_CHECK, _defaultCheck);
	}


	BEGIN_MESSAGE_MAP(CNewSessionComplete, CPropertyPage)
		ON_BN_CLICKED(IDC_DEFAULT_CHECK, OnBnClickedDefault)
	END_MESSAGE_MAP()


	// CNewSessionComplete message handlers

	BOOL CNewSessionComplete::OnInitDialog()
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			CPropertyPage::OnInitDialog();

			CString strFaceName;
			if(!strFaceName.LoadString(IDS_WIZARD_HEAD_FACE_NAME))
				strFaceName = _T("Verdana");

			CClientDC dc(this);
			if(!_completeHeaderFont.CreatePointFont(120, strFaceName, &dc))
			{
				TRACE(_T("Failed to create point font!"));
			}
			else
			{
				_completeHeader.SetFont(&_completeHeaderFont);
			}

			CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();

			CString path;
			// Use fixPath to turn filename into full path to location
			// controled by Ini With Exe setting
			path.Append(psheet->_characterName +".mms");
			_sessionPath = MMFileUtils::fixPath(path);


			UpdateData(FALSE);

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return TRUE;  
	}

	BOOL CNewSessionComplete::OnSetActive()
	{
		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();   
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

		return CPropertyPage::OnSetActive();
	}

	BOOL CNewSessionComplete::OnWizardFinish()
	{
		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
		
			CString path;
			// Use fixPath to turn filename into full path to location
			// controled by Ini With Exe setting
			path.Append(psheet->_characterName +".mms");
			_newSessionFilename = MMFileUtils::fixPath(path);

		bool overwrite = true;

		CFileStatus fs = {0};
		if(CFile::GetStatus(_newSessionFilename, fs))
		{
			CString strMessage;
			strMessage.Format(
				_T("There is already a character session file named %s\n")
				_T("Would you like to overwrite the old character session file with this new one?"),
				_newSessionFilename);

			INT_PTR nRes = AfxMessageBox(strMessage, MB_YESNO);
			if(IDNO == nRes)
				overwrite = false;
		}

		if(overwrite)
		{
			COptions options;
			options.LoadDefaults();

			options.chatOptions_.ChatName(psheet->_characterName);
			options.terminalOptions_.TelnetGA(psheet->_detectEndOfRecord);
			options.sessionInfo_.Address(psheet->_mudAddress);
			options.sessionInfo_.Port(psheet->_mudPort);
			options.sessionInfo_.AutoLoadScript(true);
			options.sessionInfo_.ScriptFilename(psheet->_scriptPath);
			options.sessionInfo_.SessionName(psheet->_characterName);

			options.Save(_newSessionFilename);
		}

		return CPropertyPage::OnWizardFinish();
	}

	void CNewSessionComplete::OnBnClickedDefault()
	{
		if(_defaultCheck.GetCheck() == BST_CHECKED)
		{
			_connectNowCheck.SetCheck(BST_UNCHECKED);
			_connectNowCheck.EnableWindow(FALSE);
		}
		else
		{
			_connectNowCheck.EnableWindow(TRUE);
		}
	}



}