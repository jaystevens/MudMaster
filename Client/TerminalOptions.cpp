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
#include "TerminalOptions.h"
#include "Colors.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;
using namespace std;

namespace SerializedOptions
{
	CTerminalOptions::CTerminalOptions()
		: m_bDoTelnetGA(TRUE)
		, m_bReconnectOnZap(TRUE)
		, m_bLocalEcho(TRUE)
		, m_nBufferSize(DEFAULT_BUFFER_SIZE)
		, m_crDefaultForeColor(COLOR_GRAY)
		, m_crDefaultBackColor(COLOR_BLACK)
		, m_crHighFore(WINDOWS_BLACK)
		, m_crHighBack(WINDOWS_GRAY)
		, m_bFullScreenScrollback(TRUE)
		, m_nLineLength(200)
	{
		m_TerminalWindowFont.CreateStockObject(ANSI_FIXED_FONT);

		for(int i = 0; i < MAX_FORE_COLORS; i++)
			m_crForeColors[i] = DEFAULT_COLORS[i];

		for(int i = 0; i < MAX_BACK_COLORS; i++)
			m_crBackColors[i] = DEFAULT_COLORS[i];
	}

	CTerminalOptions::CTerminalOptions(const CTerminalOptions &src)
		: m_bDoTelnetGA(src.m_bDoTelnetGA)
		, m_bReconnectOnZap(src.m_bReconnectOnZap)
		, m_bLocalEcho(src.m_bLocalEcho)
		, m_nBufferSize(src.m_nBufferSize)
		, m_crDefaultForeColor(src.m_crDefaultForeColor)
		, m_crDefaultBackColor(src.m_crDefaultBackColor)
		, m_crHighFore(src.m_crHighFore)
		, m_crHighBack(src.m_crHighBack)
		, m_bFullScreenScrollback(src.m_bFullScreenScrollback)
		, m_nLineLength(src.m_nLineLength)
	{
		LOGFONT lf;
		src.m_TerminalWindowFont.GetLogFont(&lf);
		m_TerminalWindowFont.CreateFontIndirect(&lf);

		for(int i = 0; i < MAX_FORE_COLORS; i++)
			m_crForeColors[i] = src.m_crForeColors[i];

		for(int i = 0; i < MAX_BACK_COLORS; i++)
			m_crBackColors[i] = src.m_crBackColors[i];
	}

