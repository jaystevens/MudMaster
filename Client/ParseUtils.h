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
// ParseUtils.h: interface for the CParseUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEUTILS_H__C6526543_7803_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_PARSEUTILS_H__C6526543_7803_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSession;
#include "ColorLine.h"

class CParseUtils  
{
public:
	static void ReplaceVariables(CSession *pSession, CString &strLine, BOOL bQuoteStrings = FALSE);
	static std::string FindParam( PCTSTR strText, CString& strResult );
	static BOOL FindStatement(CSession *pSession, CString &strText, CString &strResult);

	static void Commatize(CString& strNum);

	static BOOL IsWalkable(const std::string &line);

	static int LeftToToken(LPCSTR pszText, int nStart, LPCSTR pszToken);
	static int LeftToToken(LPCSTR pszText, int nStart); //overload which handles both && and ||
	static int RightToToken(LPCSTR pszText, int nStart, LPCSTR pszToken);
	static int RightToToken(LPCSTR pszText, int nStart); //overload which handles both && and ||

	static BOOL EvaluateLine(CSession *pSession, LPCSTR pszLine);
	static BOOL EvaluateStatement(CSession *pSession, LPCSTR pszStatement);
	static BOOL EvaluateCondition(CSession *pSession, LPCSTR pszCondition);
	static BOOL EvaluatePlusMinus(CSession *pSession, CString &strFormula);
	static BOOL EvaluateMultDivMod(CSession *pSession, CString &strFormula);

	static BOOL CalculateMath(CSession *pSession, LPCSTR pszParam, long &lNum);
	static BOOL CalculateMath(CSession *pSession, LPCSTR pszParam, double &lNum);
	static BOOL IsPartial(LPCSTR pszLookFor, LPCSTR pszText);
	static BOOL IsNumber(LPCSTR pszText);
	static void EscapeQuote(LPCSTR pszText, CString &strQuoted);
	// This function is used only for findind the @PreTrans procedure
	// and replacing the vars in it.  Pretranlation is used on commands
	// that you want a variable to be replaced before it is actually
	// executed.  For example:  /mac f1 $Target, when you press F1
	// $Target gets translated.  You may want to replace the variable
	// when the macro is created, and just store the value in the macro.
	// /mac f1 @PreTrans($Target), would replace the variable before 
	// the macro is defined.	
	static void PretranslateString(CSession *pSession, CString &strLine);
	static BOOL GetLines(const CString &line, std::vector<CString> &lines);

	static void ReplaceText(
		CSession *pSession, 
		CColorLine::SpColorLine &line, 
		int nStart, 
		int nLength, 
		CString &strNew);

	CParseUtils();
	virtual ~CParseUtils();

};

#endif // !defined(AFX_PARSEUTILS_H__C6526543_7803_11D3_BCD6_00E029482496__INCLUDED_)
