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
// SeException.cpp: implementation of the CSeException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ifx.h"
#include "SeException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define CASE(nSeCode,CsString) \
	case EXCEPTION_##nSeCode: \
		CsString.Format(_T("Exception %s (0x%.8x) at address 0x%.8x."),_T(#nSeCode),EXCEPTION_##nSeCode,m_pExcPointers->ExceptionRecord->ExceptionAddress); \
		break;

void SeTranslator(UINT nSeCode, _EXCEPTION_POINTERS *pExcPointers)
{
	TRACE("Creating a new CSeException");
	throw new CSeException(nSeCode, pExcPointers);
}

IMPLEMENT_DYNAMIC(CSeException,CException)

CSeException::~CSeException()
{
}

CSeException::CSeException(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{ 
	m_nSeCode = nSeCode;
	m_pExcPointers = pExcPointers;
}

CSeException::CSeException(const CSeException & CseExc)
{
	m_nSeCode = CseExc.m_nSeCode;
	m_pExcPointers = CseExc.m_pExcPointers;
}

UINT CSeException::GetSeCode()
{
	return m_nSeCode;
}

_EXCEPTION_POINTERS* CSeException::GetSePointers()
{
	return m_pExcPointers;
}

PVOID CSeException::GetExceptionAddress()
{
	return m_pExcPointers->ExceptionRecord->ExceptionAddress;
}

void CSeException::Delete(void)
{
#ifdef _DEBUG
	m_bReadyForDelete = TRUE;
#endif
	delete this;
}

int CSeException::ReportError(UINT nType/* = MB_OK*/, UINT nIDHelp/* = 0*/)
{
	int rc;
	CString strMessage;

	GetErrorMessage(strMessage);
	rc = AfxMessageBox(strMessage,nType,nIDHelp);
	
	return rc;
}

BOOL CSeException::GetErrorMessage(CString & CsErrDescr, PUINT pnHelpContext/* = NULL*/)
{
	BOOL rc = TRUE;

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	switch (m_nSeCode)    {   
		CASE(ACCESS_VIOLATION,CsErrDescr);
		CASE(DATATYPE_MISALIGNMENT,CsErrDescr);
		CASE(BREAKPOINT,CsErrDescr);
		CASE(SINGLE_STEP,CsErrDescr);
		CASE(ARRAY_BOUNDS_EXCEEDED,CsErrDescr);
		CASE(FLT_DENORMAL_OPERAND,CsErrDescr);
		CASE(FLT_DIVIDE_BY_ZERO,CsErrDescr);
		CASE(FLT_INEXACT_RESULT,CsErrDescr);
		CASE(FLT_INVALID_OPERATION,CsErrDescr);
		CASE(FLT_OVERFLOW,CsErrDescr);
		CASE(FLT_STACK_CHECK,CsErrDescr);
		CASE(FLT_UNDERFLOW,CsErrDescr);
		CASE(INT_DIVIDE_BY_ZERO,CsErrDescr);
		CASE(INT_OVERFLOW,CsErrDescr);
		CASE(PRIV_INSTRUCTION,CsErrDescr);
		CASE(IN_PAGE_ERROR,CsErrDescr);
		CASE(ILLEGAL_INSTRUCTION,CsErrDescr);
		CASE(NONCONTINUABLE_EXCEPTION,CsErrDescr);
		CASE(STACK_OVERFLOW,CsErrDescr);
		CASE(INVALID_DISPOSITION,CsErrDescr);
		CASE(GUARD_PAGE,CsErrDescr);
		CASE(INVALID_HANDLE,CsErrDescr);
	default:
		CsErrDescr = _T("Unknown exception.");
		rc = FALSE;
		break;
	}

	return rc;
}

BOOL CSeException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext/* = NULL*/)
{
	ASSERT(lpszError != NULL && AfxIsValidString(lpszError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	CString strMessage;
	GetErrorMessage(strMessage);

	if ((UINT)strMessage.GetLength() >= nMaxError) {
		lpszError[0] = 0;
		return FALSE;
	} else {
		lstrcpyn(lpszError, strMessage, nMaxError);
		return TRUE;
	}
}

