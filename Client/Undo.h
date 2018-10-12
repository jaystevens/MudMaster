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
#ifndef _UNDO_H_
#define _UNDO_H_
#define MAX_UNDO_ITEMS		50

class CUndo
{
public:
	struct UNDO
	{
		// This is the text that undoes the action.  For instance, if you
		// were to /unact something, the line in the undo buffer would
		// be the /action that would restore it.
		CString strText;
	};

	CUndo();
	~CUndo();

	// Adds a string to the undo buffer.
	BOOL Add(LPCSTR pszText);
	void Add(const std::string &text);

	// Gets the first undo item in the list.  This needs to be 
	// called before GetNext.
	UNDO* GetFirst();
	// Gets the next item in the list, GetFirst needs to be called
	// first.
	UNDO* GetNext();
	// Returns the undo item as a specific index.
	UNDO* GetAt(int nIndex);
	// Gets the index of the current find item.
	int   GetFindIndex()			{ return(m_nFindIndex); }

private:
	int  m_nPos;
	int  m_nFindIndex;
	UNDO *m_pUndoArray[MAX_UNDO_ITEMS];
};
#endif