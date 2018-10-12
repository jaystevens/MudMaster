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
// TermWindow.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "TermWindow.h"
#include "MemDC.h"
#include "LineDef.h"
#include "CharFormat.h"
#include "ColorLine.h"
#include "AnsiParser.h"
#include <sstream>
#include "Colors.h"
#include "QPerformanceTimer.h"
#include <gdiplus.h>
#include "Telnet.h"

using namespace Gdiplus;
using namespace MudmasterColors;
using namespace Telnet;

#define ID_TIMER_MOUSE_EVENT 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTermWindow

CTermWindow::CTermWindow()
: m_bLogging(FALSE)
, m_bPrompt(FALSE)
, m_bScrolling(FALSE)
, m_bSelection(FALSE)
, m_crBackColor(RGB(0,0,0))
, m_crForeColor(RGB(128,128,128))
, m_crHighFore(RGB(0,0,0))
, m_crHighBack(RGB(128,128,128))
, m_nBufferSize(0)
, m_nCharHeight(0)
, m_nCharsWide(0)
, m_nCharWidth(0)
, m_nLinesHigh(0)
, m_nLineLength(200)
, m_nPaintCount(0)
, m_nScrollBackBorderWidth(2)
, m_pBackColors(NULL)
, m_pBitmap(NULL)
, m_pBackground(NULL)
, m_pImage(NULL)
, m_pFont(NULL)
, m_pForeColors(NULL)
, m_ptSelEnd(0,0)
, m_ptSelStart(0,0)
, m_rectClient(0,0,0,0)
, m_bTimestampLog(false)
{
}

CTermWindow::~CTermWindow()
{
	SAFE_DELETE(m_pBitmap);
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pDc);

	_screenBuffer.clear();
	_lineDefs.clear();
	_holdBuffer.clear();
}


BEGIN_MESSAGE_MAP(CTermWindow, CWnd)
	//{{AFX_MSG_MAP(CTermWindow)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTermWindow message handlers

BOOL CTermWindow::OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
{
	m_pDc = new CClientDC(this);
	if(NULL == m_pDc) return FALSE;

	return TRUE;
}

void CTermWindow::SetBackground(CImage *background_image)
{
    this->m_pImage = background_image;
}

void CTermWindow::OnPaint() 
{
	/**
	 * Get a device context to paint on
	 */
	CPaintDC adc(this); // device context for painting

	/**
	 * Make sure the font variable has been set
	 */
	ASSERT(m_pFont != NULL);
	if(m_pFont == NULL)
	{
		AfxMessageBox("Font is null in CTermWindow::OnPaint");
		return;
	}
	
	/**
	 * Make back buffer to paint on...when the function exits
	 * it will be fliped to the screen
	 */
	CMemDC dc(&adc, m_crBackColor, m_pBitmap);
// If we have background image, rebuild it here.
	if(m_pImage != NULL) {
        CRect window_size;
        GetClientRect(&window_size);
	    
        int width  = window_size.Width();
        int height = window_size.Height();

        if(m_bIsUpperWindow == TRUE) {
            m_pImage->BitBlt(dc, 0, 0, width, height, 0, 0, SRCCOPY);
        } else {
            m_pImage->BitBlt(dc, 0, 0, width, height, 0, m_pImage->GetHeight() - height, SRCCOPY);
        }       
	}

	/**
	 * Select the font into the DC.
	 * Is there someway I can do this only once or
	 * at least every time the font is changed so I don't
	 * have to keep selecting the font every time?
	 */
	CFont *pOldFont = dc.SelectObject(m_pFont);

	/**
	 * nY is the current y position that we'll be painting on
	 * we start painting at the bottom of the screen and then
	 * we move up the screen, painting line by line
	 */
	int nY = m_rectClient.bottom - m_nCharHeight;

	/**
	 * we need to adjust our painting bounds by enough for 
	 * the scroll back border, even if we're not in scroll back
	 */
	int nX = m_rectClient.left;;

	/**
	 * the scroll offset tells us where in scroll back we are
	 */
	int nScrollOffset = GetScrollOffset();

	/**
	 * Set the default back color
	 * and the default fore color
	 */
	dc.SetTextColor(m_crForeColor);
	dc.SetBkColor(m_crBackColor);
	
	/**
	 * This is the line we're on...this needs more explaining
	 */
	int nLine = 0;

	LineDefBufferType::iterator it = _lineDefs.begin();
	LineDefBufferType::iterator end = _lineDefs.end();
	while(it != end)
	{
		CLineDef::SpLineDef lineDef = *it;
		if(nLine < nScrollOffset)
		{
			nLine++;
			++it;
			continue;
		}

		int nX1 = nX;
		for(int i = 0; i < lineDef->nLength; i++)
		{
			BYTE crF = lineDef->cptr[i].Fore();
			BYTE crB = lineDef->cptr[i].Back();

			int x = i;
			while(true)
			{
				if(x >= lineDef->nLength)			break;
				if(lineDef->cptr[x].Fore() != crF)	break;
				if(lineDef->cptr[x].Back() != crB)	break;

				++x;
			}

			dc.SetTextColor(m_pForeColors[crF]);
			dc.SetBkColor(m_pBackColors[crB]);

			// Only display the back color if it does not match our window background color.
			if(m_crBackColor == m_pBackColors[crB])	SetBkMode(dc, TRANSPARENT);
			else SetBkMode(dc, OPAQUE);

			//dc.ExtTextOut(nX1, nY, 0, NULL, &lineDef->ptr[i], x-i, NULL);
			dc.TextOut(nX1, nY, &lineDef->ptr[i], x-i);
			nX1 += (x-i) * m_nCharWidth;
			i=x-1;
		}

		++nLine;
		nY-=m_nCharHeight;
		if(nY < 0) break;
		++it;
	}

	if(m_bSelection)	PaintHighlight(dc);
	if(IsScrolling())	PaintScrollBackBorder(dc);
//HDC hMyDC;
//hMyDC = CreateCompatibleDC(dc);
	//HBITMAP _Bitmap;
	m_Bitmap = (HBITMAP)m_pBitmap->GetSafeHandle();
	m_pScreenShot = m_Bitmap;
	//SelectObject(dc,m_pScreenShot);
	//BitBlt(hMyDC,0,0,m_rectClient.Width(),m_rectClient.Height(),dc,0,0,SRCCOPY);
//m_pScreenShot->CreateCompatibleBitmap(dc, m_rectClient.Width(), m_rectClient.Height());
	dc.SelectObject(pOldFont);
}

int CTermWindow::GetScrollOffset()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	GetScrollInfo(SB_VERT, &si, SIF_ALL);
	int nPos = si.nPos;
	int nMax = si.nMax;
	int nPage = si.nPage;

	int nScrollOffset = 0;
	if(si.nMax != si.nMin)
		nScrollOffset = (nMax - nPos - nPage) + 1;

	return nScrollOffset;
}

