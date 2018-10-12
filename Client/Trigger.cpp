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
#include "ifx.h"
#include "Trigger.h"
#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace MMScript
{
	CTrigger::CTrigger()
	{
		m_bAnchored = FALSE;
		m_bEnabled = TRUE;
		m_bStartVar = FALSE;
		m_nNumTokens = 0;
		m_nNumVars = 0;
		for(int i = 0; i < MAX_TRIGGER_VARS; i++)
		{
			m_nVar[i] = 0;
		}
	}

	CTrigger::~CTrigger()
	{
	}

	CTrigger::CTrigger(const CTrigger& src) : 
	m_bAnchored(src.m_bAnchored),
		m_bEnabled(src.m_bEnabled),
		m_bStartVar(src.m_bStartVar),
		m_nNumTokens(src.m_nNumTokens),
		m_nNumVars(src.m_nNumVars),
		m_strGroup(src.m_strGroup),
		m_strTrigger(src.m_strTrigger)
	{
		for(int i = 0; i < MAX_TRIGGER_VARS; i++)
		{
			m_nVar[i] = src.m_nVar[i];
			m_strTokens[i] = src.m_strTokens[i];
		}
	}

	CTrigger& CTrigger::operator =(const CTrigger& src)
	{
		if(this == &src)
			return *this;

		m_bAnchored = src.m_bAnchored;
		m_bEnabled = src.m_bEnabled;
		m_bStartVar = src.m_bStartVar;
		m_nNumTokens = src.m_nNumTokens;
		m_nNumVars = src.m_nNumVars;
		m_strGroup = src.m_strGroup;
		m_strTrigger = src.m_strTrigger;

		for(int i = 0; i < MAX_TRIGGER_VARS; i++)
		{
			m_nVar[i] = src.m_nVar[i];
			m_strTokens[i] = src.m_strTokens[i];
		}

		return *this;
	}

	BOOL CTrigger::SetTrigger(const CString &strTrigger)
	{
		int nIndex = 0;
		if(strTrigger[nIndex] == '^')
		{
			nIndex++;
			m_bAnchored = TRUE;
		}

		int nLen = strTrigger.GetLength();

		while(nIndex < nLen)
		{

			// we've found a "\%" combination this allows the user to
			// trigger on the % character instead of us thinking its 
			// supposed to be a variable
			if(	strTrigger[nIndex] == CGlobals::GetEscapeCharacter() &&
				nIndex + 1 < nLen &&
				strTrigger[nIndex+1] == '%')
			{
				m_strTokens[m_nNumTokens] += '%';
				nIndex += 2;
				continue;
			}

			// we've found a "\#" combination, where # is a digit between 0 and 9
			// this indicates a variable
			if( strTrigger[nIndex] == '%' &&
				nIndex + 1 < nLen &&
				isdigit(strTrigger[nIndex + 1]))
			{
				// If we're at the start of the string, then this trigger begins
				// with a variable and we need to note that by setting m_bStartVar = TRUE
				if(nIndex == 0)
					m_bStartVar = TRUE;


				// If the current token is empty, this means the last action was to set a
				// variable.  Since we're setting a variable again, the trigger must look
				// something like "%0%1",  this trigger is impossible to evaluate. Bail out.
				if( m_strTokens[m_nNumTokens].IsEmpty() &&
					( nIndex != 0 && m_bStartVar))
					return FALSE;


				// If this variable isn't the first thing in the trigger, we need to 
				// advance the number of tokens
				if(nIndex == 0 && m_bStartVar)
				{
					// We just set a "start var", don't advance the token count
				}
				else
				{
					// not a "start var"  advance the token count
					m_nNumTokens++;
				}

				// We currently don't allow more than 10 variables in a trigger
				if(m_nNumVars > MAX_TRIGGER_VARS)
					return FALSE;

				m_nVar[m_nNumVars] = strTrigger[nIndex+1] - '0';
				m_nNumVars++;
				nIndex += 2;
				continue;
			} // handling variable

			if(m_nNumTokens > 9)
				return FALSE;

			m_strTokens[m_nNumTokens] += strTrigger[nIndex];
			nIndex++;

		} // while nIndex < nLen

		// We've reached the end of the trigger.  If there is data in the current
		// token, we need to advance the token counter by one.
		if( !m_strTokens[m_nNumTokens].IsEmpty() )
			m_nNumTokens++;

		// triggers that begin with a variable and declared as anchored are pointless
		// turn off the anchored variable
		if( m_bAnchored && m_bStartVar )
			m_bAnchored = FALSE;

		FormatTrigger();

		return TRUE;
	}

	const CString& CTrigger::GetTrigger() const
	{
		return m_strTrigger;
	}

	void CTrigger::FormatTrigger()
	{
		if(m_bAnchored)
			m_strTrigger = "^";

		int nVarIndex = 0;
		if(m_bStartVar)
		{
			m_strTrigger += '%';
			TCHAR varChar = m_nVar[nVarIndex] + '0';
			m_strTrigger += varChar;
			//		m_strTrigger += '0';
			nVarIndex++;
		}

		for(int i = 0; i < m_nNumTokens; i++)
		{
			m_strTrigger += m_strTokens[i];
			if(nVarIndex < m_nNumVars)
			{
				m_strTrigger += '%';
				TCHAR varChar = m_nVar[nVarIndex] + '0';
				m_strTrigger += varChar;
				//			m_strTrigger += '0';
				nVarIndex++;
			}
		}
	}

	BOOL CTrigger::Match(const CString& strText) const
	{
		// if the trigger is disabled, it never matches
		if(!m_bEnabled)
			return FALSE;


		// If there are no vars, its just a straight string compare against the 0th token
		if(m_nNumVars == 0)
		{
			if(m_bAnchored)
			{
				if(strText.Find(m_strTokens[0]) == 0)
					return TRUE;
				else
					return FALSE;
			}
			else
			{
				if(strText.Find(m_strTokens[0]) != -1)
					return TRUE;
				else
					return FALSE;
			}
		}

		// Well, ok, there's vars so its a little more complicated, but we'll be all right!
		int nVarCount = 0;

		if(m_nNumTokens == 0)
		{
			// We have a var but no tokens...so the trigger must be just "%#" where
			// # is the number of some variable.  All strings will match this trigger
			CGlobals::SetGlobalVar(m_nVar[nVarCount], strText);
			return TRUE;
		}

		// Its even more complicated now, because we've got tokens AND vars
		// we'll be ok tho, because Aaron has already worked out the logic for us =)

		int nIndex = strText.Find(m_strTokens[0]);

		// If first token isn't found in the string, then its not a match
		if(nIndex == -1)
			return FALSE;

		// If the trigger is anchored, and the token isn't found at the 
		// beginning of the string, then its not a match
		if(m_bAnchored && nIndex != 0)
			return FALSE;

		// If the trigger begins with a var...
		if(m_bStartVar)
		{
			// If the first token is found at the beginning of the string,
			// then we need to see if the first token is repeated in the string
			if(nIndex == 0)
			{
				nIndex = strText.Find(m_strTokens[0], nIndex + 1);

				// if the token isn't repeated, then the string is not a match
				if(nIndex == -1)
					return FALSE;
			}

			CGlobals::SetGlobalVar(m_nVar[0], strText.Left(nIndex));
			nVarCount++;
		}

		nIndex += m_strTokens[0].GetLength();

		for(int i = 1; i < m_nNumTokens; i++)
		{
			// Search for the token
			int nNewIndex = strText.Find(m_strTokens[i], nIndex);

			// If the token isn't found, the trigger doesn't match
			if(nNewIndex == -1)
				return FALSE;

			CGlobals::SetGlobalVar(m_nVar[nVarCount], strText.Mid(nIndex, nNewIndex-nIndex));
			nVarCount++;
			nIndex = nNewIndex + m_strTokens[i].GetLength();

		} // for(i = 0; i < m_nTokens; i++)

		// If we've gone through all of the tokens, but we haven't collected all of the
		// variables, then the trigger must have ended with a variable
		if(nVarCount < m_nNumVars)
			CGlobals::SetGlobalVar(m_nVar[nVarCount], strText.Mid(nIndex));

		return TRUE;
	}
}