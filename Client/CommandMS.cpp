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
#include "CommandTable.h"
#include "ParseUtils.h"
#include "Macro.h"
#include "Dll.h"
#include "direct.h"
#include "Event.h"
#include "Array.h"
#include "BarItem.h"
#include "Gag.h"
#include "Variable.h"
#include "Sub.h"
#include "High.h"
#include "MMList.h"
#include "ObString.h"
#include "Sess.h"
#include "Colors.h"
#include "VarList.h"
#include "DllList.h"
#include "ChatServer.h"
#include "EventList.h"
#include "AliasList.h"
#include "ArrayList.h"
#include "StatusBar.h"
#include "GagList.h"
#include "HighList.h"
#include "UserLists.h"
#include "SubList.h"
#include "Alias.h"
#include "MacroList.h"
#include "ScriptProgressDlg.h"
#include "SessionInfo.h"
#include "MessageOptions.h"
#include "StatusbarOptions.h"
#include "PathOptions.h"
#include "MSPOptions.h"
#include "Globals.h"
#include "DefaultOptions.h"
#include "ErrorHandling.h"
#include "FileUtils.h"
#include "StatementParser.h"
#include "Colors.h"
#include "MainFrm.h"
#include "ChangesThread.h"
#include <sstream>
//#include "ODBCWrapper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace MMScript;
using namespace MMSystem;
using namespace SerializedOptions;
using namespace std;
using namespace Utilities;


BOOL CCommandTable::Macro(CSession *pSession, CString &strLine)
{
	CString strParam1, strParam2, strGroup;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strParam2))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strParam1);
	CParseUtils::ReplaceVariables(pSession, strGroup);
	CParseUtils::PretranslateString(pSession, strParam2);

	if (!strParam1.IsEmpty() && !strParam2.IsEmpty())
	{
		int nDest;
		if (strParam2.Right(1) == ";")
		{
			nDest = CMacro::MacroDestBar;
			strParam2 = strParam2.Left(strParam2.GetLength()-1);
		}
		else
		{
			if (strParam2.Right(1) == ":")
			{
				nDest = CMacro::MacroDestBarCr;
				strParam2 = strParam2.Left(strParam2.GetLength()-1);
			}
			else
			{
				nDest = CMacro::MacroDestMud;
			}
		}

		if (pSession->GetMacros()->Add(strParam1,strParam2,strGroup,nDest))
		{
			pSession->UpdateMacros();
			pSession->QueueMessage(CMessages::MM_MACRO_MESSAGE, "Macro added.");
		}
		else
		{
			pSession->QueueMessage(CMessages::MM_MACRO_MESSAGE, "Macro not added.");
		}
		return TRUE;
	}

	if(strParam1.IsEmpty() && strParam2.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Macros:");
		CMacro *pMacro = (CMacro *)pSession->GetMacros()->GetFirst();
		int nLineCount = 0;
		CString strMessage;
		while(pMacro != NULL)
		{
			CString strTemp, strText;
			pMacro->MapToText(strTemp, TRUE);
			strText.Format("%s%03d:%s%c%s\n",
				ANSI_F_BOLDWHITE,
				pSession->GetMacros()->GetFindIndex()+1,
				ANSI_RESET,
				(pMacro->Enabled() ? ' ' : '*'),
				(LPCSTR)strTemp);
			strMessage += strText;
			pMacro = (CMacro *)pSession->GetMacros()->GetNext();
			nLineCount++;
		}
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}
	return TRUE;
}

BOOL CCommandTable::Math(CSession *pSession, CString &strLine)
{
	CString strVar, strMath;
	if(!CParseUtils::FindStatement(pSession,  strLine,strVar))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strMath))
		return FALSE;

	if (strVar.IsEmpty() || strMath.IsEmpty())
		return TRUE;

	CParseUtils::ReplaceVariables(pSession, strVar);
	CParseUtils::ReplaceVariables(pSession, strMath);
	if(strMath.Find(".") == -1)
	{
		long lResult;
		if (CParseUtils::CalculateMath(pSession, strMath,lResult))
		{
			CString strValue;
			strValue.Format("%ld",lResult);

			std::string name = static_cast<LPCTSTR>( strVar );
			std::string value = static_cast<LPCTSTR>( strValue );

			pSession->GetVariables()->Add( name, value, "" );
		}
		else
			pSession->QueueMessage(CMessages::MM_MATH_MESSAGE, "Error in math formula.");
	}
	else
	{
		double lResult;
		if (CParseUtils::CalculateMath(pSession, strMath,lResult))
		{
			CString strValue;
			strValue.Format("%.4f",lResult);

			std::string name = static_cast<LPCTSTR>( strVar );
			std::string value = static_cast<LPCTSTR>( strValue );

			pSession->GetVariables()->Add( name, value, "" );
		}
		else
			pSession->QueueMessage(CMessages::MM_MATH_MESSAGE, "Error in math formula.");
	}

	return TRUE;
}

