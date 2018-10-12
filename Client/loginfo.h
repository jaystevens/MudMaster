// loginfo.h
//
// Yarantsau Andrei, QA
// Minsk, Belarus
//
#ifndef LOGINFO_H
#define LOGINFO_H
//TODO: KW compiling without precompiled headers causes an error because of this
//#include <windows.h>

class CLogfile
{
public:
	CLogfile(char* filename) 
	{
		m_hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS , NULL, NULL );
		InitializeCriticalSection(&m_CriticalSection); 
	};
	~CLogfile() 
	{ 
		if (NULL != m_hFile)
			CloseHandle(m_hFile); 
		DeleteCriticalSection(&m_CriticalSection);
	};
	void print(LPCTSTR lpszFormat, ...)
	{
		EnterCriticalSection(&m_CriticalSection);

		va_list args;
		va_start(args, lpszFormat);

		int		nBuf;
		TCHAR	szBuffer[1024];
		DWORD dwOut;
		nBuf = wvsprintf(szBuffer, lpszFormat, args);
		WriteFile(m_hFile, (LPCVOID)szBuffer, lstrlen(szBuffer) , &dwOut , NULL);
		::OutputDebugString(szBuffer);
		va_end(args);

		LeaveCriticalSection(&m_CriticalSection);
	};
public:
	HANDLE m_hFile;
	CRITICAL_SECTION m_CriticalSection; 

};

#endif // LOGINFO_H