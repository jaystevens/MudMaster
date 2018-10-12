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
#include "MessTriggerOptionsPage.h"
#include "DefaultOptions.h"
#include ".\messtriggeroptionspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMessTriggerOptionsPage::CMessTriggerOptionsPage(COptions &options) 
: CPropertyPage(CMessTriggerOptionsPage::IDD)
, _options(options)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	//{{AFX_DATA_INIT(CMessTriggerOptionsPage)
	m_bAction = FALSE;
	m_bAlias = FALSE;
	m_bArray = FALSE;
	m_bAuto = FALSE;
	m_bEnable = FALSE;
	m_bEvent = FALSE;
	m_bGag = FALSE;
	m_bHigh = FALSE;
	m_bIgnoreAction = FALSE;
	m_bIgnoreAll = FALSE;
	m_bIgnoreGag = FALSE;
	m_bIgnoreHigh = FALSE;
	m_bIgnoreSub = FALSE;
	m_bList = FALSE;
	m_bListItem = FALSE;
	m_bMacro = FALSE;
	m_bStatusBarItem = FALSE;
	m_bSub = FALSE;
	m_bTab = FALSE;
	m_bVar = FALSE;
	//}}AFX_DATA_INIT
}

CMessTriggerOptionsPage::~CMessTriggerOptionsPage()
{
}

void CMessTriggerOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessTriggerOptionsPage)
	DDX_Control(pDX, IDC_IGNORE_SUBS, m_chkSub);
	DDX_Control(pDX, IDC_IGNORE_HIGHS, m_chkHigh);
	DDX_Control(pDX, IDC_IGNORE_GAGS, m_chkGag);
	DDX_Control(pDX, IDC_IGNORE_ALL, m_chkAll);
	DDX_Control(pDX, IDC_IGNORE_ACTIONS, m_chkAction);
	DDX_Check(pDX, IDC_ACTION_MESSAGES, m_bAction);
	DDX_Check(pDX, IDC_ALIAS_MESSAGES, m_bAlias);
	DDX_Check(pDX, IDC_ARRAY_MESSAGES, m_bArray);
	DDX_Check(pDX, IDC_AUTO_MESSAGES, m_bAuto);
	DDX_Check(pDX, IDC_ENABLE_MESSAGES, m_bEnable);
	DDX_Check(pDX, IDC_EVENT_MESSAGES, m_bEvent);
	DDX_Check(pDX, IDC_GAG_MESSAGES, m_bGag);
	DDX_Check(pDX, IDC_HIGH_MESSAGES, m_bHigh);
	DDX_Check(pDX, IDC_IGNORE_ACTIONS, m_bIgnoreAction);
	DDX_Check(pDX, IDC_IGNORE_ALL, m_bIgnoreAll);
	DDX_Check(pDX, IDC_IGNORE_GAGS, m_bIgnoreGag);
	DDX_Check(pDX, IDC_IGNORE_HIGHS, m_bIgnoreHigh);
	DDX_Check(pDX, IDC_IGNORE_SUBS, m_bIgnoreSub);
	DDX_Check(pDX, IDC_LIST_MESSAGES, m_bList);
	DDX_Check(pDX, IDC_LISTITEM_MESSAGES, m_bListItem);
	DDX_Check(pDX, IDC_MACRO_MESSAGES, m_bMacro);
	DDX_Check(pDX, IDC_STATUSBARITEM_MESSAGES, m_bStatusBarItem);
	DDX_Check(pDX, IDC_SUB_MESSAGES, m_bSub);
	DDX_Check(pDX, IDC_TAB_MESSAGES, m_bTab);
	DDX_Check(pDX, IDC_VAR_MESSAGE, m_bVar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessTriggerOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMessTriggerOptionsPage)
	ON_BN_CLICKED(IDC_IGNORE_ALL, OnIgnoreAll)
	ON_BN_CLICKED(IDC_IGNORE_ACTIONS, OnIgnoreActions)
	ON_BN_CLICKED(IDC_IGNORE_GAGS, OnIgnoreGags)
	ON_BN_CLICKED(IDC_IGNORE_HIGHS, OnIgnoreHighs)
	ON_BN_CLICKED(IDC_IGNORE_SUBS, OnIgnoreSubs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessTriggerOptionsPage message handlers

void CMessTriggerOptionsPage::OnIgnoreAll() 
{
	int nCheck = m_chkAll.GetCheck();
	m_chkAction.SetCheck(nCheck);
	m_chkGag.SetCheck(nCheck);
	m_chkHigh.SetCheck(nCheck);
	m_chkSub.SetCheck(nCheck);
}

void CMessTriggerOptionsPage::OnIgnoreActions() 
{
	int nCheck = m_chkAction.GetCheck();
	if(nCheck == 0)
		m_chkAll.SetCheck(0);
}

void CMessTriggerOptionsPage::OnIgnoreGags() 
{
	int nCheck = m_chkGag.GetCheck();
	if(nCheck == 0)
		m_chkAll.SetCheck(0);
}

void CMessTriggerOptionsPage::OnIgnoreHighs() 
{
	int nCheck = m_chkHigh.GetCheck();
	if(nCheck == 0)
		m_chkAll.SetCheck(0);
}

void CMessTriggerOptionsPage::OnIgnoreSubs() 
{
	int nCheck = m_chkSub.GetCheck();
	if(nCheck == 0)
		m_chkAll.SetCheck(0);
}

BOOL CMessTriggerOptionsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bAction			= _options.messageOptions_.ActionMessages();
	m_bAlias			= _options.messageOptions_.AliasMessages();
	m_bArray			= _options.messageOptions_.ArrayMessages();
	m_bAuto				= _options.messageOptions_.AutoexecMessages();
	m_bEnable			= _options.messageOptions_.EnableMessages();
	m_bEvent			= _options.messageOptions_.EventMessages();
	m_bGag				= _options.messageOptions_.GagMessages();
	m_bHigh				= _options.messageOptions_.HighMessages();
	m_bIgnoreAction		= _options.triggerOptions_.IgnoreActions();
	m_bIgnoreAll		= _options.triggerOptions_.IgnoreTriggers();
	m_bIgnoreGag		= _options.triggerOptions_.IgnoreGags();
	m_bIgnoreHigh		= _options.triggerOptions_.IgnoreHighs();
	m_bIgnoreSub		= _options.triggerOptions_.IgnoreSubs();
	m_bList				= _options.messageOptions_.ListMessages();
	m_bListItem			= _options.messageOptions_.ItemMessages();
	m_bMacro			= _options.messageOptions_.MacroMessages();
	m_bStatusBarItem	= _options.messageOptions_.BarMessages();
	m_bSub				= _options.messageOptions_.SubMessages();
	m_bTab				= _options.messageOptions_.TabMessages();
	m_bVar				= _options.messageOptions_.VariableMessages();

	UpdateData(FALSE);

	return TRUE;
}

