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
// SplitterBar.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "SplitterBar.h"
#include "MemDC.h"
#include "IfxDoc.h"
#include "Sess.h"
#include "ColorLine.h"
#include "LineDef.h"
#include "TerminalOptions.h"
#include "Globals.h"
#include "ConfigFileConstants.h"
#include "QPerformanceTimer.h"

#pragma warning(push)
#pragma warning(disable : 4389 4018 4701)
#include <atlrx.h>
#pragma warning(pop)
#include "boost/regex/mfc.hpp"
#include "boost/regex.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterBar
static LPCTSTR FULL_SCREEN_KEY = _T("Full Screen Scrollback");
static LPCTSTR SCROLLBACK_HEIGHT_KEY = _T("Scroll Back Height");

CSplitterBar::CSplitterBar() : 
m_bTracking(FALSE),
m_hCurPrev(NULL),
m_pDocument(NULL),
m_clrBtnFace(0),
m_clrWindowFrame(0),
m_clrBtnHilite(0),
m_clrBtnShadow(0),
m_nBackgroundMode(TERM_WINDOW_BACKGROUND_NONE),
m_pOldWnd(NULL)
{
	m_rectSplitter.SetRectEmpty();
	m_rectScrollback.SetRectEmpty();
	m_rectTerminal.SetRectEmpty();

   m_wndScrollback.SetTopBottomFlag(true);
   m_wndTerm.SetTopBottomFlag(false);

	BOOL bFullScroll = FALSE;

	CString dir = CGlobals::GetConfigFilename();

	bFullScroll = ::GetPrivateProfileInt(
		DEFAULT_SECTION_NAME,
		FULL_SCREEN_KEY,
		FALSE,
		dir );

	m_bFullScreenScrollback = bFullScroll;

	int nHeight = ::GetPrivateProfileInt(
		DEFAULT_SECTION_NAME,
		SCROLLBACK_HEIGHT_KEY,
		60,
		dir );
		
	m_nSplitterPercent = (double)((double)nHeight / 100.0);
}

CSplitterBar::~CSplitterBar()
{
	if(!m_imgBackground.IsNull()) {
		m_imgBackground.Destroy();
		m_wndTerm.m_pImage       = NULL;	
		m_wndScrollback.m_pImage = NULL;
	}

    if(!m_imgOriginal.IsNull()) m_imgOriginal.Destroy();
}


BEGIN_MESSAGE_MAP(CSplitterBar, CWnd)
	//{{AFX_MSG_MAP(CSplitterBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterBar message handlers

void CSplitterBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if(m_rectSplitter.Height() > 0)	DrawSplitter(&dc);
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CSplitterBar::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

BOOL CSplitterBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(nHitTest == HTCLIENT && pWnd == this && !m_bTracking)
		return TRUE;

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CSplitterBar::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	if(IsFullScreenScrollback())
		return;

	if(GetCapture() != this)
		StopTracking();

	CRect rectClient;
	GetClientRect(&rectClient);

	if(m_bTracking)
	{
		if(point.y < 0)
			point.y = 0;

		if(point.y > (rectClient.bottom-7))
			point.y = rectClient.bottom-7;

		if(m_rectTracker.top != point.y)
		{
			OnInvertTracker();
			m_rectTracker.top    = point.y;
			m_rectTracker.bottom = m_rectTracker.top + 7;
			m_rectTracker.left   = rectClient.left;
			m_rectTracker.right  = rectClient.right;
			OnInvertTracker();
		}
	}
	else
	{
		if(m_rectSplitter.PtInRect(point))
		{
			HCURSOR hCur = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SPLITTER));
			m_hCurPrev = SetCursor(hCur);
			//DestroyCursor(hCur);
		}
		else
		{
			m_hCurPrev = SetCursor(m_hCurPrev);
			DestroyCursor(m_hCurPrev);
			m_hCurPrev = NULL;
		}
	}
	//CWnd::OnMouseMove(nFlags, point);
}

