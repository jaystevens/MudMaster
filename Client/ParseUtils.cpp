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
#include "stdafx.h"
#include "Ifx.h"
#include "ParseUtils.h"
#include "ProcedureTable.h"
#include "Globals.h"
#include "VarList.h"
#include "Variable.h"
#include "Sess.h"
#include "DebugStack.h"
#include "ColorLine.h"
#include "DefaultOptions.h"
#include "ReplaceVariables.h"
#include <strsafe.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MMScript;

CParseUtils::CParseUtils()
{

}

CParseUtils::~CParseUtils()
{

}

inline bool IsEscapedCharacter( PCTSTR szString, TCHAR character )
{
	return 
		*szString == CGlobals::GetEscapeCharacter() && 
		*(szString + 1) != '\0' && 
		*(szString + 1) == character;
}

inline bool IsEscapedAt( PCTSTR szString, TCHAR procChar )
{
	return IsEscapedCharacter( szString, procChar );
}

inline bool IsEscapedComma( PCTSTR szString )
{
	return IsEscapedCharacter( szString, ',' );
}

inline bool IsEscapedRightParen( PCTSTR szString )
{
	return IsEscapedCharacter( szString, ')' );
}

// Pass in strText to look for the first black statement in 
// the string.  A block statement will be enclosed by the
// blockstart and blockend chars from _config.  If those are
// no present, spaces are used to determine the block.  strText
// is modified to point the the next character after the block
// statement.  Or an Empty string if nothing is left.
BOOL CParseUtils::FindStatement(CSession *pSession, CString &strText, CString &strResult)
{
	ASSERT(NULL != pSession);

	strResult.Empty();
	//strText.TrimRight();

	const char *ptrInc = strText;
	const char *ptrInc1 = strText;
	while(*ptrInc != '\0' && (*ptrInc == ' '  || *ptrInc == '\t'))
		ptrInc++;

	if(*ptrInc == '\0')
		return TRUE;

	char chEndChar = CGlobals::GetBlockEndCharacter();
	char chStartChar = CGlobals::GetBlockStartCharacter();
	char chEscape = CGlobals::GetEscapeCharacter();

	int nBlockCount = 0;
	if (*ptrInc != chStartChar)
	{
		chEndChar = ' ';
		chStartChar = ' ';
		// If we are using spaces for parameter separators we
		// need to start it at one since we will never encounter
		// a starting space.
		nBlockCount = 1;
	}

	int nProcCount = 0;
	int nPotentialProcCount = 0;

	std::string buffer;

	char ch1 = '\0';

	TCHAR procChar = CGlobals::GetProcedureCharacter();

	while(*ptrInc != '\0')
	{
		char ch = *ptrInc;
		ch1 = *(ptrInc+1);

		if (ch == chEscape && ch1 == procChar)
		{
			buffer.push_back('\\');
			buffer.push_back(procChar);
			ptrInc += 2;
			continue;
		}

		// Need to watch for procedures.  Each time we find a procedure
		// we need to look for a matched set of parens.
		if (ch == procChar)
		{
			nPotentialProcCount++;
			ptrInc++;
			buffer.push_back(ch);
			continue;
		}
		//if we've seen an @ and now a ( it is a procedure
		if (ch == '(' && nPotentialProcCount)
		{
			nProcCount++;
			nPotentialProcCount--;  // a real procedure not just potential
			ptrInc++;
			buffer.push_back(ch);
			continue;
		}

		// If the user wants to print a closing paren while inside
		// a procedure definition, they need to use the escape char.
		if (ch == chEscape && ch1 == ')')
		{
			buffer.push_back('\\');
			buffer.push_back(')');
			ptrInc++;
			continue;
		}

		if (ch == ')' && nProcCount)
			nProcCount--;

		if (ch == chEndChar && !nProcCount)
		{
			nBlockCount--;
			if (!nBlockCount)
			{
				ptrInc++;
				break;
			}
		}

		// If this is the first block symbol we need to skip over it, it should not
		// be part of our result string.
		if (ch == chStartChar && !nBlockCount)
		{
			nBlockCount++;
			ptrInc++;
			continue;
		}
		
		if (ch == chStartChar && !nProcCount)
			nBlockCount++;

		buffer.push_back(ch);
		ptrInc++;
	}

	if(nProcCount > 0)
	{
		CString strMessage;
		strMessage.Format("Mismatched parens processing text:\r\nThis Line-->[%s]\n", strText);
		CGlobals::PrintDebugText(pSession, strMessage);
		//CGlobals::PrintDebugText(pSession, "Mismatched Parens");
	}

	if (ch1 != '\0')
		strText = strText.Right(strText.GetLength()-(ptrInc - ptrInc1));
	else
		strText.Empty();

	strResult = buffer.c_str();
	
	return TRUE;
}


