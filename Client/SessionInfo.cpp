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
#include "ifx.h"
#include "SessionInfo.h"
#include "Globals.h"
#include "ConfigFileConstants.h"
#include "FileUtils.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

namespace SerializedOptions
{
#define SESSION_INFO_SECTION	_T("Session Information")
#define AUTO_LOG_KEY			_T("Auto Log")
#define APPEND_LOG_KEY			_T("Append Log")
#define LOG_FILENAME_KEY		_T("Log Filename")
#define MAX_ON_OPEN_KEY			_T("Maximize On Open")
#define PRE_SUB_KEY				_T("PreSub")
#define PROCESS_EVENTS_KEY		_T("Process Events")
#define PROMPT_OVERWRITE_KEY	_T("Prompt Overwrite")
#define SPEEDWALK_KEY			_T("Speedwalk")
#define START_CHAT_ON_OPEN_KEY	_T("Start Chat On Open")
#define PACE_KEY				_T("Pace")
#define MUD_PORT_KEY			_T("Mud Port")
#define MUD_ADDRESS_KEY			_T("Mud Address")
#define SESSION_NAME_KEY		_T("Session Name")
#define STATUS_BAR_KEY			_T("Status Bar")
#define IGNORE_ALIASES_KEY		_T("Ignore Aliases")
#define SESSION_FILENAME_KEY	_T("Session Filename")
#define SESSION_TOP_KEY			_T("Top")
#define SESSION_LEFT_KEY		_T("Left")
#define SESSION_BOTTOM_KEY		_T("Bottom")
#define SESSION_RIGHT_KEY		_T("Right")
#define MAXIMIZE_KEY			_T("Maximize")

	CSessionInfo::CSessionInfo()
		: m_bAppendLog(false)
		, m_bAutoLoadScript(true)
		, m_bAutoLog(false)
		, m_bIgnoreAliases(false)
		, m_bMaxOnOpen(false)
		, m_bPreSub(false)
		, m_bProcessEvents(true)
		, m_bPromptOverwrite(true)
		, m_bShowStatusBar(true)
		, m_bSpeedWalk(false)
		, m_bStartChatOnOpen(true)
		, m_nPace(0)
		, m_nPort(0)
		, m_nSubDepth(1)
		, m_timeCreate(0)
		, m_timeSpent(0)
		, m_timeStart(0)
		, m_strMaximize(_T("none"))
	{
	}

	CSessionInfo::CSessionInfo(const CSessionInfo &src)
		: m_bAppendLog(src.m_bAppendLog)
		, m_bAutoLoadScript(src.m_bAutoLoadScript)
		, m_bAutoLog(src.m_bAutoLog)
		, m_bIgnoreAliases(src.m_bIgnoreAliases)
		, m_bMaxOnOpen(src.m_bMaxOnOpen)
		, m_bPreSub(src.m_bPreSub)
		, m_bProcessEvents(src.m_bProcessEvents)
		, m_bPromptOverwrite(src.m_bPromptOverwrite)
		, m_bShowStatusBar(src.m_bShowStatusBar)
		, m_bSpeedWalk(src.m_bSpeedWalk)
		, m_bStartChatOnOpen(src.m_bStartChatOnOpen)
		, m_nPace(src.m_nPace)
		, m_nPort(src.m_nPort)
		, m_nSubDepth(src.m_nSubDepth)
		, m_strAddress(src.m_strAddress)
		, m_strLogFileName(src.m_strLogFileName)
		, _scriptFilename(src._scriptFilename)
		, m_strSessionName(src.m_strSessionName)
		, m_strSessionNotes(src.m_strSessionNotes)
		, m_timeCreate(src.m_timeCreate)
		, m_timeSpent(src.m_timeSpent)
		, m_timeStart(src.m_timeStart)
		, m_strMaximize(src.m_strMaximize)
	{
	}

	CSessionInfo &CSessionInfo::operator =(const CSessionInfo &src)
	{
		CSessionInfo temp(src);
		Swap(temp);
		return *this;
	}

