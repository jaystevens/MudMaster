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
#include "StdAfx.h"
#include "Ifx.h"
#include "Event.h"
#include "Globals.h"
#include "Sess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	CMMEvent::CMMEvent()
	{
		m_bEnabled = TRUE;
		m_nSeconds = 0;
		m_tStarted = 0;
	}

	CMMEvent::CMMEvent(const CMMEvent& src):
	m_bEnabled(src.m_bEnabled),
		m_nSeconds(src.m_nSeconds),
		m_strEvent(src.m_strEvent),
		m_strGroup(src.m_strGroup),
		m_strName(src.m_strName),
		m_tStarted(src.m_tStarted)
	{
	}

	CMMEvent& CMMEvent::operator =(const CMMEvent& src)
	{
		if(this == &src)
			return *this;

		m_bEnabled = src.m_bEnabled;
		m_nSeconds = src.m_nSeconds;
		m_strEvent = src.m_strEvent;
		m_strGroup = src.m_strGroup;
		m_strName = src.m_strName;
		m_tStarted = src.m_tStarted;

		return *this;
	}
	CMMEvent::~CMMEvent()
	{
	}

	void CMMEvent::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		if(bIncludeGroup)
		{
			strText.Format("{%s} {%d} {%s} {%s}",
				Name(),
				Seconds(),
				Event(),
				Group());
		}
		else
		{
			strText.Format("{%s} {%d} {%s} {%s}",
				Name(),
				Seconds(),
				Event());
		}
	}

	void CMMEvent::MapToCommand(CString &strCommand)
	{
		CString strText;
		MapToText(strText, TRUE);
		strCommand.Format("%cevent %s", CGlobals::GetCommandCharacter(), strText);
	}

	void CMMEvent::MapToDisplayText(CString &strText)
	{
		time_t tNow = time(NULL);
		int nDif = Seconds() - (tNow - Started());
		if (nDif < 0)
			nDif = 0;
		strText.Format("{%s} {F:%d} {T:%d} {%s} {%s}",
			Name(),
			Seconds(),
			nDif,
			Event(),
			Group());
	}
}