BOOL CCommandTable::MaxOnOpen(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession,  strLine, strParam))
		return FALSE;


	if(strParam.IsEmpty())
	{
		pSession->GetOptions().sessionInfo_.MaxOnOpen(!pSession->GetOptions().sessionInfo_.MaxOnOpen());

		CString strMessage;
		strMessage.Format("The window will be displayed %s when the session is next opened.", pSession->GetOptions().sessionInfo_.MaxOnOpen() ? "maximized" : "normal");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().sessionInfo_.MaxOnOpen(TRUE);
		CString strMessage;
		strMessage.Format("The window will be displayed %s when the session is next opened.", pSession->GetOptions().sessionInfo_.MaxOnOpen() ? "maximized" : "normal");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().sessionInfo_.MaxOnOpen(FALSE);
		CString strMessage;
		strMessage.Format("The window will be displayed %s when the session is next opened.", pSession->GetOptions().sessionInfo_.MaxOnOpen() ? "maximized" : "normal");

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /maxonopen.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage);
	return TRUE;
}
BOOL CCommandTable::MaxRestore(CSession *pSession, CString &strLine)
{
	strLine.Empty();
	pSession->GetHost()->MaxRestoreSession();
	return TRUE;
}
BOOL CCommandTable::Message(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;


	if (strParam1.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You can toggle messages of the following types");
		CString strMessage = "     Actions\n     Aliases\n     Arrays\n     Bar\n	Enable\n	Events\n     Items\n     Lists\n     Macros\n     TabList\n     Variables\n";
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}
	strParam1.MakeLower();
	CString strText;

	if (CParseUtils::IsPartial(strParam1,"actions"))
	{
		if (pSession->GetOptions().messageOptions_.ActionMessages())
			pSession->GetOptions().messageOptions_.ActionMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.ActionMessages(TRUE);
		strText.Format("Messages concerning actions are now %s.",
			(pSession->GetOptions().messageOptions_.ActionMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"aliases"))
	{
		if (pSession->GetOptions().messageOptions_.AliasMessages())
			pSession->GetOptions().messageOptions_.AliasMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.AliasMessages(TRUE);
		strText.Format("Messages concerning aliases are now %s.",
			(pSession->GetOptions().messageOptions_.AliasMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if(CParseUtils::IsPartial(strParam1, "allon"))
	{
		pSession->GetOptions().messageOptions_.AliasMessages(TRUE);
		pSession->GetOptions().messageOptions_.ArrayMessages(TRUE);
		pSession->GetOptions().messageOptions_.ActionMessages(TRUE);
		pSession->GetOptions().messageOptions_.BarMessages(TRUE);
		pSession->GetOptions().messageOptions_.EnableMessages(TRUE);
		pSession->GetOptions().messageOptions_.EventMessages(TRUE);
		pSession->GetOptions().messageOptions_.GagMessages(TRUE);
		pSession->GetOptions().messageOptions_.HighMessages(TRUE);
		pSession->GetOptions().messageOptions_.ItemMessages(TRUE);
		pSession->GetOptions().messageOptions_.ListMessages(TRUE);
		pSession->GetOptions().messageOptions_.MacroMessages(TRUE);
		pSession->GetOptions().messageOptions_.SubMessages(TRUE);
		pSession->GetOptions().messageOptions_.TabMessages(TRUE);
		pSession->GetOptions().messageOptions_.VariableMessages(TRUE);
	}

	if(CParseUtils::IsPartial(strParam1, "alloff"))
	{
		pSession->GetOptions().messageOptions_.AliasMessages(FALSE);
		pSession->GetOptions().messageOptions_.ArrayMessages(FALSE);
		pSession->GetOptions().messageOptions_.ActionMessages(FALSE);
		pSession->GetOptions().messageOptions_.BarMessages(FALSE);
		pSession->GetOptions().messageOptions_.EnableMessages(FALSE);
		pSession->GetOptions().messageOptions_.EventMessages(FALSE);
		pSession->GetOptions().messageOptions_.GagMessages(FALSE);
		pSession->GetOptions().messageOptions_.HighMessages(FALSE);
		pSession->GetOptions().messageOptions_.ItemMessages(FALSE);
		pSession->GetOptions().messageOptions_.ListMessages(FALSE);
		pSession->GetOptions().messageOptions_.MacroMessages(FALSE);
		pSession->GetOptions().messageOptions_.SubMessages(FALSE);
		pSession->GetOptions().messageOptions_.TabMessages(FALSE);
		pSession->GetOptions().messageOptions_.VariableMessages(FALSE);
	}

	if (CParseUtils::IsPartial(strParam1,"arrays"))
	{
		if (pSession->GetOptions().messageOptions_.ArrayMessages())
			pSession->GetOptions().messageOptions_.ArrayMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.ArrayMessages(TRUE);
		strText.Format("Messages concerning arrays are now %s.",
			(pSession->GetOptions().messageOptions_.ArrayMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"bar"))
	{
		if (pSession->GetOptions().messageOptions_.BarMessages())
			pSession->GetOptions().messageOptions_.BarMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.BarMessages(TRUE);
		strText.Format("Messages concerning the status bar are now %s.",
			(pSession->GetOptions().messageOptions_.BarMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"enable"))
	{
		if (pSession->GetOptions().messageOptions_.EnableMessages())
			pSession->GetOptions().messageOptions_.EnableMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.EnableMessages(TRUE);
		strText.Format("Messages concerning message enables are now %s.",
			(pSession->GetOptions().messageOptions_.EnableMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"events"))
	{
		if (pSession->GetOptions().messageOptions_.EventMessages())
			pSession->GetOptions().messageOptions_.EventMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.EventMessages(TRUE);
		strText.Format("Messages concerning events are now %s.",
			(pSession->GetOptions().messageOptions_.EventMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"gag"))
	{
		if (pSession->GetOptions().messageOptions_.GagMessages())
			pSession->GetOptions().messageOptions_.GagMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.GagMessages(TRUE);
		strText.Format("Messages concerning Gags are now %s.",
			(pSession->GetOptions().messageOptions_.GagMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"highlight"))
	{
		if (pSession->GetOptions().messageOptions_.HighMessages())
			pSession->GetOptions().messageOptions_.HighMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.HighMessages(TRUE);
		strText.Format("Messages concerning the status bar are now %s.",
			(pSession->GetOptions().messageOptions_.HighMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"items"))
	{
		if (pSession->GetOptions().messageOptions_.ItemMessages())
			pSession->GetOptions().messageOptions_.ItemMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.ItemMessages(TRUE);
		strText.Format("Messages concerning items are now %s",
			(pSession->GetOptions().messageOptions_.ItemMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"lists"))
	{
		if (pSession->GetOptions().messageOptions_.ListMessages())
			pSession->GetOptions().messageOptions_.ListMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.ListMessages(TRUE);
		strText.Format("Messages concerning lists are now %s",
			(pSession->GetOptions().messageOptions_.ListMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"macros"))
	{
		if (pSession->GetOptions().messageOptions_.MacroMessages())
			pSession->GetOptions().messageOptions_.MacroMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.MacroMessages(TRUE);
		strText.Format("Messages concerning macros are now %s.",
			(pSession->GetOptions().messageOptions_.MacroMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"substitute"))
	{
		if (pSession->GetOptions().messageOptions_.SubMessages())
			pSession->GetOptions().messageOptions_.SubMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.SubMessages(TRUE);
		strText.Format("Messages concerning the status bar are now %s.",
			(pSession->GetOptions().messageOptions_.SubMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"tablist"))
	{
		if (pSession->GetOptions().messageOptions_.TabMessages())
			pSession->GetOptions().messageOptions_.TabMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.TabMessages(TRUE);
		strText.Format("Messages concerning the tab list are now %s.",
			(pSession->GetOptions().messageOptions_.TabMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}

	if (CParseUtils::IsPartial(strParam1,"variables"))
	{
		if (pSession->GetOptions().messageOptions_.VariableMessages())
			pSession->GetOptions().messageOptions_.VariableMessages(FALSE);
		else
			pSession->GetOptions().messageOptions_.VariableMessages(TRUE);
		strText.Format("Messages concerning variables are now %s.",
			(pSession->GetOptions().messageOptions_.VariableMessages() ? "on" : "off"));
		pSession->QueueMessage(CMessages::MM_ENABLE_MESSAGE, strText);
		return TRUE;
	}
	return TRUE;
}

BOOL CCommandTable::MMLoadLibrary(CSession *pSession, CString &strLine)
{
	bool result = false;

	CString lib;
	CString path;
	CString sParams;
	std::stringstream message;

	CStatementParser parser(pSession);
	if (parser.GetTwoStatements(strLine, lib, false, path, false))
	{
		if(!parser.GetOneStatement(strLine, sParams, false)) sParams = "";

		if(!lib.IsEmpty())
		{
			if(path.IsEmpty()) path = lib;
			path = MMFileUtils::fixPath(path);

			if( pSession->GetDlls().add(pSession, lib, path, sParams) )
			{
				message << "Successfully loaded the DLL named [" << lib << "]";
				result = true;
			}
			else
			{
				message << "Failed to load the dll named [" << lib << "]";
			}
		}
		else
		{
			message << "LoadLibrary called without specifying a DLL to load";
		}
	}
	else
	{
		message << "Failed to parse the parameters to LoadLibrary";
	}

	pSession->QueueMessage(CMessages::MM_DLL_MESSAGE, message.str().c_str());
	return result ? TRUE : FALSE;
}

BOOL CCommandTable::Msp(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CString strPath;
		if(!CParseUtils::FindStatement(pSession,  strLine,strPath))
			return FALSE;

		CParseUtils::ReplaceVariables(pSession, strPath);
		strPath = strPath.MakeLower();
		if (strPath.IsEmpty())
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "MSP path cleared.");
			((CIfxApp*)AfxGetApp())->MSP().Clear();
			pSession->GetOptions().mspOptions_.On(FALSE);
		}
		else
		{
			if (strPath[0] == '+')
			{
				CString strTemp;
				strTemp = ((CIfxApp*)AfxGetApp())->MSP().MatchWildcard(strPath.Right(strPath.GetLength()-1),MATCH_WAVE);
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strTemp);
			}
			else if (strPath == "?")
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "MSP Files Found:");

				CString strFile;

				strFile = ((CIfxApp*)AfxGetApp())->MSP().GetFirstWave();
				CString strMessage;
				while(!strFile.IsEmpty())
				{
					strMessage += strFile;
					strMessage += "\n";
					strFile = ((CIfxApp*)AfxGetApp())->MSP().GetNextWave();
				}
				strFile = ((CIfxApp *)AfxGetApp())->MSP().GetFirstMidi();
				while(!strFile.IsEmpty())
				{
					strMessage += strFile;
					strMessage += "\n";
					strFile = ((CIfxApp*)AfxGetApp())->MSP().GetNextMidi();
				}
				pSession->PrintAnsiNoProcess(strMessage);
			}
			else if (strPath == "off")
			{
				pSession->GetOptions().mspOptions_.On(FALSE);
				pSession->QueueMessage(
					CMessages::MM_GENERAL_MESSAGE, 
					"MSP processing off.");
			}
			else if(strPath == "gather" || strPath == "on")
			{
				pSession->QueueMessage(
					CMessages::MM_GENERAL_MESSAGE, 
					"MudMaster 2k6 uses your Sound Path for the MSP Root directory.");

				std::string path;
				if(FAILED(pSession->GetOptions().pathOptions_.Get_SoundPath(path)))
				{
					ATLASSERT(FALSE);
				}
				else
				{
					if(path.empty())
					{
						CString strEmptySoundPath;
						if(!strEmptySoundPath.LoadString(IDS_EMPTYSOUNDPATH))
						{
							strEmptySoundPath = _T("You must set your sound path");
						}

						pSession->QueueMessage(
							CMessages::MM_GENERAL_MESSAGE,
							strEmptySoundPath);
					}
					else
					{
						CString strMessage;
						strMessage.Format(
							_T("Gathering MSP Files from %s"), 
							path.c_str());

						pSession->QueueMessage(
							CMessages::MM_GENERAL_MESSAGE, 
							strMessage);

						((CIfxApp*)AfxGetApp())->MSP().GatherFiles(path.c_str());
						pSession->QueueMessage(
							CMessages::MM_GENERAL_MESSAGE, 
							"Done gathering files.");

						pSession->GetOptions().mspOptions_.On(TRUE);
					}
				}
			}
			else
			{
				CString strMessage;
				strMessage.Format(
					_T("Unknown subcommand on /msp: %s"),
					strPath);

				pSession->QueueMessage(
					CMessages::MM_GENERAL_MESSAGE, 
					strMessage);
			}
		}
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return SUCCEEDED(hr);
}

BOOL CCommandTable::Pace(CSession *pSession, CString &strLine)
{
	CString strPace;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPace))
		return FALSE;

	if (!strPace.IsEmpty())
		pSession->GetOptions().sessionInfo_.Pace(atoi(strPace));
	CString strTemp;
	strTemp.Format("SpeedWalk pacing set at: %d",pSession->GetOptions().sessionInfo_.Pace());
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strTemp);
	return TRUE;
}

BOOL CCommandTable::PeekConnections(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,  strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().PeekConnections(strName);

	return TRUE;
}

BOOL CCommandTable::Ping(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,  strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().Ping(strName);

	return TRUE;
}

BOOL CCommandTable::PlayMidi(CSession *pSession, CString &strLine)
{
	CString strMidi;
	if(!CParseUtils::FindStatement(pSession,  strLine,strMidi))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strMidi);

	// An empty string means we want to stop the current midi
	// file from playing.
	if (strMidi.IsEmpty())
	{
		if (((CIfxApp*)AfxGetApp())->MidiOpen())
		{
			mciSendString("close MudMaster", NULL, NULL, NULL);
			((CIfxApp*)AfxGetApp())->MidiOpen(FALSE);
		}
		return TRUE;
	}

	// Make sure to close it if we already have one open.
	if (((CIfxApp*)AfxGetApp())->MidiOpen())
	{
		mciSendString("close MudMaster", NULL, NULL, NULL);
		((CIfxApp*)AfxGetApp())->MidiOpen(FALSE);
	}

	// Create the send string for opening the file.
	CString strSend;
	strSend.Format(_T("open %s type sequencer alias MudMaster"),
		(LPCSTR)strMidi);
	
	// Returns 0 if successful.
	if (mciSendString(strSend, NULL, NULL, NULL))
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Cannot open the midi file.");
		return TRUE;
	}

	if (mciSendString("play MudMaster", NULL, NULL, NULL))
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Unable to play the midi file.");
		mciSendString("close MudMaster", NULL, NULL, NULL);
		return TRUE;
	}

	((CIfxApp*)AfxGetApp())->MidiOpen(TRUE);
	return TRUE;
}

BOOL CCommandTable::PlayWav(CSession *pSession, CString &strLine)
{
	CString strWav;
	if(!CParseUtils::FindStatement(pSession,  strLine,strWav))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strWav);
	if (strWav.IsEmpty())
		return TRUE;

	pSession->PlayASound(strWav);
	return TRUE;
}

BOOL CCommandTable::Presubstitute(CSession *pSession, CString &strLine)
{
	strLine.Empty();
	if (pSession->GetOptions().sessionInfo_.PreSub())
	{
		pSession->GetOptions().sessionInfo_.PreSub(FALSE);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Substitutes will be processed after actions and gags.");
	}
	else
	{
		pSession->GetOptions().sessionInfo_.PreSub(TRUE);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Substitutes will be processed before actions and gags.");
	}
	return TRUE;
}

BOOL CCommandTable::PromptOverwrite(CSession *pSession, CString &strLine)
{
	CString strParam;
	if(!CParseUtils::FindStatement(pSession,  strLine, strParam))
		return FALSE;

    if(strParam.IsEmpty())
	{
		pSession->GetOptions().sessionInfo_.PromptOverwrite(!pSession->GetOptions().sessionInfo_.PromptOverwrite());

		CString strMessage;
		strMessage.Format("You will %s be prompted when a file is about to be overwritten.", pSession->GetOptions().sessionInfo_.PromptOverwrite() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, strMessage);
		return TRUE;
	}

	strParam.MakeLower();
	if(!strParam.Compare("true"))
	{
		pSession->GetOptions().sessionInfo_.PromptOverwrite(TRUE);
		CString strMessage;
		strMessage.Format("You will %s be prompted when a file is about to be overwritten.", pSession->GetOptions().sessionInfo_.PromptOverwrite() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, strMessage);
		return TRUE;
	}

	if(!strParam.Compare("false"))
	{
		pSession->GetOptions().sessionInfo_.PromptOverwrite(FALSE);
		CString strMessage;
		strMessage.Format("You will %s be prompted when a file is about to be overwritten.", pSession->GetOptions().sessionInfo_.PromptOverwrite() ? "now" : "no longer");

		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, strMessage);
		return TRUE;
	}

	CString strMessage;
	strMessage = "Invalid paramater sent to /promptoverwrite.  Valid arguments are [true, false or no argument at all]";
	pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, strMessage);

	return TRUE;
}

BOOL CCommandTable::Read(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString strParameter;
		CParseUtils::FindStatement(pSession, strLine, strParameter);
		CParseUtils::ReplaceVariables(pSession, strParameter);

		CPath path;
		ErrorHandlingUtils::TESTHR(CGlobals::FixPath(path, strParameter));

		CString strPath(path.m_strPath);
		CStdioFile file;

		CFileException fe;
		if (!file.Open(
			strPath,
			CFile::modeRead|CFile::typeText,
			&fe))
		{
			CGlobals::OpenFileError(pSession, fe);
			::OutputDebugString(strPath);    //in case findstatement or replacevariables mangled path
			return TRUE;
		}

		bool bAct = pSession->GetOptions().messageOptions_.ActionMessages();
		bool bAlias = pSession->GetOptions().messageOptions_.AliasMessages();
		bool bArray = pSession->GetOptions().messageOptions_.ArrayMessages();
		bool bAuto = pSession->GetOptions().messageOptions_.AutoexecMessages();
		bool bBar = pSession->GetOptions().messageOptions_.BarMessages();
		bool bEnable = pSession->GetOptions().messageOptions_.EnableMessages();
		bool bEvent = pSession->GetOptions().messageOptions_.EventMessages();
		bool bGag = pSession->GetOptions().messageOptions_.GagMessages();
		bool bHigh = pSession->GetOptions().messageOptions_.HighMessages();
		bool bItem = pSession->GetOptions().messageOptions_.ItemMessages();
		bool bList = pSession->GetOptions().messageOptions_.ListMessages();
		bool bMacro = pSession->GetOptions().messageOptions_.MacroMessages();
		bool bSub = pSession->GetOptions().messageOptions_.SubMessages();
		bool bTab = pSession->GetOptions().messageOptions_.TabMessages();
		bool bVar = pSession->GetOptions().messageOptions_.VariableMessages();

		pSession->GetOptions().messageOptions_.AllOff();

		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Reading command file...");

		BOOL bFirstLine = TRUE;

		CScriptProgressDlg dlg;

		dlg.Create(IDD_SCRIPT_PROGRESS, NULL);

		if(!CGlobals::CenterWindow(dlg.GetSafeHwnd(), AfxGetMainWnd()->GetSafeHwnd()))
		{
			ASSERT(FALSE);
		}

		dlg.ShowWindow(SW_SHOWNORMAL);
		dlg.BringWindowToTop();
		dlg.SetWindowText("Reading script...");
		dlg.UpdateWindow();
		short nLines = 0;

		CString strInput, strLineToContinue;
		while(file.ReadString(strInput))
			nLines++;

		dlg.m_ctlProgress.SetRange(0, nLines);
		dlg.m_ctlProgress.SetColor(MudmasterColors::WINDOWS_LIGHTBLUE);
		file.SeekToBegin();

		int nPos = 0;
		pSession->ReadMode(TRUE);
		while(file.ReadString(strInput))
		{
			dlg.m_ctlProgress.SetPos(nPos);
			dlg.UpdateWindow();
			if (strInput.IsEmpty())
				continue;

			// This will assume the first character of the first
			// line has the command character.  This should allow
			// people with any command char to load a command file.
			if (bFirstLine)
			{
				bFirstLine = FALSE;
				strInput.TrimLeft();
				CGlobals::SetCommandCharacter(strInput.GetAt(0));
			}
			
			////trim leading whitespace, append to string we are building,
			//strLineToContinue.Append(strInput.TrimLeft(" \t"));
			////check for line continuation
			//if(strLineToContinue.Right(1) == "\\" || strLineToContinue.Right(1) == "_")
			//{
			//	//remove the continuation character and don't execute yet.
			//	strLineToContinue = strLineToContinue.Left(strLineToContinue.GetLength()-1);
			//	//strip trailing whitespace
			//	strLineToContinue.TrimRight(" \t");
			//}
			strLineToContinue.Append(strInput);
			if(strLineToContinue.Right(1) == "\\" || strLineToContinue.Right(1) == "_")
			{
				//add the return and don't execute yet.
				strLineToContinue.AppendChar('\r');
			}
			else
			{
				pSession->ExecuteCommand( strLineToContinue );
				strLineToContinue.Empty();
			}

			nPos++;
		}

		
		pSession->ReadMode(FALSE);
		dlg.DestroyWindow();
		file.Close();

		pSession->GetOptions().messageOptions_.ActionMessages(bAct);
		pSession->GetOptions().messageOptions_.AliasMessages(bAlias);
		pSession->GetOptions().messageOptions_.ArrayMessages(bArray);
		pSession->GetOptions().messageOptions_.AutoexecMessages(bAuto);
		pSession->GetOptions().messageOptions_.BarMessages(bBar);
		pSession->GetOptions().messageOptions_.EnableMessages(bEnable);
		pSession->GetOptions().messageOptions_.EventMessages(bEvent);
		pSession->GetOptions().messageOptions_.GagMessages(bGag);
		pSession->GetOptions().messageOptions_.HighMessages(bHigh);
		pSession->GetOptions().messageOptions_.ItemMessages(bItem);
		pSession->GetOptions().messageOptions_.ListMessages(bList);
		pSession->GetOptions().messageOptions_.MacroMessages(bMacro);
		pSession->GetOptions().messageOptions_.SubMessages(bSub);
		pSession->GetOptions().messageOptions_.TabMessages(bTab);
		pSession->GetOptions().messageOptions_.VariableMessages(bVar);

		if (pSession->GetOptions().messageOptions_.ShowInfo())
		{
			CString notUsed;
			Information(pSession, notUsed);
		}

		if(pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return TRUE;
}
BOOL CCommandTable::ReadViaWeb(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		CString strURL;
		CParseUtils::FindStatement(pSession, strLine, strURL);
		CParseUtils::ReplaceVariables(pSession, strURL);


		bool bAct = pSession->GetOptions().messageOptions_.ActionMessages();
		bool bAlias = pSession->GetOptions().messageOptions_.AliasMessages();
		bool bArray = pSession->GetOptions().messageOptions_.ArrayMessages();
		bool bAuto = pSession->GetOptions().messageOptions_.AutoexecMessages();
		bool bBar = pSession->GetOptions().messageOptions_.BarMessages();
		bool bEnable = pSession->GetOptions().messageOptions_.EnableMessages();
		bool bEvent = pSession->GetOptions().messageOptions_.EventMessages();
		bool bGag = pSession->GetOptions().messageOptions_.GagMessages();
		bool bHigh = pSession->GetOptions().messageOptions_.HighMessages();
		bool bItem = pSession->GetOptions().messageOptions_.ItemMessages();
		bool bList = pSession->GetOptions().messageOptions_.ListMessages();
		bool bMacro = pSession->GetOptions().messageOptions_.MacroMessages();
		bool bSub = pSession->GetOptions().messageOptions_.SubMessages();
		bool bTab = pSession->GetOptions().messageOptions_.TabMessages();
		bool bVar = pSession->GetOptions().messageOptions_.VariableMessages();

		pSession->GetOptions().messageOptions_.AllOff();
		strLine = "Reading command file...";
		strLine.Append(strURL);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strLine);


		CHttpConnection *pHttpServer = NULL;
		CHttpFile *pHttpFile = NULL;
		DWORD dwServiceType;
		CString strServerName, strLineToContinue;
		CString strObject;
		INTERNET_PORT nPort;

		try
		{
			CInternetSession isVersion("HTTPREAD");
			if (!AfxParseURL(strURL, 
				dwServiceType, strServerName, strObject, nPort))
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error reading command file... invalid URL");
				return(1);
			}
			pHttpServer = isVersion.GetHttpConnection(strServerName,nPort);
			if (!pHttpServer)
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error reading command file... host/port failure");
				return(1);
			}
			pHttpFile = pHttpServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
				strObject,NULL,1,NULL,NULL,
				INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_NO_AUTO_REDIRECT);
			if (!pHttpFile)
			{
				pHttpServer->Close();
				delete pHttpServer;
				pHttpServer = NULL;
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error reading command file... connection failure");
				return(1);
			}
			if (!pHttpFile->AddRequestHeaders("Accept: */*\r\n"))
			{
				pHttpServer->Close();
				delete pHttpServer;
				pHttpServer = NULL;
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error reading command file... connection failure");
				return(1);
			}
			pHttpFile->SetReadBufferSize(2048);
			pHttpFile->SendRequest();

			while(pHttpFile->ReadString(strLine))
			{
				if (!strLine.IsEmpty())
				{
					strLineToContinue.Append(strLine);
					if(strLineToContinue.Right(1) == "\\" || strLineToContinue.Right(1) == "_")
					{
						//add the return and don't execute yet.
						strLineToContinue.AppendChar('\r');
					}
					else
					{
						pSession->ExecuteCommand( strLineToContinue );
						strLineToContinue.Empty();
					}
				}
			} 

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
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Error reading command file... inet exception");
			return(1);
		}



		pSession->GetOptions().messageOptions_.ActionMessages(bAct);
		pSession->GetOptions().messageOptions_.AliasMessages(bAlias);
		pSession->GetOptions().messageOptions_.ArrayMessages(bArray);
		pSession->GetOptions().messageOptions_.BarMessages(bBar);
		pSession->GetOptions().messageOptions_.EnableMessages(bEnable);
		pSession->GetOptions().messageOptions_.EventMessages(bEvent);
		pSession->GetOptions().messageOptions_.GagMessages(bGag);
		pSession->GetOptions().messageOptions_.HighMessages(bHigh);
		pSession->GetOptions().messageOptions_.ItemMessages(bItem);
		pSession->GetOptions().messageOptions_.ListMessages(bList);
		pSession->GetOptions().messageOptions_.MacroMessages(bMacro);
		pSession->GetOptions().messageOptions_.SubMessages(bSub);
		pSession->GetOptions().messageOptions_.TabMessages(bTab);
		pSession->GetOptions().messageOptions_.VariableMessages(bVar);

		if (pSession->GetOptions().messageOptions_.ShowInfo())
		{
			CString notUsed;
			Information(pSession, notUsed);
		}

		if(pSession->GetOptions().sessionInfo_.ShowStatusBar())
			pSession->RedrawStatusBar();

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return TRUE;
}