void CTermWindow::OrderPoints(CPoint &start, CPoint &end)
{
	CPoint pt1 = start;
	CPoint pt2 = end;

	if(start.y > end.y)
	{
		start = end;
		end = pt1;
		return;
	}

	if(start.y < end.y)
		return;

	if(start.x < end.x)
		return;

	if(start.x > end.x)
	{
		start = end;
		end = pt1;
		return;
	}
}

BOOL CTermWindow::LineOut(CMemDC &dc, char *pszLine, CCharFormat *pColorBuffer, int nY, int nX, int nChars)
{
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

#ifdef _DEBUG

	for(int test = 0; test < nChars; test++)
	{
		BYTE debugFore = pColorBuffer[test].Fore();
		BYTE debugBack = pColorBuffer[test].Back();
		ASSERT(debugFore >= 0 && debugFore < 16);
		ASSERT(debugBack >= 0 && debugBack < 8);
	}

#endif

	COLORREF crFore = m_pForeColors[pColorBuffer[0].Fore()];
	COLORREF crBack = m_pBackColors[pColorBuffer[0].Back()];
	
	dc.SetTextColor(crFore);
	dc.SetBkColor(crBack);

	char *ptr = pszLine;
	CCharFormat *cPtr = pColorBuffer;
	
	CSize szSize;

	int i = 0;
	while(i < nChars)
	{
		BYTE crF = cPtr[i].Fore();
		BYTE crB = cPtr[i].Back();

		int x = i;
		while(cPtr[x].Fore() == crF && cPtr[x].Back() == crB && x < nChars) x++;
		
		dc.SetTextColor(m_pForeColors[crF]);
		dc.SetBkColor(m_pBackColors[crB]);
		dc.ExtTextOut(nX, nY, 0, NULL, &ptr[i], x-i, NULL);
		GetTextExtentPoint32(dc, &ptr[i], x-i, &szSize);
		nX = nX + szSize.cx;
		i=x;
	}
	return TRUE;
}

BOOL CTermWindow::IsScrolling()
{
	return m_bScrolling;
}

void CTermWindow::PaintScrollBackBorder(CMemDC &dc)
{
	CRect rect;
	GetClientRect(&rect);

	LOGPEN lp;
	lp.lopnColor = RGB(255,0,0);
	lp.lopnStyle = PS_SOLID;
	lp.lopnWidth.x = m_nScrollBackBorderWidth;
	lp.lopnWidth.y = m_nScrollBackBorderWidth;

	CPen aPen;

	aPen.CreatePenIndirect(&lp);

	CPen *pOldPen = dc->SelectObject(&aPen);

	dc->MoveTo(1,1);
	dc->LineTo(rect.right-m_nScrollBackBorderWidth, 1);
	dc->LineTo(rect.right-m_nScrollBackBorderWidth, rect.bottom-m_nScrollBackBorderWidth);
	dc->LineTo(1, rect.bottom-m_nScrollBackBorderWidth);
	dc->LineTo(1,1);

	dc->SelectObject(pOldPen);
	aPen.DeleteObject();
}

//
// This function is called when the user has entered some text.
// This function turns off the prompt flag, which will treat,
// the previous line as if it was a regular line.
//
void CTermWindow::CheckPromptAppend()
{
    if(m_bPrompt) m_bPrompt = FALSE;
}

void CTermWindow::AddLine(CColorLine::SpColorLine &line)
{
	if(GetSafeHwnd() == NULL) return;
    
	if(m_bSelection || IsScrolling())
	{
		_holdBuffer.push_back(line);
		return;
	}

    // it = last line
    BufferType::iterator it = _screenBuffer.begin();    

    // If prompt flag is still set then the last line, was a promt
    // and nothing was entered. We should treat last line as
    // if it was a regular line, and we should log it.
    if(m_bPrompt) 
	{
        CColorLine::SpColorLine lastLine = *it;
        AvertAppendNext();
        Log(lastLine->GetTextBuffer());

        // If there is an empty line after the prompt, ignore it      
        char *ptr = line->GetTextBuffer();
        if(*ptr == '\0')
            return;
    }

    // Only append the text to the previous line, if there is a 
    // previous line and it has appendNext flag set.   
    if(!_screenBuffer.empty() && (*it)->AppendNext())
	{
        CColorLine::SpColorLine lastLine = *it;

        RemoveLineDefs(lastLine, TRUE);
        lastLine->Append(line);
		lastLine->AppendNext(line->AppendNext());

		AddLineDef(lastLine);
        Invalidate();
        
        if(!lastLine->AppendNext()) {
            Log(lastLine->GetTextBuffer());
        }
    }
	else
	{
		_screenBuffer.push_front(line);
		AddLineDef(line);
        if(!line->AppendNext())	{
            Log(line->GetTextBuffer());
        }
    }

    // Clear the prompt flag.
    m_bPrompt = FALSE;

	//TODO: KW is doing test faster than just setting size?
	//      after a short time the buffer is always at it's limit
	// I wonder if it just makes sense to always resize
	//_screenBuffer.resize(m_nBufferSize);

	if(_screenBuffer.size() > m_nBufferSize)
	{
		// resize apparently removes the lines at the end of the buffer
		// so no need for a loop to remove the last lines
		_screenBuffer.resize(m_nBufferSize - 100);
	    //
	    ////	int numLinesToRemove = _screenBuffer.size() - m_nBufferSize;

    		
	    //TODO: KW patch to make scrollback overflow stay under control
	    //   
	    //	for(int i = 0; i < numLinesToRemove; ++i)
	    //	{
	    //		_screenBuffer.pop_back();
	    //	}
    	
	    //end KW patch
		//BufferType::reverse_iterator rit = _screenBuffer.rbegin();
		//CColorLine::SpColorLine l = *rit++;
		RemoveLineDefs(line,FALSE );
	}
}

// Go back and erase line from the screen, may need to erase a lot of them
// if the line is broken into pieces
void CTermWindow::RemoveLineDefs(CColorLine::SpColorLine &line, BOOL bHead)
{
	// Hrm, if the screen buffer is empty so should the the line def buffer
	if(_screenBuffer.empty())
	{
		_lineDefs.clear();
		return;
	}
	if(bHead)
	{
		LineDefBufferType::iterator it     = _lineDefs.begin();      
		LineDefBufferType::iterator end    = _lineDefs.end();
		while(it != end)
		{
			CLineDef::SpLineDef l = *it;           
            ++it;
            
            // If the line we are trying to remove is not the first line,
            // or if that line is broken into many pieces, chop off
            // each piece from the buffer until the line is removed.
			if(l->line.get() != line.get())
			{	
				return;
			}
            _lineDefs.pop_front();
		}
	}
	else
	{
		//If we are removing from the end it is because we've hit the
		//max buffer size. Chop off the last 100 so we don't have to
		//do it every line.
		LineDefBufferType::iterator it = _lineDefs.end();
		_lineDefs.resize(_lineDefs.size() - 100);
		return;
		//LineDefBufferType::iterator it = _lineDefs.end();
		//LineDefBufferType::iterator end = _lineDefs.begin();
		//while(it != end)
		//{
		//	--it;
		//	CLineDef::SpLineDef l = *it;
		//	if(l->line.get() == line.get())
		//	{
		//		_lineDefs.erase(it,_lineDefs.end());
		//		return;
		//	}
		//}		
	}
}

