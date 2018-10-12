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
// CommandTable.h: interface for the CCommandTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDTABLE_H__C6526542_7803_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_COMMANDTABLE_H__C6526542_7803_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "HashTable.h"
class CSession;
class CChat;

namespace MMScript
{
	class CTriggerList;
	class CScriptEntity;
}

// Declaration for a command function.
class CCommandTable  
{
public:
	typedef BOOL (*COMMANDPROC)(CSession *pSession, CString &strLine);	

	struct COMMANDLIST
	{
		char szCommand[31];
		COMMANDPROC proc;
		BOOL bWholeMatch;			// TRUE to require full text match for sensitive commands.
	};

public:
	static BOOL ShowHelpFile(CSession *pSession, const CString& strHelpFileName);
//	static BOOL ICQSendFile(CSession *pSession, CString &strSearch);
	static BOOL GroupCommand(CSession *pSession, CString& strGroup, MMScript::CScriptEntity *pEntity);
	static BOOL GrepSubs(CSession *pSession, CString &strSearch);
	static BOOL GrepMacros(CSession *pSession, CString &strSearch);
	static BOOL GrepHighs(CSession *pSession, CString &strSearch);
	static BOOL GrepLists(CSession *pSession, CString &strSearch);
	static BOOL GrepArrays(CSession *pSession, CString &strSearch);
	static BOOL GrepVariables(CSession *pSession, CString &strSearch);
	static BOOL GrepAliases(CSession *pSession, CString &strSearch);
	static BOOL GrepActions(CSession *pSession, CString &strSearch);
	static BOOL GrepEvents(CSession *pSession, CString &strSearch);
	static BOOL GrepGags(CSession *pSession, CString &strSearch);
	static BOOL DisableTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list);
	static BOOL EditTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list);
	static BOOL ToggleTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list, BOOL bEnabled);
	static BOOL EnableTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list);
	static BOOL KillTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list);
	static BOOL GrepTrigger(CSession *pSession, CString& strParam, MMScript::CTriggerList& list);

	static BOOL SendTrigger(CSession *pSession, CString& strLine, MMScript::CTriggerList& list, BYTE chCommand);
	static BOOL WriteTriggers(CStdioFile& file, MMScript::CTriggerList& list);
	static int WriteTriggersGroup(CStdioFile& file, const CString& strGroup, MMScript::CTriggerList& list);

	static COMMANDPROC FindCommand(CSession *pSession, LPCSTR pszCommand);
	static COMMANDLIST *FindCommandCommon(CSession *pSession, const CString& strCommand);
	static char *FindCommandName(CSession *pSession, const CString& strCommand);

	static BOOL Hash();
	static int Count();
	CCommandTable();
	virtual ~CCommandTable();

	static BOOL Action(CSession *pSession, CString &strLine);
	static BOOL AddCaption(CSession *pSession, CString &strLine);
	static BOOL Alias(CSession *pSession, CString &strLine);	
	static BOOL AllCommands(CSession *pSession, CString &strLine);	
	static BOOL Array(CSession *pSession, CString &strLine);
	static BOOL ArrayCopy(CSession *pSession, CString &strLine);
	static BOOL ArrayDBSave(CSession *pSession, CString &strLine);
	static BOOL ArrayDBRead(CSession *pSession, CString &strLine);
	static BOOL Assign(CSession *pSession, CString &strLine);
	static BOOL AutoAccept(CSession *pSession, CString &strLine);
	static BOOL AutoExec(CSession *pSession, CString& strLine);
	static BOOL AutoServe(CSession *pSession, CString &strLine);
    static BOOL Background(CSession *pSession, CString &strLine);
	static BOOL BarBack(CSession *pSession, CString &strLine);
	static BOOL BarFore(CSession *pSession, CString &strLine);
	static BOOL BarItem(CSession *pSession, CString &strLine);
	static BOOL BarItemBack(CSession *pSession, CString &strLine);
	static BOOL BarItemFore(CSession *pSession, CString &strLine);
	static BOOL BarItemShift(CSession *pSession, CString &strLine);
	static BOOL BarSeparator(CSession *pSession, CString &strLine);
	static BOOL Bell(CSession *pSession, CString &strLine);
	static BOOL Call(CSession *pSession, CString &strLine);
	static BOOL CallDLL(CSession *pSession, CString &strLine);
	static BOOL Changes(CSession *pSession, CString &strLine);
	static BOOL Char(CSession *pSession, CString &strLine);
	static BOOL Chat(CSession *pSession, CString &strLine);
	static BOOL ChatAll(CSession *pSession, CString &strLine);
	static BOOL ChatBack(CSession *pSession, CString &strLine);
	static BOOL ChatFore(CSession *pSession, CString &strLine);
	static BOOL ChatCommands(CSession *pSession, CString &strLine);
	static BOOL ChatGroupRemove(CSession *pSession, CString &strLine);
	static BOOL ChatGroupSet(CSession *pSession, CString &strLine);
	static BOOL ChatIgnore(CSession *pSession, CString &strLine);
	static BOOL ChatName(CSession *pSession, CString &strLine);
	static BOOL ChatPort(CSession *pSession, CString &strLine);
	static BOOL ChatPrivate(CSession *pSession, CString &strLine);
	static BOOL ChatExcludeServe(CSession *pSession, CString &strLine);
	static BOOL ChatServe(CSession *pSession, CString &strLine);
	static BOOL ChatSnoop(CSession *pSession, CString &strLine);
	static BOOL ChatTransfers(CSession *pSession, CString &strLine);
	static BOOL ChatVersions(CSession *pSession, CString &strLine);
	static BOOL ClearList(CSession *pSession, CString &strLine);
	static BOOL ClearScreen(CSession *pSession, CString &strLine);
	static BOOL CloseLog(CSession *pSession, CString &strLine);
	static BOOL CloseRawMudLog(CSession *pSession, CString &strLine);
	static BOOL CR(CSession *pSession, CString &strLine);
	static BOOL DBConnect(CSession *pSession, CString &strLine);
	static BOOL DebugDepth(CSession *pSession, CString &strLine);
	static BOOL DefaultScript(CSession *pSession, CString &strLine);
	static BOOL DefaultSession(CSession *pSession, CString &strLine);
	static BOOL Dir(CSession *pSession, CString &strLine);
	static BOOL DisableAction(CSession *pSession, CString &strLine);
	static BOOL DisableAlias(CSession *pSession, CString &strLine);
	static BOOL DisableBarItem(CSession *pSession, CString &strLine);
	static BOOL DisableEvent(CSession *pSession, CString &strLine);
	static BOOL DisableGag(CSession *pSession, CString &strLine);
	static BOOL DisableGroup(CSession *pSession, CString &strLine);
	static BOOL DisableHighlight(CSession *pSession, CString &strLine);
	static BOOL DisableMacro(CSession *pSession, CString &strLine);
	static BOOL DisableSub(CSession *pSession, CString &strLine);
	static BOOL Dll(CSession *pSession, CString &strLine);
	static BOOL Dnd(CSession *pSession, CString &strLine);
	static BOOL DoEvents(CSession *pSession, CString &strLine);
	static BOOL DownloadPath(CSession *pSession, CString &strLine);
	static BOOL Echo(CSession *pSession, CString &strLine);
	static BOOL EditAction(CSession *pSession, CString &strLine);
	static BOOL EditAlias(CSession *pSession, CString &strLine);
	static BOOL EditBarItem(CSession *pSession, CString &strLine);
	static BOOL EditEvent(CSession *pSession, CString &strLine);
	static BOOL EditGag(CSession *pSession, CString &strLine);
	static BOOL EditCurrentSession(CSession *pSession, CString &strLine);
	static BOOL EditHighlight(CSession *pSession, CString &strLine);
	static BOOL EditMacro(CSession *pSession, CString &strLine);
	static BOOL EditSub(CSession *pSession, CString &strLine);
	static BOOL EditVariable(CSession *pSession, CString &strLine);
	static BOOL EnableBarItem(CSession *pSession, CString &strLine);
	static BOOL EnableGag(CSession *pSession, CString &strLine);
	static BOOL EnableGroup(CSession *pSession, CString &strLine);
	static BOOL EnableHighlight(CSession *pSession, CString &strLine);
	static BOOL EnableSub(CSession *pSession, CString &strLine);
	static BOOL Emote(CSession *pSession, CString &strLine);
	static BOOL EmoteAll(CSession *pSession, CString &strLine);
	static BOOL Empty(CSession *pSession, CString &strLine);
	static BOOL EnableAction(CSession *pSession, CString &strLine);
	static BOOL EnableAlias(CSession *pSession, CString &strLine);
	static BOOL EnableEvent(CSession *pSession, CString &strLine);
	static BOOL EnableMacro(CSession *pSession, CString &strLine);
	static BOOL Event(CSession *pSession, CString &strLine);
	static BOOL FileCancel(CSession *pSession, CString &strLine);
	static BOOL FileDelete(CSession *pSession, CString &strLine);
	static BOOL FileStatus(CSession *pSession, CString &strLine);
	static BOOL FireEvent(CSession *pSession, CString &strLine);
	static BOOL FreeLibrary(CSession *pSession, CString &strLine);
	static BOOL FullScreen(CSession *pSession, CString &strLine);
	static BOOL Gag(CSession *pSession, CString &strLine);
	static BOOL Grep(CSession *pSession, CString &strLine);
	static BOOL GroupActions(CSession *pSession, CString &strLine);
	static BOOL GroupAliases(CSession *pSession, CString &strLine);
	static BOOL GroupArrays(CSession *pSession, CString& strLine);
	static BOOL GroupBarItems(CSession *pSession, CString &strLine);
	static BOOL GroupEvents(CSession *pSession, CString &strLine);
	static BOOL GroupChat(CSession *pSession, CString &strLine);
	static BOOL GroupEmote(CSession *pSession, CString &strLine);
	static BOOL GroupGags(CSession *pSession, CString& strLine);
	static BOOL GroupHighs(CSession *pSession, CString& strLine);
	static BOOL GroupLists(CSession *pSession, CString &strLine);
	static BOOL GroupMacros(CSession *pSession, CString &strLine);
	static BOOL GroupSubs(CSession *pSession, CString &strLine);
	static BOOL GroupTablist(CSession *pSession, CString &strLine);
	static BOOL GroupVariables(CSession *pSession, CString &strLine);
	static BOOL HandledByDll(CSession *pSession, CString& strLine);
	static BOOL Help(CSession *pSession, CString& strLine);
	static BOOL Highlight(CSession *pSession, CString &strLine);
	static BOOL Ignore(CSession *pSession, CString &strLine);
	static BOOL IgnoreAliases(CSession *pSession, CString &strLine);
	static BOOL Information(CSession *pSession, CString &strLine);
	static BOOL Intersection(CSession *pSession, CString &strLine);
	static BOOL ItemAdd(CSession *pSession, CString &strLine);
	static BOOL ItemDelete(CSession *pSession, CString &strLine);
	static BOOL If(CSession *pSession, CString &strLine);
	static BOOL KillAll(CSession *pSession, CString &strLine);
	static BOOL KillGroup(CSession *pSession, CString &strLine);
	static BOOL ListAdd(CSession *pSession, CString &strLine);
	static BOOL ListCopy(CSession *pSession, CString &strLine);
	static BOOL ListDelete(CSession *pSession, CString &strLine);
	static BOOL ListItems(CSession *pSession, CString &strLine);
	static BOOL Lists(CSession *pSession, CString &strLine);
	static BOOL ListSort(CSession *pSession, CString &strLine);
	static BOOL LoadSession(CSession *pSession, CString &strLine);
	static BOOL LogPath(CSession *pSession, CString &strLine);
	static BOOL Loop(CSession *pSession, CString &strLine);
	static BOOL MMLoadLibrary(CSession *pSession, CString &strLine);
	static BOOL MaxOnOpen(CSession *pSession, CString &strLine);
	static BOOL MaxRestore(CSession *pSession, CString &strLine);
	static BOOL Message(CSession *pSession, CString &strLine);
	static BOOL Macro(CSession *pSession, CString &strLine);
	static BOOL Math(CSession *pSession, CString &strLine);
	static BOOL Msp(CSession *pSession, CString &strLine);
	static BOOL OnTop(CSession *pSession, CString &strLine);
	static BOOL OpenLog(CSession *pSession, CString &strLine);
	static BOOL OpenLogAppend(CSession *pSession, CString &strLine);
	static BOOL OpenRawMudLog(CSession *pSession, CString &strLine);
	static BOOL Pace(CSession *pSession, CString &strLine);
	static BOOL PeekConnections(CSession *pSession, CString &strLine);
	static BOOL Ping(CSession *pSession, CString &strLine);
	static BOOL PlayMidi(CSession *pSession, CString &strLine);
	static BOOL PlayWav(CSession *pSession, CString &strLine);
	static BOOL Presubstitute(CSession *pSession, CString &strLine);
	static BOOL ProcessFX(CSession *pSession, CString &strLine);
	static BOOL ProcessMidi(CSession *pSession, CString &strLine);
	static BOOL PromptOverwrite(CSession *pSession, CString &strLine);
	static BOOL Read(CSession *pSession, CString &strLine);
	static BOOL ReadViaWeb(CSession *pSession, CString &strLine);
	static BOOL Remark(CSession *pSession, CString &strLine);
	static BOOL RemoveCaption(CSession *pSession, CString &strLine);
	static BOOL RequestConnects(CSession *pSession, CString &strLine);
	static BOOL ResetEvent(CSession *pSession, CString &strLine);
	static BOOL SaveScrollback(CSession *pSession, CString &strLine);
	static BOOL ScreenShot(CSession *pSession, CString &strLine);
	static BOOL ScrollBack(CSession *pSession, CString &strLine);
	static BOOL ScrollGrep(CSession *pSession, CString &strLine);
	static BOOL SendAction(CSession *pSession, CString &strLine);
	static BOOL SendAlias(CSession *pSession, CString &strLine);
	static BOOL SendArray(CSession *pSession, CString &strLine);
	static BOOL SendBarItem(CSession *pSession, CString &strLine);
	static BOOL SendEvent(CSession *pSession, CString &strLine);
	static BOOL SendFile(CSession *pSession, CString &strLine);
	static BOOL SendGag(CSession *pSession, CString &strLine);
	static BOOL SendGroup(CSession *pSession, CString &strLine);
	static BOOL SendHighlight(CSession *pSession, CString &strLine);
	static BOOL SendList(CSession *pSession, CString &strLine);
	static BOOL SendMacro(CSession *pSession, CString &strLine);
	static BOOL SendSub(CSession *pSession, CString &strLine);
	static BOOL SendVariable(CSession *pSession, CString &strLine);
	static BOOL Session(CSession *pSession, CString &strLine);
	static BOOL SessionAddress(CSession *pSession, CString &strLine);
	static BOOL SessionName(CSession *pSession, CString &strLine);
	static BOOL SessionPath(CSession *pSession, CString &strLine);
	static BOOL SessionPort(CSession *pSession, CString &strLine);
	static BOOL SetBackColor(CSession *pSession, CString &strLine);
	static BOOL SetEventTime(CSession *pSession, CString &strLine);
	static BOOL SetForeColor(CSession *pSession, CString &strLine);
	//static BOOL SetFocus(CSession *pSession, CString &strLine);
	static BOOL SetIP(CSession *pSession, CString &strLine);
	static BOOL ShowMe(CSession *pSession, CString &strLine);
	static BOOL ShowMeViaWeb(CSession *pSession, CString &strLine);
	static BOOL ShowOther(CSession *pSession, CString &strLine);
	static BOOL ShowToast(CSession *pSession, CString &strLine);
	static BOOL ShowMSPTriggers(CSession *pSession, CString &strLine);
	static BOOL Snoop(CSession *pSession, CString &strLine);
	static BOOL SoundPath(CSession *pSession, CString &strLine);
	static BOOL Spam(CSession *pSession, CString &strLine);
	static BOOL SpeedWalk(CSession *pSession, CString &strLine);
	static BOOL SQLExecute(CSession *pSession, CString &strLine);
	static BOOL SQLQuery(CSession *pSession, CString &strLine);
	static BOOL StatusBar(CSession *pSession, CString &strLine);
	static BOOL StatusBarSize(CSession *pSession, CString &strLine);
	static BOOL StopChatServer(CSession *pSession, CString &strLine);
	static BOOL StartChatServer(CSession *pSession, CString &strLine);
	static BOOL SubDepth(CSession *pSession, CString &strLine);
	static BOOL Substitute(CSession *pSession, CString &strLine);
	static BOOL TabAdd(CSession *pSession, CString &strLine);
	static BOOL TabDelete(CSession *pSession, CString &strLine);
	static BOOL TabList(CSession *pSession, CString &strLine);
	static BOOL TextIn(CSession *pSession, CString &strLine);
	static BOOL TextOut(CSession *pSession, CString &strLine);
	static BOOL TimestampLog(CSession *pSession, CString &strLine);
	static BOOL UnAction(CSession *pSession, CString &strLine);
	static BOOL UnAlias(CSession *pSession, CString &strLine);
	static BOOL UnArray(CSession *pSession, CString &strLine);
	static BOOL UnAutoExec(CSession *pSession, CString& strLine);
	static BOOL UnBarItem(CSession *pSession, CString &strLine);
	static BOOL UnChat(CSession *pSession, CString &strLine);
	static BOOL UnDo(CSession *pSession, CString &strLine);
	static BOOL UnEvent(CSession *pSession, CString &strLine);
	static BOOL UnGag(CSession *pSession, CString &strLine);
	static BOOL UnHighlight(CSession *pSession, CString &strLine);
	static BOOL UnMacro(CSession *pSession, CString &strLine);
	static BOOL UsePlaySound(CSession *pSession, CString &strLine);
	static BOOL UnSubstitute(CSession *pSession, CString &strLine);
	static BOOL UnVariable(CSession *pSession, CString &strLine);
	static BOOL UpdateBarItem(CSession *pSession, CString &strLine);
	static BOOL UpdateStatusBar(CSession *pSession, CString &strLine);
	static BOOL UploadPath(CSession *pSession, CString &strLine);
	static BOOL Variable(CSession *pSession, CString &strLine);
	static BOOL Verbatim(CSession *pSession, CString &strLine);
	static BOOL Version(CSession *pSession, CString &strLine);
	static BOOL ViewLog(CSession *pSession, CString &strLine);
	static BOOL WalkStop(CSession *pSession, CString &strLine);
	static BOOL While(CSession *pSession, CString &strLine);
	static BOOL Window(CSession *pSession, CString &strLine);
	static BOOL Write(CSession *pSession, CString &strLine);
	static BOOL WriteGroup(CSession *pSession, CString &strLine);
	static BOOL UnZap(CSession *pSession, CString &strLine);
	static BOOL Zap(CSession *pSession, CString &strLine);

private:
	static HRESULT PrintChatList(CSession *pSession);

	static COMMANDLIST m_List[];
	static CHashTable m_HashTable;
};

#endif // !defined(AFX_COMMANDTABLE_H__C6526542_7803_11D3_BCD6_00E029482496__INCLUDED_)