void CSplitterBar::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	if(m_bTracking)
		return;

	if(IsFullScreenScrollback())
		return;

	StartTracking();
}

void CSplitterBar::StartTracking()
{
	SetCapture();
	m_pOldWnd = SetFocus();

	m_bTracking = TRUE;
	m_rectTracker = m_rectSplitter;
	OnInvertTracker();
}

void CSplitterBar::OnInvertTracker()
{
	CDC *pDC = GetDC();

	CBrush *pBrush = CDC::GetHalftoneBrush();
	HBRUSH hOldBrush = NULL;
	if(pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);
	pDC->PatBlt(m_rectTracker.left, m_rectTracker.top, m_rectTracker.Width(), m_rectTracker.Height(), PATINVERT);
	if(hOldBrush != NULL)
		SelectObject(pDC->m_hDC, hOldBrush);
	ReleaseDC(pDC);
}

void CSplitterBar::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	if(IsFullScreenScrollback())
		return;

	StopTracking();
}

void CSplitterBar::StopTracking()
{
	if(!m_bTracking)
		return;

	ReleaseCapture();
	OnInvertTracker();
	m_bTracking           = FALSE;
	m_rectSplitter.top    = m_rectTracker.top;
	m_rectSplitter.bottom = m_rectSplitter.top+7;

	CRect rectClient;
	GetClientRect(&rectClient);

	if(rectClient.bottom == 0)
		m_nSplitterPercent = 0;
	else
		m_nSplitterPercent = (double)m_rectSplitter.top / (double)rectClient.bottom;

	//TODO: KW fix the scrollback percent output to ini file
	// converting to int allows correct conversion to string
	int iHeightPercent = int(m_nSplitterPercent * 100);
	CString strHeight;
	strHeight.Format(_T("%d"),iHeightPercent);

	CString dir = CGlobals::GetConfigFilename();
	::WritePrivateProfileString(DEFAULT_SECTION_NAME,SCROLLBACK_HEIGHT_KEY, strHeight, dir);

	if(m_pOldWnd != NULL)
		m_pOldWnd->SetFocus();

	SetFullScreenScrollback(FALSE);
	DrawSplitter();
	RecalcLayout();
}

int CSplitterBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!m_wndTerm.Create(
		NULL,
		((CIfxApp *)AfxGetApp())->GetTerminalWindowClassName(),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0,0,100,100),
		this,
		TERM_WINDOW_ID,
		NULL))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	m_wndTerm.SetDocument(GetDocument());

	if(!m_wndScrollback.Create(
		NULL,
		((CIfxApp *)AfxGetApp())->GetTerminalWindowClassName(),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0,0,100,100),
		this,
		TERM_WINDOW_ID,
		NULL))
	{
		ASSERT(FALSE);
		return FALSE;
	}
	m_wndScrollback.SetDocument(GetDocument());
	m_rectSplitter.top=0;
	m_rectSplitter.left=0;
	m_rectSplitter.right = 100;
	m_rectSplitter.bottom = 7;

	m_clrBtnFace		= ::GetSysColor(COLOR_BTNFACE);
	m_clrWindowFrame	= ::GetSysColor(COLOR_WINDOWFRAME);
	m_clrBtnHilite		= ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrBtnShadow		= ::GetSysColor(COLOR_BTNSHADOW);

	return 0;
}

void CSplitterBar::SetDocument(CIfxDoc *pDoc)
{
	m_pDocument = pDoc;
}

CIfxDoc * CSplitterBar::GetDocument()
{
	return m_pDocument;
}

void CSplitterBar::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/) 
{
	if(m_wndTerm.GetSafeHwnd()==NULL)       return;
	if(m_wndScrollback.GetSafeHwnd()==NULL)	return;

	RecalcLayout();
	m_wndScrollback.SetScrollSizes();
	m_wndScrollback.ScrollCaret();
	m_wndTerm.ScrollCaret();	
    RecreateBitmap();
}