void CTermWindow::AddLineDef(CColorLine::SpColorLine &line)
{
	//ASSERT(m_pFont != NULL);
	//if(m_pFont == NULL)
	//{
	//	AfxMessageBox("Font is null in CTermWindow::AddLineDef");
	//	return;
	//}

	size_t nLen = strlen(line->GetTextBuffer());
	if(nLen < m_nCharsWide)
	{
		CLineDef::SpLineDef lineDef(new CLineDef);
		lineDef->nLength = nLen;
		lineDef->ptr     = line->GetTextBuffer();
		lineDef->cptr    = line->GetColorBuffer();
		lineDef->line    = line;
		lineDef->nWidth  = m_nCharWidth * nLen;
		_lineDefs.push_front(lineDef);
		return;
	}

	char *ptr = line->GetTextBuffer();
	CCharFormat *cptr = line->GetColorBuffer();

	char *ptr2 = ptr;
	CCharFormat *cptr2 = cptr;

	while(*ptr2 != '\0')
	{
		ptr  += min(m_nCharsWide, strlen(ptr2));
		cptr += ptr - ptr2;

		CLineDef::SpLineDef lineDef(new CLineDef);
		lineDef->nLength = ptr - ptr2;
		lineDef->ptr = ptr2;
		lineDef->cptr = cptr2;
		lineDef->line = line;
		lineDef->nWidth = m_nCharWidth * lineDef->nLength;
		_lineDefs.push_front(lineDef);

		ptr2 =ptr;
		cptr2=cptr;
	}
}

void CTermWindow::SetScrollSizes()
{
	LineDefBufferType::size_type nLineCount = _lineDefs.size();

	SCROLLINFO si;
	si.cbSize = sizeof SCROLLINFO;

	if(nLineCount <= m_nLinesHigh)
	{
		si.fMask = SIF_RANGE;
		si.nMin = 0;
		si.nMax = 0;
		SetScrollInfo(SB_VERT, &si);
		return;
	}

	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = nLineCount;

	int nTemp = m_nLinesHigh - 1;
	si.nPage = nTemp > 0 ? nTemp : 1;
	SetScrollInfo(SB_VERT, &si);
}

void CTermWindow::ScrollCaret()
{
	//If we're in scroll back, lets not scroll to the caret
	//We're not really using a caret, but I'm just copying
	//Function names from the Edit Control
	if(IsScrolling())
		return;

	//How many lines are currently in the screen buffer.  The
	//Screen buffer counts lines based on the number of characters
	//that fit across the screen.
	LineDefBufferType::size_type nLineCount = _lineDefs.size();

	CRect rect;
	GetClientRect(&rect);

	UINT nLinesHigh = 0;
	if(m_nCharHeight > 0)
		nLinesHigh = rect.bottom/m_nCharHeight;

	//If the number of lines in the screen buffer is less than the
	//number of lines that will fit down the screen, no need to scroll
	if(nLineCount < nLinesHigh)
		return;

	//Set the top line to the one calculated above
	SetScrollPos(SB_VERT, nLineCount);
}

BOOL CTermWindow::OnEraseBkgnd(CDC* /*pDC*/) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CTermWindow::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	m_bSelection = TRUE;
	CPoint e = CharFromPos(point);
	SetCapture();
	SetSelStart(e);
	SetTimer(ID_TIMER_MOUSE_EVENT, 100, 0);
}

CPoint CTermWindow::CharFromPos(CPoint &point)
{
	ASSERT(m_pFont != NULL);
	if(m_pFont == NULL)
	{
		AfxMessageBox("Font is null in CTermWindow::CharFromPos");
		return CPoint(0,0);
	}

	CRect rect;
	GetClientRect(&rect);

	int nLine = ((rect.bottom - point.y)/m_nCharHeight) + GetScrollOffset();

	LineDefBufferType::size_type x = nLine;
	LineDefBufferType::iterator it = _lineDefs.begin();
	LineDefBufferType::iterator end = _lineDefs.end();

	while(it != end && x > 0)
	{
		x--;
		++it;
	}

	CLineDef::SpLineDef lineDef;

	if(it == end)
		return CPoint(0,0);
	else
		lineDef = *it;

	if(point.x > lineDef->nWidth)
		return CPoint(lineDef->nLength, nLine);

	char *ptr = lineDef->ptr;

	CFont *pOldFont = m_pDc->SelectObject(m_pFont);
	CSize size;
	do
	{
		ptr++;
		GetTextExtentPoint32(m_pDc->operator HDC(), lineDef->ptr, ptr-lineDef->ptr, &size);
	}
	while(size.cx < point.x);

	m_pDc->SelectObject(pOldFont);
	return CPoint(ptr-lineDef->ptr-1,nLine);
}

void CTermWindow::SetSelStart(CPoint &point)
{
	m_ptSelStart = point;
}

void CTermWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(GetCapture()==this)
	{
		CPoint e = CharFromPos(point);
		SetSelEnd(e);
		TRACE("Sel Start = (%d, %d)", m_ptSelStart.y, m_ptSelStart.x);
		TRACE("Sel End = (%d, %d)", m_ptSelEnd.y, m_ptSelEnd.x);
		ReleaseCapture();
		CString strSelection;
		//Holding control while copying with copy the ansi codes for colour as well
		if (nFlags && MK_CONTROL)
		{
			CopyANSISelection(strSelection);
			//m_pDocument set for both terminal and scrollback
			
			m_pDocument->Session()->SetANSICopy(strSelection);
		}
		else
		{
			CopySelection(strSelection);
		}

		PutSelectionToClipboard(strSelection);
		CPoint aPoint(0,0);
		SetSelStart(aPoint);
		SetSelEnd(aPoint);
		m_bSelection = FALSE;
		UnloadHoldBuffer();
		Invalidate();
		GetParentFrame()->PostMessage(WM_SETFOCUS, 0, 0);
		KillTimer(ID_TIMER_MOUSE_EVENT);
	}
}

void CTermWindow::SetSelEnd(CPoint &point)
{
	m_ptSelEnd = point;
}

