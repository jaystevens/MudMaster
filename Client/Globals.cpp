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
// Globals.cpp: implementation of the CGlobals class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ifx.h"
#include "Globals.h"
#include "IPListDlg.h"
#include "Sess.h"
#include "Resource.h"
#include <vector>
#include <sstream>
#include "DefaultOptions.h"
#include "ErrorHandling.h"
#include "FileUtils.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Utilities;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TCHAR CGlobals::g_chCommand		= _T('/');
TCHAR CGlobals::g_chProcedure	= _T('@');
TCHAR CGlobals::g_chSeparator	= _T(';');
TCHAR CGlobals::g_chEscape		= _T('\\');
TCHAR CGlobals::g_chBlockStart	= _T('{');
TCHAR CGlobals::g_chBlockEnd	= _T('}');

CEdit *CGlobals::g_pDebugView = NULL;

CString CGlobals::g_strGlobalVars[10];
CString CGlobals::g_strHostName;
CString CGlobals::g_strIPAddress;
CString CGlobals::g_strMsgBoxTitle;
CString CGlobals::g_strIconPath;
CString CGlobals::g_strMinimiseToTray;

BOOL CGlobals::Initialize( const std::string &ipAddress )
{
	SetCommandCharacter('/');
	SetSeparatorCharacter(';');
	SetEscapeCharacter('\\');
	SetBlockStartCharacter('{');
	SetBlockEndCharacter('}');
	
	g_strMsgBoxTitle.LoadString(IDS_MSGBOX_TITLE);
// configurable branding
	TCHAR Brand[90] = {0};

	::GetPrivateProfileString(
		_T("Window"), 
		_T("VSize"), 
		CGlobals::g_strMsgBoxTitle,
		Brand,
		90,
		CGlobals::GetConfigFilename() );
	if(Brand != CGlobals::g_strMsgBoxTitle)
	{
		std::string strBrand = CGlobals::XOR(Brand,"B");
		CGlobals::g_strMsgBoxTitle = strBrand.c_str();
		CGlobals::g_strMsgBoxTitle = CGlobals::g_strMsgBoxTitle.Mid(1);
	}
	//now for icon...
	CGlobals::g_strIconPath =  _T("NoIcon");
	::GetPrivateProfileString(
		_T("Window"), 
		_T("HSize"), 
		CGlobals::g_strIconPath,
		Brand,
		40,
		CGlobals::GetConfigFilename() );
	if(Brand != CGlobals::g_strIconPath)
	{
		std::string strBrand = CGlobals::XOR(Brand,"B");
		CGlobals::g_strIconPath = strBrand.c_str();
		CGlobals::g_strIconPath = CGlobals::g_strIconPath.Mid(1);
	}

	//Finally determine whether minimise should hide or not
	//default is not to hide in tray
	//if MinimiseToTray= is in mudmaster2k6.ini in the Window section minimise will hide to tray
	CGlobals::g_strMinimiseToTray = _T("show");
	::GetPrivateProfileString(
		_T("Window"), 
		_T("MinimiseToTray"), 
		CGlobals::g_strMinimiseToTray,
		Brand,
		10,
		CGlobals::GetConfigFilename() );
	if(Brand != CGlobals::g_strMinimiseToTray)
	{
		CGlobals::g_strMinimiseToTray = _T("hide");
	}

	GetMachineIPAddress( ipAddress );

	return TRUE;
}

string CGlobals::XOR(std::string value,std::string key)
{
	using std::string;
    string retval(value);

    short unsigned int klen=key.length();
    short unsigned int vlen=value.length();
    short unsigned int k=0;
    short unsigned int v=0;
    
    for(v;v<vlen;v++)
    {
        retval[v]=value[v]^key[k];
        k=(++k<klen?k:0);
    }
    
    return retval;
}
// If the Debug View is open this prints a line of text in it.
void CGlobals::PrintDebugText(CSession *pSession, LPCSTR pszText)
{
	CString strSession;
	strSession.Format("<%s> %s\n", 
		pSession->GetOptions().sessionInfo_.SessionName(),
		pszText);
	::OutputDebugString(strSession);
}

