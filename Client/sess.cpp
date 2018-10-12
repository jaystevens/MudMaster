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
// Sess.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "SessionHost.h"
#include "DefaultOptions.h"
#include "SessionHost.h"
#include "DllList.h"
#include "SubList.h"
#include "ActionList.h"
#include "AliasList.h"
#include "GagList.h"
#include "HighList.h"
#include "MacroList.h"
#include "VarList.h"
#include "TabList.h"
#include "EventList.h"
#include "GroupList.h"
#include "StatusBar.h"
#include "UserLists.h"
#include "ChatServer.h"
#include "Undo.h"
#include "ArrayList.h"
#include "DebugStack.h"

#include "MSPOptions.h"
#include "Ifx.h"

#include "Sess.h"
#include "Globals.h"
#include "SeException.h"
#include "ErrorHandling.h"
#include "StringUtils.h"
#include "FileUtils.h"
#include "ParseUtils.h"
#include "Alias.h"
#include "Event.h"
#include <sstream>
#include <iomanip>
#include <cctype>


using namespace MMScript;
using namespace MMSystem;
using namespace SerializedOptions;
using namespace Utilities;
using namespace MMChatServer;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSessionImpl
class CSessionImpl
{
public:
	BOOL m_bReadMode;
	BOOL m_bOnTop;
	int m_nLoopCount;
	int m_nCmdDepth;
	//TODO: KW see if I can add a Cmds Processed variable
	//TODO: KW and a LinesReceived variable
	//TODO: KW and a BytesReceived variable
	//         and a TimeSinceLastInput variable
	//         and a milliseconds between last input and first received byte after input
	long m_nCmdsProcessed;
	long m_nLinesReceived;
	long m_nBytesReceived;
	CString m_nReturnTripMsecs;
	BOOL m_bReturnTripSet;
	LARGE_INTEGER m_nInputRawTicks;
	time_t m_tTimeSinceLastInput;
	CString m_ANSICopy;
	CString m_strListItem;
	CString m_strCurrentLogFile;

	CTabList		m_lstTabs;
	CVarList		m_lstVars;
	CMacroList		m_lstMacros;
	CArrayList		m_lstArrays;
	CUndo			m_lstUndo;
	CChatServer		m_lstChat;
	CUserLists		m_lstLists;
	CMMStatusBar	m_lstStatusBar;
	CEventList		m_lstEvents;
	CGagList		m_lstGags;
	CActionList		m_lstActions;
	CAliasList		m_lstAliases;
	CHighList		m_lstHighs;
	CSubList		m_lstSubs;
	CDllList		m_lstDlls;
	CDebugStack		m_lstDebug;
	CStringList		m_lstAuto;
	CGroupList		m_lstGroups;

	COptions		m_options;
	CMapStringToOb	mapNamedItems;
	BOOL			m_bConnected;
	BOOL			m_bHandledByDll;
	CStringList		m_CommandHistoryBuffer;
	LPCSTR			m_telnetAuthkeyResponse;
	CStringList		m_ShowmeViaWebBuffer;
	bool			m_bTelnetNAWS;
	LPCSTR			m_TelnetNAWSmessage;
};

/////////////////////////////////////////////////////////////////////////////
// CSession

//IMPLEMENT_SERIAL(CSession, CCmdTarget, 1)


CSession::CSession()
{
	m_pSessionHost = NULL;
	m_pThis = new CSessionImpl;

	m_pThis->m_lstChat.SetClient(this);
	m_pThis->m_bReadMode = FALSE;
	m_pThis->m_bOnTop = FALSE;
	m_pThis->m_nLoopCount = 0;
	m_pThis->m_nLinesReceived = 0;
	m_pThis->m_nBytesReceived = 0;
	m_pThis->m_nCmdsProcessed = 0;
	m_pThis->m_nReturnTripMsecs = "";
	m_pThis->m_strCurrentLogFile = "";
	m_pThis->m_bReturnTripSet = TRUE;
	m_pThis->m_ANSICopy = " ";
	m_pThis->m_tTimeSinceLastInput = time(NULL);
	m_pThis->m_bConnected = FALSE;
	m_pThis->m_lstAliases.SetGlobalVars(CGlobals::GetGlobalVars());
	m_pThis->m_lstActions.SetGlobalVars(CGlobals::GetGlobalVars());
	m_pThis->m_nCmdDepth = 0;
	m_pThis->m_bTelnetNAWS = false;
	//m_pThis->m_MDB.Connect("C:\\mm2k6\\MudMaster\\builds\\debug\\db1.mdb","admin");
}

