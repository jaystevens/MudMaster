// MemDc.cpp: implementation of the MM2K6_CMemDC class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MemDc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MM2K6_CMemDC::MM2K6_CMemDC(CDC *pDC, COLORREF crFill, CBitmap *pBitmap) : CDC(), m_pOldBitmap(NULL), m_pDC(pDC)
{
	ASSERT(m_pDC != NULL);

	m_bMemDC = ! pDC->IsPrinting();

	if(m_bMemDC)
	{
		// Step 1 - Create a Memory DC
		CreateCompatibleDC(pDC);

		// Step 2 - Get the clip area
		pDC->GetClipBox(&m_rect);
		pDC->LPtoDP(&m_rect);

		// Step 3 - See if the user passed in a bitmap
		if(pBitmap != NULL)
		{
			m_pOldBitmap = SelectObject(pBitmap);
		}
		else
		{
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
		}

		// Step 4 - Set the map mode
		SetMapMode(pDC->GetMapMode());
		pDC->LPtoDP(&m_rect);
		
		// Step 5 - Set the window org
		SetWindowOrg(m_rect.left, m_rect.top);

		// Step 6 - Fill the background
		if(crFill == RGB(255,255,255))
		{
			PatBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), WHITENESS);
		}
		else if(crFill == RGB(0,0,0))
		{
			PatBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), BLACKNESS);
		}
		else
		{
			COLORREF crOldBK = GetBkColor();
			FillSolidRect(m_rect, crFill);
			SetBkColor(crOldBK);
		}
	}
	else
	{
		m_bPrinting = pDC->m_bPrinting;
		m_hDC = pDC->m_hDC;
		m_hAttribDC = pDC->m_hAttribDC;
	}

}

MM2K6_CMemDC::~MM2K6_CMemDC()
{
	if(m_bMemDC)
	{
		m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			this, m_rect.left, m_rect.top, SRCCOPY);
		SelectObject(m_pOldBitmap);
	}
	else
	{
		m_hDC = m_hAttribDC = NULL;
	}
}
