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

#include "ETSLayout.h"
#include "MMListCtrl.h"
#include "ActionList.h"

#pragma warning (disable: 4267)

namespace EditorDialogs
{
	class CActionEditorDlg;

	class CActionOnlyListDlg : public ETSLayoutDialog
	{
	public:
		MMScript::CActionList * m_pActionList;
		CActionOnlyListDlg(CWnd* pParent = NULL);

		enum
		{
			ACTSORT_TRIGGER,
			ACTSORT_ACTION,
			ACTSORT_GROUP
		};

		static int CALLBACK ActionCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		
		enum { IDD = IDD_ACTION_LISTDLG };
		CButton	m_btnAdd;
		CButton	m_btnDelete;
		CButton	m_btnEdit;
		CButton	m_btnOk;
		CMMListCtrl	m_lstActionList;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);

		afx_msg void OnBtnEditAction();
		afx_msg void OnBtnDeleteAction();
		afx_msg void OnBtnAddAction();
		virtual BOOL OnInitDialog();
		afx_msg void OnDestroy();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnColumnclickActionlist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkActionlist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemchangedActionlist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	private:
		int GetSelectedItem();
		void FlipSortDirection();
		int m_nSortDirection;
		int m_nSortType;
		BOOL FillActions();
		void AddActionFromDialog( CActionEditorDlg &dlg, int nIndex );
		void AddItem( const MMScript::CAction::PtrType &action, int nIndex );
	public:
		void SetupLayout(void);
	};
}