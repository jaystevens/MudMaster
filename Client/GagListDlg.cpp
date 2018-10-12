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
#include "GagListDlg.h"
#include "Gag.h"
#include "Globals.h"
#include "GagList.h"
#include "GagEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

namespace EditorDialogs
{
	CGagListDlg::CGagListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CGagListDlg::IDD, pParent)
	{
		m_nSortType = GAGSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
		//{{AFX_DATA_INIT(CGagListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CGagListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CGagListDlg)
		DDX_Control(pDX, IDC_GAG_LIST, m_lstGagList);
		DDX_Control(pDX, IDC_GAG_EDIT, m_btnEdit);
		DDX_Control(pDX, IDC_GAG_DELETE, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CGagListDlg, CDialog)
		//{{AFX_MSG_MAP(CGagListDlg)
		ON_BN_CLICKED(IDC_GAG_ADD, OnGagAdd)
		ON_BN_CLICKED(IDC_GAG_DELETE, OnGagDelete)
		ON_BN_CLICKED(IDC_GAG_EDIT, OnGagEdit)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_GAG_LIST, OnColumnclickGagList)
		ON_NOTIFY(NM_DBLCLK, IDC_GAG_LIST, OnDblclkGagList)
		ON_WM_SIZE()
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_GAG_LIST, OnItemchangedGagList)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_GAG_LIST, OnItemActivate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CGagListDlg message handlers
	int CALLBACK CGagListDlg::GagCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CGag *ptr1 = (CGag *)lParam1;
		CGag *ptr2 = (CGag *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		CString strKey1;
		CString strKey2;

		switch(nSortType)
		{
		case GAGSORT_KEY :
			strKey1 = ptr1->GetTrigger();
			strKey2 = ptr2->GetTrigger();
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strKey1.Compare(strKey2));
			else
				return(strKey2.Compare(strKey1));

		//case GAGSORT_ACTION :
		//	if (nSortDir == CGlobals::LISTSORT_ASCEND)
		//		return(strcmp(ptr1->Action(),ptr2->Action()));
		//	else
		//		return(strcmp(ptr2->Action(),ptr1->Action()));

		case GAGSORT_GROUP :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
		}

		// If it doesn't hit anything in the case statement return
		// that they are equal.
		return(0);
	}

	int CGagListDlg::GetSelectedItem()
	{
		return(m_lstGagList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CGagListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	BOOL CGagListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstGagList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);


		SetupLayout();

		CRect rect;
		m_lstGagList.GetClientRect(&rect);
		m_lstGagList.InsertColumn(0, "Mask", LVCFMT_LEFT, 250);
		m_lstGagList.InsertColumn(1, "Group", LVCFMT_LEFT, rect.Width()-250);
		
		m_nSortType = GAGSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillGags())
			return FALSE;
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CGagListDlg::FillGags()
	{
		m_lstGagList.DeleteAllItems();
		m_lstGagList.SetItemCount(m_pGagList->GetCount());

		CGag *pGag = (CGag *)m_pGagList->GetFirst();
		int nIndex = 0;
		while(pGag != NULL)
		{
			CString strKey;
			strKey = pGag->GetTrigger();
			int nItem = m_lstGagList.InsertItem(nIndex, strKey);
			m_lstGagList.SetItemData(nItem, reinterpret_cast<DWORD>(pGag));
			m_lstGagList.SetItemText(nItem, 1, pGag->Group());
			m_lstGagList.SetCheck(nItem, pGag->Enabled());
			nIndex++;
			pGag = (CGag *)m_pGagList->GetNext();
		}

		m_lstGagList.SortItems(GagCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	void CGagListDlg::OnGagAdd() 
	{
		CGagEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		CGag *pGag = (CGag *)m_pGagList->Add(dlg.m_strMask, "", dlg.m_strGroup);
		if(pGag == NULL)
		{
			AfxMessageBox("Gag failed to add.  Please retry.");
			return;
		}

		int nCount = m_lstGagList.GetItemCount();
		m_lstGagList.SetItemCount(nCount+1);
		int nItem = m_lstGagList.InsertItem(m_pGagList->GetFindIndex(), dlg.m_strMask);
		m_lstGagList.SetItemData(nItem, reinterpret_cast<DWORD>(pGag));
		m_lstGagList.SetItemText(nItem, 1, pGag->Group());
	}

	void CGagListDlg::OnGagDelete() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CGag *ptr = reinterpret_cast<CGag *>(m_lstGagList.GetItemData(nIndex));
		m_pGagList->Remove(ptr);
		m_lstGagList.DeleteItem(nIndex);	
	}

	void CGagListDlg::OnGagEdit() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		CGag *pGag = reinterpret_cast<CGag *>(m_lstGagList.GetItemData(nIndex));
		CGagEditorDlg dlg;
		dlg.m_pGag = pGag;
		if(dlg.DoModal() != IDOK)
			return;

		m_pGagList->Remove(pGag);

		pGag = (CGag *)m_pGagList->Add(dlg.m_strMask, dlg.m_strMask, dlg.m_strGroup);
		if(pGag == NULL)
		{
			AfxMessageBox("Gag failed to add.  Please retry");
			return;
		}

		if(dlg.m_bDisabled)
			pGag->Enabled(FALSE);

		CString strKey;
		strKey = pGag->GetTrigger();
		m_lstGagList.SetItemText(nIndex, 0, strKey);
		m_lstGagList.SetItemText(nIndex, 1, pGag->Group());	
		m_lstGagList.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(pGag));
	}

	void CGagListDlg::OnColumnclickGagList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == GAGSORT_KEY)
				FlipSortDirection();
			else
			{
				m_nSortType = GAGSORT_KEY;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		//case 1 :
		//	if (m_nSortType == GagSORT_ACTION)
		//		FlipSortDirection();
		//	else
		//	{
		//		m_nSortType = GagSORT_ACTION;
		//		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
		//	}
		//	break;

		case 1 :
			if (m_nSortType == GAGSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = GAGSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
		m_lstGagList.SortItems(GagCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CGagListDlg::OnDblclkGagList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnGagEdit();	
		*pResult = 0;
	}

	void CGagListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		if(m_lstGagList.GetSafeHwnd() == NULL)
			return;

		UpdateLayout();

		CRect rect;
		m_lstGagList.GetClientRect(&rect);
		m_lstGagList.SetColumnWidth(0, 100);	
		m_lstGagList.SetColumnWidth(1, rect.Width()-170);	
		m_lstGagList.SetColumnWidth(2, 70);	
	}

	void CGagListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_GAG_ADD, NORESIZE)
			<< item(IDC_GAG_EDIT, NORESIZE)
			<< item(IDC_GAG_DELETE, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_GAG_LIST)
			<< rightPane;

		UpdateLayout();
	}
	void CGagListDlg::OnItemchangedGagList(NMHDR* pNMHDR, LRESULT* pResult) 
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

		CGag *pGag = (CGag *)m_lstGagList.GetItemData(pNMListView->iItem);
		pGag->Enabled(bChecked);
	}


	void CGagListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstGagList.GetSelectedCount();
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