void CTermWindow::CopySelection(CString &strSelection)
{
	CPoint ptStart = m_ptSelStart;
	CPoint ptEnd = m_ptSelEnd;

	OrderPoints(ptStart, ptEnd);

	strSelection = "";
	BOOL bInSel = FALSE;
	int nLine = 0;

	LineDefBufferType::iterator it = _lineDefs.begin();
	LineDefBufferType::iterator end = _lineDefs.end();
	while(it != end)
	{
		CLineDef::SpLineDef line = *it;
		if(nLine == ptStart.y && ptStart.y == ptEnd.y)
		{
			for(int i = 0; i < ptEnd.x - ptStart.x; i++)
			{
				strSelection += line->ptr[ptStart.x + i];
			}
			return;
		}
		else if(nLine == ptStart.y)
		{
			CString strLine;
			for(int i = 0; i < ptStart.x; i++)
			{
				strLine += line->ptr[i];
			}
			strSelection = strLine;
			bInSel = TRUE;
		}
		else if(bInSel && nLine < ptEnd.y)
		{
			CString strLine;
			for(int i = 0; i < line->nLength; i++)
			{
				strLine += line->ptr[i];
			}
			strLine += "\r\n";
			strLine += strSelection;
			strSelection = strLine;
		}
		else if(bInSel)
		{
			CString strLine;
			for(int i = 0; i < line->nLength - ptEnd.x; i++)
			{
				strLine  += line->ptr[ptEnd.x + i];
			}
			strLine += "\r\n";
			strLine += strSelection;
			strSelection = strLine;
			return;
		}

		nLine++;
		++it;
	}
}
void CTermWindow::CopyANSISelection(CString &strSelection)
{
	CPoint ptStart = m_ptSelStart;
	CPoint ptEnd = m_ptSelEnd;

	OrderPoints(ptStart, ptEnd);
	// linedefs start at the bottom of the screen and increment up (received earlier)
	// so ptStart is the point nearer the bottom of the screen
	// and ptEnd is the point nearer the top
	strSelection = "";
	BOOL bInSel = FALSE;
	int nLine = ptEnd.y;
//try to follow colour changes
		BYTE crF = 7;
		BYTE crB = 0;
		int iColour;
//----------------
	LineDefBufferType::iterator begin = _lineDefs.begin();
	LineDefBufferType::iterator it = _lineDefs.begin();
	// starting a the bottom of the screen increment the iterator
	// until it reaches the linedef at the top of the selection
	for(int j = 0; j < ptEnd.y; j++)
	{
		++it;
	}
	CLineDef::SpLineDef line = *it;
	//When selection starts above zero (not on the very left of screen)  there is not an extra empty line
	//if(ptEnd.x != 0) --it;
	if (ptStart.y == 0 && ptEnd.y ==0)   //one single line selected and it is bottom line
		{
		if(nLine == ptStart.y && ptStart.y == ptEnd.y)   //one single line selected
		{

			// start at first selected character for the number selected
			for(int i = ptStart.x; i < ptEnd.x; i++)
			{
				//check colour
				if (line->cptr[i].Fore() != crF)
				{ 
					iColour = line->cptr[i].Fore() ;
					strSelection += ANSI_FORE_COLORS[iColour];
					crF = line->cptr[i].Fore();
				}
				if (line->cptr[i].Back() != crB)
				{
					iColour = line->cptr[i].Back();
					strSelection += ANSI_BACK_COLORS[iColour];
					crB = line->cptr[i].Back();
				}

				strSelection += line->ptr[i];
			}
			return;
		}
		}

	while(it != begin)
	{
		line = *it;
		if(nLine == ptStart.y && ptStart.y == ptEnd.y)   //one single line selected
		{

			// start at first selected character for the number selected
			for(int i = ptStart.x; i < ptEnd.x; i++)
			{
				//check colour
				if (line->cptr[i].Fore() != crF)
				{ 
					iColour = line->cptr[i].Fore() ;
					strSelection += ANSI_FORE_COLORS[iColour];
					crF = line->cptr[i].Fore();
				}
				if (line->cptr[i].Back() != crB)
				{
					iColour = line->cptr[i].Back();
					strSelection += ANSI_BACK_COLORS[iColour];
					crB = line->cptr[i].Back();
				}

				strSelection += line->ptr[i];
			}
			return;
		}
		else if(nLine == ptEnd.y)
		{
			CString strLine;
			for(int i = ptEnd.x; i < line->nLength; i++)

			{
				//check colour
				if (line->cptr[i].Fore() != crF)
				{ 
					iColour = line->cptr[i].Fore() ;
					strLine += ANSI_FORE_COLORS[iColour];
					crF = line->cptr[i].Fore();
				}
				if (line->cptr[i].Back() != crB)
				{
					iColour = line->cptr[i].Back() ;
					strLine += ANSI_BACK_COLORS[iColour];
					crB = line->cptr[i].Back();
				}

				strLine += line->ptr[i];
			}
			if(ptStart.y != ptEnd.y) strLine += "\r\n";
			strSelection = strLine;
//			strSelection += ANSI_RESET;
			bInSel = TRUE;
		}
		else if(bInSel && nLine > ptStart.y)
		{
			CString strLine;
			for(int i = 0; i < line->nLength; i++)
			{
				//check colour
				if (line->cptr[ i].Fore() != crF)
				{ 
					iColour = line->cptr[i].Fore() ;
					strLine += ANSI_FORE_COLORS[iColour];
					crF = line->cptr[ i].Fore();
				}
				if (line->cptr[i].Back() != crB)
				{
					iColour = line->cptr[i].Back() ;
					strLine += ANSI_BACK_COLORS[iColour];
					crB = line->cptr[ i].Back();
				}

				strLine += line->ptr[i];
			}
//			strLine += ANSI_RESET;
			strLine += "\r\n";
			//strLine += strSelection;
			//strSelection = strLine;
			strSelection += strLine;
		}
		else if(bInSel)  // would be the last line or partial line
		{
			CString strLine;
			// remember ptStart is the lower point of the selection
			for(int i = 0; i <  ptStart.x; i++)
			{
				//check colour
				if (line->cptr[i].Fore() != crF)
				{ 
					iColour = line->cptr[i].Fore() ;
					strLine += ANSI_FORE_COLORS[iColour];
					crF = line->cptr[ptStart.x + i].Fore();
				}
				if (line->cptr[i].Back() != crB)
				{
					iColour = line->cptr[i].Back() ;
					strLine += ANSI_BACK_COLORS[iColour];
					crB = line->cptr[i].Back();
				}

				strLine  += line->ptr[i];
			}
//			strLine += ANSI_RESET;
			strLine += "\r\n";
//			strLine += strSelection;
//			strSelection = strLine;
			strSelection += strLine;
			return;
		}

		nLine--;
		--it;
	}
}

void CTermWindow::PutSelectionToClipboard(CString &strText)
{
	if(strText.IsEmpty())
		return;

	if(!OpenClipboard())
	{
		TRACE("Unable to open clipboard.");
		return;
	}

	EmptyClipboard();
	
	HGLOBAL hg = GlobalAlloc(GHND | GMEM_SHARE, (strText.GetLength() * sizeof(TCHAR))+1);
	if(hg == NULL)
	{
		ASSERT(FALSE);
		CloseClipboard();
		return;
	}

	LPTSTR lptstrString = (LPTSTR)GlobalLock(hg);

	if(lptstrString == NULL)
	{
		ASSERT(FALSE);
		AfxMessageBox("Clipboard operation failed.", MB_OK | MB_ICONEXCLAMATION);
		CloseClipboard();
		GlobalFree(hg);
		return;
	}

	strcpy(lptstrString, strText);

	BOOL bResult = GlobalUnlock(hg);
	int nError = GetLastError();

	if(!bResult && nError != NO_ERROR)
	{
		ASSERT(FALSE);
		AfxMessageBox("Clipboard operation failed.", MB_OK | MB_ICONEXCLAMATION);
		CloseClipboard();
		return;
	}

	HANDLE hData = SetClipboardData(CF_TEXT, hg);
	if(hData == NULL)
	{
		ASSERT(FALSE);
		AfxMessageBox("Clipboard operation failed.", MB_OK | MB_ICONEXCLAMATION);
		CloseClipboard();
		GlobalFree(hg);
		return;
	}

	if(!CloseClipboard())
	{
		AfxMessageBox("Clipboard operation failed.", MB_OK | MB_ICONEXCLAMATION);
		ASSERT(FALSE);
		return;
	}

	m_bSelection = FALSE;
	//RedrawWindow();
	Invalidate();
}

