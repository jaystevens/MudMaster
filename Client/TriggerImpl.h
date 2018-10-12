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
#pragma once

#include "StringUtils.h"

namespace MMScript
{
	template<typename T>
	class CTriggerImpl
	{
		static const DWORD MAX_TRIGGER_VARS = 10;

		bool m_bAnchored;
		int m_nNumTokens;
		int m_nNumVars;
		bool m_bStartVar;
		CString m_strTokens[MAX_TRIGGER_VARS];
		byte m_nVar[MAX_TRIGGER_VARS];

	public:
		CTriggerImpl()
			: m_bAnchored( false )
			, m_nNumTokens( 0 )
			, m_nNumVars( 0 )
			, m_bStartVar( false )
		{
			for( int i = 0; i < MAX_TRIGGER_VARS; ++i )
				m_nVar[i] = 0;
		}

		bool Match( const CString &strText )
		{
			T *pThis = (T*)this;

			// if the trigger is disabled, it never matches
			if( !pThis->IsEnabled() )
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

	protected:
		bool ProcessTrigger(const std::string &trigger)
		{
			int nIndex = 0;
			if(trigger[nIndex] == '^')
			{
				nIndex++;
				m_bAnchored = true;
			}

			if( trigger.find('%') == string::npos )
			{
				std::string triggerNoCaret = (m_bAnchored ? trigger.substr(1) : trigger);
				m_strTokens[ m_nNumTokens ] = triggerNoCaret.c_str();
				++m_nNumTokens;
			}
			else
			{
				int nLen = trigger.length();

				while(nIndex < nLen)
				{

					// we've found a "\%" combination this allows the user to
					// trigger on the % character instead of us thinking its 
					// supposed to be a variable
					if(	trigger[nIndex] == CGlobals::GetEscapeCharacter() &&
						nIndex + 1 < nLen &&
						trigger[nIndex+1] == '%')
					{
						m_strTokens[m_nNumTokens] += '%';
						nIndex += 2;
						continue;
					}

					// we've found a "\#" combination, where # is a digit between 0 and 9
					// this indicates a variable
					if( trigger[nIndex] == '%' &&
						nIndex + 1 < nLen &&
						isdigit(trigger[nIndex + 1]))
					{
						// If we're at the start of the string, then this trigger begins
						// with a variable and we need to note that by setting _startsWithVar = TRUE
						if(nIndex == 0)
							m_bStartVar = true;

						// If the current token is empty, this means the last action was to set a
						// variable.  Since we're setting a variable again, the trigger must look
						// something like "%0%1",  this trigger is impossible to evaluate. Bail out.
						if( m_strTokens[m_nNumTokens].IsEmpty() && 
							nIndex != 0 && 
							m_bStartVar)
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

						m_nVar[m_nNumVars] = trigger[nIndex+1] - '0';
						m_nNumVars++;
						nIndex += 2;
						continue;
					} // handling variable

					if(m_nNumTokens > 9)
						return FALSE;

					m_strTokens[m_nNumTokens] += trigger[nIndex];
					nIndex++;

				} // while nIndex < nLen
			}

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

		void FormatTrigger()
		{
			std::stringstream buffer;

			if(m_bAnchored)
				buffer << "^";

			int nVarIndex = 0;

			int var = m_nVar[nVarIndex];
			if(m_bStartVar)
			{
				buffer << '%';
				buffer << var;
				nVarIndex++;
			}

			if( 0 == m_nNumVars )
			{
				buffer << m_strTokens[0];
			}
			else
			{
				for( int i = 0; i < m_nNumTokens; ++i )
				{
					buffer << m_strTokens[i];
					if( nVarIndex < m_nNumVars )
					{
						buffer << '%';

						var = m_nVar[nVarIndex];
						buffer << var;

						nVarIndex++;
					}
				}
			}

			T *pThis = (T*)this;
			pThis->SetTrigger( buffer.str().c_str() );
		}
	};
}