CSession::CSession(CSessionHost *pHost)
{
	m_pThis->m_bReadMode = FALSE;
	m_pThis->m_bOnTop = FALSE;
	m_pThis->m_nLoopCount = 0;
	m_pThis->m_nLinesReceived = 0;
	m_pThis->m_nBytesReceived = 0;
	m_pThis->m_nCmdsProcessed = 0;
	m_pThis->m_nReturnTripMsecs = "";
	m_pThis->m_strCurrentLogFile = "";
	m_pThis->m_bReturnTripSet = TRUE;
	m_pThis->m_tTimeSinceLastInput = time(NULL);
	m_pThis->m_bConnected = FALSE;
	m_pThis->m_lstAliases.SetGlobalVars(CGlobals::GetGlobalVars());
	m_pThis->m_lstActions.SetGlobalVars(CGlobals::GetGlobalVars());
	m_pSessionHost = pHost;
	m_pThis->m_nCmdDepth = 0;
	m_pThis->m_bTelnetNAWS = false;

}

CSession::~CSession()
{
	// Dlls may have external apps running that might still
	// need to communicate with mudMaster before shutting down.
	// Thats why we want to remove external apps before we
	GetDlls().RemoveAll(this);
	StopChatServer();
	delete m_pThis;
}

/////////////////////////////////////////////////////////////////////////////
// CSession message handlers

void CSession::SetHost(CSessionHost *pHost)
{
	m_pSessionHost = pHost;
}

CSessionHost *CSession::GetHost()
{
	return m_pSessionHost;
}

void CSession::KillAll() 
{
	GetDlls().RemoveAll(this);
	GetActions()->RemoveAll();
	GetAliases()->RemoveAll();
	GetEvents()->RemoveAll();
	GetLists()->RemoveAll();
	GetMacros()->RemoveAll();
	GetTabList()->RemoveAll();
	GetVariables()->RemoveAll();
	GetArrays()->RemoveAll();
	GetGags()->RemoveAll();
	GetHighlights()->RemoveAll();
	GetSubs()->RemoveAll();
	GetStatusBar()->RemoveAll();	
	m_pThis->m_lstAuto.RemoveAll();
	RedrawStatusBar();
}

bool CSession::SystemVar( const std::string &varName, std::string &result )
{
	bool retval = false;

	if( varName == "ListItem" )
	{
		result = m_pThis->m_strListItem;
		retval = true;
	}
	else if( varName == "LoopCount" )
	{
		std::stringstream buffer;
		buffer << m_pThis->m_nLoopCount;
		result = buffer.str();
		retval = true;
	}
	else if( varName == "BytesReceived" )
	{
		std::stringstream buffer;
		buffer << m_pThis->m_nBytesReceived;
		result = buffer.str();
		retval = true;
	}
	else if( varName == "LinesReceived" )
	{
		std::stringstream buffer;
		buffer << m_pThis->m_nLinesReceived;
		result = buffer.str();
		retval = true;
	}
	else if( varName == "CmdsProcessed" )
	{
		std::stringstream buffer;
		buffer << m_pThis->m_nCmdsProcessed;
		result = buffer.str();
		retval = true;
	}
	else if( varName == "TimeSinceLastInput" )
	{
		CString sTime;
		time_t tNow = time(NULL);
		sTime.Format("%ld",tNow - m_pThis->m_tTimeSinceLastInput);
		result = sTime;
		retval = true;
	}
	else if( varName == "ANSICopy" )
	{
		std::stringstream buffer;
		buffer << m_pThis->m_ANSICopy;
		result = buffer.str();
		retval = true;
	}
	else if( varName == "ReturnTripMsecs" )
	{
		result = m_pThis->m_nReturnTripMsecs;
		retval = true;
	}
	else if (varName == "CurrentLogFile")
	{
		result = m_pThis->m_strCurrentLogFile;
		retval = true;
	}

	return retval;
}

