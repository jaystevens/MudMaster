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
#include "MiscOptions.h"
#include "ForeColorsDlg.h"
#include "BackColors.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMiscOptionsPage::CMiscOptionsPage(COptions &options) 
: CPropertyPage(CMiscOptionsPage::IDD)
, _options(options)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	m_nChatBack = 0;
	m_nChatFore = 0;

	//{{AFX_DATA_INIT(CMiscOptions)
	m_bAutoAccept = FALSE;
	m_strChatName = _T("");
	m_bDND = FALSE;
	m_nPort = 0;
	m_bUseMSP = FALSE;
	m_bProcessFX = FALSE;
	m_bProcessMidi = FALSE;
	m_nSoundPlaybackMethod = -1;
	m_bShowMSPTriggers = FALSE;

	//}}AFX_DATA_INIT
}

CMiscOptionsPage::~CMiscOptionsPage()
{
}

void CMiscOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiscOptions)
	DDX_Check(pDX, IDC_AUTO_ACCEPT, m_bAutoAccept);
	DDX_Text(pDX, IDC_CHAT_NAME, m_strChatName);
	DDX_Check(pDX, IDC_DND, m_bDND);
	DDX_Text(pDX, IDC_CHAT_PORT, m_nPort);
	DDX_Control(pDX, IDC_DEBUG_MSP_TRIGGERS, m_cbShowMSPTriggers);
	DDX_Control(pDX, IDC_PROCESS_SOUND, m_cbProcessSound);
	DDX_Control(pDX, IDC_PROCESS_MUSIC, m_cbProcessMusic);
	DDX_Control(pDX, IDC_USE_MSP, m_cbUseMSP);
	DDX_Check(pDX, IDC_USE_MSP, m_bUseMSP);
	DDX_Check(pDX, IDC_PROCESS_SOUND, m_bProcessFX);
	DDX_Check(pDX, IDC_PROCESS_MUSIC, m_bProcessMidi);
	DDX_Radio(pDX, IDC_DX_80, m_nSoundPlaybackMethod);
	DDX_Check(pDX, IDC_DEBUG_MSP_TRIGGERS, m_bShowMSPTriggers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMiscOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMiscOptions)
	ON_BN_CLICKED(IDC_CHAT_FORECOLOR, OnChatForecolor)
	ON_BN_CLICKED(IDC_CHAT_BACKCOLOR, OnChatBackcolor)
	ON_BN_CLICKED(IDC_USE_MSP, OnUseMsp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiscOptions message handlers

BOOL CMiscOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_nChatBack = _options.colorOptions_.GetChatBackColor();
	m_nChatFore = _options.colorOptions_.GetChatForeColor();
	m_bAutoAccept = _options.chatOptions_.AutoAccept();
	m_strChatName = _options.chatOptions_.ChatName();
	m_bDND = _options.chatOptions_.DoNotDisturb();
	m_nPort = _options.chatOptions_.ListenPort();

	m_btnChatBack.SubclassDlgItem(IDC_CHAT_BACKCOLOR,this);
	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nChatBack];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnChatBack.DrawFlatFocus(TRUE);

	m_btnChatFore.SubclassDlgItem(IDC_CHAT_FORECOLOR,this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nChatFore];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnChatFore.DrawFlatFocus(TRUE);

	m_bUseMSP				= _options.mspOptions_.On();
	m_bProcessFX			= _options.mspOptions_.ProcessFX();
	m_bProcessMidi			= _options.mspOptions_.ProcessMidi();

	if(_options.mspOptions_.UsePlaySound())
		m_nSoundPlaybackMethod = 1;
	else
		m_nSoundPlaybackMethod = 0;

	m_bShowMSPTriggers		= _options.mspOptions_.ShowMSPTriggers();

	if(m_bUseMSP == FALSE)
	{
		m_cbProcessMusic.EnableWindow(FALSE);
		m_cbProcessSound.EnableWindow(FALSE);
		m_cbShowMSPTriggers.EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	return TRUE;
}

void CMiscOptionsPage::OnChatForecolor() 
{
	// TODO: Add your control notification handler code here
	CForeColorsDlg dlg(_options, this);

	if(IDOK == dlg.DoModal())
	{
		m_nChatFore = dlg.m_nCurrent;

		COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nChatFore];
		int r = (cr & 0xFF);
		int g = ((cr & 0xFF00) >> 8);
		int b = ((cr & 0xFF0000) >> 16);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
		m_btnChatFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	}
}

void CMiscOptionsPage::OnChatBackcolor() 
{
	CBackColorsDlg dlg(_options, this);

	dlg.m_nCurrent = m_nChatBack;

	if(IDOK == dlg.DoModal())
	{
		m_nChatBack = dlg.m_nCurrent;

		COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nChatBack];
		int r = (cr & 0xFF);
		int g = ((cr & 0xFF00) >> 8);
		int b = ((cr & 0xFF0000) >> 16);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
		m_btnChatBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	}	
}


void CMiscOptionsPage::OnUseMsp() 
{
	// TODO: Add your control notification handler code here
	if(m_cbUseMSP.GetCheck() == 0x0000)	//unchecked
	{
		m_cbProcessMusic.EnableWindow(FALSE);
		m_cbProcessSound.EnableWindow(FALSE);
		m_cbShowMSPTriggers.EnableWindow(FALSE);
		return;
	}

	if(m_cbUseMSP.GetCheck() == 0x0001) //checked
	{
		m_cbProcessMusic.EnableWindow(TRUE);
		m_cbProcessSound.EnableWindow(TRUE);
		m_cbShowMSPTriggers.EnableWindow(TRUE);
	}
}

void CMiscOptionsPage::OnOK() 
{
	UpdateData(TRUE);

	_options.colorOptions_.SetChatBackColor(m_nChatBack);
	_options.colorOptions_.SetChatForeColor(m_nChatFore);
	_options.chatOptions_.AutoAccept(m_bAutoAccept);
	_options.chatOptions_.ChatName(m_strChatName);
	_options.chatOptions_.DoNotDisturb(m_bDND);
	_options.chatOptions_.ListenPort(m_nPort);

	_options.mspOptions_.On(m_bUseMSP);
	_options.mspOptions_.ProcessFX(m_bProcessFX);
	_options.mspOptions_.ProcessMidi(m_bProcessMidi);
	_options.mspOptions_.UsePlaySound(m_nSoundPlaybackMethod == 1 ? TRUE : FALSE);
	_options.mspOptions_.ShowMSPTriggers(m_bShowMSPTriggers);

	CPropertyPage::OnOK();
}
