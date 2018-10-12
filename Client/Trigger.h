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
	static const DWORD MAX_TRIGGER_VARS = 10;

	/**
	* \class CTrigger Trigger.h 
	* \brief base class for all triggers used by MudMaster 2k6
	*/
	class CTrigger : public CScriptEntity 
	{
	public:
		CTrigger();
		virtual ~CTrigger();

		// Copy constructor
		CTrigger(const CTrigger& src);

		// Assignment operator
		virtual CTrigger &operator=(const CTrigger& src);

		////////////////////////////////////////////////////////////////////
		// Mutators
		////////////////////////////////////////////////////////////////////
		// Parses the trigger and fills in the var and token lists
		BOOL SetTrigger(const CString& strTrigger);

		// Set the enabled status of the trigger
		void Enabled(BOOL bEnabled)		{m_bEnabled = bEnabled;}

		// Set the group that this trigger belongs to
		void Group(const CString& strGroup)	{m_strGroup = strGroup;}

		////////////////////////////////////////////////////////////////////
		// Accessors
		////////////////////////////////////////////////////////////////////
		// Returns a formatted trigger built from the tokens and variables
		const CString& GetTrigger() const;

		// All triggers are "groupable"  so we'll put the Group variable here
		const CString& Group() const	{return m_strGroup;}

		// All triggers are "disablable" so we'll put the enabled variable at this level
		BOOL Enabled(void) const		{return m_bEnabled;}

		////////////////////////////////////////////////////////////////////
		// Accessors
		////////////////////////////////////////////////////////////////////
		// This function returns true if the string passed in matches the trigger
		BOOL Match(const CString& strText) const;

		// Implementation routines
	private:
		// Using the variables and tokens, this function will initialize the m_strTrigger variable
		void FormatTrigger();

		// Instance Data
	protected:
		// The group that this trigger belongs to
		CString m_strGroup;

		// The formatted trigger.
		CString m_strTrigger;

		// True if the trigger should be processed
		BOOL m_bEnabled;

		// True if the trigger must start at the beginning of a line
		BOOL m_bAnchored;

		// True if the trigger begins with a wildcard
		BOOL m_bStartVar;

		// There can be up to ten variables in an trigger
		BYTE m_nVar[MAX_TRIGGER_VARS];		// Holds the variables
		BYTE m_nNumVars;		// The number of variables for this trigger

		// There can be up to ten tokens in a trigger
		int m_nNumTokens;			// The number of tokens for this trigger
		CString m_strTokens[MAX_TRIGGER_VARS];	// Holds the tokens
	};
}