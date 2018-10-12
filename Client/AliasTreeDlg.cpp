#include "stdafx.h"
#include "ifx.h"
#include ".\aliastreedlg.h"
using namespace EditorDialogs;

CAliasTreeDlg::CAliasTreeDlg(COptions &treeoptions, CSession* treepParent )
:CActionListDlg(treeoptions,treepParent)
{
	
}

CAliasTreeDlg::~CAliasTreeDlg(void)
{

}
BOOL CAliasTreeDlg::OnInitDialog() 
	{
		AddAliases();
		

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
void CAliasTreeDlg::AddAliases()
	{
		CActionListDlg::AddAliases();
	}

