// ChatTree.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "ChatTree.h"
#include "Chat.h"
#include "ChatServer.h"
#include "Sess.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatTree

CChatTree::CChatTree()
{
	m_tiRoot = NULL;
}

CChatTree::~CChatTree()
{
}


BEGIN_MESSAGE_MAP(CChatTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CChatTree)
	ON_NOTIFY_REFLECT(TVN_GETINFOTIP, OnGetInfoTip)
	ON_COMMAND(ID_POPUP_SENDFILE, OnPopupSendfile)
	ON_COMMAND(ID_POPUP_ALLOWCOMMANDS, OnPopupAllowcommands)
	ON_COMMAND(ID_POPUP_ALLOWSNOOP, OnPopupAllowsnoop)
	ON_COMMAND(ID_POPUP_ALLOWTRANSFERS, OnPopupAllowtransfers)
	ON_COMMAND(ID_POPUP_IGNORE, OnPopupIgnore)
	ON_COMMAND(ID_POPUP_MARKPRIVATE, OnPopupMarkprivate)
	ON_COMMAND(ID_POPUP_PING, OnPopupPing)
	ON_COMMAND(ID_POPUP_SERVE, OnPopupServe)
	ON_COMMAND(ID_POPUP_SNOOP, OnPopupSnoop)
	ON_COMMAND(ID_POPUP_UNCHAT, OnPopupUnchat)
	ON_UPDATE_COMMAND_UI(ID_POPUP_ALLOWCOMMANDS, OnUpdatePopupAllowcommands)
	ON_UPDATE_COMMAND_UI(ID_POPUP_SENDFILE, OnUpdatePopupSendfile)
	ON_UPDATE_COMMAND_UI(ID_POPUP_UNCHAT, OnUpdatePopupUnchat)
	ON_UPDATE_COMMAND_UI(ID_POPUP_SNOOP, OnUpdatePopupSnoop)
	ON_UPDATE_COMMAND_UI(ID_POPUP_SERVE, OnUpdatePopupServe)
	ON_UPDATE_COMMAND_UI(ID_POPUP_PING, OnUpdatePopupPing)
	ON_UPDATE_COMMAND_UI(ID_POPUP_MARKPRIVATE, OnUpdatePopupMarkprivate)
	ON_UPDATE_COMMAND_UI(ID_POPUP_IGNORE, OnUpdatePopupIgnore)
	ON_UPDATE_COMMAND_UI(ID_POPUP_ALLOWTRANSFERS, OnUpdatePopupAllowtransfers)
	ON_UPDATE_COMMAND_UI(ID_POPUP_ALLOWSNOOP, OnUpdatePopupAllowsnoop)
	ON_COMMAND(ID_POPUP_REQUESTCONNECTIONS, OnPopupRequestconnections)
	ON_UPDATE_COMMAND_UI(ID_POPUP_REQUESTCONNECTIONS, OnUpdatePopupRequestconnections)
	ON_COMMAND(ID_POPUP_PEEKCONNECTIONS, OnPopupPeekconnections)
	ON_UPDATE_COMMAND_UI(ID_POPUP_PEEKCONNECTIONS, OnUpdatePopupPeekconnections)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatTree message handlers

void CChatTree::SetRootItem(HTREEITEM hti)
{
	m_tiRoot = hti;
}

HTREEITEM CChatTree::AddSession(const char *pszSessionName)
{
	return InsertItem(pszSessionName, m_tiRoot);
}

HTREEITEM CChatTree::AddChatConnection(const char *pszChatName, HTREEITEM ti)
{
	return InsertItem(pszChatName, ti);
}

void CChatTree::SetImage(UINT /*nImage*/)
{
	m_image.Create(IDB_CHAT_TREE_BMP, 16, 1, RGB (255, 0, 255));
	SetImageList(&m_image, TVSIL_NORMAL);
}

void CChatTree::OnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVGETINFOTIP *pNMTVGETINFOTIP = (LPNMTVGETINFOTIP)pNMHDR;
	if(pNMTVGETINFOTIP->hItem == m_tiRoot)
		return;

	if(GetParentItem(pNMTVGETINFOTIP->hItem) == m_tiRoot)
	{
		CChatServer *pServer = reinterpret_cast<CChatServer *>(pNMTVGETINFOTIP->lParam);
		CString strTip;
		strTip.Format("%d connections.", pServer->GetCount());
		strcpy(pNMTVGETINFOTIP->pszText, strTip);
		return;
	}

	CChat *pChat = reinterpret_cast<CChat *>(pNMTVGETINFOTIP->lParam);
	CString strTip;
	strTip.Format(
		"Chat Name:	%s\n"
		"IP Address: %s Port: %d\n"
		"Client Type: %s\n"
		"---------------------------------------------------------------------\n"
		"Can Snoop you:	%s\n"
		"You are snooping: %s\n"
		"Can send you commands: %s\n"
		"You are ignoring: %s\n"
		"Marked private: %s\n"
		"Served: %s\n"
		"Can send you files: %s\n",
		pChat->GetName(),
		pChat->GetAddress(),
		pChat->GetPort(),
		pChat->GetVersion(),
		pChat->GetAllowSnoop() ? "Yes" : "No",
		pChat->GetSnooped() ? "Yes" : "No",
		pChat->GetCommands() ? "Yes" : "No",
		pChat->GetIgnore() ? "Yes" : "No",
		pChat->GetPrivate() ? "Yes" : "No",
		pChat->GetServe() ? "Yes" : "No",
		pChat->GetTransfers() ? "Yes" : "No"
		);

	strcpy(pNMTVGETINFOTIP->pszText, strTip);


	*pResult = 0;
}