// This function is used only for findind the @PreTrans procedure
// and replacing the vars in it.  Pretranlation is used on commands
// that you want a variable to be replaced before it is actually
// executed.  For example:  /mac f1 $Target, when you press F1
// $Target gets translated.  You may want to replace the variable
// when the macro is created, and just store the value in the macro.
// /mac f1 @PreTrans($Target), would replace the variable before 
// the macro is defined.
void CParseUtils::PretranslateString(CSession *pSession, CString &strLine)
{
	if(pSession->ReadMode())
		return;

	CString strPreTrans;
	strPreTrans.Format(_T("%cPreTrans("), CGlobals::GetProcedureCharacter());
	if (strLine.Find(strPreTrans) == -1)
		return;

	BOOL bFoundProc = FALSE;
	BOOL bFoundProcParen = FALSE;
	int  nProcParenCount = 0;
	CString strText;
	CString strProcName;
	CString strTemp;
	int nIndex = 0;
	int nLen = strLine.GetLength();

	TCHAR procChar = CGlobals::GetProcedureCharacter();

	while(nIndex < nLen)
	{
		if (bFoundProc)
		{
			if (strLine[nIndex] == '(')
			{
				bFoundProcParen = TRUE;
				nProcParenCount++;
			}

			// Can use the escape character to force a closing parent without meaning part of the procedure.
			if (strLine[nIndex] == CGlobals::GetEscapeCharacter() && nIndex+1 < nLen && strLine[nIndex+1] == ')')
			{
				strProcName += CGlobals::GetEscapeCharacter();
				strProcName += ')';
				nIndex += 2;
				continue;
			}

			if (strLine[nIndex] == ')')
				nProcParenCount--;

			// We are collecting a procedure name until we read
			// a final closing paren.
			if (bFoundProcParen && !nProcParenCount)
			{
				// No matter what, there is no longer a possibility
				// of looking for a variable.
				bFoundProc = FALSE;

				strProcName += ')';
				nIndex++;

				// Only procedure we want to process here is the
				// @PreTrans procedure.
				if (strProcName.Find("PreTrans") == 0)
				{
					std::string procResult;
					CProcedureTable::ProcedureVar( 
						pSession, 
						static_cast<LPCTSTR>( strProcName ), 
						procResult );
					strTemp += procResult.c_str();
					continue;
				}

				strTemp += CGlobals::GetProcedureCharacter() + strProcName;
				continue;
			}
			else
			{
				strProcName += strLine[nIndex];
				nIndex++;
				continue;
			}
		} // if (bFoundProc)

		if (strLine[nIndex] == procChar)
		{
			bFoundProc = TRUE;
			bFoundProcParen = FALSE;
			nProcParenCount = 0;
			strProcName.Empty();
			nIndex++;
			continue;
		}

		if (strLine[nIndex] == CGlobals::GetEscapeCharacter() && nIndex+1 < nLen && strLine[nIndex+1] == procChar)
		{
			strTemp += procChar;
			nIndex += 2;
			continue;
		}

		strTemp += strLine[nIndex];
		nIndex++;
	} // while(nIndex < nLen)

	strLine = strTemp;
}

std::string CParseUtils::FindParam( PCTSTR szText, CString &strResult )
{
	std::string retval;
	PCTSTR szPtr = szText;

	strResult.Empty();
	if( NULL != szText && '\0' != szText[0] )
	{
		int nNestedProcs = 0;
		TCHAR procChar = CGlobals::GetProcedureCharacter();

		while( '\0' != *szPtr )
		{
			// Might be using the escape char to force an @.
			if( IsEscapedAt( szPtr, procChar ) )
			{
				strResult += procChar;
				szPtr += 2;
			}
			else if( IsEscapedComma( szPtr ) )
			{
				strResult += ',';
				szPtr += 2;
			}
			else if( IsEscapedRightParen( szPtr ) )
			{
				strResult += ')';
				szPtr += 2;
			}
			else if( nNestedProcs && *szPtr == ')' )
			{
				nNestedProcs--;
			}
			else if( *szPtr == procChar )
			{
				nNestedProcs++;
			}
			else if( !nNestedProcs && *szPtr == ',' )
			{
				szPtr++;
				break;
			}

			strResult += *szPtr;
			szPtr++;
		}
	}

	if( *szPtr + 1 != '\0' )
		retval = szPtr;

	return retval;
}

void CParseUtils::ReplaceText(
	CSession *pSession, 
	CColorLine::SpColorLine &line, 
	int nStart, 
	int nLength, 
	CString& strNew)
{
	// Make a new color line to receive the subbed text into
	CColorLine aColorLine(pSession->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), pSession->GetOptions().terminalOptions_.GetTerminalWindowBackColor());
	//Give the new copy the same append state as the original
	aColorLine.AppendNext(line->AppendNext());

	// Copy the beginning of the line up to the start of the replacement
	CCharFormat *pCharFormat = line->GetColorBuffer();
	CString strLine = line->GetTextBuffer();
	int nReplacementLength = strNew.GetLength();
	int nIndex = 0;
	aColorLine.SetFormat(pCharFormat[0]);

	for(nIndex=0;nIndex<nStart;nIndex++)
	{
		aColorLine.SetFormat(pCharFormat[nIndex]);
		aColorLine.Add(strLine.GetAt(nIndex));
	}

	aColorLine.SetFormat(pCharFormat[nIndex]);
	for(int i = 0; i < nReplacementLength; i++)
	{
		if(i < nLength) //original string being replaced length
			aColorLine.SetFormat(pCharFormat[nStart+i]);
		else
		{
			//if(nStart >0)
				aColorLine.SetFormat(pCharFormat[nStart + nLength - 1]); //use last original colour of text being replaced
			//else
			//	aColorLine.SetFormat(pCharFormat[strLine.GetLength()- (strNew.GetLength() - nLength) -1]);
		}
		aColorLine.Add(strNew.GetAt(i));
	}

	nIndex += nLength;

	for(;nIndex < strLine.GetLength();nIndex++)
	{
		aColorLine.SetFormat(pCharFormat[nIndex]);
		aColorLine.Add(strLine.GetAt(nIndex));
	}

	*line = aColorLine;
}

void CParseUtils::ReplaceVariables(
	CSession *pSession, 
	CString &strLine, 
	BOOL bQuoteStrings )
{
	ATLASSERT( NULL != pSession );

	CReplaceVariables variableReplacer( 
		pSession, strLine, bQuoteStrings ? true : false );
	variableReplacer.execute();
}

void CParseUtils::Commatize(CString &strNum)
{
	CString strTemp(strNum);
	strNum.Empty();
	int nLen = strTemp.GetLength();
	int nNumSections = nLen / 3;
	int nExtras = nLen % 3;
	if (nExtras)
		nNumSections++;
	int nStartPos = 0;
	for (int i=0;i<nNumSections;i++)
	{
		if (!i && nExtras)
		{
			strNum = strTemp.Left(nExtras);
			nStartPos += nExtras;
		}
		else
		{
			if (i)
				strNum += ",";
			strNum += strTemp.Mid(nStartPos,3);
			nStartPos += 3;
		}
	}
}

BOOL CParseUtils::IsNumber(LPCSTR pszText)
{
	while(*pszText)
	{
		if (!isdigit(*pszText))
			return(FALSE);
		pszText++;
	}
	return(TRUE);
}

