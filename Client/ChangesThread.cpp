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
#include "StdAfx.h"
//#include "Extern.h"
#include "ChangesThread.h"
#include "CommandTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CChangesThread::CHANGESTHREADINFO CChangesThread::m_cti;
//CChangesThread::m_cti.pszTextToPrint = new char[CHANGES_BUFFER+1];

void CChangesThread::ProcessChangesFile(CHttpFile *pHttpFile, CHANGESTHREADINFO *pInfo)
{
	CString strLine;
	CString strSep;
	pInfo->pszTextToPrint[0] = '\x0';

	if (!pHttpFile)
		return;
	try
	{
		while(pHttpFile->ReadString(strLine) && strlen(pInfo->pszTextToPrint) + strLine.GetLength() < CHANGES_BUFFER)
		{
			if (strLine.IsEmpty())
				strcat(pInfo->pszTextToPrint,"\n");
			else
				strcat(pInfo->pszTextToPrint,strLine+"\n");
		} 
	}
	catch(...)
	{		
		if (pHttpFile)
			pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
		CChangesThread::m_cti.nThreadStatus = THREAD_CHANGES_QUIT;
		
	}

}

UINT CChangesThread::ChangesThreadProc(LPVOID pVoid)
{
	CHANGESTHREADINFO *pThreadInfo;
	CHttpConnection *pHttpServer = NULL;
	CHttpFile *pHttpFile = NULL;
	DWORD dwServiceType;
	CString strServerName;
	CString strObject;
	CString strHeaders;
	CString strFormData;
	INTERNET_PORT nPort;
//	double nLength;

	pThreadInfo = (CHANGESTHREADINFO*)pVoid;

	try
	{
		CInternetSession isVersion("CHANGES");
		//in case this is to become a POST
		strFormData = CString(pThreadInfo->pURL); 
		if(strlen(pThreadInfo->pURL) >2000)
		{
			pThreadInfo->pURL[2000] = '\x0';
		}

		if (!AfxParseURL(pThreadInfo->pURL, 
			dwServiceType, strServerName, strObject, nPort))
		{
			pThreadInfo->nThreadStatus = THREAD_CHANGES_QUIT;
			return(1);
		}
		pHttpServer = isVersion.GetHttpConnection(strServerName,nPort);
		if (!pHttpServer)
		{
			pThreadInfo->nThreadStatus = THREAD_CHANGES_QUIT;
			return(1);
		}
		if(strObject.GetLength() <256)
		{
			pHttpFile = pHttpServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
				strObject,NULL,1,NULL,NULL,
				INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_NO_AUTO_REDIRECT|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);

			if (!pHttpFile->AddRequestHeaders("Content-Type: application/x-www-form-urlencoded\r\nAccept: text/*\r\nCache-Control: no-cache\r\n"))
			{
				pHttpServer->Close();
				delete pHttpServer;
				pHttpServer = NULL;
				pThreadInfo->nThreadStatus = THREAD_CHANGES_QUIT;
				return(1);
			}
			pHttpFile->SetReadBufferSize(CHANGES_BUFFER);
			pHttpFile->SendRequest();

		}
		else
		{
			strHeaders = _T("Content-Type: application/x-www-form-urlencoded\r\nAccept: text/*\r\nCache-Control: no-cache\r\n");
			
			strFormData =	strFormData.Mid(strFormData.Find("?")+1);

			strFormData = URLEncode(strFormData);
			pHttpFile = pHttpServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				strObject.Mid(0,strObject.Find("?")),NULL,1,NULL,NULL,
				INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_NO_AUTO_REDIRECT|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
			if (!pHttpFile)
			{
				pHttpServer->Close();
				delete pHttpServer;
				pHttpServer = NULL;
				pThreadInfo->nThreadStatus = THREAD_CHANGES_QUIT;
				return(1);
			}
			pHttpFile->SetReadBufferSize(CHANGES_BUFFER);
			pHttpFile->SendRequest(strHeaders,(LPVOID)(LPCTSTR)strFormData, strFormData.GetLength());

		}


		ProcessChangesFile(pHttpFile,pThreadInfo);
		if (pHttpFile)
			pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
		if (pHttpServer)
			pHttpServer->Close();
		delete pHttpServer;
		pHttpServer = NULL;
	}
	catch(CInternetException *pEx)
	{
		if (pHttpFile)
			pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
		if (pHttpServer)
			pHttpServer->Close();
		delete pHttpServer;
		pHttpServer = NULL;
		pEx->Delete();
		pThreadInfo->nThreadStatus = THREAD_CHANGES_QUIT;
		return(1);
	}
	catch(...)
	{
		::OutputDebugString(_T("Non Internet Exception Error executing ChangesThreadProc\n"));
		if (pHttpFile)
			pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;

	}

	pThreadInfo->nThreadStatus = THREAD_CHANGES_DONE;
	return(0);
}
CString CChangesThread::URLEncode(CString sIn)
{
    CString sOut;
	
    const int nLen = sIn.GetLength() + 1;

    register LPBYTE pOutTmp = NULL;
    LPBYTE pOutBuf = NULL;
    register LPBYTE pInTmp = NULL;
    LPBYTE pInBuf =(LPBYTE)sIn.GetBuffer(nLen);
//    BYTE b = 0;
	
    //alloc out buffer
    pOutBuf = (LPBYTE)sOut.GetBuffer(nLen  * 3 - 2);//new BYTE [nLen  * 3];

    if(pOutBuf)
    {
        pInTmp	= pInBuf;
	pOutTmp = pOutBuf;
		
	// do encoding
	while (*pInTmp)
	{
	    if(isalnum(*pInTmp) || *pInTmp == '&' || *pInTmp == '=')
	        *pOutTmp++ = *pInTmp;
	    else
	        if(*pInTmp != '\n' && isspace(*pInTmp))
		    *pOutTmp++ = '+';
		else
		{
		    *pOutTmp++ = '%';
		    *pOutTmp++ = toHex(*pInTmp>>4);
		    *pOutTmp++ = toHex(*pInTmp%16);
		}
	    pInTmp++;
	}
	*pOutTmp = '\0';
	//sOut=pOutBuf;
	//delete [] pOutBuf;
	sOut.ReleaseBuffer();
    }
    sIn.ReleaseBuffer();
    return sOut;
}

