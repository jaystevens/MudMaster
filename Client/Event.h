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

#include "ScriptEntity.h"

namespace MMScript
{
	class CMMEvent : public CScriptEntity
	{
	public:
		void MapToDisplayText(CString &strText);
		void MapToCommand(CString &strCommand);
		void MapToText(CString &strText, BOOL bIncludeGroup);

		CMMEvent();
		CMMEvent(const CMMEvent& src);
		~CMMEvent();

		CMMEvent& operator=(const CMMEvent& src);

		int Seconds()					{return m_nSeconds;}
		time_t Started()				{return m_tStarted;}
		CString& Event()				{return m_strEvent;}
		CString& Name()					{return m_strName;}
		BOOL Enabled()					{return m_bEnabled;}
		const CString& Group() const	{return m_strGroup;}

		void Seconds(int nSeconds)		{m_nSeconds = nSeconds;}
		void Started(time_t tStarted)	{m_tStarted = tStarted;}
		void Event(const char *strEvent)	{m_strEvent = strEvent;}
		void Name(const char *strName)		{m_strName = strName;}
		void Group(const CString& strGroup)	{m_strGroup = strGroup;}
		void Enabled(BOOL bEnabled)		{m_bEnabled = bEnabled;}

	private:
		// Number of seconds to elapse before firing the
		// event.
		int		m_nSeconds;

		// Time the event was started.
		time_t	m_tStarted;

		// The stuff to do once the event gets fired.  This is just
		// a string that gets sent to the command parser.  It can
		// be text to be sent to the mud or mud master commands.
		CString	m_strEvent;

		// The events can be named to make things easier to automate
		// them.
		CString	m_strName;

		// True when it should be active.
		BOOL	m_bEnabled;

		CString	m_strGroup;
	};
}