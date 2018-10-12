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
#include "Ifx.h"
#include "OptionsSheet.h"

#include "TerminalForeColorsPage.h"
#include "MessTriggerOptionsPage.h"
#include "PathOptionsDlg.h"
#include "MiscOptions.h"
#include "TerminalOptionsPage.h"
#include "DefaultOptions.h"
#include "SessionInfoDialog.h"

struct COptionsSheet::OptionsSheetImpl
{
	CTerminalForeColorsPage _foreColorsPage;
	CMessTriggerOptionsPage _messTriggersPage;
	CPathOptionsDlg			_pathOptionsPage;
	CMiscOptionsPage		_miscOptionsPage;
	CTerminalOptionsPage	_terminalOptionsPage;
	CSessionInfoDialog		_sessionInfoPage;

	COptions				&_options;
	bool					_isDefault;

	OptionsSheetImpl(bool isDefault, COptions &options)
		: _options(options)
		, _foreColorsPage(options)
		, _messTriggersPage(options)
		, _miscOptionsPage(options)
		, _terminalOptionsPage(options)
		, _pathOptionsPage(isDefault, options)
		, _sessionInfoPage(options)
		, _isDefault(isDefault)
	{
	}
};

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)
COptionsSheet::COptionsSheet(
	bool isDefault,
	COptions &options,
	UINT nIDCaption, 
	CWnd* pParentWnd, 
	UINT iSelectPage)
	: CPropertySheet(
		nIDCaption, 
		pParentWnd, 
		iSelectPage)
	, _pimpl(new OptionsSheetImpl(isDefault, options))
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
	EnableStackedTabs(FALSE);
	AddPages();
}

COptionsSheet::~COptionsSheet()
{
	delete _pimpl;
}

void COptionsSheet::AddPages()
{
	AddPage(&_pimpl->_sessionInfoPage);
	AddPage(&_pimpl->_terminalOptionsPage);
	AddPage(&_pimpl->_foreColorsPage);
	AddPage(&_pimpl->_messTriggersPage);
	AddPage(&_pimpl->_miscOptionsPage);
	AddPage(&_pimpl->_pathOptionsPage);
}

BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
END_MESSAGE_MAP()
