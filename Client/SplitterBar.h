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

#include "TermWindow.h"
#include "TerminalOptions.h"
#include <atlimage.h>

#define TERM_WINDOW_BACKGROUND_NONE     0
#define TERM_WINDOW_BACKGROUND_NORMAL   1
#define TERM_WINDOW_BACKGROUND_CENTER   2
#define TERM_WINDOW_BACKGROUND_STRETCH  3
#define TERM_WINDOW_BACKGROUND_TILE     4
#define TERM_WINDOW_BACKGROUND_MIDRIGHT 5
#define TERM_WINDOW_BACKGROUND_BOTTOMRIGHT 6
#define TERM_WINDOW_BACKGROUND_TOPRIGHT 7

[event_source(native)]
class CSplitterBar : public CWnd
{
// Construction
public:
	CSplitterBar();

// Attributes
public:

// Operations
public:
	void SetTheFont(CFont *pFont, BOOL bRedraw);
	void SetBackColor(COLORREF crBack, BOOL bRedraw);
	void SetForeColor(COLORREF crFore, BOOL bRedraw);
	void SetColorArrays(COLORREF *pFore, COLORREF *pBack);
	void SetBufferSize(int nBufferSize);
	void SetLineLength(int nLineLength);
	void SetDisplayBufferSize(int nBufferSize);
	void AvertAppendNext();
	void SetLogging(BOOL bValue);
	void PrintLine(LPCSTR pszLine, BOOL bAppendNext, CAnsiParser &apParser,BYTE byFore, BYTE byBack);
	void Repaint();
	void AppendNext(BOOL bValue);
	void ScrollCaret();
	BOOL ResetAppendNext(CColorLine::SpColorLine &line);
	BOOL SetBackground(CString &strFilename, CString &strMode);
	BOOL CSplitterBar::ScreenShot(CString &strFilename);
	void PrintLine(CColorLine::SpColorLine &line);
	void SetPrompt(BOOL bPrompt);
    BOOL GetPrompt();
    void CheckPromptAppend();
	void SaveScrollBack(CString &strFileName);
	void ScrollHome();
	void ClearScreen();
	void ScrollPageDown();
	void ScrollPageUp();
	void ScrollLineDown();
	void ScrollLineUp();
	void EndScrollBack();
	void SaveScrollBack(LogFilePtr pLogFile);
	void SetLogFile(LogFilePtr pLogFile);
	void TimestampLog();
	void ScrollGrep(ULONG nLines, const CString &strSearchCriteria);
	void UpdateAllOptions(SerializedOptions::CTerminalOptions &options);
	//LPCSTR CSplitterBar::GetNAWSmessage();
	__event void ErrorMessage(const CString &errorMessage);
	__event void ScrollGrepMatch(const CStringList &matchString);

	BOOL IsFullScreenScrollback();
	void SetFullScreenScrollback(BOOL bFullscreen);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CIfxDoc * GetDocument();
	void SetDocument(CIfxDoc *pDoc);
	virtual ~CSplitterBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitterBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void CloseSplitterWindow();
	void OpenSplitterWindow();
	void DrawSplitter(CDC *dc=NULL);
	void RecalcLayout();
	void StopTracking();
	void OnInvertTracker();
	void StartTracking();
    void RecreateBitmap();
	BOOL m_bTracking;
	HCURSOR m_hCurPrev;

	CRect m_rectSplitter;
	CRect m_rectScrollback;
	CRect m_rectTerminal;
	CRect m_rectTracker;
	//KW track old rectangle to be able to decide not to rebuild linedefs
	CRect m_oldRectScrollback;

	CTermWindow m_wndTerm;
	CTermWindow m_wndScrollback;
	CIfxDoc    *m_pDocument;

	CImage      m_imgBackground;    // Background image (Original image + stretch,tile operation applied)
    CImage      m_imgOriginal;      // Original image
    int         m_nBackgroundMode;

	DWORD m_clrBtnFace;
	DWORD m_clrWindowFrame;
	DWORD m_clrBtnHilite;
	DWORD m_clrBtnShadow;
	CWnd *m_pOldWnd;

	BOOL m_bFullScreenScrollback;

	double m_nSplitterPercent;
};
