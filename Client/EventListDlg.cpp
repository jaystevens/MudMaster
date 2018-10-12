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
#include "EventListDlg.h"
#include "Globals.h"
#include "Event.h"
#include "EventList.h"
#include "EventEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

namespace EditorDialogs
{
	CEventListDlg::CEventListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CEventListDlg::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CEventListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CEventListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CEventListDlg)
		DDX_Control(pDX, IDC_EVENT_LIST, m_lstEventList);
		DDX_Control(pDX, IDC_EVENT_EDIT, m_btnEdit);
		DDX_Control(pDX, IDC_EVENT_DELETE, m_btnDelete);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CEventListDlg, CDialog)
		//{{AFX_MSG_MAP(CEventListDlg)
		ON_BN_CLICKED(IDC_EVENT_ADD, OnEventAdd)
		ON_BN_CLICKED(IDC_EVENT_DELETE, OnEventDelete)
		ON_BN_CLICKED(IDC_EVENT_EDIT, OnEventEdit)
		ON_WM_DESTROY()
		ON_WM_SIZE()
		ON_NOTIFY(NM_DBLCLK, IDC_EVENT_LIST, OnDblclkEventList)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENT_LIST, OnColumnclickEventList)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_EVENT_LIST, OnItemchangedEventList)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_EVENT_LIST, OnItemActivate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CEventListDlg message handlers

	BOOL CEventListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		// TODO: Add extra initialization here
		m_lstEventList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);

		SetupLayout();

		CRect rect;
		m_lstEventList.GetClientRect(&rect);
		m_lstEventList.InsertColumn(0, "Name", LVCFMT_LEFT, 100);
		m_lstEventList.InsertColumn(1, "Seconds", LVCFMT_LEFT, 70);
		m_lstEventList.InsertColumn(2, "Action", LVCFMT_LEFT, rect.Width()-240);
		m_lstEventList.InsertColumn(3, "Group", LVCFMT_LEFT, 70);

		m_nSortType = EVENTSORT_NAME;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillEvents())
			return FALSE;

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CEventListDlg::FillEvents()
	{
		m_lstEventList.DeleteAllItems();
		m_lstEventList.SetItemCount(m_pEventList->GetCount());

		CMMEvent *pEvent = (CMMEvent *)m_pEventList->GetFirst();
		int nIndex = 0;
		while(pEvent)
		{
			int nItem = m_lstEventList.InsertItem(nIndex, pEvent->Name());
			m_lstEventList.SetItemData(nItem, reinterpret_cast<DWORD>(pEvent));
			CString strSeconds;
			strSeconds.Format("%d", pEvent->Seconds());
			m_lstEventList.SetItemText(nItem, 1, strSeconds);
			m_lstEventList.SetItemText(nItem, 2, pEvent->Event());
			m_lstEventList.SetItemText(nItem, 3, pEvent->Group());
			m_lstEventList.SetCheck(nItem, pEvent->Enabled());
			nIndex++;
			pEvent = (CMMEvent *)m_pEventList->GetNext();
		}

		m_lstEventList.SortItems(EventCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	int CEventListDlg::EventCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		CMMEvent *ptr1 = (CMMEvent *)lParam1;
		CMMEvent *ptr2 = (CMMEvent *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		switch(nSortType)
		{
		case EVENTSORT_NAME:
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Name(),ptr2->Name()));
			else
				return(strcmp(ptr2->Name(),ptr1->Name()));
			break;
		case EVENTSORT_SECONDS:
			if(nSortDir == CGlobals::LISTSORT_ASCEND)
				if(ptr1->Seconds() == ptr2->Seconds())
					return 0;
			if(ptr1->Seconds() > ptr2->Seconds())
				return 1;
			if(ptr1->Seconds() < ptr2->Seconds())
				return -1;
			else
				if(ptr2->Seconds() == ptr1->Seconds())
					return 0;
			if(ptr2->Seconds() > ptr1->Seconds())
				return 1;
			if(ptr2->Seconds() < ptr1->Seconds())
				return -1;
			break;
		case EVENTSORT_EVENT:
			{
				if (nSortDir == CGlobals::LISTSORT_ASCEND)
					return(strcmp(ptr1->Event(),ptr2->Event()));
				else
					return(strcmp(ptr2->Event(),ptr1->Event()));
			}
			break;
		case EVENTSORT_GROUP:
			if (nSortDir == CGlobals::LISTSORT_ASCEND)
				return(strcmp(ptr1->Group(),ptr2->Group()));
			else
				return(strcmp(ptr2->Group(),ptr1->Group()));
		}

		return 0;
	}

	void CEventListDlg::OnEventAdd() 
	{
		CEventEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		m_pEventList->Add(dlg.m_strName, dlg.m_nSeconds, dlg.m_strAction, dlg.m_strGroup);
		CMMEvent *pEvent = (CMMEvent *)m_pEventList->FindExact(dlg.m_strName);
		int nCount = m_lstEventList.GetItemCount();
		m_lstEventList.SetItemCount(nCount+1);
		int nItem = m_lstEventList.InsertItem(m_pEventList->GetFindIndex(), dlg.m_strName);
		m_lstEventList.SetItemData(nItem, reinterpret_cast<DWORD>(pEvent));
		CString strSeconds;
		strSeconds.Format("%d", pEvent->Seconds());
		m_lstEventList.SetItemText(nItem, 1, strSeconds);
		m_lstEventList.SetItemText(nItem, 2, pEvent->Event());
		m_lstEventList.SetItemText(nItem, 3, pEvent->Group());	
	}

	void CEventListDlg::OnEventDelete() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		CMMEvent *ptr = reinterpret_cast<CMMEvent *>(m_lstEventList.GetItemData(nIndex));
		m_pEventList->Remove(ptr);
		m_lstEventList.DeleteItem(nIndex);	
	}

	void CEventListDlg::OnEventEdit() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		CMMEvent *ptr = reinterpret_cast<CMMEvent *>(m_lstEventList.GetItemData(nIndex));

		CEventEditorDlg dlg;
		dlg.m_pEvent = ptr;
		if(dlg.DoModal() != IDOK)
			return;

		m_pEventList->Remove(ptr);

		m_pEventList->Add(dlg.m_strName, dlg.m_nSeconds, dlg.m_strAction, dlg.m_strGroup);
		ptr = (CMMEvent *)m_pEventList->FindExact(dlg.m_strName);
		if(dlg.m_bDisabled)
			ptr->Enabled(FALSE);

		m_lstEventList.SetItemText(nIndex, 0, ptr->Name());
		CString strSeconds;
		strSeconds.Format("%d", ptr->Seconds());
		m_lstEventList.SetItemText(nIndex, 1, strSeconds);
		m_lstEventList.SetItemText(nIndex, 2, ptr->Event());
		m_lstEventList.SetItemText(nIndex, 3, ptr->Group());
		m_lstEventList.SetCheck(nIndex, ptr->Enabled());
		m_lstEventList.SetItemData(nIndex, (DWORD)ptr);	
	}

	void CEventListDlg::OnDestroy() 
	{
		CDialog::OnDestroy();
	}

	void CEventListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		if(m_lstEventList.GetSafeHwnd() == NULL)
			return;

		UpdateLayout();

		CRect rect;
		m_lstEventList.GetClientRect(&rect);
		m_lstEventList.SetColumnWidth(0, 100);	
		m_lstEventList.SetColumnWidth(1, 70);	
		m_lstEventList.SetColumnWidth(2, rect.Width()-240);	
		m_lstEventList.SetColumnWidth(3, 70);	
	}

	void CEventListDlg::OnDblclkEventList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnEventEdit();
		*pResult = 0;
	}

	void CEventListDlg::OnColumnclickEventList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		// TODO: Add your control notification handler code here
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == EVENTSORT_NAME)
				FlipSortDirection();
			else
			{
				m_nSortType = EVENTSORT_NAME;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 1 :
			if (m_nSortType == EVENTSORT_SECONDS)
				FlipSortDirection();
			else
			{
				m_nSortType = EVENTSORT_SECONDS;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 2 :
			if (m_nSortType == EVENTSORT_EVENT)
				FlipSortDirection();
			else
			{
				m_nSortType = EVENTSORT_EVENT;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		case 3 :
			if (m_nSortType == EVENTSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = EVENTSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
		}
		m_lstEventList.SortItems(EventCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CEventListDlg::OnItemchangedEventList(NMHDR* pNMHDR, LRESULT* pResult) 
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

		CMMEvent *pEvent = (CMMEvent *)m_lstEventList.GetItemData(pNMListView->iItem);
		pEvent->Enabled(bChecked);
	}

	void CEventListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	int CEventListDlg::GetSelectedItem()
	{
		return(m_lstEventList.GetNextItem(-1,LVNI_SELECTED));
	}

	// Configures the layout manager for the resizable dialog
	void CEventListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_EVENT_ADD, NORESIZE)
			<< item(IDC_EVENT_EDIT, NORESIZE)
			<< item(IDC_EVENT_DELETE, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_EVENT_LIST)
			<< rightPane;

		UpdateLayout();
	}

	void CEventListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstEventList.GetSelectedCount();
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