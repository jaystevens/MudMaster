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
#include "InputControlOptions.h"
#include <strsafe.h>
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SerializedOptions
{
	CInputControlOptions::CInputControlOptions()
		: m_bRetainCommands(FALSE)
		, m_crBackColor(DEFAULT_BACK_COLOR)
		, m_bShowBorder(FALSE)
		, m_nBufferSize(100)
	{
		ZeroMemory(&m_Font, sizeof(CHARFORMAT));
		m_Font.cbSize = sizeof(CHARFORMAT);
		m_Font.dwMask = CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_CHARSET | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_ITALIC;
		m_Font.dwEffects =  0;
		m_Font.yHeight = 200;
		m_Font.crTextColor = DEFAULT_TEXT_COLOR;
		m_Font.bCharSet = 255;
		m_Font.bPitchAndFamily = DEFAULT_PITCH_FAM;
		StringCbCopy(m_Font.szFaceName, LF_FACESIZE, DEFAULT_FACE_NAME);
	}

	CInputControlOptions::CInputControlOptions(const CInputControlOptions &src)
		: m_bRetainCommands(src.m_bRetainCommands)
		, m_crBackColor(src.m_crBackColor)
		, m_bShowBorder(FALSE)
		, m_nBufferSize(src.m_nBufferSize)
	{
		ZeroMemory(&m_Font, sizeof(CHARFORMAT));
		m_Font.cbSize = sizeof(CHARFORMAT);
		m_Font.dwMask = src.m_Font.dwMask;
		m_Font.dwEffects =  src.m_Font.dwEffects;
		m_Font.yHeight = src.m_Font.yHeight;
		m_Font.crTextColor = src.m_Font.crTextColor;
		m_Font.bCharSet = src.m_Font.bCharSet;
		m_Font.bPitchAndFamily = src.m_Font.bPitchAndFamily;
		StringCbCopy(m_Font.szFaceName, LF_FACESIZE, m_Font.szFaceName);
	}

	CInputControlOptions &CInputControlOptions::operator =(const CInputControlOptions &src)
	{
		CInputControlOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CInputControlOptions::Swap(CInputControlOptions &src)
	{
		std::swap(m_bRetainCommands, src.m_bRetainCommands);
		std::swap(m_crBackColor, src.m_crBackColor);
		std::swap(m_nBufferSize, src.m_nBufferSize);
		std::swap(m_Font, src.m_Font);
	}

	HRESULT CInputControlOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, RETAIN_COMMANDS_KEY,	m_bRetainCommands);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, BACK_COLOR_KEY,		m_crBackColor);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_EFFECTS_KEY,		m_Font.dwEffects);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_OFFSET_KEY,		m_Font.yOffset);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_TEXTCOLOR_KEY,	m_Font.crTextColor);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_CHARSET_KEY,		m_Font.bCharSet);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_PITCH_KEY,		m_Font.bPitchAndFamily);
		PUT_ENTRY_STR(INPUT_CONTROL_SECTION, FONT_FACENAME,			m_Font.szFaceName);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_SIZE_KEY,			m_Font.yHeight);
		PUT_ENTRY_INT(INPUT_CONTROL_SECTION, "Show Border",			m_bShowBorder);
		return S_OK;
	}

	HRESULT CInputControlOptions::Read(LPCTSTR lpszPathName)
	{
		m_bRetainCommands		= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	RETAIN_COMMANDS_KEY,	FALSE);
		m_bShowBorder			= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	"Show Border",	FALSE);
		m_crBackColor			= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	BACK_COLOR_KEY,			DEFAULT_BACK_COLOR);
		m_Font.dwEffects		= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	FONT_EFFECTS_KEY,		0);
		m_Font.yOffset			= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	FONT_OFFSET_KEY,		0);
		m_Font.crTextColor		= GET_ENTRY_INT(INPUT_CONTROL_SECTION,	FONT_TEXTCOLOR_KEY,		DEFAULT_TEXT_COLOR);
		m_Font.bCharSet			= static_cast<BYTE>(GET_ENTRY_INT(INPUT_CONTROL_SECTION,	FONT_CHARSET_KEY,		255));
		m_Font.bPitchAndFamily	= static_cast<BYTE>(GET_ENTRY_INT(INPUT_CONTROL_SECTION,	FONT_PITCH_KEY,			DEFAULT_PITCH_FAM));
		CString strFaceName = GET_ENTRY_STR(INPUT_CONTROL_SECTION,		FONT_FACENAME,		DEFAULT_FACE_NAME);
		StringCbCopy(m_Font.szFaceName, LF_FACESIZE, CT2A(strFaceName));
		m_Font.yHeight			= GET_ENTRY_INT(INPUT_CONTROL_SECTION, FONT_SIZE_KEY,			280);
		return S_OK;
	}

	void CInputControlOptions::SetInputFormat(CHARFORMAT &Font)
	{
		m_Font.dwEffects		= Font.dwEffects;
		m_Font.yHeight			= Font.yHeight; 
		m_Font.yOffset			= Font.yOffset; 
		m_Font.crTextColor		= Font.crTextColor; 
		m_Font.bCharSet			= Font.bCharSet; 
		m_Font.bPitchAndFamily	= Font.bPitchAndFamily; 
		StringCbCopy(m_Font.szFaceName, LF_FACESIZE, Font.szFaceName);
	}
}