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
#include "TriggerOptions.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

namespace SerializedOptions
{
	CTriggerOptions::CTriggerOptions()
		: m_bIgnoreTriggers(FALSE)
		, m_bIgnoreActions(FALSE)
		, m_bIgnoreGags(FALSE)
		, m_bIgnoreHighs(FALSE)
		, m_bIgnoreSubs(FALSE)
	{

	}

	CTriggerOptions::CTriggerOptions(const CTriggerOptions &src)
		: m_bIgnoreTriggers(src.m_bIgnoreTriggers)
		, m_bIgnoreActions(src.m_bIgnoreActions)
		, m_bIgnoreGags(src.m_bIgnoreGags)
		, m_bIgnoreHighs(src.m_bIgnoreHighs)
		, m_bIgnoreSubs(src.m_bIgnoreSubs)
	{
	}

	CTriggerOptions &CTriggerOptions::operator =(const CTriggerOptions &src)
	{
		CTriggerOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CTriggerOptions::Swap(CTriggerOptions &src)
	{
		swap(m_bIgnoreTriggers, src.m_bIgnoreTriggers);
		swap(m_bIgnoreActions, src.m_bIgnoreActions);
		swap(m_bIgnoreGags, src.m_bIgnoreGags);
		swap(m_bIgnoreHighs, src.m_bIgnoreHighs);
		swap(m_bIgnoreSubs, src.m_bIgnoreSubs);
	}

	HRESULT CTriggerOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_TRIGGERS_KEY, m_bIgnoreTriggers);
		PUT_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_ACTIONS_KEY,	m_bIgnoreActions);
		PUT_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_GAGS_KEY,		m_bIgnoreGags);
		PUT_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_HIGHS_KEY,	m_bIgnoreHighs);
		PUT_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_SUBS_KEY,		m_bIgnoreSubs);
		return S_OK;
	}

	HRESULT CTriggerOptions::Read(LPCTSTR lpszPathName)
	{
		m_bIgnoreTriggers	= GET_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_TRIGGERS_KEY,	FALSE);
		m_bIgnoreActions	= GET_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_ACTIONS_KEY,	FALSE);
		m_bIgnoreGags		= GET_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_GAGS_KEY,		FALSE);
		m_bIgnoreHighs		= GET_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_HIGHS_KEY,		FALSE);
		m_bIgnoreSubs		= GET_ENTRY_INT(TRIGGER_OPTIONS_SECTION, IGNORE_SUBS_KEY,		FALSE);
		return S_OK;
	}
}