BOOL CCommandTable::Remark(
	CSession * /*pSession*/, 
	CString & /*&strLine*/)
{

	return TRUE;
}

BOOL CCommandTable::RequestConnects(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,  strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);
	if (strName.IsEmpty())
		return TRUE;

	pSession->GetChat().RequestConnects(strName);

	return TRUE;
}

BOOL CCommandTable::ResetEvent(CSession *pSession, CString &strLine)
{
	CString strIndex;
	if(!CParseUtils::FindStatement(pSession,  strLine,strIndex))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strIndex);
	if (strIndex.IsEmpty())
		return TRUE;

	if (strIndex == "*")
	{
		time_t tNow = time(NULL);
		CMMEvent *pEvent = (CMMEvent *)pSession->GetEvents()->GetFirst();
		while(pEvent)
		{
			pEvent->Started(tNow);
			pEvent = (CMMEvent *)pSession->GetEvents()->GetNext();
		}
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "All events reset.");
		return TRUE;
	}

	CMMEvent *pEvent;
	int nIndex = atoi(strIndex);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strIndex);

	if (pEvent)
	{
		pEvent->Started(time(NULL));
		pSession->QueueMessage(CMessages::MM_EVENT_MESSAGE, "Event reset.");
	}
	return TRUE;
}


BOOL CCommandTable::SaveScrollback(CSession *pSession, CString &strLine)
{
	CString strFilename;
	if(!CParseUtils::FindStatement(pSession, strLine, strFilename))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strFilename);

	if(strFilename.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "[/savescrollback]: You must supply a filename with the [/savescrollback] command");
		return TRUE;
	}

	pSession->SaveScrollBack(strFilename);
	return TRUE;
}

