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
#include "stdafx.h"
#include "ifx.h"
#include "MacroListDlg.h"
#include "Macro.h"
#include "Globals.h"
#include "MacroList.h"
#include "MacroEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

namespace EditorDialogs
{
	CMacroListDlg::CMacroListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CMacroListDlg::IDD, pParent)
	{
		m_nSortType = MACROSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
		//{{AFX_DATA_INIT(CMacroListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CMacroListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CMacroListDlg)
		DDX_Control(pDX, IDC_MACRO_LIST, m_lstMacroList);
		DDX_Control(pDX, IDC_MACRO_EDIT, m_btnEdit);
		DDX_Control(pDX, IDC_MACRO_DELETE, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CMacroListDlg, CDialog)
		//{{AFX_MSG_MAP(CMacroListDlg)
		ON_BN_CLICKED(IDC_MACRO_ADD, OnMacroAdd)
		ON_BN_CLICKED(IDC_MACRO_DELETE, OnMacroDelete)
		ON_BN_CLICKED(IDC_MACRO_EDIT, OnMacroEdit)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_MACRO_LIST, OnColumnclickMacroList)
		ON_NOTIFY(NM_DBLCLK, IDC_MACRO_LIST, OnDblclkMacroList)
		ON_WM_SIZE()
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_MACRO_LIST, OnItemActivate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CMacroListDlg message handlers
	int CALLBACK CMacroListDlg::MacroCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CMacro *ptr1 = (CMacro *)lParam1;
		CMacro *ptr2 = (CMacro *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		CString strKey1;
		CString strKey2;

		switch(nSortType)
		{
		case MACROSORT_KEY :
			ptr1->KeyToName(strKey1);
			ptr2->KeyToName(strKey2);
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strKey1.Compare(strKey2));
			else
				return(strKey2.Compare(strKey1));

		case MACROSORT_ACTION :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Action(),ptr2->Action()));
			else
				return(strcmp(ptr2->Action(),ptr1->Action()));

		case MACROSORT_GROUP :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
		}

		// If it doesn't hit anything in the case statement return
		// that they are equal.
		return(0);
	}

	int CMacroListDlg::GetSelectedItem()
	{
		return(m_lstMacroList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CMacroListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	BOOL CMacroListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstMacroList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);


		SetupLayout();

		CRect rect;
		m_lstMacroList.GetClientRect(&rect);
		m_lstMacroList.InsertColumn(0, "Key", LVCFMT_LEFT, 100);
		m_lstMacroList.InsertColumn(1, "Action", LVCFMT_LEFT, rect.Width()-170);
		m_lstMacroList.InsertColumn(2, "Group", LVCFMT_LEFT, 70);

		m_nSortType = MACROSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillMacros())
			return FALSE;
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CMacroListDlg::FillMacros()
	{
		m_lstMacroList.DeleteAllItems();
		m_lstMacroList.SetItemCount(m_pMacroList->GetCount());

		CMacro *pMacro = (CMacro *)m_pMacroList->GetFirst();
		int nIndex = 0;
		while(pMacro != NULL)
		{
			CString strKey;
			pMacro->KeyToName(strKey);
			int nItem = m_lstMacroList.InsertItem(nIndex, strKey);
			m_lstMacroList.SetItemData(nItem, reinterpret_cast<DWORD>(pMacro));
			m_lstMacroList.SetItemText(nItem, 1, pMacro->Action());
			m_lstMacroList.SetItemText(nItem, 2, pMacro->Group());
			m_lstMacroList.SetCheck(nItem, pMacro->Enabled());
			nIndex++;
			pMacro = (CMacro *)m_pMacroList->GetNext();
		}

		m_lstMacroList.SortItems(MacroCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	void CMacroListDlg::OnMacroAdd() 
	{
		CMacroEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		CMacro *pMacro = (CMacro *)m_pMacroList->Add(dlg.m_strKey, dlg.m_strAction, dlg.m_strGroup, dlg.m_nDest);
		if(pMacro == NULL)
		{
			AfxMessageBox("Macro failed to add.  Please retry.");
			return;
		}

		int nCount = m_lstMacroList.GetItemCount();
		m_lstMacroList.SetItemCount(nCount+1);
		int nItem = m_lstMacroList.InsertItem(m_pMacroList->GetFindIndex(), dlg.m_strKey);
		m_lstMacroList.SetItemData(nItem, reinterpret_cast<DWORD>(pMacro));
		m_lstMacroList.SetItemText(nItem, 1, pMacro->Action());
		m_lstMacroList.SetItemText(nItem, 2, pMacro->Group());
	}

	void CMacroListDlg::OnMacroDelete() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CMacro *ptr = reinterpret_cast<CMacro *>(m_lstMacroList.GetItemData(nIndex));
		m_pMacroList->Remove(ptr);
		m_lstMacroList.DeleteItem(nIndex);	
	}

	void CMacroListDlg::OnMacroEdit() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		CMacro *ptr = reinterpret_cast<CMacro *>(m_lstMacroList.GetItemData(nIndex));

		CMacroEditorDlg dlg;
		dlg.m_pMacro = ptr;
		if(dlg.DoModal() != IDOK)
			return;

		m_pMacroList->Remove(ptr);

		ptr = m_pMacroList->Add(dlg.m_strKey, dlg.m_strAction, dlg.m_strGroup, dlg.m_nDest);
		if(ptr == NULL)
		{
			AfxMessageBox("Macro failed to add.  Please retry");
			return;
		}

		if(dlg.m_bDisabled)
			ptr->Enabled(FALSE);

		CString strKey;
		ptr->KeyToName(strKey);
		m_lstMacroList.SetItemText(nIndex, 0, strKey);
		m_lstMacroList.SetItemText(nIndex, 1, ptr->Action());
		m_lstMacroList.SetItemText(nIndex, 2, ptr->Group());	
		m_lstMacroList.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(ptr));
	}

	void CMacroListDlg::OnColumnclickMacroList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == MACROSORT_KEY)
				FlipSortDirection();
			else
			{
				m_nSortType = MACROSORT_KEY;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 1 :
			if (m_nSortType == MACROSORT_ACTION)
				FlipSortDirection();
			else
			{
				m_nSortType = MACROSORT_ACTION;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 2 :
			if (m_nSortType == MACROSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = MACROSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
		m_lstMacroList.SortItems(MacroCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CMacroListDlg::OnDblclkMacroList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnMacroEdit();	
		*pResult = 0;
	}

	void CMacroListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		if(m_lstMacroList.GetSafeHwnd() == NULL)
			return;

		UpdateLayout();

		CRect rect;
		m_lstMacroList.GetClientRect(&rect);
		m_lstMacroList.SetColumnWidth(0, 100);	
		m_lstMacroList.SetColumnWidth(1, rect.Width()-170);	
		m_lstMacroList.SetColumnWidth(2, 70);	
	}

	void CMacroListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_MACRO_ADD, NORESIZE)
			<< item(IDC_MACRO_EDIT, NORESIZE)
			<< item(IDC_MACRO_DELETE, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_MACRO_LIST)
			<< rightPane;

		UpdateLayout();
	}

	void CMacroListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstMacroList.GetSelectedCount();
		if(0 == nSelectionCount)
		{
			m_btnEdit.EnableWindow(FALSE);
			m_btnDelete.EnableWindow(FALSE);
		}
		else
		{
			m_btnEdit.EnableWindow(TRUE);
			m_btnDelete.EnableWindow(TRUE);
		}
	}
}