	void CSessionInfo::Swap(CSessionInfo &src)
	{
		swap(m_bAppendLog,			src.m_bAppendLog);
		swap(m_bAutoLoadScript,		src.m_bAutoLoadScript);
		swap(m_bAutoLog,			src.m_bAutoLog);
		swap(m_bIgnoreAliases,		src.m_bIgnoreAliases);
		swap(m_bMaxOnOpen,			src.m_bMaxOnOpen);
		swap(m_bPreSub,				src.m_bPreSub);
		swap(m_bProcessEvents,		src.m_bProcessEvents);
		swap(m_bPromptOverwrite,	src.m_bPromptOverwrite);
		swap(m_bShowStatusBar,		src.m_bShowStatusBar);
		swap(m_bSpeedWalk,			src.m_bSpeedWalk);
		swap(m_bStartChatOnOpen,	src.m_bStartChatOnOpen);
		swap(m_nPace,				src.m_nPace);
		swap(m_nPort,				src.m_nPort);
		swap(m_nSubDepth,			src.m_nSubDepth);
		swap(m_strAddress,			src.m_strAddress);
		swap(m_strLogFileName,		src.m_strLogFileName);
		swap(m_strSessionName,		src.m_strSessionName);
		swap(m_strSessionNotes,		src.m_strSessionNotes);
		swap(m_timeCreate,			src.m_timeCreate);
		swap(m_timeSpent,			src.m_timeSpent);
		swap(m_timeStart,			src.m_timeStart);
		swap(_scriptFilename,		src._scriptFilename);
		swap(m_strMaximize,		src.m_strMaximize);
	}

	CSessionInfo::~CSessionInfo()
	{
	}

