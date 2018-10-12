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
#include "TerminalOptionsPage.h"
#include "Colors.h"
#include "BackColors.h"
#include "ForeColorsDlg.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;

CTerminalOptionsPage::CTerminalOptionsPage(COptions &options) 
: CPropertyPage(CTerminalOptionsPage::IDD)
, _options(options)
, m_bTimestampLog(FALSE)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	m_term_fore = COLOR_GRAY;
	m_term_back = COLOR_BLACK;
	m_crInputBack = WINDOWS_WHITE;
	m_crHighBack = WINDOWS_GRAY;
	m_crHighFore = WINDOWS_BLACK;
	m_nMessBack = 0;
	m_nMessFore = 0;
	m_nStatBack = 0;
	m_nStatFore = 0;

	//{{AFX_DATA_INIT(CTerminalOptionsPage)
	m_bLocalEcho = FALSE;
	m_bTelnetGA = TRUE;
	m_bZmudRetain = FALSE;
	m_bShowBorder = FALSE;
	m_nSBBSize = 0;
	m_nCommHistBuffSize = 0;
	m_nLineLength = 200;
	m_bFullScreen = FALSE;
	//}}AFX_DATA_INIT
}

CTerminalOptionsPage::~CTerminalOptionsPage()
{
}

void CTerminalOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalOptionsPage)
	DDX_Control(pDX, IDC_CHB_SPNNER, m_spinCommandHistory);
	DDX_Control(pDX, IDC_SBB_SPNNER, m_spinScrollBack);
	DDX_Check(pDX, IDC_LOCAL_ECHO, m_bLocalEcho);
	DDX_Check(pDX, IDC_TELNET_GA, m_bTelnetGA);
	DDX_Check(pDX, IDC_ZMUD_RETENT, m_bZmudRetain);
	DDX_Check(pDX, IDC_SHOWBORDER, m_bShowBorder);
	DDX_Text(pDX, IDC_SCROLLBACK_BUFFER_SIZE, m_nSBBSize);
	DDX_Text(pDX, IDC_COMMAND_BUFFER_SIZE, m_nCommHistBuffSize);
	DDX_Text(pDX, IDC_LINE_LENGTH, m_nLineLength);
	DDX_Check(pDX, IDC_FULLSCREEN, m_bFullScreen);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_TIMESTAMP_LOG, m_bTimestampLog);
}