BOOL CCommandTable::ScrollGrep(CSession *pSession, CString &strLine)
{
	CString strParameter1;
	CString strParameter2;
	CParseUtils::FindStatement(pSession, strLine, strParameter1);
	CParseUtils::FindStatement(pSession, strLine, strParameter2);

	if(strParameter1.IsEmpty() && strParameter2.IsEmpty())
	{
		CString strMessage(_T("You must supply a search pattern"));
		pSession->QueueMessage(
			CMessages::MM_GENERAL_MESSAGE, 
			strMessage);
	}
	else if (strParameter2.IsEmpty())
	{
		pSession->ScrollGrep(1000, strParameter1); //limit to 1000
	}
	else if(!strParameter1.IsEmpty() && !strParameter2.IsEmpty())
	{
		ULONG nLines = _ttoi(strParameter1);
		if(0 == nLines)
		{
			// bad... num lines must be > 0 .. spam an error message
		}

		pSession->ScrollGrep(nLines, strParameter2);
	}
	return TRUE;
}
BOOL CCommandTable::ScreenShot(CSession *pSession, CString &strLine)
{
	CString strFilename;
	
    CStatementParser parser(pSession);
	if(parser.GetOneStatement(strLine, strFilename, false) && strFilename.IsEmpty()) 
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must specify the filename.");
		return FALSE;
	}
	pSession->ScreenShot(strFilename);
	return TRUE;
}

