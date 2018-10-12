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

namespace SerializedOptions
{
	class CTriggerOptions
	{
#define TRIGGER_OPTIONS_SECTION	_T("Trigger Options")
#define IGNORE_TRIGGERS_KEY		_T("Ignore All Triggers")
#define IGNORE_ACTIONS_KEY		_T("Ignore Actions")
#define IGNORE_GAGS_KEY			_T("Ignore Gags")
#define IGNORE_HIGHS_KEY		_T("Ignore Highlights")
#define IGNORE_SUBS_KEY			_T("Ignore Substitutions")

	public:
		CTriggerOptions();
		CTriggerOptions(const CTriggerOptions &src);
		CTriggerOptions &operator=(const CTriggerOptions &src);

		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT Read(LPCTSTR lpszPathName);

		//ACCESSORS
		BOOL IgnoreTriggers(){return m_bIgnoreTriggers;}
		BOOL IgnoreActions() {return m_bIgnoreActions;}
		BOOL IgnoreGags()	 {return m_bIgnoreGags;}
		BOOL IgnoreHighs()	 {return m_bIgnoreHighs;}
		BOOL IgnoreSubs()	 {return m_bIgnoreSubs;}

		//MUTATORS
		void IgnoreTriggers(BOOL bIgnoreTriggers) {m_bIgnoreTriggers = bIgnoreTriggers;}
		void IgnoreActions(BOOL bIgnoreActions) {m_bIgnoreActions = bIgnoreActions;}
		void IgnoreGags(BOOL bIgnoreGags) {m_bIgnoreGags = bIgnoreGags;}
		void IgnoreHighs(BOOL bIgnoreHighs) {m_bIgnoreHighs = bIgnoreHighs;}
		void IgnoreSubs(BOOL bIgnoreSubs) {m_bIgnoreSubs = bIgnoreSubs;}

	private:
		void Swap(CTriggerOptions &src);
		BOOL m_bIgnoreTriggers;
		BOOL m_bIgnoreActions;
		BOOL m_bIgnoreGags;
		BOOL m_bIgnoreHighs;
		BOOL m_bIgnoreSubs;
	};
}