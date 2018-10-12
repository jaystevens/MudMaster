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

#include "StdAfx.h"
#include "Undo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUndo::CUndo()
{
	for (int i=0;i<MAX_UNDO_ITEMS;i++)
		m_pUndoArray[i] = NULL;
	m_nPos = 0;
	m_nFindIndex = 0;
}

CUndo::~CUndo()
{
	for (int i=0;i<MAX_UNDO_ITEMS;i++)
		if (m_pUndoArray[i] != NULL)
		{
			delete m_pUndoArray[i];
			m_pUndoArray[i] = NULL;
		}
}

void CUndo::Add(const std::string &text)
{
	Add(text.c_str());
}

BOOL CUndo::Add(LPCSTR pszText)
{
	// If at the end of the buffer, need to scroll the pointers.
	// This was easier to implement than a circle buffer.
	if (m_nPos == MAX_UNDO_ITEMS)
	{
		// Delete the on that is about to get flushed out.
		delete m_pUndoArray[0];
		m_pUndoArray[0] = NULL;
		int nTop = MAX_UNDO_ITEMS-1;
		for (int i=0;i<nTop;i++)
			m_pUndoArray[i] = m_pUndoArray[i+1];
		m_nPos--;
	}

	// Allocate a new undo object.
	UNDO *pNew = new UNDO;
	if (pNew == NULL)
		return(FALSE);
	pNew->strText = pszText;

	m_pUndoArray[m_nPos] = pNew;
	m_nPos++;
	return(TRUE);
}

CUndo::UNDO* CUndo::GetFirst()
{
	if (m_pUndoArray[0] == NULL)
		return(NULL);
	m_nFindIndex = 0;
	return(m_pUndoArray[0]);
}

CUndo::UNDO* CUndo::GetNext()
{
	if (m_nFindIndex+1 < MAX_UNDO_ITEMS)
	{
		m_nFindIndex++;
		return(m_pUndoArray[m_nFindIndex]);
	}
	return(NULL);
}

CUndo::UNDO* CUndo::GetAt(int nIndex)
{
	if (nIndex < 0 || nIndex > MAX_UNDO_ITEMS)
		return(NULL);
	return(m_pUndoArray[nIndex]);
}