BOOL CSession::SystemVar(const CString &strVarName, CString &strResult)
{
	if (strVarName == "ListItem")
	{
		strResult = m_pThis->m_strListItem;
		return(TRUE);
	}

	if (strVarName == "LoopCount")
	{
		strResult.Format("%d",m_pThis->m_nLoopCount);
		return(TRUE);
	}
	if (strVarName == "LinesReceived")
	{
		strResult.Format("%d",m_pThis->m_nLinesReceived);
		return(TRUE);
	}
	if (strVarName == "BytesReceived")
	{
		strResult.Format("%d",m_pThis->m_nBytesReceived);
		return(TRUE);
	}
	if (strVarName == "CmdsProcessed")
	{
		strResult.Format("%d",m_pThis->m_nCmdsProcessed);
		return(TRUE);
	}
	else if( strVarName == "TimeSinceLastInput" )
	{
		time_t tNow = time(NULL);
		strResult.Format("%ld",tNow - m_pThis->m_tTimeSinceLastInput);
		
		return(TRUE);
	}
	else if( strVarName == "ANSICopy" )
	{
		strResult = m_pThis->m_ANSICopy;
		return(TRUE);
	}
	else if (strVarName == "ReturnTripMsecs")
	{
		strResult = m_pThis->m_nReturnTripMsecs;
		return(TRUE);
	}
	else if (strVarName == "CurrentLogFile")
	{
		strResult = m_pThis->m_strCurrentLogFile;
		return(TRUE);
	}

	
	return(FALSE);
}

BOOL CSession::Read(LPCTSTR lpszPathName)
{
	m_pThis->m_options.Load(lpszPathName);
	m_pSessionHost->LoadScript();
	return TRUE;
}

BOOL CSession::Save(LPCTSTR lpszPathName)
{
	CString strMessage;
	strMessage.Format(_T("Saving character session file to %s"), lpszPathName);
	QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	m_pThis->m_options.Save(lpszPathName);

	CString scriptFilename = m_pThis->m_options.sessionInfo_.ScriptFilename();
	//TODO: KW allow for saving just script name or with path relative to the working directory
	//         in the startup script option
	//         fixPath checks for relative paths and converts to full path
	CString path;

	if(	m_pThis->m_options.sessionInfo_.AutoLoadScript() && !scriptFilename.IsEmpty())
	{		
		scriptFilename = MMFileUtils::fixPath(scriptFilename);
		std::stringstream buffer;
		buffer
			<< CGlobals::GetCommandCharacter()
			<< "write {"
			<< static_cast<LPCTSTR>( scriptFilename )
			<< "}";

		ExecuteCommand( buffer.str().c_str() );
	}

	return TRUE;
}

BOOL CSession::SaveSessionPosition()
{
	m_pThis->m_options.SaveSessionPosition();

	return TRUE;
}

void CSession::LoadScript()
{
	//TODO: KW allow for saving just script name or with path relative to the working directory
	//         in the startup script option
	//         fixPath looks for relative paths to convert to full paths
	CString scriptFilename = m_pThis->m_options.sessionInfo_.ScriptFilename();
	if(m_pThis->m_options.sessionInfo_.AutoLoadScript() && !scriptFilename.IsEmpty())
	{
		scriptFilename = MMFileUtils::fixPath(scriptFilename);

		std::stringstream buffer;
		buffer
			<< CGlobals::GetCommandCharacter()
			<< "read {"
			<< static_cast<LPCTSTR>(scriptFilename)
			<< "}";

		ExecuteCommand( buffer.str().c_str() );
	}

	CString logFilename = m_pThis->m_options.sessionInfo_.LogFileName();

	if(m_pThis->m_options.sessionInfo_.AutoLog() && !logFilename.IsEmpty())
	{
		MMFileUtils::fixPath(logFilename);
		m_pSessionHost->StartLog();
	}
}