BOOL CParseUtils::EvaluateLine(CSession *pSession, LPCSTR pszLine)
{
	CString strLine(pszLine);
	int nLen = strLine.GetLength();
	int nIndex = 0;
	int nLeftIndex;
	BOOL bInQuote = FALSE;
	BOOL bNot = FALSE;
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_LINE, pszLine);
	while(nIndex < nLen)
	{
		if (!bInQuote && strLine.GetAt(nIndex) == ')')
		{
			// Found an closing paren operator, need to evaluate
			// the statement inside it.
			nLeftIndex = CParseUtils::LeftToToken(strLine,nIndex,"(");
			// The left index could only be zero is a matching left
			// paren was not found.
			if (nLeftIndex == 0)
			{
				CGlobals::PrintDebugText(pSession, CString("Missing opening paren: ")+pszLine);
				pSession->DebugStack().DumpStack(
					pSession, pSession->GetOptions().debugOptions_.DebugDepth());
				pSession->DebugStack().Pop();
				return(FALSE);
			}
			// Check for NOT operator before parenthesis
			if (strLine.GetAt(nLeftIndex-1) == '!')
				bNot = TRUE;

			CString strStatement(strLine.Mid(nLeftIndex,nIndex-nLeftIndex));
			pSession->DebugStack().Push(0, STACK_TYPE_EVAL_STATEMENT, strStatement);
			BOOL bResult = EvaluateStatement(pSession, strStatement);
			pSession->DebugStack().Pop();
			if (bNot)
				bResult = !bResult;  //negate result if statement was preceeded by !

			if (!bResult)
			{
				// Replace what was in the parens with something that evaluates to false
				strLine = strLine.Left(nLeftIndex-1) + CString("0") + strLine.Right(nLen-nIndex-1);
			}
			else
			{

			// Replace what was in the parens with something that evaluates
			// to TRUE.
				strLine = strLine.Left(nLeftIndex-1) + CString("1") + strLine.Right(nLen-nIndex-1);
			}
			nLen = strLine.GetLength();
			nIndex = 0;
			continue;
		}

		if (strLine.GetAt(nIndex) == '\\' && nIndex+1 < nLen && strLine.GetAt(nIndex+1) == '"')
		{
			nIndex += 2;
			continue;
		}

		if (strLine.GetAt(nIndex) == '"')
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;

		nIndex++;
	}
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_STATEMENT, strLine);
	BOOL bResult = EvaluateStatement(pSession, strLine);
	pSession->DebugStack().Pop(); // STACK_TYPE_EVALUATE_STATEMENT
	pSession->DebugStack().Pop(); // STACK_TYPE_EVALUATE_LINE
	return bResult;
}

void CParseUtils::EscapeQuote(LPCSTR pszText, CString &strQuoted)
{
	// Make a quick check to see if there actual are any quotes
	// in the text.
	if (strchr(pszText,'"') == NULL)
	{
		strQuoted = pszText;
		return;
	}

	while(*pszText)
	{
		if (*pszText == '"')
			strQuoted += CString(CGlobals::GetEscapeCharacter() ) + '"';
		else
			strQuoted += *pszText;
		pszText++;
	}
}
int CParseUtils::LeftToToken(LPCSTR pszText, int nStart)  //overload handling both && and ||
{
    LPCSTR pszToken1 = "&&";
    LPCSTR pszToken2 = "||";
	BOOL bInQuote = FALSE;
	int nTokenLen = strlen(pszToken1);
	int nMaxPrev = nTokenLen - 1;
	while(nStart > -1)
	{
		if (*(pszText+nStart) == '"' && nStart > 0 && *(pszText+nStart-1) == '\\')
		{
			nStart -= 2;
			continue;
		}

		if (*(pszText+nStart) == '"')
		{
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;
			nStart--;
			continue;
		}

		if (bInQuote)
		{
			nStart--;
			continue;
		}

		if (nStart-nMaxPrev > -1 && (!memcmp(pszText+(nStart-nMaxPrev),pszToken1,nTokenLen) ||!memcmp(pszText+(nStart-nMaxPrev),pszToken2,nTokenLen) ))
			return(nStart+1);
		nStart--;
	}
	// If fell out of the loop it is because we reached the start of
	// the buffer, return index 0.
	return(0);
}

int CParseUtils::LeftToToken(LPCSTR pszText, int nStart, LPCSTR pszToken)
{
	BOOL bInQuote = FALSE;
	int nTokenLen = strlen(pszToken);
	int nMaxPrev = nTokenLen - 1;
	while(nStart > -1)
	{
		if (*(pszText+nStart) == '"' && nStart > 0 && *(pszText+nStart-1) == '\\')
		{
			nStart -= 2;
			continue;
		}

		if (*(pszText+nStart) == '"')
		{
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;
			nStart--;
			continue;
		}

		if (bInQuote)
		{
			nStart--;
			continue;
		}

		if (nStart-nMaxPrev > -1 && !memcmp(pszText+(nStart-nMaxPrev),pszToken,nTokenLen))
			return(nStart+1);
		nStart--;
	}
	// If fell out of the loop it is because we reached the start of
	// the buffer, return index 0.
	return(0);
}

