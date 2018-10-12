// compinfo.h (Windows NT/2000)
//
// Yarantsau Andrei, QA
// Minsk, Belarus
//
#ifndef COMPINFO_H
#define COMPINFO_H

//TODO: KW compiling without precompiled headers causes an error because of this
//#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "loginfo.h"

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define TIME_TEXT_SIZE				12
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;


// ntdll!NtQuerySystemInformation (NT specific!)
//
// The function copies the system information of the
// specified type into a buffer
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQuerySystemInformation(
//    IN UINT SystemInformationClass,    // information type
//    OUT PVOID SystemInformation,       // pointer to buffer
//    IN ULONG SystemInformationLength,  // buffer size in bytes
//    OUT PULONG ReturnLength OPTIONAL   // pointer to a 32-bit
//                                       // variable that receives
//                                       // the number of bytes
//                                       // written to the buffer 
// );
typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

PROCNTQSI NtQuerySystemInformation;

#define HEAP_DUMP_FILENAME "heap.out"

class CCompInfo
{
public:
	CCompInfo(char* filename);
	CCompInfo();
	~CCompInfo();
	void SetAnalyzeTime(UINT nTime);
	UINT GetCPUInfo();
	DWORD HeapMakeSnapShot();
	DWORD HeapCommitedBytes();
	DWORD HeapCompareSnapShots();
	void HeapStoreDumpToFile();
	void HeapCompareDumpWithFile(BOOL bShowContent);
	void HeapPrintDump(BOOL bShowContent);
	char* GetTimeString();
private:
	void DoInit();
	void GetHeaps();
	void GetHeapWalk();
public:
	unsigned			m_nHeaps;
	HANDLE				m_aHeaps[ 256 ];
	DWORD				m_dwCurrentSize;
	DWORD				m_dwSnapShotSize;
	UINT				m_Sleep;
	CLogfile*			m_log;
	char				m_sTime[ TIME_TEXT_SIZE ];
};

CCompInfo::CCompInfo(char* filename) 
{ 
	DoInit();
	m_log = new CLogfile(filename);
};

CCompInfo::CCompInfo() 
{ 
	DoInit();
};

CCompInfo::~CCompInfo() 
{
	if (NULL != m_log)
		delete m_log;

	DeleteFile(HEAP_DUMP_FILENAME);
};

void CCompInfo::DoInit()
{
	//Sleep for getting results
	m_Sleep = 1000;
	//Switch Crt_heap to Process_heap
	//Variable:__MSVCRT_HEAP_SELECT
	//Value:__GLOBAL_HEAP_SELECTED,1
	if (!SetEnvironmentVariable( "__MSVCRT_HEAP_SELECT","__GLOBAL_HEAP_SELECTED,1"))
		return;
	m_dwCurrentSize = 0;
	m_dwSnapShotSize = 0;	
	m_log = NULL;
}

void CCompInfo::SetAnalyzeTime(UINT nTime) 
{ 
	if (nTime > 0) 
		m_Sleep = nTime; 
};

UINT CCompInfo::GetCPUInfo()
{
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION        SysTimeInfo;
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;
	double                         dbIdleTime;
	double                         dbSystemTime;
	LONG                           status;
	LARGE_INTEGER                  liOldIdleTime = {0,0};
	LARGE_INTEGER                  liOldSystemTime = {0,0};

	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(
		GetModuleHandle("ntdll"),
		"NtQuerySystemInformation"
		);

	if (!NtQuerySystemInformation)
		return 0;

	// get number of processors in the system
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
	if (status != NO_ERROR)
		return 0;

	while(1)
	{
		// get new system time
		status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
		if (status!=NO_ERROR)
			return 0;

		// get new CPU's idle time
		status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
		if (status != NO_ERROR)
			return 0;

		// if it's a first call - skip it
		if (liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;

			return (UINT)dbIdleTime;
		}

		// store new CPU's idle and system time
		liOldIdleTime = SysPerfInfo.liIdleTime;
		liOldSystemTime = SysTimeInfo.liKeSystemTime;

		// wait one second
		Sleep(m_Sleep);
	}
}

void CCompInfo::GetHeaps()
{
	memset(m_aHeaps,0,sizeof(m_aHeaps));
	m_nHeaps = GetProcessHeaps(sizeof(m_aHeaps)/sizeof(m_aHeaps[0]),m_aHeaps);
};

void CCompInfo::GetHeapWalk() 
{
	PROCESS_HEAP_ENTRY pEntry;
	m_dwCurrentSize = 0;

	GetHeaps();

	for ( unsigned i=0; i < m_nHeaps; i++ )
	{
		pEntry.lpData = NULL;
		while (HeapWalk(m_aHeaps[i], &pEntry ))
		{
			if ((PROCESS_HEAP_ENTRY_BUSY & pEntry.wFlags) == PROCESS_HEAP_ENTRY_BUSY)
				m_dwCurrentSize += pEntry.cbData;
		}
	}
};

DWORD CCompInfo::HeapMakeSnapShot()
{
	GetHeapWalk();
	m_dwSnapShotSize = m_dwCurrentSize;
	return m_dwSnapShotSize;
};

DWORD CCompInfo::HeapCommitedBytes()
{
	GetHeapWalk();
	return m_dwCurrentSize;
}

DWORD CCompInfo::HeapCompareSnapShots()
{
	GetHeapWalk();
	return (m_dwCurrentSize - m_dwSnapShotSize);
}

void CCompInfo::HeapStoreDumpToFile()
{
	PROCESS_HEAP_ENTRY pEntry;
	m_dwCurrentSize = 0;
	HANDLE hFile;
	DWORD dwIn[2], dwOut;

	GetHeaps();

	hFile = CreateFile(HEAP_DUMP_FILENAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS , NULL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile ) return;

	for ( unsigned i=0; i < m_nHeaps; i++ )
	{
		pEntry.lpData = NULL;
		while (HeapWalk(m_aHeaps[i], &pEntry ))
		{
			if ((PROCESS_HEAP_ENTRY_BUSY & pEntry.wFlags) == PROCESS_HEAP_ENTRY_BUSY)
			{
				dwIn[0] = (DWORD)pEntry.lpData;
				dwIn[1] = (DWORD)pEntry.cbData;
				WriteFile(hFile, &dwIn, sizeof(DWORD)*2 , &dwOut , NULL);
			}
		}
	}

	CloseHandle(hFile);
}

void CCompInfo::HeapCompareDumpWithFile(BOOL bShowContent)
{
	PROCESS_HEAP_ENTRY pEntry;
	m_dwCurrentSize = 0;
	BOOL bIsPresent = FALSE, bResult = FALSE;
	HANDLE hFile;
	DWORD dwIn[2], dwOut;

	GetHeaps();

	hFile = CreateFile(HEAP_DUMP_FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING , FILE_ATTRIBUTE_READONLY , NULL );
	if ( INVALID_HANDLE_VALUE == hFile ) return;

	if (NULL != m_log)
	{
		if (bShowContent)
			m_log->print( "\n\n ----- E X T E N D E D  H E A P  D U M P ----- \n\n");
		else
			m_log->print("\n\n ----- B A S I C  H E A P  D U M P ----- \n\n");
	}

	for ( unsigned i=0; i < m_nHeaps; i++ )
	{
		pEntry.lpData = NULL;
		while (HeapWalk(m_aHeaps[i], &pEntry ))
		{
			if ((PROCESS_HEAP_ENTRY_BUSY & pEntry.wFlags) == PROCESS_HEAP_ENTRY_BUSY)
			{
				SetFilePointer (hFile, NULL , NULL, FILE_BEGIN) ; 
				bIsPresent = FALSE;
				memset( dwIn , 0 , sizeof(DWORD)*2 );
				dwOut = 0;
				while( 1 )
				{      
					bResult = ReadFile(hFile, &dwIn, sizeof(DWORD)*2 , &dwOut, NULL );
					if (bResult && dwOut == 0)  break;
					if ( dwIn[0] == (DWORD)pEntry.lpData && dwIn[1] == (DWORD)pEntry.cbData )
					{
						bIsPresent = TRUE;
						break;
					}

				}
				if (!bIsPresent)
				{
					if (NULL != m_log)
						m_log->print("block at [0x%x] size [%d]\n" , pEntry.lpData , pEntry.cbData );

					if (bShowContent)
					{
						char* pData = (char *)pEntry.lpData;
						for ( unsigned k = 0 ; k < pEntry.cbData; k++ )
						{
							if ( NULL != m_log )
								m_log->print( "%c" , pData[k] );
						}
						if ( NULL != m_log )
							m_log->print("\n\n");
					}
				}

			}
		}
	}

	CloseHandle(hFile);
}

void CCompInfo::HeapPrintDump(BOOL bShowContent)
{
	PROCESS_HEAP_ENTRY pEntry;
	m_dwCurrentSize = 0;

	GetHeaps();

	if ( NULL != m_log )
	{
		if (bShowContent)
			m_log->print( "--------- Extended heap dump ---------\n");
		else
			m_log->print( "--------- Basic heap dump ---------\n");
	}

	for ( unsigned i=0; i < m_nHeaps; i++ )
	{
		pEntry.lpData = NULL;
		unsigned j = 0;
		while (HeapWalk(m_aHeaps[i], &pEntry ))
		{
			if ((PROCESS_HEAP_ENTRY_BUSY & pEntry.wFlags) == PROCESS_HEAP_ENTRY_BUSY)
			{
				if ( NULL != m_log )
					m_log->print("heap[%d] block[%d] at [0x%x] size[%d]\n" , i , j , pEntry.lpData , pEntry.cbData );
				if (bShowContent)
				{
					char* pData = (char *)pEntry.lpData;
					for ( unsigned k = 0 ; k < pEntry.cbData; k++ )
					{
						if ( NULL != m_log )
							m_log->print( "%c" , pData[k] );
					}
					if ( NULL != m_log )
						m_log->print( "\n\n" );
				}
			}
			j++;
		}
	}
	if ( NULL != m_log )
		m_log->print( "\n------------------------------------\n");
}

char* CCompInfo::GetTimeString()
{
	SYSTEMTIME  sysTime;
	GetLocalTime(&sysTime);
	memset(m_sTime, 0, TIME_TEXT_SIZE );
	wsprintf(m_sTime, "%02d:%02d:%02d%s", (sysTime.wHour == 0 ? 12 : (sysTime.wHour <= 12 ? sysTime.wHour : sysTime.wHour -12)),
		sysTime.wMinute,
		sysTime.wSecond,
		(sysTime.wHour < 12 ? "AM":"PM"));
	return m_sTime;
}

#endif // COMPINFO_H