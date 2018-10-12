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

#include "ScriptEntityList.h"
#include "ChatServer.h"

class CSession;

namespace MMScript
{
	class CBarItem;

	class CMMStatusBar : public CScriptEntityList
	{
		static const DWORD DEFAULT_BAR_ARRAY_SIZE = 100;
		static const DWORD DEFAULT_BAR_GROW_SIZE = 10;

	public:
		virtual CScriptEntity *FindExactImpl(const CString& strSearch) const;
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity);
		virtual BOOL RemoveImpl(const CString& strSearch);

		CMMStatusBar();
		~CMMStatusBar();

		// Draws the status bar on the screen.  Takes care of adjusting
		// the scrolling lines of the mud window as well as moving the
		// input bar.
		void ShowBar(CSession *pSession);

		// Hides the bar, adjusts the input bar position and number of
		// scrolling lines.
		void HideBar(CSession *pSession);

		// Redraw the whole status bar and all the items.
		// void Redraw(CSession *pSession);

		void DrawItem(CSession *pSession, CBarItem *pItem);

		void SetBarFore(CSession *pSession, int nColor);
		void SetBarBack(CSession *pSession, int nColor);

		int  GetBarFore()			{ return(m_nFore); }
		int  GetBarBack()			{ return(m_nBack); }

		void SetBarItemFore(CSession *pSession, CBarItem *pItem, BYTE nColor);
		void SetBarItemBack(CSession *pSession, CBarItem *pItem, BYTE nColor);
		CBarItem *FindItem(LPCSTR pszItem);
		CBarItem *FindItemByPosition(int iPos);
		void ShiftItems(int nStart, int nEnd, int nNum);

		int AddBarItem(LPCSTR pszItem, LPCSTR pszText, int nPos,
			int nLen, BYTE nFore, BYTE nBack, LPCSTR pszGroup);
		int AddSeparator(LPCSTR pszItem, int nPos, LPCSTR pszGroup);

		int DisableGroup(LPCSTR pszGroup);
		int EnableGroup(LPCSTR pszGroup);

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const CString &groupName, 
			const CString &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);


	private:
		// The line to draw the status bar on.
		int m_nLine;

		// Color of the status bar.
		int  m_nFore;
		int  m_nBack;

		// TRUE when showing.
		BOOL m_bShowing;

		// Whether above or below the status bar.
		int m_nPosition;
	};
}