void CTermWindow::UnloadHoldBuffer()
{
	if(IsScrolling() || m_bSelection)
		return;

	if(_holdBuffer.empty())
		return;

	BufferType::iterator end = _holdBuffer.end();
	for(BufferType::iterator it = _holdBuffer.begin(); it != end; ++it)
	{
		PrintLine(*it);
	}
	_holdBuffer.clear();

	if(_screenBuffer.empty())
		return;
	
	if(_screenBuffer.size() > m_nBufferSize)
		_screenBuffer.resize(m_nBufferSize - 100);

	//{
	//	int linesToDelete = _screenBuffer.size() - m_nBufferSize;
	//	for(int i = 0; i < linesToDelete; ++i)
	//	{
	//		_screenBuffer.pop_back();
	//	}
	//}
	//RedrawWindow();
	Invalidate();
}

void CTermWindow::PrintLine(CColorLine::SpColorLine &line)
{
	AddLine(line);
	Repaint();
}

void CTermWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if(m_pBitmap == NULL) m_pBitmap = new CBitmap();

	GetClientRect(&m_rectClient);

	// Create a bitmap for double buffering.
	m_pBitmap->DeleteObject();
	m_pBitmap->CreateCompatibleBitmap(m_pDc, m_rectClient.Width(), m_rectClient.Height());

	RecalcSizes();
}

void CTermWindow::RebuildLineDefs()
{
		// track timing 
	//QPerformanceTimer qpTimer("RebuildLineDefs");
	_lineDefs.clear();

	BufferType::reverse_iterator it = _screenBuffer.rbegin();
	BufferType::reverse_iterator end = _screenBuffer.rend();
	while(it != end)
		AddLineDef(*it++);
	//CString strMessage;
	//strMessage.Format("linedef size is now %d screenbuffer size %d\n", _lineDefs.size(),_screenBuffer.size());
	//::OutputDebugString(strMessage);

}
void CTermWindow::ClearLineDefs()
{
	_screenBuffer.clear();
	_lineDefs.clear();
	//Must make sure the m_bPrompt is false to prevent an
	//attempt to append to a line that doesn't exist
	SetPrompt(FALSE);
	Repaint();

}

void CTermWindow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
{
	switch(nSBCode)
	{
	case SB_LINEDOWN:
		ScrollLineDown();
		break;
	case SB_LINEUP:
		ScrollLineUp();
		break;
	case SB_PAGEUP:
		ScrollPageUp();
		break;
	case SB_PAGEDOWN:
		ScrollPageDown();
		break;
	case SB_THUMBTRACK:
		OnThumbTrack(nPos);
		break;
	}
}
void CTermWindow::ScreenShot(CString &strFilename) 
{
// clear the bitmap for shot
	GetClientRect(&m_rectClient);
	m_pBitmap->DeleteObject();
	m_pBitmap->CreateCompatibleBitmap(m_pDc, m_rectClient.Width(), m_rectClient.Height());

	CTermWindow::OnPaint();
	CImage screenShot;
	screenShot.Attach(m_pScreenShot);
	if(strFilename.MakeLower().Find(_T("png"),strFilename.GetLength() - 4))
		screenShot.Save(strFilename,ImageFormatPNG);
	else
		screenShot.Save(strFilename,ImageFormatJPEG);
	screenShot.Detach();

}

BOOL CTermWindow::ScrollLineUp()
{
	BOOL bWasScrolling = IsScrolling();
	SetScrolling(TRUE);
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);
	int nNewPos = si.nPos - 1;
	if(nNewPos < si.nMin)
	{
		return bWasScrolling;
	}
	SetScrollPos(SB_VERT, nNewPos);
	//RedrawWindow();	
	Invalidate();
	return bWasScrolling;
}

BOOL CTermWindow::ScrollLineDown()
{
	BOOL bWasScrolling = IsScrolling();
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);
	int nNewPos = si.nPos + 1;

	SetScrollPos(SB_VERT, nNewPos);

	if(GetScrollOffset() == 0)
	{
		SetScrolling(FALSE);
		UnloadHoldBuffer();
	}

	//RedrawWindow();	
	Invalidate();
	return bWasScrolling;
}

BOOL CTermWindow::ScrollPageUp()
{
	BOOL bWasScrolling = IsScrolling();
	if(!bWasScrolling)
		SetScrolling(TRUE);

	//Get the current scroll bar information
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);

	//If we page up, what would be our new position?
	int nNewPos = si.nPos - si.nPage;

	//If the new position is beyond the top of the
	//scroll back buffer, set the new position to the
	//top of the scrollback buffer.
	if(nNewPos < si.nMin)
		nNewPos = si.nMin;

	//Set the scroll bar position
	SetScrollPos(SB_VERT, nNewPos);
	//RedrawWindow();
	Invalidate();
	return bWasScrolling;
}

BOOL CTermWindow::ScrollPageDown()
{
	if(!IsScrolling())
		return FALSE;

	BOOL bWasScrolling = IsScrolling();
	//Get Scroll Bar Information
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);

	//If we page down, how many lines would that be?
	int nNewPos = si.nPos + si.nPage;

	//If we're currently displaying the bottom of the
	//screen buffer, we're done scrolling.
	if(si.nPos > si.nMax)
	{
		SetScrolling(FALSE);
		UnloadHoldBuffer();
		return bWasScrolling;
	}

	//If the new position would put the last visible line
	//higher than the bottom line of the screen, adjust the
	//new position so that the last line of text in the buffer
	//gets printed on the bottom line of the screen.
	if(nNewPos  > si.nMax)
	{
		nNewPos = si.nMax;
		SetScrolling(FALSE);
		UnloadHoldBuffer();
		return bWasScrolling;
	}

	//Set the scroll bar to the new position
	SetScrollPos(SB_VERT, nNewPos);

	if(GetScrollOffset() == 0)
	{
		SetScrolling(FALSE);
		UnloadHoldBuffer();
	}

	//RedrawWindow();
	Invalidate();
	return bWasScrolling;
}

void CTermWindow::OnThumbTrack(int nPos)
{
	SetScrolling(TRUE);
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);
	SetScrollPos(SB_VERT, nPos);

	if(GetScrollOffset() == 0)
	{
		SetScrolling(FALSE);
		UnloadHoldBuffer();
	}
	//RedrawWindow();	
	Invalidate();
}

void CTermWindow::SetScrolling(BOOL bValue)
{
	m_bScrolling = bValue;
}

