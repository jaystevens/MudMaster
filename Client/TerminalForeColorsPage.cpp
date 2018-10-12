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
#include "TerminalForeColorsPage.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;
/////////////////////////////////////////////////////////////////////////////
// CTerminalForeColorsPage property page

CTerminalForeColorsPage::CTerminalForeColorsPage(COptions &options) 
: CPropertyPage(CTerminalForeColorsPage::IDD)
, _options(options)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CTerminalForeColorsPage::~CTerminalForeColorsPage()
{
}

void CTerminalForeColorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTerminalForeColorsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTerminalForeColorsPage)
	ON_BN_CLICKED(IDC_FORE_BLACK,			OnForeBlack)
	ON_BN_CLICKED(IDC_FORE_BLUE,			OnForeBlue)
	ON_BN_CLICKED(IDC_FORE_BROWN,			OnForeBrown)
	ON_BN_CLICKED(IDC_FORE_CYAN,			OnForeCyan)
	ON_BN_CLICKED(IDC_FORE_DARKGRAY,		OnForeDarkgray)
	ON_BN_CLICKED(IDC_FORE_GRAY,			OnForeGray)
	ON_BN_CLICKED(IDC_FORE_GREEN,			OnForeGreen)
	ON_BN_CLICKED(IDC_FORE_LIGHTBLUE,		OnForeLightblue)
	ON_BN_CLICKED(IDC_FORE_LIGHTCYAN,		OnForeLightcyan)
	ON_BN_CLICKED(IDC_FORE_LIGHTGREEN,		OnForeLightgreen)
	ON_BN_CLICKED(IDC_FORE_LIGHTMAGENTA,	OnForeLightmagenta)
	ON_BN_CLICKED(IDC_FORE_LIGHTRED,		OnForeLightred)
	ON_BN_CLICKED(IDC_FORE_MAGENTA,			OnForeMagenta)
	ON_BN_CLICKED(IDC_FORE_RED,				OnForeRed)
	ON_BN_CLICKED(IDC_FORE_WHITE,			OnForeWhite)
	ON_BN_CLICKED(IDC_FORE_YELLOW,			OnForeYellow)
	ON_BN_CLICKED(IDC_BACK_BLACK,			OnBackBlack)
	ON_BN_CLICKED(IDC_BACK_BLUE,			OnBackBlue)
	ON_BN_CLICKED(IDC_BACK_BROWN,			OnBackBrown)
	ON_BN_CLICKED(IDC_BACK_CYAN,			OnBackCyan)
	ON_BN_CLICKED(IDC_BACK_GRAY,			OnBackGray)
	ON_BN_CLICKED(IDC_BACK_GREEN,			OnBackGreen)
	ON_BN_CLICKED(IDC_BACK_MAGENTA,			OnBackMagenta)
	ON_BN_CLICKED(IDC_BACK_RED,				OnBackRed)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalForeColorsPage message handlers

BOOL CTerminalForeColorsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_btnFore[COLOR_BLACK].SubclassDlgItem(IDC_FORE_BLACK,this);
	m_btnFore[COLOR_BLUE].SubclassDlgItem(IDC_FORE_BLUE,this);
	m_btnFore[COLOR_GREEN].SubclassDlgItem(IDC_FORE_GREEN,this);
	m_btnFore[COLOR_CYAN].SubclassDlgItem(IDC_FORE_CYAN,this);
	m_btnFore[COLOR_RED].SubclassDlgItem(IDC_FORE_RED,this);
	m_btnFore[COLOR_MAGENTA].SubclassDlgItem(IDC_FORE_MAGENTA,this);
	m_btnFore[COLOR_BROWN].SubclassDlgItem(IDC_FORE_BROWN,this);
	m_btnFore[COLOR_GRAY].SubclassDlgItem(IDC_FORE_GRAY,this);
	m_btnFore[COLOR_DARKGRAY].SubclassDlgItem(IDC_FORE_DARKGRAY,this);
	m_btnFore[COLOR_LIGHTBLUE].SubclassDlgItem(IDC_FORE_LIGHTBLUE,this);
	m_btnFore[COLOR_LIGHTGREEN].SubclassDlgItem(IDC_FORE_LIGHTGREEN,this);
	m_btnFore[COLOR_LIGHTCYAN].SubclassDlgItem(IDC_FORE_LIGHTCYAN,this);
	m_btnFore[COLOR_LIGHTRED].SubclassDlgItem(IDC_FORE_LIGHTRED,this);
	m_btnFore[COLOR_LIGHTMAGENTA].SubclassDlgItem(IDC_FORE_LIGHTMAGENTA,this);
	m_btnFore[COLOR_YELLOW].SubclassDlgItem(IDC_FORE_YELLOW,this);
	m_btnFore[COLOR_WHITE].SubclassDlgItem(IDC_FORE_WHITE,this);

	for(int i = 0; i < MAX_FORE_COLORS; i++)
	{
		COLORREF cr = _options.terminalOptions_.TerminalWindowForeColors()[i];
		m_crTempForeColors[i] = cr;
		int r = (cr & 0xFF);
		int g = ((cr & 0xFF00) >> 8);
		int b = ((cr & 0xFF0000) >> 16);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
		m_btnFore[i].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
		m_btnFore[i].DrawFlatFocus(TRUE);
	}
	
	m_btnBack[COLOR_BLACK].SubclassDlgItem(IDC_BACK_BLACK,this);
	m_btnBack[COLOR_BLUE].SubclassDlgItem(IDC_BACK_BLUE,this);
	m_btnBack[COLOR_GREEN].SubclassDlgItem(IDC_BACK_GREEN,this);
	m_btnBack[COLOR_CYAN].SubclassDlgItem(IDC_BACK_CYAN,this);
	m_btnBack[COLOR_RED].SubclassDlgItem(IDC_BACK_RED,this);
	m_btnBack[COLOR_MAGENTA].SubclassDlgItem(IDC_BACK_MAGENTA,this);
	m_btnBack[COLOR_BROWN].SubclassDlgItem(IDC_BACK_BROWN,this);
	m_btnBack[COLOR_GRAY].SubclassDlgItem(IDC_BACK_GRAY,this);

	for(int i = 0; i < MAX_BACK_COLORS; i++)
	{
		COLORREF cr = _options.terminalOptions_.TerminalWindowBackColors()[i];
		m_crTempBackColors[i] = cr;
		int r = (cr & 0xFF);
		int g = ((cr & 0xFF00) >> 8);
		int b = ((cr & 0xFF0000) >> 16);

		r = ~r; g = ~g; b = ~b;

		COLORREF crText = RGB(r,g,b);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
		m_btnBack[i].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
		m_btnBack[i].DrawFlatFocus(TRUE);
	}

	return TRUE;
}