BEGIN_MESSAGE_MAP(CTerminalOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTerminalOptionsPage)
	ON_BN_CLICKED(IDC_TERM_FORECOLOR, OnTermForecolor)
	ON_BN_CLICKED(IDC_TERM_BACKCOLOR, OnTermBackcolor)
	ON_BN_CLICKED(IDC_INPUT_BACKCOLOR, OnInputBackcolor)
	ON_BN_CLICKED(IDC_TERM_FONT, OnTermFont)
	ON_BN_CLICKED(IDC_INPUT_FONT, OnInputFont)
	ON_BN_CLICKED(IDC_HIGHLIGHT, OnHighlight)
	ON_BN_CLICKED(IDC_HIGHLIGHTTEXT, OnHighlighttext)
	ON_BN_CLICKED(IDC_STAT_FORECOLOR, OnStatForecolor)
	ON_BN_CLICKED(IDC_STAT_BACKCOLOR, OnStatBackcolor)
	ON_BN_CLICKED(IDC_MESSAGE_FORE, OnMessageForecolor)
	ON_BN_CLICKED(IDC_MESSAGE_BACK, OnMessageBackcolor)
	ON_BN_CLICKED(IDC_STAT_FONT, OnStatFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalOptionsPage message handlers

BOOL CTerminalOptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_term_back         = _options.terminalOptions_.GetTerminalWindowBackColor();
	m_term_fore         = _options.terminalOptions_.GetTerminalWindowForeColor();
	m_crInputBack       = _options.inputOptions_.GetBackColor();
	m_crHighFore        = _options.terminalOptions_.GetHighFore();
	m_crHighBack        = _options.terminalOptions_.GetHighBack();
	m_bLocalEcho        = _options.terminalOptions_.LocalEcho();
	m_bTelnetGA         = _options.terminalOptions_.TelnetGA();
	m_bZmudRetain       = _options.inputOptions_.RetainCommands();
	m_bShowBorder       = _options.inputOptions_.ShowBorder();
	m_nSBBSize          = _options.terminalOptions_.GetTerminalWindowBufferSize();
	m_nCommHistBuffSize = _options.inputOptions_.CommandHistoryBufferSize();
	m_bFullScreen       = _options.terminalOptions_.FullScreenScrollback();
	m_nLineLength		= _options.terminalOptions_.GetTerminalLineLength();

	m_spinScrollBack.SetRange32(0, 100000);
	m_spinCommandHistory.SetRange32(0, 100000);

	if(m_term_fore > 15)
		m_term_fore = COLOR_GRAY;

	m_btnTermFore.SubclassDlgItem(IDC_TERM_FORECOLOR, this);
	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_term_fore];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnTermFore.DrawFlatFocus(TRUE);

	if(m_term_back > 7)
		m_term_back = COLOR_BLACK;
	m_btnTermBack.SubclassDlgItem(IDC_TERM_BACKCOLOR, this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_term_back];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnTermBack.DrawFlatFocus(TRUE);

	m_btnInputBackColor.SubclassDlgItem(IDC_INPUT_BACKCOLOR, this);
	r = (m_crInputBack & 0xFF);//GetRValue(m_crInputBack);
	g = ((m_crInputBack & 0xFF00) >> 8);//GetGValue(m_crInputBack);
	b = ((m_crInputBack & 0xFF0000) >> 16);//GetBValue(m_crInputBack);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crInputBack);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crInputBack);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crInputBack);
	m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnInputBackColor.DrawFlatFocus(TRUE);

	m_btnHighFore.SubclassDlgItem(IDC_HIGHLIGHTTEXT, this);
	r = GetRValue(m_crHighFore);
	g = GetGValue(m_crHighFore);
	b = GetBValue(m_crHighFore);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crHighFore);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crHighFore);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crHighFore);
	m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnHighFore.DrawFlatFocus(TRUE);

	m_btnHighBack.SubclassDlgItem(IDC_HIGHLIGHT, this);
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16); 

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crHighBack);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crHighBack);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crHighBack);
	m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnHighBack.DrawFlatFocus(TRUE);

	_options.terminalOptions_.GetTerminalWindowFont()->GetLogFont(&m_lfTerm);

	m_cfInputTemp.cbSize			= _options.inputOptions_.GetInputFormat()->cbSize;
	m_cfInputTemp.dwMask			= _options.inputOptions_.GetInputFormat()->dwMask;
	m_cfInputTemp.dwEffects			= _options.inputOptions_.GetInputFormat()->dwEffects;
	m_cfInputTemp.yHeight			= _options.inputOptions_.GetInputFormat()->yHeight;
	m_cfInputTemp.yOffset			= _options.inputOptions_.GetInputFormat()->yOffset;
	m_cfInputTemp.crTextColor		= _options.inputOptions_.GetInputFormat()->crTextColor;
	m_cfInputTemp.bCharSet			= _options.inputOptions_.GetInputFormat()->bCharSet;
	m_cfInputTemp.bPitchAndFamily	= _options.inputOptions_.GetInputFormat()->bPitchAndFamily;
	strcpy(m_cfInputTemp.szFaceName, _options.inputOptions_.GetInputFormat()->szFaceName);
	
	m_nMessBack = _options.colorOptions_.GetMessageBackColor();
	m_nMessFore = _options.colorOptions_.GetMessageForeColor();
	m_nStatBack = _options.statusOptions_.GetStatusBarBackColor();
	m_nStatFore = _options.statusOptions_.GetStatusBarForeColor();

	m_btnMessBack.SubclassDlgItem(IDC_MESSAGE_BACK,this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nMessBack];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnMessBack.DrawFlatFocus(TRUE);

	m_btnMessFore.SubclassDlgItem(IDC_MESSAGE_FORE,this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nMessFore];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnMessFore.DrawFlatFocus(TRUE);

	m_btnStatBack.SubclassDlgItem(IDC_STAT_BACKCOLOR,this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nStatBack];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnStatBack.DrawFlatFocus(TRUE);

	m_btnStatFore.SubclassDlgItem(IDC_STAT_FORECOLOR,this);
	cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nStatFore];
	r = (cr & 0xFF);
	g = ((cr & 0xFF00) >> 8);
	b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	crText = RGB(r,g,b);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	m_btnStatFore.DrawFlatFocus(TRUE);

	_options.statusOptions_.GetStatusBarFont()->GetLogFont(&m_lfStatusBar);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTerminalOptionsPage::OnTermForecolor() 
{
	CForeColorsDlg dlg(_options);

	dlg.m_nCurrent = m_term_fore;

	dlg.DoModal();

	m_term_fore = dlg.m_nCurrent;
	
	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_term_fore];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnTermFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnTermBackcolor() 
{
	CBackColorsDlg dlg(_options);

	dlg.m_nCurrent = m_term_back;

	dlg.DoModal();

	m_term_back = dlg.m_nCurrent;
	
	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_term_back];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnTermBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnInputBackcolor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg(m_crInputBack, CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR);
	
	if(dlg.DoModal() == IDOK)
	{
		m_crInputBack = dlg.GetColor();
		int r = GetRValue(m_crInputBack);
		int g = GetGValue(m_crInputBack);
		int b = GetBValue(m_crInputBack);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crInputBack);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crInputBack);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crInputBack);
		m_btnInputBackColor.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	}
	
}

