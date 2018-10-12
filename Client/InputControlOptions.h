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

#include "richedit.h"

namespace SerializedOptions
{
	class CInputControlOptions
	{
#define INPUT_CONTROL_SECTION	"Input Control"
#define DEFAULT_BACK_COLOR		RGB(192,165,148)
#define DEFAULT_TEXT_COLOR		RGB(0,0,0)
#define DEFAULT_PITCH_FAM		49
#define DEFAULT_FACE_NAME		"Terminal"
#define RETAIN_COMMANDS_KEY		"Retain Commands"
#define BACK_COLOR_KEY			"Back Color"
#define FONT_EFFECTS_KEY		"Font Effects"
#define FONT_OFFSET_KEY			"Font Offset"
#define FONT_TEXTCOLOR_KEY		"Font Color"
#define FONT_CHARSET_KEY		"Font Charset"
#define FONT_PITCH_KEY			"Font Pitch"
#define FONT_FACENAME			"Font Facename"
#define FONT_SIZE_KEY			"Font Size"


	public:
		void SetInputFormat(CHARFORMAT& Font);
		CInputControlOptions();
		CInputControlOptions(const CInputControlOptions &src);
		CInputControlOptions &operator=(const CInputControlOptions &src);

		HRESULT Save(LPCTSTR lpszPathName);
		HRESULT Read(LPCTSTR lpszPathName);

		///////////////////////////////////////////////////////////////////
		// Input Control Settings
		///////////////////////////////////////////////////////////////////
		// Accessors
		CStringList& GetCommandHistoryBuffer() {return m_CommandHistory;}

		BOOL RetainCommands() {return m_bRetainCommands;}
		BOOL ShowBorder() {return m_bShowBorder;}
		UINT CommandHistoryBufferSize() {return m_nBufferSize;}
		CHARFORMAT *GetInputFormat() {return &m_Font;}
		COLORREF GetBackColor() {return m_crBackColor;}

		// Mutators
		void RetainCommands(BOOL bRetain) {m_bRetainCommands = bRetain;}
		void ShowBorder(BOOL bBorder) {m_bShowBorder = bBorder;}
		void CommandHistoryBufferSize(UINT nBufferSize) {m_nBufferSize = nBufferSize;}
		void SetBackColor(COLORREF crBack) {m_crBackColor = crBack;}

	private:
		void Swap(CInputControlOptions &src);

		BOOL m_bRetainCommands;
		BOOL m_bShowBorder;
		int m_nBufferSize;
		CStringList m_CommandHistory;
		CHARFORMAT m_Font;
		COLORREF m_crBackColor;
	};
}