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
// Globals.h: interface for the CGlobals class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALS_H__0FA263A1_7825_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_GLOBALS_H__0FA263A1_7825_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDebugView;
class CSession;
class CCharFormat;

class CGlobals  
{
	static TCHAR g_chCommand;
	static TCHAR g_chProcedure;
	static TCHAR g_chSeparator;
	static TCHAR g_chEscape;
	static TCHAR g_chBlockStart;
	static TCHAR g_chBlockEnd;

	static CEdit *g_pDebugView;

	static CString g_strGlobalVars[10];
	static CString g_strHostName;
	static CString g_strIPAddress;
	static CString g_strMsgBoxTitle;
	static CString g_strIconPath;
	static CString g_strMinimiseToTray;

public:
	static const TCHAR DOUBLE_QUOTE = _T('"');
	static const TCHAR DOLLAR_SIGN = _T('$');
	static const TCHAR PROC_OPEN_PAREN = _T('(');
	static const TCHAR PROC_CLOSE_PAREN = _T(')');

	static void PrepareForWrite(LPCSTR pszText, CString& strOut);
	// These are used by the list controls.
	typedef enum SortDirections
	{ 
		LISTSORT_ASCEND		= 0x0100, 
		LISTSORT_DESCEND	= 0x0200 
	} SortDirections;

	static BOOL Initialize( const std::string &ipAddress );
	static std::string CGlobals::XOR(std::string value,std::string key);
	//ACCESSORS
	static TCHAR GetCommandCharacter();
	static TCHAR GetProcedureCharacter();
	static TCHAR GetSeparatorCharacter();
	static TCHAR GetEscapeCharacter();
	static TCHAR GetBlockStartCharacter();
	static TCHAR GetBlockEndCharacter();
	static const CString& GetMsgBoxTitle();
	static const CString& GetIconPath();
	static const CString& GetIPAddress();
	static const CString& GetHostName();
	static const CString& GetMinimiseToTray();
	static CString * GetGlobalVars();
	static CString& GetGlobalVar(int i);
	static CEdit *GetDebugView();

	//MUTATORS
	static void SetCommandCharacter(char ch);
	static void SetSeparatorCharacter(char ch);
	static void SetEscapeCharacter(char ch);
	static void SetBlockStartCharacter(char ch);
	static void SetBlockEndCharacter(char ch);
	static void SetIPAddress(const CString& strIPAddress);
	static void SetHostName(const CString& strHostName);
	static void SetMinimiseToTray(const CString& strHostName);
	static void SetDebugView(CEdit *pDebugView);
	static void SetGlobalVar(int index, const char *strValue);
	static void ClearGlobalVar();

	//GLOBAL FUNCTIONS
	static void PrintDebugText(CSession *pSession, LPCSTR pszText);
	static HRESULT GetMachineIPAddress( const std::string &currentAddress );
	static BOOL CreateDir(LPCSTR pszPath, BOOL bDisplayMessage=FALSE);
	static BOOL BackupFile(LPCTSTR lpszSourceFilename);
	static HRESULT GetVersionData(std::vector<char> &buffer);
	static bool GetFileVersion(std::string &version);
	static HRESULT GetBuildNumber(std::string &buildNumber);
	static BOOL CenterWindow(HWND hwndCentered, HWND hwndPreferredOwner);
	static BOOL ProtectFile(LPCTSTR lpszFilename, BOOL bPromptOverwrite);
	
	static HRESULT GetModuleDirectory(std::string &directory);
	static HRESULT FixPath(CPath &path, const CString &strGivenPath);
	static HRESULT InterpretException(CException &e, CString &strMessage);
	static HRESULT OpenFileError(CSession *pSession, CFileException &e);
	static HRESULT BrowseForFolder(UINT nDialogTitle, std::string &path);

	static CString GetIniString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault, LPCTSTR szFilename);
	static void WriteIniInt(LPCTSTR szSection, LPCTSTR szKey, DWORD val, LPCTSTR szFilename);
	static CString GetConfigFilename();

private:
	static HRESULT GetWindowsHostName( std::string &hostName );
	static HRESULT GetHostEntry( const std::string &hostName, HOSTENT *&entry );
	CGlobals(){}
	virtual ~CGlobals(){}
};

#endif // !defined(AFX_GLOBALS_H__0FA263A1_7825_11D3_BCD6_00E029482496__INCLUDED_)