BOOL CCommandTable::ScrollBack(CSession *pSession, CString &strLine)
{
	CString strParameter1, strScrollAction;
	CParseUtils::FindStatement(pSession, strLine, strParameter1);

	if(strParameter1.IsEmpty())
	{
		pSession->ScrollPageUp();
	}
	else
	{
		strScrollAction = strParameter1.MakeLower();
		if(strScrollAction == "lineup")
		{  
			pSession->ScrollLineUp();
		}
		else if(strScrollAction == "linedown")
		{  
			pSession->ScrollLineDown();
		}
		else if(strScrollAction == "pageup")
		{  
			pSession->ScrollPageUp();
		}
		else if(strScrollAction == "pagedown")
		{  
			pSession->ScrollPageDown();
		}
		else if(strScrollAction == "home")
		{  
			pSession->ScrollHome();
		}
		else if(strScrollAction == "end")
		{  
			pSession->EndScrollBack();
		}

	}

	return TRUE;
}

BOOL CCommandTable::SendAlias(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strAlias;
	if(!CParseUtils::FindStatement( pSession, strLine, strPerson ))
		return FALSE;

	if(!CParseUtils::FindStatement( pSession, strLine, strAlias ))
		return FALSE;

	CParseUtils::ReplaceVariables( pSession, strPerson );
	CParseUtils::ReplaceVariables( pSession, strAlias );
	if (strPerson.IsEmpty() || strAlias.IsEmpty())
		return TRUE;

	pSession->GetAliases()->SendAlias( 
		CGlobals::GetCommandCharacter(), strAlias, 
		static_cast<LPCTSTR>(strPerson), pSession->GetChat() );

	return TRUE;
}

BOOL CCommandTable::SendArray(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strArray;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strArray))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strArray);
	if (strPerson.IsEmpty() || strArray.IsEmpty())
		return TRUE;

	CMMArray *pArray;
	int nIndex = atoi(strArray);
	if (nIndex)
		pArray = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		pArray = (CMMArray *)pSession->GetArrays()->FindExact(strArray);

	if (pArray == NULL)
		return TRUE;

	CString strText;
	pArray->MapToCommand(strText);

	pSession->GetChat().SendCommand(MMChatServer::CommandTypeArray, strPerson, strText);

	CString strTemp;
	CString strPrepare;
	if (pArray->SingleDim())
	{
		for (int i=1;i<=pArray->Dim(0);i++)
		{
			pSession->GetArrays()->GetItem(pArray,i,0,strTemp);
			CGlobals::PrepareForWrite(strTemp,strPrepare);
			if (!strPrepare.IsEmpty())
			{
				strText.Format("%cassign {%s} {%d} {%s}",
					CGlobals::GetCommandCharacter(),
					(LPCSTR)pArray->Name(),
					i,
					(LPCSTR)strPrepare);

				pSession->GetChat().SendCommand(MMChatServer::CommandTypeArray, strPerson, strText);
			}
		}
	}
	else
	{
		for (int i=1;i<=pArray->Dim(0);i++)
		{
			for (int j=1;j<=pArray->Dim(1);j++)
			{
				pSession->GetArrays()->GetItem(pArray,i,j,strTemp);
				CGlobals::PrepareForWrite(strTemp,strPrepare);
				if (!strPrepare.IsEmpty())
				{
					strText.Format(_T("%cassign {%s} {%d,%d} {%s}"),
						CGlobals::GetCommandCharacter(),
						(LPCSTR)pArray->Name(),
						i,j,
						(LPCSTR)strPrepare);

					pSession->GetChat().SendCommand(MMChatServer::CommandTypeArray, strPerson, strText);
				}
			}
		}
	}

	return TRUE;
}

BOOL CCommandTable::SendBarItem(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strItem;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strItem))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strItem);
	if (strPerson.IsEmpty() || strItem.IsEmpty())
		return TRUE;

	CBarItem *pItem;
	int nIndex = atoi(strItem);
	if (nIndex)
		pItem = (CBarItem *)pSession->GetStatusBar()->GetAt(nIndex-1);
	else
		pItem = pSession->GetStatusBar()->FindItem(strItem);

	if (pItem == NULL)
		return TRUE;

	CString strText;
	pItem->MapToCommand(strText);

	pSession->GetChat().SendCommand(MMChatServer::CommandTypeBarItem, strPerson, strText);
	return TRUE;
}

BOOL CCommandTable::SendEvent(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strEvent;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strEvent))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strEvent);
	if (strPerson.IsEmpty() || strEvent.IsEmpty())
		return TRUE;

	CMMEvent *pEvent;
	int nIndex = atoi(strEvent);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strEvent);

	if (pEvent == NULL)
		return TRUE;

	CString strText;
	pEvent->MapToCommand(strText);
	pSession->GetChat().SendCommand(MMChatServer::CommandTypeEvent, strPerson, strText);
	return TRUE;
}

BOOL CCommandTable::SendFile(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strFilename;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strFilename))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strFilename);
	if (strPerson.IsEmpty() || strFilename.IsEmpty())
		return TRUE;

	pSession->GetChat().SendFile(strPerson, strFilename);
	return TRUE;
}

BOOL CCommandTable::SendGag(CSession *pSession, CString &strLine)
{
	SendTrigger(pSession, strLine, *(pSession->GetGags()), MMChatServer::CommandTypeGag);
	return TRUE;
}


BOOL CCommandTable::SendHighlight(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strHigh;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strHigh))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strHigh);
	if (strPerson.IsEmpty() || strHigh.IsEmpty())
		return TRUE;

	CHigh *pHigh;
	int nIndex = atoi(strHigh);
	if (nIndex)
		pHigh = (CHigh *)pSession->GetHighlights()->GetAt(nIndex-1);
	else
		pHigh = (CHigh *)pSession->GetHighlights()->FindExact(strHigh);

	if (pHigh == NULL)
		return TRUE;

	CString strText;
	pHigh->MapToCommand(strText);
	pSession->GetChat().SendCommand(MMChatServer::CommandTypeHighlight, strPerson, strText);
	return TRUE;
}

BOOL CCommandTable::SendList(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strList;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strList))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strList);
	if (strPerson.IsEmpty() || strList.IsEmpty())
		return TRUE;

	CMMList *pList;
	int nIndex = atoi(strList);
	if (nIndex)
		pList = (CMMList *)pSession->GetLists()->GetAt(nIndex-1);
	else
		pList = pSession->GetLists()->FindByName(strList);

	if (pList == NULL)
		return TRUE;

	CString strText;
	pList->MapToCommand(strText);

	pSession->GetChat().SendCommand(MMChatServer::CommandTypeList, strPerson, strText);

	// Write the items in the list out.
	int nCount = pList->Items().GetCount();
	for(int i = 0; i < nCount; ++i)
	{
		CString listItem = pList->Items().GetAt(i);
		strText.Format("%citemadd {%s} {%s}",
			CGlobals::GetCommandCharacter(),
			(LPCSTR)pList->ListName(),
			(LPCSTR)listItem);

		pSession->GetChat().SendCommand(MMChatServer::CommandTypeList, strPerson, strText);
	}
	return TRUE;
}

BOOL CCommandTable::SendMacro(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strMacro;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strMacro))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strMacro);
	if (strPerson.IsEmpty() || strMacro.IsEmpty())
		return TRUE;

	CMacro *pMac = pSession->GetMacros()->FindKeyByName(strMacro);
	if (pMac == NULL)
		return TRUE;

	CString strText;
	pMac->MapToCommand(strText);

	pSession->GetChat().SendCommand(MMChatServer::CommandTypeMacro, strPerson, strText);
	return TRUE;
}

BOOL CCommandTable::SendSub(CSession *pSession, CString &strLine)
{
	SendTrigger(pSession, strLine, *(pSession->GetSubs()), MMChatServer::CommandTypeSubstitute);
	return TRUE;
}

