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
#include "stdafx.h"
#include "Ifx.h"
#include "Sub.h"
#include "Globals.h"
#include "ColorLine.h"
#include "ParseUtils.h"
#include "Colors.h"
#include "ColorLine.h"
#include "Sess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace MMScript
{
	CSub::CSub() 
	{
	}

	CSub::CSub(const CSub& src):
	CTrigger(src),
		m_strSub(src.m_strSub)
	{
	}

	CSub::~CSub()
	{

	}


	CSub& CSub::operator =(const CSub & src)
	{
		if(this == &src)
			return *this;

		CTrigger::operator =(src);
		m_strSub = src.m_strSub;

		return *this;
	}

	void CSub::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		CString strTrigger = GetTrigger();
		if(bIncludeGroup)
			strText.Format("{%s} {%s} {%s}",
			strTrigger,
			Sub(),
			Group());
		else
			strText.Format("{%s} {%s}",
			strTrigger,
			Sub());

	}

	void CSub::MapToCommand(CString &strCommand)
	{
		CString strTemp;
		MapToText(strTemp, TRUE);
		strCommand.Format("%csubstitute %s", CGlobals::GetCommandCharacter(), strTemp);
	}

	void CSub::DoSub(CSession *pSession, CColorLine::SpColorLine &line)
	{
		CString strNew = m_strSub;
		CString strOld ("x",1024);  //start with a string longer than expected to reduce the requirement to expand it with each substitution
		//original text
		strOld = line->GetTextBuffer();
			
		CParseUtils::ReplaceVariables(pSession, strNew);
		//TODO:KW count commands processed - substitute counts as a command
		pSession->SetCmdsProcessed(pSession->GetCmdsProcessed() + 1);
		if(m_nNumVars == 0)
		{
			if(m_bAnchored)
			{
				CParseUtils::ReplaceText(
					pSession,						//Pointer to the session
					line,							//Original Line
					0,								//Index to start at
					m_strTokens[0].GetLength(),		//Number of characters to replace
					strNew);						//New String
				return;
			}
			else
			{
				strOld = line->GetTextBuffer();
				int nIndex = 0;
				while(nIndex < strOld.GetLength())
				{
					nIndex = strOld.Find(m_strTokens[0], nIndex);
					if(nIndex >= 0)
					{
						CParseUtils::ReplaceText(
							pSession,
							line,
							nIndex,
							m_strTokens[0].GetLength(),
							strNew);
						nIndex+=strNew.GetLength();
					   strOld = line->GetTextBuffer();
						
					}
					else
					{
						break;
					}
					//strOld = line->GetTextBuffer(); moved this up since it is only needed if subs was done
				}
				return;
			}
		}
		else
		{
			strOld = line->GetTextBuffer();
			CParseUtils::ReplaceText(
					pSession,
					line,
					0,
					strOld.GetLength(),
					strNew);

		}
	}
}