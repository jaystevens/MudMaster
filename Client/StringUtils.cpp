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
#include ".\stringutils.h"
#include "ErrorHandling.h"
#include <algorithm>
#include <cctype>

using namespace Utilities;
using namespace std;

void StringUtils::LowerString(const string &in, string &out)
{
	out = in;
	std::transform(out.begin(), out.end(), out.begin(), std::tolower);
}

HRESULT StringUtils::LoadString(UINT stringId, string &string)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		static const DWORD STRING_BUFFER_SIZE = 1024;
		char buffer[STRING_BUFFER_SIZE];

		HMODULE hMod = ::GetModuleHandle(NULL);
		if(NULL == hMod)
			ErrorHandlingUtils::err(_T("GetModuleHandle(NULL)"));

		int nChars = ::LoadString(hMod, stringId, buffer, STRING_BUFFER_SIZE);
		if(nChars <= 0)
			ErrorHandlingUtils::err(_T("LoadString()"));

		string = buffer;

		hr = S_OK;
	}
	catch(_com_error)
	{
		::OutputDebugString("Error in StringUtils LoadString\n");
	}

	return hr;
}