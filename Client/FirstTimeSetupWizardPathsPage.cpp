// FirstTimeSetupWizardPathsPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ifx.h"
#include "FirstTimeSetupWizardPathsPage.h"
#include ".\firsttimesetupwizardpathspage.h"
#include "Globals.h"
#include "ErrorHandling.h"
#include <string>


using namespace std;
using namespace Utilities;

namespace Wizards
{
	IMPLEMENT_DYNAMIC(CFirstTimeSetupWizardPathsPage, CPropertyPage)
		CFirstTimeSetupWizardPathsPage::CFirstTimeSetupWizardPathsPage()
		: CPropertyPage(CFirstTimeSetupWizardPathsPage::IDD)
		, helpPath_(_T(""))
		, downloadPath_(_T(""))
		, uploadPath_(_T(""))
		, soundPath_(_T(""))
		, logPath_(_T(""))
	{
		m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
		m_psp.pszHeaderTitle = _T("Folder Locations");
		m_psp.pszHeaderSubTitle = _T("Are these folder locations where you want them?");
	}

	CFirstTimeSetupWizardPathsPage::~CFirstTimeSetupWizardPathsPage()
	{
	}

	void CFirstTimeSetupWizardPathsPage::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_HELP_FILES, helpPath_);
		DDX_Text(pDX, IDC_DOWNLOAD_FOLDER, downloadPath_);
		DDX_Text(pDX, IDC_UPLOAD_FOLDER, uploadPath_);
		DDX_Text(pDX, IDC_SOUND_FOLDER, soundPath_);
		DDX_Text(pDX, IDC_LOG_FOLDER, logPath_);
		DDX_Control(pDX, IDC_HELP_FILES, helpPathEdit_);
		DDX_Control(pDX, IDC_DOWNLOAD_FOLDER, downloadPathEdit_);
		DDX_Control(pDX, IDC_UPLOAD_FOLDER, uploadPathEdit_);
		DDX_Control(pDX, IDC_SOUND_FOLDER, soundPathEdit_);
		DDX_Control(pDX, IDC_LOG_FOLDER, logPathEdit_);
	}

	BEGIN_MESSAGE_MAP(CFirstTimeSetupWizardPathsPage, CPropertyPage)
		ON_BN_CLICKED(IDC_BROWSE_HELP, OnBnClickedBrowseHelp)
		ON_BN_CLICKED(IDC_BROWSE_DOWNLOAD, OnBnClickedBrowseDownload)
		ON_BN_CLICKED(IDC_BROWSE_UPLOAD, OnBnClickedBrowseUpload)
		ON_BN_CLICKED(IDC_BROWSE_SOUND, OnBnClickedBrowseSound)
		ON_BN_CLICKED(IDC_BROWSE_LOG, OnBnClickedBrowseLog)
	END_MESSAGE_MAP()


	BOOL CFirstTimeSetupWizardPathsPage::OnInitDialog()
	{
		CPropertyPage::OnInitDialog();

		GetDefaultHelpPath();
		GetDefaultDownloadPath();
		GetDefaultUploadPath();
		GetDefaultSoundPath();
		GetDefaultLogPath();

		UpdateData(FALSE);

		return TRUE;  
	}

	void CFirstTimeSetupWizardPathsPage::GetDefaultHelpPath()
	{
		std::string dir;
		if(SUCCEEDED(CGlobals::GetModuleDirectory(dir)))
		{
			dir += "Help";
		}

		helpPath_ = dir.c_str();
	}

	void CFirstTimeSetupWizardPathsPage::GetDefaultDownloadPath()
	{
		std::string dir;
		if(SUCCEEDED(CGlobals::GetMusicFolder(dir)))
		{
			dir += "\\Mud Master\\Transfers";
		}

		downloadPath_ = dir.c_str();
	}

	void CFirstTimeSetupWizardPathsPage::GetDefaultUploadPath()
	{
		std::string dir;
		if(SUCCEEDED(CGlobals::GetMusicFolder(dir)))
		{
			dir += "\\Mud Master\\Transfers";
		}

		uploadPath_ = dir.c_str();
	}

	void CFirstTimeSetupWizardPathsPage::GetDefaultSoundPath()
	{
		std::string dir;
		if(SUCCEEDED(CGlobals::GetMusicFolder(dir)))
		{
			dir += "\\Mud Master\\Sounds";
		}

		soundPath_ = dir.c_str();
	}

	void CFirstTimeSetupWizardPathsPage::GetDefaultLogPath()
	{
		std::string dir;
		if(SUCCEEDED(CGlobals::GetMusicFolder(dir)))
		{
			dir += "\\Mud Master\\Logs";
		}

		logPath_ = dir.c_str();
	}
	void CFirstTimeSetupWizardPathsPage::OnBnClickedBrowseHelp()
	{
		string path;
		if(SUCCEEDED(CGlobals::BrowseForFolder(IDS_BROWSE_HELP_FOLDER, path)))
		{
			if(!path.empty())
			{
				helpPath_ = path.c_str();
				UpdateData(FALSE);
			}
		}
	}

	void CFirstTimeSetupWizardPathsPage::OnBnClickedBrowseDownload()
	{
		string path;
		if(SUCCEEDED(CGlobals::BrowseForFolder(IDS_BROWSE_DOWNLOAD_FOLDER, path)))
		{
			if(!path.empty())
			{
				downloadPath_ = path.c_str();
				UpdateData(FALSE);
			}
		}
	}

	void CFirstTimeSetupWizardPathsPage::OnBnClickedBrowseUpload()
	{
		string path;
		if(SUCCEEDED(CGlobals::BrowseForFolder(IDS_BROWSE_UPLOAD_FOLDER, path)))
		{
			if(!path.empty())
			{
				uploadPath_ = path.c_str();
				UpdateData(FALSE);
			}
		}
	}

	void CFirstTimeSetupWizardPathsPage::OnBnClickedBrowseSound()
	{
		string path;
		if(SUCCEEDED(CGlobals::BrowseForFolder(IDS_BROWSE_SOUND_FOLDER, path)))
		{
			if(!path.empty())
			{
				soundPath_ = path.c_str();
				UpdateData(FALSE);
			}
		}
	}

	void CFirstTimeSetupWizardPathsPage::OnBnClickedBrowseLog()
	{
		string path;
		if(SUCCEEDED(CGlobals::BrowseForFolder(IDS_BROWSE_LOG_FOLDER, path)))
		{
			if(!path.empty())
			{
				logPath_ = path.c_str();
				UpdateData(FALSE);
			}
		}
	}

	HRESULT CFirstTimeSetupWizardPathsPage::ConfirmDirectory(
		const CString &dir,
		CEdit &ctrl,
		int missingPathId,
		int invalidPathId)
	{
		NFUNTRACE;
		HRESULT hr = E_UNEXPECTED;

		try
		{
			NTRACE(_T("Confirming directory: %s"), dir);

			if(dir.IsEmpty())
			{
				AfxMessageBox(missingPathId);
				ctrl.SetFocus();
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			if(!PathFileExists(dir))
			{
				int nRet = AfxMessageBox(invalidPathId, MB_YESNO | MB_ICONQUESTION);
				if(IDYES == nRet)
				{
					if(!::CreateDirectory(dir, NULL))
					{
						DWORD dwError = ::GetLastError();
						NW32TRACE(dwError, _T("CreateDirectory()"));
						ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
					}
				}
				else
				{
					ctrl.SetFocus();
					ErrorHandlingUtils::TESTHR(E_INVALIDARG);
				}
			}

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return hr;
	}

	LRESULT CFirstTimeSetupWizardPathsPage::OnWizardNext()
	{
		UpdateData(TRUE);

		HRESULT hr = E_UNEXPECTED;

		try
		{
			ErrorHandlingUtils::TESTHR(ConfirmDirectory(
				helpPath_, helpPathEdit_, IDS_MISSING_HELPPATH, IDS_INVALID_HELPPATH ));

			ErrorHandlingUtils::TESTHR(ConfirmDirectory(
				downloadPath_, downloadPathEdit_, IDS_MISSING_DOWNLOADPATH, IDS_INVALID_DOWNLOADPATH ));

			ErrorHandlingUtils::TESTHR(ConfirmDirectory(
				uploadPath_, uploadPathEdit_, IDS_MISSING_UPLOADPATH, IDS_INVALID_UPLOADPATH ));

			ErrorHandlingUtils::TESTHR(ConfirmDirectory(
				soundPath_, soundPathEdit_, IDS_MISSING_SOUNDPATH, IDS_INVALID_SOUNDPATH ));

			ErrorHandlingUtils::TESTHR(ConfirmDirectory(
				logPath_, logPathEdit_, IDS_MISSING_LOGPATH, IDS_INVALID_LOGPATH ));

			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}


		LRESULT retVal = 0;

		if(SUCCEEDED(hr))
			retVal = CPropertyPage::OnWizardNext();
		else
			retVal = -1;

		return retVal;
	}

	BOOL CFirstTimeSetupWizardPathsPage::OnSetActive()
	{
		CPropertySheet* psheet = (CPropertySheet*) GetParent();
		psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

		return CPropertyPage::OnSetActive();
	}
}