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
// MyInputControl.cpp : implementation file
//

#include "stdafx.h"
#include "ifx.h"
#include "MyInputControl.h"
#include "oledlg.h"
#include "EventArgs/SetEditTextEventArgs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyInputControl

CMyInputControl::CMyInputControl()
{
}

CMyInputControl::~CMyInputControl()
{
}


BEGIN_MESSAGE_MAP(CMyInputControl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CMyInputControl)
	ON_WM_PARENTNOTIFY_REFLECT()
	ON_WM_CONTEXTMENU()
	ON_WM_INITMENUPOPUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_INPUTCONTEXT_CUT, OnInputcontextCut)
	ON_UPDATE_COMMAND_UI(ID_INPUTCONTEXT_CUT, OnUpdateInputcontextCut)
	ON_COMMAND(ID_INPUTCONTEXT_COPY, OnInputcontextCopy)
	ON_UPDATE_COMMAND_UI(ID_INPUTCONTEXT_COPY, OnUpdateInputcontextCopy)
	ON_COMMAND(ID_INPUTCONTEXT_PASTE, OnInputcontextPaste)
	ON_UPDATE_COMMAND_UI(ID_INPUTCONTEXT_PASTE, OnUpdateInputcontextPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CMyInputControl, CRichEditCtrl)
	INTERFACE_PART(CMyInputControl, IID_IRichEditOleCallback, Callback)
END_INTERFACE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMyInputControl message handlers

BOOL CMyInputControl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_MOUSEWHEEL)
	{
		return GetParentFrame()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return CRichEditCtrl::PreTranslateMessage(pMsg);
}

void CMyInputControl::ParentNotify(UINT /*message*/, LPARAM /*lParam*/) 
{
}

void CMyInputControl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/) 
{
	
}

void CMyInputControl::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CRichEditCtrl::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	// TODO: Add your message handler code here
	
}

int CMyInputControl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	VERIFY(SetOLECallback(&m_xCallback));

	return 0;
}

#undef m_dwRef
#undef m_pOuterUnknown
#undef InternalQueryInterface
#undef InternalAddRef
#undef InternalRelease