BOOL CCommandTable::SendVariable(CSession *pSession, CString &strLine)
{
	CString strPerson;
	CString strVar;
	if(!CParseUtils::FindStatement(pSession,  strLine,strPerson))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strVar))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strPerson);
	CParseUtils::ReplaceVariables(pSession, strVar);
	if (strPerson.IsEmpty() || strVar.IsEmpty())
		return TRUE;

	CVariable v;
	int nIndex = atoi(strVar);
	if (0 != nIndex)
	{
		if(0 > nIndex || nIndex > pSession->GetVariables()->GetSize())
		{
			CString strFormat;
			if(!strFormat.LoadString(IDS_INVALID_VARIABLE_INDEX))
				strFormat = _T("Invalid variable index [%d]");

			CString strMessage;
			strMessage.Format(strFormat, nIndex);

			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, strMessage);
			return TRUE;
		}
		else
		{
			v = pSession->GetVariables()->GetAt(nIndex-1);
		}
	}
	else
	{
		std::string name = static_cast<LPCTSTR>( strVar );
		if( !pSession->GetVariables()->FindByName( name, v ) )
		{
			CString strFormat;
			if(!strFormat.LoadString(IDS_VARNAME_NOT_FOUND))
				strFormat = _T("Variable name not found [%s]");

			CString strMessage;
			strMessage.Format(strFormat, strVar);

			pSession->QueueMessage(CMessages::MM_VARIABLE_MESSAGE, strMessage);
			return TRUE;
		}
	}

	CString strTemp;
	v.MapToCommand(strTemp);

	pSession->GetChat().SendCommand(MMChatServer::CommandTypeVariable, strPerson, strTemp);

	return TRUE;
}

BOOL CCommandTable::Session(CSession *pSession, CString &strLine)
{
	CString strSessionName, strSessionAddress, strSessionPort;

	if(!CParseUtils::FindStatement(pSession,  strLine, strSessionName))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine, strSessionAddress))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine, strSessionPort))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strSessionName);
	CParseUtils::ReplaceVariables(pSession, strSessionAddress);
	CParseUtils::ReplaceVariables(pSession, strSessionPort);

	if(strSessionName.IsEmpty())
	{
		CString strMessage, strText;
		strMessage = "Session information for this session:";
		strText.Format("     Session Name: %s", pSession->GetOptions().sessionInfo_.SessionName());
		strMessage += strText;
		strText.Format("     Address: %s", pSession->GetOptions().sessionInfo_.Address());
		strMessage += strText;
		strText.Format("     Port: %d", pSession->GetOptions().sessionInfo_.Port());
		strMessage += strText;
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	if(strSessionAddress.IsEmpty())
	{
		strSessionAddress = pSession->GetOptions().sessionInfo_.Address();
		if(strSessionAddress.IsEmpty())
		{
			CString strMessage = "You must specify an address for this session.\n";
			strMessage = "\te.g. /session {session name} {address} {port}";
			pSession->PrintAnsiNoProcess(strMessage);
			return TRUE;
		}
	}

	if(!strSessionPort.IsEmpty())
	{
		SessionPort(pSession, strSessionPort);
	}

	strSessionName = pSession->GetOptions().sessionInfo_.SessionName();
	SessionName(pSession, strSessionName);
	SessionAddress(pSession, strSessionAddress);
	pSession->ConnectToMud();
	return TRUE;
}

BOOL CCommandTable::SessionAddress(CSession *pSession, CString &strLine)
{
	CString strAddress;
	if(!CParseUtils::FindStatement(pSession,  strLine, strAddress))
		return FALSE;

	if(strAddress.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must provide an address. eg: /sessionaddress {xxx.xxx.xxx.xxx}", TRUE);
		return TRUE;
	}

	pSession->GetOptions().sessionInfo_.Address(strAddress);
	CString strMessage;
	strMessage.Format("Session address set to [%s]", strAddress);
	if (pSession->GetOptions().messageOptions_.ShowInfo())
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		}
	return TRUE;
}

BOOL CCommandTable::SessionName(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,  strLine, strName))
		return FALSE;

	if(strName.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must provide a name. eg: /sessionname {MySession}", TRUE);
		return TRUE;
	}

	pSession->GetOptions().sessionInfo_.SessionName(strName);
	CString strMessage;
	strMessage.Format("Session name set to [%s]", strName);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, FALSE);
	return TRUE;
}

BOOL CCommandTable::SessionPath(CSession *pSession, CString &/*strLine*/)
{
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, _T("SessionPath command has been deprecated"));

	return TRUE;
}

BOOL CCommandTable::SessionPort(CSession *pSession, CString &strLine)
{
	CString strPort;
	if(!CParseUtils::FindStatement(pSession,  strLine, strPort))
		return FALSE;

	if(strPort.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must provide a port. eg: /sessionport {4000}", TRUE);
		return TRUE;
	}

	int nPort = atoi(strPort);
	pSession->GetOptions().sessionInfo_.Port(nPort);
	CString strMessage;
	strMessage.Format("Session port set to [%d]", nPort);
	if (pSession->GetOptions().messageOptions_.ShowInfo())
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage, TRUE);
		}
	return TRUE;
}

BOOL CCommandTable::SetEventTime(CSession *pSession, CString &strLine) 
{
	CString strEvent;
	CString strTime;

	if(!CParseUtils::FindStatement(pSession,  strLine,strEvent))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strTime))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strEvent);
	CParseUtils::ReplaceVariables(pSession, strTime);

	int nTime = atoi(strTime);
	if (nTime < 1)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid event time.");
		return TRUE;
	}

	CMMEvent *pEvent;
	int nIndex = atoi(strEvent);
	if (nIndex)
		pEvent = (CMMEvent *)pSession->GetEvents()->GetAt(nIndex-1);
	else
		pEvent = (CMMEvent *)pSession->GetEvents()->FindExact(strEvent);

	if (pEvent != NULL)
	{
		pEvent->Seconds(nTime);
		//pEvent->Elapse(nTime);
		//pEvent->Started(_dwTime);
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Event time changed.");
	}
	else
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Event not found.");
	return TRUE;
}


BOOL CCommandTable::SetIP(CSession *pSession, CString &strLine)
{
	CString strIP;
	if(!CParseUtils::FindStatement(pSession,  strLine,strIP))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strIP);

	if (strIP.IsEmpty())
		return TRUE;

	CGlobals::SetIPAddress(strIP);

	CString strText;
	strText.Format("IP Address Changed: %s",(LPCSTR)strIP);
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strText,FALSE);
	return TRUE;
}
//BOOL CCommandTable::SetFocus(CSession *pSession, CString &strLine)
//{
//	CMainFrame* pMainFrame2 = new CMainFrame;
//	if (!pMainFrame2->LoadFrame(IDR_MAINFRAME))
//		return FALSE;
//	pMainFrame2->ShowWindow(1);
//	pMainFrame2->UpdateWindow();
////	pMainFrame2->ModifyStyle(0,WS_EX_TOPMOST,SWP_FRAMECHANGED);
//	
//	
//	CString strIP;
//	CParseUtils::FindStatement(pSession,  strLine,strIP);
//	
//	//pSession->GetHost()->ThisWnd()->SetFocus();
//	return TRUE;
//}

BOOL CCommandTable::ShowMeViaWeb(CSession *pSession, CString &strLine) 
{
	CString strLowerLine(strLine);
	strLowerLine.MakeLower();
	if(strLowerLine.Find("flushqueue")==0 || strLowerLine.Find("flushqueue")==1)
	{
		pSession->GetShowmeViaWebBuffer().RemoveAll();
		CChangesThread::m_cti.nThreadStatus = THREAD_CHANGES_DONE;
		::OutputDebugString(_T( " Flushing ShowmeViaWeb Queue\n") );
		return TRUE;
	}
	else if(strLowerLine.Find("showqueue")==0 || strLowerLine.Find("showqueue")==1)
	{
		if(!pSession->GetShowmeViaWebBuffer().IsEmpty())
		{

			for(POSITION pos = pSession->GetShowmeViaWebBuffer().GetHeadPosition();pos;)
			{
				pSession->PrintAnsi(pSession->GetShowmeViaWebBuffer().GetAt(pos), true);
				pSession->GetShowmeViaWebBuffer().GetNext(pos);
			}

		}
		return TRUE;
	}

	if (strLowerLine.Find("releasequeue")==0 || strLowerLine.Find("releasequeue")==1)
	{
		if(!pSession->GetShowmeViaWebBuffer().IsEmpty())
		{
			strLine = pSession->GetShowmeViaWebBuffer().GetHead();
			strLowerLine = strLine;
			strLowerLine.MakeLower();
            pSession->GetShowmeViaWebBuffer().RemoveHead();
		}
		else
			return TRUE;
	}

	if(CChangesThread::m_cti.nThreadStatus ==  THREAD_CHANGES_RUNNING)
		pSession->GetShowmeViaWebBuffer().AddTail(strLine);
	else if (strLowerLine.Find("queue ")==0 || strLowerLine.Find("queue ")==1)
	{
		if(strLine.Find("{") == 0)
			pSession->GetShowmeViaWebBuffer().AddTail("{" + strLine.Mid(7));
		else
			pSession->GetShowmeViaWebBuffer().AddTail(strLine.Mid(6));
		return TRUE;
	}
	else if (!strLine.IsEmpty())
	{

		if(strLowerLine.Find("http://") == -1)
		{
			if(strLine.Find("{") == 0)
                strLine.Insert(1,"http://");
			else
				strLine.Insert(0,"http://");
		}
		CCommandTable::Changes(pSession, strLine);
	}
	return TRUE;
}
BOOL CCommandTable::ShowMe(CSession *pSession, CString &strLine) 
{
	CString strProcess;

	BOOL retVal = FALSE;

	CString strText;
	try
	{
		if(CParseUtils::FindStatement(pSession,  strLine, strText))
		{
			strText += _T(ANSI_RESET); // _T("@AnsiReset()");
			CParseUtils::ReplaceVariables(pSession, strText);
			//if no second parameter is passed print text without processing
			if(CParseUtils::FindStatement(pSession,  strLine, strProcess))
			{
				//be careful only do trigger processing if second parameter == true
				CParseUtils::ReplaceVariables(pSession, strProcess);
			}
			if(strProcess.MakeLower() == "true")
				pSession->PrintAnsi(strText,true);
			else
				pSession->PrintAnsi(strText,false);


			retVal = TRUE;
		}

		return retVal;
	}
	catch(...)
	{
		::OutputDebugString(_T( " Caught Error in ShowMe.\n") );
		return TRUE;
	}
}