HRESULT CGlobals::GetWindowsHostName( std::string &hostName )
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		static const byte hostNameLength = 40;
		char szHostName[hostNameLength];
		if( SOCKET_ERROR == ::gethostname( szHostName, hostNameLength - 1 ) )
		{
			DWORD dwError = ::WSAGetLastError();
			TESTHR( HRESULT_FROM_WIN32( dwError ) );
		}

		hostName = szHostName;

		hr = S_OK;
	}
	catch( _com_error &e )
	{
		hr = e.Error();
	}

	return hr;
}

HRESULT CGlobals::GetHostEntry( const std::string &hostName, HOSTENT *&entry )
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		entry = ::gethostbyname( hostName.c_str() );
		if( NULL == entry )
		{
			DWORD dwError = ::WSAGetLastError();
			TESTHR( HRESULT_FROM_WIN32( dwError ) );
		}

		hr = S_OK;
	}
	catch( _com_error &e )
	{
		hr = e.Error();
	}

	return hr;
}

CString CGlobals::GetConfigFilename()
{
	CString dir;

	dir = MMFileUtils::fixPath(INI_FILE_NAME);

	return dir;
}


// Even though an ip address is cached, the user could be using DHCP
// and the address may no longer be valid.  Sweep through the addresses
// and make sure the currently cached address is still a valid address
// if not, let the user select one
HRESULT CGlobals::GetMachineIPAddress( const std::string &currentAddress )
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		long ipaddr;

		string hostName;
		TESTHR( GetWindowsHostName( hostName ) );
		if( !hostName.empty() )
		{
			g_strHostName = hostName.c_str();
			HOSTENT *entry = NULL;
			TESTHR( GetHostEntry( hostName, entry ) );

			// count how many addresses are in the list

			bool currentIsGood = false;

			unsigned long currentAddr = inet_addr( currentAddress.c_str() );

			int x = 0;
			for(; entry->h_addr_list[x] != NULL; x++)
			{
				unsigned long l = *(unsigned long *)(entry->h_addr_list[x]);
				if( currentAddr == l )
					currentIsGood = true;
			}

			if( currentIsGood )
			{
				g_strIPAddress = currentAddress.c_str();
			}
			else
			{
				ipaddr = *(reinterpret_cast<LONG *>(entry->h_addr_list[0]));

				if( x > 1 )
				{
					CIPListDlg dlg;
					dlg.SetList( entry->h_addr_list );
					int nRes = dlg.DoModal();
					if(nRes == IDOK)
					{
						ipaddr = dlg.GetSelectedIp();
					}
				}

				in_addr in;
				in.s_addr = ipaddr;
				g_strIPAddress = CA2T( ::inet_ntoa( in ) );

				CString dir = GetConfigFilename();
				::WritePrivateProfileString(
					DEFAULT_SECTION_NAME,
					DEFAULT_IP_ADDRESS,
					g_strIPAddress,
					dir );
			}
		}
		hr = S_OK;
	}
	catch( _com_error &e )
	{
		hr = e.Error();
	}

	return hr;
}

BOOL CGlobals::CreateDir(LPCSTR pszPath, BOOL bDisplayMessage)
{
	// First check to see if it already exists.
	if (GetFileAttributes(pszPath) == FILE_ATTRIBUTE_DIRECTORY)
		return(TRUE);

	if (!CreateDirectory(pszPath,NULL))
	{
		if (bDisplayMessage)
		{
			CString strMessage;
			strMessage.Format(
				"Unable to create the session directory.\n\n\"%s\"",
				pszPath);
			AfxGetMainWnd()->MessageBox(strMessage,
				CGlobals::GetMsgBoxTitle(),
				MB_ICONINFORMATION);
		}
		return(FALSE);
	}
	return(TRUE);
}

void CGlobals::PrepareForWrite(LPCSTR  pszText, CString & strOut)
{
	LPSTR ptr = strOut.GetBuffer(strlen(pszText)*2);

	TCHAR procChar = CGlobals::GetProcedureCharacter();

	while(*pszText)
	{
		if (	*pszText == procChar	|| 
				*pszText == '$'			|| 
				*pszText == GetEscapeCharacter())
		{
			*ptr = GetEscapeCharacter();
			ptr++;
		}
		*ptr = *pszText;
		ptr++;
		pszText++;
	}
	*ptr = '\x0';
	strOut.ReleaseBuffer(-1);
}

TCHAR CGlobals::GetCommandCharacter()
{
	return g_chCommand;
}

TCHAR CGlobals::GetProcedureCharacter()
{
	return g_chProcedure;
}

