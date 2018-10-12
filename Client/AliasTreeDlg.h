#pragma once
#include <map>
#include "actionlistdlg.h"
#include "ETSLayout.h"

namespace EditorDialogs
{

	class CAliasTreeDlg
		:public CActionListDlg
	{
	public:
		CAliasTreeDlg(COptions &options, CSession* pParent /*= NULL*/);   // standard constructor
		enum { IDD = IDD_ACTION_LIST }; //114
		/** a pointer to the list of aliases currently in use by the script */
		MMScript::CAliasList * m_pAliasList;
		virtual ~CAliasTreeDlg(void);
	protected:
		virtual	BOOL OnInitDialog();

		CSession* m_pParent;
	private:		
		typedef std::map<HTREEITEM, MMScript::CAlias::PtrType> AliasMap;
		AliasMap _aliases;
		HTREEITEM FindGroupNode( const std::string &group );
		void EditAlias(MMScript::CAlias::PtrType alias, HTREEITEM hItem);
		void AddAliasToTree(MMScript::CAlias::PtrType alias);
		CMapPtrToPtr m_mapGroupNodeToEntityList;
		HTREEITEM GetGroup( LPCTSTR groupName );
		virtual void AddAliases();
		CTreeCtrl	m_treeGroups;
		CImageList m_ilImageList;
		HTREEITEM m_hRoot;



	};
}