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
// Messages.h: interface for the CMessages class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGES_H__08CF5721_7A98_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_MESSAGES_H__08CF5721_7A98_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMessages : public CObject  
{
public:
	enum MsgType {
		MM_ACTION_MESSAGE				= 0,
		MM_ALIAS_MESSAGE				= 1,
		MM_CHAT_MESSAGE					= 2,
		MM_EVENT_MESSAGE				= 3,
		MM_MACRO_MESSAGE				= 4,
		MM_VARIABLE_MESSAGE				= 5,
		MM_GENERAL_MESSAGE				= 6,
		MM_TAB_MESSAGE					= 7,
		MM_ARRAY_MESSAGE				= 8,
		MM_MATH_MESSAGE					= 9,
		MM_DLL_MESSAGE					= 10,
		MM_PROCEDURE_MESSAGE			= 11,
		MM_LIST_MESSAGE					= 12,
		MM_GAG_MESSAGE					= 13,
		MM_HIGHLIGHT_MESSAGE			= 14,
		MM_SUB_MESSAGE					= 15,
		MM_ITEM_MESSAGE					= 16,
		MM_BAR_MESSAGE					= 17,
		MM_ENABLE_MESSAGE				= 18,
		MM_AUTO_MESSAGE					= 19

	};

	enum {
		MSG_SOCKET_INCOMING			= WM_USER+100,
		MSG_SOCKET_CLOSE			= WM_USER+101,
		MSG_SOCKET_CONNECT			= WM_USER+102,
		MSG_SOCKET_RECEIVE			= WM_USER+103,
		MSG_SOCKET_SEND				= WM_USER+104,

		MSG_CHATSOCKET_INCOMING		= WM_USER+110,
		MSG_CHATSOCKET_CLOSE		= WM_USER+111,
		MSG_CHATSOCKET_CONNECT		= WM_USER+112,
		MSG_CHATSOCKET_RECEIVE		= WM_USER+113,
		MSG_CHATSOCKET_SEND			= WM_USER+114,

	// Use with SendMessage to print text to the mud view window.
	// Put the address of the NULL terminated string to print in
	// lParam.  wParam should have the color indices.  The color
	// indices are stored the same way as a BYTE as they are in 
	// the text window.
		MSG_MUDVIEW_PRINT			= WM_USER+120,

	// This message is used for printing a client message.  The 
	// message has the format of "# Message."
		TERMVIEW_ENTER				= WM_USER+121,
		PUSH_DEBUG_STACK			= WM_USER+122,
		POP_DEBUG_STACK				= WM_USER+123,
		HANDLE_COMMAND				= WM_USER+124,
		MM_LOAD_SCRIPT				= WM_USER+125,

	//	Messages to notify the parent frame of changed script entites
		MSG_ACTIONS_CHANGED			= WM_USER+1000,

		PRINT_MESSAGE				= WM_USER+2000,

		WM_ICON_NOTIFY				= WM_APP+3000,
	// Leave a nice gap around the messages used after this one.
	// To be safe, I'd start the next message grouping at +1000.
	// The way I've set up the macros is the command message assigned
	// to the accelerator table is the MSG_MACRO + (macro list index).
	// This allows me to retrieve the specific macro pressed.
	// Using MSG_MACRO_END as a marker of the end of the range.
		MSG_MACRO					= WM_USER+200,
		MSG_MACRO_END				= WM_USER+999
	};

	CMessages();
	virtual ~CMessages();

	CString m_strText;
	MsgType m_Type;
};

#endif // !defined(AFX_MESSAGES_H__08CF5721_7A98_11D3_BCD6_00E029482496__INCLUDED_)
