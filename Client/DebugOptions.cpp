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
#include "DebugOptions.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static LPCTSTR DEBUG_OPTIONS_SECTION = _T("Debug Options");
static LPCTSTR DEBUG_ECHO_KEY = _T("Debug Echo");
static LPCTSTR DEBUG_DEPTH_KEY = _T("Debug Depth");

using namespace std;

namespace SerializedOptions
{
	CDebugOptions::CDebugOptions()
		: m_bDebugEcho(FALSE)
		, m_nDebugDepth(0)
	{
	}

	CDebugOptions::CDebugOptions(const CDebugOptions &src)
		: m_bDebugEcho(src.m_bDebugEcho)
		, m_nDebugDepth(src.m_nDebugDepth)
	{
	}

	CDebugOptions &CDebugOptions::operator =(const CDebugOptions &src)
	{
		CDebugOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CDebugOptions::Swap(CDebugOptions &src)
	{
		swap(m_bDebugEcho, src.m_bDebugEcho);
		swap(m_nDebugDepth, src.m_nDebugDepth);
	}

	HRESULT CDebugOptions::Save(LPCTSTR lpszPathName)
	{
		PUT_ENTRY_INT(DEBUG_OPTIONS_SECTION, DEBUG_ECHO_KEY, m_bDebugEcho);
		PUT_ENTRY_INT(DEBUG_OPTIONS_SECTION, DEBUG_DEPTH_KEY, m_nDebugDepth);
		return S_OK;
	}

	HRESULT CDebugOptions::Read(LPCTSTR lpszPathName)
	{
		m_bDebugEcho = GET_ENTRY_INT(DEBUG_OPTIONS_SECTION, DEBUG_ECHO_KEY, FALSE);
		m_nDebugDepth = GET_ENTRY_INT(DEBUG_OPTIONS_SECTION, DEBUG_DEPTH_KEY, 5);
		return S_OK;
	}
}