CStringList &CSession::GetCommandHistoryBuffer()		
{
	return m_pThis->m_CommandHistoryBuffer;
}
CStringList &CSession::GetShowmeViaWebBuffer()		
{
	return m_pThis->m_ShowmeViaWebBuffer;
}

// Script Entities
CDllList & CSession::GetDlls()				
{
	return m_pThis->m_lstDlls;
}

CSubList *CSession::GetSubs()					
{
	return &m_pThis->m_lstSubs;
}

CHighList *CSession::GetHighlights()			
{
	return &m_pThis->m_lstHighs;
}

CAliasList *CSession::GetAliases()			
{
	return &m_pThis->m_lstAliases;
}

CActionList *CSession::GetActions()			
{
	return &m_pThis->m_lstActions;
}

CGagList *CSession::GetGags()					
{
	return &m_pThis->m_lstGags;
}

CEventList *CSession::GetEvents()				
{
	return &m_pThis->m_lstEvents;
}

CMMStatusBar *CSession::GetStatusBar()		
{
	return &m_pThis->m_lstStatusBar;
}

CUserLists *CSession::GetLists()				
{
	return &m_pThis->m_lstLists;
}

CChatServer & CSession::GetChat()				
{
	return m_pThis->m_lstChat;
}

CUndo & CSession::Undo()						
{
	return m_pThis->m_lstUndo;
}

CArrayList *CSession::GetArrays()				
{
	return &m_pThis->m_lstArrays;
}

CMacroList *CSession::GetMacros()				
{
	return &m_pThis->m_lstMacros;
}

CVarList *CSession::GetVariables()			
{
	return &m_pThis->m_lstVars;
}

CTabList *CSession::GetTabList()				
{
	return &m_pThis->m_lstTabs;
}

CStringList& CSession::GetAutoList()			
{
	return m_pThis->m_lstAuto;
}

CGroupList &CSession::GetGroups()				
{
	return m_pThis->m_lstGroups;
}

CDebugStack & CSession::DebugStack()			
{
	return m_pThis->m_lstDebug;
}

void CSession::ReadMode(BOOL bReadMode)		
{
	m_pThis->m_bReadMode = bReadMode;
}
BOOL CSession::ReadMode()						
{
	return m_pThis->m_bReadMode;
}
void CSession::OnTop(BOOL bOnTop)		
{
	m_pThis->m_bOnTop = bOnTop;
}

BOOL CSession::OnTop()						
{
	return m_pThis->m_bOnTop;
}

int CSession::GetCmdDepth()					
{
	return m_pThis->m_nCmdDepth;
}

void CSession::SetCmdDepth(int nNewValue)	
{
	m_pThis->m_nCmdDepth = nNewValue;
}
long CSession::GetLoopCount()					
{
	return m_pThis->m_nLoopCount;
}

void CSession::SetLoopCount(long nNewValue)	
{
	m_pThis->m_nLoopCount = nNewValue;
}
long CSession::GetCmdsProcessed()					
{
	return m_pThis->m_nCmdsProcessed;
}

void CSession::SetCmdsProcessed(long nNewValue)	
{
	m_pThis->m_nCmdsProcessed = nNewValue;
}

long CSession::GetLinesReceived()					
{
	return m_pThis->m_nLinesReceived;
}

void CSession::SetLinesReceived(long nNewValue)	
{
	m_pThis->m_nLinesReceived = nNewValue;
}
long CSession::GetBytesReceived()					
{
	return m_pThis->m_nBytesReceived;
}

void CSession::SetBytesReceived(long nNewValue)	
{
	m_pThis->m_nBytesReceived = nNewValue;
}
CString CSession::GetReturnTripMsecs()					
{
	return m_pThis->m_nReturnTripMsecs;
}

