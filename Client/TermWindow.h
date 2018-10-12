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
#if !defined(AFX_TERMWINDOW_H__848F6704_6C7D_11D5_BCDA_00E029482496__INCLUDED_)
#define AFX_TERMWINDOW_H__848F6704_6C7D_11D5_BCDA_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TermWindow.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CTermWindow window
class CMemDC;
class CAnsiParser;

#include "ColorLine.h"
#include "LineDef.h"
#include <list>
#include "IfxDoc.h"
#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CFile> LogFilePtr;
#include <atlimage.h>
#include <gdiplus.h>
#include "Telnet.h"

class CTermWindow : public CWnd
{
	friend class CSplitterBar;
// Construction
public:
	CTermWindow();
	virtual ~CTermWindow();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RebuildLineDefs();
	void ClearLineDefs();
	void RecalcSizes();
	void SetScrollSizes();
	void SetPrompt(BOOL bPrompt);
    BOOL GetPrompt();
    void CheckPromptAppend();
	void SetLogFile(LogFilePtr pLogFile);
	void SetLogging(BOOL bValue);

	BOOL ScrollHome();
	BOOL ScrollPageDown();
	BOOL ScrollPageUp();
	BOOL ScrollLineDown();
	BOOL ScrollLineUp();

	void SetBufferSize(int nBufferSize);
	void SetLineLength(int nLineLength);
	void SetColorArrays(COLORREF *pFore, COLORREF *pBack);

	void SaveScrollBack(LogFilePtr pLogFile);
	void SaveScrollBack(CString &strFileName);

	BOOL ResetAppendNext(CColorLine::SpColorLine &aLine);

	void ScrollCaret();
	void AppendNext(BOOL bValue);
	void EndScrollBack();
    void PrintLine(CColorLine::SpColorLine &line);
	void PrintLine(LPCSTR pszLine, BOOL bAppendNext, CAnsiParser &apParser, BYTE byFore, BYTE byBack);
	void Repaint();
	void AvertAppendNext();
	void SetForeColor(COLORREF crFore, BOOL bRedraw);
	void SetBackColor(COLORREF crBack, BOOL bRedraw);
	void SetTheFont(CFont *pFont, BOOL bRedraw);
	void InitCharHeight();
	void AddLine(CColorLine::SpColorLine &line);
	void TimestampLog();
    void Log(CString message);
    void SetTopBottomFlag(BOOL flag);
    void ScreenShot(CString &strFileName);

    void SetBackground(CImage *background_image);
	// variable used to track performance timing
	int elapsedTime;
	//LPCSTR GetNAWSmessage();
	// Generated message map functions
protected:
	//{{AFX_MSG(CTermWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditFind();
	afx_msg void OnDestroy();
#ifndef _WIN64
	afx_msg void OnTimer(UINT nIDEvent);
#else
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#endif

	afx_msg BOOL OnCreate(LPCREATESTRUCT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString CTermWindow::EncodeToUTF8(LPCTSTR szSource);
	void PaintHighlight(CMemDC &dc);
	void SetScrolling(BOOL bValue);
	void OnThumbTrack(int nPos);
	void UnloadHoldBuffer();
	void PutSelectionToClipboard(CString &strText);
	void CopySelection(CString &strSelection);
	void CopyANSISelection(CString &strSelection);
	void SetSelEnd(CPoint &point);
	void SetSelStart(CPoint &point);
	CPoint CharFromPos(CPoint &point);
	void AddLineDef(CColorLine::SpColorLine &line);
	void RemoveLineDefs(CColorLine::SpColorLine &line, BOOL bHead);
	void PaintScrollBackBorder(CMemDC &dc);
	BOOL LineOut(CMemDC &dc, char  *pszLine, CCharFormat *pColorBuffer, int nY, int nX, int nChars);
	void OrderPoints(CPoint &start, CPoint &end);
	int GetScrollOffset();
	BOOL IsScrolling();

	// TRUE if the logging is currently active
	BOOL m_bLogging;

	// TRUE if the last line in the buffer is a prompt
	BOOL m_bPrompt;

	// TRUE if the window is currently in scrollback mode.  This is only valid if m_bIsScrollbackWindow is TRUE
	BOOL m_bScrolling;

	// TRUE if there is currently a selection.  This is only important if m_bIsScrollbackWindow is TRUE
	BOOL m_bSelection;

	// The default background color of the window
	COLORREF m_crBackColor;

	// the default foreground color of the window
	COLORREF m_crForeColor;

	// the default highlight back color (this is no longer a necessary variable)
	COLORREF m_crHighBack;

	// the default highlight fore color (this is no longer a necessary variable)
	COLORREF m_crHighFore;

	// If the window is in scrollback then we want to dump new lines of text into the hold buffer
	typedef std::list<CColorLine::SpColorLine> BufferType;
	BufferType _holdBuffer;

	// The maximum number of lines to stuff into the scrollback buffer
	BufferType::size_type m_nBufferSize;

	// The highth of a character in pixels
	int m_nCharHeight;

	// the number of characters that will fit across the screen at its current width
	UINT m_nCharsWide;

	// the width of a character in pixels
	int m_nCharWidth;
	// line wrap length
	int m_nLineLength;

	// The number of lines of text that will fit down the screen
	UINT m_nLinesHigh;

	// the number of times that this window has been painted?
	int m_nPaintCount;

	// the width of the border that is painted around the scrollback window
	int m_nScrollBackBorderWidth;

	// an array of COLORREF values that indicate what color should be painted for each caracters color index
	COLORREF *m_pBackColors;

	// a pointer to the bitmap used for the back buffer during double buffering.
	 CBitmap *m_pBitmap;
	// a pointer to the bitmap used for the screenshots.
	 HBITMAP m_pScreenShot;
	HBITMAP m_Bitmap;
	// a storage bitmap that contains the background.
	CBitmap *m_pBackground;

	// Optional image for background painting.
	CImage  *m_pImage;

    // Is this window at the top or at the bottom. Used when splitting image.
    BOOL    m_bIsUpperWindow;

	// the currently selected font for painting
	CFont * m_pFont;

	// an array of COLORREF values that indicate what color should be painted for each characters color index
	COLORREF *m_pForeColors;

	// a list of line defs that represent each line as it would be painted on the screen
	typedef std::list<CLineDef::SpLineDef> LineDefBufferType;
	LineDefBufferType _lineDefs;

	// pointer to the file used for logging
	LogFilePtr _pLogFile;

	// a list of text lines as they were actually created.  This has no regard for the width of the screen
	BufferType _screenBuffer;

	// the end point of the highlighted selection
	CPoint m_ptSelEnd;

	// the start point of the highlighted selection
	CPoint m_ptSelStart;

	// the rect of the current client
	CRect m_rectClient;

	// if true all log entries will be prepended with a timestamp
	bool m_bTimestampLog;


	CClientDC *m_pDc;
	//pointer to document
	CIfxDoc *m_pDocument;
	void CTermWindow::SetDocument(CIfxDoc *pDoc);

	CIfxDoc * CTermWindow::GetDocument();
	BYTE NAWSmessage[10];
	void UpdateNAWSmessage();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMWINDOW_H__848F6704_6C7D_11D5_BCDA_00E029482496__INCLUDED_)
