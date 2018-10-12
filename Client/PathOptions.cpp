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
#include "PathOptions.h"
#include "Globals.h"
#include "direct.h"
#include "ConfigFileConstants.h"
#include "ErrorHandling.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Utilities;

namespace SerializedOptions
{
	CPathOptions::CPathOptions()
	{
	}

	CPathOptions::CPathOptions(const CPathOptions & src)
		: _downloadPath(src._downloadPath)
		, m_strLogPath(src.m_strLogPath)
		, m_strSoundPath(src.m_strSoundPath)
		, _uploadPath(src._uploadPath)
	{
	}

	CPathOptions &CPathOptions::operator =(const CPathOptions &src)
	{
		CPathOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CPathOptions::Swap(CPathOptions &src)
	{
		swap(_downloadPath, src._downloadPath);
		swap(m_strLogPath, src.m_strLogPath);
		swap(m_strSoundPath, src.m_strSoundPath);
		swap(_uploadPath, src._uploadPath);
	}

	HRESULT CPathOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_STR(PATH_OPTIONS_SECTION, SOUND_PATH_KEY,		m_strSoundPath.c_str());
		PUT_ENTRY_STR(PATH_OPTIONS_SECTION, DOWNLOAD_PATH_KEY,	_downloadPath.c_str());
		PUT_ENTRY_STR(PATH_OPTIONS_SECTION, UPLOAD_PATH_KEY,	_uploadPath.c_str());
		PUT_ENTRY_STR(PATH_OPTIONS_SECTION, LOG_PATH_KEY,		m_strLogPath.c_str());
		return S_OK;
	}

	HRESULT CPathOptions::Read(LPCTSTR lpszPathName)
	{
		m_strSoundPath		= GET_ENTRY_STR(PATH_OPTIONS_SECTION, SOUND_PATH_KEY,		_T(""));
		_downloadPath		= GET_ENTRY_STR(PATH_OPTIONS_SECTION, DOWNLOAD_PATH_KEY,	_T(""));
		_uploadPath			= GET_ENTRY_STR(PATH_OPTIONS_SECTION, UPLOAD_PATH_KEY,		_T(""));
		m_strLogPath		= GET_ENTRY_STR(PATH_OPTIONS_SECTION, LOG_PATH_KEY,			_T(""));

		return S_OK;
	}

	HRESULT CPathOptions::Put_SoundPath(LPCTSTR szSoundPath)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CString strPath(szSoundPath);
			strPath.Trim();
			if(strPath.IsEmpty())
			{
				ErrorHandlingUtils::TESTHR(E_INVALIDARG);
			}

			m_strSoundPath = strPath;
			string::reverse_iterator it = m_strSoundPath.rbegin();
			if(*it == '\\')
			{
				m_strSoundPath = m_strSoundPath.substr(m_strSoundPath.length()-1);
			}	

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString("Error in Put_SoundPath\n");
		}

		return hr;
	}
}