TCHAR CGlobals::GetSeparatorCharacter()
{
	return g_chSeparator;
}

TCHAR CGlobals::GetEscapeCharacter()
{
	return g_chEscape;
}

TCHAR CGlobals::GetBlockStartCharacter()
{
	return g_chBlockStart;
}

TCHAR CGlobals::GetBlockEndCharacter()
{
	return g_chBlockEnd;
}

const CString& CGlobals::GetMsgBoxTitle()
{
	return g_strMsgBoxTitle;
}
const CString& CGlobals::GetIconPath()
{
	return g_strIconPath;
}

const CString& CGlobals::GetIPAddress()
{
	return g_strIPAddress;
}

const CString& CGlobals::GetHostName()
{
	return g_strHostName;
}
const CString& CGlobals::GetMinimiseToTray()
{
	return g_strMinimiseToTray;
}

CString *CGlobals::GetGlobalVars()
{
	return g_strGlobalVars;
}

CString& CGlobals::GetGlobalVar(int i)
{
	return g_strGlobalVars[i];
}

CEdit *CGlobals::GetDebugView()
{
	return g_pDebugView;
}

void CGlobals::SetCommandCharacter(char ch)
{ 
	g_chCommand = ch; 
}

void CGlobals::SetSeparatorCharacter(char ch)
{ 
	g_chSeparator = ch;
}

void CGlobals::SetEscapeCharacter(char ch)
{ 
	g_chEscape = ch;
}

void CGlobals::SetBlockStartCharacter(char ch)
{ 
	g_chBlockStart = ch; 
}

void CGlobals::SetBlockEndCharacter(char ch)
{ 
	g_chBlockEnd = ch;
}

void CGlobals::SetIPAddress(const CString& strIPAddress)
{ 
	g_strIPAddress = strIPAddress;
}

void CGlobals::SetHostName(const CString& strHostName)
{ 
	g_strHostName = strHostName;
}
void CGlobals::SetMinimiseToTray(const CString& strMinimiseToTray)
{ 
	g_strMinimiseToTray = strMinimiseToTray;
}

void CGlobals::SetDebugView(CEdit *pDebugView)
{ 
	g_pDebugView = pDebugView;
}

void CGlobals::SetGlobalVar(int index, const char *strValue)
{ 
	g_strGlobalVars[index] = strValue;
}

void CGlobals::ClearGlobalVar()
{ 
	int x = 0;
	for(; x < 11; x++)
	g_strGlobalVars[x] = "";
}

BOOL CGlobals::BackupFile(LPCTSTR lpszSourceFilename)
{
	bool retval = false; // assume failure
	TCHAR lpszDestFilename[MAX_PATH + 1] = {0};
	int ret = _sntprintf(
		lpszDestFilename, 
		MAX_PATH, 
		_T("%s.%s"), 
		lpszSourceFilename, 
		_T(".bak"));
	if(ret > 0)
	{
		if(CopyFile(lpszSourceFilename, lpszDestFilename, FALSE))
		{
			retval = true;
		}
	}
	return retval;
}

HRESULT CGlobals::GetVersionData(std::vector<char> &buffer)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		TCHAR lpszExeName[MAX_PATH + 1] = {0};

		DWORD dwNumChars = GetModuleFileName(
			AfxGetInstanceHandle(), 
			lpszExeName, 
			MAX_PATH);

		if(0 == dwNumChars)
			ErrorHandlingUtils::err(_T("GetModuleFileName"));

		DWORD dwHandle = NULL;
		DWORD dwVersionSize = GetFileVersionInfoSize(lpszExeName, &dwHandle);
		if(0 == dwVersionSize)
			ErrorHandlingUtils::err(_T("GetFileVersionInfoSize"));

		buffer.resize(dwVersionSize);

		if (!GetFileVersionInfo(
			lpszExeName, 
			dwHandle, 
			dwVersionSize, 
			reinterpret_cast<LPVOID>(&buffer[0])))
			ErrorHandlingUtils::err(_T("GetFileVersionInfo"));

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}

