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
#include "SessionInfoDialog.h"
#include "DefaultOptions.h"
#include ".\sessioninfodialog.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSessionInfoDialog::CSessionInfoDialog(COptions &options)
	: CPropertyPage(CSessionInfoDialog::IDD)
	, _options(options)
{
	//{{AFX_DATA_INIT(CSessionInfoDialog)
	m_bChatServer = FALSE;
	m_bEvents = FALSE;
	m_bIgnoreAliases = FALSE;
	m_bMaximize = FALSE;
	m_bMaxWidth = FALSE;
	m_bMaxHeight = FALSE;
	m_bPreSub = FALSE;
	m_bPromptOverwrite = FALSE;
	m_strAddress = _T("");
	m_strName = _T("");
	m_bSpeedWalk = FALSE;
	m_bStatusBar = FALSE;
	m_strSessionFile = _T("");
	m_bAutoLoadScript = FALSE;
	m_bAppendLog = FALSE;
	m_bAutoLog = FALSE;
	m_strLogFileName = _T("");
	m_nPace = 0;
	m_nPort = 0;
	m_nSubDepth = 0;
	m_bShowInfo = FALSE;
	m_strCommandChar = "";
	//}}AFX_DATA_INIT
}


void CSessionInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSessionInfoDialog)
	DDX_Check(pDX, IDC_CHAT_SERVER, m_bChatServer);
	DDX_Check(pDX, IDC_EVENTS, m_bEvents);
	DDX_Check(pDX, IDC_IGNORE_ALIASES, m_bIgnoreAliases);
	DDX_Check(pDX, IDC_MAXIMIZE, m_bMaximize);
	DDX_Check(pDX, IDC_MAX_WIDTH, m_bMaxWidth);
	DDX_Check(pDX, IDC_MAX_HEIGHT, m_bMaxHeight);
	DDX_Check(pDX, IDC_DOCK_RIGHT, m_bDockRight);
	DDX_Check(pDX, IDC_PRESUB, m_bPreSub);
	DDX_Check(pDX, IDC_PROMPT_OVERWRITE, m_bPromptOverwrite);
	DDX_Text(pDX, IDC_SESSION_ADDRESS, m_strAddress);
	DDX_Text(pDX, IDC_SESSION_NAME, m_strName);
	DDX_Check(pDX, IDC_SPEEDWALK, m_bSpeedWalk);
	DDX_Check(pDX, IDC_STATUSBAR, m_bStatusBar);
	DDX_Text(pDX, IDC_SCRIPT_PATH, m_strSessionFile);
	DDX_Check(pDX, IDC_AUTO_LOADSCRIPT, m_bAutoLoadScript);
	DDX_Check(pDX, IDC_APPEND_LOG, m_bAppendLog);
	DDX_Check(pDX, IDC_AUTO_LOG, m_bAutoLog);
	DDX_Text(pDX, IDC_LOG_FILENAME, m_strLogFileName);
	DDX_Text(pDX, IDC_PACE, m_nPace);
	DDX_Text(pDX, IDC_SESSION_PORT, m_nPort);
	DDX_Text(pDX, IDC_SUB_DEPTH, m_nSubDepth);
	DDV_MinMaxShort(pDX, m_nSubDepth, 1, 10);
	DDX_Check(pDX, IDC_SHOW_INFO, m_bShowInfo);
	DDX_Text(pDX, IDC_COMMAND_CHAR, m_strCommandChar);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSessionInfoDialog, CDialog)
	//{{AFX_MSG_MAP(CSessionInfoDialog)
	ON_BN_CLICKED(IDC_NOTES, OnNotes)
	ON_BN_CLICKED(IDC_SCRIPT_BROWSE, OnScriptBrowse)
	ON_BN_CLICKED(IDC_LOGFILE_BROWSE, OnLogfileBrowse)
	ON_BN_CLICKED(IDC_MAXIMIZE, OnMaxChange)
	ON_BN_CLICKED(IDC_MAX_WIDTH, OnMaxChange)
	ON_BN_CLICKED(IDC_MAX_HEIGHT, OnMaxChange)
	ON_BN_CLICKED(IDC_DOCK_RIGHT, OnMaxChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSessionInfoDialog message handlers

void CSessionInfoDialog::OnNotes() 
{
	
}

void CSessionInfoDialog::OnMaxChange()
{
	BOOL m_bOldMaxWidth(m_bMaxWidth), m_bOldMaxHeight(m_bMaxHeight) , m_bOldDockRight(m_bDockRight);

	UpdateData(TRUE);
	//if(m_bMaximize)
	//{
	//	m_bMaxWidth = FALSE;
	//	m_bMaxHeight = FALSE;
	//	return;
	//}
	if(m_bMaxWidth && !m_bOldMaxWidth)
	{
		m_bMaximize = FALSE;
		m_bMaxHeight = FALSE;
		m_bDockRight = FALSE;
	UpdateData(FALSE);

		return;
	}
	if(m_bMaxHeight && !m_bOldMaxHeight)
	{
		m_bMaximize = FALSE;
		m_bMaxWidth = FALSE;
		m_bDockRight = FALSE;
			UpdateData(FALSE);

		return;
	}
	if(m_bDockRight && !m_bOldDockRight)
	{
		m_bMaximize = FALSE;
		m_bMaxWidth = FALSE;
		m_bMaxHeight = FALSE;
			UpdateData(FALSE);

		return;
	}

}

void CSessionInfoDialog::OnScriptBrowse() 
{
	UpdateData(TRUE);
	CFileDialog dlg(TRUE);
	
	if(dlg.DoModal() != IDOK)
		return;

	m_strSessionFile = dlg.GetPathName();
	UpdateData(FALSE);
}

void CSessionInfoDialog::OnLogfileBrowse() 
{
	UpdateData(TRUE);
	CTime timeNow;
	timeNow = CTime::GetCurrentTime();

	CString strLogFileName;
	strLogFileName.Format("log%d", timeNow);
	CFileDialog dlg(TRUE, "log", strLogFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Log Files (*.log)|*.log|All Files (*.*)|*.*||");

	if(dlg.DoModal() != IDOK)
		return;

	m_strLogFileName = dlg.GetPathName();
	UpdateData(FALSE);
}

BOOL CSessionInfoDialog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bChatServer		= _options.sessionInfo_.StartChatOnOpen();
	m_bEvents			= _options.sessionInfo_.ProcessEvents();
	m_bIgnoreAliases	= _options.sessionInfo_.IgnoreAliases();
	m_bMaximize			= _options.sessionInfo_.MaxOnOpen();
	m_strMaximize		= _options.sessionInfo_.Maximize();
	m_bPreSub			= _options.sessionInfo_.PreSub();
	m_bPromptOverwrite	= _options.sessionInfo_.PromptOverwrite();
	m_strAddress		= _options.sessionInfo_.Address();
	m_strName			= _options.sessionInfo_.SessionName();
	m_bSpeedWalk		= _options.sessionInfo_.SpeedWalk();
	m_bStatusBar		= _options.sessionInfo_.ShowStatusBar();
	m_strSessionFile	= _options.sessionInfo_.ScriptFilename();
	m_bAutoLoadScript	= _options.sessionInfo_.AutoLoadScript();
	m_bAppendLog		= _options.sessionInfo_.AppendLog();
	m_bAutoLog			= _options.sessionInfo_.AutoLog();
	m_strLogFileName	= _options.sessionInfo_.LogFileName();
	m_nPace				= _options.sessionInfo_.Pace();
	m_nPort				= _options.sessionInfo_.Port();
	m_nSubDepth			= _options.sessionInfo_.SubDepth();
	m_bShowInfo			= _options.messageOptions_.ShowInfo();
	m_strCommandChar    = CString(CGlobals::GetCommandCharacter());
	if(m_strMaximize == "width")
	{
		m_bMaxWidth = TRUE;
		m_bMaxHeight = FALSE;
		m_bDockRight = FALSE;
	}
	if(m_strMaximize == "height")
	{
		m_bMaxWidth = FALSE;
		m_bMaxHeight = TRUE;
		m_bDockRight = FALSE;
	}
	if(m_strMaximize == "right")
	{
		m_bMaxWidth = FALSE;
		m_bMaxHeight = FALSE;
		m_bDockRight = TRUE;
	}

	UpdateData(FALSE);

	return TRUE;
}

void CSessionInfoDialog::OnOK()
{
	UpdateData(TRUE);

	_options.sessionInfo_.StartChatOnOpen(m_bChatServer ? true : false);
	_options.sessionInfo_.ProcessEvents(m_bEvents ? true : false);
	_options.sessionInfo_.IgnoreAliases(m_bIgnoreAliases ? true : false);
	_options.sessionInfo_.MaxOnOpen(m_bMaximize ? true : false);
	_options.sessionInfo_.Maximize(m_strMaximize);
	_options.sessionInfo_.PreSub(m_bPreSub ? true : false);
	_options.sessionInfo_.PromptOverwrite(m_bPromptOverwrite ? true : false);
	_options.sessionInfo_.Address(m_strAddress);
	_options.sessionInfo_.SessionName(m_strName);
	_options.sessionInfo_.SpeedWalk(m_bSpeedWalk ? true : false);
	_options.sessionInfo_.ShowStatusBar(m_bStatusBar ? true : false);
	_options.sessionInfo_.ScriptFilename(m_strSessionFile);
	_options.sessionInfo_.AutoLoadScript(m_bAutoLoadScript ? true : false);
	_options.sessionInfo_.AppendLog(m_bAppendLog ? true : false);
	_options.sessionInfo_.AutoLog(m_bAutoLog ? true : false);
	_options.sessionInfo_.LogFileName(m_strLogFileName);
	_options.sessionInfo_.Pace(m_nPace);
	_options.sessionInfo_.Port(m_nPort);
	_options.sessionInfo_.SubDepth(m_nSubDepth);
	_options.messageOptions_.ShowInfo(m_bShowInfo ? true : false);

	_options.sessionInfo_.Maximize(_T(""));
	
	CGlobals::SetCommandCharacter(m_strCommandChar.GetAt(0));
	if(m_bMaxWidth) 
	{
		_options.sessionInfo_.Maximize(_T("width"));
	}
	if(m_bMaxHeight)
	{
		_options.sessionInfo_.Maximize(_T("height"));
	}
	if(m_bDockRight)
	{
		_options.sessionInfo_.Maximize(_T("right"));
	}



	CPropertyPage::OnOK();
}
