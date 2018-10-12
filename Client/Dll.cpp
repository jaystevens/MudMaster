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
#include "Ifx.h"
#include "Dll.h"
#include "Globals.h"
#include "ObString.h"
#include "Sess.h"
#include "Notifications.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" void DllCallback(LPCSTR pszText);

using namespace std;
using namespace Utilities;

namespace MMSystem
{
	CDll::CDll()
		: _h(0)
	{
	}

	CDll::CDll(const CDll& src) 
		: _name(src._name)
		, _version(src._version)
		, _h(src._h)
	{
	}

	CDll::CDll(const CString &name, const CString &path)
		: _name(name)
		, _path(path)
		, _h(0)
	{
	}

	CDll &CDll::operator=(const CDll &src)
	{
		CDll temp(src);
		Swap(temp);
		return *this;
	}

	bool CDll::operator ==(LPCTSTR szName)
	{
		return _name == szName;
	}

	bool CDll::operator ==(HMODULE h)
	{
		return _h == h;
	}

	bool CDll::operator <(const CDll &dll)
	{
		return _name < dll._name;
	}

	void CDll::Swap(CDll &src)
	{
		swap(_name, src._name);
		swap(_version, src._version);
		swap(_h, src._h);
	}

	CDll::~CDll()
	{
	}

	HRESULT CDll::Free()
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			if(NULL != _h)
			{
				if(!::FreeLibrary(_h))
				{
					DWORD dwError = ::GetLastError();
					MMERRTRACE(dwError, _T("FreeLibrary"));
					ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
				}
				_name    = "";
				_version = "";
				_h       = NULL;
			}
			
			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString(_T("Error trying to remove dll\n"));
		}

		return hr;
	}

	bool CDll::getProcAddress( const CString &func, MYPROC &proc )
	{
		proc = reinterpret_cast<MYPROC>( GetProcAddress( _h, func ) );
		return NULL != proc;
	}

	HRESULT CDll::Load()
	{
		ATLASSERT(!_name.IsEmpty());

		HMODULE hDll = NULL;
		HRESULT hr = E_UNEXPECTED;

		try
		{
			if(_name.IsEmpty())
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
		
			hDll = ::LoadLibrary( _path );
			if (hDll == NULL)
			{
				DWORD dwError = ::GetLastError();
				MMERRTRACE(dwError, _T("LoadLibrary"));
				ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
			}

			// Check for the Version function.  If it doesn't exists consider
			// it not to be a MM DLL.
			MYPROC VersionProc = reinterpret_cast<MYPROC>(::GetProcAddress(hDll, "Version"));
			if (NULL == VersionProc)
			{
				DWORD dwError = ::GetLastError();
				MMERRTRACE(dwError, _T("GetProcAddress"));
				ErrorHandlingUtils::TESTHR(E_FAIL);
			}

			char result[DLL_RESULT_LEN + 1] = {0};
			VersionProc( _name , result );
			_version = result;
			if(_version.Find(_T("singleton")) > -1)
			{
				CString text;
				text = "/showme {@ansibold()@foreyellow()An attempt to load the " + _name + " dll again from another session was blocked}";
				DllCallback(text);
			}
			else
			{
//only set things up if version call succeeds and doesn't return singleton
			_h = hDll;
			DLLCALLBACK SetCallback = (DLLCALLBACK)GetProcAddress(hDll, "CallbackAddress");
			if(NULL != SetCallback)
				SetCallback(DllCallback);

			hr = S_OK;
			}
		}
		catch(_com_error)
		{
			if(NULL != hDll)
			{
				if(!::FreeLibrary(hDll))
				{
					MMERRTRACE(::GetLastError(), _T("FreeLibrary"));
				}
			}

			::OutputDebugString(_T("Error loading dll\n"));
		}

		return hr;
	}
}