HRESULT CGlobals::GetBuildNumber(std::string &buildNumber)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		buildNumber = "????";

		std::vector<char> buffer;

		ErrorHandlingUtils::TESTHR(GetVersionData(buffer));

		VS_FIXEDFILEINFO *pFileInfo = NULL;
		UINT nResultLen = 0;

		if (!VerQueryValue(
			reinterpret_cast<LPVOID>(&buffer[0]), 
			_T("\\"), 
			reinterpret_cast<LPVOID*>(&pFileInfo), 
			&nResultLen))
		{
			ErrorHandlingUtils::err(_T("VerQueryValue"));
		}

		std::stringstream buf;
		buf << LOWORD(pFileInfo->dwFileVersionLS);

		buildNumber = buf.str();

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return SUCCEEDED(hr) ? true : false;
}

bool CGlobals::GetFileVersion(std::string &version)
{
	HRESULT hr = E_UNEXPECTED;
	
	try
	{
		version = "Unknown Version";

		std::vector<char> buffer;

		ErrorHandlingUtils::TESTHR(GetVersionData(buffer));

		VS_FIXEDFILEINFO *pFileInfo = NULL;
		UINT nResultLen = 0;

		if (!VerQueryValue(
			reinterpret_cast<LPVOID>(&buffer[0]), 
			_T("\\"), 
			reinterpret_cast<LPVOID*>(&pFileInfo), 
			&nResultLen))
		{
			ErrorHandlingUtils::err(_T("VerQueryValue"));
		}

		std::stringstream buf;
		buf 
			<< " - " 
			<< HIWORD(pFileInfo->dwFileVersionMS)
			<< "." 
			<< LOWORD(pFileInfo->dwFileVersionMS);

		version = buf.str();

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return SUCCEEDED(hr) ? true : false;
}

BOOL CGlobals::CenterWindow(HWND hwndCentered, HWND hwndPreferredOwner)
{
	BOOL Result;
	POINT OwnerCenter, CenteredUL;
	RECT WindowRect, OwnerRect, WorkArea;
	SIZE CenteredWindow;

	_ASSERT(NULL != hwndCentered);
	_ASSERT(NULL != hwndPreferredOwner);
	_ASSERT(IsWindow(hwndCentered));

	Result = SystemParametersInfo(
		SPI_GETWORKAREA, 
		sizeof(RECT), 
		&WorkArea,
		0);

	if(!Result)
	{
		SetRect(
			&WorkArea, 
			0, 
			0, 
			GetSystemMetrics(SM_CXSCREEN), 
			GetSystemMetrics(SM_CYSCREEN));
	}

	if(NULL!=hwndPreferredOwner)
	{
		_ASSERT(IsWindow(hwndPreferredOwner));
		GetWindowRect(hwndPreferredOwner, &OwnerRect);
	}
	else
	{
		OwnerRect = WorkArea;
	}

	OwnerCenter.x = (OwnerRect.left + OwnerRect.right) / 2;
	OwnerCenter.y = (OwnerRect.top + OwnerRect.bottom) / 2;

	GetWindowRect(hwndCentered, &WindowRect);
	CenteredWindow.cx = WindowRect.right - WindowRect.left;
	CenteredWindow.cy = WindowRect.bottom - WindowRect.top;

	CenteredUL.x = OwnerCenter.x - CenteredWindow.cx / 2;
	CenteredUL.y = OwnerCenter.y - CenteredWindow.cy / 2;

	if(CenteredUL.x + CenteredWindow.cx > WorkArea.right)
		CenteredUL.x = WorkArea.right-CenteredWindow.cx;

	if(CenteredUL.y < WorkArea.top)
		CenteredUL.y = WorkArea.top;
	else if (CenteredUL.y + CenteredWindow.cy > WorkArea.bottom)
		CenteredUL.y = WorkArea.bottom - CenteredWindow.cy;

	return SetWindowPos(
		hwndCentered, 
		NULL, 
		CenteredUL.x, 
		CenteredUL.y, 
		0, 
		0, 
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
}

BOOL CGlobals::ProtectFile(
	LPCTSTR lpszFilename, 
	BOOL bPromptOverwrite)
{
	bool retval = true; // assume success

	if (bPromptOverwrite)
	{
		DWORD dwFileAttrs = GetFileAttributes(lpszFilename);
		if(INVALID_FILE_ATTRIBUTES == dwFileAttrs)
		{
		}
		else
		{
			CString strMessage;
			strMessage.Format(
				_T("The file [%s] already exists. Overwrite it?"),
				lpszFilename);

			int nSave = AfxMessageBox(
				strMessage, 
				MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);

			if(nSave != IDOK)
			{
				retval = false;
			}
			else
			{
				BOOL bBackupSuccess = BackupFile(lpszFilename);

				if(!bBackupSuccess)
				{
					int nSave = AfxMessageBox(
						_T("Backup failed, overwrite original anyway?"), 
						MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);

					if(nSave != IDOK)
						retval = false;
				}			
			}
		}
	}
	return retval;
}


HRESULT CGlobals::GetModuleDirectory(std::string &directory)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		TCHAR pszFilename[MAX_PATH] = {0};
		DWORD dwNumChars = GetModuleFileName(NULL, pszFilename, MAX_PATH);
		if(0 == dwNumChars)
		{
			DWORD dwError = ::GetLastError();
			MMERRTRACE(dwError, _T("Failed to get the module filename"));
			ErrorHandlingUtils::TESTHR(HRESULT_FROM_WIN32(dwError));
		}

		directory = pszFilename;
		string::size_type pos = directory.find_last_of('\\');
		directory = directory.substr(0, pos + 1);
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		CString strDebug;
		strDebug.Format(" got %s", directory);
		::OutputDebugString(strDebug);

		hr = e.Error();
	}

	return hr;
}

