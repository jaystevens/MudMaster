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
#include "AliasList.h"

namespace EditorDialogs
{
	class CAliasEditorDlg;
	class CAliasListDlg : public ETSLayoutDialog
	{
		// Construction
	public:
		MMScript::CAliasList * m_pAliasList;
		CAliasListDlg(CWnd* pParent = NULL);   // standard constructor
		static int CALLBACK AliasCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

		enum
		{
			ALIASSORT_ALIAS,
			ALIASSORT_ACTION,
			ALIASSORT_GROUP
		};

		enum { IDD = IDD_ALIAS_LISTDLG };
		CListCtrl	m_lstAliasList;
		CButton		m_btnEdit;
		CButton		m_btnDelete;


	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected:

		virtual BOOL OnInitDialog();
		afx_msg void OnBtnAddAction();
		afx_msg void OnBtnDeleteAction();
		afx_msg void OnBtnEditAction();
		afx_msg void OnDblclkAliaslist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnColumnclickAliaslist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnItemchangedAliaslist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemActivate(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()
	private:
		void AddItem(
			const MMScript::CAlias::PtrType &alias,
			int nIndex);

		void AddAliasFromDialog( CAliasEditorDlg &dlg, int nIndex );

		void FlipSortDirection();
		int GetSelectedItem();
		BOOL FillAliases();
		int m_nSortDirection;
		int m_nSortType;
	public:
		void SetupLayout(void);
	};
}