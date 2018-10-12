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
#include "FileUtils.h"
#include "Globals.h"
#include "ErrorHandling.h"
#include <shlobj.h>
#include "ConfigFileConstants.h"

//TODO: KW add sstream
#include <sstream>
using namespace Utilities;

namespace MMFileUtils
{
	CString fixPath(const CString &path)
	{

		CPath pathHelp(path);
		if(pathHelp.IsRelative())
		{
			CString dir;
			getPersonalFolder(dir);

			if(dir.Right(1) != "\\")
			{
				dir += "\\";
			}
			CPath newPath(dir);
			newPath.Append(pathHelp.m_strPath);
			pathHelp = newPath;
		}
		return pathHelp.m_strPath;
	}

	bool getPersonalFolder(CString &folder)
	{
		bool result = false;
		int ret =0;
		//TODO: KW Check for ini file is application directory
		std::string path;
		ErrorHandlingUtils::TESTHR(CGlobals::GetModuleDirectory(path));
		std::stringstream buffer;

		buffer 
			<< path 
			<< INI_FILE_NAME ;
			
		CString dir(buffer.str().c_str());
		int iniWithExe = ::GetPrivateProfileInt(
		"Defaults",
		"Ini With Exe",
		FALSE,
		dir );
		int iniInMyDocs = ::GetPrivateProfileInt(
		"Defaults",
		"Ini In My Documents",
		FALSE,
		dir );

	if (iniWithExe == 1 || iniInMyDocs == 0)
	{
		folder  = path.c_str();
		result = true;
		return result;
		}
	else
		{

		TCHAR pszPath[MAX_PATH] = {0};

		HMODULE hMod = ::LoadLibrary("SHFolder.DLL");
		if(NULL == hMod)
		{
			DWORD dwError = ::GetLastError();
			MMERRTRACE(dwError, _T("Failed to load SHFolder.DLL"));
		} else {
			HRESULT get_path = reinterpret_cast<HRESULT>(::GetProcAddress(hMod, "SHGetFolderPathA"));

			if(NULL == get_path)
			{
				DWORD dwError = ::GetLastError();
				MMERRTRACE(dwError, _T("Failed to find the SHGetFolderPathA process address"));
			}
			else
			{
				/* FIXME: Need to bring this upto standards
				if(SUCCEEDED(get_path(NULL, CSIDL_PERSONAL, NULL, 0, pszPath)))
				{
					//TODO: KW add directory name if using my documents directory
	 ret = _sntprintf(
		pszPath, 
		MAX_PATH, 
		_T("%s\\%s"),
		pszPath,
		_T(DIR_NAME));

					folder = pszPath;
					result = true;
				}
				*/
			}
		}

		return result;
	}
	}
}