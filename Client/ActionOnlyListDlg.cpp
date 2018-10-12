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
#include "ActionList.h"
#include "ActionOnlyListDlg.h"
#include "Action.h"
#include "Globals.h"
#include "ActionEditorDlg.h"

using namespace MMScript;
using namespace EditorDialogs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CActionOnlyListDlg::CActionOnlyListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CActionOnlyListDlg::IDD, pParent)
	{
		m_pActionList = NULL;
	}


	void CActionOnlyListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_BTN_ADD_ACTION, m_btnAdd);
		DDX_Control(pDX, IDC_BTN_DELETE_ACTION, m_btnDelete);
		DDX_Control(pDX, IDC_BTN_EDIT_ACTION, m_btnEdit);
		DDX_Control(pDX, IDOK, m_btnOk);
		DDX_Control(pDX, IDC_ACTIONLIST, m_lstActionList);
	}


	BEGIN_MESSAGE_MAP(CActionOnlyListDlg, CDialog)
		ON_BN_CLICKED(IDC_BTN_EDIT_ACTION, OnBtnEditAction)
		ON_BN_CLICKED(IDC_BTN_DELETE_ACTION, OnBtnDeleteAction)
		ON_BN_CLICKED(IDC_BTN_ADD_ACTION, OnBtnAddAction)
		ON_WM_DESTROY()
		ON_WM_SIZE()
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_ACTIONLIST, OnColumnclickActionlist)
		ON_NOTIFY(NM_DBLCLK, IDC_ACTIONLIST, OnDblclkActionlist)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_ACTIONLIST, OnItemchangedActionlist)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_ACTIONLIST, OnItemActivate)
	END_MESSAGE_MAP()

	
	/////////////////////////////////////////////////////////////////////////////
	// CActionOnlyListDlg message handlers

	void CActionOnlyListDlg::OnBtnEditAction() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;

		//TODO: KW original code
		//CAction::PtrType pItem = 
		//	reinterpret_cast<CAction::PtrType >( m_lstActionList.GetItemData( nIndex ) );

		//CAction::PtrType item = *pItem;
		//TODO: KW new code (still a problem with this)
		CAction  *pItem = 
			reinterpret_cast<MMScript::CAction *>( m_lstActionList.GetItemData( nIndex ) );
		

		CActionEditorDlg dlg;
		dlg.SetAction(pItem->GetAction());
		dlg.SetTrigger( pItem->GetTrigger());
		
		dlg.SetGroup( pItem->GetGroup());
		dlg.SetEnabled(pItem->IsEnabled()) ;
		//dlg.SetAction( actionitem ); 
		
		if(dlg.DoModal() != IDOK)
		{
            return;
		}

		//TODO: KW add !m_lstActionList.DeleteItem(nIndex) to remove from listdialog

		if(!m_lstActionList.DeleteItem(nIndex))
		{
			AfxMessageBox(_T("Failed to edit the action (at remove)."),
				MB_ICONERROR | MB_OK );
		}

		if (!m_pActionList->Remove(pItem->GetTrigger()))
		{
			AfxMessageBox(_T("Failed to edit the action (at remove)."),
				MB_ICONERROR | MB_OK );
		}

		// add the new version to both the actionlist and the list control in the dialog
		AddActionFromDialog( dlg, m_lstActionList.GetItemCount()  );
	}

	void CActionOnlyListDlg::AddActionFromDialog( CActionEditorDlg &dlg, int nIndex )
	{
		CAction *pEmpty = NULL;
		MMScript::CAction::PtrType emptyaction( pEmpty );

		CAction::PtrType newAction;
		if( m_pActionList->Add( 
			dlg.GetTrigger(), 
			dlg.GetAction(), 
			dlg.GetGroup(), 
			newAction ) )
		{
			newAction->SetEnabled( dlg.IsEnabled() );
			AddItem( newAction, nIndex );
			newAction = emptyaction;
		}
		else
		{
			AfxMessageBox("Error adding trigger to list.");
		}
	}

	void CActionOnlyListDlg::OnBtnDeleteAction() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;

		//TODO: KW original code
		//CAction::PtrType *pItem = 
		//	reinterpret_cast<CAction::PtrType *>( m_lstActionList.GetItemData( nIndex ) );
		//TODO: KW new code
		CAction *pItem = 
			reinterpret_cast<CAction *>( m_lstActionList.GetItemData( nIndex ) );
		

		//TODO: KW add the checks for returns and messages and delete from list and dialog
		if(  !m_lstActionList.DeleteItem(nIndex)   )
		{
			AfxMessageBox( _T("Failed to delete the action."),
				MB_ICONERROR | MB_OK );
		}
		else
		{
		if(!m_pActionList->Remove( pItem->GetTrigger() ))
				AfxMessageBox( _T("Failed to delete the action (internal list)."),
				MB_ICONERROR | MB_OK );
		
		}
	}

	void CActionOnlyListDlg::OnBtnAddAction() 
	{
		CActionEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		AddActionFromDialog( dlg, m_lstActionList.GetItemCount() );
	}

	BOOL CActionOnlyListDlg::OnInitDialog() 
	{
		ASSERT(m_pActionList != NULL);

		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstActionList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);

		SetupLayout();

		CRect rect;
		m_lstActionList.GetClientRect(&rect);
		m_lstActionList.InsertColumn(0, "Mask", LVCFMT_LEFT, 100);
		m_lstActionList.InsertColumn(1, "Action", LVCFMT_LEFT, rect.Width()-170);
		m_lstActionList.InsertColumn(2, "Group", LVCFMT_LEFT, 70);

		m_nSortType = ACTSORT_TRIGGER;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;


		if(!FillActions())
			return FALSE;

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CActionOnlyListDlg::OnDestroy() 
	{
		CDialog::OnDestroy();
	}

	void CActionOnlyListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		if(GetSafeHwnd()== NULL)
			return;

		UpdateLayout();

		if(m_lstActionList.GetSafeHwnd() == NULL)
			return;

		CRect rect;
		m_lstActionList.GetClientRect(&rect);
		m_lstActionList.SetColumnWidth(0, 100);	
		m_lstActionList.SetColumnWidth(1, 70);	
		m_lstActionList.SetColumnWidth(2, rect.Width()-170);	
	}

	BOOL CActionOnlyListDlg::FillActions()
	{
		m_lstActionList.DeleteAllItems();
		m_lstActionList.SetItemCount(m_pActionList->GetCount());

		const CActionList::ListType &list = m_pActionList->GetList();
		CActionList::ListType::const_iterator it = list.begin();
		CActionList::ListType::const_iterator end = list.end();

		int nIndex = 0;
		while( it != end )
		{
			const CAction::PtrType action = *it;
			AddItem( action, nIndex );
			nIndex++;
			++it;
		}

		m_lstActionList.SortItems(ActionCompareFunc, m_nSortType | m_nSortDirection );

		return TRUE;
	}
