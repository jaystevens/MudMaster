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
#pragma	once

// Temporary object used to handle overlapping buffers in a substitution.
//extern CTextLine _tlSub;

#include "TriggerList.h"
#include "ColorLine.h"

class CSession;

namespace MMScript
{
	class CSub;

	class CSubList : public CTriggerList
	{
		static const DWORD DEFAULT_SUB_ARRAY_SIZE = 150;
		static const DWORD DEFAULT_SUB_GROW_SIZE = 20;

		// This is the number of time it will do a sub before
		// it breaks out of the loop.  When a sub is found the
		// list starts over and looks to see if there are any
		// more to be replaced.  A problem with this is if a
		// user has two subs that continually replace each other
		// it gets in an endless loop.  This will limite errors
		// such as those to 10.
		static const DWORD MAX_SUBS	= 2;

	public:
		CSubList();
		~CSubList();

		void CheckSub(CSession *pSession, CColorLine::SpColorLine &line);

		// Adds a substitute to the list.  The sbustittiue is inserted into the
		// list in sorted order.
		CTrigger *Add(const CString& strTrigger, const CString& strSub, const CString& strGroup);

	private:
		BOOL ReplaceText(CString &strLine, CCharFormat *pColorBuffer, CCharFormat *pReplaceColorBuffer, int nStart, int nOriginalLength, LPCSTR pszReplace, int nReplacementLength, BOOL bWholeLine);
	};
}