BOOL CParseUtils::EvaluateStatement(CSession *pSession, LPCSTR pszStatement)
{
	// Remove all the spaces from the string.
	CString strStatement(pszStatement);
	int nLen = strStatement.GetLength();
	int nIndex = 0;
	int nLeftIndex, nRightIndex;
	CString strCondition;
	BOOL bInQuote = FALSE;
	while(nIndex < nLen)
	{
		if (!bInQuote && strStatement.GetAt(nIndex) == '&' && nIndex+1 < nLen && strStatement.GetAt(nIndex+1) == '&')
		{
			// Found an && operator.
			nLeftIndex = CParseUtils::LeftToToken(strStatement,nIndex);
			nRightIndex = CParseUtils::RightToToken(strStatement,nIndex+1);
			if (!CParseUtils::EvaluateCondition(pSession, strStatement.Mid(nLeftIndex,nIndex-nLeftIndex)) || 
				 !CParseUtils::EvaluateCondition(pSession, strStatement.Mid(nIndex+2,nRightIndex-nIndex-1)))
				//return(FALSE);
			{
			// Remove those 2 statements and replace with a statement that
			// will evaluate to true.
				strStatement = strStatement.Left(nLeftIndex) + CString("0") + strStatement.Right(nLen-nRightIndex-1);
			}
			else

			// Remove those 2 statements and replace with a statement that
			// will evaluate to true.
			{
				strStatement = strStatement.Left(nLeftIndex) + CString("1") + strStatement.Right(nLen-nRightIndex-1);
			}
			nLen = strStatement.GetLength();
			nIndex = 0;
			continue;
		}

		if (strStatement.GetAt(nIndex) == '\\' && nIndex+1 < nLen && strStatement.GetAt(nIndex+1) == '"')
		{
			nIndex += 2;
			continue;
		}

		if (strStatement.GetAt(nIndex) == '"')
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;

		nIndex++;
	}

	// Look for ||
	nIndex = 0;
	bInQuote = FALSE;
	while(nIndex < nLen)
	{
		if (!bInQuote && strStatement.GetAt(nIndex) == '|' && nIndex+1 < nLen && strStatement.GetAt(nIndex+1) == '|')
		{
			// Found an || operator.
			nLeftIndex = CParseUtils::LeftToToken(strStatement,nIndex);
			nRightIndex = CParseUtils::RightToToken(strStatement,nIndex+1);
			if (!EvaluateCondition(pSession, strStatement.Mid(nLeftIndex,nIndex-nLeftIndex)) && 
				 !EvaluateCondition(pSession, strStatement.Mid(nIndex+2,nRightIndex-nIndex-1)))
			//	return(FALSE);
			{
			// Remove those 2 statements and replace with a statement that
			// will evaluate to false.
				strStatement = strStatement.Left(nLeftIndex) + CString("0") + strStatement.Right(nLen-nRightIndex-1);
			}
			else
			{

			// Remove those 2 statements and replace with a statement that
			// will evaluate to true.
				strStatement = strStatement.Left(nLeftIndex) + CString("1") + strStatement.Right(nLen-nRightIndex-1);
			}
			nLen = strStatement.GetLength();
			nIndex = 0;
			continue;
		}

		if (strStatement.GetAt(nIndex) == '\\' && nIndex+1 < nLen && strStatement.GetAt(nIndex+1) == '"')
		{
			nIndex += 2;
			continue;
		}

		if (strStatement.GetAt(nIndex) == '"')
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;

		nIndex++;
	}
	
	return(EvaluateCondition(pSession, strStatement));
}

BOOL CParseUtils::EvaluateCondition(CSession *pSession, LPCSTR pszCondition)
{
	// Expecting something in the form of: "Arithon"=="Arithon"
	// or 1 > 2, etc...
	CString strCond(pszCondition);
	strCond.TrimRight();
	strCond.TrimLeft();
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_CONDITION, pszCondition);
	// After trimming the first character must either be a quote for
	// a string or a number.  If it isn't, return FALSE.
	if (!isdigit(strCond[0]) && strCond[0] != '"' &&  strCond[0] != '-' &&  strCond[0] != '!')
	{
		CGlobals::PrintDebugText(pSession, CString("Error in condition (strings must use quotes): ")+pszCondition);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		pSession->DebugStack().Pop();
		return(FALSE);
	}

	// If this is a string condition handle it differently.  This
	// will be a bit more code but I think it will go a little faster
	// if I just do it all separately.
	if (strCond[0] == '"')
	{
		CString strLeft;
		int nLen = strCond.GetLength();
		int nIndex = 1;
		while(nIndex < nLen)
		{
			// If we find a backslash, and the next char is a quote,
			// then this is an embedded quoation, want to keep it.
			if (strCond.GetAt(nIndex) == CGlobals::GetEscapeCharacter() && nIndex+1 < nLen && strCond.GetAt(nIndex+1) == '"')
			{
				strLeft += '"';
				nIndex += 2;
				continue;
			}

			// If we find the closing quote then break out of loop.
			if (strCond.GetAt(nIndex) == '"')
				break;

			strLeft += strCond.GetAt(nIndex);
			nIndex++;
		}

		// If already at the end then it is an invalid condition.
		if (nIndex >= nLen)
		{
			CGlobals::PrintDebugText(pSession, CString("Invalid conditional: ")+pszCondition);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			pSession->DebugStack().Pop();
			return(FALSE);
		}

		// Walk the string to the next quote character and we should
		// have our operator.
		CString strOperator;
		nIndex++;
		while(nIndex < nLen)
		{
			if (strCond.GetAt(nIndex) == '"')
				break;

			strOperator += strCond.GetAt(nIndex);
			nIndex++;
		}

		// Same deal, if at end of line then it is an incomplete
		// condition.
		nIndex++;
		if (nIndex >= nLen)
		{
			CGlobals::PrintDebugText(pSession, CString("Error in condition (at end of line this seems incomplete before condition): ")+pszCondition);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			pSession->DebugStack().Pop();
			return(FALSE);
		}

		strOperator.TrimLeft();
		strOperator.TrimRight();
		if (strOperator.IsEmpty())
		{
			CGlobals::PrintDebugText(pSession, CString("Condition missing operator: ")+pszCondition);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			pSession->DebugStack().Pop();
			return(FALSE);
		}

		// Now pick up the right side of the condition.
		CString strRight;
		while(nIndex < nLen)
		{
			// If we find a backslash, and the next char is a quote,
			// then this is an embedded quoation, want to keep it.
			if (strCond.GetAt(nIndex) == CGlobals::GetEscapeCharacter() && nIndex+1 < nLen && strCond.GetAt(nIndex+1) == '"')
			{
				strRight += '"';
				nIndex += 2;
				continue;
			}

			// If we find the closing quote then break out of loop.
			if (strCond.GetAt(nIndex) == '"')
				break;

			strRight += strCond.GetAt(nIndex);
			nIndex++;
		}

		// Do the comparison.
		if (strOperator == "=" || strOperator == "==")
		{
			pSession->DebugStack().Pop();
			return(strLeft == strRight);
		}
		else if (strOperator == "!=")
		{
			pSession->DebugStack().Pop();
			return(strLeft != strRight);
		}
		else if (strOperator == ">")
		{
			pSession->DebugStack().Pop();
			return(strLeft > strRight);
		}
		else if (strOperator == "<")
		{
			pSession->DebugStack().Pop();
			return(strLeft < strRight);
		}
		else if (strOperator == ">=")
		{
			pSession->DebugStack().Pop();
			return(strLeft >= strRight);
		}
		else if (strOperator == "<=")
		{
			pSession->DebugStack().Pop();
			return(strLeft <= strRight);
		}
		else
		{
			CGlobals::PrintDebugText(pSession, CString("Invalid operator: ")+pszCondition);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			pSession->DebugStack().Pop();
			return(FALSE);
		}
	}

	// Must be a number condition.
	BOOL bNot = FALSE;
	int nIndex = 0;
	double nLeft;
	int nLen = strCond.GetLength();
	//Check for negation
	if ( strCond[0] == '!')
	{
		nIndex = 1;
		bNot =  TRUE;
		nLeft = atof(strCond.Right(nLen - 1));
	}
	else
	{
		nLeft = atof(strCond);
	}


	
	
	while(nIndex < nLen)
	{
		if (strCond.GetAt(nIndex) != ' ' && !isdigit(strCond.GetAt(nIndex)) && strCond.GetAt(nIndex) != '-')
			break;
		nIndex++;
	}

 	// Walk the string to the next space or digit, should then have
	// our operator
	CString strOperator;
	while(nIndex < nLen)
	{
		if (strCond.GetAt(nIndex) == ' ' || isdigit(strCond.GetAt(nIndex)))
			break;

		strOperator += strCond.GetAt(nIndex);
		nIndex++;
	}

	strOperator.TrimRight();
	strOperator.TrimLeft();
	// If there is not an operator, evalute the number.
	if (strOperator.IsEmpty())
	{
		pSession->DebugStack().Pop();
		if (bNot)
		{
			return((int)!nLeft);
		}
		else
		{
			return((int)nLeft);
		}
	}
	
	if (nIndex >= nLen)
	{
		CGlobals::PrintDebugText(pSession, CString("Invalid conditional: ")+pszCondition);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		pSession->DebugStack().Pop();
		return(FALSE);
	}

	if (isalpha(strCond.GetAt(nIndex)))
	{
		CGlobals::PrintDebugText(pSession, CString("Number expected on right of operator: ")+pszCondition);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		pSession->DebugStack().Pop();
		return(FALSE);
	}

	double nRight = atof(strCond.Right(nLen-nIndex));

	// Do the comparison.
	if (strOperator == "=" || strOperator == "==")
	{
		pSession->DebugStack().Pop();
		return(nLeft == nRight);
	}
	else if (strOperator == "!=")
	{
		pSession->DebugStack().Pop();
		return(nLeft != nRight);
	}
	else if (strOperator == ">")
	{
		pSession->DebugStack().Pop();
		return(nLeft > nRight);
	}
	else if (strOperator == "<")
	{
		pSession->DebugStack().Pop();
		return(nLeft < nRight);
	}
	else if (strOperator == ">=")
	{
		pSession->DebugStack().Pop();
		return(nLeft >= nRight);
	}
	else if (strOperator == "<=")
	{
		pSession->DebugStack().Pop();
		return(nLeft <= nRight);
	}
	else
	{
		CGlobals::PrintDebugText(pSession, CString("Invalid operator: ")+pszCondition);
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
		pSession->DebugStack().Pop();
		return(FALSE);
	}
}

