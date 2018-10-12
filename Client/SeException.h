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
// SeException.h: interface for the CSeException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEEXCEPTION_H__7949E061_67F9_11D5_BCDA_00E029482496__INCLUDED_)
#define AFX_SEEXCEPTION_H__7949E061_67F9_11D5_BCDA_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSeException : public CException
{
	DECLARE_DYNAMIC(CSeException)
public:
	CSeException(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers);
	CSeException(const CSeException & CseExc);
	virtual ~CSeException();


	UINT GetSeCode(void);
	_EXCEPTION_POINTERS* GetSePointers(void);
	PVOID GetExceptionAddress(void);

	void Delete(void);
	int ReportError(UINT nType = MB_OK, UINT nIDHelp = 0);
	BOOL GetErrorMessage(CString & CsErrDescr, PUINT pnHelpContext = NULL);
	BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);

private:
	UINT m_nSeCode;
	_EXCEPTION_POINTERS* m_pExcPointers;
};

void SeTranslator(UINT nSeCode, _EXCEPTION_POINTERS *pExcPointers);

#endif // !defined(AFX_SEEXCEPTION_H__7949E061_67F9_11D5_BCDA_00E029482496__INCLUDED_)
