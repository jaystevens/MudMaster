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
// Ifx.h : main header file for the IFX application
//

#if !defined(AFX_IFX_H__337A98C7_20BC_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_IFX_H__337A98C7_20BC_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
//#include "sound.h"
#include "msp.h"

/////////////////////////////////////////////////////////////////////////////
// CIfxApp:
// See Ifx.cpp for the implementation of this class
//
//#ifndef MM2K6_NODIRECTSOUND
class CGlobalSoundManager;
//#endif /* MM2K6_NODIRECTSOUND */

class CIfxApp : public CWinApp
{
public:
	CIfxApp();

	void MidiOpen(BOOL bMidiOpen);
	BOOL MidiOpen();
	CMsp& MSP();

//#ifndef MM2K6_NODIRECTSOUND
	CGlobalSoundManager *m_pSoundManager;
//#endif /* MM2K6_NODIRECTSOUND */

	const CString &GetTerminalWindowClassName() const;
	const CString &GetStatusWindowClassName() const;
	const CString &GetDefaultSessionFileName() const;

	void SetDefaultSessionFileName(const CString &name);

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

private:
#ifdef _DEBUG_PROCESS
	static DWORD WINAPI UserThreadProc(LPVOID);
#endif

private:
	struct AppImpl;
	std::auto_ptr<AppImpl> _pImpl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IFX_H__337A98C7_20BC_11D4_BCD6_00E029482496__INCLUDED_)
