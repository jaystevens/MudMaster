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
// StatusWindow.cpp : implementation file
//

#include "stdafx.h"

#include "ifx.h"
#include "StatusWindow.h"
#include "MemDC.h"
#include "IfxDoc.h"
#include "Sess.h"
#include "BarItem.h"
#include "ParseUtils.h"
#include "SeException.h"
#include "StatusBar.h"
#include "TWChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MudmasterColors;

CStatusWindow::CStatusWindow()
{
	m_pForeColors = NULL;
	m_pBackColors = NULL;
	m_pStatusBarInfo = NULL;

	m_crBackColor = COLOR_BLACK;
	m_crForeColor = COLOR_GRAY;
	m_pFont = NULL;
	m_pBitmap = NULL;
	//TODO: KW make the status bar up to 480 characters wide display on 3 lines
	m_arStatusBarContents.SetSize(480);
	
	for(int i = 0; i < 480; i++)
	{
		CTWChar *pChar = new CTWChar;
		pChar->Character(' ');
		BYTE attr = (BYTE)(((m_crBackColor << 4) | m_crForeColor));
		pChar->Attribute(attr, FALSE);
		m_arStatusBarContents[i] = pChar;
	}
}

CStatusWindow::~CStatusWindow()
{
	delete m_pBitmap;
	m_pBitmap = NULL;
	//TODO: KW change 80 to 160
	for(int i = 0; i < 480; i++)
	{
		CTWChar *pChar = (CTWChar *)m_arStatusBarContents[i];
		delete pChar;
		pChar = NULL;

	}
	m_arStatusBarContents.RemoveAll();
}


BEGIN_MESSAGE_MAP(CStatusWindow, CWnd)
	//{{AFX_MSG_MAP(CStatusWindow)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStatusWindow message handlers
void CStatusWindow::OnLButtonUp(UINT /*nFlags*/, CPoint point)
{
	CPoint e = point;
	int iClickPosition;
	//CRect rect;
	//GetClientRect(&rect);

	iClickPosition = (e.y / m_iStatusBarCharHeight) * m_iStatusBarWidth + ( e.x / m_iStatusBarCharWidth);
	
	CBarItem *pItem = (CBarItem*)m_pStatusBarInfo->FindItemByPosition(iClickPosition);
	if(pItem == NULL)
		return;
	else
	{
		CString strItem = pItem->Item();
		CString strItemMixed = strItem;
		if(strItem.MakeLower().Find(_T("button")) == 0)
		{
			_pSession->ExecuteCommand(strItemMixed.Mid(7));
		}
		return;
	}

}
void CStatusWindow::OnPaint() 
{
	if(m_pFont == NULL)
		return;

	if(m_pBitmap == NULL)
		return;

	CPaintDC adc(this); // device context for painting

	ASSERT(m_pFont != NULL);
	ASSERT(m_pBitmap != NULL);
	MM2K6_CMemDC dc(
		&adc, 
		m_pBackColors[m_crBackColor], 
		m_pBitmap);

	CFont *pFont = (CFont *)(dc.SelectObject(m_pFont));

	int x = 5;
	int y = 2;

	char buf[2];
	buf[1] = '\0';
	CSize szSize;
	//TODO: KW make 80 160
	for(int i = 0; i < 480; i++)
	{
		CTWChar *pChar = (CTWChar *)m_arStatusBarContents[i];
		buf[0] = pChar->Character();
		GetTextExtentPoint32(dc, buf, 1, &szSize);

		dc.SetBkColor(m_pBackColors[pChar->Back()]);
		dc.SetTextColor(m_pForeColors[pChar->Fore()]);
		dc.ExtTextOut(x, y, ETO_CLIPPED, NULL, buf, 1, NULL);
		x += szSize.cx; // +1 Would add additional pixel sapce and prevent serif overlap;
		if ((i+1) % m_iStatusBarWidth == 0)
		{
			y += szSize.cy + 2;
			x = 5;
		}
	}

	dc.SelectObject(pFont);
}

void CStatusWindow::UpdateAllOptions()
{
	Invalidate();
}

void CStatusWindow::SetBackColor(BYTE crBack)
{
	m_crBackColor = crBack;
	Invalidate();
}

void CStatusWindow::SetForeColor(BYTE crFore)
{
	m_crForeColor = crFore;
	Invalidate();
}

void CStatusWindow::ResetStatusBar()
{
	//TODO: KW make 80 160
	for(int i = 0; i < 480; i++)
	{
		BYTE attr = (BYTE)(((m_crBackColor << 4) | m_crForeColor));
		((CTWChar *)m_arStatusBarContents[i])->Character(' ');
		((CTWChar *)m_arStatusBarContents[i])->Attribute(attr, FALSE);
	}
}

void CStatusWindow::RedrawStatusBar(CSession *pSession)
{
	ResetStatusBar();

	CBarItem *pItem = (CBarItem*)m_pStatusBarInfo->GetFirst();
	while(pItem != NULL)
	{
		if (pItem->Enabled())
			pItem->DrawItem(pSession, m_arStatusBarContents);

		pItem = (CBarItem *)m_pStatusBarInfo->GetNext();
	} 

	Invalidate();
}

void CStatusWindow::DrawItem(CSession *pSession, CBarItem *pItem)
{
	pItem->DrawItem(pSession, m_arStatusBarContents);
}

void CStatusWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if(m_pBitmap == NULL)
	{
		m_pBitmap = new CBitmap();
	}

	CRect rect;
	GetClientRect(&rect);

	CClientDC dc(this);
	m_pBitmap->DeleteObject();
	m_pBitmap->CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	
}

BOOL CStatusWindow::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CStatusWindow::HideStatusBar()
{
	ResetStatusBar();
	Invalidate();
}

void CStatusWindow::SetFont(CFont *pFont)
{
	m_pFont = pFont;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(m_pFont);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_iStatusBarCharHeight = tm.tmHeight + tm.tmExternalLeading;
	m_iStatusBarCharWidth = tm.tmMaxCharWidth;
	dc.SelectObject(pOldFont);
	Invalidate();
}

int CStatusWindow::DesiredHeight()
{
	CClientDC dc(this);
	HGDIOBJ hobj = m_pFont->GetSafeHandle();
	if(hobj == NULL)
		return -1;

	CFont *pOldFont = dc.SelectObject(m_pFont);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int nHeight = tm.tmHeight + tm.tmExternalLeading;
	dc.SelectObject(pOldFont);
	return (nHeight + 4) * m_iStatusBarLines;
}

void CStatusWindow::SetColorArrays(COLORREF *pFore, COLORREF *pBack)
{
	m_pForeColors = pFore;
	m_pBackColors = pBack;
}

void CStatusWindow::SetStatusBarInfo(CMMStatusBar *pStatusBar)
{
	m_pStatusBarInfo = pStatusBar;
}


void CStatusWindow::OnDestroy() 
{
	CWnd::OnDestroy();
	
	UnHookEvents();
}

void CStatusWindow::HookEvents(CSession *pSession)
{
	__hook(&CSession::RedrawStatusBar, pSession, &CStatusWindow::RedrawStatusBar);
	_pSession = pSession;
}

void CStatusWindow::UnHookEvents()
{
	__unhook(&CSession::RedrawStatusBar, _pSession, &CStatusWindow::RedrawStatusBar);
}