void CTerminalOptionsPage::OnTermFont() 
{
	LOGFONT lf;
	memcpy(&lf, &m_lfTerm, sizeof(lf));

	CFontDialog dlg(&lf, CF_EFFECTS | CF_FIXEDPITCHONLY | CF_SCREENFONTS);

	if(dlg.DoModal() != IDOK)
		return;

	memcpy(&m_lfTerm, &lf, sizeof(m_lfTerm));
}

void CTerminalOptionsPage::OnInputFont() 
{
	CFontDialog dlg(m_cfInputTemp);

	if(dlg.DoModal() != IDOK)
		return;

	dlg.GetCharFormat(m_cfInputTemp);
}

void CTerminalOptionsPage::OnOK() 
{
	UpdateData(TRUE);

	_options.terminalOptions_.GetTerminalWindowFont()->DeleteObject();
	_options.terminalOptions_.GetTerminalWindowFont()->CreateFontIndirect(&m_lfTerm);

	_options.inputOptions_.GetInputFormat()->cbSize = m_cfInputTemp.cbSize;
	_options.inputOptions_.GetInputFormat()->dwMask = m_cfInputTemp.dwMask;
	_options.inputOptions_.GetInputFormat()->dwEffects = m_cfInputTemp.dwEffects;
	_options.inputOptions_.GetInputFormat()->yHeight = m_cfInputTemp.yHeight;
	_options.inputOptions_.GetInputFormat()->yOffset = m_cfInputTemp.yOffset;
	_options.inputOptions_.GetInputFormat()->crTextColor = m_cfInputTemp.crTextColor;
	_options.inputOptions_.GetInputFormat()->bCharSet = m_cfInputTemp.bCharSet;
	_options.inputOptions_.GetInputFormat()->bPitchAndFamily = m_cfInputTemp.bPitchAndFamily;
	strcpy(_options.inputOptions_.GetInputFormat()->szFaceName, m_cfInputTemp.szFaceName);

	_options.terminalOptions_.SetTerminalWindowBackColor(m_term_back);
	_options.terminalOptions_.SetTerminalWindowForeColor(m_term_fore);
	_options.inputOptions_.SetBackColor(m_crInputBack);
	_options.terminalOptions_.SetHighFore(m_crHighFore);
	_options.terminalOptions_.SetHighBack(m_crHighBack);

	_options.terminalOptions_.LocalEcho(m_bLocalEcho);
	_options.terminalOptions_.TelnetGA(m_bTelnetGA);
	_options.inputOptions_.RetainCommands(m_bZmudRetain);
	_options.inputOptions_.ShowBorder(m_bShowBorder);
	_options.terminalOptions_.SetTerminalWindowBufferSize(m_nSBBSize);
	_options.terminalOptions_.SetTerminalLineLength(m_nLineLength);
	_options.inputOptions_.CommandHistoryBufferSize(m_nCommHistBuffSize);
	_options.terminalOptions_.FullScreenScrollback(m_bFullScreen);

	_options.statusOptions_.GetStatusBarFont()->DeleteObject();
	_options.statusOptions_.GetStatusBarFont()->CreateFontIndirect(&m_lfStatusBar);

	_options.colorOptions_.SetMessageBackColor(m_nMessBack);
	_options.colorOptions_.SetMessageForeColor(m_nMessFore);
	_options.statusOptions_.SetStatusBarBackColor(m_nStatBack);
	_options.statusOptions_.SetStatusBarForeColor(m_nStatFore);

	CPropertyPage::OnOK();
}

