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
#include "AliasList.h"
#include "AliasListDlg.h"
#include "Globals.h"
#include "Alias.h"
#include "AliasEditorDlg.h"

using namespace MMScript;
using namespace EditorDialogs;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace EditorDialogs
{
	CAliasListDlg::CAliasListDlg(CWnd* pParent /*=NULL*/)
		: ETSLayoutDialog(CAliasListDlg::IDD, pParent)
	{
		m_pAliasList = NULL;
	}


	void CAliasListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_ALIASLIST, m_lstAliasList);
		DDX_Control(pDX, IDC_BTN_EDIT_ACTION, m_btnEdit);
		DDX_Control(pDX, IDC_BTN_DELETE_ACTION, m_btnDelete);
	}


	BEGIN_MESSAGE_MAP(CAliasListDlg, CDialog)
		ON_BN_CLICKED(IDC_BTN_ADD_ACTION, OnBtnAddAction)
		ON_BN_CLICKED(IDC_BTN_DELETE_ACTION, OnBtnDeleteAction)
		ON_BN_CLICKED(IDC_BTN_EDIT_ACTION, OnBtnEditAction)
		ON_NOTIFY(NM_DBLCLK, IDC_ALIASLIST, OnDblclkAliaslist)
		ON_NOTIFY(LVN_COLUMNCLICK, IDC_ALIASLIST, OnColumnclickAliaslist)
		ON_WM_SIZE()
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_ALIASLIST, OnItemchangedAliaslist)
		ON_NOTIFY(LVN_ITEMACTIVATE, IDC_ALIASLIST, OnItemActivate)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CAliasListDlg message handlers

	BOOL CAliasListDlg::OnInitDialog() 
	{
		ASSERT(m_pAliasList != NULL);

		CDialog::OnInitDialog();

#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text

		m_lstAliasList.SetExtendedStyle(
			LVS_EX_CHECKBOXES | 
			LVS_EX_HEADERDRAGDROP | 
			LVS_EX_FULLROWSELECT | 
			LVS_EX_ONECLICKACTIVATE | 
			LVS_EX_LABELTIP |
			LVS_EX_UNDERLINEHOT);

		SetupLayout();


		CRect rect;
		m_lstAliasList.GetClientRect(&rect);
		m_lstAliasList.InsertColumn(0, "Alias", LVCFMT_LEFT, 100);
		m_lstAliasList.InsertColumn(1, "Action", LVCFMT_LEFT, rect.Width()-170);
		m_lstAliasList.InsertColumn(2, "Group", LVCFMT_LEFT, 70);

		m_nSortType = ALIASSORT_ALIAS;
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;

		if(!FillAliases())
			return FALSE;

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	BOOL CAliasListDlg::FillAliases()
	{
		m_lstAliasList.DeleteAllItems();
		m_lstAliasList.SetItemCount(m_pAliasList->GetCount());

		const CAliasList::ListType &list = m_pAliasList->GetAliases();

		CAliasList::ListType::const_iterator it = list.begin();
		CAliasList::ListType::const_iterator end = list.end();

		int nIndex = 0;
		while(it != end)
		{
			const CAlias::PtrType alias = *it;
			AddItem(alias, nIndex);
			nIndex++;
			++it;
		}
	m_lstAliasList.SortItems(AliasCompareFunc, m_nSortType | m_nSortDirection);
		return TRUE;
	}

	int CALLBACK CAliasListDlg::AliasCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{

		CAlias *lhs = (CAlias *)lParam1;
		CAlias *rhs = (CAlias *)lParam2;

		int nSortType = lParamSort & 0x00FF;
		int nSortDir = lParamSort & 0xFF00;
//TODO: KW Changes the logic below so the sort direction actually has an effect
		int result = 0;
		switch(nSortType)
		{
		case ALIASSORT_ALIAS :
			result = lhs->name().Compare(rhs->name());
			break;
		case ALIASSORT_ACTION :
			result = lhs->action().Compare(rhs->action());
			break;
		case ALIASSORT_GROUP :
			result = lhs->group().compare(rhs->group());
		}

		if (nSortDir == CGlobals::LISTSORT_DESCEND)
			result = -result;

		return result;
	}

	void CAliasListDlg::OnBtnAddAction() 
	{
		CAliasEditorDlg dlg;
		if(dlg.DoModal() != IDOK)
			return;

		CAlias::PtrType alias;
		if( m_pAliasList->Add( dlg.m_strText, dlg.m_strAction, dlg.m_strGroup, alias ) )
		{
			AddItem(alias, m_lstAliasList.GetItemCount());
		}
	}

	void CAliasListDlg::OnBtnDeleteAction() 
	{
		int nIndex = GetSelectedItem();
		if (nIndex == -1)
			return;
		CAlias *pItem = 
			reinterpret_cast<CAlias *>(m_lstAliasList.GetItemData(nIndex));
		
		if (!m_pAliasList->RemoveByDisplayName(pItem->GetName()))
		{
			AfxMessageBox(_T("Failed to edit the alias (at remove)."),
				MB_ICONERROR | MB_OK );
		}
					
		m_lstAliasList.DeleteItem(nIndex);	

	}

	void CAliasListDlg::OnBtnEditAction() 
	{
		int nIndex = GetSelectedItem();
		if(nIndex == -1)
			return;
		//TODO: KW original code
		//CAlias::PtrType *pItem = 
		//	reinterpret_cast<CAlias::PtrType *>(m_lstAliasList.GetItemData(nIndex));
		//TODO: KW new code
		CAlias *pItem = 
			reinterpret_cast<CAlias *>(m_lstAliasList.GetItemData(nIndex));
		CAliasEditorDlg dlg;

		dlg.SetName( pItem->DisplayName());
		dlg.SetAction( pItem->GetAction());
		dlg.SetGroup( pItem->GetGroup().c_str());
		dlg.SetIsDisabled(!pItem->IsEnabled()) ;
		
		//TODO: KW end of new code
		//dlg._alias = item;
		if(dlg.DoModal() != IDOK)
			return;

		
//original		m_pAliasList->RemoveByDisplayName(item->DisplayName());
		if (!m_pAliasList->RemoveByDisplayName(pItem->GetName()))
		{
			AfxMessageBox(_T("Failed to edit the alias (at remove)."),
				MB_ICONERROR | MB_OK );
		}
		else
		{
			m_lstAliasList.DeleteItem(nIndex);
			CAliasListDlg::AddAliasFromDialog( dlg, nIndex ); 
			{

//				AddItem(item, nIndex);

				m_lstAliasList.SetSelectionMark(nIndex);
			}

		}
	}
	//TODO: KW added method to add alias from dialog to avoid shared_ptr problems
	void CAliasListDlg::AddAliasFromDialog( CAliasEditorDlg &dlg, int nIndex )
	{
		CAlias *pEmpty = NULL;
		MMScript::CAlias::PtrType emptyalias( pEmpty );

		CAlias::PtrType newAlias;
		if( m_pAliasList->Add( 
			dlg.GetName(), 
			dlg.GetAction(), 
			dlg.GetGroup(), 
			newAlias ) )
		{
			newAlias->enabled(dlg.IsEnabled() );
			AddItem( newAlias, nIndex );
			newAlias = emptyalias;
		}
		else
		{
			AfxMessageBox("Error adding alias to list.");
		}
	}

	int CAliasListDlg::GetSelectedItem()
	{
		return(m_lstAliasList.GetNextItem(-1,LVNI_SELECTED));
	}

	void CAliasListDlg::OnDblclkAliaslist(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		OnBtnEditAction();	
		*pResult = 0;
	}

	void CAliasListDlg::OnColumnclickAliaslist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		switch(pNMListView->iSubItem)
		{
		case 0 :
			if (m_nSortType == ALIASSORT_ALIAS)
				FlipSortDirection();
			else
			{
				m_nSortType = ALIASSORT_ALIAS;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 1 :
			if (m_nSortType == ALIASSORT_ACTION)
				FlipSortDirection();
			else
			{
				m_nSortType = ALIASSORT_ACTION;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;

		case 2 :
			if (m_nSortType == ALIASSORT_GROUP)
				FlipSortDirection();
			else
			{
				m_nSortType = ALIASSORT_GROUP;
				m_nSortDirection = CGlobals::LISTSORT_ASCEND;
			}
			break;
		}
			m_lstAliasList.SortItems(AliasCompareFunc,m_nSortType | m_nSortDirection);

		*pResult = 0;
	}

	void CAliasListDlg::FlipSortDirection()
	{
		if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
			m_nSortDirection = CGlobals::LISTSORT_DESCEND;
		else
			m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	}

	void CAliasListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		UpdateLayout();
	}

	void CAliasListDlg::OnItemActivate(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		//NM_LISTVIEW *pNMLV = reinterpret_cast<NM_LISTVIEW *>(pNMHDR);
		*pResult = 0;
		int nSelectionCount = m_lstAliasList.GetSelectedCount();
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

	void CAliasListDlg::OnItemchangedAliaslist(NMHDR* pNMHDR, LRESULT* pResult) 
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

		//CAlias::PtrType *pItem = 
		//	reinterpret_cast<CAlias::PtrType *>(m_lstAliasList.GetItemData(pNMListView->iItem));

		//CAlias::PtrType item = *pItem;

		//item->enabled( bChecked ? true : false );
		//TODO: KW patch to fix checkbox error
        CAlias *pItem = (CAlias *)m_lstAliasList.GetItemData(pNMListView->iItem); 
        bool tf = bChecked ? true : false; 
        pItem->enabled(tf); 

	}

	void CAliasListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDOK, NORESIZE)
			<< itemGrowing(VERTICAL)
			<< item(IDC_BTN_ADD_ACTION, NORESIZE)
			<< item(IDC_BTN_EDIT_ACTION, NORESIZE)
			<< item(IDC_BTN_DELETE_ACTION, NORESIZE);


		CreateRoot(HORIZONTAL)
			<< item(IDC_ALIASLIST)
			<< rightPane;

		UpdateLayout();
	}

	void CAliasListDlg::AddItem(
		const CAlias::PtrType &alias,
		int nIndex)
	{
		int nItem = m_lstAliasList.InsertItem( nIndex, alias->DisplayName() );
		m_lstAliasList.SetItemData( nItem, reinterpret_cast<DWORD>(alias.get()) );
		m_lstAliasList.SetItemText( nItem, 1, alias->action() );
		m_lstAliasList.SetItemText( nItem, 2, alias->group().c_str() );
		m_lstAliasList.SetCheck( nItem, alias->enabled() );
	}
}