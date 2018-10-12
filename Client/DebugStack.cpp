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
#include "DebugStack.h"
#include "Macro.h"
#include "Globals.h"
#include "Ifx.h"
#include "Sess.h"
#include "MacroList.h"
#include "Alias.h"
#include "Event.h"
#include "Action.h"
#include "ActionList.h"
#include "AliasList.h"
#include "EventList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;

CDebugStack::CDebugStack()
{
	m_pnNum = new int[STACK_DEPTH];
	m_pnType = new int[STACK_DEPTH];
	m_pstrValue = new CString[STACK_DEPTH];
	m_nCurrentDepth = 0;
}

CDebugStack::~CDebugStack()
{
	delete [] m_pnNum;
	m_pnNum = NULL;

	delete [] m_pnType;
	m_pnType = NULL;

	delete [] m_pstrValue;
	m_pstrValue = NULL;
}

void CDebugStack::Push(CAlias::PtrType /*alias*/, const std::string &/*value*/)
{
}

void CDebugStack::Push(int nNum, int nType, const char *pszValue)
{
	if (m_nCurrentDepth < STACK_DEPTH)
	{
		m_pnNum[m_nCurrentDepth] = nNum;
		m_pnType[m_nCurrentDepth] = nType;
		m_pstrValue[m_nCurrentDepth] = pszValue;
		m_nCurrentDepth++;
	}
}

void CDebugStack::Pop()
{
	if (m_nCurrentDepth)
		m_nCurrentDepth--;
}

void CDebugStack::DumpStack(CSession *pSession, UINT nDepth)
{
	if (!m_nCurrentDepth || !nDepth)
		return;

	CString strText;
	CString strTemp;
	CString strKey;
	CString strMod;

	int nCurrent = m_nCurrentDepth - 1;

	int nStop = nCurrent - nDepth;
	if(nStop < 0)
		nStop = 0;

	//CAlias	*pAlias		= NULL;
	CMMEvent	*pEvent		= NULL;
//	CAction *pAction	= NULL;
	CMacro	*pMac		= NULL;

	CGlobals::PrintDebugText(pSession, "[BEGIN] Debug Stack:");
	try
	{

	while(nCurrent >= nStop)
	{
		strText.Format("(%04d) ",nCurrent);
		switch(m_pnType[nCurrent])
		{
		case STACK_TYPE_EVAL_ONELINE:
			strTemp.Format("Handle One Line: {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_ACTION :
			{
				CAction::PtrType foundAction;
				if( pSession->GetActions()->GetAt( m_pnNum[nCurrent]-1, foundAction ) )
				{
//TODO: KW  output text that triggered the action and the text of the action 
					CString actionText;
					actionText = m_pstrValue[nCurrent] + " Action: " + foundAction->GetAction().c_str();
					strTemp.Format("Action #%d Triggered by {%s}",
						m_pnNum[nCurrent], 
						actionText);
				}
				else
				{
					strTemp.Format( "Action #%d not found.", m_pnNum[nCurrent] + 1 );
				}
			}
			break;
		case STACK_TYPE_ALIAS :
					strTemp.Format("Alias #%d Alias {%s} has an error",
						m_pnNum[nCurrent], 
						m_pstrValue[nCurrent]);
			//CAlias::PtrType *pAlias = 
			//reinterpret_cast<CAlias::PtrType *> (pSession->GetAliases()->GetByIndex(m_pnNum[nCurrent]));
			////pAlias = (CAlias *)pSession->GetAliases()->GetAt(m_pnNum[nCurrent]);
			//strTemp.Format("Alias #%d: {%s} {%s}",
			//	m_pnNum[nCurrent]+1, 
			//	m_pstrValue[nCurrent],
			//	pAlias->GetAction());
			break;
		case STACK_TYPE_EVENT :
			pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(m_pnNum[nCurrent]);
// FRUITMAN - stop annoying exception window
			if (!pEvent)
			   strTemp.Format("Event <Unknown>");
			else
			   strTemp.Format("Event #%d: {%s}",m_pnNum[nCurrent]+1, pEvent->Event());
			break;
		case STACK_TYPE_MACRO :
			pMac = (CMacro *)pSession->GetMacros()->GetAt(m_pnNum[nCurrent]);
			if (!pMac)
			{
				strTemp.Format("Macro <Unknown>");
			}
			else
			{
				pSession->GetMacros()->GetVirtualKeyName(pMac->VirtualKeyCode(), strKey);
				pSession->GetMacros()->GetModifierName(pMac->Modifiers(), strMod);
				strTemp.Format("Macro <%s %s> {%s}", strMod, strKey, pMac->Action());
			}
			break;
		case STACK_TYPE_USER_INPUT :
			strTemp.Format("User Input: {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_DLL_CALLBACK :
			strTemp.Format("DLL Callback: {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_DLL_RETURN :
			strTemp.Format("DLL Return Value: {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_MATH:
			strTemp.Format("%cMath() {%s}", CGlobals::GetProcedureCharacter(), m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_EVAL_LINE:
			strTemp.Format("EvaluateLine {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_EVAL_STATEMENT:
			strTemp.Format("EvaluateStatement {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_EVAL_CONDITION:
			strTemp.Format("EvaluateCondition {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_EVAL_MULDIV:
			strTemp.Format("EvaluateMulDiv {%s}", m_pstrValue[nCurrent]);
			break;
		case STACK_TYPE_EVAL_PLUSMINUS:
			strTemp.Format("EvaluatePlusMinus {%s}", m_pstrValue[nCurrent]);
			break;
		default:
			strTemp.Format("Unknown debug item.");
			break;
		}
		strText += strTemp;
		CGlobals::PrintDebugText(pSession, strText);
		nCurrent--;
	}
	}
	catch (...)
	{
		CGlobals::PrintDebugText(pSession, "Error Dumping Debug Stack");
	}

	CGlobals::PrintDebugText(pSession, "[END] Debug Stack");
}
