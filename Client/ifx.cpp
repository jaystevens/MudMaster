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
#define INITGUID
#include "Ifx.h"

#include <dos.h>
#include <direct.h>
#include <eh.h>

#include "MainFrm.h"
#include "ChildFrm.h"
#include "IfxDoc.h"
#include "NewWindowView.h"
#include "Globals.h"
#include "CommandTable.h"
#include "ProcedureTable.h"
#include "SeException.h"
#include "shlobj.h"
#include "Sess.h"
#include "SessionInfo.h"
#include "MessageOptions.h"
#include "compinfo.h"
#include "FirstTimeSetupWizard.h"
#include "DefaultOptions.h"
#include "NewSessionWizard.h"
#include "ConfigFileConstants.h"
#include "ErrorHandling.h"
#include "FileUtils.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace Wizards;
using namespace Utilities;

struct CIfxApp::AppImpl
{
	HRESULT ProcessDefaults(
		CFirstTimeSetupWizard &wizard, 
		bool &launchNewCharacter,
		CString &newSessionFilename);

	HRESULT SetPaths(COptions &defaultOptions);

	BOOL RegisterWindowClasses();

	BOOL m_bMidiOpen;
	CMsp m_MSP;
	CString m_strTermWndClassName;
	CString m_strStatusWndClassName;
	CString m_strDefaultSessionFileName;
};

