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
#include "ChatOptions.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static LPCTSTR CHAT_OPTIONS_SECTION = _T("Chat Options");
static LPCTSTR AUTOACCEPT_KEY		= _T("Auto Accept");
static LPCTSTR AUTOSERVE_KEY		= _T("Auto ChatServe");
static LPCTSTR DONOTDISTURB_KEY		= _T("Do Not Disturb");
static LPCTSTR CHAT_LISTEN_PORT_KEY	= _T("Chat Port");
static LPCTSTR CHAT_NAME_KEY		= _T("Chat Name");

using namespace std;

namespace SerializedOptions
{
	CChatOptions::CChatOptions()
		: m_bAutoAccept(TRUE)
		, m_bAutoServe(FALSE)
		, m_bDoNotDisturb(FALSE)
		, m_nListenPort(4050)
		, _chatName("mm2k6monkey")
	{
	}

	CChatOptions::CChatOptions(const CChatOptions & src)
		: m_bAutoAccept(src.m_bAutoAccept)
		, m_bAutoServe(src.m_bAutoServe)
		, m_bDoNotDisturb(src.m_bDoNotDisturb)
		, m_nListenPort(src.m_nListenPort)
		, _chatName(src._chatName)
	{

	}

	CChatOptions &CChatOptions::operator =(const CChatOptions &src)
	{
		CChatOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CChatOptions::Swap(CChatOptions &src)
	{
		swap(_chatName, src._chatName);
		swap(m_bAutoAccept, src.m_bAutoAccept);
		swap(m_bAutoServe, src.m_bAutoServe);
		swap(m_bDoNotDisturb, src.m_bDoNotDisturb);
		swap(m_nListenPort, src.m_nListenPort);
	}

	CChatOptions::~CChatOptions()
	{
	}

	HRESULT CChatOptions::Read(LPCTSTR lpszPathName)
	{
		m_bAutoAccept	= GET_ENTRY_INT(CHAT_OPTIONS_SECTION, AUTOACCEPT_KEY, TRUE);
		m_bAutoServe	= GET_ENTRY_INT(CHAT_OPTIONS_SECTION, AUTOSERVE_KEY, FALSE);
		m_bDoNotDisturb = GET_ENTRY_INT(CHAT_OPTIONS_SECTION, DONOTDISTURB_KEY, FALSE);
		m_nListenPort	= GET_ENTRY_INT(CHAT_OPTIONS_SECTION, CHAT_LISTEN_PORT_KEY, 4050);
		_chatName		= GET_ENTRY_STR(CHAT_OPTIONS_SECTION, CHAT_NAME_KEY, "mm2k6monkey");
		return S_OK;
	}

	HRESULT CChatOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(CHAT_OPTIONS_SECTION, AUTOACCEPT_KEY, m_bAutoAccept);
		PUT_ENTRY_INT(CHAT_OPTIONS_SECTION, AUTOSERVE_KEY, m_bAutoServe);
		PUT_ENTRY_INT(CHAT_OPTIONS_SECTION, DONOTDISTURB_KEY, m_bDoNotDisturb);
		PUT_ENTRY_INT(CHAT_OPTIONS_SECTION, CHAT_LISTEN_PORT_KEY, m_nListenPort);
		PUT_ENTRY_STR(CHAT_OPTIONS_SECTION, CHAT_NAME_KEY, _chatName.c_str());
		return S_OK;
	}
}