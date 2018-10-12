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

#include "CharFormat.h"
#include "ScriptEntity.h"

namespace MMScript
{
	class CHigh : public CScriptEntity
	{
	public:
		void MapToText(CString &strText, BOOL bIncludeGroup);
		void MapToCommand(CString &strCommand);
		void ToMask(CString &strMask);

		void ColorToString(CString &strColor);
		BYTE back();
		BYTE fore();
		void back(BYTE nBack);
		void fore(BYTE nFore);
		CHigh();
		CHigh(const CHigh& src);

		CHigh& operator=(const CHigh& src);

		virtual ~CHigh();

		//ACCESSORS
		BOOL Anchored()	const					{return m_bAnchored;}
		int Tokens() const						{return m_nTokens;}
		int Vars() const						{return m_nVars;}
		const CString& Token(int nToken) const	{return m_strToken[nToken];}
		BOOL StartVar()	const					{return m_bStartVar;}
		const CString& Group(void) const		{return m_strGroup;}
		BOOL Enabled(void) const				{return m_bEnabled;}

		//MUTATORS
		void Anchored(BOOL bAnchored)					{m_bAnchored = bAnchored;}
		void Tokens(int nTokens)						{m_nTokens = nTokens;}
		void Vars(int nVars)							{m_nVars = nVars;}
		void Token(int nToken, const CString& strToken) {m_strToken[nToken] = strToken;}
		void StartVar(BOOL bStartVar)					{m_bStartVar = bStartVar;}
		void Group(const CString& strGroup)				{m_strGroup = strGroup;}
		void Enabled(BOOL bEnabled)						{m_bEnabled = bEnabled;}

	private:
		BYTE m_fore;
		BYTE m_back;

		BOOL	m_bAnchored;
		int		m_nTokens;			// Number of tokens in the highlight.
		int		m_nVars;			// Number of variables in the highlight.
		CString m_strToken[10];		// The tokens for the action.
		BOOL	m_bStartVar;		// True if the action starts with a variable.
		CString m_strGroup;
		BOOL	m_bEnabled;
	};
}