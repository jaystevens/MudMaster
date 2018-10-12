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
	class CSessionInfo {
	public:
		CSessionInfo();
		CSessionInfo(const CSessionInfo &src);
		CSessionInfo &operator=(const CSessionInfo &src);

		virtual ~CSessionInfo();

		HRESULT Read(LPCTSTR lpszPathName);
		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT SaveSessionPosition();
		///////////////////////////////////////////////////////////
		// Accessors
		///////////////////////////////////////////////////////////
		// IP address of the server
		LPCTSTR Address() {return m_strAddress.c_str();}

		// Name for this session
		LPCTSTR SessionName() {return m_strSessionName.c_str();}
		LPCTSTR SessionFileName() {return m_strSessionFileName.c_str();}

		// Port that the server runs on
		UINT Port() {return m_nPort;}

		// Name of the file for the session???
		LPCTSTR ScriptFilename() {return _scriptFilename.c_str();}

		// Free form text notes about this session file
		LPCTSTR SessionNotes() {return m_strSessionNotes.c_str();}

		// Time and date sesion was created
		CTime& CreationTime() {return m_timeCreate;}	

		// Total amount of time spent in this session
		CTime& TimeSpent() {return m_timeSpent;}	

		// Time this session started
		CTime& SessionStart() {return m_timeStart;}	

		// Start the chat server when this session is opened
		BOOL StartChatOnOpen() {return m_bStartChatOnOpen;}

		// Maximize the session window when this session is opened
		BOOL MaxOnOpen() {return m_bMaxOnOpen;}

		// Proccess substitutions before everything else
		BOOL PreSub() {return m_bPreSub;}

		// Set the pace in seconds (used for speed walk)
		int Pace() {return m_nPace;}

		// Start a timer to process events
		BOOL ProcessEvents() {return m_bProcessEvents;}

		// Do speedwalk
		BOOL SpeedWalk() {return m_bSpeedWalk;}

		// Prompt before overwriting files
		BOOL PromptOverwrite() {return m_bPromptOverwrite;}

		BOOL ShowStatusBar()	{return m_bShowStatusBar;}

		BOOL IgnoreAliases()	{return m_bIgnoreAliases;}

		BOOL AutoLoadScript()	{return m_bAutoLoadScript;}
		BOOL AutoLog()			{return m_bAutoLog;}
		BOOL AppendLog()		{return m_bAppendLog;}
		LPCTSTR LogFileName()	{return m_strLogFileName.c_str();}
		USHORT SubDepth()		{return m_nSubDepth;}
		//window position points and sizing
		int Top()				{return m_nTop;}
		int Left()				{return m_nLeft;}
		int Bottom()			{return m_nBottom;}
		int Right()				{return m_nRight;}
		CString Maximize()		{return m_strMaximize;}


		///////////////////////////////////////////////////////////
		// Mutators
		///////////////////////////////////////////////////////////
		void Address(LPCTSTR strAddress) {m_strAddress = strAddress;}
		void SessionName(LPCTSTR strSessionName) {m_strSessionName = strSessionName;}
		void SessionFileName(LPCTSTR strSessionFileName) {m_strSessionFileName = strSessionFileName;}
		void Port(UINT nPort) {m_nPort = nPort;}
		void ScriptFilename(LPCTSTR strSessionFileName) {_scriptFilename = strSessionFileName;}
		void SessionNotes(LPCTSTR strSessionNotes) {m_strSessionNotes = strSessionNotes;}
		void CreationTime(CTime& timeCreate) {m_timeCreate = timeCreate;}
		void TimeSpent(CTime& timeSpent) {m_timeSpent = timeSpent;}
		void SessionStart(CTime& timeStart) {m_timeStart = timeStart;}
		void StartChatOnOpen(bool bStart) {m_bStartChatOnOpen = bStart;}
		void MaxOnOpen(bool bMaxOnOpen) {m_bMaxOnOpen = bMaxOnOpen;}
		void PreSub(bool bPreSub) {m_bPreSub = bPreSub;}
		void Pace(int nPace) {m_nPace = nPace;}
		void ProcessEvents(bool bProcess) {m_bProcessEvents = bProcess;}
		void SpeedWalk(bool bSpeedWalk) {m_bSpeedWalk = bSpeedWalk;}
		void PromptOverwrite(bool bPromptOverwrite) {m_bPromptOverwrite = bPromptOverwrite;}
		void ShowStatusBar(bool bShow)	{m_bShowStatusBar = bShow;}
		void IgnoreAliases(bool bIgnore) {m_bIgnoreAliases = bIgnore;}
		void AutoLoadScript(bool bAuto) {m_bAutoLoadScript = bAuto;}
		void AutoLog(bool bAutoLog)		{m_bAutoLog = bAutoLog;}
		void AppendLog(bool bAppend)	{m_bAppendLog = bAppend;}
		void LogFileName(LPCTSTR strFileName) {m_strLogFileName = strFileName;}
		void SubDepth(USHORT nSubDepth) {m_nSubDepth = nSubDepth;}
		//window position
		void Top(int nTop)				{m_nTop = nTop;}
		void Left(int nLeft)			{m_nLeft = nLeft;}
		void Bottom(int nBottom)			{m_nBottom = nBottom;}
		void Right(int nRight)			{m_nRight = nRight;}
		void Maximize(CString sMaximize) {m_strMaximize = sMaximize;}

	private:
		void Swap(CSessionInfo &src);

		USHORT m_nSubDepth;
		bool m_bAutoLoadScript;
		bool m_bAutoLog;
		bool m_bAppendLog;
		std::string m_strLogFileName;
		std::string m_strAddress;
		std::string m_strSessionName;
		std::string m_strSessionFileName;
		int m_nPort;
		std::string _scriptFilename;
		std::string m_strSessionNotes;
		CTime m_timeCreate;
		CTime m_timeSpent;
		CTime m_timeStart;
		bool m_bStartChatOnOpen;
		bool m_bMaxOnOpen;
		bool m_bPreSub;
		int m_nPace;
		bool m_bProcessEvents;
		bool m_bSpeedWalk;
		bool m_bPromptOverwrite;
		bool m_bShowStatusBar;
		bool m_bIgnoreAliases;
		//TODO: KW try including window position points
		int m_nTop;
		int m_nLeft;
		int m_nBottom;
		int m_nRight;
		CString m_strMaximize;
	};
}