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
#pragma once

#include "Colors.h"

namespace SerializedOptions
{
	class CTerminalOptions
	{
		static const DWORD DEFAULT_BUFFER_SIZE = 200;

	public:
		CTerminalOptions();
		CTerminalOptions(const CTerminalOptions &src);
		CTerminalOptions &operator=(const CTerminalOptions &src);
		~CTerminalOptions();

		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT Read(LPCTSTR lpszPathName);

		///////////////////////////////////////////////////////////////////
		// Terminal Window Settings
		///////////////////////////////////////////////////////////////////
		// Accessors
		int GetTerminalWindowBufferSize() {return m_nBufferSize;}
		int GetTerminalLineLength() {return m_nLineLength;}
		COLORREF * TerminalWindowBackColors() {return m_crBackColors;}
		COLORREF * TerminalWindowForeColors() {return m_crForeColors;}
		BYTE GetTerminalWindowForeColor() {return m_crDefaultForeColor;}
		BYTE GetTerminalWindowBackColor() {return m_crDefaultBackColor;}
		COLORREF GetHighFore()				  {return m_crHighFore;}
		COLORREF GetHighBack()				  {return m_crHighBack;}
		CFont * GetTerminalWindowFont() {return &m_TerminalWindowFont;}
		BOOL TelnetGA() {return m_bDoTelnetGA;}
		BOOL ReconnectOnZap() {return m_bReconnectOnZap;}
		BOOL LocalEcho() {return m_bLocalEcho;}
		BOOL TimeStampLog() {return m_bTimestampLog;}
		BOOL FullScreenScrollback() { return m_bFullScreenScrollback; }

		// Mutators
		void TelnetGA(BOOL bTelnetGA) {m_bDoTelnetGA = bTelnetGA;}
		void ReconnectOnZap(BOOL bReconnectOnZap) {m_bReconnectOnZap = bReconnectOnZap;}
		void SetTerminalWindowBufferSize(int nSize) {m_nBufferSize = nSize;}
		void SetTerminalLineLength(int nSize) {m_nLineLength = nSize;}
		void SetTerminalWindowFont(LOGFONT *plf)
		{
			m_TerminalWindowFont.DeleteObject();
			m_TerminalWindowFont.CreateFontIndirect(plf);
		}
		void LocalEcho(BOOL bLocalEcho) {m_bLocalEcho = bLocalEcho;}
		void SetTerminalWindowForeColor(BYTE crFore) {m_crDefaultForeColor = crFore;}
		void SetTerminalWindowBackColor(BYTE crBack) {m_crDefaultBackColor = crBack;}
		void SetHighBack(COLORREF crBack)				 {m_crHighBack = crBack;}
		void SetHighFore(COLORREF crFore)				 {m_crHighFore = crFore;}
		void TimeStampLog(BOOL bTimeStamp) {m_bTimestampLog = bTimeStamp;}
		void FullScreenScrollback(BOOL bFullScreenScrollback) { m_bFullScreenScrollback = bFullScreenScrollback;}
	private:
		void Swap(CTerminalOptions &src);

		int m_nBufferSize;
		int m_nLineLength;

		COLORREF m_crBackColors[MudmasterColors::MAX_BACK_COLORS];
		COLORREF m_crForeColors[MudmasterColors::MAX_FORE_COLORS];
		BYTE m_crDefaultForeColor;
		BYTE m_crDefaultBackColor;
		COLORREF m_crHighFore;
		COLORREF m_crHighBack;
		mutable CFont m_TerminalWindowFont; // tagged mutable because GetLogFont is not marked const
		BOOL m_bDoTelnetGA;
		BOOL m_bLocalEcho;
		BOOL m_bTimestampLog;
		BOOL m_bFullScreenScrollback;
		BOOL m_bReconnectOnZap;

	};
}