void CTerminalOptionsPage::OnHighlight() 
{
	CColorDialog dlg(m_crHighBack, CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR);
	
	if(dlg.DoModal() == IDOK)
	{
		m_crHighBack = dlg.GetColor();
		int r = GetRValue(m_crHighBack);
		int g = GetGValue(m_crHighBack);
		int b = GetBValue(m_crHighBack);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crHighBack);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crHighBack);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crHighBack);
		m_btnHighBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	}
}

void CTerminalOptionsPage::OnHighlighttext() 
{
	CColorDialog dlg(m_crHighFore, CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR);
	
	if(dlg.DoModal() == IDOK)
	{
		m_crHighFore = dlg.GetColor();
		int r = GetRValue(m_crHighFore);
		int g = GetGValue(m_crHighFore);
		int b = GetBValue(m_crHighFore);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, m_crHighFore);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, m_crHighFore);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, m_crHighFore);
		m_btnHighFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
	}
}

void CTerminalOptionsPage::OnStatForecolor() 
{
	CForeColorsDlg dlg(_options, this);

	dlg.m_nCurrent = m_nStatFore;

	dlg.DoModal();

	m_nStatFore = dlg.m_nCurrent;

	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nStatFore];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnStatFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnStatBackcolor() 
{
	CBackColorsDlg dlg(_options, this);

	dlg.m_nCurrent = m_nStatBack;

	dlg.DoModal();

	m_nStatBack = dlg.m_nCurrent;

	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nStatBack];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnStatBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnMessageForecolor() 
{
	CForeColorsDlg dlg(_options, this);

	dlg.m_nCurrent = m_nMessFore;

	dlg.DoModal();

	m_nMessFore = dlg.m_nCurrent;

	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nMessFore];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnMessFore.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnMessageBackcolor() 
{
	CBackColorsDlg dlg(_options, this);

	dlg.m_nCurrent = m_nMessBack;

	dlg.DoModal();

	m_nMessBack = dlg.m_nCurrent;

	COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[m_nMessBack];
	int r = (cr & 0xFF);
	int g = ((cr & 0xFF00) >> 8);
	int b = ((cr & 0xFF0000) >> 16);

	r = ~r; g = ~g; b = ~b;

	COLORREF crText = RGB(r,g,b);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
	m_btnMessBack.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
}

void CTerminalOptionsPage::OnStatFont() 
{
	LOGFONT lf;

	memcpy(&lf, &m_lfStatusBar, sizeof(lf));

	CFontDialog dlg(&lf);

	if(dlg.DoModal() != IDOK)
		return;

	memcpy(&m_lfStatusBar, &lf, sizeof(m_lfStatusBar));
}