//TODO: KW change to be like eventlistDlg
//	int CALLBACK CActionOnlyListDlg::ActionCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	int CALLBACK CActionOnlyListDlg::ActionCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		//TODO: KW changed to not use PtrType
		//CAction::PtrType *ptr1 = reinterpret_cast<CAction::PtrType *>(lParam1);
		//CAction::PtrType *ptr2 = reinterpret_cast<CAction::PtrType *>(lParam2);
		//CAction::PtrType lhs = *ptr1;
		//CAction::PtrType rhs = *ptr2;
		 
		
		CAction *lhs = (CAction *)lParam1;
		CAction *rhs = (CAction *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;

		int retval = 0;

		switch(nSortType)
		{
		case ACTSORT_TRIGGER :
			retval = lhs->GetTrigger().compare( rhs->GetTrigger() );
			break;
		case ACTSORT_ACTION:
			retval = lhs->GetAction().compare( rhs->GetAction() );
			break;
		case ACTSORT_GROUP:
			retval = lhs->GetGroup().compare( rhs->GetGroup() );
		}

		if( nSortDir == CGlobals::LISTSORT_DESCEND )
			retval = -retval;

		return retval;
	}

	void CActionOnlyListDlg::OnColumnclickActionlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == ACTSORT_TRIGGER)
				FlipSortDirection();
			else
			{
				m_nSortType = ACTSORT_TRIGGER;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 1 :
			if (m_nSortType == ACTSORT_ACTION)
				FlipSortDirection();
			else
			{
				m_nSortType = ACTSORT_ACTION;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 2 :
			if (m_nSortType == ACTSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = ACTSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}

		m_lstActionList.SortItems(ActionCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CActionOnlyListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	int CActionOnlyListDlg::GetSelectedItem()
	{
		return(m_lstActionList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CActionOnlyListDlg::OnDblclkActionlist(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnBtnEditAction();
		*pResult = 0;
	}

	void CActionOnlyListDlg::OnItemchangedActionlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

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

		//CAction::PtrType *pItem = 
		//	reinterpret_cast<CAction::PtrType *>(m_lstActionList.GetItemData(pNMListView->iItem));

		//CAction::PtrType item = *pItem;

		//item->SetEnabled( bChecked ? true : false );
		//TODO: KW patch to fix checkbox error
        CAction *pItem = (CAction *)m_lstActionList.GetItemData(pNMListView->iItem); 
        bool tf = bChecked ? true : false; 
        pItem->SetEnabled(tf); 

	}

	void CActionOnlyListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_BTN_ADD_ACTION, NORESIZE)
			<< item(IDC_BTN_EDIT_ACTION, NORESIZE)
			<< item(IDC_BTN_DELETE_ACTION, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_ACTIONLIST)
			<< rightPane;

		UpdateLayout();
	}

	void CActionOnlyListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstActionList.GetSelectedCount();
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

	void CActionOnlyListDlg::AddItem( 
		const MMScript::CAction::PtrType &action, 
		int nIndex )
	{
		int nItem = m_lstActionList.InsertItem( nIndex, action->GetTrigger().c_str() );
		m_lstActionList.SetItemData( nItem, reinterpret_cast<DWORD>(action.get()) );
		m_lstActionList.SetCheck( nItem, action->IsEnabled() );
		m_lstActionList.SetItemText( nItem, 1, action->GetAction().c_str() );
		m_lstActionList.SetItemText( nItem, 2, action->GetGroup().c_str() );
	}
}