void CSession::SetReturnTripMsecs()	
{
	LARGE_INTEGER rawTicks;
	LARGE_INTEGER freq;
	::QueryPerformanceCounter(&rawTicks);
	::QueryPerformanceFrequency(&freq);
	rawTicks.QuadPart *= 1000;
	rawTicks.QuadPart /= freq.QuadPart;

	if(!m_pThis->m_bReturnTripSet)
	{
	if (rawTicks.QuadPart - m_pThis->m_nInputRawTicks.QuadPart  < 0)
		m_pThis->m_nReturnTripMsecs.Format("%u",rawTicks.QuadPart - m_pThis->m_nInputRawTicks.QuadPart + 2147483647);
	else
		m_pThis->m_nReturnTripMsecs.Format("%u",rawTicks.QuadPart - m_pThis->m_nInputRawTicks.QuadPart);


		m_pThis->m_bReturnTripSet = TRUE;
	}
}
CString CSession::GetCurrentLogFile()
{
	return m_pThis->m_strCurrentLogFile;
}
void CSession::SetCurrentLogFile(CString strNewValue)
{
	m_pThis->m_strCurrentLogFile = strNewValue;
}

void CSession::SetANSICopy(CString sNewValue)	
{
	m_pThis->m_ANSICopy = sNewValue;
}

time_t CSession::GetTimeSinceLastInput()					
{
	return m_pThis->m_tTimeSinceLastInput;
}

void CSession::SetTimeSinceLastInput()	
{
	m_pThis->m_bReturnTripSet = FALSE;
	LARGE_INTEGER rawTicks;
	LARGE_INTEGER freq;
	::QueryPerformanceCounter(&rawTicks);
	::QueryPerformanceFrequency(&freq);
	rawTicks.QuadPart *= 1000;
	rawTicks.QuadPart /= freq.QuadPart;
	m_pThis->m_nInputRawTicks = rawTicks;

	m_pThis->m_tTimeSinceLastInput = time(NULL);
}
LPCSTR CSession::GetTelnetAuthkeyResponse()					
{
	return m_pThis->m_telnetAuthkeyResponse;
}

void CSession::SetTelnetAuthkeyResponse(LPCSTR newValue)					
{
	m_pThis->m_telnetAuthkeyResponse = newValue;
}
void CSession::SetTelnetNAWS(bool newValue)					
{
	m_pThis->m_bTelnetNAWS = newValue;
}
bool CSession::GetTelnetNAWS()					
{
	return m_pThis->m_bTelnetNAWS;
}
void CSession::SetTelnetNAWSmessage(LPCSTR newValue)					
{
	m_pThis->m_TelnetNAWSmessage = newValue;
}
LPCSTR CSession::GetTelnetNAWSmessage()					
{
	return m_pThis->m_TelnetNAWSmessage;
}

BOOL CSession::GetConnected()					
{
	return m_pThis->m_bConnected;
}
BOOL CSession::GetHandledByDll()					
{
	return m_pThis->m_bHandledByDll;
}

void CSession::SetConnected(BOOL bValue)		
{
	m_pThis->m_bConnected = bValue;
}
void CSession::SetHandledByDll(BOOL bValue)		
{
	m_pThis->m_bHandledByDll = bValue;
}

CString &CSession::GetListItem()				
{
	return m_pThis->m_strListItem;
}

void CSession::SetListItem(LPCTSTR lpszNewValue) 
{
	m_pThis->m_strListItem = lpszNewValue;
}

COptions &CSession::GetOptions() const
{
	return m_pThis->m_options;
}

HRESULT CSession::CloseChatSocket(LPCTSTR pszIndex)
{
	CChatServer &server = GetChat();
	return server.CloseChatSocket(pszIndex);
}

HRESULT CSession::PrintChatText(const char *szMsg)
{
	//TODO:KW test calling all dlls
	CString onReceive = "onReceiveChatLine";
	CString strReceived = szMsg;
	SetHandledByDll(FALSE);
	GetDlls().CallAll( this, onReceive, strReceived );
	//TODO:KW END test calling all dlls

	//Try with processing
	if(!GetHandledByDll())
		PrintAnsi(szMsg, true);
	return S_OK;
}