void CSplitterBar::RecalcLayout()
{
	CRect rectClient;
	GetClientRect(&rectClient);

	if(m_rectSplitter.bottom > rectClient.bottom)
		m_rectSplitter.bottom = rectClient.bottom;

	if(m_rectSplitter.bottom < 7)
		m_rectSplitter.bottom = 7;

	if(m_bFullScreenScrollback)
		m_rectSplitter.bottom = rectClient.bottom;

	m_rectSplitter.top = m_rectSplitter.bottom - 7;

	if(m_bFullScreenScrollback)
		m_rectSplitter.top = rectClient.bottom;

	m_rectSplitter.left = rectClient.left;
	m_rectSplitter.right = rectClient.right;

	m_rectTerminal.top = m_rectSplitter.bottom;
	m_rectTerminal.left = rectClient.left;
	m_rectTerminal.right = rectClient.right;
	m_rectTerminal.bottom = rectClient.bottom;

	m_rectScrollback.top=rectClient.top;
	m_rectScrollback.left=rectClient.left;
	m_rectScrollback.right=rectClient.right;
	m_rectScrollback.bottom = m_rectSplitter.top;

	m_wndTerm.MoveWindow(m_rectTerminal);
	m_wndScrollback.MoveWindow(m_rectScrollback);
	m_wndScrollback.RecalcSizes();
	m_wndTerm.RecalcSizes();

	//TODO: KW Watch that this is okay...compare old width to new width
	if(m_oldRectScrollback.Width() != m_rectScrollback.Width())
		m_wndScrollback.RebuildLineDefs();
	m_oldRectScrollback = m_rectScrollback;
	m_wndTerm.RebuildLineDefs();

	m_wndScrollback.SetScrollSizes();

//	if(m_rectSplitter.Height() > 0)
		Invalidate();
}

