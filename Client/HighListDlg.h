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
	class CHighList;
}

class COptions;

namespace EditorDialogs
{
	class CHighListDlg : public ETSLayoutDialog
	{
		// Construction
		COptions &_options;
	public:
		MMScript::CHighList *m_pHighList;

		CHighListDlg(COptions &options,CWnd* pParent= NULL );   // standard constructor
		


		static int CALLBACK HighCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		enum
		{
			HIGHSORT_KEY,
			HIGHSORT_GROUP
		};

		// Dialog Data
		//{{AFX_DATA(CHighListDlg)
		enum { IDD = IDD_HIGH_LISTDLG };
		CMMListCtrl	m_lstHighList;
		CButton m_btnEdit;
		CButton m_btnDelete;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CHighListDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CHighListDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnHighAdd();
		afx_msg void OnHighDelete();
		afx_msg void OnHighEdit();
		afx_msg void OnColumnclickHighList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkHighList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnItemchangedHighList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		void FlipSortDirection();
		int GetSelectedItem();
		BOOL FillHighs();
		int m_nSortDirection;
		int m_nSortType;
	public:
		void SetupLayout(void);
	};
}