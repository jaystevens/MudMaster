#include "stdafx.h"
#include ".\logfile.h"
#include "resource.h"

CLogFile::CLogFile(void)
: _logging(false)
, _timestamp(false)
, _bytesWritten(0)
{
}

CLogFile::~CLogFile(void)
{
}

CLogFile &CLogFile::Instance()
{
	static CLogFile file;
	return file;
}


HRESULT CLogFile::Open(
	LPCTSTR pszFilename,
	bool append)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		if(INVALID_HANDLE_VALUE != _file.m_h)
		{
			NTRACE(_T("Log is already open!"));
			TESTHR(E_FAIL);
		}

		DWORD disp = append ? OPEN_ALWAYS : CREATE_ALWAYS;

		TESTHR(_file.Create(
			pszFilename,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			disp));

		CString strLogOpenMessage;
		if(!strLogOpenMessage.LoadString(IDS_LOG_OPEN))
		{
			strLogOpenMessage.Format(_T("Log opened"));
		}

		if(append)
			TESTHR(_file.Seek(0, FILE_END));

		TESTHR(Log(strLogOpenMessage));

		_logging = true;

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}

HRESULT CLogFile::Close()
{
	if(!_logging)
		return S_OK;

	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString closeEntry;
		if(!closeEntry.LoadString(IDS_LOG_CLOSE))
		{
			NTRACE(_T("LoadString failed"));
			TESTHR(E_UNEXPECTED);
		}

		TESTHR(Log(closeEntry));
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}

HRESULT CLogFile::Log(LPCTSTR pszText)
{
	if(!_logging)
		return S_OK;

	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString logEntry;
		if(_timestamp)
		{
			CTime ts = CTime::GetCurrentTime();
			logEntry = ts.Format(_T("%c"));
			logEntry += _T(":: ");
		}

		logEntry += pszText;

		LPCTSTR pszLogEntry = logEntry;
		DWORD cbBytesWritten = 0;

		TESTHR(_file.Write(
			reinterpret_cast<LPVOID>(const_cast<LPSTR>(pszLogEntry)),
			logEntry.GetLength() * sizeof(TCHAR),
			&cbBytesWritten));
		
		_bytesWritten += cbBytesWritten;

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return hr;
}