STDMETHODIMP CMyInputControl::XCallback::QueryInterface(REFIID iid, void **ppvObj)
{
	METHOD_PROLOGUE_EX_(CMyInputControl, Callback)
	return (HRESULT)pThis->InternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP_(ULONG) CMyInputControl::XCallback::AddRef(void)
{
	METHOD_PROLOGUE_EX_(CMyInputControl, Callback)
	return (ULONG)pThis->InternalAddRef();
}

STDMETHODIMP_(ULONG) CMyInputControl::XCallback::Release(void)
{
	METHOD_PROLOGUE_EX_(CMyInputControl, Callback)
	return (ULONG)pThis->InternalRelease();
}

STDMETHODIMP CMyInputControl::XCallback::GetNewStorage(struct IStorage **)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::GetInPlaceContext(struct IOleInPlaceFrame **, struct IOleInPlaceUIWindow **, struct tagOIFI *)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::ShowContainerUI(int)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::QueryInsertObject(
	LPCLSID /*lpclsid*/, LPSTORAGE /*lpstg*/, LONG /*cp*/)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::DeleteObject(struct IOleObject *)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::QueryAcceptData(struct IDataObject *, unsigned short *, unsigned long, int, void *)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::ContextSensitiveHelp(int)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::GetClipboardData(
	CHARRANGE FAR * /*lpchrg*/, DWORD /*reco*/, LPDATAOBJECT FAR * /*lplpdataobj*/)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::GetDragDropEffect(int, unsigned long, unsigned long *)
{
	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CMyInputControl::XCallback::GetContextMenu(
	WORD /*seltype*/, LPOLEOBJECT lpoleobj, CHARRANGE FAR * lpchrg, HMENU FAR * lphmenu)
{
	METHOD_PROLOGUE_EX_(CMyInputControl, Callback)
	HMENU hmenu = NULL;
	HMENU hmenuParent = NULL;
	HMENU hmenuVerbs = NULL;
	UINT uiT;

	hmenuParent = CreatePopupMenu();
	if(hmenuParent==NULL)
	{
		if(hmenu == hmenuVerbs && hmenuParent)
			DestroyMenu(hmenuParent);
		else if(hmenu == hmenuParent && hmenuVerbs)
			DestroyMenu(hmenuVerbs);
	}

	if(lpoleobj != NULL)
	{
		/*
		OleUIAddVerbMenu(
			lpoleobj,				//Pointer to the object
			NULL,					//Pointer to the short name corresponding to the object
			hmenuParent,			//Handle to the menu to modify
			0,						//Position of the menu item
			IDR_INPUTMENU,			//Value at which to start the verbs
			0,						//Maximum identifier value for object verbs
			TRUE,					//whether to add convert item
			IDM_OBJECTCONVERT,		//Value to use for convert item
			&hmenuVerbs);			//Pointer to the cascading verb menu if created
		*/
	}

	*lphmenu = hmenu = hmenuVerbs ? hmenuVerbs : hmenuParent;

	if(lpoleobj)
		AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);

	uiT = MF_STRING;
	uiT |= lpchrg->cpMin == lpchrg->cpMax ? MF_GRAYED : MF_ENABLED;
	AppendMenu(hmenu, uiT, ID_INPUTCONTEXT_CUT, "Cut");
	AppendMenu(hmenu, uiT, ID_INPUTCONTEXT_COPY, "Copy");

	uiT = MF_STRING;
	uiT |= pThis->CanPaste() ? MF_ENABLED : MF_GRAYED;
	AppendMenu(hmenu, uiT, ID_INPUTCONTEXT_PASTE, "Paste");

	//TODO:KW just testing popup menu options
	AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hmenu, uiT, ID_ALIAS_TREE, "Show Aliases");
	
	//TODO:KW end test
	if(hmenu == hmenuVerbs && hmenuParent)
		DestroyMenu(hmenuParent);
	else if(hmenu == hmenuParent && hmenuVerbs)
		DestroyMenu(hmenuVerbs);

	return S_OK;
}


void CMyInputControl::OnDestroy() 
{
	CRichEditCtrl::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CMyInputControl::OnInputcontextCut() 
{
	// TODO: Add your command handler code here
	
}

void CMyInputControl::OnUpdateInputcontextCut(CCmdUI* /*pCmdUI*/) 
{
}

void CMyInputControl::OnInputcontextCopy() 
{
	// TODO: Add your command handler code here
	
}

void CMyInputControl::OnUpdateInputcontextCopy(CCmdUI* /*pCmdUI*/) 
{
}

void CMyInputControl::OnInputcontextPaste() 
{
	// TODO: Add your command handler code here

}

void CMyInputControl::OnUpdateInputcontextPaste(CCmdUI* /*pCmdUI*/) 
{

}

void CMyInputControl::OnSetEditText(SetEditTextEventArgs *args)
{
	ReplaceSel(args->GetText());
}

void CMyInputControl::notify(EventType type, EventArgs *args)
{
	switch(type)
	{
	case eSetEditText:
		{
			SetEditTextEventArgs *pArgs = dynamic_cast<SetEditTextEventArgs *>(args);
			OnSetEditText(pArgs);
		}
		break;
	default:
		TRACE(_T("Unknown event type!"));
	}
}

void CMyInputControl::UpdateAllOptions(SerializedOptions::CInputControlOptions &options)
{
	CHARFORMAT *pFormat = options.GetInputFormat();
	SetDefaultCharFormat(*pFormat);
	SetSelectionCharFormat(*pFormat);
	SetBackgroundColor(FALSE, options.GetBackColor());
	
	if(options.ShowBorder())
		ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED);
	else
		ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_FRAMECHANGED);

	RequestResize();
}