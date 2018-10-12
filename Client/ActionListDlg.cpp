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
#include "IfxDoc.h"
#include "ActionListDlg.h"
#include "ActionList.h"
#include "Action.h"
#include "ActionEditorDlg.h"
#include "AliasEditorDlg.h"
#include "BarItemEditorDlg.h"
#include "VarEditDlg.h"
#include "EventEditorDlg.h"
#include "GagEditorDlg.h"
#include "SubEditorDlg.h"
#include "MacroEditorDlg.h"
#include "HighEditorDlg.h"
#include "Colors.h"

#include "Alias.h"
#include "AliasList.h"
#include "ArrayList.h"
#include "Array.h"
#include "BarItem.h"
#include "StatusBar.h"
#include "Event.h"
#include "EventList.h"
#include "Gag.h"
#include "GagList.h"
#include "High.h"
#include "HighList.h"
#include "UserLists.h"
#include "MMList.h"
#include "Macro.h"
#include "MacroList.h"
#include "Variable.h"
#include "VarList.h"
#include "Sub.h"
#include "SubList.h"
#include "Tab.h"
#include "StringUtils.h"

using namespace std;
using namespace MMScript;
using namespace MudmasterColors;
using namespace EditorDialogs;
using namespace Utilities;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ActionListDlg dialog
LPCTSTR CActionListDlg::ACTIONS		= "Actions";
LPCTSTR CActionListDlg::ALIASES		= "Aliases";
LPCTSTR CActionListDlg::LISTS		= "Lists";
LPCTSTR CActionListDlg::ARRAYS		= "Arrays";
LPCTSTR CActionListDlg::EVENTS		= "Events";
LPCTSTR CActionListDlg::GAGS		= "Gags";
LPCTSTR CActionListDlg::HIGHS		= "Highlights";
LPCTSTR CActionListDlg::MACROS		= "Macros";
LPCTSTR CActionListDlg::BARITEMS	= "Status Bar Items";
LPCTSTR CActionListDlg::SUBS		= "Substitutions";
LPCTSTR CActionListDlg::TABS		= "Tabs";
LPCTSTR CActionListDlg::VARS		= "Variables";

namespace EditorDialogs
{
	CActionListDlg::CActionListDlg(COptions &options, CSession* pParent /*=NULL*/)
		: ETSLayoutDialog(CActionListDlg::IDD, NULL)
		, _options(options)
	{
		m_pActionList = NULL;
		m_pAliasList = NULL;
		m_pArrayList = NULL;
		m_pBarItems = NULL;
		m_pEventList = NULL;
		m_pGagList = NULL;
		m_pHighList = NULL;
		m_pLists = NULL;
		m_pMacroList = NULL;
		m_pSubList = NULL;
		m_pVarList = NULL;
		m_pParent = pParent;
		//{{AFX_DATA_INIT(CActionListDlg)
		// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CActionListDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CActionListDlg)
		DDX_Control(pDX, IDC_ACTION_GROUPS, m_treeGroups);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CActionListDlg, CDialog)
		//{{AFX_MSG_MAP(CActionListDlg)
		ON_WM_SIZE()
		ON_NOTIFY(NM_DBLCLK, IDC_ACTION_GROUPS, OnDblclkActionGroups)
		ON_NOTIFY(NM_RCLICK, IDC_ACTION_GROUPS, OnRclkActionGroups)
		ON_WM_DESTROY()
		//}}AFX_MSG_MAP
		ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	END_MESSAGE_MAP()
		//ON_NOTIFY(NM_RCLICK, IDC_ACTION_GROUPS, OnRclkActionGroups)

	/////////////////////////////////////////////////////////////////////////////
	// ActionListDlg message handlers

	BOOL CActionListDlg::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		SetupLayout();

		m_ilImageList.Create(IDB_CATEGORY, 16, 1, RGB(255,255,255));

		m_treeGroups.SetImageList(&m_ilImageList, TVSIL_NORMAL);

		m_hRoot = m_treeGroups.InsertItem("Script Entities", 0, 1);