BOOL CCommandTable::ShowOther(CSession *pSession, CString &strLine) 
{
	ASSERT(NULL != pSession);

	CString window;
    CString text;
    CString noreset = "";
	CString processTriggers = "";
	CStatementParser parser(pSession);
	if(CParseUtils::FindStatement(pSession,  strLine, window))
		CParseUtils::ReplaceVariables(pSession, window);
	if(CParseUtils::FindStatement(pSession,  strLine, text))
		CParseUtils::ReplaceVariables(pSession, text);
	if(CParseUtils::FindStatement(pSession,  strLine, noreset))
		CParseUtils::ReplaceVariables(pSession, noreset);
	if(CParseUtils::FindStatement(pSession,  strLine, processTriggers))
		CParseUtils::ReplaceVariables(pSession, processTriggers);


	if(noreset != "noreset")		
        text += _T(ANSI_RESET); // default is to add ansi reset
	if(processTriggers.MakeLower() == "true" || (noreset.MakeLower() == "true" && processTriggers == ""))
		pSession->GetHost()->SendTextToAnotherWindow(window, text,true);
	else
		pSession->GetHost()->SendTextToAnotherWindow(window, text,false);

	return TRUE;
}
BOOL CCommandTable::ShowToast(CSession *pSession, CString &strLine) 
{
	CString strText;
	CParseUtils::ReplaceVariables(pSession, strLine);
	// not testing for whether statement is found just using it to remove { } if there
	CParseUtils::FindStatement(pSession,  strLine,strText);
	pSession->TaskbarNotification(strText);
	
	return TRUE;
}

BOOL CCommandTable::Snoop(CSession *pSession, CString &strLine)
{
	CString strName;
	if(!CParseUtils::FindStatement(pSession,  strLine,strName))
		return FALSE;

	CParseUtils::ReplaceVariables(pSession, strName);

	pSession->GetChat().Snoop(strName);

	return TRUE;
}

BOOL CCommandTable::SoundPath(CSession *pSession, CString &strLine)
{
	HRESULT hr = E_UNEXPECTED;
	try
	{
		CString strPath;
		if(!CParseUtils::FindStatement(pSession,  strLine,strPath))
			return FALSE;

		CParseUtils::ReplaceVariables(pSession, strPath);

		strPath.Trim();
		if(strPath.IsEmpty())
		{
			CString strMessage;
			std::string strSoundPath;
			pSession->GetOptions().pathOptions_.Get_SoundPath(strSoundPath);
			strPath = strSoundPath.c_str();
			if (!strPath.IsEmpty())
			{
				strMessage.Format("The sound path is:  %s",
									strSoundPath.c_str());
			}
			else
			{
				strMessage =_T("The sound path is not set")	;			
			}
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage,FALSE);
			return TRUE;

		}
		else
		{
			ErrorHandlingUtils::TESTHR(pSession->GetOptions().pathOptions_.Put_SoundPath(strPath));

			pSession->QueueMessage(
				CMessages::MM_GENERAL_MESSAGE, 
				CString("Sound path changed to: " + strPath));
		}
	
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return SUCCEEDED(hr);
}

BOOL CCommandTable::SpeedWalk(CSession *pSession, CString &strLine)
{
	CString strParam1;
	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	strParam1.MakeLower();
	if (strParam1.IsEmpty())
	{
		if (pSession->GetOptions().sessionInfo_.SpeedWalk())
			pSession->GetOptions().sessionInfo_.SpeedWalk(FALSE);
		else
			pSession->GetOptions().sessionInfo_.SpeedWalk(TRUE);
	}
	else
		if (!CString("on").Find(strParam1))
			pSession->GetOptions().sessionInfo_.SpeedWalk(TRUE);
		else
			if (!CString("off").Find(strParam1))
				pSession->GetOptions().sessionInfo_.SpeedWalk(FALSE);

	CString strTemp;
	strTemp.Format("Speed walking is %s.",
		(pSession->GetOptions().sessionInfo_.SpeedWalk() ? "on" : "off"));
	pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strTemp);
	return TRUE;
}

BOOL CCommandTable::StartChatServer(
	CSession *pSession, 
	CString &/*strLine*/)
{
	pSession->StartChatServer();
	return TRUE;
}

BOOL CCommandTable::StatusBar(CSession *pSession, CString &strLine)
{
	CString strPosition;

	CParseUtils::FindStatement(pSession, strLine, strPosition);
	CParseUtils::ReplaceVariables(pSession, strPosition);

	if(strPosition.IsEmpty())
	{
		BOOL bStatBar = pSession->GetOptions().statusOptions_.IsVisible();
		pSession->GetOptions().statusOptions_.SetVisible(!bStatBar);
		if(pSession->GetOptions().statusOptions_.IsVisible())
			pSession->ShowStatusBar();
		else
			pSession->HideStatusBar();
		return TRUE;
	}

	strPosition.MakeLower();
	if(!strPosition.Compare("above"))
	{
		pSession->GetOptions().statusOptions_.SetStatusbarPos(CStatusBarOptions::STATUS_POS_ABOVE);
	}
	else if(!strPosition.Compare("below"))
	{
		pSession->GetOptions().statusOptions_.SetStatusbarPos(CStatusBarOptions::STATUS_POS_BELOW);
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid parameter passed to /statusbar");
		return TRUE;
	}

	pSession->UpdateStatusBarOptions();
	return TRUE;
}
BOOL CCommandTable::StatusBarSize(CSession *pSession, CString &strLine)
{
	CString strLines;
	CString strWidth;

	CParseUtils::FindStatement(pSession, strLine, strLines);
	CParseUtils::FindStatement(pSession, strLine, strWidth);
	CParseUtils::ReplaceVariables(pSession, strLines);
	CParseUtils::ReplaceVariables(pSession, strWidth);

	if(strLines.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "correct usage: /statusbarsize {lines high}{characters wide}");
		return TRUE;
	}
	int nLines;
	int nWidth;
	nLines = atoi(strLines);	
	
	if(strWidth.IsEmpty())
	{
		nWidth = pSession->GetOptions().statusOptions_.GetStatusBarWidth();
		if (!nWidth)
			nWidth = 120;
	}
	else
	{
		nWidth = atoi(strWidth);
	}

	if(nLines && nWidth)
	{
		pSession->GetOptions().statusOptions_.SetStatusBarLines(nLines);
		pSession->GetOptions().statusOptions_.SetStatusBarWidth(nWidth);
	}
	else
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Invalid parameter passed to /statusbarsize");
		return TRUE;
	}

	pSession->UpdateStatusBarOptions();
	return TRUE;
}

BOOL CCommandTable::StopChatServer(
	CSession *pSession, 
	CString &/*strLine*/)
{
	pSession->StopChatServer();
	return TRUE;
}

BOOL CCommandTable::SubDepth(CSession *pSession, CString &strLine)
{
	CString strDepth;

	if(!CParseUtils::FindStatement(pSession, strLine, strDepth))
		return FALSE;


	if(strDepth.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must supply a value between 1 and 0 to the /subdepth command");
		return TRUE;
	}

	int nDepth = atoi(strDepth);

	if(nDepth < 1  || nDepth > 10)
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "You must supply a value between 1 and 0 to the /subdepth command");
		return TRUE;
	}

	pSession->GetOptions().sessionInfo_.SubDepth(static_cast<USHORT>(nDepth));
	return TRUE;
}

