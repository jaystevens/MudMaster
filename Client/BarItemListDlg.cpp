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
#include "BarItemListDlg.h"
#include "Globals.h"
#include "StatusBar.h"
#include "BarItem.h"
#include "BarItemEditorDlg.h"
#include "SeException.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

namespace EditorDialogs
{
	CBarItemListDlg::CBarItemListDlg(COptions &options, CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CBarItemListDlg::IDD, pParent)
		, _options(options)
	{
		//{{AFX_DATA_INIT(CBarItemListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
		m_pBarItemList=NULL;
	}


	void CBarItemListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CBarItemListDlg)
		DDX_Control(pDX, IDC_BARITEM_LIST, m_lstBarItems);
		DDX_Control(pDX, IDC_BTN_EDIT_BARITEM, m_btnEdit);
		DDX_Control(pDX, IDC_BTN_DELETE_BARITEM, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CBarItemListDlg, CDialog)
		//{{AFX_MSG_MAP(CBarItemListDlg)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_BARITEM_LIST, OnColumnclickAliaslist)
		ON_NOTIFY(NM_DBLCLK, IDC_BARITEM_LIST, OnDblclkAliaslist)
		ON_WM_SIZE()
		ON_BN_CLICKED(IDC_BTN_ADD_BARITEM, OnBtnAddBaritem)
		ON_BN_CLICKED(IDC_BTN_DELETE_BARITEM, OnBtnDeleteBaritem)
		ON_BN_CLICKED(IDC_BTN_EDIT_BARITEM, OnBtnEditBaritem)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_BARITEM_LIST, OnItemActivate)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_BARITEM_LIST, OnItemchangedBaritemList)

		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CBarItemListDlg message handlers

	BOOL CBarItemListDlg::OnInitDialog() 
	{
		ASSERT(m_pBarItemList != NULL);

		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstBarItems.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);

		SetupLayout();

		CRect rect;
		m_lstBarItems.GetClientRect(&rect);
		m_lstBarItems.InsertColumn(COLUMN_POSITION,		"Position", LVCFMT_LEFT, 50);
		m_lstBarItems.InsertColumn(COLUMN_LENGTH,		"Length",	LVCFMT_LEFT, 50);
		m_lstBarItems.InsertColumn(COLUMN_SEPARATOR,	"Separator",LVCFMT_LEFT, 50);
		m_lstBarItems.InsertColumn(COLUMN_ITEM,			"Item",		LVCFMT_LEFT, 70);
		m_lstBarItems.InsertColumn(COLUMN_TEXT,			"Text",		LVCFMT_LEFT, 70);
		m_lstBarItems.InsertColumn(COLUMN_GROUP,		"Group",	LVCFMT_LEFT, 70);

		m_nSortType = BARITEMSORT_POSITION;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillBarItems())
			return FALSE;

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CBarItemListDlg::FillBarItems()
	{
		m_lstBarItems.DeleteAllItems();
		m_lstBarItems.SetItemCount(m_pBarItemList->GetCount());

		CBarItem *pBarItem = (CBarItem *)m_pBarItemList->GetFirst();
		int nIndex = 0;
		while(pBarItem != NULL)
		{
			CString strPosition;
			strPosition.Format("%d", pBarItem->Position());
			int nItem = m_lstBarItems.InsertItem(nIndex, strPosition);
			m_lstBarItems.SetItemData(nItem, reinterpret_cast<DWORD>(pBarItem));
			m_lstBarItems.SetCheck(nItem, pBarItem->Enabled());

			if(pBarItem->Separator())
			{
				m_lstBarItems.SetItemText(nItem, COLUMN_ITEM, pBarItem->Item());
				m_lstBarItems.SetItemText(nItem, COLUMN_SEPARATOR, "YES");
				m_lstBarItems.SetItemText(nItem, COLUMN_GROUP, pBarItem->Group());
			}
			else
			{
				int nLength = pBarItem->Length();
				CString strLength;
				strLength.Format("%d", nLength);
				m_lstBarItems.SetItemText(nItem, COLUMN_LENGTH, strLength);
				m_lstBarItems.SetItemText(nItem, COLUMN_ITEM, pBarItem->Item());
				m_lstBarItems.SetItemText(nItem, COLUMN_TEXT, pBarItem->Text());
				m_lstBarItems.SetItemText(nItem, COLUMN_GROUP, pBarItem->Group());
			}
			nIndex++;
			pBarItem = (CBarItem *)m_pBarItemList->GetNext();
		}

		m_lstBarItems.SortItems(AliasCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	int CALLBACK CBarItemListDlg::AliasCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CBarItem *ptr1 = (CBarItem *)lParam1;
		CBarItem *ptr2 = (CBarItem *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		switch(nSortType)
		{
		case BARITEMSORT_POSITION :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
			{
				if(ptr1->Position() > ptr2->Position())
					return 1;
				if(ptr1->Position() < ptr2->Position())
					return -1;
				return 0;
			}
			else
			{
				if(ptr2->Position() > ptr1->Position())
					return 1;
				if(ptr2->Position() < ptr1->Position())
					return -1;
				return 0;
			}
			break;
		case BARITEMSORT_LENGTH :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
			{
				if(ptr1->Length() > ptr2->Length())
					return 1;
				if(ptr1->Length() < ptr2->Length())
					return -1;
				return 0;
			}
			else
			{
				if(ptr2->Length() > ptr1->Length())
					return -1;
				if(ptr2->Length() < ptr1->Length())
					return 1;
				return 0;
			}
			break;

		case BARITEMSORT_SEPARATOR:
			if(nSortDir == CGlobals::LISTSORT_ASCEND)
			{
				if(ptr1->Enabled() > ptr2->Enabled())
					return 1;
				if(ptr1->Enabled() < ptr2->Enabled())
					return -1;
				return 0;
			}
			else
			{
				if(ptr2->Enabled() > ptr1->Enabled())
					return -1;
				if(ptr2->Enabled() < ptr1->Enabled())
					return 1;
				return 0;
			}
			break;

		case BARITEMSORT_ITEM :
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Item(),ptr2->Item()));
			else
				return(strcmp(ptr2->Item(),ptr1->Item()));

		case BARITEMSORT_GROUP:
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
			break;

		case BARITEMSORT_TEXT:
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Item(),ptr2->Item()));
			else
				return(strcmp(ptr2->Item(),ptr1->Item()));
		}

		// If it doesn't hit anything in the case statement return
		// that they are equal.
		return(0);
	}

	void CBarItemListDlg::OnColumnclickAliaslist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case COLUMN_POSITION :
			if (m_nSortType == BARITEMSORT_POSITION)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_POSITION;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case COLUMN_LENGTH :
			if (m_nSortType == BARITEMSORT_LENGTH)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_LENGTH;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case COLUMN_SEPARATOR :
			if (m_nSortType == BARITEMSORT_SEPARATOR)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_SEPARATOR;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case COLUMN_ITEM:
			if (m_nSortType == BARITEMSORT_ITEM)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_ITEM;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case COLUMN_TEXT:
			if (m_nSortType == BARITEMSORT_TEXT)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_TEXT;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case COLUMN_GROUP:
			if (m_nSortType == BARITEMSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = BARITEMSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
		m_lstBarItems.SortItems(AliasCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CBarItemListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	void CBarItemListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);
		UpdateLayout();
	}

	void CBarItemListDlg::OnDblclkAliaslist(NMHDR* /*pNMHDR*/, LRESULT* pResult)
	{
		OnBtnEditBaritem();	
		*pResult = 0;
	}

	void CBarItemListDlg::OnBtnAddBaritem() 
	{
		CBarItemEditorDlg dlg(_options, this);

		if(dlg.DoModal() != IDOK)
			return;

		if(!dlg.m_bSeparator)
		{
			m_pBarItemList->AddBarItem(
				dlg.m_strName,
				dlg.m_strText,
				dlg.m_nPosition,
				dlg.m_nLength,
				dlg.m_nForeColor,
				dlg.m_nBackColor,
				dlg.m_strGroup);
		}
		else
		{
			m_pBarItemList->AddSeparator(
				dlg.m_strText,
				dlg.m_nPosition,
				dlg.m_strGroup);
		}

		CBarItem *pItem = m_pBarItemList->FindItem(dlg.m_strName);
		int nCount = m_lstBarItems.GetItemCount();
		m_lstBarItems.SetItemCount(nCount+1);

		CString strPosition;
		strPosition.Format("%d", dlg.m_nPosition);
		int nItem = m_lstBarItems.InsertItem(m_pBarItemList->GetFindIndex(), strPosition);
		m_lstBarItems.SetItemData(nItem, reinterpret_cast<DWORD>(pItem));
		m_lstBarItems.SetCheck(nItem, pItem->Enabled());

		if(pItem->Separator())
		{
			m_lstBarItems.SetItemText(nItem, COLUMN_ITEM, pItem->Item());
			m_lstBarItems.SetItemText(nItem, COLUMN_SEPARATOR, "YES");
			m_lstBarItems.SetItemText(nItem, COLUMN_GROUP, pItem->Group());
		}
		else
		{
			int nLength = pItem->Length();
			CString strLength;
			strLength.Format("%d", nLength);
			m_lstBarItems.SetItemText(nItem, COLUMN_LENGTH, strLength);
			m_lstBarItems.SetItemText(nItem, COLUMN_ITEM, pItem->Item());
			m_lstBarItems.SetItemText(nItem, COLUMN_TEXT, pItem->Text());
			m_lstBarItems.SetItemText(nItem, COLUMN_GROUP, pItem->Group());
		}
	}

	void CBarItemListDlg::OnBtnDeleteBaritem() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CBarItem *ptr = reinterpret_cast<CBarItem *>(m_lstBarItems.GetItemData(nIndex));
		m_pBarItemList->Remove(ptr);
		m_lstBarItems.DeleteItem(nIndex);		
	}

	void CBarItemListDlg::OnBtnEditBaritem() 
	{
		int nIndex = 0;

		nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		CBarItem *ptr = NULL;
		CBarItemEditorDlg dlg(_options, this);

		ptr = reinterpret_cast<CBarItem *>(m_lstBarItems.GetItemData(nIndex));

		dlg.m_pItem = ptr;

		if(dlg.DoModal() != IDOK)
			return;

		m_pBarItemList->Remove(ptr);

		if(!dlg.m_bSeparator)
		{
			m_pBarItemList->AddBarItem(
				dlg.m_strName,
				dlg.m_strText,
				dlg.m_nPosition,
				dlg.m_nLength,
				dlg.m_nForeColor,
				dlg.m_nBackColor,
				dlg.m_strGroup);
		}
		else
		{
			m_pBarItemList->AddSeparator(
				dlg.m_strText,
				dlg.m_nPosition,
				dlg.m_strGroup);
		}

		ptr=(CBarItem *)m_pBarItemList->FindItem(dlg.m_strName);
		if(ptr == NULL)
		{
			ASSERT(FALSE);
			AfxMessageBox("ERROR: Cannot find bar item in the bar item list!");
			return;
		}
		if(dlg.m_bDisabled)
			ptr->Enabled(FALSE);

		CString strPosition;
		strPosition.Format("%d", dlg.m_nPosition);
		m_lstBarItems.SetItemText(nIndex, COLUMN_POSITION, strPosition);
		m_lstBarItems.SetItemData(nIndex, reinterpret_cast<DWORD>(ptr));
		m_lstBarItems.SetCheck(nIndex, ptr->Enabled());

		if(ptr->Separator())
		{
			m_lstBarItems.SetItemText(nIndex, COLUMN_ITEM, ptr->Item());
			m_lstBarItems.SetItemText(nIndex, COLUMN_SEPARATOR, "YES");
			m_lstBarItems.SetItemText(nIndex, COLUMN_GROUP, ptr->Group());
		}
		else
		{
			int nLength = ptr->Length();
			CString strLength;
			strLength.Format("%d", nLength);
			m_lstBarItems.SetItemText(nIndex, COLUMN_LENGTH, strLength);
			m_lstBarItems.SetItemText(nIndex, COLUMN_ITEM, ptr->Item());
			m_lstBarItems.SetItemText(nIndex, COLUMN_TEXT, ptr->Text());
			m_lstBarItems.SetItemText(nIndex, COLUMN_GROUP, ptr->Group());
		}
	}

	int CBarItemListDlg::GetSelectedItem()
	{
		return(m_lstBarItems.GetNextItem(-1,LVNI_SELECTED));
	}

	void CBarItemListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_BTN_ADD_BARITEM, NORESIZE)
			<< item(IDC_BTN_EDIT_BARITEM, NORESIZE)
			<< item(IDC_BTN_DELETE_BARITEM, NORESIZE);

		CreateRoot(HORIZONTAL)
			<< item(IDC_BARITEM_LIST)
			<< rightPane;

		UpdateLayout();
	}


	void CBarItemListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstBarItems.GetSelectedCount();
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
	void CBarItemListDlg::OnItemchangedBaritemList(NMHDR* pNMHDR, LRESULT* pResult) 
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
		CBarItem *pBarItem = (CBarItem *)m_lstBarItems.GetItemData(pNMListView->iItem);
		pBarItem->Enabled(bChecked);
	}

}