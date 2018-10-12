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
// ColorLine.cpp: implementation of the CColorLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorLine.h"
#include "CharFormat.h"
#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct CColorLine::ColorLineImpl
{
	ColorLineImpl(BYTE fore, BYTE back)
	{
		m_pBuffer = new char[1];
		m_pBuffer[0] = '\0';

		m_pColors = new CCharFormat[1];

		m_nCurrentSize = 1;
		m_nCurrentIndex = 0;

		m_fore = fore;
		m_back = back;
		m_bAppendNext = FALSE;
	}

	ColorLineImpl( const ColorLineImpl &src )
		: m_nCurrentSize(src.m_nCurrentSize)
		, m_nCurrentIndex(src.m_nCurrentIndex)
		, m_fore(src.m_fore)
		, m_back(src.m_back)
		, m_bAppendNext(src.m_bAppendNext)
	{
 		CopyInfo(src.m_pBuffer, src.m_pColors);
	}

	~ColorLineImpl()
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;

		delete [] m_pColors;
		m_pBuffer = NULL;
	}

	void Add(char c);
	void Append(SpColorLine &line);

	void NewInfo(const char *pszText, CCharFormat *pColorBuffer);
    void CopyInfo(const char *pszText, CCharFormat *pColorBuffer);
	void AllocateStorage(int size = 100);    

	short m_nCurrentSize;
	BOOL m_bBlink;
	int m_nCurrentIndex;
	BYTE m_fore;
	BYTE m_back;
	BOOL m_bBold;
	BOOL m_bAppendNext;

	char *m_pBuffer;
	CCharFormat *m_pColors;
};

void CColorLine::ColorLineImpl::Add(char c)
{
	ASSERT(NULL != m_pBuffer);
	ASSERT(NULL != m_pColors);

	if(m_nCurrentIndex >= m_nCurrentSize-1)
		AllocateStorage();

	m_pBuffer[m_nCurrentIndex] = c;
	m_pColors[m_nCurrentIndex].Fore(m_fore);
	m_pColors[m_nCurrentIndex].Back(m_back);
	m_pColors[m_nCurrentIndex].Bold(m_bBold ? true : false);
	m_nCurrentIndex++;
}

void CColorLine::ColorLineImpl::Append(SpColorLine &line)
{
	for(UINT i = 0; i < strlen(line->GetTextBuffer()); i++)
	{
		m_fore = line->GetColorBuffer()[i].Fore();
		m_back = line->GetColorBuffer()[i].Back();
		m_bBold = line->GetColorBuffer()[i].Bold();
		Add(line->GetTextBuffer()[i]);
	}
}

CColorLine::CColorLine(BYTE fore, BYTE back)
: _pImpl( new ColorLineImpl(fore, back) )
{
}

CColorLine::CColorLine(const CColorLine &src)
: _pImpl( new ColorLineImpl( *src._pImpl ) )
{
}

CColorLine::~CColorLine()
{
	delete _pImpl;
}

void CColorLine::SetFormat(CCharFormat &cfFormat)
{
	_pImpl->m_fore = cfFormat.Fore();
	_pImpl->m_back = cfFormat.Back();
	_pImpl->m_bBold = cfFormat.Bold();
}

void CColorLine::AddChars(int nNumChars, char c)
{
	for(int i = 0; i < nNumChars; i++)
	{
		Add(c);
	}
}

void CColorLine::Add(char c)
{
	_pImpl->Add(c);
}

char * CColorLine::GetTextBuffer() const
{
	return _pImpl->m_pBuffer;
}

CCharFormat * CColorLine::GetColorBuffer() const
{
	return _pImpl->m_pColors;
}

void CColorLine::Append(SpColorLine &line)
{
	_pImpl->Append(line);
}

void CColorLine::ColorLineImpl::AllocateStorage(int size)
{
	short nLen = (short)(m_nCurrentSize + size);

	char *buffer = new char[nLen];
	if(NULL == buffer)
	{
		TRACE("OUT OF MEMORY");
		ASSERT(FALSE);
		return;
	}

	CCharFormat *colors = new CCharFormat[nLen];
	if(NULL == colors)
	{
		TRACE("OUT OF MEMORY");
		ASSERT(FALSE);
		return;
	}

	ZeroMemory(buffer, nLen);
    CopyMemory(buffer, m_pBuffer, m_nCurrentIndex);
    CopyMemory(colors, m_pColors, m_nCurrentIndex);
	/*for(int i = 0; i < m_nCurrentIndex; i++)
	{
		buffer[i] = m_pBuffer[i];
		colors[i] = m_pColors[i];
	}*/

	delete [] m_pBuffer;
	m_pBuffer = NULL;

	delete [] m_pColors;
	m_pColors = NULL;

	m_pBuffer = buffer;
	m_pColors = colors;

	m_nCurrentSize = nLen;
}

void CColorLine::ColorLineImpl::NewInfo(const char *pszText, CCharFormat *pColorBuffer)
{
	delete [] m_pBuffer;
	m_pBuffer = NULL;

	delete [] m_pColors;
	m_pColors = NULL;

    CopyInfo(pszText, pColorBuffer);
}

// Copies the info into the bufffer
// creating the new buffer in the process.
void CColorLine::ColorLineImpl::CopyInfo(const char *pszText, CCharFormat *pColorBuffer) 
{
	m_nCurrentSize = 1;
	m_nCurrentIndex = 0;

	short nLen = (short)strlen(pszText);
	if(0 < nLen)
	{
		m_pBuffer = new char[nLen+1];
		if(NULL == m_pBuffer)
		{
			TRACE("OUT OF MEMORY");
			ASSERT(FALSE);
			return;
		}

		ZeroMemory(m_pBuffer, nLen+1);

		strcpy(m_pBuffer, pszText);

		m_pColors = new CCharFormat[nLen];
		if(NULL == m_pColors)
		{
			TRACE("OUT OF MEMORY");
			ASSERT(FALSE);
			return;
		}

		for(int i = 0; i < nLen; i++)
			m_pColors[i] = pColorBuffer[i];

		m_nCurrentSize = nLen;
		m_nCurrentIndex = nLen;
	}
	else
	{
		m_pBuffer = new char[1];
		m_pBuffer[0] = '\0';
		m_pColors = new CCharFormat[1];
	}
}

CColorLine& CColorLine::operator =(const CColorLine& src)
{
	if(this == &src)
		return *this;

	AppendNext(src.AppendNext());
	_pImpl->m_bBold = src._pImpl->m_bBold;
	_pImpl->m_back = src._pImpl->m_back;
	_pImpl->m_fore = src._pImpl->m_fore;

	_pImpl->NewInfo(src.GetTextBuffer(), src.GetColorBuffer());
	return *this;
}

BOOL CColorLine::AppendNext() const
{
	return _pImpl->m_bAppendNext;
}

void CColorLine::AppendNext(BOOL bAppendNext)
{
	_pImpl->m_bAppendNext = bAppendNext;
}
