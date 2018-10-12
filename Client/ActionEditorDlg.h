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

/** 
  * \file
  *	\brief Contains the interface for the CActionEditorDlg class
  */

#include "Action.h"
#include "ETSLayout.h"

/** 
  * \brief Contains all of the Entity Editor Dialogs
  */
namespace EditorDialogs
{
	/**
	  * Provides UI to edit a single action.
	  */
	class CActionEditorDlg : public ETSLayoutDialog
	{
	public:
		/**
		  * Creates an instance of the dialog
		  * \param pParent a pointer to the window which will be the parent of this dialog
		  */
		CActionEditorDlg(CWnd* pParent = NULL);

		/**
		  * nameless enumeration specifying the constant for the dialog resource ID
		  */
		enum { IDD = IDD_ACTION_EDIT };

		/**
		  * Sets the Action entity this dialog is to edit
		  * \param action the action to set on this dialog
		  */
		void SetAction( MMScript::CAction::PtrType &action );

		void CActionEditorDlg::SetEnabled( bool enabled );
		void CActionEditorDlg::SetAction( std::string  strAction );
		void CActionEditorDlg::SetTrigger(std::string  strTrigger );
		void CActionEditorDlg::SetGroup(std::string  strGroup );

		/**
		  * Gets the "action to take" from the currently edited action
		  * \return a string containing the Action entity's action
		  */
		const std::string GetAction() const;

		/**
		  * Gets the "trigger to match" from the currently edited action
		  * \return a string containing the Action entity's trigger
		  */
		const std::string GetTrigger() const;

		/**
		  * Gets the group to which the currently edited action belongs
		  * \return a string containing the name fo the group to which this action belongs
		  */
		const std::string GetGroup() const;

		/**
		  * Gets the enabled state of the currently edited action
		  * \return true if the Action is currently set to enabled
		  */
		bool IsEnabled() const;

	private:
		MMScript::CAction::PtrType _action; /**< Holds the action currently being edited */
		CString	_strAction;					/**< holds the action text */
		BOOL _bDisabled;					/**< holds the enabled state of the action */
		CString	_strGroup;					/**< holds the group for the action */
		CString	_strTrigger;				/**< holds the trigger for the action */

		/**
		  * Sets up the dialogs layout for the ETSLayout baseclass
		  */
		void SetupLayout(void);

		/**
		  * Performs data exchange for MFC dialog dataexchange
		  * \param pDX pointer to a dataexchange object
		  */
		virtual void DoDataExchange(CDataExchange* pDX);

		/**
		  * sets up the dialog before displaying to the user
		  */
		virtual BOOL OnInitDialog();

		/**
		  * processes thd dialog when the user clicks the OK button
		  */
		virtual void OnOK();

		/**
		  * processes the WM_SIZE message for this dialog
		  * \param nType the type of sizing that is occuring
		  * \param cx the width
		  * \param cy the height
		  */
		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	};
}