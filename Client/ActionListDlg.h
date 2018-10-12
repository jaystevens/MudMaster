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

/**
 *\file actionlistdlg.h
 * header file for the action list dialog box
 * This is the header for the action list dialog box.  Although this class is named
 * action list, it is actually the script editor that manages all the script entities.
 */

#include <map>
#include "ETSLayout.h"
#include "AliasList.h"
#include "UserLists.h"	// Added by ClassView
#include "TabList.h"	// Added by ClassView
#include "Action.h"

namespace MMScript
{
	class CVarList;
	class CSubList;
	class CMMStatusBar;
	class CHighList;
	class CMacroList;
	class CArrayList;
	class CEventList;
	class CAlias;
	class CGag;
	class CActionList;
	class CGagList;
	class CMMArray;
	class CBarItem;
	class CVariable;
	class CSub;
	class CMacro;
	class CHigh;
	class CMMEvent;
}

class COptions;

/** 
* \brief Contains all of the Entity Editor Dialogs
*/
namespace EditorDialogs
{
	/**
	* \class CActionListDlg actionlistdlg.h
	* \brief Wrapper for the ScriptEntity Editor Dialog.
	*/
	class CActionListDlg 
		: public ETSLayoutDialog
	{
		static LPCTSTR ACTIONS;
		static LPCTSTR ALIASES;
		static LPCTSTR LISTS;
		static LPCTSTR ARRAYS;
		static LPCTSTR EVENTS;
		static LPCTSTR GAGS;
		static LPCTSTR HIGHS;
		static LPCTSTR MACROS;
		static LPCTSTR BARITEMS;
		static LPCTSTR SUBS;
		static LPCTSTR TABS;
		static LPCTSTR VARS;

		COptions &_options;
		// Construction
	public:
		/** pointer to the list of variables currently in use by the script */
		MMScript::CVarList * m_pVarList;

		/** a pointer to the list of substitutions currently in use by the script */
		MMScript::CSubList * m_pSubList;

		/** a pointer to the list of status bar items currently in use by the script */
		MMScript::CMMStatusBar * m_pBarItems;

		/** a pointer to the list of macros currently in use by the script */
		MMScript::CMacroList * m_pMacroList;

		/** a pointer to the list of highlights currently in use by the script */
		MMScript::CHighList * m_pHighList;

		/** a pointer to the list of gags currently in use by the script */
		MMScript::CGagList * m_pGagList;

		/** a pointer to the list of events currently in use by the script */
		MMScript::CEventList * m_pEventList;

		/** a pointer to the list of arrays currently in use by the script */
		MMScript::CArrayList * m_pArrayList;

		/** a pointer to the list of lists currently in use by the script */
		MMScript::CUserLists *m_pLists;

		/** a pointer to the list of aliases currently in use by the script */
		MMScript::CAliasList * m_pAliasList;

		/** a pointer to the list of actions currently in use by the script */
		MMScript::CActionList * m_pActionList;

		/**
		  * The constructor for the ActionListDialog class wrapper
		  * \param pParent is a pointer that represents the window that owns this dialog box
		  * \param options reference to the current MudMaster settings
		  */
		CActionListDlg(COptions &options, CSession* pParent /*= NULL*/);   // standard constructor

		enum { IDD = IDD_ACTION_LIST }; //114
		CTreeCtrl	m_treeGroups;
		void AddAliases();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
	protected:
		virtual BOOL OnInitDialog();

		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnDblclkActionGroups(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnRclkActionGroups(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDestroy();

		DECLARE_MESSAGE_MAP()
	private:
		CSession* m_pParent;
		/**
		  * \brief given a group name, will return the associated HTREEITEM
		  * \param group the name of the group to find
		  * \return the associated tree item
		  */
		HTREEITEM FindGroupNode( const std::string &group );

		/**
		* \brief Takes a list of variables and hItem, finds the item in the 
		*        tree and displays an editor for the variable.
		* \param pVar a pointer to the list of variables in use by the script
		* \param hItem an HTREEITEM that identifies the item in the treeview 
		*        that is currently selected
		* \sa EditSub(), EditMacro(), EditList(), EditHigh(), EditGag(), 
		*     EditEvent(), EditBarItem(), EditArray(), EditAlias(), 
		*     EditAction()
		*/
		void EditVar(MMScript::CVariable *pVar, HTREEITEM hItem);

		/**
		* \brief Takes a list of substitutions and hItem, finds the item in 
		*        the tree and displays an editor for the substitution.
		* \param pVar a pointer to the list of variables in use by the script
		* \param hItem an HTREEITEM that identifies the item in the treeview 
		*        that is currently selected
		* \sa EditVar(), EditMacro(), EditList(), EditHigh(), EditGag(), 
		*     EditEvent(), EditBarItem(), EditArray(), EditAlias(), 
		*     EditAction()
		*/
		void EditSub(MMScript::CSub *pSub, HTREEITEM hItem);

		/**
		  * \brief Takes a list of macros and an hItem, finds the item in
		  *        the tree and displays an editor for that macro.
		  * \param pMacro a pointer to the list of macros in use by the script.
		  * \param hItem an HTREEITEM that identifies the item in the treeview
		  *        that is currently selected.
		  * \sa EditVar(), EditSub(), EditList(), EditHigh(), EditGag(),
		  *     EditEvent(), EditBarItem(), EditArray(), EditAlias(),
		  *     EditAction()
		  */
		void EditMacro(MMScript::CMacro *pMacro, HTREEITEM hItem);

		/**
		  * \brief Takes a list of MM2K lists and an hItem, finds the list
		  *        in the tree and displays an editor for that list
		  * \param pList a pointer to a list of Lists in use by the script.
		  * \param hItem an HTREEITEM that identifies the item in the treeview
		  *        that is currently selected.
		  * \sa EditVar(), EditSub(), EditMacro(), EditHigh(), EditGag(),
		  *     EditEvent(), EditBarItem(), EditArray(), EditAlias(),
		  *     EditAction()
		  */
		void EditList(MMScript::CMMList *pList, HTREEITEM hItem);

		/**
		  * \brief Takes a list of Highlights and an hItem, finds the 
		  *        highlight in the tree and displays the editor for that
		  *        highlight
		  * \param pHigh a pointer to a list of highlights
		  * \param hItem an HTREEITEM that identifies the item in the
		  *        treeview that is currently selected
		  * \sa EditVar(), EditSub(), EditMacro(), EditList(), EditGag()
		  *     EditEvent(), EditBarItem(), EditArray(), EditAlias(),
		  *     EditAction()
		  */
		void EditHigh(MMScript::CHigh *pHigh, HTREEITEM hItem);

		/**
		  * \brief Takes a list of gags and an hItem, finds the gag in the
		  *        tree and displays the editor for that gag
		  * \param pGag a pointer to a list of gags
		  * \param hItem an HTREEITEM that identifies the item in the
		  *        treeview that is currently selected
		  * \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		  *     EditEvent(), EditBarItem(), EditArray(), EditAlias(),
		  *     EditAction()
		  */
		void EditGag(MMScript::CGag *pGag, HTREEITEM hItem);

		/**
		  * \brief Takes a list of events and an hItem, finds the event in
		  *        the tree and displays the editor for that event.
		  * \param pEvent a pointer to a list of events
		  * \param hItem an HTREEITEM that identifies the item in the
		  *        treeview that is currently selected
		  * \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		  *     EditGag(), EditBarItem(), EditArray(), EditAlias(),
		  *     EditAction()
		  */
		void EditEvent(MMScript::CMMEvent *pEvent, HTREEITEM hItem);

		/**
		* \brief Takes a list of baritems and an hItem, finds the baritem in
		*        the tree and displays the editor for that baritem.
		* \param pEvent a pointer to a list of baritems
		* \param hItem an HTREEITEM that identifies the item in the
		*        treeview that is currently selected
		* \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		*     EditGag(), EditEvent(), EditArray(), EditAlias(),
		*     EditAction()
		*/
		void EditBarItem(MMScript::CBarItem *pItem, HTREEITEM hItem);

		/**
		* \brief Takes a list of Arrays and an hItem, finds the Array in
		*        the tree and displays the editor for that Array.
		* \param pEvent a pointer to a list of Arrays
		* \param hItem an HTREEITEM that identifies the item in the
		*        treeview that is currently selected
		* \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		*     EditGag(), EditEvent(), EditBarItem(), EditAlias(),
		*     EditAction()
		*/
		void EditArray(MMScript::CMMArray *pArray, HTREEITEM hItem);

		/**
		* \brief Takes a list of Aliases and an hItem, finds the Alias in
		*        the tree and displays the editor for that Alias.
		* \param pEvent a pointer to a list of Aliases
		* \param hItem an HTREEITEM that identifies the item in the
		*        treeview that is currently selected
		* \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		*     EditGag(), EditEvent(), EditBarItem(), EditArray(),
		*     EditAction()
		*/
		void EditAlias(MMScript::CAlias::PtrType alias, HTREEITEM hItem);

		/**
		* \brief Takes a list of Actions and an hItem, finds the Action in
		*        the tree and displays the editor for that Action.
		* \param pEvent a pointer to a list of Actions
		* \param hItem an HTREEITEM that identifies the item in the
		*        treeview that is currently selected
		* \sa EditVar(), EditSub(), EditMacro(), EditList(), EditHigh(),
		*     EditGag(), EditEvent(), EditBarItem(), EditArray(),
		*     EditAlias()
		*/
		void EditAction(MMScript::CAction::PtrType action, HTREEITEM hItem);


		HRESULT AddVarToTree( MMScript::CVariable &v );
		void AddSubToTree(MMScript::CSub *pSub);
		void AddMacroToTree(MMScript::CMacro *pMacro);
		void AddListToTree(MMScript::CMMList *pList);
		void AddHighToTree(MMScript::CHigh *pHigh);
		void AddGagToTree(MMScript::CGag *pGag);
		void AddEventToTree(MMScript::CMMEvent *pEvent);
		void AddVars();
		void AddSubs();
		void AddMacros();
		void AddLists();
		void AddHighs();
		void AddGags();
		void AddEvents();
		void AddBarItemToTree(MMScript::CBarItem *pItem);
		void AddBarItems();
		void AddArrayToTree(MMScript::CMMArray *pArray);
		void AddArrays();
		void AddAliasToTree(MMScript::CAlias::PtrType alias);
		CMapPtrToPtr m_mapGroupNodeToEntityList;
		HTREEITEM GetGroup( LPCTSTR groupName );
		void AddActions();
		void AddActionToTree(MMScript::CAction::PtrType action);
		CImageList m_ilImageList;
		HTREEITEM m_hRoot;
		CMapStringToPtr m_mapGroupToGroupNode;


		typedef std::map<HTREEITEM, MMScript::CAction::PtrType> ActionMap;
		ActionMap _actions;

		typedef std::map<HTREEITEM, MMScript::CAlias::PtrType> AliasMap;
		AliasMap _aliases;

		CMapPtrToPtr m_mapArrays;
		CMapPtrToPtr m_mapBarItems;
		CMapPtrToPtr m_mapEvents;
		CMapPtrToPtr m_mapGags;
		CMapPtrToPtr m_mapHighs;
		CMapPtrToPtr m_mapLists;
		CMapPtrToPtr m_mapMacros;
		CMapPtrToPtr m_mapSubs;
		CMapPtrToPtr m_mapVars;
	public:
		bool HandleAction( HTREEITEM hItem );

		void SetupLayout(void);
		afx_msg void OnBnClickedClose();
	};
}