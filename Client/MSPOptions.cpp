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
#include "MSPOptions.h"
#include "Globals.h"
#include "ConfigFileConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
#define MSP_SECTION		"Msp Options"
#define MSP_ON_KEY		"Msp On"
#define USE_PS_KEY		"Use Playsound"
#define SHOW_MSP_KEY	"Show MSP Triggers"
#define PROC_FX_KEY		"Process Effects"

namespace SerializedOptions
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CMspOptions::CMspOptions()
		: m_bOn(FALSE)
		, m_bProcessFX(FALSE)
		, m_bProcessMidi(FALSE)
		, m_bShowMSPTriggers(FALSE)
		, m_bUsePlaySound(FALSE)
	{
	}

	CMspOptions::CMspOptions(const CMspOptions& src) 
		: m_bOn(src.m_bOn)
		, m_bProcessFX(src.m_bProcessFX)
		, m_bProcessMidi(src.m_bProcessMidi)
		, m_bShowMSPTriggers(src.m_bShowMSPTriggers)
		, m_bUsePlaySound(src.m_bUsePlaySound)
	{
	}

	CMspOptions &CMspOptions::operator =(const CMspOptions &src)
	{
		CMspOptions temp(src);
		Swap(temp);
		return *this;
	}

	void CMspOptions::Swap(CMspOptions &src)
	{
		swap(m_bOn, src.m_bOn);
		swap(m_bProcessFX, src.m_bProcessFX);
		swap(m_bShowMSPTriggers, src.m_bShowMSPTriggers);
		swap(m_bUsePlaySound, src.m_bUsePlaySound);
	}

	HRESULT CMspOptions::Read(LPCTSTR lpszPathName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			m_bOn				= GET_ENTRY_INT(MSP_SECTION, MSP_ON_KEY,	FALSE) ? true : false;
			m_bProcessFX		= GET_ENTRY_INT(MSP_SECTION, PROC_FX_KEY,	FALSE) ? true : false;
			m_bShowMSPTriggers	= GET_ENTRY_INT(MSP_SECTION, SHOW_MSP_KEY,	FALSE) ? true : false;
			m_bUsePlaySound		= GET_ENTRY_INT(MSP_SECTION, USE_PS_KEY,	FALSE) ? true : false;
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return hr;
	}

	HRESULT CMspOptions::Save(LPCTSTR lpszPathName)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			PUT_ENTRY_INT(MSP_SECTION, MSP_ON_KEY,			m_bOn);
			PUT_ENTRY_INT(MSP_SECTION, PROC_FX_KEY,			m_bProcessFX);
			PUT_ENTRY_INT(MSP_SECTION, SHOW_MSP_KEY,		m_bShowMSPTriggers);
			PUT_ENTRY_INT(MSP_SECTION, USE_PS_KEY,			m_bUsePlaySound);
			hr = S_OK;
		}
		catch(_com_error &e)
		{
			hr = e.Error();
		}

		return hr;
	}

	CMspOptions::~CMspOptions()
	{
	}
}