HRESULT CGlobals::FixPath(CPath &path, const CString &strGivenPath)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CPath pathFirst(strGivenPath);
		if(pathFirst.IsRelative())
		{
			string strDirectory;
			ErrorHandlingUtils::TESTHR(CGlobals::GetModuleDirectory(strDirectory));
			path.Combine(strDirectory.c_str(), strGivenPath);
		}
		else
		{
			path = pathFirst;
		}
		
		CString strDebug;
		strDebug.Format("given %s got %s\n", strGivenPath,path);
		::OutputDebugString(strDebug);

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		CString strDebug;
		strDebug.Format("given %s got %s\n", strGivenPath,path);
		::OutputDebugString(strDebug);
		hr = e.Error();
	}

	return hr;
}

HRESULT CGlobals::InterpretException(CException &e, CString &strMessage)
{
	TCHAR szErrorMessage[255] = {0};
	if(!e.GetErrorMessage(szErrorMessage, 255))
	{
		strMessage = _T("Unknown Error");
	}
	else
	{
		strMessage = szErrorMessage;
	}
	return S_OK;
}

HRESULT CGlobals::OpenFileError(CSession *pSession, CFileException &fe)
{
	CString errorMessage;
	CGlobals::InterpretException(fe, errorMessage);

	CString strMessage;
	strMessage.Format(
		_T("Error opening file [%d]: %s"), 
		fe.m_cause,
		errorMessage);

	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);

	return S_OK;
}

CString CGlobals::GetIniString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault, LPCTSTR szFilename)
{
	TCHAR buffer[4096] = {0};
	::GetPrivateProfileString(
		szSection,
		szKey,
		szDefault,
		buffer,
		4096,
		szFilename);
	return buffer;
}

void CGlobals::WriteIniInt(LPCTSTR szSection, LPCTSTR szKey, DWORD val, LPCTSTR szFilename)
{
	CString strVal;
	strVal.Format(_T("%d"), val);
	::WritePrivateProfileString(szSection, szKey, strVal, szFilename);
}

HRESULT CGlobals::BrowseForFolder(UINT nDialogTitle, std::string &path)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString strDialogTitle;
		if(!strDialogTitle.LoadString(nDialogTitle))
		{
			TRACE(_T("Failed to load dialog title"));
			strDialogTitle = _T("Select a folder");
		}

		BROWSEINFO bi;
		LPITEMIDLIST ilist;

		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = NULL;
		bi.lpszTitle = strDialogTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_VALIDATE;
		bi.lpfn = NULL;
		bi.lParam = 0;

		std::vector<char> buffer;
		buffer.resize(MAX_PATH);

		ilist = SHBrowseForFolder(&bi);
		if(ilist != NULL)
		{
			if(!SHGetPathFromIDList(ilist, &buffer[0]))
			{
				TRACE(_T("SHGetPathFromIDList failed"));
				ASSERT(FALSE);
			}

			LPMALLOC pMalloc;
			if(FAILED(SHGetMalloc(&pMalloc)))
			{
				TRACE(_T("SHGetPathFromIDList failed"));
				ASSERT(FALSE);
			}

			pMalloc->Free(ilist);
			path.assign(buffer.begin(), buffer.end());
		}

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}