void CSplitterBar::DrawSplitter(CDC *dc/*=NULL*/)
{
	if (dc == NULL)
	{
		RedrawWindow(m_rectSplitter, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}

	CRect rect;
	rect = m_rectSplitter;
	dc->Draw3dRect(rect, m_clrBtnFace, m_clrWindowFrame);
	rect.InflateRect(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
	dc->Draw3dRect(rect, m_clrBtnHilite, m_clrBtnShadow);
	rect.InflateRect(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
	dc->FillSolidRect(rect, m_clrBtnFace);
}

void CSplitterBar::SetTheFont(CFont *pFont, BOOL bRedraw)
{
	m_wndTerm.SetTheFont(pFont, bRedraw);
	m_wndScrollback.SetTheFont(pFont, bRedraw);
	m_wndTerm.RebuildLineDefs();
}

void CSplitterBar::SetBackColor(COLORREF crBack, BOOL bRedraw)
{
	m_wndTerm.SetBackColor(crBack, bRedraw);
	m_wndScrollback.SetBackColor(crBack, bRedraw);
}

void CSplitterBar::SetForeColor(COLORREF crFore, BOOL bRedraw)
{
	m_wndTerm.SetForeColor(crFore, bRedraw);
	m_wndScrollback.SetForeColor(crFore, bRedraw);
}

void CSplitterBar::SetColorArrays(COLORREF *pFore, COLORREF *pBack)
{
	m_wndTerm.SetColorArrays(pFore, pBack);
	m_wndScrollback.SetColorArrays(pFore, pBack);
}

void CSplitterBar::SetBufferSize(int nBufferSize)
{
	m_wndTerm.SetBufferSize(nBufferSize);
	m_wndScrollback.SetBufferSize(nBufferSize);
}
void CSplitterBar::SetLineLength(int nLineLength)
{
	m_wndTerm.SetLineLength(nLineLength);
	m_wndScrollback.SetLineLength(nLineLength);
}
void CSplitterBar::SetDisplayBufferSize(int nBufferSize)
{
	m_wndTerm.SetBufferSize(nBufferSize);
}

void CSplitterBar::AvertAppendNext()
{
	m_wndTerm.AvertAppendNext();
    m_wndScrollback.AvertAppendNext();
}

void CSplitterBar::PrintLine(LPCSTR pszLine, BOOL bAppendNext, CAnsiParser &apParser,BYTE byFore, BYTE byBack)
{
	m_wndTerm.PrintLine(pszLine, bAppendNext, apParser, byFore, byBack);
	m_wndScrollback.PrintLine(pszLine, bAppendNext, apParser, byFore, byBack);
	m_wndScrollback.SetScrollSizes();
	m_wndScrollback.ScrollCaret();
	m_wndTerm.ScrollCaret();
}

void CSplitterBar::Repaint()
{    
	m_wndTerm.Repaint();
	m_wndScrollback.Repaint();
}

//
// Re-calculate background image.
//
void CSplitterBar::RecreateBitmap() {

    if (m_nBackgroundMode == TERM_WINDOW_BACKGROUND_NONE) return;

    CRect uppper_window_size;
    CRect lower_window_size;

    m_wndScrollback.GetClientRect(&uppper_window_size);
    m_wndTerm.GetClientRect(&lower_window_size);

	int width  = uppper_window_size.Width();
    int height = uppper_window_size.Height() + lower_window_size.Height();

    int x_pos;
    int y_pos;

    m_wndTerm.SetBackground(NULL);
    m_wndScrollback.SetBackground(NULL); 

    // Recreate background image.
    if(!m_imgBackground.IsNull()) m_imgBackground.Destroy();
    m_imgBackground.Create(width, height, m_imgOriginal.GetBPP(), 0);

    HDC dc = m_imgBackground.GetDC();

	switch (m_nBackgroundMode) {
   			
        case TERM_WINDOW_BACKGROUND_NORMAL:
    	    m_imgOriginal.BitBlt(dc, 0, 0, width, height, 0, 0, SRCCOPY);
	    break;

        case TERM_WINDOW_BACKGROUND_CENTER:
            x_pos = width - m_imgOriginal.GetWidth();
            if(x_pos < 0) x_pos = 0;
            else x_pos = x_pos >> 1; // Divide by two

            y_pos = height - m_imgOriginal.GetHeight();
            if(y_pos < 0) y_pos = 0;
            else y_pos = y_pos >> 1; // Divide by two
            m_imgOriginal.BitBlt(dc, x_pos, y_pos, width, height, 0, 0, SRCCOPY);
	    break;

        case TERM_WINDOW_BACKGROUND_MIDRIGHT:
            x_pos = width - m_imgOriginal.GetWidth();
            if(x_pos < 0) x_pos = 0;
            y_pos = height - m_imgOriginal.GetHeight();
            if(y_pos < 0) y_pos = 0;
            else y_pos = y_pos >> 1; // Divide by two
            m_imgOriginal.BitBlt(dc, x_pos, y_pos, width, height, 0, 0, SRCCOPY);
	    break;

        case TERM_WINDOW_BACKGROUND_BOTTOMRIGHT:
            x_pos = width - m_imgOriginal.GetWidth();
            if(x_pos < 0) x_pos = 0;
            y_pos = height - m_imgOriginal.GetHeight();
            if(y_pos < 0) y_pos = 0;
            m_imgOriginal.BitBlt(dc, x_pos, y_pos, width, height, 0, 0, SRCCOPY);
	    break;

        case TERM_WINDOW_BACKGROUND_TOPRIGHT:
            x_pos = width - m_imgOriginal.GetWidth();
            if(x_pos < 0) x_pos = 0;
            y_pos = 0;
            m_imgOriginal.BitBlt(dc, x_pos, y_pos, width, height, 0, 0, SRCCOPY);
	    break;

        case TERM_WINDOW_BACKGROUND_STRETCH:
            SetStretchBltMode(dc, COLORONCOLOR);
            m_imgOriginal.StretchBlt(dc, 0, 0, width, height, SRCCOPY);
        break;

        case TERM_WINDOW_BACKGROUND_TILE:
            HBRUSH NewBrush = CreatePatternBrush(m_imgOriginal);
            SelectObject(dc, NewBrush);	
            Rectangle(dc, 0, 0, width, height);
            DeleteObject(NewBrush);
        break;
    }

    m_wndTerm.SetBackground(&m_imgBackground);
    m_wndScrollback.SetBackground(&m_imgBackground);   

    m_imgBackground.ReleaseDC();
}

void CSplitterBar::	AppendNext(BOOL bValue)
{
	m_wndTerm.AppendNext(bValue);
    m_wndScrollback.AppendNext(bValue);
}

void CSplitterBar::ScrollCaret()
{
	m_wndTerm.ScrollCaret();
}
void CSplitterBar::ClearScreen()
{
	m_wndTerm.ClearLineDefs();
}

BOOL CSplitterBar::ResetAppendNext(CColorLine::SpColorLine &line)
{
    m_wndScrollback.ResetAppendNext(line);
	return m_wndTerm.ResetAppendNext(line);
}
BOOL CSplitterBar::ScreenShot(CString &strFilename)
{	
	m_wndTerm.ScreenShot(strFilename);
	return TRUE;

}
BOOL CSplitterBar::SetBackground(CString &strFilename, CString &strMode)
{	
	HRESULT hResult;
    m_nBackgroundMode = TERM_WINDOW_BACKGROUND_NONE;
	
    if(!m_imgOriginal.IsNull()) m_imgOriginal.Destroy();

	m_wndTerm.SetBackground(NULL);	
    m_wndScrollback.SetBackground(NULL);

	// User just wanted to clear the background.
	if(strFilename == "") {
		Repaint(); 
		return TRUE; 
	}

	// Try loading the file, if file fails to load, too bad.
	hResult = m_imgOriginal.Load(strFilename);
	if (FAILED(hResult)) {
		CString strMessage;
		Repaint();
		return FALSE;
	}

    // Determine the mode.
    if(strMode == "normal")       m_nBackgroundMode = TERM_WINDOW_BACKGROUND_NORMAL;
    else if(strMode == "center")  m_nBackgroundMode = TERM_WINDOW_BACKGROUND_CENTER;
    else if(strMode == "stretch") m_nBackgroundMode = TERM_WINDOW_BACKGROUND_STRETCH;
    else if(strMode == "tile")    m_nBackgroundMode = TERM_WINDOW_BACKGROUND_TILE;
    else if(strMode == "right")   m_nBackgroundMode = TERM_WINDOW_BACKGROUND_MIDRIGHT;
    else if(strMode == "bottomright")   m_nBackgroundMode = TERM_WINDOW_BACKGROUND_BOTTOMRIGHT;
    else if(strMode == "topright")   m_nBackgroundMode = TERM_WINDOW_BACKGROUND_TOPRIGHT;

    RecreateBitmap();

	// File loaded ok, set the appropriate image and mode.
    Repaint();

	return TRUE;
}


BOOL AFXAPI AllocHook( size_t /*nSize*/, BOOL /*bObject*/, LONG /*lRequestNumber*/ )
{
	return TRUE;
}

void CSplitterBar::PrintLine(CColorLine::SpColorLine &line)
{
    // Need to duplicate the incoming line because we
    // do not want the line object shared by both wndTerm and wndScrollback
    // as it might cause problems if the line is modified in one window.    
    CColorLine::SpColorLine duplicate(new CColorLine(*line));

	m_wndTerm.PrintLine(line);
    m_wndScrollback.PrintLine(duplicate);

	m_wndScrollback.SetScrollSizes();
	m_wndScrollback.ScrollCaret();
	m_wndTerm.ScrollCaret();
	//AfxSetAllocHook(h);
}

void CSplitterBar::SetPrompt(BOOL bPrompt)
{
	m_wndTerm.SetPrompt(bPrompt);
	m_wndScrollback.SetPrompt(bPrompt);
}

BOOL CSplitterBar::GetPrompt()
{
    return m_wndTerm.GetPrompt();
}

void CSplitterBar::CheckPromptAppend()
{
	m_wndTerm.CheckPromptAppend();
	m_wndScrollback.CheckPromptAppend();
}

void CSplitterBar::SaveScrollBack(CString &strFileName)
{
	m_wndScrollback.SaveScrollBack(strFileName);
}

void CSplitterBar::ScrollHome()
{
	m_wndScrollback.ScrollHome();
	OpenSplitterWindow();
}

void CSplitterBar::ScrollPageDown()
{
	m_wndScrollback.ScrollPageDown();
}

void CSplitterBar::ScrollPageUp()
{
	m_wndScrollback.ScrollPageUp();
	OpenSplitterWindow();
}

void CSplitterBar::ScrollLineDown()
{
	m_wndScrollback.ScrollLineDown();
}

void CSplitterBar::ScrollLineUp()
{
	m_wndScrollback.ScrollLineUp();
	OpenSplitterWindow();

}

void CSplitterBar::EndScrollBack()
{
	m_wndScrollback.EndScrollBack();
	m_wndTerm.EndScrollBack();
	CloseSplitterWindow();
}

void CSplitterBar::SaveScrollBack(LogFilePtr pFile)
{
	m_wndScrollback.SaveScrollBack(pFile);
}

void CSplitterBar::SetLogFile(LogFilePtr pLogFile)
{
	m_wndTerm.SetLogFile(pLogFile);
}

void CSplitterBar::SetLogging(BOOL bValue)
{
	m_wndTerm.SetLogging(bValue);
}

void CSplitterBar::OpenSplitterWindow()
{
	//QPerformanceTimer::QPerformanceTimer(m_wndScrollback.elapsedTime);
	if(m_rectSplitter.top != 0)
		return;

	CRect rect;
	GetClientRect(rect);

	int y = static_cast<int>(rect.bottom * m_nSplitterPercent);

	m_rectSplitter.bottom = y;

	RecalcLayout();

}

void CSplitterBar::CloseSplitterWindow()
{
	m_rectSplitter.bottom = 7;
	RecalcLayout();
}

BOOL CSplitterBar::IsFullScreenScrollback()
{
	return m_bFullScreenScrollback;
}

void CSplitterBar::SetFullScreenScrollback(BOOL bFullScreen)
{
	m_bFullScreenScrollback = bFullScreen;

	CString dir = CGlobals::GetConfigFilename();

	::WritePrivateProfileString(
		DEFAULT_SECTION_NAME,
		FULL_SCREEN_KEY,
		bFullScreen ? _T("1") : _T("0"),
		dir );

//	RecalcLayout();
}

void CSplitterBar::TimestampLog()
{
	m_wndTerm.TimestampLog();
}

#pragma warning(push)
#pragma warning(disable : 4701)
void CSplitterBar::ScrollGrep(ULONG nLines, const CString &strSearchCriteria)
{
	//CAtlRegExp<ATL::CAtlRECharTraitsA> re;
	//REParseError status = re.Parse(strSearchCriteria);
	//if(REPARSE_ERROR_OK != status)
	//{
	//	switch(status)
	//	{
	//	case REPARSE_ERROR_OUTOFMEMORY:
	//		ErrorMessage(_T("Out of memory."));
	//		break;
	//	case REPARSE_ERROR_BRACE_EXPECTED:
	//		ErrorMessage(_T("A closing brace was expected."));
	//		break;
	//	case REPARSE_ERROR_PAREN_EXPECTED:
	//		ErrorMessage(_T("A closing parenthesis was expected."));
	//		break;
	//	case REPARSE_ERROR_BRACKET_EXPECTED:
	//		ErrorMessage(_T("A closing bracket was expected."));
	//		break;
	//	case REPARSE_ERROR_UNEXPECTED:
	//		ErrorMessage(_T("An unspecified fatal error occurred."));
	//		break;
	//	case REPARSE_ERROR_EMPTY_RANGE:
	//		ErrorMessage(_T("A range expression was empty."));
	//		break;
	//	case REPARSE_ERROR_INVALID_GROUP:
	//		ErrorMessage(_T("A back reference was made to a group that did not exist."));
	//		break;
	//	case REPARSE_ERROR_INVALID_RANGE:
	//		ErrorMessage(_T("An invalid range was specified."));
	//		break;
	//	case REPARSE_ERROR_EMPTY_REPEATOP:
	//		ErrorMessage(_T("A repeat operator (* or +) was applied to an expression that could be empty."));
	//		break;
	//	case REPARSE_ERROR_INVALID_INPUT:
	//		ErrorMessage(_T("The input string was invalid."));
	//		break;
	//	}
	//	return;
	//}

	CTermWindow::BufferType::iterator it = m_wndScrollback._screenBuffer.begin();
	CTermWindow::BufferType::iterator end = m_wndScrollback._screenBuffer.end();

	CString sBugTracking;

	ULONG nMatches = 0;
	CStringList lineList;
//TODO: KW a regular expression to avoid previously displayed scrollgrep matches
//   These all start with 3 digits and a colon
		//CAtlRegExp<ATL::CAtlRECharTraitsA> reOldMatch;
		////if I use anything like ^!([0-9] or ^!\\d the match operation blows up on characters
		////which are not a-z or 0-9 but if i use ^!(...:) any 3 characters followed by : it is happy.
		//REParseError secondStatus = reOldMatch.Parse("^!(...:)");
		//if(REPARSE_ERROR_OK != secondStatus)
		//return;
	CColorLine::SpColorLine line;
	CString lineText;

	//CAtlREMatchContext<> mcMatch;
	//CAtlREMatchContext<> mcOldMatch;
	CString strFoundPrefix("^(?!\\d\\d\\d:)");
	strFoundPrefix += strSearchCriteria;
	boost::tregex expression(strFoundPrefix);
	boost::tmatch matchText;
	while(it != end)
	{
		try
		{
			//TODO: KW added ++ to *it so this actually moves through the iterator
			line = *it;
			if(NULL != line->GetTextBuffer())
			{
				lineText = line->GetTextBuffer();
				sBugTracking = lineText;
				//if match and not in a line starting with a previous scrollgrep match
				if ( boost::regex_match(lineText,matchText,expression) )
			//	if(re.Match(lineText, &mcMatch) )
				{
					//sBugTracking.Format("Matched %s after %d matches after reMatch check",sBugTracking,nMatches);
					//if(reOldMatch.Match(lineText,&mcOldMatch))
					//{
					//	sBugTracking.Format("Matched %s after %d matches before adding to list",sBugTracking,nMatches);
					//	lineList.AddTail(lineText);
						lineList.AddTail(CString(matchText[0].first,matchText.length(0)));
					//	sBugTracking.Format("Matched %s after %d matches after adding to list",sBugTracking,nMatches);

						nMatches++;
					//}
				}

				if(nMatches >= nLines)
					break;
			}
		}
		catch (...)
		{
			CString sMessage;
			sMessage.Format("Error in SplitterBar::ScrollGrep: %s \n",sBugTracking);
			::OutputDebugString(sMessage);

		}
		it++;
	}

	if(nMatches)
	{
		ScrollGrepMatch(lineList);
	}
}

void CSplitterBar::UpdateAllOptions(SerializedOptions::CTerminalOptions &options)
{
	SetTheFont(options.GetTerminalWindowFont(), FALSE);

	SetColorArrays(
		options.TerminalWindowForeColors(),
		options.TerminalWindowBackColors());

	SetBackColor(options.GetTerminalWindowBackColor(), FALSE);

	SetForeColor(options.GetTerminalWindowForeColor(), FALSE);	

	SetBufferSize(options.GetTerminalWindowBufferSize());
	//don't let changes to scrollback size make the display buffer change
	SetDisplayBufferSize(300);
	SetFullScreenScrollback(options.FullScreenScrollback());
	SetLineLength(options.GetTerminalLineLength());

	Invalidate(TRUE);
}
//LPCSTR CSplitterBar::GetNAWSmessage()
//{
//	//return m_wndTerm.GetNAWSmessage();
//}
#pragma warning (pop)