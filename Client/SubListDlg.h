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

namespace MMScript
{
	class CSubList;
}

namespace EditorDialogs
{
	class CSubListDlg : public ETSLayoutDialog
	{
		// Construction
	public:
		MMScript::CSubList *m_pSubList;

		CSubListDlg(CWnd* pParent = NULL);   // standard constructor

		static int CALLBACK SubCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		enum
		{
			SUBSORT_MASK,
			SUBSORT_SUB,
			SUBSORT_GROUP
		};

		// Dialog Data
		//{{AFX_DATA(CSubListDlg)
		enum { IDD = IDD_SUBS_LISTDLG };
		CMMListCtrl	m_lstSubList;
		CButton m_btnEdit;
		CButton m_btnDelete;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSubListDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CSubListDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnSubAdd();
		afx_msg void OnSubDelete();
		afx_msg void OnSubEdit();
		afx_msg void OnColumnclickSubList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkSubList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnItemchangedSubList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		void FlipSortDirection();
		int GetSelectedItem();
		BOOL FillSubs();
		int m_nSortDirection;
		int m_nSortType;
	public:
		void SetupLayout(void);
	};
}