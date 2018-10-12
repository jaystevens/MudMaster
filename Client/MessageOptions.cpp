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
#include "MessageOptions.h"
#include "Globals.h"
#include "Ifx.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

namespace SerializedOptions
{
	CMessageOptions::CMessageOptions()
		: m_bActionMessages(true)
		, m_bAliasMessages(true)
		, m_bArrayMessages(false)
		, m_bAutoMessages(false)
		, m_bBarMessages(true)
		, m_bEnableMessages(true)
		, m_bEventMessages(false)
		, m_bGagMessages(true)
		, m_bHighMessages(true)
		, m_bItemMessages(false)
		, m_bListMessages(false)
		, m_bMacroMessages(true)
		, m_bShowInfo(true)
		, m_bSubMessages(true)
		, m_bTabMessages(true)
		, m_bVarMessages(false)
	{
	}

	CMessageOptions::CMessageOptions(const CMessageOptions & src)
		: m_bActionMessages(src.m_bActionMessages)
		, m_bAliasMessages(src.m_bAliasMessages)
		, m_bArrayMessages(src.m_bArrayMessages)
		, m_bAutoMessages(src.m_bAutoMessages)
		, m_bBarMessages(src.m_bBarMessages)
		, m_bEnableMessages(src.m_bEnableMessages)
		, m_bEventMessages(src.m_bEventMessages)
		, m_bGagMessages(src.m_bGagMessages)
		, m_bHighMessages(src.m_bHighMessages)
		, m_bItemMessages(src.m_bItemMessages)
		, m_bListMessages(src.m_bListMessages)
		, m_bMacroMessages(src.m_bMacroMessages)
		, m_bShowInfo(src.m_bShowInfo)
		, m_bSubMessages(src.m_bSubMessages)
		, m_bTabMessages(src.m_bTabMessages)
		, m_bVarMessages(src.m_bVarMessages)
	{
	}

	CMessageOptions &CMessageOptions::operator =(const CMessageOptions &src)
	{
		CMessageOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CMessageOptions::Swap(CMessageOptions &src)
	{
		swap(m_bActionMessages, src.m_bActionMessages);
		swap(m_bAliasMessages, src.m_bAliasMessages);
		swap(m_bArrayMessages, src.m_bArrayMessages);
		swap(m_bAutoMessages, src.m_bAutoMessages);
		swap(m_bBarMessages, src.m_bBarMessages);
		swap(m_bEnableMessages, src.m_bEnableMessages);
		swap(m_bEventMessages, src.m_bEventMessages);
		swap(m_bGagMessages, src.m_bGagMessages);
		swap(m_bHighMessages, src.m_bHighMessages);
		swap(m_bItemMessages, src.m_bItemMessages);
		swap(m_bListMessages, src.m_bListMessages);
		swap(m_bMacroMessages, src.m_bMacroMessages);
		swap(m_bShowInfo, src.m_bShowInfo);
		swap(m_bSubMessages, src.m_bSubMessages);
		swap(m_bTabMessages, src.m_bTabMessages);
		swap(m_bVarMessages, src.m_bVarMessages);
	}

	HRESULT CMessageOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ACTION_MSG_KEY, m_bActionMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ALIAS_MSG_KEY, m_bAliasMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ARRAY_MSG_KEY, m_bArrayMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, AUTO_MSG_KEY, m_bAutoMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, BAR_MSG_KEY, m_bBarMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ENABLE_MSG_KEY, m_bEnableMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, EVENT_MSG_KEY, m_bEventMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, GAG_MSG_KEY, m_bGagMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, HIGH_MSG_KEY, m_bHighMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ITEM_MSG_KEY, m_bItemMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, LIST_MSG_KEY, m_bListMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, MACRO_MSG_KEY, m_bMacroMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, SHOW_INFO_KEY, m_bShowInfo);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, SUB_MSG_KEY, m_bSubMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, TAB_MSG_KEY, m_bTabMessages);
		PUT_ENTRY_INT(MESSAGE_OPTIONS_SECTION, VAR_MSG_KEY, m_bVarMessages);
		return S_OK;
	}

	HRESULT CMessageOptions::Read(LPCTSTR lpszPathName)
	{
		m_bActionMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ACTION_MSG_KEY,	FALSE) ? true : false;
		m_bAliasMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ALIAS_MSG_KEY,		FALSE) ? true : false;
		m_bArrayMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ARRAY_MSG_KEY,		FALSE) ? true : false;
		m_bAutoMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, AUTO_MSG_KEY,		FALSE) ? true : false;
		m_bBarMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, BAR_MSG_KEY,		FALSE) ? true : false;
		m_bEnableMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ENABLE_MSG_KEY,	FALSE) ? true : false;
		m_bEventMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, EVENT_MSG_KEY,		FALSE) ? true : false;
		m_bGagMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, GAG_MSG_KEY,		FALSE) ? true : false;
		m_bHighMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, HIGH_MSG_KEY,		FALSE) ? true : false;
		m_bItemMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, ITEM_MSG_KEY,		FALSE) ? true : false;
		m_bListMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, LIST_MSG_KEY,		FALSE) ? true : false;
		m_bMacroMessages	= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, MACRO_MSG_KEY,		FALSE) ? true : false;
		m_bShowInfo			= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, SHOW_INFO_KEY,		TRUE) ? true : false;
		m_bSubMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, SUB_MSG_KEY,		FALSE) ? true : false;
		m_bTabMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, TAB_MSG_KEY,		FALSE) ? true : false;
		m_bVarMessages		= GET_ENTRY_INT(MESSAGE_OPTIONS_SECTION, VAR_MSG_KEY,		FALSE) ? true : false;
		return S_OK;
	}

	void CMessageOptions::AllOff()
	{
		m_bActionMessages = FALSE;
		m_bAliasMessages = FALSE;
		m_bArrayMessages = FALSE;
		m_bAutoMessages = FALSE;
		m_bBarMessages = FALSE;
		m_bEnableMessages = FALSE;
		m_bEventMessages = FALSE;
		m_bGagMessages = FALSE;
		m_bHighMessages = FALSE;
		m_bItemMessages = FALSE;
		m_bListMessages = FALSE;
		m_bMacroMessages = FALSE;
		m_bSubMessages = FALSE;
		m_bTabMessages = FALSE;
		m_bVarMessages = FALSE;

	}
}