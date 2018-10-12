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
// NewSessionCharacterScriptPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "NewSessionCharacterScriptPage.h"
#include ".\newsessioncharacterscriptpage.h"
#include "NewSessionWizard.h"
#include "Globals.h"
#include "FileUtils.h"
#include <string>
#include <sstream>
// CNewSessionCharacterScriptPage dialog

using namespace std;

namespace Wizards
{
	IMPLEMENT_DYNAMIC(CNewSessionCharacterScriptPage, CPropertyPage)
		CNewSessionCharacterScriptPage::CNewSessionCharacterScriptPage()
		: CPropertyPage(CNewSessionCharacterScriptPage::IDD)
		, _scriptPath(_T(""))
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		m_psp.pszHeaderTitle = _T("Mud/Character Script Information");
		m_psp.pszHeaderSubTitle = _T("Which script file would you like to load when you open this Character in Mud Master?");
	}

	CNewSessionCharacterScriptPage::~CNewSessionCharacterScriptPage()
	{
	}

	void CNewSessionCharacterScriptPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_SCRIPT_NAME, _scriptPath);
		DDX_Control(pDX, IDC_SCRIPT_NAME, _scriptPathEdit);
	}


	BEGIN_MESSAGE_MAP(CNewSessionCharacterScriptPage, CPropertyPage)
		ON_BN_CLICKED(IDC_BROWSE_FOR_SCRIPT, OnBrowseForScript)
	END_MESSAGE_MAP()


	// CNewSessionCharacterScriptPage message handlers

	void CNewSessionCharacterScriptPage::OnBrowseForScript()
	{
		static char BASED_CODE szFilter[] = {
			"Mud Master Script Files (*.mm)|*.mm|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"
		};

		CFileDialog dlg(
			TRUE, 
			_T("*.mm"), 
			NULL,
			OFN_HIDEREADONLY | 
			OFN_CREATEPROMPT,
			szFilter,
			this);

		INT_PTR res = dlg.DoModal();
		if(res == IDOK)
		{
			_scriptPath = dlg.GetPathName();

			CFileStatus fs = {0};
			if(!CFile::GetStatus(_scriptPath, fs))
			{
				CFile scriptFile;
				CFileException fe;
				if(!scriptFile.Open(
					_scriptPath,
					CFile::modeCreate|CFile::modeWrite,
					&fe))
				{
					TCHAR error_buffer[1024] = {0};

					fe.GetErrorMessage(error_buffer, 1024);

					CString strError;
					strError.Format(
						_T("Failed to create script file [%s]\n")
						_T("Error: [%d] - %s"),
						scriptFile,
						fe.m_cause,
						error_buffer);
					AfxMessageBox(strError, MB_YESNO);
					_scriptPathEdit.SetFocus();
				}
				else
				{
					scriptFile.Close();
				}
			}
		}

		UpdateData(FALSE);
	}

	BOOL CNewSessionCharacterScriptPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();
		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
			
		CString path;
			// Use fixPath to turn filename into full path to location
			// controled by Ini With Exe setting
		path.Append(psheet->_characterName +".mm");
		_scriptPath = MMFileUtils::fixPath(path);

		UpdateData(FALSE);

		return TRUE;  
	}

	BOOL CNewSessionCharacterScriptPage::OnSetActive()
	{
		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
		return CPropertyPage::OnSetActive();
	}

	LRESULT CNewSessionCharacterScriptPage::OnWizardNext()
	{
		UpdateData();
		if(_scriptPath.IsEmpty())
		{
			AfxMessageBox(IDS_EMPTY_SCRIPT_PATH);
			_scriptPathEdit.SetFocus();
			return -1;
		}

		CFileStatus fs = {0};
		if(!CFile::GetStatus(_scriptPath, fs))
		{
			CFile scriptFile;
			CFileException fe;
			if(!scriptFile.Open(
				_scriptPath,
				CFile::modeCreate|CFile::modeWrite,
				&fe))
			{
				TCHAR error_buffer[1024] = {0};

				fe.GetErrorMessage(error_buffer, 1024);

				CString strError;
				strError.Format(
					_T("Failed to create script file [%s]\n")
					_T("Error: [%d] - %s"),
					scriptFile,
					fe.m_cause,
					error_buffer);
				AfxMessageBox(strError, MB_YESNO);
				_scriptPathEdit.SetFocus();
				return -1;
			}
			else
			{
				scriptFile.Close();
			}
		}

		CNewSessionWizard* psheet = (CNewSessionWizard*) GetParent();
		psheet->_scriptPath = _scriptPath;

		return CPropertyPage::OnWizardNext();
	}

}