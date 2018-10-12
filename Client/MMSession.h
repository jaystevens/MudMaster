// MMSession.h : Declaration of the CMMSession

#pragma once
#include "resource.h"       // main symbols

#include "Ifx.h"


// CMMSession

class ATL_NO_VTABLE CMMSession : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMMSession, &CLSID_MMSession>,
	public ISupportErrorInfo,
	public IDispatchImpl<IMMSession, &IID_IMMSession, &LIBID_IfxLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	VARIANT_BOOL _verbatim;

public:
	CMMSession()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MMSESSION)


BEGIN_COM_MAP(CMMSession)
	COM_INTERFACE_ENTRY(IMMSession)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		_verbatim = VARIANT_FALSE;
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(get_Verbatim)(VARIANT_BOOL* pVal);
	STDMETHOD(put_Verbatim)(VARIANT_BOOL newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(MMSession), CMMSession)