void CTerminalForeColorsPage::ColorPick(int nColor, bool back)
{
	CColorDialog dlg(
		m_crTempForeColors[nColor], 
		CC_FULLOPEN|CC_RGBINIT|CC_SOLIDCOLOR);
	
	if(dlg.DoModal() == IDOK)
	{
		if(back)
		{
			COLORREF cr = dlg.GetColor();
			m_crTempBackColors[nColor] = cr;
			int r = (cr & 0xFF);
			int g = ((cr & 0xFF00) >> 8);
			int b = ((cr & 0xFF0000) >> 16);

			r = ~r; g = ~g; b = ~b;

			COLORREF crText = RGB(r,g,b);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
			m_btnBack[nColor].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
		}
		else
		{
			COLORREF cr = dlg.GetColor();
			m_crTempForeColors[nColor] = cr;
			int r = (cr & 0xFF);
			int g = ((cr & 0xFF00) >> 8);
			int b = ((cr & 0xFF0000) >> 16);

			r = ~r; g = ~g; b = ~b;
			COLORREF crText = RGB(r,g,b);

			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_BK_IN, cr);
			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_FG_IN, crText);
			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_BK_OUT, cr);
			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_FG_OUT, crText);
			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, cr);
			m_btnFore[nColor].SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, crText);
		}
	}
}

void CTerminalForeColorsPage::OnForeBlack() 
{
	ColorPick(COLOR_BLACK, false);	
}

void CTerminalForeColorsPage::OnForeBlue() 
{
	ColorPick(COLOR_BLUE, false);		
}

void CTerminalForeColorsPage::OnForeBrown() 
{
	ColorPick(COLOR_BROWN, false);		
}

void CTerminalForeColorsPage::OnForeCyan() 
{
	ColorPick(COLOR_CYAN, false);		
}

void CTerminalForeColorsPage::OnForeDarkgray() 
{
	ColorPick(COLOR_DARKGRAY, false);	
}

void CTerminalForeColorsPage::OnForeGray() 
{
	ColorPick(COLOR_GRAY, false);	
}

void CTerminalForeColorsPage::OnForeGreen() 
{
	ColorPick(COLOR_GREEN, false);	
}

void CTerminalForeColorsPage::OnForeLightblue() 
{
	ColorPick(COLOR_LIGHTBLUE, false);	
}

void CTerminalForeColorsPage::OnForeLightcyan() 
{
	ColorPick(COLOR_LIGHTCYAN, false);	
}

void CTerminalForeColorsPage::OnForeLightgreen() 
{
	ColorPick(COLOR_LIGHTGREEN, false);	
}

void CTerminalForeColorsPage::OnForeLightmagenta() 
{
	ColorPick(COLOR_LIGHTMAGENTA, false);	
}

void CTerminalForeColorsPage::OnForeLightred() 
{
	ColorPick(COLOR_LIGHTRED, false);	
}

void CTerminalForeColorsPage::OnForeMagenta() 
{
	ColorPick(COLOR_MAGENTA, false);	
}

void CTerminalForeColorsPage::OnForeRed() 
{
	ColorPick(COLOR_RED, false);	
}

void CTerminalForeColorsPage::OnForeWhite() 
{
	ColorPick(COLOR_WHITE, false);	
}

void CTerminalForeColorsPage::OnForeYellow() 
{
	ColorPick(COLOR_YELLOW, false);	
}

void CTerminalForeColorsPage::OnBackBlack() 
{
	ColorPick(COLOR_BLACK, true);
}

void CTerminalForeColorsPage::OnBackBlue() 
{
	ColorPick(COLOR_BLUE, true);
}

void CTerminalForeColorsPage::OnBackBrown() 
{
	ColorPick(COLOR_BROWN, true);
}

void CTerminalForeColorsPage::OnBackCyan() 
{
	ColorPick(COLOR_CYAN, true);
}

void CTerminalForeColorsPage::OnBackGray() 
{
	ColorPick(COLOR_GRAY, true);
}

void CTerminalForeColorsPage::OnBackGreen() 
{
	ColorPick(COLOR_GREEN, true);
}

void CTerminalForeColorsPage::OnBackMagenta() 
{
	ColorPick(COLOR_MAGENTA, true);
}

void CTerminalForeColorsPage::OnBackRed() 
{
	ColorPick(COLOR_RED, true);
}

void CTerminalForeColorsPage::OnOK() 
{
	for(int i = 0; i < MAX_FORE_COLORS; i++)
		_options.terminalOptions_.TerminalWindowForeColors()[i] = m_crTempForeColors[i];

	for(int i = 0; i < MAX_BACK_COLORS; i++)
		_options.terminalOptions_.TerminalWindowBackColors()[i] = m_crTempBackColors[i];
	CPropertyPage::OnOK();
}