	HRESULT CSessionInfo::Save(LPCTSTR lpszPathName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			CMDIFrameWnd *pFrame = 
             (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
			CMDIChildWnd* pChild=(CMDIChildWnd *)pFrame->MDIGetActive();

			if(pChild->GetSafeHwnd()	!= NULL) //Can't get window dimensions if it doesn't exist
			{

				CRect SessionRect,ParentRect;
				pFrame->GetParentOwner()->GetWindowRect(&ParentRect);
				pFrame->GetActiveWindow()->GetWindowRect(&SessionRect);
				pChild->GetWindowRect(&SessionRect);

				//get the offsets from outer frame
				Bottom(SessionRect.bottom - ParentRect.bottom);
				Top(SessionRect.top - ParentRect.top);
				Left(SessionRect.left - ParentRect.left);
				Right(SessionRect.right - ParentRect.right);
			}	

			PUT_ENTRY_INT(SESSION_INFO_SECTION, AUTO_LOG_KEY,			m_bAutoLog);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, APPEND_LOG_KEY,			m_bAppendLog);
			PUT_ENTRY_STR(SESSION_INFO_SECTION, LOG_FILENAME_KEY,		m_strLogFileName.c_str());
			PUT_ENTRY_INT(SESSION_INFO_SECTION, MAX_ON_OPEN_KEY,		m_bMaxOnOpen);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, PRE_SUB_KEY,			m_bPreSub);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, PROCESS_EVENTS_KEY,		m_bProcessEvents);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, PROMPT_OVERWRITE_KEY,	m_bPromptOverwrite);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SPEEDWALK_KEY,			m_bSpeedWalk);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, START_CHAT_ON_OPEN_KEY,	m_bStartChatOnOpen);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, PACE_KEY,				m_nPace);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, MUD_PORT_KEY,			m_nPort);
			PUT_ENTRY_STR(SESSION_INFO_SECTION, MUD_ADDRESS_KEY,		m_strAddress.c_str());
			PUT_ENTRY_STR(SESSION_INFO_SECTION, SESSION_NAME_KEY,		m_strSessionName.c_str());
			PUT_ENTRY_INT(SESSION_INFO_SECTION, STATUS_BAR_KEY,			m_bShowStatusBar);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, IGNORE_ALIASES_KEY,		m_bIgnoreAliases);
			PUT_ENTRY_STR(SESSION_INFO_SECTION, SESSION_FILENAME_KEY,	_scriptFilename.c_str());
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_TOP_KEY,		m_nTop);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_LEFT_KEY,		m_nLeft);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_BOTTOM_KEY,		m_nBottom);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_RIGHT_KEY,		m_nRight);
			PUT_ENTRY_STR(SESSION_INFO_SECTION, MAXIMIZE_KEY,			m_strMaximize);
			CSessionInfo::SessionFileName(lpszPathName);
			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString(_T("Error saving session information\n"));
		}

		return hr;
	}
	HRESULT CSessionInfo::SaveSessionPosition()
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			LPCTSTR lpszPathName;
			CString SessionName;
			CMDIFrameWnd *pFrame = 
           (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
			CMDIChildWnd* pChild=(CMDIChildWnd *)pFrame->MDIGetActive();

			if(pChild->GetSafeHwnd()	!= NULL) //Can't get window dimensions if it doesn't exist
			{

			CRect SessionRect,ParentRect;
			pFrame->GetParentOwner()->GetWindowRect(&ParentRect);
			pFrame->GetActiveWindow()->GetWindowRect(&SessionRect);
			pChild->GetWindowRect(&SessionRect);	
			//pChild->GetWindowText(SessionName);
			 //((CIfxApp*)AfxGetApp())->GetDefaultSessionFileName();
			//SessionName = MMFileUtils::fixPath(SessionName);
			lpszPathName = SessionFileName();
			//get the offsets from outer frame
			Bottom(SessionRect.bottom - ParentRect.bottom);
			Top(SessionRect.top - ParentRect.top);
			Left(SessionRect.left - ParentRect.left);
			Right(SessionRect.right - ParentRect.right);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_TOP_KEY,		m_nTop);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_LEFT_KEY,		m_nLeft);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_BOTTOM_KEY,		m_nBottom);
			PUT_ENTRY_INT(SESSION_INFO_SECTION, SESSION_RIGHT_KEY,		m_nRight);

			}
		}

		catch(...)
		{
			::OutputDebugString(_T("Error saving session position information\n"));
		}

		return hr;
	}
	HRESULT CSessionInfo::Read(LPCTSTR lpszPathName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			m_bAutoLog			= GET_ENTRY_INT(SESSION_INFO_SECTION, AUTO_LOG_KEY,				FALSE) ? true : false;
			m_bAppendLog		= GET_ENTRY_INT(SESSION_INFO_SECTION, APPEND_LOG_KEY,			TRUE) ? true : false;
			m_strLogFileName	= GET_ENTRY_STR(SESSION_INFO_SECTION, LOG_FILENAME_KEY,			_T(""));
			m_bMaxOnOpen		= GET_ENTRY_INT(SESSION_INFO_SECTION, MAX_ON_OPEN_KEY,			FALSE) ? true : false;
			m_bPreSub			= GET_ENTRY_INT(SESSION_INFO_SECTION, PRE_SUB_KEY,				FALSE) ? true : false;
			m_bProcessEvents	= GET_ENTRY_INT(SESSION_INFO_SECTION, PROCESS_EVENTS_KEY,		TRUE) ? true : false;
			m_bPromptOverwrite	= GET_ENTRY_INT(SESSION_INFO_SECTION, PROMPT_OVERWRITE_KEY,		TRUE) ? true : false;
			m_bSpeedWalk		= GET_ENTRY_INT(SESSION_INFO_SECTION, SPEEDWALK_KEY,			FALSE) ? true : false;
			m_bStartChatOnOpen	= GET_ENTRY_INT(SESSION_INFO_SECTION, START_CHAT_ON_OPEN_KEY,	TRUE) ? true : false;
			m_nPace				= GET_ENTRY_INT(SESSION_INFO_SECTION, PACE_KEY,					1000);
			m_nPort				= GET_ENTRY_INT(SESSION_INFO_SECTION, MUD_PORT_KEY,				0);
			m_strAddress		= GET_ENTRY_STR(SESSION_INFO_SECTION, MUD_ADDRESS_KEY,			_T(""));
			m_strSessionName	= GET_ENTRY_STR(SESSION_INFO_SECTION, SESSION_NAME_KEY,			_T(""));
			m_bShowStatusBar	= GET_ENTRY_INT(SESSION_INFO_SECTION, STATUS_BAR_KEY,			TRUE) ? true : false;
			m_bIgnoreAliases	= GET_ENTRY_INT(SESSION_INFO_SECTION, IGNORE_ALIASES_KEY,		FALSE) ? true : false;
			_scriptFilename		= GET_ENTRY_STR(SESSION_INFO_SECTION, SESSION_FILENAME_KEY,		_T(""));
			m_nTop				= GET_ENTRY_INT(SESSION_INFO_SECTION, SESSION_TOP_KEY,			0);
			m_nLeft				= GET_ENTRY_INT(SESSION_INFO_SECTION, SESSION_LEFT_KEY,			0);
			m_nBottom			= GET_ENTRY_INT(SESSION_INFO_SECTION, SESSION_BOTTOM_KEY,		0);
			m_nRight			= GET_ENTRY_INT(SESSION_INFO_SECTION, SESSION_RIGHT_KEY,		0);
			m_strMaximize		= GET_ENTRY_STR(SESSION_INFO_SECTION, MAXIMIZE_KEY,				_T("none"));
			CSessionInfo::SessionFileName(lpszPathName);

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString(_T("Error reading session information\n"));
		}

		return hr;
	}
}