		AddAliases();
		if(m_pActionList!=NULL)
		{
		AddActions();
		AddArrays();
		AddBarItems();
		AddEvents();
		AddGags();
		AddHighs();
		AddLists();
		AddMacros();
		AddSubs();
		AddVars();
		}
		//May as well show all the first level nodes since that's why this tree view was opened
		m_treeGroups.Expand(m_treeGroups.GetRootItem(),TVE_EXPAND);
		m_treeGroups.SortChildren(m_treeGroups.GetRootItem());
		if(m_pActionList == NULL)
		{
			HTREEITEM hItem = m_treeGroups.GetChildItem(m_treeGroups.GetRootItem());
			while (hItem != NULL)
			{
				m_treeGroups.Expand(m_treeGroups.GetChildItem(hItem),TVE_EXPAND);
				m_treeGroups.SortChildren(m_treeGroups.GetChildItem(hItem)); 
				hItem = m_treeGroups.GetNextSiblingItem(hItem);
			}
		}
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CActionListDlg::OnSize(UINT nType, int cx, int cy) 
	{
		CDialog::OnSize(nType, cx, cy);

		UpdateLayout();

		//if(m_treeGroups.GetSafeHwnd() != NULL)
		//	m_treeGroups.MoveWindow(0,0,cx,cy);
	}

	bool CActionListDlg::HandleAction( HTREEITEM hItem )
	{
		bool retval = false;

		ActionMap::iterator it = _actions.find( hItem );
		if( it != _actions.end() )
		{
			CAction::PtrType action = it->second;
			EditAction( action, hItem );
			retval = true;
		}

		return retval;
	}

	void CActionListDlg::OnDblclkActionGroups(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		HTREEITEM hItem = m_treeGroups.GetNextItem(TVI_ROOT, TVGN_CARET);
		if(hItem == NULL)
		{
			AfxMessageBox("Error selecting item."); 
			return;
		}

		if( HandleAction( hItem ) )
		{
			*pResult = 1;
			return;
		}

		{
			AliasMap::iterator it = _aliases.find(hItem);
			if(it != _aliases.end())
			{
				CAlias::PtrType alias = it->second;
				EditAlias(alias, hItem);
				*pResult = 1;
				return;
			}
		}

		CMMArray *pArray = NULL;
		BOOL bResult = m_mapArrays.Lookup(hItem, (void *&)pArray);
		if(bResult)
		{
			EditArray(pArray, hItem);
			*pResult = 1;
			return;
		}

		CBarItem *pBarItem = NULL;
		bResult = m_mapBarItems.Lookup(hItem, (void *&)pBarItem);
		if(bResult)
		{
			EditBarItem(pBarItem, hItem);
			*pResult = 1;
			return;
		}

		CMMEvent *pEvent = NULL;
		bResult = m_mapEvents.Lookup(hItem, (void *&)pEvent);
		if(bResult)
		{
			EditEvent(pEvent, hItem);
			*pResult = 1;
			return;
		}

		CGag *pGag = NULL;
		bResult = m_mapGags.Lookup(hItem, (void *&)pGag);
		if(bResult)
		{
			EditGag(pGag, hItem);
			*pResult = 1;
			return;
		}

		CHigh *pHigh = NULL;
		bResult = m_mapHighs.Lookup(hItem, (void *&)pHigh);
		if(bResult)
		{
			EditHigh(pHigh, hItem);
			*pResult = 1;
			return;
		}

		CMMList *pList = NULL;
		bResult = m_mapLists.Lookup(hItem, (void *&)pList);
		if(bResult)
		{
			EditList(pList, hItem);
			*pResult = 1;
			return;
		}

		CMacro *pMacro = NULL;
		bResult = m_mapMacros.Lookup(hItem, (void *&)pMacro);
		if(bResult)
		{
			EditMacro(pMacro, hItem);
			*pResult = 1;
			return;
		}

		CSub *pSub = NULL;
		bResult = m_mapSubs.Lookup(hItem, (void *&)pSub);
		if(bResult)
		{
			EditSub(pSub, hItem);
			*pResult = 1;
			return;
		}

		CVariable *pVar = NULL;
		bResult = m_mapVars.Lookup(hItem, (void *&)pVar);
		if(bResult)
		{
			EditVar(pVar, hItem);
			*pResult = 1;
			return;
		}

		*pResult = 0;
	}
	void CActionListDlg::OnRclkActionGroups(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
	{
		HTREEITEM hItem = m_treeGroups.GetSelectedItem();
		
		AliasMap::iterator it = _aliases.find(hItem);
		if(it != _aliases.end())
		{
			CAlias::PtrType alias = it->second;
			m_pParent->ExecuteCommand(alias.get()->GetName()) ;
			EndDialog(IDOK);		
		}
		*pResult = 1;

		return;
			

	}

	void CActionListDlg::AddActionToTree(CAction::PtrType action)
	{
		string strGroup = action->GetGroup();
		strGroup = Utilities::trim_left( strGroup );

		if(strGroup.empty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup( strGroup.c_str() );
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)ACTIONS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iActions = m_treeGroups.InsertItem(ACTIONS, 0, 1, node);
			map->SetAt((void *)ACTIONS, iActions);
			map->Lookup((void *)ACTIONS, (void *&)node);
			
		}


		HTREEITEM anItem = m_treeGroups.InsertItem( action->GetTrigger().c_str(), 2, 2, node);
		_actions[anItem] = action;
	}

