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

class CIfxDoc;
class CSession;

namespace MMScript
{
	class CMMStatusBar;
	class CBarItem;
}

[event_receiver(native)]
class CStatusWindow 
	: public CWnd
{
public:
	CStatusWindow();

public:
	void SetStatusBarInfo(MMScript::CMMStatusBar *pStatusBar);
	void SetColorArrays(COLORREF *pFore, COLORREF *pBack);
	int DesiredHeight();
	void SetDoc(CIfxDoc *doc);
	void SetFont(CFont *pFont);
	void HideStatusBar();
	void RedrawStatusBar(CSession *pSession);
	void DrawItem(CSession *pSession, MMScript::CBarItem *pItem);
	void ResetStatusBar();
	virtual ~CStatusWindow();

	void SetForeColor(BYTE crFore);
	void SetBackColor(BYTE crBack);
	void UpdateAllOptions();
	void SetStatusBarLines(int iLines)			{m_iStatusBarLines = iLines;}
	void SetStatusBarWidth(int iWidth)			{m_iStatusBarWidth = iWidth;}

	void HookEvents(CSession *pSession);
	void UnHookEvents();

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	MMScript::CMMStatusBar *m_pStatusBarInfo;
	CSession *_pSession;

	BYTE m_crBackColor;
	BYTE m_crForeColor;

	COLORREF *m_pForeColors;
	COLORREF *m_pBackColors;
	int m_iStatusBarLines;
	int m_iStatusBarWidth;
	int m_iStatusBarCharWidth;
	int m_iStatusBarCharHeight;
	CBitmap * m_pBitmap;
	CFont * m_pFont;
	CPtrArray m_arStatusBarContents;
};