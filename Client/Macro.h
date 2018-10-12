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
	class CMacro : public CScriptEntity
	{
	public:
		enum MacroDestination
		{
			MacroDestMud	= 0,
			MacroDestBar	= 1,
			MacroDestBarCr	= 2,
		};

		void KeyToName(CString &strName);
		void MapToText(CString &strText, BOOL bIncludeGroup);
		void MapToCommand(CString &strCommand);

		CMacro();
		virtual ~CMacro();

		//ACCESSORS
		WORD VirtualKeyCode()				{return m_wVirtualKeyCode;}
		WORD Modifiers()					{return m_wModifiers;}
		int	 Dest()							{return m_nDest;}
		CString& Action()					{return m_strAction;}
		const CString& Group() const		{return m_strGroup;}
		BOOL Enabled()						{return m_bEnabled;}
		CString& Name()						{return m_strName;}

		//MUTATORS
		void VirtualKeyCode(WORD wKeyCode)	{m_wVirtualKeyCode = wKeyCode;}
		void Modifiers(WORD wModifiers)		{m_wModifiers = wModifiers;}
		void Dest(int nDest)				{m_nDest = nDest;}
		void Action(const CString& strAction){m_strAction = strAction;}
		void Group(const CString& strGroup)	{m_strGroup = strGroup;}
		void Enabled(BOOL bEnabled)			{m_bEnabled = bEnabled;}
		void Name(const CString& strName)	{m_strName = strName;}

	private:
		CString m_strName;
		WORD		m_wVirtualKeyCode;
		WORD		m_wModifiers;
		int			m_nDest;			// See defines above.
		CString		m_strAction;		// What the user has defined for it.
		CString		m_strGroup;
		BOOL		m_bEnabled;
	};
}