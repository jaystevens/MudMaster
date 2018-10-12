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
#include "StatusBarOptions.h"
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
	CStatusBarOptions::CStatusBarOptions()
		: m_crBack(DEFAULT_STATUSBAR_BACK_COLOR)
		, m_crFore(DEFAULT_STATUSBAR_FORE_COLOR)
		, m_bVisible(TRUE)
		, m_StatusPosition(STATUS_POS_BELOW)
		, m_iStatusBarLines(1)
		, m_iStatusBarWidth(160)
	{
		m_Font.CreateStockObject(ANSI_FIXED_FONT);
	}

	CStatusBarOptions::~CStatusBarOptions()
	{
		m_Font.DeleteObject();
	}

	CStatusBarOptions::CStatusBarOptions(const CStatusBarOptions &src)
		: m_crBack(src.m_crBack)
		, m_crFore(src.m_crFore)
		, m_bVisible(src.m_bVisible)
		, m_StatusPosition(src.m_StatusPosition)
		, m_iStatusBarLines(src.m_iStatusBarLines)
		, m_iStatusBarWidth(src.m_iStatusBarWidth)
	{
		LOGFONT lf;
		src.m_Font.GetLogFont(&lf);
		m_Font.CreateFontIndirect(&lf);
	}

	CStatusBarOptions &CStatusBarOptions::operator =(const CStatusBarOptions &src)
	{
		CStatusBarOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CStatusBarOptions::Swap(CStatusBarOptions &src)
	{
		swap(m_crBack, src.m_crBack);
		swap(m_crFore, src.m_crFore);
		swap(m_bVisible, src.m_bVisible);
		swap(m_StatusPosition, src.m_StatusPosition);
		swap(m_iStatusBarLines,src.m_iStatusBarLines);
		swap(m_iStatusBarWidth,src.m_iStatusBarWidth);
		LOGFONT lf_this;
		LOGFONT lf_src;

		m_Font.GetLogFont(&lf_this);
		src.m_Font.GetLogFont(&lf_src);

		m_Font.DeleteObject();
		src.m_Font.DeleteObject();

		m_Font.CreateFontIndirect(&lf_src);
		src.m_Font.CreateFontIndirect(&lf_this);
	}

	HRESULT CStatusBarOptions::Save(LPCTSTR lpszPathName)
	{
		LOGFONT lf;
		m_Font.GetLogFont(&lf);

		PUT_ENTRY_INT(SB_OPTIONS_SECTION, BACK_COLOR_KEY,	m_crBack);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, FORE_COLOR_KEY,	m_crFore);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, CHARSET_KEY,		lf.lfCharSet);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, CLIPPRES_KEY,		lf.lfClipPrecision);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, ESCAPE_KEY,		lf.lfEscapement);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, HEIGHT_KEY,		lf.lfHeight);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, ITALIC_KEY,		lf.lfItalic);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, ORIENTATION_KEY,	lf.lfOrientation);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, OUTPREC_KEY,		lf.lfOutPrecision);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, PITCHFAM_KEY,		lf.lfPitchAndFamily);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, QUALITY_KEY,		lf.lfQuality);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, STRIKEOUT_KEY,	lf.lfStrikeOut);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, UNDERLINE_KEY,	lf.lfUnderline);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, WEIGHT_KEY,		lf.lfWeight);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, WIDTH_KEY,		lf.lfWidth);
		PUT_ENTRY_STR(SB_OPTIONS_SECTION, FACE_KEY,			lf.lfFaceName);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, LINES_KEY,		m_iStatusBarLines);
		PUT_ENTRY_INT(SB_OPTIONS_SECTION, SB_WIDTH_KEY,		m_iStatusBarWidth);
		return S_OK;
	}

	HRESULT CStatusBarOptions::Read(LPCTSTR lpszPathName)
	{
		LOGFONT lf;
		m_Font.GetLogFont(&lf);

		m_crBack			= GET_ENTRY_INT(SB_OPTIONS_SECTION, BACK_COLOR_KEY,	DEFAULT_STATUSBAR_BACK_COLOR);
		m_crFore			= GET_ENTRY_INT(SB_OPTIONS_SECTION, FORE_COLOR_KEY,	DEFAULT_STATUSBAR_FORE_COLOR);
		lf.lfCharSet		= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, CHARSET_KEY,	255));
		lf.lfClipPrecision	= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, CLIPPRES_KEY,	2));
		lf.lfEscapement		= GET_ENTRY_INT(SB_OPTIONS_SECTION, ESCAPE_KEY,		0);
		lf.lfHeight			= GET_ENTRY_INT(SB_OPTIONS_SECTION, HEIGHT_KEY,		-19);
		lf.lfItalic			= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, ITALIC_KEY,		0));
		lf.lfOrientation	= GET_ENTRY_INT(SB_OPTIONS_SECTION, ORIENTATION_KEY,0);
		lf.lfOutPrecision	= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, OUTPREC_KEY,	1));
		lf.lfPitchAndFamily	= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, PITCHFAM_KEY,	49));
		lf.lfQuality		= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, QUALITY_KEY,	1));
		lf.lfStrikeOut		= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, STRIKEOUT_KEY,	0));
		lf.lfUnderline		= static_cast<BYTE>(GET_ENTRY_INT(SB_OPTIONS_SECTION, UNDERLINE_KEY,	0));
		lf.lfWeight			= GET_ENTRY_INT(SB_OPTIONS_SECTION, WEIGHT_KEY,		400);
		lf.lfWidth			= GET_ENTRY_INT(SB_OPTIONS_SECTION, WIDTH_KEY,		0);
		CString strFaceName = GET_ENTRY_STR(SB_OPTIONS_SECTION, FACE_KEY,		_T("Terminal"));
		strcpy(lf.lfFaceName, strFaceName);
		m_iStatusBarLines   = GET_ENTRY_INT(SB_OPTIONS_SECTION, LINES_KEY,		1);
		m_iStatusBarWidth   = GET_ENTRY_INT(SB_OPTIONS_SECTION, SB_WIDTH_KEY,	160);

		m_Font.DeleteObject();
		m_Font.CreateFontIndirect(&lf);
		return S_OK;
	}
}