HRESULT CSession::PrintChatMessage(PCTSTR pszText)
{
	//TODO: KW added false for extra line make sure this is right
	QueueMessage(CMessages::MM_CHAT_MESSAGE, pszText,FALSE);
	return S_OK;
}

HRESULT CSession::PrintChatMessage(UINT nStringId)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		string str;
		ErrorHandlingUtils::TESTHR(StringUtils::LoadString(nStringId, str));
		//TODO: KW added false for extra line make sure this is right
		QueueMessage(CMessages::MM_CHAT_MESSAGE, str.c_str(),FALSE);

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}

int CSession::GetChatServerPort() const
{
	return GetOptions().chatOptions_.ListenPort();
}

CString CSession::GetMaximize() const
{
	return GetOptions().sessionInfo_.Maximize();
}
bool CSession::GetAutoAccept() const
{
	return GetOptions().chatOptions_.AutoAccept() ? true : false;
}
bool CSession::GetAutoServe() const
{
	return GetOptions().chatOptions_.AutoServe() ? true : false;
}

bool CSession::GetDoNotDisturb() const
{
	return GetOptions().chatOptions_.DoNotDisturb() ? true : false;
}

BYTE CSession::GetChatForeColor() const
{
	return GetOptions().colorOptions_.GetChatForeColor();
}

BYTE CSession::GetChatBackColor() const
{
	return GetOptions().colorOptions_.GetChatBackColor();
}

HRESULT CSession::GetSoundPath(std::string &path) const
{
	return GetOptions().pathOptions_.Get_SoundPath(path);
}

HRESULT CSession::GetUploadPath(std::string &path) const
{
	return GetOptions().pathOptions_.Get_UploadPath(path);
}

HRESULT CSession::GetDownloadPath(std::string &path) const
{
	return GetOptions().pathOptions_.Get_DownloadPath(path);
}

HRESULT CSession::GetFileVersion(std::string &version) const
{
	std::string v;
	CGlobals::GetFileVersion(v);
	version = v.c_str();
	return S_OK;
}

HRESULT CSession::GetIpAddress(std::string &address) const
{
	CString strIp = CGlobals::GetIPAddress();
	address = static_cast<PCSTR>(strIp);
	return S_OK;
}

HRESULT CSession::SendGroupToChat(
	const std::string &groupName, const std::string &personName)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		int nActions = 0;
		int nAliases = 0;
		int nMacros = 0;
		int nVariables = 0;
		int nEvents = 0;
		int nSubs = 0;
		int nGags = 0;
		int nHighs = 0;
		int nLists = 0;
		int nArrays = 0;
		int nBarItems = 0;

		nActions = GetActions()->SendGroup( 
			groupName.c_str(), personName.c_str(), GetChat() );

		nAliases = GetAliases()->SendGroup(
			CGlobals::GetCommandCharacter(),
			CommandTypeAlias, groupName, personName, GetChat());

		GetMacros()->SendGroup(
			CommandTypeMacro, groupName.c_str(),personName.c_str(), 
			GetChat(), nMacros);

		GetVariables()->SendGroup(
			CommandTypeVariable, groupName, personName, GetChat(), nVariables);

		GetEvents()->SendGroup(
			CommandTypeEvent, groupName.c_str(), personName.c_str(), 
			GetChat(), nEvents);

		GetEvents()->SendGroup(
			CommandTypeSubstitute, groupName.c_str(), personName.c_str(), 
			GetChat(), nEvents);

		GetGags()->SendGroup(
			CommandTypeGag, groupName.c_str(), personName.c_str(), 
			GetChat(), nGags);

		GetHighlights()->SendGroup(
			CommandTypeHighlight, groupName.c_str(), personName.c_str(), 
			GetChat(), nHighs);

		GetLists()->SendGroup(
			CommandTypeList, groupName.c_str(), personName.c_str(), 
			GetChat(), nLists);

		GetArrays()->SendGroup(
			CommandTypeArray, groupName.c_str(), personName.c_str(), 
			GetChat(), nArrays);

		GetStatusBar()->SendGroup(
			CommandTypeBarItem, groupName.c_str(), personName.c_str(), 
			GetChat(), nBarItems);

		CString strMessage;
		strMessage.Format("<CHAT> Sent %d actions, %d aliases, %d macros, %d variables,\n"
			"%d events, %d subs, %d gags, %d highlights, %d lists, %d arrays, %d status bar items.",
			nActions, nAliases, nMacros, 
			nVariables, nEvents, nSubs, nGags, nHighs, nLists, nArrays, nBarItems);

		PrintAnsiNoProcess(strMessage);
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}

