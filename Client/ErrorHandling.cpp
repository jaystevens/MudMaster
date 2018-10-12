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
#include "ErrorHandling.h"
#include "DllHandle.h"
#include <sstream>
#include <lmerr.h>
#include <strsafe.h>
#include <iomanip>

using namespace std;

void Utilities::MMERRTRACE(DWORD dwError, PCTSTR szFmtString, ... )
{
	if( ERROR_SUCCESS != dwError )
	{
		std::string errorString;
		va_list args;
		va_start( args, szFmtString );

		try
		{
			TESTHR( ErrorHandlingUtils::GetErrorString( dwError, errorString ) );
		}
		catch(...)
		{
			::OutputDebugString("Unknown error in Utilities::MMERRTRACE in errorHandling.cpp ") ;
		}
		try
		{

			TCHAR buffer[1024] = {0};
			StringCchVPrintf( buffer, 1024, szFmtString, args );

			std::stringstream buf;
			buf 
				<< "ERROR: (0x" 
				<< hex 
				<< setw(8)
				<< static_cast<unsigned long>( dwError )
				<< ")"
				<< buffer 
				<< " [" 
				<< errorString 
				<< "]";

			::OutputDebugString( buf.str().c_str() );
		}
		catch (...)
		{
			::OutputDebugString("Unknown error in Utilities::MMERRTRACE in errorHandling.cpp:");
		}
	}
}

HRESULT Utilities::ErrorHandlingUtils::GetErrorString(DWORD dwErrorCode, std::string &errorString)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS;

		DllHandle h;

		if ( dwErrorCode >= NERR_BASE && dwErrorCode < MAX_NERR )
		{
			HMODULE hModule = ::LoadLibraryEx( "netmsg.dll", NULL, LOAD_LIBRARY_AS_DATAFILE );
			if ( hModule )
			{
				h.Attach(hModule);
				dwFlags |= FORMAT_MESSAGE_FROM_HMODULE;
			}
		}

		if ( dwErrorCode >= INTERNET_ERROR_BASE && dwErrorCode <= INTERNET_ERROR_LAST )
		{
			HMODULE hModule = ::LoadLibraryEx( "wininet.dll", NULL, LOAD_LIBRARY_AS_DATAFILE );
			if ( hModule )
			{
				h.Attach(hModule);
				dwFlags |= FORMAT_MESSAGE_FROM_HMODULE;
			}
		}

		LPTSTR pMsgBuf = NULL;

		DWORD dwNumChars = ::FormatMessage( dwFlags,
			h,
			dwErrorCode,
			0,
			(LPTSTR) &pMsgBuf,
			0,
			NULL );

		if(0 == dwNumChars)
		{
			DWORD dwErrorCode = ::GetLastError();
//TODO: KW seems to be a source of infinite looping when some errors are thrown.
			//MMERRTRACE(dwErrorCode, _T("FormatMessage"));
			TESTHR(HRESULT_FROM_WIN32(dwErrorCode));
		}

		errorString = pMsgBuf;
//TODO: KW seems to be a source of infinite looping when some errors are thrown.
		if(NULL == ::LocalFree(pMsgBuf))
		{
			DWORD dwError = ::GetLastError();
			TESTHR(HRESULT_FROM_WIN32(dwError));
// remove the call which calls back to this error handling thus causing a loop
			//MMERRTRACE(dwError, _T("LocalFree"));
		}

		hr = S_OK;
	}
	catch(_com_error)
	{
		::OutputDebugString(_T("Error in Utilities::ErrorHandlingUtils \n"));
	}
		catch(...)
		{
			::OutputDebugString("Error in Utilities::ErrorHandlingUtils::GetErrorString\n");
		}

	return hr;
}