BOOL CChatTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_INFOTIP;	
	return CTreeCtrl::PreCreateWindow(cs);
}

HTREEITEM CChatTree::GetRootItem()
{
	return m_tiRoot;
}

void CChatTree::OnPopupAllowcommands() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%schatc %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);			
}

void CChatTree::OnUpdatePopupAllowcommands(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetCommands());				
}

BOOL CChatTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT (pNMHDR != NULL);

	if (pNMHDR->code == TTN_SHOW && GetToolTips () != NULL)
	{
		GetToolTips ()->SetWindowPos (&wndTop, -1, -1, -1, 	-1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}
	
	return bRes;
	
}


void CChatTree::OnUpdatePopupSendfile(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->Enable(TRUE);			
}

void CChatTree::OnPopupSendfile() 
{
	//CChat *pChat = NULL;
	//CChatServer *pChatServer = NULL;
	//BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	//if(!bResult)
	//	return;

	//CFileDialog dlg(TRUE);

	//int nRes = dlg.DoModal();

	//if(nRes != IDOK)
	//	return;

	//CSession *pSession = pChatServer->GetSession();

	//CString strFileName;
	//strFileName = dlg.GetPathName();

	//pSession->GetChat().SendFile(pChat, strFileName);	
}

CChat * CChatTree::UpdatePopup(CCmdUI *pCmdUI)
{
	HTREEITEM ccItem = GetSelectedItem();
	if(ccItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return NULL;
	}

	CChat *pChat = reinterpret_cast<CChat *>(GetItemData(ccItem));
	if(pChat == NULL)
	{
		pCmdUI->Enable(FALSE);
		return NULL;
	}

	if(GetParentItem(ccItem) == GetRootItem())
	{
		pCmdUI->Enable(FALSE);
		return NULL;
	}

	if(ccItem == GetRootItem())
	{
		pCmdUI->Enable(FALSE);
		return NULL;
	}

	return pChat;
}

BOOL CChatTree::GetChatAndServer(CChat **ppChat, CChatServer **ppChatServer)
{
	HTREEITEM ccItem = GetSelectedItem();
	if(ccItem == NULL)
	{
		AfxMessageBox("Cannot find selected item.");
		return FALSE;
	}

	if(GetParentItem(ccItem) == GetRootItem())
	{
		AfxMessageBox("Please select the chat connection for this command.");
		return FALSE;
	}

	if(ccItem == GetRootItem())
	{
		AfxMessageBox("Please select a chat connection for this command.");
		return FALSE;
	}

	HTREEITEM csItem = GetParentItem(ccItem);
	if(csItem == NULL)
	{
		AfxMessageBox("Cannot find server for selected chat connection");
		return FALSE;
	}

	*ppChat = reinterpret_cast<CChat *>(GetItemData(ccItem));
	if(*ppChat == NULL)
	{
		AfxMessageBox("Cannot find selected chat connection");
		return FALSE;
	}

	*ppChatServer = reinterpret_cast<CChatServer *>(GetItemData(csItem));
	if(*ppChatServer == NULL)
	{
		AfxMessageBox("Cannot find selected chat connection");
		return FALSE;
	}

	return TRUE;
}

void CChatTree::OnPopupAllowsnoop() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cchatsnoop %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);	
}

void CChatTree::OnPopupAllowtransfers() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cchatt %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);
}

void CChatTree::OnPopupIgnore() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cchati %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);
}

void CChatTree::OnPopupMarkprivate() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cchatp %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);
}

void CChatTree::OnPopupPing() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cping %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);	
}

void CChatTree::OnPopupServe() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cchatserve %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);			
}

void CChatTree::OnPopupSnoop() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%csnoop %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);		
}

void CChatTree::OnPopupUnchat() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cunchat %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);	
}

void CChatTree::OnUpdatePopupUnchat(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;
	pCmdUI->Enable(TRUE);	
}

void CChatTree::OnUpdatePopupSnoop(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetSnooped());					
}

void CChatTree::OnUpdatePopupServe(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetServe());						
}

void CChatTree::OnUpdatePopupPing(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->Enable(TRUE);	
}

void CChatTree::OnUpdatePopupMarkprivate(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetPrivate());
}

void CChatTree::OnUpdatePopupIgnore(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetIgnore());			
}

void CChatTree::OnUpdatePopupAllowtransfers(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetTransfers());		
}

void CChatTree::OnUpdatePopupAllowsnoop(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->SetCheck(pChat->GetAllowSnoop());				
}

void CChatTree::OnPopupRequestconnections() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%crequest %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);	
}

void CChatTree::OnUpdatePopupRequestconnections(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->Enable(TRUE);	
}

void CChatTree::OnPopupPeekconnections() 
{
	CChat *pChat = NULL;
	CChatServer *pChatServer = NULL;
	BOOL bResult = GetChatAndServer(&pChat, &pChatServer);
	if(!bResult)
		return;

	CSession *pSession = pChatServer->GetSession();

	CString strCommand;
	strCommand.Format("%cpeek %s", 
		CGlobals::GetCommandCharacter(),
		pChat->GetName());
	pSession->ExecuteCommand(strCommand);	
}

void CChatTree::OnUpdatePopupPeekconnections(CCmdUI* pCmdUI) 
{
	CChat *pChat = UpdatePopup(pCmdUI);
	if(pChat == NULL)
		return;

	pCmdUI->Enable(TRUE);	
}
