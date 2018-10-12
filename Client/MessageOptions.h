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

#include "resource.h"
#include "Ifx.h"

namespace SerializedOptions
{
	class CMessageOptions
	{
#define MESSAGE_OPTIONS_SECTION	"Message Options"
#define ACTION_MSG_KEY			"Action Msgs"
#define ALIAS_MSG_KEY			"Alias Msgs"
#define ARRAY_MSG_KEY			"Array Msgs"
#define AUTO_MSG_KEY			"Autoexec Msgs"
#define BAR_MSG_KEY				"Bar Msgs"
#define ENABLE_MSG_KEY			"Enable Msgs"
#define EVENT_MSG_KEY			"Event Msgs"
#define GAG_MSG_KEY				"Gag Msgs"
#define HIGH_MSG_KEY			"High Msgs"
#define ITEM_MSG_KEY			"Item Msgs"
#define LIST_MSG_KEY			"List Msgs"
#define MACRO_MSG_KEY			"Macro Msgs"
#define SHOW_INFO_KEY			"Show Info"
#define SUB_MSG_KEY				"Sub Msgs"
#define TAB_MSG_KEY				"Tab Msgs"
#define VAR_MSG_KEY				"Variable Msgs"

	public:
		CMessageOptions();
		CMessageOptions(const CMessageOptions& src);
		CMessageOptions &operator=(const CMessageOptions &src);

		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT Read(LPCTSTR lpszPathName);

		void AllOff();

		//ACCESSORS
		bool ActionMessages()	{return m_bActionMessages;}
		bool AliasMessages()	{return m_bAliasMessages;}
		bool ArrayMessages()	{return m_bArrayMessages;}
		bool AutoexecMessages()	{return m_bAutoMessages;}
		bool BarMessages()		{return m_bBarMessages;}
		bool EnableMessages()	{return m_bEnableMessages;}
		bool EventMessages()	{return m_bEventMessages;}
		bool GagMessages()		{return m_bGagMessages;}
		bool HighMessages()		{return m_bHighMessages;}
		bool ItemMessages()		{return m_bItemMessages;}
		bool ListMessages()		{return m_bListMessages;}
		bool MacroMessages()	{return m_bMacroMessages;}
		bool ShowInfo()			{return m_bShowInfo;}
		bool SubMessages()		{return m_bSubMessages;}
		bool TabMessages()		{return m_bTabMessages;}
		bool VariableMessages()	{return m_bVarMessages;}

		//MUTATORS
		void ActionMessages(bool bActionMessages)	{m_bActionMessages	= bActionMessages;}
		void AliasMessages(bool bAliasMessages)		{m_bAliasMessages	= bAliasMessages;}
		void ArrayMessages(bool bArrayMessages)		{m_bArrayMessages	= bArrayMessages;}
		void AutoexecMessages(bool bAutoMessages)	{m_bAutoMessages	= bAutoMessages;}
		void BarMessages(bool bBarMessages)			{m_bBarMessages		= bBarMessages;}
		void EnableMessages(bool bEnableMessages)	{m_bEnableMessages	= bEnableMessages;}
		void EventMessages(bool bEventMessages)		{m_bEventMessages	= bEventMessages;}
		void GagMessages(bool bGagMessages)			{m_bGagMessages		= bGagMessages;}
		void HighMessages(bool bHighMessages)		{m_bHighMessages	= bHighMessages;}
		void ItemMessages(bool bItemMessages)		{m_bItemMessages	= bItemMessages;}
		void ListMessages(bool bListMessages)		{m_bListMessages	= bListMessages;}
		void MacroMessages(bool bMacroMessages)		{m_bMacroMessages	= bMacroMessages;}
		void ShowInfo(bool bShowInfo)				{m_bShowInfo		= bShowInfo;}
		void SubMessages(bool bSubMessages)			{m_bSubMessages		= bSubMessages;}
		void TabMessages(bool bTabMessages)			{m_bTabMessages		= bTabMessages;}
		void VariableMessages(bool bVarMessages)	{m_bVarMessages		= bVarMessages;}

	private:
		void Swap(CMessageOptions &src);
		//////////////////////////////////////////////////////////////////
		// Messages.
		bool m_bActionMessages;
		bool m_bAliasMessages;
		bool m_bArrayMessages;
		bool m_bAutoMessages;
		bool m_bBarMessages;
		bool m_bEnableMessages;
		bool m_bEventMessages;
		bool m_bGagMessages;
		bool m_bHighMessages;
		bool m_bItemMessages;
		bool m_bListMessages;
		bool m_bMacroMessages;
		bool m_bShowInfo;
		bool m_bSubMessages;
		bool m_bTabMessages;
		bool m_bVarMessages;
	};
}
