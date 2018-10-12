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
// SessionHost.h: interface for the CSessionHost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONHOST_H__0984D4C7_20FA_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_SESSIONHOST_H__0984D4C7_20FA_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Messages.h"

class CSession;
class CChatContext;

namespace MMScript
{
	class CBarItem;
}

class CSessionHost  
{
public:
	virtual void OpenLog(CString &strFileName, bool bAppend)                                     = 0;
	virtual void CloseLog()                                                                      = 0;
	virtual void OpenRawMudLog(CString &strFileName, bool bAppend)                               = 0;
	virtual void CloseRawMudLog()                                                                = 0;
	virtual void LoadScript()                                                                    = 0;
	virtual void HideStatusBar()                                                                 = 0;
	virtual void UpdateBarItem(MMScript::CBarItem *pItem)                                        = 0;
	virtual void SetEditText(const CString &strText)                                             = 0;
	virtual void ExecuteCommand(LPCTSTR pszCommand)                                              = 0;
	virtual void StartChangesThread()                                                            = 0;
	virtual void UpdateMacros()                                                                  = 0;
	virtual void ReadScript()                                                                    = 0;
	virtual void ConnectToMud()                                                                  = 0;
	virtual void CloseMud()                                                                      = 0;
	virtual void QueueMessage(CMessages::MsgType msgType, const CString& strMsg, BOOL bCR=FALSE) = 0;
	virtual void PlayASound(CString &strFilename)                                                = 0;
	virtual void SaveScrollBack(CString& strFilename)                                            = 0;
	virtual void ClearScreen(/*CView *pView*/)			                                         = 0;
	virtual void ScrollHome(/*CView *pView*/)			                                         = 0;
	virtual void ScrollLineUp(/*CView *pView*/)			                                         = 0;
	virtual void ScrollLineDown(/*CView *pView*/)		                                         = 0;
	virtual void ScrollPageUp(/*CView *pView*/)			                	                     = 0;
	virtual void ScrollPageDown(/*CView *pView*/)		                                         = 0;
	virtual void EndScrollBack(/*CView *pView*/)		               	                         = 0;
	virtual void StartLog()							               	                             = 0;
	virtual void SubmitEditText()						                                         = 0;
	virtual HRESULT PrintSnoop(const char *pszLine)		                                         = 0;
	virtual void SendCommandToAnotherWindow(CString &window, const char *command)                = 0;
	virtual void SendTextToAnotherWindow(CString &window, const char *command,bool dotriggers)   = 0;
	virtual void RemoveWindowCaption()			                                                 = 0;
	virtual void MaxRestoreSession()                                                             = 0;
	virtual void AddWindowCaption()			                                                     = 0;
	virtual void UsePlaySound(BOOL bUsePlaySound)		                                         = 0;
	virtual void UpdateStatusBarOptions()				                                         = 0;
	virtual void ToggleVerbatim()						                                         = 0;
	virtual CWnd* ThisWnd()                                                                       = 0;
	virtual void TimeStampLog()					               		                             = 0;
	virtual void TaskbarNotification(CString &strMessage)     		                             = 0;

};

#endif // !defined(AFX_SESSIONHOST_H__0984D4C7_20FA_11D4_BCD6_00E029482496__INCLUDED_)
