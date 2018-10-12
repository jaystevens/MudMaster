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
#include "BackColors.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MudmasterColors;

CBackColorsDlg::CBackColorsDlg(COptions &options, CWnd* pParent /*=NULL*/)
	: CDialog(CBackColorsDlg::IDD, pParent)
	, _options(options)
{
	m_nCurrent = 0;
	//{{AFX_DATA_INIT(CBackColors)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBackColorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackColors)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackColorsDlg, CDialog)
	//{{AFX_MSG_MAP(CBackColors)
	ON_BN_CLICKED(IDC_FORE_BLACK, OnForeBlack)
	ON_BN_CLICKED(IDC_FORE_BLUE, OnForeBlue)
	ON_BN_CLICKED(IDC_FORE_BROWN, OnForeBrown)
	ON_BN_CLICKED(IDC_FORE_CYAN, OnForeCyan)
	ON_BN_CLICKED(IDC_FORE_GRAY, OnForeGray)
	ON_BN_CLICKED(IDC_FORE_GREEN, OnForeGreen)
	ON_BN_CLICKED(IDC_FORE_MAGENTA, OnForeMagenta)
	ON_BN_CLICKED(IDC_FORE_RED, OnForeRed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackColors message handlers

BOOL CBackColorsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_btnBack[COLOR_BLACK].SubclassDlgItem(IDC_FORE_BLACK,this);
	m_btnBack[COLOR_BLUE].SubclassDlgItem(IDC_FORE_BLUE,this);
	m_btnBack[COLOR_GREEN].SubclassDlgItem(IDC_FORE_GREEN,this);
	m_btnBack[COLOR_CYAN].SubclassDlgItem(IDC_FORE_CYAN,this);
	m_btnBack[COLOR_RED].SubclassDlgItem(IDC_FORE_RED,this);
	m_btnBack[COLOR_MAGENTA].SubclassDlgItem(IDC_FORE_MAGENTA,this);
	m_btnBack[COLOR_BROWN].SubclassDlgItem(IDC_FORE_BROWN,this);
	m_btnBack[COLOR_GRAY].SubclassDlgItem(IDC_FORE_GRAY,this);
	
	for(int i = 0; i < MAX_BACK_COLORS; i++)
	{
		m_btnBack[i].SetColor(_options.terminalOptions_.TerminalWindowBackColors()[i]);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBackColorsDlg::OnForeBlack() 
{
	m_nCurrent = COLOR_BLACK;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeBlue() 
{
	m_nCurrent = COLOR_BLUE;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeBrown() 
{
	m_nCurrent = COLOR_BROWN;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeCyan() 
{
	m_nCurrent = COLOR_CYAN;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeGray() 
{
	m_nCurrent = COLOR_GRAY;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeGreen() 
{
	m_nCurrent = COLOR_GREEN;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeMagenta() 
{
	m_nCurrent = COLOR_MAGENTA;
	EndDialog(IDOK);
}

void CBackColorsDlg::OnForeRed() 
{
	m_nCurrent = COLOR_RED;
	EndDialog(IDOK);
}
