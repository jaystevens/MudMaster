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
// IPListDlg.cpp : implementation file
//

#pragma warning (disable : 4244)

#include "stdafx.h"
#include "IPListDlg.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPListDlg dialog

int CALLBACK IPCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

CIPListDlg::CIPListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_h_addr_list = NULL;
}


void CIPListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPListDlg)
	DDX_Control(pDX, IDC_IPLIST, m_lcIPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPListDlg, CDialog)
	//{{AFX_MSG_MAP(CIPListDlg)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_IPLIST, OnColumnclickIplist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPListDlg message handlers

BOOL CIPListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect;
	m_lcIPList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lcIPList.GetClientRect(&rect);
	m_lcIPList.InsertColumn(0,"IP Addresses",LVCFMT_LEFT,IPADDRESS_WIDTH);
	m_lcIPList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	m_nSortDirection = CGlobals::LISTSORT_ASCEND;
	FillList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CIPListDlg::OnColumnclickIplist(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	FlipSortDirection();
	m_lcIPList.SortItems(IPCompareFunc,m_nSortDirection);
	*pResult = 0;
}

void CIPListDlg::SetList( char ** list )
{
	ASSERT( NULL != list );
	if( NULL != list )
	{
		m_h_addr_list = list;
	}
}

long CIPListDlg::GetSelectedIp() const
{
	return m_IPSelected;
}

void CIPListDlg::FillList()
{
	CString strTrigger;

	m_lcIPList.DeleteAllItems();

	int nInsertIndex = 0;
	int nIndex = 0;
	while(m_h_addr_list[nIndex] != NULL)
	{
		in_addr in;
		in.s_addr = *(reinterpret_cast<LONG *>(m_h_addr_list[nIndex]));
		char *ipAddr = inet_ntoa( in );
		CA2T tAddr( ipAddr );

		nInsertIndex = m_lcIPList.InsertItem( 0, tAddr );

		m_lcIPList.SetItemData(
			nInsertIndex,
			reinterpret_cast<DWORD>( m_h_addr_list[nIndex] ));

		nIndex++;
	}

	m_lcIPList.SortItems(IPCompareFunc,m_nSortDirection);
	if(m_lcIPList.GetItemCount() > 0)
		m_lcIPList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

int CALLBACK IPCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{

	int nSortDir = lParamSort;
	char *ptr1 = (char *)lParam1;
	char *ptr2 = (char *)lParam2;

	if (nSortDir == CGlobals::LISTSORT_ASCEND)
		return(strcmp(ptr1,ptr2));
	else
		return(strcmp(ptr1,ptr2));
}

void CIPListDlg::FlipSortDirection()
{
	if (m_nSortDirection == CGlobals::LISTSORT_ASCEND)
		m_nSortDirection = CGlobals::LISTSORT_DESCEND;
	else
		m_nSortDirection = CGlobals::LISTSORT_ASCEND;
}

void CIPListDlg::OnOK() 
{
	int nIndex = m_lcIPList.GetNextItem(-1,LVNI_SELECTED);
	if (nIndex == -1)
	{
		AfxMessageBox("Please select an IP Address.");
		return;
	}

	DWORD dwData = m_lcIPList.GetItemData( nIndex );

	m_IPSelected = *reinterpret_cast<LONG *>(dwData);
	
	CDialog::OnOK();
}
