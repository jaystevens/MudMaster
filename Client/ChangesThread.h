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
#ifndef _CHANGESTHREAD_H_
#define _CHANGESTHREAD_H_

#define THREAD_CHANGES_FAILED			0		// Failed to start the thread.
#define THREAD_CHANGES_RUNNING		1		// Currently running.
#define THREAD_CHANGES_DONE			2		// Finished running and is ok.
#define THREAD_CHANGES_QUIT			3		// Finished running but failed.
#define THREAD_CHANGES_FINISHED		4		// Finished running and data in structure
											//   has been processed. This gets set regardless
											//   of success or failure of the proc.

// Size of the receive buffer for the version data.
#define CHANGES_BUFFER					50000

#define CHANGESFILE						"Changes.txt"

class CChangesThread
{
	struct CHANGESTHREADINFO
	{
		// Lets the main app know how the thread is doing.
		int	nThreadStatus;
		LPSTR pURL;
		// Text to print if the thread exits with THREAD_VERSION_DONE.
		LPSTR pszTextToPrint;
	};

public:
	static UINT ChangesThreadProc(LPVOID pVoid);
	static void ProcessChangesFile(CHttpFile *pHttpFile, CHANGESTHREADINFO *pInfo);
	static inline BYTE toHex(const BYTE &x)
	{
		return x > 9 ? x + 55: x + 48;
	}
	static CString URLEncode(CString sIn);
	static CHANGESTHREADINFO m_cti;
};	
#endif