void CTermWindow::InitCharHeight()
{
	ASSERT(m_pFont != NULL);
	if(m_pFont == NULL)
	{
		AfxMessageBox("Font is null in CTermWindow::InitCharHeight");
		ASSERT(FALSE);
		return;
	}
	RecalcSizes();
}

void CTermWindow::SetTheFont(CFont *pFont, BOOL bRedraw)
{
	m_pFont = pFont;
	InitCharHeight();
	if(bRedraw)
	{
		Invalidate();
		//RedrawWindow();
	}
}

void CTermWindow::SetBackColor(COLORREF crBack, BOOL bRedraw)
{
	m_crBackColor = m_pBackColors[crBack];
	if(bRedraw)
	{
		Invalidate();
		//RedrawWindow();
	}
}

void CTermWindow::SetForeColor(COLORREF crFore, BOOL bRedraw)
{
	m_crForeColor = m_pForeColors[crFore];
	if(bRedraw)
	{
		Invalidate();
		//RedrawWindow();
	}
}

void CTermWindow::AvertAppendNext()
{
	if(!_screenBuffer.empty())
		(*_screenBuffer.begin())->AppendNext(FALSE);

}

void CTermWindow::Repaint()
{
	if(!IsScrolling() && !m_bSelection)
	{
		Invalidate();
	}
}

void CTermWindow::PrintLine(LPCSTR pszLine, BOOL bAppendNext, CAnsiParser &apParser,BYTE byFore, BYTE byBack)
{
	CColorLine::SpColorLine line(new CColorLine(byFore, byBack));
	apParser.ParseAnsi(pszLine, line);
    line->AppendNext(bAppendNext);

	PrintLine(line);
}

void CTermWindow::EndScrollBack()
{
	SetScrolling(FALSE);
	UnloadHoldBuffer();
	ScrollCaret();
	//RedrawWindow();
	Invalidate();
}

void CTermWindow::AppendNext(BOOL bValue)
{
	if(!_screenBuffer.empty())
		(*_screenBuffer.begin())->AppendNext(bValue);
}

BOOL CTermWindow::ResetAppendNext(CColorLine::SpColorLine &line)
{

	if(!_screenBuffer.empty())
	{
		(*_screenBuffer.begin())->AppendNext(line->AppendNext());
		return TRUE;
	}
	return FALSE;
}

void CTermWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(GetCapture()==this)
	{
		if(nFlags & MK_LBUTTON)
		{
			if(point.y < m_rectClient.top)
			{
				int nLinesOff = (-point.y) / m_nCharHeight;
				for(int x = 0; x < nLinesOff; x++)
					ScrollLineUp();
				point.y=m_rectClient.top;
			}
			if(point.y > m_rectClient.bottom)
			{
				int nLinesOff = (point.y - m_rectClient.bottom) / m_nCharHeight;
				for(int x = 0; x < nLinesOff; x++)
					ScrollLineDown();
				point.y = m_rectClient.bottom;
			}
			CPoint e = CharFromPos(point);
			SetSelEnd(e);
			Invalidate();
		}
	}
}

void CTermWindow::SaveScrollBack(CString &strFileName)
{
	CFile aFile;
	
	try
	{
		aFile.Open(strFileName , CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
		
	}
	catch(CFileException *e)
	{
		e->Delete();
		AfxMessageBox("Error opening scroll back file.");
		return;
	}

	BufferType::reverse_iterator it = _screenBuffer.rbegin();
	BufferType::reverse_iterator end = _screenBuffer.rend();
	//if saving the file with htm or html extension then save as html otherwise plain text
	CString strFileNameLower(strFileName);
	CString strUTF8;
	strFileNameLower.MakeLower();
	if(strFileNameLower.Find(".htm")>0)
	{
		//save as html with CSS
		//try to follow colour changes
		BYTE crF = 7;
		BYTE crB = 0;
		int iColour, iSpans;
		CCharFormat * lineColours;
		char * lineText;
		CString strLine;
		CString strCSS,strCSSFore,strCSSBack;
		strLine = "<?xml version =\"1.0\" encoding=\"ISO-8859-1\"><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"scrollback.css\"></head><body><pre>";

		aFile.Write(strLine, strLine.GetLength());
		iSpans = 0;
		while(it != end)
		{
		CColorLine::SpColorLine line = *it;
			strLine.Empty();
			lineColours = line->GetColorBuffer();
			lineText = line->GetTextBuffer();
			crF = 7;
			crB = 0;

			for(int i = 0; i < (int)strlen(lineText)  ; i++)
			{
				strCSS.Empty();
				strCSSFore.Empty();
				strCSSBack.Empty();
				//check colour
				if (lineColours[i].Back() != crB || lineColours[i].Fore() != crF)
				{
					while( (iSpans >0) )
					{
						iSpans--;
						strCSS += _T("</span>");
					}
				}
				if (lineColours[i].Fore() != crF)
				{ 
					strCSS += _T("<span class=");
					iColour = lineColours[i].Fore() ;
					strCSSFore = HTML_FORE_COLORS[iColour];
					strCSSFore += _T(">");
					crF = iColour;
					iSpans++;
				}
				if (lineColours[i].Back() != crB)
				{
					iColour = lineColours[i].Back();
					strCSSBack += _T("<span class=");
					strCSSBack += HTML_BACK_COLORS[iColour];
					strCSSBack += _T(">");
					crB = iColour;
					iSpans++;
				}

				strCSS += strCSSFore;
				strCSS += strCSSBack;
				strLine += strCSS;
				strLine += lineText[i];
			}

			strLine += _T("\r\n");
			aFile.Write(strLine, strLine.GetLength());
			it++;
		}
		strLine = _T("</pre></body></html>");
		aFile.Write(strLine, strLine.GetLength());

	}
	else 
	{
		while(it != end)
		{
			CString strLine = (*it)->GetTextBuffer();
			strLine += "\r\n";
			aFile.Write(strLine, strLine.GetLength());
			it++;
		}
	}
	aFile.Close();
}

void CTermWindow::SaveScrollBack(LogFilePtr pLogFile)
{
	BufferType::reverse_iterator it = _screenBuffer.rbegin();
	BufferType::reverse_iterator end = _screenBuffer.rend();
	while(it != end)
	{
		CColorLine::SpColorLine line = *it;
		pLogFile->Write(line->GetTextBuffer(), strlen(line->GetTextBuffer()));
		pLogFile->Write("\r\n", 2);
        it++;
	}
}

void CTermWindow::SetColorArrays(COLORREF *pFore, COLORREF *pBack)
{
	m_pForeColors = pFore;
	m_pBackColors = pBack;
}

void CTermWindow::SetBufferSize(int nBufferSize)
{
	m_nBufferSize = nBufferSize;
}
void CTermWindow::SetLineLength(int nLineLength)
{
	m_nLineLength = nLineLength;
}

BOOL CTermWindow::ScrollHome()
{
	BOOL bWasScrolling = IsScrolling();
	SetScrolling(TRUE);

	//Get the current scroll bar information
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si,SIF_ALL);

	//Set the scroll bar position
	SetScrollPos(SB_VERT, si.nMin);
	//RedrawWindow();
	Invalidate();
	return bWasScrolling;
}

