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
// GeneralOptions.h: interface for the CGeneralOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERALOPTIONS_H__65032D4A_7CDD_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_GENERALOPTIONS_H__65032D4A_7CDD_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CGeneralOptions : public CObject  
{
public:

	CGeneralOptions();
	CGeneralOptions(const CGeneralOptions& src);
	virtual ~CGeneralOptions();

	//GENERAL FUNCTIONS
	static void SaveDefaults();
	static void LoadDefaults();

	//ACCESSORS
	BOOL DoEvents()			{return m_bDoEvents;}
	BOOL IgnoreAliases()	{return m_bIgnoreAliases;}
	BOOL Maximize()			{return m_bMaximize;}
	BOOL Pause()			{return m_bPause;}
	BOOL SpeedWalk()		{return m_bSpeedWalk;}
	BOOL PromptOverwrite()	{return m_bPromptOverwrite;}
	BOOL ShowMismatches()	{return m_bShowMismatches;}
	UINT DebugDepth()		{return m_nDebugDepth;}
	BOOL DebugEcho()		{return m_bDebugEcho;}

	//MUTATORS
	void IgnoreAliases(BOOL bIgnoreAliases) {m_bIgnoreAliases = bIgnoreAliases;}
	void DoEvents(BOOL bDoEvents)			{m_bDoEvents = bDoEvents;}
	void SpeedWalk(BOOL bSpeedWalk)			{m_bSpeedWalk = bSpeedWalk;}
	void Maximize(BOOL bMaximize)			{m_bMaximize = bMaximize;}
	void Pause(BOOL bPause)					{m_bPause = bPause;}
	void PromptOverwrite(BOOL bPromptOverwrite)	{m_bPromptOverwrite = bPromptOverwrite;}
	void ShowMismatches(BOOL bShowMismatches)	{m_bShowMismatches = bShowMismatches;}
	void DebugDepth(UINT nDebugDepth)			{m_nDebugDepth = nDebugDepth;}
	void DebugEcho(BOOL bDebugEcho)				{m_bDebugEcho = bDebugEcho;}

private:
	// TRUE to echo all commands executed by an alias
	BOOL m_bDebugEcho;

	// TRUE to process events.
	BOOL m_bDoEvents;

	// TRUE to maximize the session window when it opens.
	BOOL m_bMaximize;

	// TRUE to speedwalk
	BOOL m_bSpeedWalk;
	
	// TRUE to ignore aliases
	BOOL m_bIgnoreAliases;

	// Pause on a full screen of text
	BOOL m_bPause;

	// Prompt before overwriting existing files
	BOOL m_bPromptOverwrite;

	// TRUE if mismatches erros in /if statements should be shown
	BOOL m_bShowMismatches;

	// Depth of debug stack
	UINT m_nDebugDepth;
};

#endif // !defined(AFX_GENERALOPTIONS_H__65032D4A_7CDD_11D3_BCD6_00E029482496__INCLUDED_)