BOOL CCommandTable::Substitute(CSession *pSession, CString &strLine)
{
	CString strParam1, strParam2, strGroup;
	CString strTemp;

	if(!CParseUtils::FindStatement(pSession,  strLine,strParam1))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strParam2))
		return FALSE;

	if(!CParseUtils::FindStatement(pSession,  strLine,strGroup))
		return FALSE;


	CParseUtils::ReplaceVariables(pSession, strParam1);
	CParseUtils::ReplaceVariables(pSession, strGroup);
	CParseUtils::PretranslateString(pSession, strParam2);

	if (!strParam1.IsEmpty() && !strParam2.IsEmpty())
	{
		CTrigger *pSub = pSession->GetSubs()->Add(strParam1,strParam2,strGroup);
		if (pSub != NULL)
		{
			pSession->QueueMessage(CMessages::MM_SUB_MESSAGE, "Substitute added.",FALSE);
		}
		else
			pSession->QueueMessage(CMessages::MM_SUB_MESSAGE, "Substitute not added.");
		return TRUE;
	}

	if (strParam1.IsEmpty() && strParam2.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_SUB_MESSAGE, "Defined Subs:",FALSE);
		CString strMessage;
		pSession->GetSubs()->PrintList(strMessage);
		pSession->PrintAnsiNoProcess(strMessage);
		return TRUE;
	}

	if (strParam2.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Defined Substitute:");
		CString strText;
		pSession->GetSubs()->PrintTrigger(strParam1, strText);
		if(!strText.IsEmpty())
		{
			pSession->PrintAnsiNoProcess(strText);
			return TRUE;
		}

		int nIndex = atoi(strParam1);
		if(nIndex == 0)
		{
			pSession->PrintAnsiNoProcess("No substitute found.");
			return TRUE;
		}

		pSession->GetSubs()->PrintTrigger(nIndex, strText);
		if(!strText.IsEmpty())
		{
			pSession->PrintAnsiNoProcess(strText);
			return TRUE;
		}
	}
	return TRUE;
}
BOOL CCommandTable::SQLExecute(CSession *pSession, CString &strLine)
{
	CString strSQLString, strValue, strDBData;
	CString strConnectString,strDBUser;
	if(!CParseUtils::FindStatement(pSession,strLine,strSQLString))
		return FALSE;
	CParseUtils::ReplaceVariables(pSession, strSQLString);

	if (strSQLString.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE , "Syntax /SQLExecute {SQL Statement}{Database Connect String}{DB User}{DB Password}");
		return TRUE;
	}
	try
	{

// Take the rest of the input params and setup the database connection
			CStatementParser dbParser(pSession);
			ODBC::ODBCConnection DB;
			dbParser.GetDBConnection(strLine,DB);
		ODBC::ODBCStmt Stmt(DB);
		if(Stmt == INVALID_HANDLE_VALUE)
		{
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "DB connection failed.");
			return TRUE;
		}
		int nRet=Stmt.Query(strSQLString);
		if(nRet)
		{
		strDBData.Format(" %d changed rows",Stmt.GetChangedRowCount());
		}
		else
		{
			strDBData.Format("Error executing sql statement");
			pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strDBData);
		}
			if(pSession->GetOptions().messageOptions_.ShowInfo())
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strDBData);
		Stmt.Free();
		DB.Disconnect();
	}
	catch(...)
	{
		//::OutputDebugString(  strSQLString + _T( " Caught Error in SQLExecute.\n") );
		strSQLString.Empty();

	}
	return TRUE;
}
BOOL CCommandTable::SQLQuery(CSession *pSession, CString &strLine)
{
	CString strSQLString,strArray, strColNames, strValue, strDBData;
	if(!CParseUtils::FindStatement(pSession,strLine,strSQLString))
		return FALSE;
	CParseUtils::FindStatement(pSession,strLine, strArray);
	CParseUtils::ReplaceVariables(pSession, strArray);
	CParseUtils::ReplaceVariables(pSession, strSQLString);
	strColNames = strArray;
	strColNames.Append("Fields");

	if (strArray.IsEmpty())
	{
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE , "Syntax /SQLQuery {SQL Statement}{Array Name}{Database Connect String}{DB Password}");
		return TRUE;
	}
	CMMArray *pColNames;
	int nIndex = atoi(strArray);
	if (nIndex)
		pColNames = (CMMArray *)pSession->GetArrays()->GetAt(nIndex-1);
	else
		pColNames = (CMMArray *)pSession->GetArrays()->FindExact(strArray);

	if (!pColNames == NULL)
		pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "Array already exists. Remove it with /unarray or use different name");
	else
	{

		try
		{
			int nRows, nCols, i=1,j=0;;
// Take the rest of the input params and setup the database connection
			CStatementParser dbParser(pSession);
			ODBC::ODBCConnection DB;
			dbParser.GetDBConnection(strLine,DB);
			ODBC::ODBCStmt Stmt(DB);
			if(Stmt == INVALID_HANDLE_VALUE)
			{
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, "DB connection failed.");
				return TRUE;
			}
			int nRet=Stmt.Query(strSQLString);
			long cbLength=0;
			int iDataType=0;

			nCols = Stmt.GetColumnCount();
			pColNames = pSession->GetArrays()->Add(strColNames,TRUE,nCols,0,CString("DBResults"));

			ODBC::ODBCRecord rec(Stmt);
			while(i< Stmt.GetColumnCount()+1)
			{
				TCHAR ColContent[255] = _T("");
				rec.GetColumnName(i,ColContent,sizeof(ColContent));
				strValue.Format("%s",ColContent);
				pColNames->SetValue(i,0,strValue);
				i++;
			}
			if(nRet)
			{
				i=1;
				j=1;
				nRows=0;
				CString strTemp1 = "";
				TCHAR strTemp[10255] = _T("");
			    long iTemp = 0;
			    long double uTemp = 0;
				double fTemp = 0;
				BOOL bTemp = TRUE;
				void* pvData;

				if(Stmt.FetchFirst())
				{
					do 
					{
						nRows++;
					}while(Stmt.FetchNext());
					CMMArray *pResultSet = pSession->GetArrays()->Add(strArray,FALSE,nRows,nCols,CString("DBResults"));
					Stmt.FetchFirst();
					do {
						while(i< Stmt.GetColumnCount()+1)
						{
							//TCHAR ColContent[255] = _T("");
							iDataType = rec.GetColumnType(i);
							switch(iDataType)
							{
							case 4:
							case 5:
								pvData = &iTemp;
							rec.GetData(i,pvData,rec.GetColumnSize(i),&cbLength,iDataType);
								break;
							case 6:
							case 8:
							case 2:
							case 3:
							case 7:
							case 9:
							case 93:
								pvData = &fTemp;
							rec.GetData(i,pvData,rec.GetColumnSize(i),&cbLength,iDataType);
								break;
							case -7:
								pvData = &bTemp;
								rec.GetData(i,pvData,1,&cbLength,iDataType);
							//break;

							default:
								pvData = &iTemp;
							rec.GetData(i,strTemp,sizeof(strTemp),0);
							}

							//rec.GetData(i,&strTemp,10,&cbLength,iDataType);
							//strValue.Format("%d",strData); // GetBuffer(0));
							//strValue.Format("%d",strData); // GetBuffer(0));
							
							//strValue.Format("%d",(LPCSTR)strData.GetBufferSetLength(cbLength)); // GetBuffer(0));
							switch(iDataType)
							{
							case 0:
							case 1:								
								strValue.Format("%s",strTemp);
								break; //ODBC::sqlDataTypeChar:
							case 2: //ODBC::sqlDataTypeNumeric:
							case 3: //ODBC::sqlDataTypeDecimal:
							case 6: //ODBC::sqlDataTypeFloat:
							case 7: //ODBC::sqlDataTypeReal:
							case 8: //ODBC::sqlDataTypeDouble:
								strValue.Format("%f",fTemp);
								break;
							case 4: //ODBC::sqlDataTypeInteger:
							case 5: //ODBC::sqlDataTypeSmallInt:
								strValue.Format("%d",iTemp);
								break;
							case 9: //ODBC::sqlDataTypeDateTime:
							case 93: //ODBC::sqlDataTypeDateTime:
								strValue.Format("%f",fTemp);
								break;
								//strValue.Format("%d",iTemp);
								//break;
							case -7: //ODBC::sqlDataTypeBoolean:
								 bTemp ? strValue = "True" : strValue = "False";
								break;
							default:
								strValue.Format("%s",strTemp);
								//strValue.Format("%d",pvData);
							}

								
							pResultSet->SetValue(j,i,strValue);
							i++;
						}
						j++;
						i=1;
					}while(Stmt.FetchNext());
					Stmt.Free();
					strDBData.Format("%d records read and stored in %s. Field names stored in %s\n",nRows,strArray,strColNames);
				}
				//Stmt.Free();
				DB.Disconnect();
			}
			else
			{
				strDBData.Format("Error executing sql query");
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strDBData);
			}
			if(pSession->GetOptions().messageOptions_.ShowInfo())
				pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE,strDBData);
		}
		catch(...)
		{
			//::OutputDebugString(_T("Caught error in SQLQuery.\n"));
			strDBData.Empty();
		}
	}
	return TRUE;
}