	void CActionListDlg::AddActions()
	{
		const CActionList::ListType &list = m_pActionList->GetList();
		CActionList::ListType::const_iterator it = list.begin();
		CActionList::ListType::const_iterator end = list.end();
		while( it != end )
		{
			AddActionToTree( *it );
			++it;
		}
	}

	HTREEITEM CActionListDlg::GetGroup( LPCTSTR groupName )
	{
		HTREEITEM node;
		if( m_mapGroupToGroupNode.Lookup( groupName, (void *&)node ) )
		{
			return node;
		}
		else
		{
			HTREEITEM aNode = m_treeGroups.InsertItem( groupName, 0, 1, m_hRoot );
			m_mapGroupToGroupNode.SetAt( groupName, aNode );

			//HTREEITEM iActions = m_treeGroups.InsertItem(ACTIONS, 0, 1, aNode);
			//HTREEITEM iAliases = m_treeGroups.InsertItem(ALIASES, 0, 1, aNode);
			//HTREEITEM iArrays = m_treeGroups.InsertItem(ARRAYS, 0, 1, aNode);
			//HTREEITEM iBarItems = m_treeGroups.InsertItem(BARITEMS, 0, 1, aNode);
			//HTREEITEM iEvents = m_treeGroups.InsertItem(EVENTS, 0, 1, aNode);
			//HTREEITEM iGags = m_treeGroups.InsertItem(GAGS, 0, 1, aNode);
			//HTREEITEM iHighs = m_treeGroups.InsertItem(HIGHS, 0, 1, aNode);
			//HTREEITEM iLists = m_treeGroups.InsertItem(LISTS, 0, 1, aNode);
			//HTREEITEM iMacros = m_treeGroups.InsertItem(MACROS, 0, 1, aNode);
			//HTREEITEM iSubs = m_treeGroups.InsertItem(SUBS, 0, 1, aNode);
			//HTREEITEM iTabs = m_treeGroups.InsertItem(TABS, 0, 1, aNode);
			//HTREEITEM iVars = m_treeGroups.InsertItem(VARS, 0, 1, aNode);

			CMapPtrToPtr *map = new CMapPtrToPtr;
			//map->SetAt((void *)ACTIONS, iActions);
			////map->SetAt((void *)ALIASES, iAliases);
			//map->SetAt((void *)ARRAYS, iArrays);
			//map->SetAt((void *)BARITEMS, iBarItems);
			//map->SetAt((void *)EVENTS, iEvents);
			//map->SetAt((void *)GAGS, iGags);
			//map->SetAt((void *)HIGHS, iHighs);
			//map->SetAt((void *)LISTS, iLists);
			//map->SetAt((void *)MACROS, iMacros);
			//map->SetAt((void *)SUBS, iSubs);
			//map->SetAt((void *)TABS, iTabs);
			//map->SetAt((void *)VARS, iVars);
			m_mapGroupNodeToEntityList.SetAt(aNode, map);

			return aNode;
		}
	}

	void CActionListDlg::OnDestroy() 
	{
		CDialog::OnDestroy();

		for(POSITION pos = m_mapGroupNodeToEntityList.GetStartPosition();pos;)
		{
			CMapPtrToPtr *map;
			HTREEITEM node;
			m_mapGroupNodeToEntityList.GetNextAssoc(pos, (void *&)node, (void *&)map);
			map->RemoveAll();
			delete map;
		}
	}

	void CActionListDlg::AddAliases()
	{
		const CAliasList::ListType &list = m_pAliasList->GetAliases();
		CAliasList::ListType::const_iterator it = list.begin();
		CAliasList::ListType::const_iterator end = list.end();
		while(it != end)
		{
			CAlias::PtrType alias = *it;
			AddAliasToTree(alias);
			++it;
		}
	}