BOOL CParseUtils::EvaluateMultDivMod(CSession *pSession, CString &strFormula)
{
	CString strNew;
	BOOL bDoMath = FALSE;
	char chLastOp = ' ';
	//long lFirstNum = 0;
	//long lSecondNum = 0;
	//long lResult = 0;
	double lFirstNum = 0;
	double lSecondNum = 0;
	double lResult = 0;
	CString strNum;
	int nLen = strFormula.GetLength();
	int nIndex = 0;
	int iFirst,iSecond;
	char ch;
	while(nIndex < nLen)
	{
		ch = strFormula.GetAt(nIndex);
		if (isdigit(ch) || (strNum.IsEmpty() && ch == '-') || ch == '.') //KW added .
		{
			strNum += ch;
			nIndex++;
			continue;
		}
		
		if (bDoMath)
		{
			bDoMath = FALSE;
			lSecondNum = atof(strNum);
			strNum.Empty();
			switch(chLastOp)
			{
				case '*' :
					lResult = lFirstNum * lSecondNum;
					break;
					
				case '/' :
					if (lSecondNum == 0)	// Division by Zero.
					{
						CGlobals::PrintDebugText(pSession, CString("Division by zero: ")+strFormula);
						pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
						return(FALSE);
					}
					lResult = lFirstNum / lSecondNum;
					break;
					
				case '%' :
					if (lSecondNum == 0)	// Division by Zero.
					{
						CGlobals::PrintDebugText(pSession, CString("Division by zero: ")+strFormula);
						pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
						return(FALSE);
					}
					iFirst = (int)lFirstNum;
					iSecond = (int)lSecondNum;
					lResult = (double)(iFirst % iSecond);
					break;
			}
			
			// If the next operator is * / or % then reset the bDoMath stuff.
			if (ch == '*' || ch == '/' || ch == '%')
			{
				bDoMath = TRUE;
				chLastOp = ch;
				lFirstNum = lResult;
				nIndex++;
				continue;
			}
   		
			// The operator must be + or -
			strNum.Format("%.8f",lResult);
			strNew += strNum;
			strNew += ch;
			nIndex++;
			strNum.Empty();
			continue;
		}
		
		if (ch == '*' || ch == '/' || ch == '%')
		{
			bDoMath = TRUE;
			chLastOp = ch;
			lFirstNum = atof(strNum);
			nIndex++;
			strNum.Empty();
			continue;
		}
		
		// Must be + or -.
		strNew += strNum;
		strNew += ch;
		nIndex++;
		strNum.Empty();
	}

	if (bDoMath)
	{
		lSecondNum = atof(strNum);
		switch(chLastOp)
		{
			case '*' :
				lResult = lFirstNum * lSecondNum;
				break;
					
			case '/' :
				if (lSecondNum == 0)	// Division by Zero.
				{
					CGlobals::PrintDebugText(pSession, CString("Division by zero: ")+strFormula);
					pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
					return(FALSE);
				}
				lResult = lFirstNum / lSecondNum;
				break;
					
			case '%' :
				if (lSecondNum == 0)	// Division by Zero.
				{
					CGlobals::PrintDebugText(pSession, CString("Division by zero: ")+strFormula);
					pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
					return(FALSE);
				}
					iFirst = (int)lFirstNum;
					iSecond = (int)lSecondNum;
					lResult = (double)(iFirst % iSecond);

				//lResult = lFirstNum % lSecondNum;
				break;
		}
			
		strNum.Format("%.8f",lResult);
		strNew += strNum;
	}
	else
		strNew += strNum;

	strFormula = strNew;
	return(TRUE);
}

