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
#include "HighListDlg.h"
#include "High.h"
#include "Globals.h"
#include "HighList.h"
#include "HighEditorDlg.h"
#include "Sess.h"
#include "IfxDoc.h"
#include "NewWindowView.h"
#include "Colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MudmasterColors;

namespace EditorDialogs
{
	CHighListDlg::CHighListDlg(COptions &options,CWnd* pParent )/*=NULL*/
		: ETSLayoutDialog(CHighListDlg::IDD, pParent)
		, _options(options)
	{
		m_nSortType = HIGHSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	
		//{{AFX_DATA_INIT(CHighListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CHighListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CHighListDlg)
		DDX_Control(pDX, IDC_HIGH_LIST, m_lstHighList);
		DDX_Control(pDX, IDC_HIGH_EDIT, m_btnEdit);
		DDX_Control(pDX, IDC_HIGH_DELETE, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CHighListDlg, CDialog)
		//{{AFX_MSG_MAP(CHighListDlg)
		ON_BN_CLICKED(IDC_HIGH_ADD, OnHighAdd)
		ON_BN_CLICKED(IDC_HIGH_DELETE, OnHighDelete)
		ON_BN_CLICKED(IDC_HIGH_EDIT, OnHighEdit)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_HIGH_LIST, OnColumnclickHighList)
		ON_NOTIFY(NM_DBLCLK, IDC_HIGH_LIST, OnDblclkHighList)
		ON_WM_SIZE()
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_HIGH_LIST, OnItemchangedHighList)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_HIGH_LIST, OnItemActivate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CHighListDlg message handlers
	int CALLBACK CHighListDlg::HighCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CHigh *ptr1 = (CHigh *)lParam1;
		CHigh *ptr2 = (CHigh *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		CString strKey1;
		CString strKey2;

		switch(nSortType)
		{
		case HIGHSORT_KEY :
			ptr1->ToMask(strKey1);
			ptr2->ToMask(strKey2);
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strKey1.Compare(strKey2));
			else
				return(strKey2.Compare(strKey1));

		case HIGHSORT_GROUP :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
		}

		// If it doesn't hit anything in the case statement return
		// that they are equal.
		return(0);
	}

	int CHighListDlg::GetSelectedItem()
	{
		return(m_lstHighList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CHighListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	BOOL CHighListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstHighList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);


		SetupLayout();

		CRect rect;
		m_lstHighList.GetClientRect(&rect);
		m_lstHighList.InsertColumn(0, "Mask", LVCFMT_LEFT, 150);
		m_lstHighList.InsertColumn(1, "Colour", LVCFMT_LEFT, rect.Width()-220);
		m_lstHighList.InsertColumn(2, "Group", LVCFMT_LEFT, 70);

		m_nSortType = HIGHSORT_KEY;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillHighs())
			return FALSE;
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CHighListDlg::FillHighs()
	{
		m_lstHighList.DeleteAllItems();
		m_lstHighList.SetItemCount(m_pHighList->GetCount());

		CHigh *pHigh = (CHigh *)m_pHighList->GetFirst();
		int nIndex = 0;
		while(pHigh != NULL)
		{
			CString strKey;
			CString strColor;
			pHigh->ColorToString(strColor);
			pHigh->ToMask(strKey);

			int nItem = m_lstHighList.InsertItem(nIndex, strKey);
			m_lstHighList.SetItemData(nItem, reinterpret_cast<DWORD>(pHigh));
			m_lstHighList.SetItemText(nItem, 1, strColor);
			m_lstHighList.SetItemText(nItem, 2, pHigh->Group());
			m_lstHighList.SetCheck(nItem, pHigh->Enabled());
			nIndex++;
			pHigh = (CHigh *)m_pHighList->GetNext();
		}

		m_lstHighList.SortItems(HighCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	void CHighListDlg::OnHighAdd() 
	{
		
		CHighEditorDlg dlg(_options, this);

		if(dlg.DoModal() != IDOK)
			return;
		
		CString strColor;
		strColor.Format("%s, back %s", COLOR_STRINGS[dlg.m_nFore], COLOR_STRINGS[dlg.m_nBack]);
		
		m_pHighList->Add(dlg.m_strMask,strColor,  dlg.m_strGroup);
		
		CHigh * pHigh = (CHigh *)m_pHighList->FindExact(dlg.m_strMask);		
        if(pHigh == NULL)
		{
			AfxMessageBox("High failed to add.  Please retry.");
			return;
		}
		
		int nCount = m_lstHighList.GetItemCount();
		m_lstHighList.SetItemCount(nCount+1);
		int nItem = m_lstHighList.InsertItem(m_pHighList->GetFindIndex(), dlg.m_strMask);
		m_lstHighList.SetItemData(nItem, reinterpret_cast<DWORD>(pHigh));
		m_lstHighList.SetItemText(nItem, 1, strColor);
		m_lstHighList.SetItemText(nItem, 2, pHigh->Group());
	}

	void CHighListDlg::OnHighDelete() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CHigh *ptr = reinterpret_cast<CHigh *>(m_lstHighList.GetItemData(nIndex));
		m_pHighList->Remove(ptr);
		m_lstHighList.DeleteItem(nIndex);	
	}

	void CHighListDlg::OnHighEdit() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;
		//CHigh *ptr = reinterpret_cast<CHigh *>(m_lstHighList.GetItemData(nIndex));
		CHigh *ptr = (CHigh *)(m_lstHighList.GetItemData(nIndex));


		CHighEditorDlg dlg(_options, this);

		dlg.m_pHigh = ptr;
	

		if(dlg.DoModal() != IDOK)
			return;

		m_pHighList->Remove(ptr);

		CString strColor;
		strColor.Format("%s, back %s", COLOR_STRINGS[dlg.m_nFore], COLOR_STRINGS[dlg.m_nBack]);

		ptr = (CHigh *)m_pHighList->Add(dlg.m_strMask, strColor, dlg.m_strGroup);
		if(ptr == NULL)
		{
			AfxMessageBox("High failed to add.  Please retry");
			return;
		}

		if(dlg.m_bDisabled)
			ptr->Enabled(FALSE);

		CString strKey;
		
		ptr->ToMask(strKey);
		

		m_lstHighList.SetItemText(nIndex, 0, strKey);
		m_lstHighList.SetItemText(nIndex, 1, strColor);
		m_lstHighList.SetItemText(nIndex, 2, ptr->Group());	
		m_lstHighList.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(ptr));
	}

	void CHighListDlg::OnColumnclickHighList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == HIGHSORT_KEY)
				FlipSortDirection();
			else
			{
				m_nSortType = HIGHSORT_KEY;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

//		case 1 :
//			if (m_nSortType == HIGHSORT_ACTION)
//				FlipSortDirection();
//			else
//			{
//				m_nSortType = MACROSORT_ACTION;
//				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
//			}
//			break;

		case 1 :
			if (m_nSortType == HIGHSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = HIGHSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
		m_lstHighList.SortItems(HighCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CHighListDlg::OnDblclkHighList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnHighEdit();	
		*pResult = 0;
	}

	void CHighListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		if(m_lstHighList.GetSafeHwnd() == NULL)
			return;

		UpdateLayout();

		CRect rect;
		m_lstHighList.GetClientRect(&rect);
		m_lstHighList.SetColumnWidth(0, 150);	
		m_lstHighList.SetColumnWidth(1, rect.Width()-220);	
		m_lstHighList.SetColumnWidth(2, 70);	
	}

	void CHighListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_HIGH_ADD, NORESIZE)
			<< item(IDC_HIGH_EDIT, NORESIZE)
			<< item(IDC_HIGH_DELETE, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_HIGH_LIST)
			<< rightPane;

		UpdateLayout();
	}
	void CHighListDlg::OnItemchangedHighList(NMHDR* pNMHDR, LRESULT* pResult) 
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

		CHigh *pHigh = (CHigh *)m_lstHighList.GetItemData(pNMListView->iItem);
		pHigh->Enabled(bChecked);
	}

	void CHighListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstHighList.GetSelectedCount();
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