BEGIN_MESSAGE_MAP(CIfxApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

CIfxApp::CIfxApp()
: _pImpl(new CIfxApp::AppImpl)
{
	_set_se_translator(SeTranslator);
}

CIfxApp theApp;

BOOL CIfxApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	if(!_pImpl->RegisterWindowClasses())
		return FALSE;

	InitCommonControls();

	if(!AfxInitRichEdit())
		return FALSE;

	////Where to store/retrieve recent file list and couple of other things in the registry
 //   SetRegistryKey(_T("MM2K6"));

	// This loads the recent file list
	// from the MudMaster.ini in the Windows directory if registry key not set

	//Try to control the name and location of the main ini file

//First free the string allocated by MFC at CWinApp startup.
//The string is allocated before InitInstance is called.
free((void*)m_pszProfileName);
//Change the name of the .INI file.
//The CWinApp destructor will free the memory.
//The ini holding the recent files and windows position information
//will be in the directory where the exe is run from
m_pszProfileName=_tcsdup(_T(".\\MudMaster2k6.ini"));


	LoadStdProfileSettings();
	CString dir = CGlobals::GetConfigFilename();
	TCHAR ipAddr[20] = {0};
	::GetPrivateProfileString(
		DEFAULT_SECTION_NAME, 
		DEFAULT_IP_ADDRESS, 
		_T(""),
		ipAddr,
		20,
		dir );

	CGlobals::Initialize( ipAddr );


	CMultiDocTemplate *pDocTemplate = new CMultiDocTemplate(
		IDR_IFXTYPE,
		RUNTIME_CLASS(CIfxDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CNewWindowView));
	AddDocTemplate(pDocTemplate);

	CCommandTable::Hash();
	CProcedureTable::Hash();


	bool launchNewCharacter = false;
	CString strNewSessionFilename;
	if( !PathFileExists( dir ) )
	{
		Wizards::CFirstTimeSetupWizard wizard(_T("First Time Setup"));
		UINT nRet = wizard.DoModal();
		switch(nRet)
		{
		case IDCANCEL:
			return FALSE;
		case ID_WIZFINISH:
			_pImpl->ProcessDefaults(
				wizard, 
				launchNewCharacter,
				strNewSessionFilename);

			break;
		default:
			ASSERT(FALSE);
		}
	}



	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
//First free the string allocated by MFC at CWinApp startup.
//The string is allocated before InitInstance is called.
free((void*)m_pszAppName);
//Change the name of the application file.
//The CWinApp destructor will free the memory.
//TODO: KW attempt to override the name used in captions
	m_pszAppName=_tcsdup(CGlobals::GetMsgBoxTitle());


	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}

	vector<TCHAR> def_session_file;
	def_session_file.resize(MAX_PATH + 1);

	::GetPrivateProfileString(
		DEFAULT_SECTION_NAME, 
		DEFAULT_SESSION_KEY, 
		_T(""),
		&def_session_file[0],
		MAX_PATH,
		dir );

	CString strFile ;
	// use fixPath to convert relative path to full path if necessary
	strFile = MMFileUtils::fixPath(&def_session_file[0]);


	_pImpl->m_strDefaultSessionFileName = strFile;


	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		if(_pImpl->m_strDefaultSessionFileName.IsEmpty())
		{
			if(!launchNewCharacter)
				cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
			else
			{
				cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
				cmdInfo.m_strFileName = strNewSessionFilename;
			}
		}
		else
		{
			CFileStatus fs;
			if(CFile::GetStatus(_pImpl->m_strDefaultSessionFileName, fs))
			{
				cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;
				cmdInfo.m_strFileName = _pImpl->m_strDefaultSessionFileName;
			}
		}
	}

	if(cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		return TRUE;
	}

	if (!ProcessShellCommand(cmdInfo))
	{
		if(!_pImpl->m_strDefaultSessionFileName.IsEmpty())
		{
			CString strMessage;
			strMessage.Format(
				_T("There was an error loading the file %s.\n\n")
				_T("Make sure that you have identified a valid MudMaster Session file for your default session.\n\n")
				_T("Would you like to disable the default session?"), 
				_pImpl->m_strDefaultSessionFileName);

			int nRet = AfxMessageBox(strMessage, MB_YESNO|MB_ICONQUESTION);
			if(nRet == IDYES)
			{
				::WritePrivateProfileString(
					DEFAULT_SECTION_NAME,
					DEFAULT_SESSION_KEY,
					_T(""),
					dir );
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/**
 * \class CAboutDlg ifx.cpp 
 * \brief wrapper for the About Dialog
 */
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CButton	m_btnOk;
	CString	m_strVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CIfxApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CIfxApp message handlers

CMsp& CIfxApp::MSP()
{
	return _pImpl->m_MSP;
}

BOOL CIfxApp::MidiOpen()
{
	return _pImpl->m_bMidiOpen;
}

void CIfxApp::MidiOpen(BOOL bMidiOpen)
{
	_pImpl->m_bMidiOpen = bMidiOpen;
}

int CIfxApp::ExitInstance() 
{
#ifdef _DEBUG_PROCESS
	if (NULL != hEvent)
		SetEvent((HANDLE)hEvent->StopEvent);

	while (WaitForSingleObject(hThread,1000) != WAIT_OBJECT_0)
		Sleep(1000);
#endif

	return CWinApp::ExitInstance();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	std::string version;
	CGlobals::GetFileVersion(version);

	std::string buildNumber;
	CGlobals::GetBuildNumber(buildNumber);

	std::stringstream versionLabel;
	versionLabel
		<< CGlobals::GetMsgBoxTitle() //"MudMaster 2k6"
		<< version
		<< " (Build "
		<< buildNumber
		<< ")";

	m_strVersion = versionLabel.str().c_str();

	UpdateData(FALSE);
	
	// CG: Following block was added by System Info Component.
	{
		CString strFreeDiskSpace;
		CString strFreeMemory;
		CString strFmt;

		// Fill available memory
		MEMORYSTATUS MemStat;
		MemStat.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&MemStat);
		strFmt.LoadString(CG_IDS_PHYSICAL_MEM);
		strFreeMemory.Format(strFmt, MemStat.dwAvailPhys / 1024L);

		//TODO: Add a static control to your About Box to receive the memory
		//      information.  Initialize the control with code like this:
		// SetDlgItemText(IDC_PHYSICAL_MEM, strFreeMemory);
		SetDlgItemText(IDC_PHYSICAL_MEM, strFreeMemory);

		
		FARPROC pGetDiskFreeSpaceEx = GetProcAddress(GetModuleHandle("kernel32.dll"), "GetDiskFreeSpaceExA");

		if(pGetDiskFreeSpaceEx != NULL)
		{
			ULARGE_INTEGER nFreeBytes;
			ULARGE_INTEGER nTotalBytes;
			ULARGE_INTEGER nTotalFreeBytes;

			BOOL bResult = GetDiskFreeSpaceEx(NULL,
				(PULARGE_INTEGER)&nFreeBytes,
				(PULARGE_INTEGER)&nTotalBytes,
				(PULARGE_INTEGER)&nTotalFreeBytes);

			if(bResult)
			{
				strFreeDiskSpace.Format("%I64u KB free of %I64u KB total.",	nFreeBytes.QuadPart / 1024L, nTotalBytes.QuadPart / 1024L);
			}
			else
			{
				strFreeDiskSpace.LoadString(CG_IDS_DISK_SPACE_UNAVAIL);
			}
			SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);
		}
		else
		{
			DWORD dwSectPerCluster;
			DWORD dwBytesPerSector;
			DWORD dwFreeClusters;
			DWORD dwTotalClusters;

			BOOL bResult = GetDiskFreeSpace(
				NULL,
				&dwSectPerCluster,
				&dwBytesPerSector,
				&dwFreeClusters,
				&dwTotalClusters);
			
			if(bResult)
			{
				strFreeDiskSpace.Format("%lu KB free.",	
					(DWORD)dwSectPerCluster * 
					(DWORD)dwBytesPerSector * 
					(DWORD)dwFreeClusters / 1024L);
				SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);
			}
			else
			{
				strFreeDiskSpace.LoadString(CG_IDS_DISK_SPACE_UNAVAIL);
			}
			SetDlgItemText(IDC_DISK_SPACE, strFreeDiskSpace);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIfxApp::WinHelp(DWORD /*dwData*/, UINT /*nCmd*/)
{
}

BOOL CIfxApp::AppImpl::RegisterWindowClasses()
{
	m_strTermWndClassName = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(BLACK_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION) );

	m_strStatusWndClassName = AfxRegisterWndClass(
		CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(BLACK_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	return TRUE;
}


#ifdef _DEBUG_PROCESS
DWORD WINAPI CIfxApp::UserThreadProc(LPVOID lpParameter)
{
	EVENTS* hWait = (EVENTS *)lpParameter;
	DWORD  dwStartTime = GetTickCount();

#define PERFORMANCE_FILENAME "performance.log"
	CCompInfo* hInfo = new CCompInfo(PERFORMANCE_FILENAME);

	hInfo->HeapMakeSnapShot();
	hInfo->HeapStoreDumpToFile();

	SetEvent((HANDLE)hWait->StartEvent);

	hInfo->m_log->print("Test started at %s\n", hInfo->GetTimeString() );

	while (1)
	{
		if (WaitForSingleObject((HANDLE)hWait->StopEvent,0) == WAIT_OBJECT_0)
			break;
		hInfo->m_log->print("%s CPU[%d%%] Memory[%d]\n",	hInfo->GetTimeString(),hInfo->GetCPUInfo(), hInfo->HeapCommitedBytes());
		Sleep(1000);
	};

	hInfo->m_log->print("%s CPU[%d%%] Memory[%dKb]\n",	hInfo->GetTimeString(),hInfo->GetCPUInfo(), hInfo->HeapCommitedBytes()/1024);
	hInfo->m_log->print("Test finished at %s\n", hInfo->GetTimeString() );
	hInfo->m_log->print("Elapsed time %d sec\n", (GetTickCount() - dwStartTime)/1000 );
	hInfo->m_log->print("Total memory difference: %dKb\n\n", hInfo->HeapCompareSnapShots()/1024 );

	CloseHandle((HANDLE)hWait->StopEvent);
	CloseHandle((HANDLE)hWait->StartEvent);

	if (NULL != hWait)
		delete hWait;

//	hInfo->HeapCompareDumpWithFile(FALSE); // basic report
//	hInfo->HeapCompareDumpWithFile(TRUE); // extended report

	if (NULL != hInfo)
		delete hInfo;

	return 0;
}
#endif

HRESULT CIfxApp::AppImpl::SetPaths(COptions &defaultOptions)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CString basePath;
		if( !MMFileUtils::getPersonalFolder( basePath ) )
			ATLASSERT(!"Failed to get personal folder!");


		if( !::CreateDirectory(basePath, NULL) )
			ErrorHandlingUtils::err(_T("CreateDirectory"));

		std::string path;
	
		path = basePath;
		path += "\\Transfers";

		if(!::CreateDirectory(path.c_str(), NULL))
			ErrorHandlingUtils::err(_T("CreateDirectory"));

		defaultOptions.pathOptions_.DownloadPath(path.c_str());
		defaultOptions.pathOptions_.UploadPath(path.c_str());

		path = basePath;
		path += "\\Log Files";
		
		if(!::CreateDirectory(path.c_str(), NULL))
			ErrorHandlingUtils::err(_T("CreateDirectory"));

		defaultOptions.pathOptions_.LogPath(path.c_str());

		path = basePath;
		path += "\\Sounds";

		if(!::CreateDirectory(path.c_str(), NULL))
			ErrorHandlingUtils::err(_T("CreateDirectory"));

		defaultOptions.pathOptions_.Put_SoundPath(path.c_str());

		hr = S_OK;
	}
	catch(_com_error)
	{
		::OutputDebugString("Error in Ifx SetPaths\n");
	}

	return hr;
}
HRESULT CIfxApp::AppImpl::ProcessDefaults(
	CFirstTimeSetupWizard &wizard, 
	bool &bLaunchCharacter,
	CString &strNewSessionFilename)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		COptions defaultOptions;

		defaultOptions.LoadDefaults();

		ErrorHandlingUtils::TESTHR(SetPaths(defaultOptions));

		defaultOptions.chatOptions_.ChatName(wizard.chatPage_.chatName_);
		defaultOptions.chatOptions_.ListenPort(wizard.chatPage_.chatPort_);
//TODO: KW must add the Ini With Exe option by this point so when the default character starts it runs from the correct location
//TODO: KW have to add the question to the wizard first though
		defaultOptions.SaveDefaults();

		if(wizard.completePage_.runCharSetup_)
		{
			bLaunchCharacter = false;
			CNewSessionWizard characterWizard(_T("Create a new character"));
			UINT_PTR nRes = characterWizard.DoModal();
			if(ID_WIZFINISH == nRes)
			{
				if(characterWizard.completePage_._defaultCharacter)
				{
					CString dir;

					dir = MMFileUtils::fixPath(INI_FILE_NAME);

					::WritePrivateProfileString(
						DEFAULT_SECTION_NAME, 
						DEFAULT_SESSION_KEY, 
						characterWizard.completePage_._newSessionFilename,
						dir );
				}
				else if(characterWizard.completePage_._connectNow)
				{
					bLaunchCharacter = true;
					strNewSessionFilename = characterWizard.completePage_._newSessionFilename;
				}
			}
		}

		hr = S_OK;
	}
	catch(_com_error)
	{
		::OutputDebugString("Error in Ifx ProcessDefaults\n");
	}

	return hr;
}

const CString &CIfxApp::GetTerminalWindowClassName() const
{
	return _pImpl->m_strTermWndClassName;
}

const CString &CIfxApp::GetStatusWindowClassName() const
{
	return _pImpl->m_strStatusWndClassName;
}

const CString &CIfxApp::GetDefaultSessionFileName() const
{
	return _pImpl->m_strDefaultSessionFileName;
}

void CIfxApp::SetDefaultSessionFileName(const CString &name)
{
	_pImpl->m_strDefaultSessionFileName = name;
}