	CTerminalOptions& CTerminalOptions::operator =(const CTerminalOptions &src)
	{
		CTerminalOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CTerminalOptions::Swap(CTerminalOptions &src)
	{
		swap(m_bDoTelnetGA, src.m_bDoTelnetGA);
		swap(m_bReconnectOnZap, src.m_bReconnectOnZap);
		swap(m_bLocalEcho, src.m_bLocalEcho);
		swap(m_nBufferSize, src.m_nBufferSize);
		swap(m_crDefaultForeColor, src.m_crDefaultForeColor);
		swap(m_crDefaultBackColor, src.m_crDefaultBackColor);
		swap(m_crHighFore, src.m_crHighFore);
		swap(m_crHighBack, src.m_crHighBack);
		swap(m_bFullScreenScrollback, src.m_bFullScreenScrollback);
		swap(m_nLineLength, src.m_nLineLength);
 
		LOGFONT lf_here;
		LOGFONT lf_there;

		m_TerminalWindowFont.GetLogFont(&lf_here);
		src.m_TerminalWindowFont.GetLogFont(&lf_there);

		m_TerminalWindowFont.DeleteObject();
		src.m_TerminalWindowFont.DeleteObject();

		swap(lf_here, lf_there);

		m_TerminalWindowFont.CreateFontIndirect(&lf_here);
		src.m_TerminalWindowFont.CreateFontIndirect(&lf_there);
	}

	CTerminalOptions::~CTerminalOptions()
	{
		m_TerminalWindowFont.DeleteObject();
	}

	HRESULT CTerminalOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, TELNET_GA_KEY,		m_bDoTelnetGA);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, UNZAP_KEY,			m_bReconnectOnZap);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, LOCAL_ECHO_KEY,		m_bLocalEcho);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BUFFER_SIZE_KEY,	m_nBufferSize);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, DEFAULT_FORE_KEY,	m_crDefaultForeColor);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, DEFAULT_BACK_KEY,	m_crDefaultBackColor);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, HIGH_FORE_KEY,		m_crHighFore);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, HIGH_BACK_KEY,		m_crHighBack);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FULL_SCREEN_SB,		m_bFullScreenScrollback);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, LINE_LENGTH_KEY,	m_nLineLength);

		LOGFONT lf;
		m_TerminalWindowFont.GetLogFont(&lf);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, CHARSET_KEY,		lf.lfCharSet);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, CLIPPRES_KEY,		lf.lfClipPrecision);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, ESCAPE_KEY,		lf.lfEscapement);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, HEIGHT_KEY,		lf.lfHeight);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, ITALIC_KEY,		lf.lfItalic);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, ORIENTATION_KEY,	lf.lfOrientation);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, OUTPREC_KEY,		lf.lfOutPrecision);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, PITCHFAM_KEY,		lf.lfPitchAndFamily);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, QUALITY_KEY,		lf.lfQuality);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, STRIKEOUT_KEY,	lf.lfStrikeOut);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, UNDERLINE_KEY,	lf.lfUnderline);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, WEIGHT_KEY,		lf.lfWeight);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, WIDTH_KEY,		lf.lfWidth);
		PUT_ENTRY_STR(TERM_OPTIONS_SECTION, FACE_KEY,			lf.lfFaceName);

		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_ONE_KEY,		m_crForeColors[0]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TWO_KEY,		m_crForeColors[1]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_THREE_KEY,		m_crForeColors[2]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FOUR_KEY,		m_crForeColors[3]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FIVE_KEY,		m_crForeColors[4]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SIX_KEY,		m_crForeColors[5]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SEVEN_KEY,		m_crForeColors[6]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_EIGHT_KEY,		m_crForeColors[7]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_NINE_KEY,		m_crForeColors[8]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TEN_KEY,		m_crForeColors[9]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_ELEVEN_KEY,	m_crForeColors[10]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TWELVE_KEY,	m_crForeColors[11]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_THIRTEEN_KEY,	m_crForeColors[12]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FOURTEEN_KEY,	m_crForeColors[13]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FIFTEEN_KEY,	m_crForeColors[14]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SIXTEEN_KEY,	m_crForeColors[15]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_ONE_KEY,		m_crBackColors[0]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_TWO_KEY,		m_crBackColors[1]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_THREE_KEY,		m_crBackColors[2]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_FOUR_KEY,		m_crBackColors[3]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_FIVE_KEY,		m_crBackColors[4]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_SIX_KEY,		m_crBackColors[5]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_SEVEN_KEY,		m_crBackColors[6]);
		PUT_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_EIGHT_KEY,		m_crBackColors[7]);

		return S_OK;
	}

	HRESULT CTerminalOptions::Read(LPCTSTR lpszPathName)
	{
		m_bDoTelnetGA			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, TELNET_GA_KEY,	TRUE);
		m_bReconnectOnZap		= GET_ENTRY_INT(TERM_OPTIONS_SECTION, UNZAP_KEY,		FALSE);
		m_bLocalEcho			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, LOCAL_ECHO_KEY,	TRUE);
		m_nBufferSize			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, BUFFER_SIZE_KEY,	DEFAULT_BUFFER_SIZE);
		m_crDefaultForeColor	= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, DEFAULT_FORE_KEY,	COLOR_GRAY));
		m_crDefaultBackColor	= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, DEFAULT_BACK_KEY,	COLOR_BLACK));
		m_crHighFore			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, HIGH_FORE_KEY,	WINDOWS_BLACK);
		m_crHighBack			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, HIGH_BACK_KEY,	WINDOWS_GRAY);
		m_bFullScreenScrollback = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FULL_SCREEN_SB,	FALSE);
		m_nLineLength			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, LINE_LENGTH_KEY,	200);

		LOGFONT lf = {0};
		lf.lfCharSet		= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, CHARSET_KEY,	255));
		lf.lfClipPrecision	= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, CLIPPRES_KEY,	2));
		lf.lfEscapement		= GET_ENTRY_INT(TERM_OPTIONS_SECTION, ESCAPE_KEY,		0);
		lf.lfHeight			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, HEIGHT_KEY,		-19);
		lf.lfItalic			= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, ITALIC_KEY,		0));
		lf.lfOrientation	= GET_ENTRY_INT(TERM_OPTIONS_SECTION, ORIENTATION_KEY,0);
		lf.lfOutPrecision	= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, OUTPREC_KEY,	1));
		lf.lfPitchAndFamily	= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, PITCHFAM_KEY,	49));
		lf.lfQuality		= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, QUALITY_KEY,	1));
		lf.lfStrikeOut		= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, STRIKEOUT_KEY,	0));
		lf.lfUnderline		= static_cast<BYTE>(GET_ENTRY_INT(TERM_OPTIONS_SECTION, UNDERLINE_KEY,	0));
		lf.lfWeight			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, WEIGHT_KEY,		400);
		lf.lfWidth			= GET_ENTRY_INT(TERM_OPTIONS_SECTION, WIDTH_KEY,		0);
		CString strFaceName = GET_ENTRY_STR(TERM_OPTIONS_SECTION, FACE_KEY,		_T("Terminal"));
		strcpy(lf.lfFaceName, strFaceName);
		if(!m_TerminalWindowFont.DeleteObject())
		{
			ASSERT(FALSE);
		}

		if(!m_TerminalWindowFont.CreateFontIndirect(&lf))
		{
			ASSERT(FALSE);
		}

		m_crForeColors[0] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_ONE_KEY,		DEFAULT_COLORS[0]);
		m_crForeColors[1] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TWO_KEY,		DEFAULT_COLORS[1]);
		m_crForeColors[2] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_THREE_KEY,		DEFAULT_COLORS[2]);
		m_crForeColors[3] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FOUR_KEY,		DEFAULT_COLORS[3]);
		m_crForeColors[4] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FIVE_KEY,		DEFAULT_COLORS[4]);
		m_crForeColors[5] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SIX_KEY,		DEFAULT_COLORS[5]);
		m_crForeColors[6] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SEVEN_KEY,		DEFAULT_COLORS[6]);
		m_crForeColors[7] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_EIGHT_KEY,		DEFAULT_COLORS[7]);
		m_crForeColors[8] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_NINE_KEY,		DEFAULT_COLORS[8]);
		m_crForeColors[9] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TEN_KEY,		DEFAULT_COLORS[9]);
		m_crForeColors[10] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_ELEVEN_KEY,	DEFAULT_COLORS[10]);
		m_crForeColors[11] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_TWELVE_KEY,	DEFAULT_COLORS[11]);
		m_crForeColors[12] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_THIRTEEN_KEY,	DEFAULT_COLORS[12]);
		m_crForeColors[13] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FOURTEEN_KEY,	DEFAULT_COLORS[13]);
		m_crForeColors[14] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_FIFTEEN_KEY,	DEFAULT_COLORS[14]);
		m_crForeColors[15] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, FORE_SIXTEEN_KEY,	DEFAULT_COLORS[15]);
		m_crBackColors[0] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_ONE_KEY,		DEFAULT_COLORS[0]);
		m_crBackColors[1] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_TWO_KEY,		DEFAULT_COLORS[1]);
		m_crBackColors[2] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_THREE_KEY,		DEFAULT_COLORS[2]);
		m_crBackColors[3] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_FOUR_KEY,		DEFAULT_COLORS[3]);
		m_crBackColors[4] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_FIVE_KEY,		DEFAULT_COLORS[4]);
		m_crBackColors[5] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_SIX_KEY,		DEFAULT_COLORS[5]);
		m_crBackColors[6] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_SEVEN_KEY,		DEFAULT_COLORS[6]);
		m_crBackColors[7] = GET_ENTRY_INT(TERM_OPTIONS_SECTION, BACK_EIGHT_KEY,		DEFAULT_COLORS[7]);

		return S_OK;
	}
}