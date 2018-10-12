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
#include "ForeColorsDlg.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;

CForeColorsDlg::CForeColorsDlg(COptions &options, CWnd* pParent /*=NULL*/)
	: CDialog(CForeColorsDlg::IDD, pParent)
	, _options(options)
	, m_nCurrent(0)
{
}


void CForeColorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForeColorsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CForeColorsDlg, CDialog)
	//{{AFX_MSG_MAP(CForeColorsDlg)
	ON_BN_CLICKED(IDC_FORE_BLACK, OnForeBlack)
	ON_BN_CLICKED(IDC_FORE_BLUE, OnForeBlue)
	ON_BN_CLICKED(IDC_FORE_BROWN, OnForeBrown)
	ON_BN_CLICKED(IDC_FORE_CYAN, OnForeCyan)
	ON_BN_CLICKED(IDC_FORE_DARKGRAY, OnForeDarkgray)
	ON_BN_CLICKED(IDC_FORE_GRAY, OnForeGray)
	ON_BN_CLICKED(IDC_FORE_GREEN, OnForeGreen)
	ON_BN_CLICKED(IDC_FORE_LIGHTBLUE, OnForeLightblue)
	ON_BN_CLICKED(IDC_FORE_LIGHTCYAN, OnForeLightcyan)
	ON_BN_CLICKED(IDC_FORE_LIGHTGREEN, OnForeLightgreen)
	ON_BN_CLICKED(IDC_FORE_LIGHTMAGENTA, OnForeLightmagenta)
	ON_BN_CLICKED(IDC_FORE_LIGHTRED, OnForeLightred)
	ON_BN_CLICKED(IDC_FORE_MAGENTA, OnForeMagenta)
	ON_BN_CLICKED(IDC_FORE_RED, OnForeRed)
	ON_BN_CLICKED(IDC_FORE_WHITE, OnForeWhite)
	ON_BN_CLICKED(IDC_FORE_YELLOW, OnForeYellow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForeColorsDlg message handlers

BOOL CForeColorsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
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
		m_btnFore[i].SetColor(_options.terminalOptions_.TerminalWindowForeColors()[i]);
	}
	
	return TRUE;  
}

void CForeColorsDlg::OnForeBlack() 
{
	m_nCurrent = COLOR_BLACK;
	EndDialog(IDOK);	
}

void CForeColorsDlg::OnForeBlue() 
{
	m_nCurrent = COLOR_BLUE;
	EndDialog(IDOK);	
}

void CForeColorsDlg::OnForeBrown() 
{
	m_nCurrent = COLOR_BROWN;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeCyan() 
{
	m_nCurrent = COLOR_CYAN;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeDarkgray() 
{
	m_nCurrent = COLOR_DARKGRAY;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeGray() 
{
	m_nCurrent = COLOR_GRAY;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeGreen() 
{
	m_nCurrent = COLOR_GREEN;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeLightblue() 
{
	m_nCurrent = COLOR_LIGHTBLUE;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeLightcyan() 
{
	m_nCurrent = COLOR_LIGHTCYAN;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeLightgreen() 
{
	m_nCurrent = COLOR_LIGHTGREEN;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeLightmagenta() 
{
	m_nCurrent = COLOR_LIGHTMAGENTA;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeLightred() 
{
	m_nCurrent = COLOR_LIGHTRED;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeMagenta() 
{
	m_nCurrent = COLOR_MAGENTA;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeRed() 
{
	m_nCurrent = COLOR_RED;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeWhite() 
{
	m_nCurrent = COLOR_WHITE;
	EndDialog(IDOK);
}

void CForeColorsDlg::OnForeYellow() 
{
	m_nCurrent = COLOR_YELLOW;
	EndDialog(IDOK);
}