void CTermWindow::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/) 
{
	
}

void CTermWindow::OnEditFind() 
{
}

void CTermWindow::SetLogging(BOOL bValue)
{
	m_bLogging = bValue;
	if(m_bLogging == FALSE)
		_pLogFile.reset();
}

void CTermWindow::SetLogFile(LogFilePtr pLogFile)
{
	ASSERT(pLogFile != NULL);
	_pLogFile = pLogFile;
}

void CTermWindow::OnDestroy() 
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here
	
}

void CTermWindow::SetPrompt(BOOL bPrompt)
{
	m_bPrompt = bPrompt;
}

BOOL CTermWindow::GetPrompt()
{
    return m_bPrompt;
}

void CTermWindow::RecalcSizes()
{
	CFont *pFont = m_pDc->SelectObject(m_pFont);

	TEXTMETRIC tm;
	m_pDc->GetTextMetrics(&tm);
	m_pDc->SelectObject(pFont);

	m_nCharHeight = tm.tmHeight + tm.tmExternalLeading;
	m_nCharWidth = tm.tmAveCharWidth;

	GetClientRect(&m_rectClient);
	m_rectClient.InflateRect(-(m_nScrollBackBorderWidth + 1),-m_nScrollBackBorderWidth,-m_nScrollBackBorderWidth,-(m_nScrollBackBorderWidth+1));
	int cx = GetSystemMetrics(SM_CXVSCROLL);
	m_rectClient.right -= cx;
	
	m_nCharsWide = 0;
	m_nLinesHigh = 0;

	if(m_nCharWidth > 0)
		m_nCharsWide = min(((m_rectClient.right - m_rectClient.left) / m_nCharWidth), m_nLineLength);

	if(m_nCharHeight > 0)
		m_nLinesHigh = (m_rectClient.bottom - m_rectClient.top) / m_nCharHeight;

	m_pDc->SelectObject(pFont);
	if(m_nLinesHigh > 5 && m_nCharsWide > 10)
		UpdateNAWSmessage();
//TODO: KW this is done in recalclayout so not repeating it here
//	RebuildLineDefs();
}
void CTermWindow::UpdateNAWSmessage()
{
	NAWSmessage[0] = IAC;
	NAWSmessage[1] = SB;
	NAWSmessage[2] = NAWS;
	NAWSmessage[3] = (BYTE)(m_nCharsWide >>8);
	NAWSmessage[4] = (BYTE)(m_nCharsWide & 0xff);
	NAWSmessage[5] = (BYTE)(m_nLinesHigh >>8);
	NAWSmessage[6] = (BYTE)(m_nLinesHigh & 0xff);
	NAWSmessage[7] = IAC;
	NAWSmessage[8] = SE;
	NAWSmessage[9] = 0x00;
	m_pDocument->Session()->SetTelnetNAWSmessage((LPCSTR)NAWSmessage);
}
void CTermWindow::PaintHighlight(CMemDC &dc)
{
	/**
	 * This is the mouse selection start and end points
	 */
	CPoint ptStart, ptEnd;
	ptStart = m_ptSelStart;
	ptEnd = m_ptSelEnd;

	/**
	 * Reorder the points so that they're bottom up
	 */
	OrderPoints(ptStart, ptEnd);

	TRACE("START:  %d, %d", ptStart.y, ptStart.x);
	TRACE("END: %d, %d", ptEnd.y, ptEnd.x);

	int nScrollOffset = GetScrollOffset();
	int nTopLine = nScrollOffset + m_nLinesHigh;

	TRACE("nScrollOffset = %d", nScrollOffset);

	if(ptStart.y == ptEnd.y)
	{
		if (ptStart.y < nScrollOffset || ptStart.y > nTopLine)
			return;

		CRect rectInvert;

		rectInvert.left = m_rectClient.left + m_nCharWidth * ptStart.x;
		rectInvert.top = (m_rectClient.bottom - m_nCharHeight * (ptStart.y - nScrollOffset))-m_nCharHeight;
		rectInvert.bottom = rectInvert.top + m_nCharHeight;
		rectInvert.right = m_rectClient.left + m_nCharWidth * ptEnd.x;

		dc.InvertRect(rectInvert);
		return;
	}

	// we have a multi line selection...lets see if any of it is currently on display
	// this means that the start selection OR the end selection OR the piece of the
	// selection between the start and the end is on the screen, then
	// there is SOME selection that needs to be painted, if not, bail out...

	// nScrollOffset shows us the bottom line currently displayed on the screen.
	// nTopLine shows us the top line currently displayed on the screen.
	// OrderPoints orders the selection points such that the start of the selection
	// is lower on the screen.  This means that for our selection to be on the screen,
	// the selection start has to be smaller than nTopLine and the selection end
	// has to be larger than nScrollOffset

	if(ptStart.y > nTopLine || ptEnd.y < nScrollOffset)
		return;

	// well if we've gotten here, then there is at least SOME selection showing on the screen
	// now we gotta figure out what part is showing.  We'll start with the selection start
	// and start painting up from there:
	CRect rectInvert;
	int y = 0;
	//POSITION pos = m_pLineDefs->GetHeadPosition();

	LineDefBufferType::iterator it = _lineDefs.begin();
	LineDefBufferType::iterator end = _lineDefs.end();
	int t = 0;
	while(it != end && t < ptStart.y)
	{
		++it;
		++t;
	}
	
	for(int i = ptStart.y; i <= ptEnd.y; i++)
	{
		if(it == end)
			break;

		CLineDef::SpLineDef line = *it++;

		if(i < nScrollOffset)
			continue;

		// the start of our selection is on the screen, since its multi line
		// we need to go from our x position to the start of the line
		if(i == ptStart.y)
		{
			rectInvert.left = m_rectClient.left;
			rectInvert.right = m_rectClient.left + m_nCharWidth * ptStart.x;
			rectInvert.top = (m_rectClient.bottom - ( m_nCharHeight * (ptStart.y-nScrollOffset) ))-m_nCharHeight;
			rectInvert.bottom = rectInvert.top + m_nCharHeight;
			dc.InvertRect(&rectInvert);
		}
		else if(i == ptEnd.y)
		{
			rectInvert.left = m_rectClient.left + m_nCharWidth * ptEnd.x;
			rectInvert.right = m_rectClient.left + (line->nLength * m_nCharWidth);
			rectInvert.top = (m_rectClient.bottom - ( m_nCharHeight * (ptEnd.y-nScrollOffset) ))-m_nCharHeight;
			rectInvert.bottom = rectInvert.top + m_nCharHeight;
			dc.InvertRect(&rectInvert);
			break;
		}
		else if(i < nTopLine)
		{
			rectInvert.left = m_rectClient.left;
			rectInvert.right = m_rectClient.left + (line->nLength * m_nCharWidth);
			rectInvert.top = (m_rectClient.bottom - ( m_nCharHeight * (i-nScrollOffset) ))-m_nCharHeight;
			rectInvert.bottom = rectInvert.top + m_nCharHeight;
			dc.InvertRect(&rectInvert);
		}
		y++;
	}
}

