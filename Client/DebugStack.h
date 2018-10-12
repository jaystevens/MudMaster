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
#ifndef _KEVIN_H_
#define _KEVIN_H_

// Max depth of the stack.
#define STACK_DEPTH					3000

// Types of items in the stack.
#define STACK_TYPE_ACTION			0
#define STACK_TYPE_ALIAS			1
#define STACK_TYPE_EVENT			2
#define STACK_TYPE_MACRO			3
#define STACK_TYPE_USER_INPUT		4
#define STACK_TYPE_DLL_CALLBACK		5
#define STACK_TYPE_DLL_RETURN		6
#define STACK_TYPE_MATH				7
#define STACK_TYPE_EVAL_LINE		8
#define STACK_TYPE_EVAL_STATEMENT	9
#define STACK_TYPE_EVAL_CONDITION	10
#define STACK_TYPE_EVAL_MULDIV		11
#define STACK_TYPE_EVAL_PLUSMINUS	12
#define STACK_TYPE_EVAL_ONELINE		13

class CSession;
#include "Alias.h"

class CDebugStack
{
public:
	CDebugStack();
	~CDebugStack();

	// nNum is the reference number.  Like action #3.  Returns TRUE
	// if there was room on the stack.
	void Push(int nNum, int nType, const char *pszValue);
	void Push(MMScript::CAlias::PtrType alias, const std::string &value);

	// Removes an element from the stack.  It doesn't actually give you
	// the data back from this element.  The data is only used for 
	// information purposes if there is an error.  And then the only
	// data needed is the data that's STILL on the stack.
	void Pop();

	// Dumps a specific number of stack entries.  This doesn't clear the
	// entries from the stack, just prints the info.
	void DumpStack(CSession *pSession, UINT nDepth);

	UINT GetDepth()							{return(m_nCurrentDepth);}

private:
	// The stack data.
	int *m_pnNum;
	int *m_pnType;
	CString *m_pstrValue;

	// Current position in the stack.  Stack is really 1 less than the depth.
	// When the depth is 0, it's considered empty.
	UINT m_nCurrentDepth;

};

#endif