BOOL CParseUtils::EvaluatePlusMinus(CSession *pSession, CString &strFormula)
{
	CString strNew;
	CString strNum;
	strNum.Empty();
	//long lResult = 0;
	double lResult = 0;
	char chLastOp = '~';
	int nLen = strFormula.GetLength();
	int nIndex = 0;
	char ch;
	while(nIndex < nLen)
	{
		ch = strFormula.GetAt(nIndex);
		if (isdigit(ch) || (strNum.IsEmpty() && ch == '-') || ch == '.')
		{
			strNum += ch;
			nIndex++;
			continue;
		}
		
		if (chLastOp == '~')
			//lResult = atol(strNum);
			lResult = atof(strNum);
		else
			switch(chLastOp)
			{
				case '+' :
					lResult += atof(strNum);
					break;
					
				case '-' :
					lResult -= atof(strNum);
					break;
					
				default :	// Unknown operator.
					CGlobals::PrintDebugText(pSession, CString("Unknown operator: ")+strFormula);
					pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
					return(FALSE);
			}
		
		strNum.Empty();
		chLastOp = ch;
		nIndex++;
	}
	if (!strNum.IsEmpty())
		if (chLastOp == '~')
			lResult = atof(strNum);
		else
			switch(chLastOp)
			{
				case '+' :
					lResult += atof(strNum);
					break;
					
				case '-' :
					lResult -= atof(strNum);
					break;
					
				default :	// Unknown operator.
					CGlobals::PrintDebugText(pSession, CString("Unknown operator: ")+strFormula);
					return(FALSE);
			}
	
	//strFormula.Format("%ld",lResult);
	strFormula.Format("%.8f",lResult);
	return(TRUE);
}
int CParseUtils::RightToToken(LPCSTR pszText, int nStart)   //overload handling both && and ||
{
    LPCSTR pszToken1 = "&&";
    LPCSTR pszToken2 = "||";
	BOOL bInQuote = FALSE;
	int nLen = strlen(pszText);
	int nTokenLen = strlen(pszToken1);
	int nMaxPrev = nTokenLen - 1;
	while(nStart < nLen)
	{
		if (*(pszText+nStart) == '\\' && nStart+1 < nLen && *(pszText+nStart+1) == '"')
		{
			nStart += 2;
			continue;
		}

		if (*(pszText+nStart) == '"')
		{
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;
			nStart++;
			continue;
		}

		if (bInQuote)
		{
			nStart++;
			continue;
		}

		if (nStart+nMaxPrev < nLen && (!memcmp(pszText+nStart,pszToken1,nTokenLen) ||!memcmp(pszText+nStart,pszToken2,nTokenLen) ))
			return(nStart-1);
		nStart++;
	}
	return(nLen-1);
}

int CParseUtils::RightToToken(LPCSTR pszText, int nStart, LPCSTR pszToken)
{
	BOOL bInQuote = FALSE;
	int nLen = strlen(pszText);
	int nTokenLen = strlen(pszToken);
	int nMaxPrev = nTokenLen - 1;
	while(nStart < nLen)
	{
		if (*(pszText+nStart) == '\\' && nStart+1 < nLen && *(pszText+nStart+1) == '"')
		{
			nStart += 2;
			continue;
		}

		if (*(pszText+nStart) == '"')
		{
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;
			nStart++;
			continue;
		}

		if (bInQuote)
		{
			nStart++;
			continue;
		}

		if (nStart+nMaxPrev < nLen && !memcmp(pszText+nStart,pszToken,nTokenLen))
			return(nStart-1);
		nStart++;
	}
	return(nLen-1);
}

