#pragma once
#include "afxwin.h"

namespace Wizards
{
	// CFirstTimeSetupWizardPathsPage dialog

	class CFirstTimeSetupWizardPathsPage : public CPropertyPage
	{
		DECLARE_DYNAMIC(CFirstTimeSetupWizardPathsPage)

	public:
		CFirstTimeSetupWizardPathsPage();
		virtual ~CFirstTimeSetupWizardPathsPage();

		// Dialog Data
		enum { IDD = IDD_FIRSTTIME_PATHS };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		void GetDefaultHelpPath();
		void GetDefaultDownloadPath();
		void GetDefaultUploadPath();
		void GetDefaultSoundPath();
		void GetDefaultLogPath();
		HRESULT ConfirmDirectory(
			const CString &dir,
			CEdit &ctrl,
			int missingPathId,
			int invalidPathId);

		DECLARE_MESSAGE_MAP()
	public:
		CString helpPath_;
		CString downloadPath_;
		CString uploadPath_;
		CString soundPath_;
		CString logPath_;
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedBrowseHelp();
		afx_msg void OnBnClickedBrowseDownload();
		afx_msg void OnBnClickedBrowseUpload();
		afx_msg void OnBnClickedBrowseSound();
		afx_msg void OnBnClickedBrowseLog();
		virtual LRESULT OnWizardNext();
		CEdit helpPathEdit_;
		CEdit downloadPathEdit_;
		CEdit uploadPathEdit_;
		CEdit soundPathEdit_;
		CEdit logPathEdit_;
		virtual BOOL OnSetActive();
	};

}