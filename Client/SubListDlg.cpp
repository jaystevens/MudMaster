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
#include "SubListDlg.h"
#include "Sub.h"
#include "Globals.h"
#include "SubList.h"
#include "SubEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

namespace EditorDialogs
{
	CSubListDlg::CSubListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CSubListDlg::IDD, pParent)
	{
		m_nSortType = SUBSORT_MASK;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
		//{{AFX_DATA_INIT(CSubListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CSubListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CSubListDlg)
		DDX_Control(pDX, IDC_SUB_LIST, m_lstSubList);
		DDX_Control(pDX, IDC_SUB_EDIT, m_btnEdit);
		DDX_Control(pDX, IDC_SUB_DELETE, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CSubListDlg, CDialog)
		//{{AFX_MSG_MAP(CSubListDlg)
		ON_BN_CLICKED(IDC_SUB_ADD, OnSubAdd)
		ON_BN_CLICKED(IDC_SUB_DELETE, OnSubDelete)
		ON_BN_CLICKED(IDC_SUB_EDIT, OnSubEdit)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_SUB_LIST, OnColumnclickSubList)
		ON_NOTIFY(NM_DBLCLK, IDC_SUB_LIST, OnDblclkSubList)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_SUB_LIST, OnItemchangedSubList)
		ON_WM_SIZE()
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SUB_LIST, OnItemActivate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CSubListDlg message handlers
	int CALLBACK CSubListDlg::SubCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CSub *ptr1 = (CSub *)lParam1;
		CSub *ptr2 = (CSub *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		CString strKey1;
		CString strKey2;

		switch(nSortType)
		{
		case SUBSORT_MASK :
			strKey1 = ptr1->GetTrigger();
			strKey2 = ptr2->GetTrigger();
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strKey1.Compare(strKey2));
			else
				return(strKey2.Compare(strKey1));

		case SUBSORT_SUB :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Sub(),ptr2->Sub()));
			else
				return(strcmp(ptr2->Sub(),ptr1->Sub()));

		case SUBSORT_GROUP :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
		}

		// If it doesn't hit anything in the case statement return
		// that they are equal.
		return(0);
	}

	int CSubListDlg::GetSelectedItem()
	{
		return(m_lstSubList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CSubListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	BOOL CSubListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstSubList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);


		SetupLayout();

		CRect rect;
		m_lstSubList.GetClientRect(&rect);
		m_lstSubList.InsertColumn(0, "Mask", LVCFMT_LEFT, 150);
		m_lstSubList.InsertColumn(1, "Sub", LVCFMT_LEFT, rect.Width()-220);
		m_lstSubList.InsertColumn(2, "Group", LVCFMT_LEFT, 70);

		m_nSortType = SUBSORT_MASK;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillSubs())
			return FALSE;
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CSubListDlg::FillSubs()
	{
		m_lstSubList.DeleteAllItems();
		m_lstSubList.SetItemCount(m_pSubList->GetCount());

		CSub *pSub = (CSub *)m_pSubList->GetFirst();
		int nIndex = 0;
		while(pSub != NULL)
		{
			CString strKey;
			strKey = pSub->GetTrigger();
			int nItem = m_lstSubList.InsertItem(nIndex, strKey);
			m_lstSubList.SetItemData(nItem, reinterpret_cast<DWORD>(pSub));
			m_lstSubList.SetItemText(nItem, 1, pSub->Sub());
			m_lstSubList.SetItemText(nItem, 2, pSub->Group());
			m_lstSubList.SetCheck(nItem, pSub->Enabled());
			nIndex++;
			pSub = (CSub *)m_pSubList->GetNext();
		}

		m_lstSubList.SortItems(SubCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	void CSubListDlg::OnSubAdd() 
	{
		CSubEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		CSub *pSub = (CSub *)m_pSubList->Add(dlg.m_strMask, dlg.m_strSub, dlg.m_strGroup);
		if(pSub == NULL)
		{
			AfxMessageBox("Sub failed to add.  Please retry.");
			return;
		}

		int nCount = m_lstSubList.GetItemCount();
		m_lstSubList.SetItemCount(nCount+1);
		int nItem = m_lstSubList.InsertItem(m_pSubList->GetFindIndex(), dlg.m_strMask);
		m_lstSubList.SetItemData(nItem, reinterpret_cast<DWORD>(pSub));
		m_lstSubList.SetItemText(nItem, 1, pSub->Sub());
		m_lstSubList.SetItemText(nItem, 2, pSub->Group());
	}

	void CSubListDlg::OnSubDelete() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CSub *ptr = reinterpret_cast<CSub *>(m_lstSubList.GetItemData(nIndex));
		m_pSubList->Remove(ptr);
		m_lstSubList.DeleteItem(nIndex);	
	}

	void CSubListDlg::OnSubEdit() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		CSub *ptr = reinterpret_cast<CSub *>(m_lstSubList.GetItemData(nIndex));

		CSubEditorDlg dlg;
		dlg.m_pSub = ptr;
		if(dlg.DoModal() != IDOK)
			return;

		m_pSubList->Remove(ptr);

		ptr = (CSub *)m_pSubList->Add(dlg.m_strMask, dlg.m_strSub, dlg.m_strGroup);
		if(ptr == NULL)
		{
			AfxMessageBox("Sub failed to add.  Please retry");
			return;
		}

		if(dlg.m_bDisabled)
			ptr->Enabled(FALSE);

		CString strKey;
		strKey = ptr->GetTrigger();
		m_lstSubList.SetItemText(nIndex, 0, strKey);
		m_lstSubList.SetItemText(nIndex, 1, ptr->Sub());
		m_lstSubList.SetItemText(nIndex, 2, ptr->Group());	
		m_lstSubList.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(ptr));
	}

	void CSubListDlg::OnColumnclickSubList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == SUBSORT_MASK)
				FlipSortDirection();
			else
			{
				m_nSortType = SUBSORT_MASK;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 1 :
			if (m_nSortType == SUBSORT_SUB)
				FlipSortDirection();
			else
			{
				m_nSortType = SUBSORT_SUB;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 2 :
			if (m_nSortType == SUBSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = SUBSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
		m_lstSubList.SortItems(SubCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CSubListDlg::OnDblclkSubList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnSubEdit();	
		*pResult = 0;
	}

	void CSubListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		if(m_lstSubList.GetSafeHwnd() == NULL)
			return;

		UpdateLayout();

		CRect rect;
		m_lstSubList.GetClientRect(&rect);
		m_lstSubList.SetColumnWidth(0, 150);	
		m_lstSubList.SetColumnWidth(1, rect.Width()-220);	
		m_lstSubList.SetColumnWidth(2, 70);	
	}

	void CSubListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_SUB_ADD, NORESIZE)
			<< item(IDC_SUB_EDIT, NORESIZE)
			<< item(IDC_SUB_DELETE, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_SUB_LIST)
			<< rightPane;

		UpdateLayout();
	}
	void CSubListDlg::OnItemchangedSubList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		// TODO: Add your control notification handler code here

		*pResult = 0;

		if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
			return;	// No change

		BOOL bPrevState = (BOOL)(((pNMListView->uOldState & LVIS_STATEIMAGEMASK)>>12)-1);   // Old check box state
		if (bPrevState < 0)	// On startup there's no previous state 
			bPrevState = 0; // so assign as false (unchecked)

		// New check box state
		BOOL bChecked=(BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12)-1);   
		if (bChecked < 0) // On non-checkbox notifications assume false
			bChecked = 0; 

		if (bPrevState == bChecked) // No change in check box
			return;

		CSub *pSub = (CSub *)m_lstSubList.GetItemData(pNMListView->iItem);
		pSub->Enabled(bChecked);
	}

	void CSubListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstSubList.GetSelectedCount();
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