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
	class CEventList;
}

namespace EditorDialogs
{
	class CEventListDlg : public ETSLayoutDialog
	{
		// Construction
	public:
		static int CALLBACK EventCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
		MMScript::CEventList * m_pEventList;
		CEventListDlg(CWnd* pParent = NULL);   // standard constructor

		enum
		{
			EVENTSORT_NAME,
			EVENTSORT_SECONDS,
			EVENTSORT_EVENT,
			EVENTSORT_GROUP
		};
		// Dialog Data
		//{{AFX_DATA(CEventListDlg)
		enum { IDD = IDD_EVENT_LISTDLG };
		CMMListCtrl	m_lstEventList;
		CButton m_btnEdit;
		CButton m_btnDelete;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEventListDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CEventListDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnEventAdd();
		afx_msg void OnEventDelete();
		afx_msg void OnEventEdit();
		afx_msg void OnDestroy();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnDblclkEventList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnColumnclickEventList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemchangedEventList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	private:
		int GetSelectedItem();
		void FlipSortDirection();
		BOOL FillEvents();
		int m_nSortDirection;
		int m_nSortType;
	public:
		// Configures the layout manager for the resizable dialog
		void SetupLayout(void);
	};
}