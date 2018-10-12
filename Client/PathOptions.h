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
#pragma once

namespace SerializedOptions
{
	class CPathOptions
	{
#define PATH_OPTIONS_SECTION	"Paths"
#define SOUND_PATH_KEY			"Sound Files Path"
#define DOWNLOAD_PATH_KEY		"Download Files Path"
#define UPLOAD_PATH_KEY			"Upload Files Path"
#define LOG_PATH_KEY			"Log Files Path"

	public:
		CPathOptions();
		CPathOptions(const CPathOptions& src);
		CPathOptions &operator=(const CPathOptions &src);

		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT Read(LPCTSTR lpszPathName);

		//ACCESSORS
		HRESULT Get_DownloadPath(std::string &downloadPath) const
		{
			downloadPath = _downloadPath;
			return S_OK;
		}

		LPCTSTR LogPath()					{return m_strLogPath.c_str();}

		HRESULT Get_SoundPath(std::string &strSoundPath) const
		{
			strSoundPath = m_strSoundPath;
			return S_OK;
		}

		HRESULT Get_UploadPath(std::string &uploadPath) const
		{
			uploadPath = _uploadPath;
			return S_OK;
		}

		//MUTATORS
		HRESULT Put_SoundPath(LPCTSTR strSoundPath);
		void LogPath(LPCTSTR strLogPath) {m_strLogPath = strLogPath;}
		void DownloadPath(LPCTSTR strDownloadPath) {_downloadPath = strDownloadPath;}
		void UploadPath(LPCTSTR strUploadPath) {_uploadPath = strUploadPath;}

	private:
		void Swap(CPathOptions &src);
		std::string m_strSoundPath;
		std::string _downloadPath;
		std::string _uploadPath;
		std::string m_strLogPath;
	};
}