void CTermWindow::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == ID_TIMER_MOUSE_EVENT)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		OnMouseMove(MK_LBUTTON, point);
		return;
	}
	CWnd::OnTimer(nIDEvent);
}

void CTermWindow::TimestampLog()
{
	m_bTimestampLog = !m_bTimestampLog;
}

inline void CTermWindow::Log(CString message)
{
    if(m_bLogging) {
        ASSERT(_pLogFile.get() != NULL);
	    CString strMessage(_T("# "));
	    if(m_bTimestampLog)
        {
            CTime t = CTime::GetCurrentTime();
		    strMessage = strMessage + t.Format(_T("%m:%d:%Y::%H:%M:%S -"));
        }

	    strMessage = strMessage + message;
	    _pLogFile->Write(strMessage, strMessage.GetLength());
	    _pLogFile->Write("\r\n", 2);
    }
}
void CTermWindow::SetDocument(CIfxDoc *pDoc)
{
	m_pDocument = pDoc;
}

void CTermWindow::SetTopBottomFlag(BOOL flag) {
    m_bIsUpperWindow = flag;
}

CIfxDoc * CTermWindow::GetDocument()
{
	return m_pDocument;
}
CString CTermWindow::EncodeToUTF8(LPCTSTR szSource)
{
  WORD ch;

  BYTE bt1, bt2, bt3, bt4, bt5, bt6;

  int n, nMax = _tcslen(szSource);

  CString sFinal, sTemp;

  for (n = 0; n < nMax; ++n)
  {
    ch = (WORD)szSource[n];

    if (ch == _T('='))
    {
      sTemp.Format(_T("=%02X"), ch);

      sFinal += sTemp;
    }
    else if (ch < 128)
    {
      sFinal += szSource[n];
    }
    else if (ch <= 2047)
    {
       bt1 = (BYTE)(192 + (ch / 64));
       bt2 = (BYTE)(128 + (ch % 64));

      sTemp.Format(_T("=%02X=%02X"), bt1, bt2);
            
      sFinal += sTemp;
    }
    else if (ch <= 65535)
    {
       bt1 = (BYTE)(224 + (ch / 4096));
       bt2 = (BYTE)(128 + ((ch / 64) % 64));
       bt3 = (BYTE)(128 + (ch % 64));

      sTemp.Format(_T("=%02X=%02X=%02X"), bt1, bt2, bt3);
            
      sFinal += sTemp;
    }
    else if (ch <= 2097151)
    {
       bt1 = (BYTE)(240 + (ch / 262144));
       bt2 = (BYTE)(128 + ((ch / 4096) % 64));
       bt3 = (BYTE)(128 + ((ch / 64) % 64));
       bt4 = (BYTE)(128 + (ch % 64));

      sTemp.Format(_T("=%02X=%02X=%02X=%02X"), bt1, bt2, bt3, bt4);
      sFinal += sTemp;
    }
    else if (ch <=67108863)
    {
      bt1 = (BYTE)(248 + (ch / 16777216));
      bt2 = (BYTE)(128 + ((ch / 262144) % 64));
      bt3 = (BYTE)(128 + ((ch / 4096) % 64));
      bt4 = (BYTE)(128 + ((ch / 64) % 64));
      bt5 = (BYTE)(128 + (ch % 64));

      sTemp.Format(_T("=%02X=%02X=%02X=%02X=%02X"), bt1, bt2, bt3, bt4, bt5);
      sFinal += sTemp;
    }
    else if (ch <=2147483647)
    {
       bt1 = (BYTE)(252 + (ch / 1073741824));
       bt2 = (BYTE)(128 + ((ch / 16777216) % 64));
       bt3 = (BYTE)(128 + ((ch / 262144) % 64));
       bt4 = (BYTE)(128 + ((ch / 4096) % 64));
       bt5 = (BYTE)(128 + ((ch / 64) % 64));
       bt6 = (BYTE)(128 + (ch % 64));

      sTemp.Format(_T("=%02X=%02X=%02X=%02X=%02X=%02X"), 
                bt1, bt2, bt3, bt4, bt5, bt6);
      sFinal += sTemp;
    }

  }

  return sFinal;
}


// $Log: TermWindow.cpp,v $
// Revision 1.23  2011/11/08 18:14:15  ghoti20
// v4.2.8  build 15
//
// Revision 1.22  2011/08/21 22:04:53  ghoti20
// v4.2.8  build 12 convert CRLF to LF in text pasted into input with ctrl-v
//
// Revision 1.21  2011/03/10 17:23:00  ghoti20
// Added telnet terminal-type response v4.2.8.10
//
// Revision 1.20  2011/02/25 18:32:19  ghoti20
// NAWS support
//
// Revision 1.18  2011/02/11 18:38:05  ghoti20
// v4.2.8 fix  build 7 fix Telnet WILL response and build 9 savescrollback to html
//
// Revision 1.17  2010/07/27 16:50:12  ghoti_20
// v4.2.5 clickable statusbar items and line wrap setting
//
// Revision 1.16  2010/03/29 16:01:37  ghoti_20
// v4.2.4  screenshot png type added
//
// Revision 1.15  2010/02/17 03:13:43  ghoti_20
// v4.2.4 link to script forum and screenshot command
//
// Revision 1.14  2010/02/17 03:00:13  ghoti_20
// v4.2.4 link to script forum and screenshot command
//
// Revision 1.13  2010/02/17 02:38:47  ghoti_20
// v4.2.4 link to script forum and screenshot command
//
// Revision 1.12  2010/02/17 00:50:24  ghoti_20
// v4.2.4 link to script forum and screenshot command
//
// Revision 1.11  2009/11/30 23:53:38  ghoti_20
// v4.2.3 command character control and speedwalk interuption
//
// Revision 1.10  2008/10/15 03:52:40  ghoti_20
// *** empty log message ***
//
// Revision 1.9  2008/09/03 15:39:36  ghoti_20
// Build 33
//
// Revision 1.8  2008/07/15 15:44:43  ghoti_20
// Build 31
//
// Revision 1.7  2008/01/06 14:56:33  ghoti_20
// Build28
//
// Revision 1.6  2007/08/20 21:22:10  ghoti_20
// Build 26
//
// Revision 1.5  2007/07/09 18:49:36  ghoti_20
// *** empty log message ***
//
// Revision 1.4  2007/05/21 15:42:56  ghoti_20
// bug fixes and display processing change enabling input on prompt line
//
// Revision 1.3  2007/01/06 16:55:31  ghoti_20
// build 23 display speed fixes, case insensitive procedres
//
// Revision 1.2  2007/01/04 00:53:24  ghoti_20
// build 23
//
// Revision 1.1  2006/09/09 22:24:41  ghoti_20
// *** empty log message ***
//
// Revision 1.3  2004/09/04 15:57:59  kevinpcook
// removing ntrace from the project
//
// Revision 1.2  2004/09/04 15:08:18  kevinpcook
// removing TRACE from the project
//
// Revision 1.1.1.1  2004/08/22 21:28:11  kevinpcook
// no message
//