void CMessTriggerOptionsPage::OnOK()
{
	UpdateData(TRUE);

	_options.messageOptions_.ActionMessages(m_bAction ? true : false);
	_options.messageOptions_.AliasMessages(m_bAlias ? true : false);
	_options.messageOptions_.ArrayMessages(m_bArray ? true : false);
	_options.messageOptions_.AutoexecMessages(m_bAuto ? true : false);
	_options.messageOptions_.EnableMessages(m_bEnable ? true : false);
	_options.messageOptions_.EventMessages(m_bEvent ? true : false);
	_options.messageOptions_.GagMessages(m_bGag ? true : false);
	_options.messageOptions_.HighMessages(m_bHigh ? true : false);
	_options.triggerOptions_.IgnoreActions(m_bIgnoreAction ? true : false);
	_options.triggerOptions_.IgnoreTriggers(m_bIgnoreAll ? true : false);
	_options.triggerOptions_.IgnoreGags(m_bIgnoreGag ? true : false);
	_options.triggerOptions_.IgnoreHighs(m_bIgnoreHigh ? true : false);
	_options.triggerOptions_.IgnoreSubs(m_bIgnoreSub ? true : false);
	_options.messageOptions_.ListMessages(m_bList ? true : false);
	_options.messageOptions_.ItemMessages(m_bListItem ? true : false);
	_options.messageOptions_.MacroMessages(m_bMacro ? true : false);
	_options.messageOptions_.BarMessages(m_bStatusBarItem ? true : false);
	_options.messageOptions_.SubMessages(m_bSub ? true : false);
	_options.messageOptions_.TabMessages(m_bTab ? true : false);
	_options.messageOptions_.VariableMessages(m_bVar ? true : false);

	CPropertyPage::OnOK();
}
