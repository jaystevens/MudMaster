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
#include "Ifx.h"
#include "Globals.h"
#include "SubList.h"
#include "ParseUtils.h"
#include "Sub.h"
#include "CharFormat.h"
#include "ColorLine.h"
#include "Sess.h"
#include "SessionInfo.h"
#include "DefaultOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	CSubList::CSubList()
	{
	}

	CSubList::~CSubList()
	{
	}

	CTrigger *CSubList::Add(const CString& strTrigger, const CString& strSub, const CString& strGroup)
	{
		if(strTrigger.IsEmpty() || strSub.IsEmpty())
			return NULL;

		CSub *pSub = new CSub;
		BOOL bResult = pSub->SetTrigger(strTrigger);
		if(!bResult)
		{
			delete pSub;
			return NULL;
		}

		pSub->Sub(strSub);
		pSub->Group(strGroup);
		return (CTrigger *)AddToList(pSub);
	}

	BOOL CSubList::ReplaceText(CString &strLine, CCharFormat *pColorBuffer, CCharFormat *pReplaceColorBuffer, int nStart, int nOriginalLength, LPCSTR pszReplace, int nReplacementLength, BOOL bWholeLine)
	{
		if (bWholeLine)
		{
			//Set the whole string to the same color as the first character
			int nLen = strlen(pszReplace);

			for(int i = 0; i<nLen; i++)
			{
				pReplaceColorBuffer[i] = pColorBuffer[0];
			}

			strLine = pszReplace;
			return TRUE;
		}

		//If the text we are replacing is smaller than the text 
		//we are replacing it with, we will slice off the pre
		//sub chars, put in the new text, then put in the rest
		//of the text
		int i = 0;
		CString aTextLine;

		if(nReplacementLength > nOriginalLength)
		{
			//copy the stuff before the subbed text
			aTextLine = strLine.Left(nStart);
			for(i = 0; i < nStart; i++)
			{
				pReplaceColorBuffer[i] = pColorBuffer[i];
			}

			//copy in the subbed text
			aTextLine += pszReplace;
			for(i = 0; i < nReplacementLength; i++)
			{
				pReplaceColorBuffer[i+nStart] = pColorBuffer[nStart];
			}

			i = 0;

			//copy the rest of the original string
			int nLen = strLine.GetLength();
			aTextLine += strLine.Right(strLine.GetLength() - (nStart+nOriginalLength));
			nLen = nLen - nStart;
			nLen = nLen - nOriginalLength;
			for(i = 0; i < nLen; i++)
			{
				pReplaceColorBuffer[i+nStart+nReplacementLength] = pColorBuffer[i+nStart+nOriginalLength];
			}

			//copy the subbed string back to the original buffer
			strLine = aTextLine;
			return TRUE;
		}

		aTextLine = strLine.Left(nStart);
		aTextLine += pszReplace;
		aTextLine += strLine.Right(strLine.GetLength() - (nStart+nOriginalLength));
		int nLen = strLine.GetLength();
		nLen = nLen - nStart;
		nLen = nLen - nOriginalLength;
		for(i = 0; i < nLen; i++)
		{
			pReplaceColorBuffer[i] = pColorBuffer[i+nStart+nOriginalLength];
		}
		strLine = aTextLine;
		return TRUE;
	}

	void CSubList::CheckSub(CSession *pSession, CColorLine::SpColorLine &line)
	{
		int nDepth = pSession->GetOptions().sessionInfo_.SubDepth();

		int nCount = 0;
		int i = 0;
		while(i < m_nCount)
		{
			CSub *pSub = (CSub *)m_list.GetAt(i);
			if(!pSub->Match(line->GetTextBuffer()))
			{
				i++;
				continue;
			}

			pSub->DoSub(pSession, line);
			nCount++;
			i++;
			if(nCount >= nDepth)
				break;
		}
	}
}