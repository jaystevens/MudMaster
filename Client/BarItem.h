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
#pragma	once

#include "ScriptEntity.h"

class CSession;

namespace MMScript
{
	class CBarItem : public CScriptEntity
	{
	public:
		void MapToDisplayText(CString &strText, CSession *pSession);
		void MapToText(CString &strText, BOOL bIncludeGroup);
		void MapToCommand(CString &strCommand);

		CBarItem();
		CBarItem(const CBarItem& src);
		virtual ~CBarItem();

		//ACCESSORS
		BOOL Enabled()						{return m_bEnabled;}
		BOOL Separator()					{return m_bSeparator;}
		BYTE BackColor()						{return m_nBack;}
		BYTE ForeColor()						{return m_nFore;}
		int Length()						{return m_nLength;}
		int Position()						{return m_nPosition;}
		const CString& Group() const		{return m_strGroup;}
		const CString& Item()				{return m_strItem;}
		const CString& Text()				{return m_strText;}

		//MUTATORS
		void Enabled(BOOL bEnabled)			{m_bEnabled = bEnabled;}
		void Separator(BOOL bSeparator)		{m_bSeparator = bSeparator;}
		void BackColor(BYTE nBack)			{m_nBack = nBack;}
		void ForeColor(BYTE nFore)			{m_nFore = nFore;}
		void Length(int nLength)			{m_nLength = nLength;}
		void Position(int nPosition)		{m_nPosition = nPosition;}
		void Group(const CString& strGroup)	{m_strGroup = strGroup;}
		void Item(const CString& strItem)	{m_strItem = strItem;}
		void Text(const CString& strText)	{m_strText = strText;}

		void DrawItem(CSession *pSession, CPtrArray &bar);

	private:
		BOOL		m_bEnabled;
		BOOL		m_bSeparator;	// TRUE if this is a separator.
		BYTE 		m_nBack;		// Background attribute.
		BYTE 		m_nFore;		// Foreground attribute.
		int			m_nLength;		// Max display len
		int			m_nPosition;	// XPos on the bar.
		CString		m_strGroup;
		CString		m_strItem;		// Name of this item.
		CString		m_strText;		// Text to display
	};
}