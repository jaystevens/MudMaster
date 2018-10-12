// MMSession.cpp : Implementation of CMMSession

#include "stdafx.h"
#include "MMSession.h"
#include ".\mmsession.h"


// CMMSession

STDMETHODIMP CMMSession::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMMSession
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CMMSession::get_Verbatim(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	if(NULL == pVal)
	{
		return E_POINTER;
	}

	*pVal = this->_verbatim;

	return S_OK;
}

STDMETHODIMP CMMSession::put_Verbatim(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	// TODO: Add your implementation code here

	return S_OK;
}
