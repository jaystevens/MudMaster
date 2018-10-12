#if !defined(AFX_CHATTREE_H__ADD989A1_2A67_11D4_BCD6_00E029482496__INCLUDED_)
#define AFX_CHATTREE_H__ADD989A1_2A67_11D4_BCD6_00E029482496__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChatTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChatTree window
class CChat;
class CChatServer;

class CChatTree : public CTreeCtrl
{
// Construction
public:
	CChatTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	HTREEITEM GetRootItem();
	void SetImage(UINT nImage);
	HTREEITEM AddChatConnection(const char *pszChatName, HTREEITEM ti);
	HTREEITEM AddSession(const char *pszSessionName);
	void SetRootItem(HTREEITEM hti);
	virtual ~CChatTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChatTree)
	afx_msg void OnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPopupSendfile();
	afx_msg void OnPopupAllowcommands();
	afx_msg void OnPopupAllowsnoop();
	afx_msg void OnPopupAllowtransfers();
	afx_msg void OnPopupIgnore();
	afx_msg void OnPopupMarkprivate();
	afx_msg void OnPopupPing();
	afx_msg void OnPopupServe();
	afx_msg void OnPopupSnoop();
	afx_msg void OnPopupUnchat();
	afx_msg void OnUpdatePopupAllowcommands(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupSendfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupUnchat(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupSnoop(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupServe(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupPing(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupMarkprivate(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupIgnore(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupAllowtransfers(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopupAllowsnoop(CCmdUI* pCmdUI);
	afx_msg void OnPopupRequestconnections();
	afx_msg void OnUpdatePopupRequestconnections(CCmdUI* pCmdUI);
	afx_msg void OnPopupPeekconnections();
	afx_msg void OnUpdatePopupPeekconnections(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CChat *UpdatePopup(CCmdUI *pCmdUI);
	BOOL GetChatAndServer(CChat **ppChat, CChatServer **ppChatServer);
	CImageList m_image;
	HTREEITEM m_tiRoot;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHATTREE_H__ADD989A1_2A67_11D4_BCD6_00E029482496__INCLUDED_)
