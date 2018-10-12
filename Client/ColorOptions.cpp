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
#include "Resource.h"
#include "ColorOptions.h"
#include "Colors.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
using namespace MudmasterColors;

static LPCTSTR COLOR_OPTIONS_SECTION = _T("Color Options");
static LPCTSTR CHAT_FORE_KEY = _T("Chat Fore Color");
static LPCTSTR CHAT_BACK_KEY = _T("Chat Back Color");
static LPCTSTR MSG_FORE_KEY = _T("Msg Fore Color");
static LPCTSTR MSG_BACK_KEY = _T("Msg Back Color");

namespace SerializedOptions
{
	CColorOptions::CColorOptions()
		: m_nChatFore(COLOR_LIGHTRED)
		, m_nChatBack(COLOR_BLACK)
		, m_nMessageFore(COLOR_WHITE)
		, m_nMessageBack(COLOR_BLACK)
	{
	}

	CColorOptions::CColorOptions(const CColorOptions &src)
		: m_nChatFore(src.m_nChatFore)
		, m_nChatBack(src.m_nChatBack)
		, m_nMessageFore(src.m_nMessageFore)
		, m_nMessageBack(src.m_nMessageBack)
	{
	}

	CColorOptions &CColorOptions::operator =(const CColorOptions &src)
	{
		CColorOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CColorOptions::Swap(CColorOptions &src)
	{
		swap(m_nChatFore, src.m_nChatFore);
		swap(m_nChatBack, src.m_nChatBack);
		swap(m_nMessageFore, src.m_nMessageFore);
		swap(m_nMessageBack, src.m_nMessageBack);
	}

	HRESULT CColorOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(COLOR_OPTIONS_SECTION, CHAT_FORE_KEY, m_nChatFore);
		PUT_ENTRY_INT(COLOR_OPTIONS_SECTION, CHAT_BACK_KEY, m_nChatBack);
		PUT_ENTRY_INT(COLOR_OPTIONS_SECTION, MSG_FORE_KEY, m_nMessageFore);
		PUT_ENTRY_INT(COLOR_OPTIONS_SECTION, MSG_BACK_KEY, m_nMessageBack);
		return S_OK;
	}

	HRESULT CColorOptions::Read(LPCTSTR lpszPathName)
	{
		m_nChatFore = GET_ENTRY_INT(COLOR_OPTIONS_SECTION, CHAT_FORE_KEY, COLOR_LIGHTRED);
		m_nChatBack = GET_ENTRY_INT(COLOR_OPTIONS_SECTION, CHAT_BACK_KEY, COLOR_BLACK);
		m_nMessageFore = GET_ENTRY_INT(COLOR_OPTIONS_SECTION, MSG_FORE_KEY, COLOR_WHITE);
		m_nMessageBack = GET_ENTRY_INT(COLOR_OPTIONS_SECTION, MSG_BACK_KEY, COLOR_BLACK);
		return S_OK;
	}
}