	void CActionListDlg::AddAliasToTree(CAlias::PtrType alias)
	{
		CString strGroup = alias->group().c_str();
		strGroup.TrimLeft();

		if( strGroup.IsEmpty() )
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		//ASSERT(bResult);

		bResult = map->Lookup((void *)ALIASES, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iAliases = m_treeGroups.InsertItem(ALIASES, 0, 1, node);
			map->SetAt((void *)ALIASES, iAliases);
			map->Lookup((void *)ALIASES, (void *&)node);
			
		}
		
		HTREEITEM anItem = m_treeGroups.InsertItem( alias->DisplayName(), 2, 2, node );
		_aliases[anItem] = alias;
	}

	void CActionListDlg::AddArrays()
	{
		CMMArray *pArray = (CMMArray *)m_pArrayList->GetFirst();
		while(pArray != NULL)
		{
			AddArrayToTree(pArray);
			pArray=(CMMArray *)m_pArrayList->GetNext();
		}

	}

	void CActionListDlg::AddArrayToTree(CMMArray *pArray)
	{
		CString strGroup = pArray->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)ARRAYS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iArrays = m_treeGroups.InsertItem(ARRAYS, 0, 1, node);
			map->SetAt((void *)ARRAYS, iArrays);
			map->Lookup((void *)ARRAYS, (void *&)node);
			
		}

		HTREEITEM anItem = m_treeGroups.InsertItem(pArray->Name(), 2, 2, node);
		m_mapArrays.SetAt(anItem, pArray);
	}

	void CActionListDlg::AddBarItems()
	{
		CBarItem *pItem = (CBarItem *)m_pBarItems->GetFirst();
		while(pItem != NULL)
		{
			AddBarItemToTree(pItem);
			pItem=(CBarItem *)m_pBarItems->GetNext();
		}
	}

	void CActionListDlg::AddBarItemToTree(CBarItem *pItem)
	{
		CString strGroup = pItem->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)BARITEMS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iBaritems = m_treeGroups.InsertItem(BARITEMS, 0, 1, node);
			map->SetAt((void *)BARITEMS, iBaritems);
			map->Lookup((void *)BARITEMS, (void *&)node);
			
		}


		CString strText;
		pItem->MapToText(strText, FALSE);
		HTREEITEM anItem = m_treeGroups.InsertItem(strText, 2, 2, node);
		m_mapBarItems.SetAt(anItem, pItem);
	}

	void CActionListDlg::AddEvents()
	{
		CMMEvent *pEvent = (CMMEvent *)m_pEventList->GetFirst();
		while(pEvent != NULL)
		{
			AddEventToTree(pEvent);
			pEvent=(CMMEvent *)m_pEventList->GetNext();
		}
	}

	void CActionListDlg::AddGags()
	{
		CGag *pItem = dynamic_cast<CGag *>(m_pGagList->GetFirst());
		while(pItem != NULL)
		{
			AddGagToTree(pItem);
			pItem = dynamic_cast<CGag *>(m_pGagList->GetNext());
		}
	}

	void CActionListDlg::AddHighs()
	{
		CHigh *pItem = (CHigh *)m_pHighList->GetFirst();
		while(pItem != NULL)
		{
			AddHighToTree(pItem);
			pItem=(CHigh *)m_pHighList->GetNext();
		}
	}

	void CActionListDlg::AddLists()
	{
		CMMList *pItem = (CMMList *)m_pLists->GetFirst();
		while(pItem != NULL)
		{
			AddListToTree(pItem);
			pItem=(CMMList *)m_pLists->GetNext();
		}
	}

	void CActionListDlg::AddMacros()
	{
		CMacro *pItem = (CMacro *)m_pMacroList->GetFirst();
		while(pItem != NULL)
		{
			AddMacroToTree(pItem);
			pItem=(CMacro *)m_pMacroList->GetNext();
		}

	}

	void CActionListDlg::AddSubs()
	{
		CSub *pItem = dynamic_cast<CSub *>(m_pSubList->GetFirst());
		while(pItem != NULL)
		{
			AddSubToTree(pItem);
			pItem=dynamic_cast<CSub *>(m_pSubList->GetNext());
		}
	}

	void CActionListDlg::AddVars()
	{
		for(int i = 0; i < m_pVarList->GetSize(); ++i)
		{
			CVariable &v = m_pVarList->GetAt(i);
			AddVarToTree(v);
		}
	}

	void CActionListDlg::AddEventToTree(CMMEvent *pEvent)
	{
		CString strGroup = pEvent->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)EVENTS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iEvents = m_treeGroups.InsertItem(EVENTS, 0, 1, node);
			map->SetAt((void *)EVENTS, iEvents);
			map->Lookup((void *)EVENTS, (void *&)node);
			
		}

		HTREEITEM anItem = m_treeGroups.InsertItem(pEvent->Name(), 2, 2, node);
		m_mapEvents.SetAt(anItem, pEvent);

	}

	void CActionListDlg::AddGagToTree(CGag *pGag)
	{
		CString strGroup = pGag->Group();
		strGroup.TrimLeft();
		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)GAGS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iGags = m_treeGroups.InsertItem(GAGS, 0, 1, node);
			map->SetAt((void *)GAGS, iGags);
			map->Lookup((void *)GAGS, (void *&)node);
			
		}


		CString strText = pGag->GetTrigger();
		HTREEITEM anItem = m_treeGroups.InsertItem(strText, 2, 2, node);
		m_mapGags.SetAt(anItem, pGag);

	}

	void CActionListDlg::AddHighToTree(CHigh *pHigh)
	{
		CString strGroup = pHigh->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)HIGHS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iHighs = m_treeGroups.InsertItem(HIGHS, 0, 1, node);
			map->SetAt((void *)HIGHS, iHighs);
			map->Lookup((void *)HIGHS, (void *&)node);
			
		}


		CString strText;
		pHigh->ToMask(strText);
		HTREEITEM anItem = m_treeGroups.InsertItem(strText, 2, 2, node);
		m_mapHighs.SetAt(anItem, pHigh);

	}

	void CActionListDlg::AddListToTree(CMMList *pList)
	{
		CString strGroup = pList->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)LISTS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iLists = m_treeGroups.InsertItem(LISTS, 0, 1, node);
			map->SetAt((void *)LISTS, iLists);
			map->Lookup((void *)LISTS, (void *&)node);
			
		}


		HTREEITEM anItem = m_treeGroups.InsertItem(pList->ListName(), 2, 2, node);
		m_mapLists.SetAt(anItem, pList);

	}

	void CActionListDlg::AddMacroToTree(CMacro *pMacro)
	{
		CString strGroup = pMacro->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)MACROS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iMacros = m_treeGroups.InsertItem(MACROS, 0, 1, node);
			map->SetAt((void *)MACROS, iMacros);
			map->Lookup((void *)MACROS, (void *&)node);
			
		}


		CString strText;
		pMacro->KeyToName(strText);
		HTREEITEM anItem = m_treeGroups.InsertItem(strText, 2, 2, node);
		m_mapMacros.SetAt(anItem, pMacro);

	}

	void CActionListDlg::AddSubToTree(CSub *pSub)
	{
		CString strGroup = pSub->Group();
		strGroup.TrimLeft();

		if(strGroup.IsEmpty())
			strGroup = "zz No Group zz";

		HTREEITEM node = GetGroup(strGroup);
		ASSERT(node != NULL);

		CMapPtrToPtr *map;
		BOOL bResult = m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);
		ASSERT(bResult);

		bResult = map->Lookup((void *)SUBS, (void *&)node);
		if(!bResult)
		{
			HTREEITEM iSubs = m_treeGroups.InsertItem(SUBS, 0, 1, node);
			map->SetAt((void *)SUBS, iSubs);
			map->Lookup((void *)SUBS, (void *&)node);
			
		}


		CString strText = pSub->GetTrigger();
		HTREEITEM anItem = m_treeGroups.InsertItem(strText, 2, 2, node);
		m_mapSubs.SetAt(anItem, pSub);

	}

	HTREEITEM CActionListDlg::FindGroupNode( const std::string &group )
	{
		std::string trimmedGroup = trim( group );

		if(trimmedGroup.empty())
			trimmedGroup = "zz No Group zz";

		return GetGroup( trimmedGroup.c_str() );
	}

	HRESULT CActionListDlg::AddVarToTree( CVariable &v )
	{
		HRESULT hr = E_UNEXPECTED;

		try
		{
			HTREEITEM node = FindGroupNode( v.GetGroup() );
			ASSERT(node != NULL);
			if( NULL != node )
			{
				CMapPtrToPtr *map;

				BOOL bResult = 
					m_mapGroupNodeToEntityList.Lookup(node, (void *&)map);

				ASSERT(bResult);
				if( bResult )
				{
					bResult = map->Lookup((void *)VARS, (void *&)node);
					if(!bResult)
						{
							HTREEITEM iVars = m_treeGroups.InsertItem(VARS, 0, 1, node);
							map->SetAt((void *)VARS, iVars);
							map->Lookup((void *)VARS, (void *&)node);
						}
					HTREEITEM anItem = m_treeGroups.InsertItem( 
					v.GetName().c_str(), 2, 2, node );
					m_mapVars.SetAt(anItem, &v);

				}
			}

			hr = S_OK;
		}
		catch( _com_error &e )
		{
			hr = e.Error();
		}

		return hr;
	}

	void CActionListDlg::EditAction(CAction::PtrType action, HTREEITEM hItem)
	{
		CActionEditorDlg dlg;

		dlg.SetAction( action );

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			_actions.erase(hItem);

			// Delete it from the action list
			if( m_pActionList->Remove( action ) )
			{
				CAction::PtrType newAction;
				// Add the new action to the action list
				m_pActionList->Add(
					dlg.GetTrigger(), 
					dlg.GetAction(), 
					dlg.GetGroup(),
					newAction);

				newAction->SetEnabled( dlg.IsEnabled() );

				AddActionToTree( newAction );
			}
			else
			{
				AfxMessageBox(_T("Failed to edit the action."),
					MB_ICONERROR | MB_OK );
			}
		}
	}

	void CActionListDlg::EditAlias(CAlias::PtrType alias, HTREEITEM hItem)
	{
		CAliasEditorDlg dlg;

		dlg._alias = alias;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			_aliases.erase(hItem);

			// Delete it from the action list
			m_pAliasList->RemoveByDisplayName(alias->DisplayName());

			// Add the new action to the action list
			if( m_pAliasList->Add( dlg.m_strText, dlg.m_strAction, dlg.m_strGroup, alias ) )
			{
				// Disable the action if appropriate
				if(dlg.m_bDisabled)
					alias->enabled(false);

				AddAliasToTree(alias);
			}
		}
	}

	void CActionListDlg::EditArray(CMMArray * /*pArray*/, HTREEITEM /*hItem*/)
	{

	}

	void CActionListDlg::EditBarItem(CBarItem *pItem, HTREEITEM hItem)
	{
		CBarItemEditorDlg dlg(_options, this);

		dlg.m_pItem = pItem;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapBarItems.RemoveKey(hItem);

			// Delete it from the action list
			m_pBarItems->Remove((CScriptEntity *)pItem);


			// Add the new action to the action list
			if(dlg.m_bSeparator)
			{
				//AddSeparator(LPCSTR pszItem, int nPos,LPCSTR pszGroup)
				m_pBarItems->AddSeparator(dlg.m_strName, dlg.m_nPosition, dlg.m_strGroup);
			}
			else
			{
				// AddBarItem(LPCSTR pszItem, LPCSTR pszText, int nPos, int nLen, int nFore, int nBack, LPCSTR pszGroup)		
				m_pBarItems->AddBarItem(dlg.m_strName, dlg.m_strText, dlg.m_nPosition, dlg.m_nLength, dlg.m_nForeColor, dlg.m_nBackColor, dlg.m_strGroup);
			}

			// Find the pointer to the new action
			pItem = m_pBarItems->FindItem(dlg.m_strName);

			// Disable the action if appropriate
			if(dlg.m_bDisabled)
				pItem->Enabled(FALSE);

			AddBarItemToTree(pItem);
		}
	}

	void CActionListDlg::EditEvent(CMMEvent *pEvent, HTREEITEM hItem)
	{
		CEventEditorDlg dlg;

		dlg.m_pEvent = pEvent;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapEvents.RemoveKey(hItem);

			// Delete it from the action list
			m_pEventList->Remove((CScriptEntity *)pEvent);

			// Add the new action to the action list
			m_pEventList->Add(dlg.m_strName, dlg.m_nSeconds, dlg.m_strAction, dlg.m_strGroup);

			// Find the pointer to the new action
			pEvent = (CMMEvent *)m_pEventList->FindExact(dlg.m_strName);

			// Disable the action if appropriate
			if(dlg.m_bDisabled)
				pEvent->Enabled(FALSE);

			AddEventToTree(pEvent);
		}

	}

	void CActionListDlg::EditGag(CGag *pGag, HTREEITEM hItem)
	{
		CGagEditorDlg dlg;

		dlg.m_pGag = pGag;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapGags.RemoveKey(hItem);

			// Delete it from the action list
			m_pGagList->Remove(pGag);

			// Add the new action to the action list
			m_pGagList->Add(dlg.m_strMask, "", dlg.m_strGroup);

			// Find the pointer to the new action
			pGag = dynamic_cast<CGag *>(m_pGagList->FindExact(dlg.m_strMask));

			// Disable the action if appropriate
			if(dlg.m_bDisabled)
				pGag->Enabled(FALSE);

			AddGagToTree(pGag);
		}
	}

	void CActionListDlg::EditHigh(CHigh *pHigh, HTREEITEM hItem)
	{
		CHighEditorDlg dlg(_options, this);

		dlg.m_pHigh = pHigh;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapHighs.RemoveKey(hItem);

			// Delete it from the action list
			m_pHighList->Remove((CScriptEntity *)pHigh);

			CString strColor;
			strColor.Format("%s, back %s", COLOR_STRINGS[dlg.m_nFore], COLOR_STRINGS[dlg.m_nBack]);

			// Add the new action to the action list
			m_pHighList->Add(dlg.m_strMask, strColor, dlg.m_strGroup);

			// Find the pointer to the new action
			pHigh = (CHigh *)m_pHighList->FindExact(dlg.m_strMask);

			// Disable the action if appropriate
			if(dlg.m_bDisabled)
				pHigh->Enabled(FALSE);

			AddHighToTree(pHigh);
		}
	}

	void CActionListDlg::EditList(CMMList * /*pList*/, HTREEITEM /*hItem*/)
	{

	}

	void CActionListDlg::EditMacro(CMacro *pMacro, HTREEITEM hItem)
	{
		CMacroEditorDlg dlg;

		dlg.m_pMacro = pMacro;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapMacros.RemoveKey(hItem);

			// Delete it from the action list
			m_pMacroList->Remove((CScriptEntity *)pMacro);

			m_pMacroList->Add(dlg.m_strKey, dlg.m_strAction, dlg.m_strGroup, dlg.m_nDest);

			// Find the pointer to the new action
			pMacro = m_pMacroList->FindKeyByName(dlg.m_strKey);

			AddMacroToTree(pMacro);
		}
	}

	void CActionListDlg::EditSub(CSub *pSub, HTREEITEM hItem)
	{
		CSubEditorDlg dlg;

		dlg.m_pSub = pSub;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapSubs.RemoveKey(hItem);

			// Delete it from the action list
			m_pSubList->Remove(pSub);


			m_pSubList->Add(dlg.m_strMask, dlg.m_strSub, dlg.m_strGroup);

			// Find the pointer to the new action
			pSub = dynamic_cast<CSub *>(m_pSubList->FindExact(dlg.m_strMask));

			AddSubToTree(pSub);
		}

	}

	void CActionListDlg::EditVar(CVariable *pVar, HTREEITEM hItem)
	{
		CVarEditDlg dlg;

		dlg.m_pVar = pVar;

		if(dlg.DoModal() == IDOK)
		{
			// Remove this item from the tree
			m_treeGroups.DeleteItem(hItem);

			// Take it out of the items map
			m_mapBarItems.RemoveKey(hItem);

			// Delete it from the action list
			int nIndex = m_pVarList->Find(*pVar);
			ASSERT(-1 != nIndex);
			m_pVarList->RemoveAt(nIndex);

			std::string name = static_cast<LPCTSTR>( dlg.m_strName );
			std::string value = static_cast<LPCTSTR>( dlg.m_strValue );
			std::string group = static_cast<LPCTSTR>( dlg.m_strGroup );

			nIndex = m_pVarList->Add( name, value, group );

			CVariable &v = m_pVarList->GetAt(nIndex);

			AddVarToTree(v);
		}
	}

	void CActionListDlg::SetupLayout(void)
	{
		CPane rightPane = pane(VERTICAL)
			<< item(IDC_CLOSE, NORESIZE);

		CreateRoot(HORIZONTAL)
			<< item(IDC_ACTION_GROUPS)
			<< rightPane;

		UpdateLayout();
	}

	void CActionListDlg::OnBnClickedClose()
	{
		EndDialog(IDOK);
	}
}