void CSession::PrintAnsiNoProcess(const std::string &text)
{
	HRESULT hr = PrintAnsiNoProcess(text.c_str());
	ATLASSERT(SUCCEEDED(hr));
}
void CSession::PrintNoChatLine(LPCTSTR szText)
{
	ProcessLineNoChat(szText, false, false);
	
}

HRESULT CSession::PrintAnsiNoProcess(LPCTSTR szText)
{
	PrintAnsi(szText, false);
	return S_OK;
}

HRESULT CSession::StartChatServer()
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		int port = GetOptions().chatOptions_.ListenPort();

		ErrorHandlingUtils::TESTHR(GetChat().InitializeChatServer(port));
		QueueMessage(CMessages::MM_CHAT_MESSAGE, "Chat server started.");

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		QueueMessage(CMessages::MM_CHAT_MESSAGE, "Failed to start the chat server.");
		hr = e.Error();
	}

	return hr;
}

HRESULT CSession::StopChatServer()
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		ErrorHandlingUtils::TESTHR(GetChat().StopChatServer());
		QueueMessage(CMessages::MM_CHAT_MESSAGE, "Chat server has been shut down.");
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		QueueMessage(CMessages::MM_CHAT_MESSAGE, "Error while chat server was shutting down.");
		::OutputDebugString(_T("Error in CSession::StopChatServer\n"));
	}
	return hr;
}

void CSession::SetEditText(const std::string &text)
{
	SetEditText(text.c_str());
}

bool CSession::printAutoList()
{
	QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Auto Commands:");

	int nCount = 1;
	std::stringstream buffer;
	for(POSITION pos = GetAutoList().GetHeadPosition();pos;)
	{
		CString str = GetAutoList().GetNext(pos);
		buffer
			<< ANSI_F_BOLDWHITE
			<< std::setw(3) << nCount++ << ": "
			<< ANSI_RESET
			<< static_cast<LPCTSTR>( str )
			<< std::endl;
	}

	buffer << std::endl;

	PrintAnsiNoProcess( buffer.str() );

	return true;
}

bool CSession::addAuto( const CString &command, const CString &execute )
{
	GetAutoList().AddHead( command );
	QueueMessage(CMessages::MM_AUTO_MESSAGE, "[autoexec] Auto command added.");

	CString ex(execute);
	ex.MakeLower();

	if( ex.IsEmpty() || ex == "true" )
	{
		ExecuteCommand( command );
	}

	return true;
}

void CSession::RedrawStatusBar()
{
	RedrawStatusBar(this);
}

void CSession::ProcessEvents()
{
	if (!GetOptions().sessionInfo_.ProcessEvents() || GetEvents()->GetCount() < 1)
		return;

	time_t now = time(NULL);
	CString eventText;
	CMMEvent *pEvent = (CMMEvent *)GetEvents()->GetFirst();
	while(pEvent)
	{
		if (pEvent->Enabled() && now - pEvent->Started() >= pEvent->Seconds())
		{
			pEvent->Started(now);
			eventText = pEvent->Event();

			DebugStack().Push(GetEvents()->GetFindIndex(), STACK_TYPE_EVENT, CString(""));
			ExecuteCommand( eventText );
			DebugStack().Pop();
		}
		pEvent = (CMMEvent *)GetEvents()->GetNext();
	}
}