// Operators supported: * / % + -
BOOL CParseUtils::CalculateMath(CSession *pSession, LPCSTR pszParam, double &lNum)
{
	// Should not have any strings in the math formula.
	// Also remove any spaces.
	CString strText;
	int nLen = strlen(pszParam);
	int nIndex = 0;
	char ch;
	while(nIndex < nLen)
	{
		ch = *(pszParam+nIndex);
		if (isalpha(ch))
		{
			CGlobals::PrintDebugText(pSession, CString("Text in math: ")+pszParam);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			return(FALSE);
		}
		if (ch != ' ')
			strText += ch;
		nIndex++;
	}
	nLen = strText.GetLength();

	// To make life easy, I want to keep simplifying the string until I'm left with just addition and subtraction
	// and I'm able to just walk the string from left to right.
	
	// Start with Parens.  Need to pull out portions of the string that are surounded by parens.	
	if (strText.Find('(') != -1 || strText.Find(')') != -1)
	{
		// There are parens.  Make sure we have matched sets.
		nIndex = 0;
		int nLeftParen = 0;
		int nRightParen = 0;
		int nParenIndex = 0;
		while(nIndex < nLen)	// nLen is still set from above.
		{
			if (strText.GetAt(nIndex) == '(')
			{
				nLeftParen++;
				nParenIndex = nIndex; 	// Keep track of innermost paren for later.
			}
			if (strText.GetAt(nIndex) == ')')
				nRightParen++;
			nIndex++;
		}
		
		// If nLeftParen and nRightParen are not equal, there are a mismatched number
		// of parens in the formula.
		if (nLeftParen != nRightParen)
		{
			CGlobals::PrintDebugText(pSession, CString("Mismatched number of parens: ")+pszParam);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			return(FALSE);
		}
		
		CString strMeat;
		// nLeftParen is equal to the number of sections to replace.
		while(nLeftParen)
		{
			// Find the matching right paren.
			nIndex = nParenIndex+1;
			while(nIndex < nLen && strText.GetAt(nIndex) != ')')
				nIndex++;
			
			// This if should never get hit unless there are some major problems.
			// I already know there are matching parens.  This checks to see if
			// we hit the end of the buffer without finding it.
			if (nIndex >= nLen)
			{
				CGlobals::PrintDebugText(pSession, "Math error that should never be hit.");
				pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
				return(FALSE);
			}
			
			// Pull out the meat between the parens.  (Parens not included)
			strMeat = strText.Mid(nParenIndex+1,nIndex-nParenIndex-1);
			if (!strMeat.IsEmpty())
			{
				// Simplify it.  This will evaluate all the * % / and return a resultant string.
				pSession->DebugStack().Push(0, STACK_TYPE_EVAL_MULDIV, strMeat);
				BOOL bResult = EvaluateMultDivMod(pSession, strMeat);
				pSession->DebugStack().Pop();

				if (!bResult)
				{
					return(FALSE);
				}
				
				// We should be left with just a number after the + - function.
				pSession->DebugStack().Push(0, STACK_TYPE_EVAL_PLUSMINUS, strMeat);
				bResult = EvaluatePlusMinus(pSession, strMeat);
				pSession->DebugStack().Pop();
				if (!bResult)
				{
					return(FALSE);
				}
			}
			
			strText = strText.Left(nParenIndex) + strMeat + strText.Right(nLen-nIndex-1);
			nLen = strText.GetLength();
			
			nLeftParen--;
			
			// If there is still more to do, find the next inner-most paren.
			if (nLeftParen)
			{
				while(nParenIndex >= 0)
				{
					if (strText.GetAt(nParenIndex) == '(')
						break;
					nParenIndex--;
				}
				
				// We better have found one or something is really wrong.
				if (nParenIndex < 0)
				{
					CGlobals::PrintDebugText(pSession, CString("Can't find matching left paren: ")+pszParam);
					pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
					return(FALSE);
				}
			}
		}
	}
	
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_MULDIV, strText);
	BOOL bResult = EvaluateMultDivMod(pSession, strText);
	pSession->DebugStack().Pop();
	if (!bResult)
		return(FALSE);
				
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_PLUSMINUS, strText);
	bResult = EvaluatePlusMinus(pSession, strText);
	pSession->DebugStack().Pop();
	if (!bResult)
		return(FALSE);
	
	lNum = atof(strText);
	return(TRUE);
}
BOOL CParseUtils::CalculateMath(CSession *pSession, LPCSTR pszParam, long &lNum)
{
	// Should not have any strings in the math formula.
	// Also remove any spaces.
	CString strText;
	int nLen = strlen(pszParam);
	int nIndex = 0;
	char ch;
	while(nIndex < nLen)
	{
		ch = *(pszParam+nIndex);
		if (isalpha(ch))
		{
			CGlobals::PrintDebugText(pSession, CString("Text in math: ")+pszParam);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			return(FALSE);
		}
		if (ch != ' ')
			strText += ch;
		nIndex++;
	}
	nLen = strText.GetLength();

	// To make life easy, I want to keep simplifying the string until I'm left with just addition and subtraction
	// and I'm able to just walk the string from left to right.
	
	// Start with Parens.  Need to pull out portions of the string that are surounded by parens.	
	if (strText.Find('(') != -1 || strText.Find(')') != -1)
	{
		// There are parens.  Make sure we have matched sets.
		nIndex = 0;
		int nLeftParen = 0;
		int nRightParen = 0;
		int nParenIndex = 0;
		while(nIndex < nLen)	// nLen is still set from above.
		{
			if (strText.GetAt(nIndex) == '(')
			{
				nLeftParen++;
				nParenIndex = nIndex; 	// Keep track of innermost paren for later.
			}
			if (strText.GetAt(nIndex) == ')')
				nRightParen++;
			nIndex++;
		}
		
		// If nLeftParen and nRightParen are not equal, there are a mismatched number
		// of parens in the formula.
		if (nLeftParen != nRightParen)
		{
			CGlobals::PrintDebugText(pSession, CString("Mismatched number of parens: ")+pszParam);
			pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
			return(FALSE);
		}
		
		CString strMeat;
		// nLeftParen is equal to the number of sections to replace.
		while(nLeftParen)
		{
			// Find the matching right paren.
			nIndex = nParenIndex+1;
			while(nIndex < nLen && strText.GetAt(nIndex) != ')')
				nIndex++;
			
			// This if should never get hit unless there are some major problems.
			// I already know there are matching parens.  This checks to see if
			// we hit the end of the buffer without finding it.
			if (nIndex >= nLen)
			{
				CGlobals::PrintDebugText(pSession, "Math error that should never be hit.");
				pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
				return(FALSE);
			}
			
			// Pull out the meat between the parens.  (Parens not included)
			strMeat = strText.Mid(nParenIndex+1,nIndex-nParenIndex-1);
			if (!strMeat.IsEmpty())
			{
				// Simplify it.  This will evaluate all the * % / and return a resultant string.
				pSession->DebugStack().Push(0, STACK_TYPE_EVAL_MULDIV, strMeat);
				BOOL bResult = EvaluateMultDivMod(pSession, strMeat);
				pSession->DebugStack().Pop();

				if (!bResult)
				{
					return(FALSE);
				}
				
				// We should be left with just a number after the + - function.
				pSession->DebugStack().Push(0, STACK_TYPE_EVAL_PLUSMINUS, strMeat);
				bResult = EvaluatePlusMinus(pSession, strMeat);
				pSession->DebugStack().Pop();
				if (!bResult)
				{
					return(FALSE);
				}
			}
			
			strText = strText.Left(nParenIndex) + strMeat + strText.Right(nLen-nIndex-1);
			nLen = strText.GetLength();
			
			nLeftParen--;
			
			// If there is still more to do, find the next inner-most paren.
			if (nLeftParen)
			{
				while(nParenIndex >= 0)
				{
					if (strText.GetAt(nParenIndex) == '(')
						break;
					nParenIndex--;
				}
				
				// We better have found one or something is really wrong.
				if (nParenIndex < 0)
				{
					CGlobals::PrintDebugText(pSession, CString("Can't find matching left paren: ")+pszParam);
					pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
					return(FALSE);
				}
			}
		}
	}
	
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_MULDIV, strText);
	BOOL bResult = EvaluateMultDivMod(pSession, strText);
	pSession->DebugStack().Pop();
	if (!bResult)
		return(FALSE);
				
	pSession->DebugStack().Push(0, STACK_TYPE_EVAL_PLUSMINUS, strText);
	bResult = EvaluatePlusMinus(pSession, strText);
	pSession->DebugStack().Pop();
	if (!bResult)
		return(FALSE);
	
	lNum = atol(strText);
	return(TRUE);
}

