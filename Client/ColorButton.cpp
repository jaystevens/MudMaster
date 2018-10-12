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
// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CMMColorButton::CMMColorButton()
{
}

CMMColorButton::~CMMColorButton()
{
}


BEGIN_MESSAGE_MAP(CMMColorButton, CButton)
	//{{AFX_MSG_MAP(CMMColorButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CMMColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(NULL != lpDrawItemStruct);
	switch(lpDrawItemStruct->itemAction)
	{
	case ODA_DRAWENTIRE:
		TRACE(_T("ODA_DRAWENTIRE"));
		break;
	case ODA_FOCUS:
		TRACE(_T("ODA_FOCUS"));
		break;
	case ODA_SELECT:
		TRACE(_T("ODA_SELECT"));
		break;
	default:
		ASSERT(FALSE);
	}

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect;

	rect.CopyRect(&lpDrawItemStruct->rcItem);
	UINT state = lpDrawItemStruct->itemState;

	COLORREF crBlack = GetSysColor(COLOR_BTNFACE);
	CPen penBlack;
	penBlack.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	CBrush brushBlue(m_crColor);

	CPoint corner(3, 3);
	if (state & ODS_SELECTED)
	{
		CPen* pOldPen = pDC->SelectObject(&penBlack);
		CBrush* pOldBrush = pDC->SelectObject(&brushBlue);
		pDC->RoundRect(&rect, corner);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);

		pDC->Draw3dRect(
			rect,
			GetSysColor(COLOR_3DDKSHADOW),
			GetSysColor(COLOR_3DHILIGHT));
	}
	else
	{
		CPen* pOldPen = pDC->SelectObject(&penBlack);
		CBrush* pOldBrush = pDC->SelectObject(&brushBlue);
		pDC->RoundRect(&rect, corner);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);

		pDC->Draw3dRect(
			rect,
			crBlack,
			crBlack);
	}
}

void CMMColorButton::SetColor(COLORREF cr)
{
	m_crColor = cr;
	if (m_hWnd != NULL)
		RedrawWindow();
}