// Checks to see if one string starts with another.  Used for
// testing paritally typed command names.
BOOL CParseUtils::IsPartial(LPCSTR pszLookFor, LPCSTR pszText)
{
	while(*pszLookFor && *pszText)
	{
		if (*pszLookFor != *pszText)
			return(FALSE);
		pszLookFor++;
		pszText++;
	}
	// if the loop terminated because pszText reached the end
	// before pszLookFor, then they cannot be matches.
	return(*pszLookFor == '\x0');
}

BOOL CParseUtils::IsWalkable(const std::string &line)
{
	static CString strWalkable("nsewud");

	BOOL bFoundAlpha = FALSE;

	const char *pszText = &line[0];

	while(*pszText)
	{
		// Not a speedwalk if there is a digit as the last
		// character.
		if (isdigit(*pszText) && *(pszText+1) == '\x0')
			return(FALSE);
		
		// Not a speedwalk if it isn't one of the movementcharacter
		// or not a digit.
		if (!isdigit(*pszText) && strWalkable.Find(*pszText) == -1)
			return(FALSE);

		// Keep track of if we found an alpha char.  If it is just
		// a string of numbers it isn't a speedwalk.
		if (isalpha(*pszText))
			bFoundAlpha = TRUE;

		*pszText++;
	}
	return(bFoundAlpha);
}

BOOL CParseUtils::GetLines(const CString &line, std::vector<CString> &lines)
{
	if(line.IsEmpty())
		return true;

	BOOL bEscaped = FALSE;
	BOOL bContinuation = FALSE;
	BOOL bInQuote = FALSE;
	int nCommandToListNested = 0;
	int nNested = 0;

	CString buffer;
	CString bufferTemp;

	/*
	const char chEscape = CGlobals::GetSeparatorCharacter();
	const char chStart = CGlobals::GetBlockStartCharacter();
	const char chEnd = CGlobals::GetBlockEndCharacter();
	const char chSep = CGlobals::GetSeparatorCharacter();
	*/

	const char *ptr1 = static_cast<const char *>(line);

	while(*ptr1 != '\0')
	{
		switch(*ptr1)
		{
		case ' ':
			if(bContinuation)
			{
			 while(*ptr1 == ' ' || *ptr1 == '\t')
			 {
				 ptr1++;
			 }
			bContinuation = FALSE;
			ptr1--;  //back up so this character gets parsed
			break;
			}
			else
			{
			buffer.AppendChar(*ptr1);
			break;
			}
		case '\t':
			if(bContinuation)
			{
			 while(*ptr1 == ' ' || *ptr1 == '\t')
			 {
				 ptr1++;
			 }
			bContinuation = FALSE;
			ptr1--; //back up so this character gets parsed
			break;
			}
			else
			{
			buffer.AppendChar(*ptr1);
			break;
			}
		case '\n':
			if(buffer.Right(1) == "\\" || buffer.Right(1) == "_")  //continuation character just before return
			{
				//remove the continuation character and don't execute yet.
				buffer = buffer.Left(buffer.GetLength()-1);
				//strip trailing whitespace
				buffer.TrimRight(" \t");
				bContinuation = TRUE;
			}
			else 
			if(!bContinuation)
				buffer.AppendChar(*ptr1);
			break;
		case '\r':
			if(buffer.Right(1) == "\\" || buffer.Right(1) == "_")  //continuation character just before return
			{
				//remove the continuation character and don't execute yet.
				buffer = buffer.Left(buffer.GetLength()-1);
				//strip trailing whitespace
				buffer.TrimRight(" \t");
				bContinuation = TRUE;
			}
			else 
				buffer.AppendChar(*ptr1);
			break;
		case ';':
			if(nCommandToListNested == 0 && nNested == 0 && !bEscaped && !bInQuote)
			{
				lines.push_back(buffer.TrimLeft()); //added TrimLeft but this is not sufficient to remove all white space
				buffer = "";
			}
			else
			{
				buffer.AppendChar(';');
				bEscaped = FALSE;
			}
			break;
		case '\\':
			if(bEscaped)
			{
				buffer.Append("\\\\");
				bEscaped = FALSE;
			}
			else
			{
				bEscaped = TRUE;
			}
			break;
		case '{':
			if(bContinuation)
				bContinuation = FALSE;
			if(bEscaped)
			{
				buffer.AppendChar('\\');
				bEscaped = FALSE;
			}
			else if (!bInQuote)
			{
				nNested++;
			}
			buffer.AppendChar('{');
			break;
		case '}':
			if(bContinuation)
				bContinuation = FALSE;
			if(bEscaped)
			{
				buffer.AppendChar('\\');
				bEscaped = FALSE;
			}
			else  if (!bInQuote)
				nNested--;
			buffer.AppendChar('}');
			break;
//Use ( to indicate check for commandToList
		case '(':
			bufferTemp = buffer.Right(14);
			bufferTemp.MakeLower();
			if(bEscaped)
			{
				buffer.AppendChar('\\');
				bEscaped = FALSE;
			}
			else if (!bInQuote && (bufferTemp == "@commandtolist" || nCommandToListNested >0))
				nCommandToListNested++;
			buffer.AppendChar('(');
			break;
//Use ) to indicate end of commandToList
		case ')':
			if(bEscaped)
			{
				buffer.AppendChar('\\');
				bEscaped = FALSE;
			}
			else if (!bInQuote && nCommandToListNested > 0)
				nCommandToListNested--;
			buffer.AppendChar(')');
			break;
		case '"':
			if (bInQuote)
				bInQuote = FALSE;
			else
				bInQuote = TRUE;
			buffer.AppendChar('"');
			break;
		default:
			if(bContinuation)
				bContinuation = FALSE;
			if(bEscaped)
			{
				buffer.AppendChar('\\');
				bEscaped = FALSE;
			}
			buffer.AppendChar(*ptr1);
			break;
		}
		ptr1++;
	}

	if(!buffer.IsEmpty())
	{
		lines.push_back(buffer);
	}

	return TRUE;
}
