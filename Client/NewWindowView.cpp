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
#include "NewWindowView.h"
#include "IfxDoc.h"
#include "Sess.h"
#include "Globals.h"
#include "MudSocket.h"
#include "Messages.h"
#include "ParseUtils.h"
#include "SeException.h"
#include "CommandTable.h"
#include "ProcedureTable.h"
#include "Alias.h"
#include "Telnet.h"
#include "ColorLine.h"
#include "Action.h"
#include "Macro.h"
#include "Event.h"
#include "ChangesThread.h"
#include "ObString.h"
#include "ChatContext.h"
#include "LogDialog.h"
#include "ActionListDlg.h"
#include "ActionOnlyListDlg.h"
#include "AliasListDlg.h"
#include "EventListDlg.h"
#include "MacroListDlg.h"
#include "GagListDlg.h"
#include "HighListDlg.h"
#include "SubListDlg.h"
#include "Tab.h"
#include "BarItemListDlg.h"
#include "GlobalSoundManager.h"
#include "dxutil.h"
#include "SessionInfo.h"
#include "TerminalOptions.h"
#include "EventList.h"
#include "DebugStack.h"
#include "MacroList.h"
#include "ChatServer.h"
#include "HighList.h"
#include "SubList.h"
#include "GagList.h"
#include "ActionList.h"
#include "AliasList.h"
#include <string>
#include "OptionsSheet.h"
#include "DefaultOptions.h"
#include "MainFrm.h"
#include "ErrorHandling.h"
#include "FileUtils.h"
#include "DllList.h"

#include "QPerformanceTimer.h"

#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <cctype>

using namespace MMScript;
using namespace MudmasterColors;
using namespace Telnet;
using namespace SerializedOptions;
using namespace EditorDialogs;
using namespace std;
using namespace Utilities;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class HandleSingleLineFunc
{
public:
	HandleSingleLineFunc( CNewWindowView *view )
		: _view(view)
	{
	}

	void operator()( const CString &line )
	{
		_view->HandleOneLine( line );
	}

private:
	CNewWindowView *_view;
};
/////////////////////////////////////////////////////////////////////////////
// CNewWindowView

namespace GlobalVariables
{
	CSession *g_pCommandHandler  = NULL;
	char *g_pszCurrentLineBuffer = NULL;
	CCharFormat *g_pszCurrentColorBuffer = NULL;
}


IMPLEMENT_DYNCREATE(CNewWindowView, CView)

CNewWindowView::CNewWindowView()
: speedWalking_(false)
, _lastLineGagged(false)
, _logging(false)
, _RawMudlogging(false)
, _changesTimer(false)
, _commandScrollPos(NULL)
, _scrollDirection(SCROLL_DIRECTION_NOTSCROLLING)
, _inputHeight(19)
, _statusBarVisible(true)
, _verbatim(false)
, _speedWalkTimer(NULL)
{
}

CNewWindowView::~CNewWindowView()
{
	if( Logging() )
	{
		if(_pLogFile.get() != NULL)
		{
			CloseLog(FALSE);
		}
	}
		if( RawMudLogging() )
	{
		if(_pRawMudLogFile.get() != NULL)
		{
			CloseRawMudLog(FALSE);
		}
	}
}

BEGIN_MESSAGE_MAP(CNewWindowView, CView)
	//{{AFX_MSG_MAP(CNewWindowView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_SESSION_CONNECT, OnSessionConnect)
	ON_UPDATE_COMMAND_UI(ID_SESSION_CONNECT, OnUpdateSessionConnect)
	ON_COMMAND(ID_SESSION_ZAP, OnSessionZap)
	ON_UPDATE_COMMAND_UI(ID_SESSION_ZAP, OnUpdateSessionZap)
	ON_COMMAND(ID_SESSION_READSCRIPT, OnSessionReadscript)
	ON_COMMAND(ID_SESSION_LOG, OnSessionLog)
	ON_COMMAND(ID_RAW_MUD_LOG, OnSessionRawMudLog)
	ON_UPDATE_COMMAND_UI(ID_SESSION_LOG, OnUpdateSessionLog)
	ON_COMMAND(ID_SESSION_SAVESCROLLBACK, OnSessionSavescrollback)
	ON_COMMAND(ID_SESSION_STARTCHATSERVER, OnSessionStartchatserver)
	ON_UPDATE_COMMAND_UI(ID_SESSION_STARTCHATSERVER, OnUpdateSessionStartchatserver)
	ON_COMMAND(ID_SESSION_STOPCHATSERVER, OnSessionStopchatserver)
	ON_UPDATE_COMMAND_UI(ID_SESSION_STOPCHATSERVER, OnUpdateSessionStopchatserver)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_CONFIGURATION_ACTIONSTRIGGERS, OnConfigurationActionstriggers)
	ON_COMMAND(ID_ALIAS_TREE, OnAliasTree)
	ON_COMMAND(ID_CONFIGURATION_ACTIONS, OnConfigurationActions)
	ON_COMMAND(ID_CONFIGURATION_ALIASES, OnConfigurationAliases)
	ON_COMMAND(ID_CONFIGURATION_EVENTS, OnConfigurationEvents)
	ON_COMMAND(ID_CONFIGURATION_MACROS, OnConfigurationMacros)
	ON_COMMAND(ID_SCRIPT_GAGS, OnConfigurationGags)
	ON_COMMAND(ID_SCRIPT_HIGHLIGHTS, OnConfigurationHighs)
	ON_COMMAND(ID_SCRIPT_SUBSTITUTIONS, OnConfigurationSubs)
	ON_NOTIFY(EN_MSGFILTER, INPUT_WINDOW_ID, OnInputMsgFilter)
	ON_NOTIFY(EN_REQUESTRESIZE, INPUT_WINDOW_ID, OnRequestresizeInputCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_CONFIGURATION_STATUSBARITEMS, OnConfigurationStatusbaritems)
	ON_WM_DESTROY()
	ON_COMMAND(ID_INPUTCONTEXT_COPY, OnInputcontextCopy)
	ON_COMMAND(ID_INPUTCONTEXT_CUT, OnInputcontextCut)
	ON_COMMAND(ID_INPUTCONTEXT_PASTE, OnInputcontextPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CMessages::MSG_SOCKET_INCOMING, ReceiveFromMud)
	ON_MESSAGE(CMessages::MSG_SOCKET_CLOSE, CloseMud)
	ON_MESSAGE(CMessages::MSG_SOCKET_CONNECT, OnConnectToMud)
	ON_MESSAGE(CMessages::HANDLE_COMMAND, OnDLLCommand)
	ON_MESSAGE(CMessages::MM_LOAD_SCRIPT, OnLoadScript)
	ON_MESSAGE(CMessages::PRINT_MESSAGE, OnPrintMessage)
	ON_COMMAND_RANGE(ID_MACRO_START, ID_MACRO_END, OnMacroKey)
	//ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

//BOOL CNewWindowView::OnWndMsg(
 //  UINT message,
 //  WPARAM wParam,
 //  LPARAM lParam,
 //  LRESULT* pResult 
  // ) {
//	   return S_OK;
//   }

/////////////////////////////////////////////////////////////////////////////
// CNewWindowView drawing

void CNewWindowView::OnDraw(CDC* /*pDC*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CNewWindowView diagnostics
#ifdef _DEBUG
void CNewWindowView::AssertValid() const
{
	CView::AssertValid();
}

void CNewWindowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewWindowView message handlers
int CNewWindowView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Terminal().SetDocument(GetDocument());

	if(!Terminal().Create(
		NULL,
		NULL,
		WS_CHILD | WS_VISIBLE,
		CRect(0,0,7, 100),
		this,
		SPLITTER_WINDOW_ID,
		NULL))
	{		
		ASSERT(FALSE);
		return FALSE;
	}

	if(!StatusWindow().Create(
		NULL,
		((CIfxApp *)AfxGetApp())->GetStatusWindowClassName(),
		WS_CHILD | WS_VISIBLE ,
		CRect(0,0, 100, 100),
		this,
		STATUS_WINDOW_ID,
		NULL))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	StatusWindow().HookEvents(GetDocument()->Session());
	DWORD inputStyle;
//	if(GetDocument()->Session()->GetOptions().inputOptions_.ShowBorder())
//		inputStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE;
//	else
		inputStyle = WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE;

	if(!Input().Create(
		inputStyle,
		CRect(0,0,100,100),
		this,
		INPUT_WINDOW_ID))
	{
		ASSERT(FALSE);
		return FALSE;
	}


	Input().SetEventMask(ENM_REQUESTRESIZE|ENM_KEYEVENTS|ENM_MOUSEEVENTS|ENM_SCROLLEVENTS);
	Input().RequestResize();

	hookEvent(GetDocument()->Session());
	hookEvent(&Terminal());

	return 0;
}

void CNewWindowView::OnSize(UINT nType, int cx, int cy) 
{
	if(GetDocument()->Session() == NULL)
		return;

	CView::OnSize(nType, cx, cy);
	
	if(this->GetSafeHwnd()	== NULL)
		return;

	if(Input().GetSafeHwnd() == NULL)
		return;

	if(StatusWindow().GetSafeHwnd() == NULL)
		return;

	RecalcLayout();

}

void CNewWindowView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	UpdateAllOptions();
	//Override the buffer length for the display part of the splitter
	//The scrollback remains large but this prevents huge duplication
	Terminal().SetDisplayBufferSize(300);
	if(GetDocument()->Session()->GetOptions().sessionInfo_.StartChatOnOpen())
	{
		GetDocument()->Session()->StartChatServer();
	}

	StartTimer();
	ChangesTimer(false);

	if(GetDocument()->Session()->GetOptions().sessionInfo_.MaxOnOpen())
	{
		GetParentFrame()->ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		CRect SessionRect, ParentRect, OtherRect, OwnerRect;
		GetParentFrame()->GetParentOwner()->GetWindowRect(&SessionRect);
			GetParentFrame()->GetParentOwner()->ScreenToClient(&SessionRect);
			SessionRect.top += (long)GetDocument()->Session()->GetOptions().sessionInfo_.Top()-26 ;
			SessionRect.left += (long)GetDocument()->Session()->GetOptions().sessionInfo_.Left() -2 ;
			SessionRect.bottom += (long)GetDocument()->Session()->GetOptions().sessionInfo_.Bottom()-26 ;
			SessionRect.right += (long)GetDocument()->Session()->GetOptions().sessionInfo_.Right() -2 ;
			if(SessionRect.bottom > 50)
			{
				//if top and bottom are 30 then position has not been saved so don't do this
				// and if they are less than 30 the window might be completely hidden
				GetParentFrame()->MoveWindow(SessionRect,true);
			//}
		}
	}

	CommandScrollPos(GetDocument()->Session()->GetCommandHistoryBuffer().GetHeadPosition());

	ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
	//GetDocument()->Session()->EndScrollBack(this);
	Terminal().EndScrollBack();
	Input().SetFocus();

	if(GetDocument()->Session()->GetOptions().mspOptions_.UsePlaySound()==FALSE)
		InitializeDirectSound();
		m_wndTaskbarNotifier.Create(this);
	if(!m_wndTaskbarNotifier.SetSkin("Toast.bmp",0,255,0))
		::OutputDebugString(_T("Error loading toast skin\n"));

	int iSize;  //for toast font size
	
	try
	{
		//Bunch of hoop jumping to get the toast font based on terminal font
		LOGFONT lf = {0};
		// grab the size from the main font used but make it 3 points bigger
		iSize = 100;
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowFont()->GetLogFont(&lf);

		if (lf.lfHeight < 0)
			lf.lfHeight = -lf.lfHeight;

		HDC hDC    = ::GetDC(NULL);
		iSize  = MulDiv(lf.lfHeight,720,GetDeviceCaps(hDC, LOGPIXELSY)) + 30;
		::ReleaseDC(NULL,hDC);
	}
	catch(...)
	{
		iSize = 130;
	}

	//hard coding font face for now.
	m_wndTaskbarNotifier.SetTextFont("Comic Sans MS",iSize,TN_TEXT_NORMAL,TN_TEXT_BOLD);
	m_wndTaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotifier.SetTextRect(CRect(15,40,m_wndTaskbarNotifier.m_nSkinWidth-15,
                                 m_wndTaskbarNotifier.m_nSkinHeight-5));
//TODO: KW test adding session level "system array"
	ProcessOneCommand(_T("/array {RegexSubMatches}{20}{Regex}"));
}

void CNewWindowView::UpdateAllOptions()
{
	COptions &options = GetDocument()->Session()->GetOptions();

	Terminal().UpdateAllOptions(options.terminalOptions_);

	GetDocument()->Session()->GetChat().SetChatName(
		options.chatOptions_.ChatName());

	Input().UpdateAllOptions(options.inputOptions_);

	UpdateStatusBarOptions();
}

void CNewWindowView::StartTimer()
{
	if (SetTimer(ID_TIMER_EVENT,1000,NULL) != ID_TIMER_EVENT)
	{
		PrintMessage("# A timer was not available. Events will not be processed.");
	}
}
void CNewWindowView::StartFastTimer(int nTime)
{
	if (SetTimer(ID_FAST_TIMER,nTime,NULL) != ID_FAST_TIMER)
	{
		PrintMessage("# A fast timer was not available. Events will not be processed.");
	}
}
void CNewWindowView::StopFastTimer()
{
	KillTimer(ID_FAST_TIMER);
}
void CNewWindowView::PrintMessage(const char *strText)
{
	Terminal().AvertAppendNext();
	CString strMessage;
	strMessage.Format("%s%s%s%s%s",
		ANSI_RESET,
		ANSI_FORE_COLORS[GetDocument()->Session()->GetOptions().colorOptions_.GetMessageForeColor()],
		ANSI_BACK_COLORS[GetDocument()->Session()->GetOptions().colorOptions_.GetMessageBackColor()],
		strText,
		ANSI_RESET);

	PrintAnsi((LPCSTR)strMessage, FALSE);
}

BOOL CNewWindowView::ChatServerStarted()
{
	bool started = false;
	GetDocument()->Session()->GetChat().ChatServerIsStarted(started);
	return started ? TRUE : FALSE;
}

BOOL CNewWindowView::AreSnoops()
{
	bool snoops = false;
	GetDocument()->Session()->GetChat().AreSnoops(snoops);
	return snoops ? TRUE : FALSE;
}

void CNewWindowView::CloseLog(BOOL bPrintMessage)
{
	if(RawMudLogging())
		CloseRawMudLog(true);
	if(_pLogFile.get() == NULL)
		return;

	CTime t = CTime::GetCurrentTime();

	CString strMessage;
	strMessage.Format("# %s :: Log Closed.", t.Format("%A, %B %d, %Y::%H:%M:%S"));
	_pLogFile->Write(strMessage, strMessage.GetLength());
	_pLogFile->Write("\r\n", 2);
	_pLogFile->Close();
	_pLogFile.reset();
	
	Terminal().SetLogging(FALSE);
	GetDocument()->Session()->SetCurrentLogFile("");
	Logging(false);

	if(bPrintMessage)
		PrintMessage(strMessage);	
}
void CNewWindowView::CloseRawMudLog(BOOL bPrintMessage)
{
	if(_pRawMudLogFile.get() == NULL)
		return;

	CTime t = CTime::GetCurrentTime();

	CString strMessage;
	strMessage.Format("# %s :: Log Closed.", t.Format("%A, %B %d, %Y::%H:%M:%S"));
	_pRawMudLogFile->Write(strMessage, strMessage.GetLength());
	_pRawMudLogFile->Write("\r\n", 2);
	_pRawMudLogFile->Close();
	_pRawMudLogFile.reset();
	

	RawMudLogging(false);

	if(bPrintMessage)
		PrintMessage(strMessage);	
}

LRESULT CNewWindowView::CloseMud(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if(_pMudSocket.get() == NULL)
	{
		PrintAnsi("\x1b[1;37m# Not connected...",TRUE);
		return 0;
	}

	_pMudSocket->Close();
	_pMudSocket.reset();

//	PrintMessage("# CNewWindowView::CloseMud");
	PrintAnsi("\x1b[1;37m# Zap! Connection closed...",TRUE); 
	GetDocument()->Session()->SetConnected(FALSE);
	if(GetDocument()->Session()->GetOptions().terminalOptions_.ReconnectOnZap())
		CNewWindowView::ConnectToMud();
	return 0;
}

BOOL CNewWindowView::Connected()
{
	return (_pMudSocket.get() != NULL);
}

void CNewWindowView::ConnectToMud()
{
	if(_pMudSocket.get() != NULL)
	{
		PrintMessage("# Already connected... Please Zap this connection first.");
		return;
	}

	_pMudSocket.reset(new CMudSocket());
	_pMudSocket->SetView(this);
	
	BOOL bResult = _pMudSocket->Create(0, SOCK_STREAM, FD_READ | FD_CONNECT | FD_CLOSE);
	if(!bResult)
	{
		PrintMessage("# Could not create mud socket!");
		_pMudSocket.reset();
		return;
	}

	bResult = _pMudSocket->Connect(
		GetDocument()->Session()->GetOptions().sessionInfo_.Address(), GetDocument()->Session()->GetOptions().sessionInfo_.Port());

	int nError = WSAGetLastError();

	if(!bResult && (nError != WSAEWOULDBLOCK))
	{
		CString strMessage;
		strMessage.Format("# Error %d: ", nError);
		switch(nError)
		{
		case WSANOTINITIALISED:
			strMessage += "A successful AfxSocketInit must occur before using this API.";
			break;
		case WSAENETDOWN:
			strMessage += "The Windows Sockets implementation detected that the network subsystem failed.";
			break;
		case WSAEADDRINUSE:
			strMessage += "The specified address is already in use.";
			break;
		case WSAEINPROGRESS:
			strMessage += "A blocking Windows Sockets call is in progress.";
			break;
		case WSAEADDRNOTAVAIL:
			strMessage += "The specified address is not available from the local machine.";
			break;
		case WSAEAFNOSUPPORT:
			strMessage += "Addresses in the specified family cannot be used with this socket.";
			break;
		case WSAECONNREFUSED:
			strMessage += "The attempt to connect was rejected.";
			break;
		case WSAEDESTADDRREQ:
			strMessage += "A destination address is required.";
			break;
		case WSAEFAULT:
			strMessage += "The nSockAddrLen argument is incorrect.";
			break;
		case WSAEINVAL:
			strMessage += "Invalid host address.";
			break;
		case WSAEISCONN:
			strMessage += "The socket is already connected.";
			break;
		case WSAEMFILE:
			strMessage += "No more file descriptors are available.";
			break;
		case WSAENETUNREACH:
			strMessage += "The network cannot be reached from this host at this time.";
			break;
		case WSAENOBUFS:
			strMessage += "No buffer space is available. The socket cannot be connected.";
			break;
		case WSAENOTSOCK:
			strMessage += "The descriptor is not a socket.";
			break;
		case WSAETIMEDOUT:
			strMessage += "Attempt to connect timed out without establishing a connection.";
			break;
		default:
			strMessage += "An unknown error occurred";			
		}
		PrintMessage(strMessage);
		_pMudSocket.reset();

	}
	CString strMessage;
	strMessage = GetDocument()->Session()->GetOptions().sessionInfo_.Address();
	if(strMessage.IsEmpty())
		GetDocument()->Session()->GetOptions().terminalOptions_.ReconnectOnZap(FALSE);
	strMessage.Format("# Connecting session [%s] to: %s:%d",
		GetDocument()->Session()->GetOptions().sessionInfo_.SessionName(),
		GetDocument()->Session()->GetOptions().sessionInfo_.Address(),
		GetDocument()->Session()->GetOptions().sessionInfo_.Port());
	PrintMessage(strMessage);
}

void CNewWindowView::DoSpeedWalk(const std::string &line)
{
	// Need to expand any numbers that exist in the speedwalk text.
	// 3nu2w would be expanded to: nnnuww
	CString strWalk;
	CString strNum;
	const char *pszText = &line[0];
	while(*pszText)
	{
		// If a digit, pull them off and save them until we hit
		// an alpha.
		if (isdigit(*pszText))
			strNum += *pszText;
		else
		{
			// If not building a number, just keep the character.
			if (strNum.IsEmpty())
				strWalk += *pszText;
			else
			{
				strWalk += CString(*pszText,atoi(strNum));
				strNum.Empty();
			}
		}

		pszText++;
	}

	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("NULL == pDoc"));
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("NULL == pSession"));
		return;
	}

	pSession->AppendToSpeedwalkQueue(strWalk);
	int nPace = pSession->GetOptions().sessionInfo_.Pace();
	SpeedWalkTimer(SetTimer(IDT_SPEEDWALK, nPace, NULL));
	speedWalking_ = true;
}

void CNewWindowView::PrintCommand(const std::string &line)
{
	static LPCTSTR empty_line = _T("");
	if(!GetDocument()->Session()->GetOptions().terminalOptions_.LocalEcho())
	{
		Terminal().PrintLine(
			empty_line, 
			FALSE, 
			_apMessageData,
			GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
			GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor());
		return;
	}

	std::string buffer(line);
	if(!buffer.empty())
	{
		if(buffer[buffer.length() - 1] == '\n')
		{
			buffer.push_back('\0');
		}
	}

	CString strMessage;
	strMessage.Format("%s%s%s",
		MESSAGE_COLORS[MM_COLOR_COMMAND],
		buffer.c_str(),
		MESSAGE_COLORS[MM_COLOR_RESET]);

    Terminal().CheckPromptAppend();
	Terminal().PrintLine(
		strMessage, 
		FALSE, 
		_apMessageData,
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor());
}

void CNewWindowView::SendTextToMud(const std::string &line)
{
	
	CSession *pSession = GetDocument()->Session();
	
	if(AreSnoops() && pSession->GetOptions().terminalOptions_.LocalEcho())
		pSession->GetChat().SendSnoop(
			line,
			pSession->GetOptions().terminalOptions_.GetTerminalWindowForeColor(),
			pSession->GetOptions().terminalOptions_.GetTerminalWindowBackColor());

	if(_pMudSocket.get() == NULL)
	{
		PrintMessage("# No connection...");
	}
	else
	{
		_pMudSocket->Send(line.c_str(), line.size());
		_pMudSocket->Send("\n", strlen("\n"));
		//TODO:KW Sending input so reset the time for tracking time since input to now.
		pSession->SetTimeSinceLastInput();

	}
}

HRESULT CNewWindowView::HandleCommandLine(const CString &line)
{
	HRESULT hr = E_UNEXPECTED;

	try
	{
		if(line.IsEmpty())
		{
			SendTextToMud("");
			Terminal().AvertAppendNext();
		}
		else
		{
			typedef std::vector<CString> LinesType;
			LinesType lines;
			CParseUtils::GetLines(line, lines);
			
			LinesType::iterator it  = lines.begin();
			LinesType::iterator end = lines.end();
			while(it != end)
			{
				CString line = *it;
				GetDocument()->Session()->DebugStack().Push(0, STACK_TYPE_EVAL_ONELINE, line );

				BOOL bRet = HandleOneLine(line);
				GetDocument()->Session()->DebugStack().Pop();
				if(!bRet)
					ErrorHandlingUtils::TESTHR(E_FAIL);

				++it;
			}
		}

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		::OutputDebugString(_T("Error executing HandleCommandLine\n"));
		::OutputDebugString(line + (_T("\n")));
		GetDocument()->Session()->DebugStack().DumpStack(GetDocument()->Session(), GetDocument()->Session()->GetOptions().debugOptions_.DebugDepth());
	}

	return hr;
}

BOOL CNewWindowView::HandleOneLine(const CString &lineIn)
{
	CString line(lineIn);
	if( !line.IsEmpty() )
	{
		if( speedWalking_ )
		{
			if(line.MakeLower() == "/walkstop")
				GetDocument()->Session()->KillSpeedwalk(); //don't want walkstop queued
			queueLine( line );
		}
		else
		{
			if(line[0] == CGlobals::GetCommandCharacter())
			{
				if( line.GetLength() > 1 )
				{
					if(line[1] != CGlobals::GetCommandCharacter())
					{
						return ProcessMudmasterCommand(line);
					}
					else
					{
						line = line.Mid(1);
					}
				}
			}

			if(line[0] == CGlobals::GetProcedureCharacter())
			{
				return ProcessMudmasterProcedure(line);
			}

			return ProcessStandardInput(line);
		}
	}

	return true;
}

bool CNewWindowView::ProcessRepetitiveCommand( const CString &line )
{
	bool retVal = false;
CString sBlank = " ";
CString sBrace = "{";
	int pos = line.Find( sBlank,0 );
	if ( pos != -1 )
	{
		CString command = line.Mid( pos + 1 );
		CString strText( command );

		BOOL foundStatement = TRUE;

		if( command[0] == sBrace )
		{
			CIfxDoc *pDoc = GetDocument();
			CSession *pSession = pDoc->Session();

			CString temp( strText );
			foundStatement = CParseUtils::FindStatement( pSession, temp, strText );
		}

		if( foundStatement )
		{
			command = static_cast<LPCTSTR>( strText );
			
		//	CString num = line.Left( pos - 1 );
			CString num = line.Mid( 1 , pos - 1  );
			int reps = 0;
			try
			{
				reps = boost::lexical_cast<int>( num );
				for(int i = 0; i < reps; ++i)
				HandleCommandLine( command );

			}
			catch( boost::bad_lexical_cast ble )
			{
				for(int i = 0; i < reps; ++i)
					HandleCommandLine( command );
			}
		}
	}

	return retVal;
}

bool CNewWindowView::ProcessOneCommand(const CString &line)
{
	bool retVal = false;

	// this function processes commands, therefore the first character
	// of the line will be the command character (/ by default)
	// we don't really want this character so we're going to eat it
	CString command = line.Mid( 1 );

	int pos = command.Find(' ');

	CString parameters;

	if( pos != -1 )
	{
		CString c2(command);
		command = c2.Left( pos );
		parameters = c2.Mid( pos + 1 );
	}

	command.MakeLower();

	CIfxDoc *pDoc = GetDocument();
	CSession *pSession = pDoc->Session();

	CCommandTable::COMMANDPROC cmd 
		= CCommandTable::FindCommand( pSession, command );

	if(cmd != NULL)
	{
		//TODO:KW count commands processed
		pSession->SetCmdsProcessed(pSession->GetCmdsProcessed() + 1);
		try
		{
			cmd( pSession, parameters );
		}
		catch (...)
		{
			::OutputDebugString(_T("Error executing command in ProcessOneCommand\n"));
			::OutputDebugString(command + (_T("\n")));
		}
	}
	else
	{
		CString strMessage;
		strMessage.Format( "# Invalid Command. [%s]", command );
		PrintMessage("");
		PrintMessage(strMessage);
		GetDocument()->Session()->DebugStack().DumpStack(
			GetDocument()->Session(), 
			GetDocument()->Session()->GetOptions().debugOptions_.DebugDepth());
	}

	return retVal;
}

bool CNewWindowView::ProcessMudmasterCommand(const CString &line)
{
	bool retVal = false;
	try
	{
		if ( line[0] == CGlobals::GetCommandCharacter() && 
			line.GetLength() > 2 && 
			isdigit(line[1]))
		{
			retVal = ProcessRepetitiveCommand(line);
		}
		else
		{
			retVal = ProcessOneCommand(line);
		}
		return TRUE;
	}
	catch (...)
	{
		::OutputDebugString(_T("Error executing command in ProcessMudMasterCommand:\n"));
		::OutputDebugString(line + (_T("\n")));
		return FALSE;
	}

}

BOOL CNewWindowView::ProcessMudmasterProcedure( const CString &line )
{
	HRESULT hr = E_UNEXPECTED;

	if( !CProcedureTable::DoProcedure( GetDocument()->Session(), line ) )
	{
		CString strLine( line );

		CParseUtils::ReplaceVariables( GetDocument()->Session(), strLine );

		CString l2( strLine );

		hr = HandleCommandLine( l2 );

		return SUCCEEDED(hr);

	}
// FRUITMAN
	else
	{
        return TRUE;
	}
// END FRUITMAN
}

BOOL CNewWindowView::ProcessStandardInput(const CString &line)
{
	HRESULT hr = E_UNEXPECTED;

	CIfxDoc *pDoc = GetDocument();
	CSession *pSession = pDoc->Session();

	try
	{
		CString l2( line );
		CParseUtils::ReplaceVariables(pSession, l2);

		CAliasList::MatchData m;
		CString search( l2 );

		if( pSession->GetAliases()->FindEnabledAlias( search, m ) )
		{
			pSession->DebugStack().Push(
				m._index,
				STACK_TYPE_ALIAS, 
				l2);
//			CParseUtils::ReplaceVariables(pSession, m._command);
			ErrorHandlingUtils::TESTHR( HandleCommandLine( m._command ) );

			pSession->DebugStack().Pop();
		} 
		else
		{
			if(pSession->GetOptions().sessionInfo_.SpeedWalk() && 
				l2.GetLength() > 1 && 
				CParseUtils::IsWalkable( static_cast<LPCTSTR>( l2 ) ) )
			{
				DoSpeedWalk( static_cast<LPCTSTR>( l2 ) );
			}
			else
			{
				if(pSession->GetOptions().terminalOptions_.LocalEcho())
					PrintCommand( static_cast<LPCTSTR>( l2 ) );
				else
				{
					Terminal().AvertAppendNext();
				}

				// For the aliases that take a parameter and no param is passed
				// I think the trailing spaces are upsetting the mud.  So trim
				// the string first.
				SendTextToMud( static_cast<LPCTSTR>( l2 ) );
			}
		}
		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}

	return SUCCEEDED(hr);
}

BOOL CNewWindowView::ProcessMudLine(const char *pszLine, BOOL bAppendNext, BOOL bPrompt)
{
	//CIfxDoc *pDoc = GetDocument();
	CSession *pSession = GetDocument()->Session();
	//TODO:KW count lines received from mud or at least buffers...
	pSession->SetLinesReceived(pSession->GetLinesReceived() + 1);
	//TODO: KW test calling all dlls
	//onReceiveLine Dll support
	CString onReceive = "onReceiveLine";
	CString strReceived = pszLine;
	pSession->SetHandledByDll(FALSE);
	pSession->GetDlls().CallAll( pSession, onReceive, strReceived );
	//TODO: KW END test calling all dlls
	//Will check below for !pSession->GetHandledByDll()
	if(RawMudLogging())
	{
		CString RawMudData = pszLine;
		if(_pRawMudLogFile.get() != NULL)
			_pRawMudLogFile->Write(RawMudData,RawMudData.GetLength());
			_pRawMudLogFile->Write("\r\n", 2);
	}
	if(AreSnoops())
	{
		pSession->GetChat().SendSnoop(
			pszLine,
			pSession->GetOptions().terminalOptions_.GetTerminalWindowForeColor(),
			pSession->GetOptions().terminalOptions_.GetTerminalWindowBackColor());
	}
	CColorLine::SpColorLine line(new CColorLine(
		pSession->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		pSession->GetOptions().terminalOptions_.GetTerminalWindowBackColor()));
	BOOL bRet = TRUE;
	//if any dll claims  to have handled the line received don't reprocess it
	if(!pSession->GetHandledByDll())
	{
		CMudSocket*  pSocket;
		pSocket = _pMudSocket.get();
		_apMudData.ParseAnsi(pszLine, line, pSocket, pSession);
		line->AppendNext(bAppendNext);

		bRet = ProcessLine(line);
	}
    Terminal().SetPrompt(bPrompt);

	return bRet;
}

BOOL CNewWindowView::ProcessLine(CColorLine::SpColorLine &line)
{
	CSession *pSession = GetDocument()->Session();
	CTriggerOptions &triggerOptions = pSession->GetOptions().triggerOptions_;
	CTerminalOptions &terminalOptions = pSession->GetOptions().terminalOptions_;
	//TODO: KW resorting to brute force debugging
	CString strProcessLineSection;


	try
	{
		if(LastLineGagged())
		{
			strProcessLineSection = "Inside LastLineGagged";
			LastLineGagged(false);
			char *ptr = line->GetTextBuffer();
			if(*ptr == '\0')
			{
				return TRUE;
			}
		}

		if(pSession->GetOptions().mspOptions_.On())
		{
			strProcessLineSection = "Inside msp on";
			if(strlen(line->GetTextBuffer()) != 0 && ProcessMSP(line))
			{
				if(pSession->GetOptions().mspOptions_.ShowMSPTriggers() == FALSE)
				{
					return TRUE;
				}
			}
		}
		if(!triggerOptions.IgnoreTriggers() && strlen(line->GetTextBuffer()) > 0)
		{
			strProcessLineSection = "Inside action section above pre sub";
			if(!triggerOptions.IgnoreSubs() && pSession->GetOptions().sessionInfo_.PreSub())
			{
				pSession->GetSubs()->CheckSub(pSession, line);
			}

			if(!triggerOptions.IgnoreActions())
			{
				//strProcessLineSection = "Inside action section";
				CString text(line->GetTextBuffer());
				BOOL bEcho;

				GlobalVariables::g_pszCurrentLineBuffer = line->GetTextBuffer();
				GlobalVariables::g_pszCurrentColorBuffer = line->GetColorBuffer();

				int nStart = 0;
				int nMatch = 0;
				CAction::PtrType matchedAction;
				//TODO: KW moved localEcho save, set and reset outside the while loop
				//TODO: KW comment out the setting of debug messages
				bEcho = pSession->GetOptions().terminalOptions_.LocalEcho();
				terminalOptions.LocalEcho(FALSE);
				while(pSession->GetActions()->CheckTrigger( text, nStart, matchedAction ))
				{
					//strProcessLineSection.Format("Inside action check loop for %s\n",text);
					pSession->DebugStack().Push( nStart, STACK_TYPE_ACTION, text );

					BOOL bRet = SUCCEEDED( HandleCommandLine( matchedAction->GetAction().c_str() ) );
					//strProcessLineSection.Format("Inside action check loop for %s\nAfter HandleCommandLine return\n",text);
					//TODO: KW testing the empying of global match variables after action completed
					CGlobals::ClearGlobalVar();
					if(bRet == FALSE)
					{
						//strProcessLineSection.Format("Inside action check loop for %s\nAfter HandleCommanLine with bRet false\n",text);
						GlobalVariables::g_pszCurrentLineBuffer = NULL;
						GlobalVariables::g_pszCurrentColorBuffer = NULL;
						return FALSE;
					}
					pSession->DebugStack().Pop();
					//if action deleted the current action then the next takes the previous index
					if(!pSession->GetActions()->Find( (LPCTSTR)matchedAction->GetTrigger().c_str(), matchedAction , nMatch ))
					{					
						nStart -= 1;
					}
					else if (nMatch >= nStart) 
					{
						//if actions were added to push the current down the list we have to skip to avoid reexecution
						nStart = nMatch +1;
						//even though this means we might jump over some actions
					}
					else if(nMatch < nStart)
					{
						// if multiple actions before the current were removed slide the pointer back to the new next.
						nStart -= (nStart - nMatch -1);
					}
					
					//strProcessLineSection.Format("nstart = %d at last line of while loop for %s\nAfter DebugStack()pop and LocalEcho reset\nNumber of Action: %d",nStart,text,pSession->GetActions()->GetCount());
				}
				//strProcessLineSection.Format("Outside action check loop for last match %s\nBefore linebuffer and colorbuffer = NULL\n",text);
				terminalOptions.LocalEcho(bEcho);

				GlobalVariables::g_pszCurrentLineBuffer = NULL;
				GlobalVariables::g_pszCurrentColorBuffer = NULL;
				//strProcessLineSection.Format("Outside action check loop for last match %s\nAfter linebuffer and colorbuffer = NULL\n",text);

			}

			if(!triggerOptions.IgnoreGags())
			{
				int nStart = 0;
				if (pSession->GetGags()->CheckTrigger(line->GetTextBuffer(), nStart))
				{
					if(Terminal().ResetAppendNext(line))
					{
						LastLineGagged(true);
					}
					return TRUE;
				}
			}

			if (!triggerOptions.IgnoreSubs() && !pSession->GetOptions().sessionInfo_.PreSub())
			{
				strProcessLineSection = "Inside Sub section";
				pSession->GetSubs()->CheckSub(pSession, line);
			}
			if(!triggerOptions.IgnoreHighs())
			{
				strProcessLineSection = "Inside Highlight section";
				pSession->GetHighlights()->CheckHighlight(pSession, line);
			}
			strProcessLineSection.Format("After all processing in ingnore trigger if\n");
		}
		strProcessLineSection.Format("After all processing in try\n");
	}
	catch(...)
	{
		::OutputDebugString(_T("Error executing ProcessLine\n"));
		::OutputDebugString(strProcessLineSection + (_T("\n")));
		pSession->DebugStack().DumpStack(pSession, pSession->GetOptions().debugOptions_.DebugDepth());
	}

	Terminal().PrintLine(line);
	return TRUE;
}

BOOL CNewWindowView::ProcessMSP(CColorLine::SpColorLine &line)
{
	CString strLine = line->GetTextBuffer();
	int nSoundIndex = strLine.Find("!!SOUND(");
	int nMusicIndex = strLine.Find("!!MUSIC(");

	if(nSoundIndex == -1 && nMusicIndex == -1)
		return FALSE;

	if(nSoundIndex != -1)
	{
		int nCloseIndex = strLine.Find(")", nSoundIndex);
		if(nCloseIndex == -1)
			return FALSE;

		CString strSound = strLine.Mid(nSoundIndex, nCloseIndex - nSoundIndex);
		HandleSoundString(strSound);
	}

	if(nMusicIndex != -1)
	{
		int nCloseIndex = strLine.Find(")", nMusicIndex);
		if(nCloseIndex == -1)
			return FALSE;

		CString strMusic = strLine.Mid(nMusicIndex, nCloseIndex - nMusicIndex);
		HandleMusicString(strMusic);
	}
	return TRUE;
}

/* TODO: finish implementing SFML */
BOOL CNewWindowView::HandleSoundString(CString &strSound)
{
	if(GetDocument()->Session()->GetOptions().mspOptions_.ProcessFX() == FALSE)
		return TRUE;	

	std::vector<char> buffer;
	buffer.resize(strSound.GetLength()+1);
	strcpy(&buffer[0], strSound);

	CString strFilename;
	char *ptr;
	CString strType;

	// The volume defaults to 100% if not specified.
	int nLeftVol = 100;
	int nRightVol = 100;

	// Defaults to play just 1 time.
	int nRepeat = 1;

	// Defaults to 50.
	int nPriority = 50;

	// Filename is the first parameter.
	ptr = strtok(&buffer[0]+8," ");
	strFilename = ptr;

	// No filename then just ignore it.
	if (!strFilename.IsEmpty())
	{
		// Filename of "Off" is supposed to stop the playing of the sounds.
		if(strFilename.GetLength() == 3 && strFilename == "Off")
		{
			_pGlobalSoundManager->StopFX();
		}
		else
		{
			// If there isn't an extension, need to default to a wave file.
			if (strFilename.Find('.') == -1)
				strFilename += ".wav";

			// Pull out any other parameters that may exist.
			char *ptrVol;
			ptr = strtok(NULL," ");
			while(ptr)
			{
				switch(*ptr)
				{
				case 'V' :
					ptrVol = strchr(ptr+2,',');
					if (!ptrVol)
					{
						nLeftVol = atoi(ptr+2);
						nRightVol = nLeftVol;
					}
					else
					{
						nLeftVol = atoi(ptr+2);
						nRightVol = atoi(ptrVol+1);
					}
					break;

				case 'L' :
					nRepeat = atoi(ptr+2);
					break;

				case 'P' :
					nPriority = atoi(ptr+2);
					break;

				case 'T' :
					strType = ptr+2;
					break;
				}

				ptr = strtok(NULL," ");
			}

			if (strchr(strFilename,'*') || strchr(strFilename,'?'))
				strFilename = ((CIfxApp *)AfxGetApp())->MSP().MatchWildcard(strFilename, MATCH_WAVE);

			if(GetDocument()->Session()->GetOptions().mspOptions_.UsePlaySound())
			{
				std::string path;
				GetDocument()->Session()->GetOptions().pathOptions_.Get_SoundPath(path);

				path += _T("\\");
				path += strFilename;

				PlaySound(
					path.c_str(), 
					NULL, 
					SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOWAIT);
			}
			else
			{
				if(_pGlobalSoundManager.get() == NULL)
				{
					ASSERT(FALSE);
				}
				else
				{
					_pGlobalSoundManager->PlayFX(
						(LPCTSTR)strFilename, 
						nLeftVol, 
						nRightVol, 
						nRepeat, 
						nPriority, 
						(LPCTSTR)strType, 
						NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL CNewWindowView::HandleMusicString(CString &strMusic)
{
#ifdef MM2K6_NODIRECTSOUND
	return TRUE;
#else
	if(GetDocument()->Session()->GetOptions().mspOptions_.ProcessMidi() == FALSE)
		return TRUE;

	char *szBuf = new char[strMusic.GetLength()+1];
	ZeroMemory(szBuf, strMusic.GetLength()+1);

	strcpy(szBuf, strMusic);

	CString strFilename;
	char *ptr;
	CString strType;

	// The volume defaults to 100% if not specified.
	int nLeftVol = 100;
	int nRightVol = 100;

	// Defaults to play just 1 time.
	int nRepeat = 1;

	// Defaults to 50.
	int nPriority = 50;

	// Filename is the first parameter.
	ptr = strtok(szBuf+8," ");
	strFilename = ptr;

	// No filename then just ignore it.
	if (strFilename.IsEmpty())
	{
		delete [] szBuf;
		szBuf = NULL;
		return FALSE;
	}

	// Filename of "Off" is supposed to stop the playing of the sounds.
	if (strFilename.GetLength() == 3 && strFilename == "Off")
	{
		_pGlobalSoundManager->StopBackgroundMusic();
		delete [] szBuf;
		szBuf = NULL;
		return TRUE;
	}

	// If there isn't an extension, need to default to a wave file.
	if (strFilename.Find('.') == -1)
		strFilename += ".mid";

	// Pull out any other parameters that may exist.
	char *ptrVol;
	ptr = strtok(NULL," ");
	while(ptr)
	{
		switch(*ptr)
		{
			case 'V' :
				ptrVol = strchr(ptr+2,',');
				if (!ptrVol)
				{
					nLeftVol = atoi(ptr+2);
					nRightVol = nLeftVol;
				}
				else
				{
					nLeftVol = atoi(ptr+2);
					nRightVol = atoi(ptrVol+1);
				}
				break;

			case 'L' :
				nRepeat = atoi(ptr+2);
				break;

			case 'P' :
				nPriority = atoi(ptr+2);
				break;

			case 'T' :
				strType = ptr+2;
				break;
		}

		ptr = strtok(NULL," ");
	}

	if (strchr(strFilename,'*') || strchr(strFilename,'?'))
		strFilename = ((CIfxApp *)AfxGetApp())->MSP().MatchWildcard(strFilename, MATCH_MIDI);

	//((CIfxApp *)AfxGetApp())->Sound().PlayMidi(((CIfxApp *)AfxGetApp())->MSP().GetPath()+strFilename);
	if( _pGlobalSoundManager.get() == NULL )
		return TRUE;

	if(GetDocument()->Session()->GetOptions().mspOptions_.ProcessMidi())
		_pGlobalSoundManager->PlayBackgroundMusic((LPCTSTR)strFilename);

	delete [] szBuf;
	szBuf = NULL;
	return TRUE;
#endif /* MM2K6_NODIRECTSOUND */
}

void CNewWindowView::HandleIncomingSocketData(const char *pszData, int nBytes)
{
	std::vector<char> dataBuffer;
	CIfxDoc *pDoc = GetDocument();
	CSession *pSession = pDoc->Session();
	BOOL bIacGa = FALSE;


	if(!previousData_.empty())
	{
		dataBuffer.assign(previousData_.begin(), previousData_.end());
		previousData_.erase(previousData_.begin(), previousData_.end());
		std::vector<char>(previousData_).swap(previousData_);
	}

	dataBuffer.insert(dataBuffer.end(), pszData, pszData + nBytes);
    dataBuffer.push_back('\0'); // Make sure that the buffer is always terminated by the End of String.
	std::vector<char>::iterator current = dataBuffer.begin();
    std::vector<char>::iterator start   = dataBuffer.begin();

	// set the return trip time
	pSession->SetReturnTripMsecs();
	while(current != dataBuffer.end() - 1)  //since we just added a \0 to the end we don't have to look at that
	{
		//TODO: KW count the bytes received
		pSession->SetBytesReceived(GetDocument()->Session()->GetBytesReceived() + 1);
		//reset IAC GA indicator
		bIacGa = FALSE;

		if(*current == '\n')
		{
			*current = '\0';
			ProcessMudLine(&(*start), FALSE, FALSE);
            start = current + 1;
		}
		else if (static_cast<BYTE>(*current) == GA)
		{
            // Replace <IAC><GA> by two '\0' '\0'
            // Leave single <GA> 
            if (static_cast<BYTE>(*(current-1)) == IAC)
            {
				//build 29 code
                //*(current - 1) = '\0';
                //*(current)     = '\0';
                //ProcessMudLine(&(*start), TRUE, TRUE);
                //start = current + 1;
				//::OutputDebugString(_T("GA section false true\n"));
				//New attempt to handle IAC GA by just removing it
				--current;
				dataBuffer.erase(current);
				dataBuffer.erase(current);
				--current;
				bIacGa = TRUE;
				//CString strDebug;
				//strDebug.Format(_T("IAC GA %d\n"),nBytes);
				//::OutputDebugString(strDebug);
            }
		}
		++current;
	}

    // There might still be data sitting in the buffer (lines not terminated by \n)
	if((current - start) > 1)
	{
        // If the data is sitting because we are still in the middle of 
        // reading data from socket, than save it in the previousData, so it can
        // be processed with the next call to this function.

		if(nBytes == MAX_SOCK_RECEIVE || (!bIacGa && pSession->GetOptions().terminalOptions_.TelnetGA()))
		{
			previousData_.assign(start, current);
			return;

			// Otherwise the left over line is a prompt (like username:) which
			// is asking the user to enter something on the same line.
			// In that case display it.
		} 
		else 
		{
			//build 29 uses true, false
		    //ProcessMudLine(&(*start), TRUE, FALSE);
			//If IAC GA is at the end of the text stream received it is likely a prompt line
			//but if it is in the middle of a stream it really means nothing and the line
			//should not be broken until a \n is received.

		    ProcessMudLine(&(*start), TRUE, bIacGa);
        }		
	}

	//Terminal().ScrollCaret();
	Terminal().Repaint();
}

LRESULT CNewWindowView::ReceiveFromMud(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	char buffer[MAX_SOCK_RECEIVE];

	int nReceive = _pMudSocket->Receive(buffer, MAX_SOCK_RECEIVE);
	if(nReceive <= 0)
	{
		PrintMessage("# Host killed session...");
		SendMessage(CMessages::MSG_SOCKET_CLOSE, 0, 0);
		return 0;
	}
	
    HandleIncomingSocketData(buffer, nReceive);
	return 0;
}

LRESULT CNewWindowView::OnConnectToMud(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	GetDocument()->Session()->SetConnected(TRUE);
//	PrintMessage("Connected...");
	//Print to screen with script processing
	PrintAnsi("\x1b[1;37mConnected...\x1b[0m",TRUE); 

	return 0;
}

LRESULT CNewWindowView::OnDLLCommand(WPARAM /*wParam*/, LPARAM lParam)
{
	CString command = (LPCSTR)lParam;
	if(FAILED( HandleCommandLine( command ) ))
	{
		TRACE(_T("Failed to execute HandleCommandLine"));
	}
	return 0;
}

LRESULT CNewWindowView::OnLoadScript(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	GetDocument()->Session()->LoadScript();
	return 0;
}

#ifndef _WIN64
void CNewWindowView::OnTimer(UINT nIDEvent) 
#else
void CNewWindowView::OnTimer(UINT_PTR nIDEvent) 
#endif
{
	CString strMessage;
	CAliasList::MatchData m;
	CString search("screenshot");
	
	switch(nIDEvent)
	{
	case IDT_SPEEDWALK:
		OnSpeedwalk();
		break;			
	case ID_FAST_TIMER:
		if( GetDocument()->Session()->GetAliases()->FindEnabledAlias( search, m ) )
		{
			CNewWindowView::ProcessStandardInput(static_cast<LPCTSTR>(_T("screenshot")));
		}
		else
			StopFastTimer();
		break;			
	case ID_TIMER_EVENT:
		{
			CIfxDoc *pDoc = GetDocument();
			CSession *pSession = pDoc->Session();
			pSession->ProcessEvents();
		}

		if( !ChangesTimer() )
			return;

		switch(CChangesThread::m_cti.nThreadStatus)
		{
		// Failed to create the thread
		case THREAD_CHANGES_FAILED:
			strMessage = "Unable to retrieve file.";
			GetDocument()->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage,FALSE);
			ChangesTimer( false );
			CheckShowmeViaWebQueue();
			break;

		// Currently running.
		case THREAD_CHANGES_RUNNING:	
			break;

		// Finished running and is ok.
		case THREAD_CHANGES_DONE:		
			strMessage = CChangesThread::m_cti.pszTextToPrint;

			CParseUtils::ReplaceVariables(GetDocument()->Session(),strMessage);
			if(strMessage.GetLength() / 80 > GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBufferSize())
			{
				Terminal().SetBufferSize(strMessage.GetLength() + 100);
				strMessage.Append(_T("\nIncreased scrollback size to accommodate the file retrieved"));
			}
			PrintAnsi(strMessage, FALSE);
			CChangesThread::m_cti.pszTextToPrint[0] = '\x0';
			ChangesTimer( false );
			CheckShowmeViaWebQueue();
			break;

		// Finished running but failed.
		case THREAD_CHANGES_QUIT:		
			strMessage = "Unable to retreive  information.";
			GetDocument()->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strMessage,FALSE);
			ChangesTimer( false );
			CheckShowmeViaWebQueue();
			break;
		}								
		break;
	}	
}

void CNewWindowView::PrintAnsi(
	const char *pszText, 
	bool doTriggers)
{
	char *current = (char *)pszText;
    char *start   = (char *)pszText;

	while(*current != '\0')
	{
		if(*current == '\n')
		{
            *current = '\0';
			ProcessChatLine(start, FALSE, doTriggers);
            *current = '\n'; // We do not want to change the actual text stream, so any changes
                             // that are made we want to revert back.
            start = current + 1;
		}
		current++;			
	}

	if((current - start) > 1)
		ProcessChatLine(start, FALSE, doTriggers);
}

BOOL CNewWindowView::SetBackground(CString &strFilename, CString &strMode)
{
	return Terminal().SetBackground(strFilename, strMode);
}

BOOL CNewWindowView::ScreenShot(CString &strFilename)
{

	if(strFilename.MakeLower() == "stop")
	{
		CNewWindowView::StopFastTimer();
		return TRUE;
	}
	else if (strFilename.MakeLower().Find("start") == 0)
	{
		CAliasList::MatchData m;
		CString search("screenshot");
		if( GetDocument()->Session()->GetAliases()->FindEnabledAlias( search, m ) )
		{
			if(strFilename.GetLength()  > 5) {
				int mSecs;
				mSecs = atoi(strFilename.Mid(6));
				CNewWindowView::StartFastTimer(mSecs);
			}
			else 
				CNewWindowView::StartFastTimer();
		}
		else
			PrintMessage("Fast timer not started because screenshot alias is missing");

		return TRUE;
	}
	return Terminal().ScreenShot(strFilename);

}

BOOL CNewWindowView::ProcessChatLine(
	const char *pszLine, 
	BOOL bAppendNext	/*=FALSE*/, 
	BOOL bDoTriggers	/*=TRUE*/)
{
	//CIfxDoc *pDoc = GetDocument();
	//CSession *pSession = GetDocument()->Session();

	if(AreSnoops())
	{
		GetDocument()->Session()->GetChat().SendSnoop(
			pszLine,
			GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(),
			GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor());
	}
	CColorLine::SpColorLine line(new CColorLine(
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor()));

	_apMessageData.ParseAnsi(pszLine, line);
	if(bAppendNext)
		line->AppendNext(TRUE);

	if(bDoTriggers)
		return ProcessLine(line);
	else
		Terminal().PrintLine(line);
	
	return TRUE;
}

BOOL CNewWindowView::ProcessLineNoChat(
	const char *pszLine, 
	BOOL bAppendNext	/*=FALSE*/, 
	BOOL bDoTriggers	/*=TRUE*/)
{

	CColorLine::SpColorLine line(new CColorLine(
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor()));

	_apMessageData.ParseAnsi(pszLine, line);
	if(bAppendNext)
		line->AppendNext(TRUE);

	if(bDoTriggers)
		return ProcessLine(line);
	else
		Terminal().PrintLine(line);
	
	return TRUE;
}

void CNewWindowView::OnUpdate(
	CView* /*pSender*/, 
	LPARAM lHint, 
	CObject* pHint) 
{
	CObString *pOString = NULL;
	CString strMessage;
	CHARFORMAT *pFormat;

	switch(lHint)
	{
	case UPDATE_TERMINAL_WINDOW_COLORS:
		Terminal().SetBackColor(GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor(), FALSE);
		Terminal().SetForeColor(GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), TRUE);
		break;

	case UPDATE_TERMINAL_WINDOW_FONT:
		Terminal().SetTheFont(GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowFont(), FALSE);
		Terminal().Repaint();
		break;

	case UPDATE_PRINT_MESSAGE:
		pOString = (CObString *)pHint;
		PrintMessage(pOString->Value());
		break;    

	case UPDATE_EXECUTE_COMMAND:
		pOString = (CObString *)pHint;
		if( FAILED( HandleCommandLine( pOString->Value() ) ) )
		{
				TRACE(_T("Failed to execute HandleCommandLine"));
		}
		break;

	case UPDATE_CLOSE_LOG:
		CloseLog(TRUE);
		break;
	case UPDATE_CLOSE_RAWMUDLOG:
		CloseRawMudLog(TRUE);
		break;

	case UPDATE_MACROS_CHANGED:
		UpdateAcceleratorTable();
		break;

	case UPDATE_CONNECT_TO_MUD:
		ConnectToMud();
		break;

	case UPDATE_DISCONNECT_FROM_MUD:
		PrintMessage("# in Update Disconnect from Mud");
		SendMessage(CMessages::MSG_SOCKET_CLOSE, 0, 0);
		break;

	case UPDATE_CHANGES_THREAD_START:
		ChangesTimer( true );
		break;

	case UPDATE_PRINT_INCOMING_CHAT:
		pOString = (CObString *)pHint;
		PrintAnsi(pOString->Value(), true);
		break;

	case UPDATE_PRINT_ANSI_NO_TRIGGER:
		pOString = (CObString *)pHint;
		PrintAnsi(pOString->Value(), false);
		break;

	case UPDATE_PRINT_OUTGOING_CHAT:
		pOString = (CObString *)pHint;
		PrintAnsi(pOString->Value(), true);
		break;

	case UPDATE_SAVE_SCROLLBACK:
		pOString = (CObString *)pHint;
		Terminal().SaveScrollBack(pOString->Value());
		strMessage = "# Scrollback buffer saved to: ";
		strMessage += pOString->Value();
		PrintMessage(strMessage);

		break;

	case UPDATE_SCROLL_HOME:
		Terminal().ScrollHome();
		break;
	case UPDATE_CLEARSCREEN:
		//Terminal().ScrollHome();
		Terminal().ClearScreen();
		break;

	case UPDATE_SCROLL_LINEUP:
		Terminal().ScrollLineUp();
		break;

	case UPDATE_SCROLL_LINEDOWN:
		Terminal().ScrollLineDown();
		break;

	case UPDATE_SCROLL_PAGEUP:
		Terminal().ScrollPageUp();
		break;

	case UPDATE_SCROLL_PAGEDOWN:
		Terminal().ScrollPageDown();
		break;

	case UPDATE_END_SCROLLBACK:
		Terminal().EndScrollBack();
		break;

	case UPDATE_ALL_OPTIONS:
		UpdateAllOptions();
		break;

	case UPDATE_AUTO_LOG:
		OpenLog(GetDocument()->Session()->GetOptions().sessionInfo_.LogFileName(), GetDocument()->Session()->GetOptions().sessionInfo_.AppendLog(), FALSE);
		break;

	case UPDATE_PRINT_SNOOP:
		pOString = (CObString *)pHint;
		PrintSnoop(pOString->Value());
		break;
		
	case UPDATE_AUTO_LOAD_SCRIPT:
		PostMessage(CMessages::MM_LOAD_SCRIPT, 0, 0);
		break;

	case UPDATE_STATUS_BAR_COLORS:
		StatusWindow().SetBackColor(
			GetDocument()->Session()->GetOptions().statusOptions_.GetStatusBarBackColor());

		StatusWindow().SetForeColor(
			GetDocument()->Session()->GetOptions().statusOptions_.GetStatusBarForeColor());
		break;

	case UPDATE_STATUS_BAR_FONT:
		StatusWindow().SetFont(GetDocument()->Session()->GetOptions().statusOptions_.GetStatusBarFont());
		StatusWindow().Invalidate();
		//RedrawWindow();
		break;

	case UPDATE_STATUS_BAR_OPTIONS:
		UpdateStatusBarOptions();
		break;

	case UPDATE_BAR_HIDE:
		StatusBarVisible(false);
		RecalcLayout();
		break;

	case UPDATE_BAR_DRAWITEM:
		StatusWindow().DrawItem(GetDocument()->Session(), (CBarItem *)pHint);
		break;

	case UPDATE_SET_EDIT_TEXT:
		pOString = (CObString *)pHint;
		Input().ReplaceSel(pOString->Value());
		break;

	case UPDATE_INPUTCONTROL_CHARFORMAT:
		pFormat = GetDocument()->Session()->GetOptions().inputOptions_.GetInputFormat();
		Input().SetDefaultCharFormat(*pFormat);
		break;

	case UPDATE_PLAY_SOUND:
//#ifndef MM2K6_NODIRECTSOUND
		pOString = (CObString *)pHint;
		if(GetDocument()->Session()->GetOptions().mspOptions_.UsePlaySound()==FALSE)
			_pGlobalSoundManager->PlayFX((LPCTSTR)pOString->Value());
		else
			PlaySound(
				(LPCTSTR)pOString->Value(), 
				NULL, 
				SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOWAIT);
//#endif
		break;

	case UPDATE_USE_PLAYSOUND:
//#ifndef MM2K6_NODIRECTSOUND
		_pGlobalSoundManager.reset();
		GetDocument()->Session()->GetOptions().mspOptions_.UsePlaySound(TRUE);
//#endif /* MM2K6_NODIRECTSOUND */
		break;

	case UPDATE_USE_DX:
//#ifndef MM2K6_NODIRECTSOUND
		if(_pGlobalSoundManager.get() == NULL)
			InitializeDirectSound();
		GetDocument()->Session()->GetOptions().mspOptions_.UsePlaySound(FALSE);
//#endif /* MM2K6_NODIRECTSOUND */
		break;

	case UPDATE_OPEN_LOG:
		pOString = (CObString *)pHint;
		OpenLog(pOString->Value(), FALSE, FALSE);
		break;
	
	case UPDATE_OPEN_RAWMUDLOG:
		pOString = (CObString *)pHint;
		OpenRawMudLog(pOString->Value(), FALSE, FALSE);
		break;

	case UPDATE_OPEN_LOG_APPEND:
		pOString = (CObString *)pHint;
		OpenLog(pOString->Value(), TRUE, FALSE);
		break;

	case UPDATE_TOGGLE_VERBATIM:
		Verbatim(!Verbatim());
		strMessage.Format("Verbatim mode now %s", Verbatim() ? "on" : "off");
		PrintMessage((LPCTSTR)strMessage);
		break;

	case UPDATE_TIMESTAMP_LOG:
		Terminal().TimestampLog();
		break;
	case UPDATE_TASKBARNOTIFICATION:
		pOString = (CObString *)pHint;
		m_wndTaskbarNotifier.Show(pOString->Value(),30,5000,1000,5);
		break;
	}		
}

void CNewWindowView::UpdateAcceleratorTable()
{
	if (GetDocument()->Session()->GetMacros()->GetCount() < 1)
		return;

	CFrameWnd *pFrame = GetParentFrame();

	if(pFrame == NULL)
		return;

	if (pFrame->m_hAccelTable != NULL)
	{
		BOOL bResult = DestroyAcceleratorTable(pFrame->m_hAccelTable);
		if(!bResult)
		{
			TRACE("Error destroying accelerator table.");
			return;
		}
	}

	pFrame->m_hAccelTable = NULL;

	int nNumMacros = GetDocument()->Session()->GetMacros()->GetCount();

	ACCEL *pAccelTable = new ACCEL[nNumMacros];
	
	int nAccelCount = 0;
	CMacro *pMacro = (CMacro *)GetDocument()->Session()->GetMacros()->GetFirst();
	while(pMacro)
	{
		// Set the modifiers.
		pAccelTable[nAccelCount].fVirt = 0;
		if (pMacro->Modifiers() & HOTKEYF_ALT)
			pAccelTable[nAccelCount].fVirt |= FALT | FVIRTKEY;
		if (pMacro->Modifiers() & HOTKEYF_SHIFT)
			pAccelTable[nAccelCount].fVirt |= FSHIFT;
		if (pMacro->Modifiers() & HOTKEYF_CONTROL)
			pAccelTable[nAccelCount].fVirt |= FCONTROL | FVIRTKEY;

		// Some keys don't have any of the above modifiers
		// but are still virtual keys.
		if ((pMacro->VirtualKeyCode() < 48 || pMacro->VirtualKeyCode() > 57) &&
			 (pMacro->VirtualKeyCode() < 65 || pMacro->VirtualKeyCode() > 90))
		{
			pAccelTable[nAccelCount].fVirt |= FVIRTKEY;
		}

		pAccelTable[nAccelCount].key = pMacro->VirtualKeyCode();
		pAccelTable[nAccelCount].cmd = (unsigned short)(ID_MACRO_START + nAccelCount);

		nAccelCount++;
		pMacro = (CMacro *)GetDocument()->Session()->GetMacros()->GetNext();
	}

	pFrame->m_hAccelTable = CreateAcceleratorTable(pAccelTable,nAccelCount);

	if(pFrame->m_hAccelTable == NULL)
	{
		PrintMessage("# Error creating accelerator table\n");
	}
	delete [] pAccelTable;
	pAccelTable = NULL;
}

void CNewWindowView::OpenLog(LPCTSTR pszFilename, BOOL bAppend, BOOL bAddScrollBack)
{
	UINT nOptions;
	nOptions = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite;
	if(bAppend)
		nOptions |= CFile::modeNoTruncate;

	try
	{
		_pLogFile.reset(new CFile(pszFilename, nOptions));
		GetDocument()->Session()->SetCurrentLogFile(pszFilename);
	}
	catch(CFileException *e)
	{
		char error_message[MAX_PATH];
		ZeroMemory(error_message, MAX_PATH);
		e->GetErrorMessage(error_message, MAX_PATH);
		CString strMessage;
		strMessage.Format("Error opening log file: %s", error_message);
		AfxMessageBox(strMessage);
		Logging(false);
		_pLogFile.reset();
		e->Delete();
		return;
	}

	if(bAppend)
		_pLogFile->SeekToEnd();

	LogFileName(pszFilename);
	Logging(true);
	CTime t = CTime::GetCurrentTime();

	CString strMessage;
	strMessage.Format(
		"# %s :: Now logging session to %s", 
		t.Format("%c"), 
		pszFilename);

	PrintMessage(strMessage);
	_pLogFile->Write(strMessage, strMessage.GetLength());
	_pLogFile->Write("\r\n", 2);
	if(bAddScrollBack)
	{
		Terminal().SaveScrollBack(_pLogFile);
	}
	Terminal().SetLogging(TRUE);
	Terminal().SetLogFile(_pLogFile);
}
void CNewWindowView::OpenRawMudLog(LPCTSTR pszFilename, BOOL bAppend, BOOL bAddScrollBack)
{
	UINT nOptions;
	nOptions = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite;
	if(bAppend)
		nOptions |= CFile::modeNoTruncate;

	try
	{
		_pRawMudLogFile.reset(new CFile(pszFilename, nOptions));
	}
	catch(CFileException *e)
	{
		char error_message[MAX_PATH];
		ZeroMemory(error_message, MAX_PATH);
		e->GetErrorMessage(error_message, MAX_PATH);
		CString strMessage;
		strMessage.Format("Error opening log file: %s", error_message);
		AfxMessageBox(strMessage);
		Logging(false);
		_pRawMudLogFile.reset();
		e->Delete();
		return;
	}

	if(bAppend)
		_pRawMudLogFile->SeekToEnd();

	RawMudLogFileName(pszFilename);
	RawMudLogging(true);
	CTime t = CTime::GetCurrentTime();

	CString strMessage;
	strMessage.Format(
		"# %s :: Now logging session to %s", 
		t.Format("%c"), 
		pszFilename);

	PrintMessage(strMessage);
	_pRawMudLogFile->Write(strMessage, strMessage.GetLength());
	_pRawMudLogFile->Write("\r\n", 2);
	if(bAddScrollBack)
	{
// do nothing yet
	}
}

void CNewWindowView::PrintSnoop(const char *pszLine)
{
	CColorLine::SpColorLine line(new CColorLine(
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor()));

	CString strPrompt;
	strPrompt.Format("%s>>%s",ANSI_F_BOLDGREEN, ANSI_RESET);
	_apMessageData.ParseAnsi(strPrompt, line);

	CColorLine::SpColorLine line2(new CColorLine(
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowForeColor(), 
		GetDocument()->Session()->GetOptions().terminalOptions_.GetTerminalWindowBackColor()));

	_apSnoopData.ParseAnsi(pszLine, line2);
	line->Append(line2);

	Terminal().PrintLine(line);
	Terminal().ScrollCaret();
	Terminal().Repaint();
}

void CNewWindowView::OnSessionConnect() 
{
	ConnectToMud();	
}

void CNewWindowView::OnUpdateSessionConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!Connected());	
}

void CNewWindowView::OnSessionZap() 
{
	//PrintMessage("# in OnSessionZap");
	SendMessage(CMessages::MSG_SOCKET_CLOSE, 0, 0);
}

void CNewWindowView::OnUpdateSessionZap(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(Connected());
}

void CNewWindowView::OnSessionReadscript() 
{
	CFileDialog dlg(
		TRUE, 
		"mm", 
		GetDocument()->Session()->GetOptions().sessionInfo_.SessionName(), 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"Script Files (*.mm)|*.mm|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||" );
	
	if(dlg.DoModal()==IDOK)
	{
		std::stringstream buffer;
		buffer
			<< CGlobals::GetCommandCharacter()
			<< "read {"
			<< static_cast<LPCTSTR>( dlg.GetPathName() )
			<< "}";

		HandleOneLine(buffer.str().c_str());
	}
}

void CNewWindowView::OnSessionLog() 
{
	CLogDialog dlg;
	if(Logging())
	{
		dlg.m_bLogging = TRUE;
		dlg.m_strLogFileName = LogFileName().c_str();
		dlg.m_pView = this;
	}
	else
	{
		dlg.m_bLogging = FALSE;
	}

	int nRet = dlg.DoModal();
	if(nRet != IDOK)
		return;

	CString strLogFileName;
	strLogFileName = dlg.m_strLogFileName;
	Logging(true);
	OpenLog(strLogFileName, dlg.m_bAppend, dlg.m_bIncludeScrollback);		
}
void CNewWindowView::OnSessionRawMudLog() 
{
	CLogDialog dlg;
	if(RawMudLogging())
	{
		dlg.m_bLogging = TRUE;
		dlg.m_strLogFileName = RawMudLogFileName().c_str();
		dlg.m_pView = this;
	}
	else
	{
		dlg.m_bLogging = FALSE;
	}

	int nRet = dlg.DoModal();
	if(nRet != IDOK)
		return;

	CString strLogFileName;
	strLogFileName = dlg.m_strLogFileName;
	RawMudLogging(true);
	OpenRawMudLog(strLogFileName, dlg.m_bAppend, dlg.m_bIncludeScrollback);		
}

void CNewWindowView::OnUpdateSessionLog(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(Logging());		
}

void CNewWindowView::OnSessionSavescrollback() 
{
	CFileDialog dlg(TRUE, "scb", GetDocument()->Session()->GetOptions().sessionInfo_.SessionName(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Scrollback Files (*.scb)|*.scb|All Files (*.*)|*.*||");

	int nRet = dlg.DoModal();
	if(nRet != IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	Terminal().SaveScrollBack(strFileName);	
}

void CNewWindowView::OnSessionStartchatserver() 
{
	GetDocument()->Session()->StartChatServer();
}

void CNewWindowView::OnUpdateSessionStartchatserver(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!ChatServerStarted());	
}

void CNewWindowView::OnUpdateSessionStopchatserver(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChatServerStarted());	
}

void CNewWindowView::OnViewOptions() 
{
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CString strOldChatname = pSession->GetOptions().chatOptions_.ChatName();
	COptionsSheet sheet(
		false,					// is not default options
		pSession->GetOptions(),
		IDS_SESSION_OPTIONS_TITLE, 
		this);

	if(IDOK == sheet.DoModal())
	{
		UpdateChatName(strOldChatname);
		pDoc->UsePlaySound(pSession->GetOptions().mspOptions_.UsePlaySound());
		pDoc->UpdateAllViews(NULL, UPDATE_ALL_OPTIONS, NULL);	
	}
}

void CNewWindowView::OnMacroKey(UINT nID)
{
	int wParam = nID- ID_MACRO_START;
	CMacro *pMacro = (CMacro *)GetDocument()->Session()->GetMacros()->GetAt(wParam);
	if (pMacro == NULL || !pMacro->Enabled())
		return;
	//TODO:KW Got input so reset the time for tracking time since input to now.
	GetDocument()->Session()->SetTimeSinceLastInput();

	CString strText;
	switch(pMacro->Dest())
	{
	case MMScript::CMacro::MacroDestMud:
		{
			strText = pMacro->Action();
			GetDocument()->Session()->DebugStack().Push(wParam, STACK_TYPE_MACRO, CString(""));
			if(strText[0] == '!')
			{
				strText = LastCommand();
			}

			CString command( strText );

			if( FAILED( HandleCommandLine( command ) ) )
			{
				TRACE(_T("Failed to execute HandleCommandLine"));
			}
			GetDocument()->Session()->DebugStack().Pop();
		}
		break;

	case MMScript::CMacro::MacroDestBar:
		strText = pMacro->Action();
		CParseUtils::ReplaceVariables(GetDocument()->Session(), strText);
		GetDocument()->Session()->SetEditText(strText);
		break;

	case MMScript::CMacro::MacroDestBarCr :
		GetDocument()->Session()->SetEditText(pMacro->Action());
		GetDocument()->Session()->DebugStack().Push(wParam, STACK_TYPE_MACRO, CString(""));
		HandleInput();
		//GetDocument()->Session()->SubmitEditText();
		GetDocument()->Session()->DebugStack().Pop();
		break;
	}
}

void CNewWindowView::OnConfigurationActionstriggers() 
{
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("document is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("session is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CActionListDlg dlg(pSession->GetOptions(), pSession);

	dlg.m_pActionList = pSession->GetActions();
	dlg.m_pAliasList = pSession->GetAliases();
	dlg.m_pArrayList = pSession->GetArrays();
	dlg.m_pBarItems = pSession->GetStatusBar();
	dlg.m_pEventList = pSession->GetEvents();
	dlg.m_pGagList = pSession->GetGags();
	dlg.m_pHighList = pSession->GetHighlights();
	dlg.m_pLists = pSession->GetLists();
	dlg.m_pMacroList = pSession->GetMacros();
	dlg.m_pSubList = pSession->GetSubs();
	dlg.m_pVarList = pSession->GetVariables();
	dlg.DoModal();	
}
void CNewWindowView::OnAliasTree() 
{
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("document is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("session is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CActionListDlg dlg(pSession->GetOptions(), pSession);

	dlg.m_pAliasList = pSession->GetAliases();
	dlg.DoModal();	
}

void CNewWindowView::OnConfigurationActions() 
{
	CActionOnlyListDlg dlg;

	dlg.m_pActionList = GetDocument()->Session()->GetActions();
	dlg.DoModal();	
}

void CNewWindowView::OnConfigurationAliases() 
{
	CAliasListDlg dlg;

	dlg.m_pAliasList = GetDocument()->Session()->GetAliases();
	dlg.DoModal();		
}

void CNewWindowView::OnConfigurationEvents() 
{
	CEventListDlg dlg;
	
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is null!"));
		ATLASSERT(FALSE);
		return;
	}

	dlg.m_pEventList = pSession->GetEvents();
	dlg.DoModal();		
}

void CNewWindowView::OnConfigurationMacros() 
{
	CMacroListDlg dlg;
	
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is null!"));
		ATLASSERT(FALSE);
		return;
	}

	dlg.m_pMacroList = pSession->GetMacros();
	dlg.DoModal();		
	GetDocument()->Session()->UpdateMacros();	
}
void CNewWindowView::OnConfigurationGags() 
{
	CGagListDlg dlg;
	
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is null!"));
		ATLASSERT(FALSE);
		return;
	}

	dlg.m_pGagList = pSession->GetGags();
	dlg.DoModal();		
	//TODO: KW is this needed? GetDocument()->Session()->UpdateGags();	
}
void CNewWindowView::OnConfigurationHighs() 
{
	
	
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is null!"));
		ATLASSERT(FALSE);
		return;
	}
	CHighListDlg dlg(pSession->GetOptions());

	dlg.m_pHighList = pSession->GetHighlights();
	
	dlg.DoModal();		
	//TODO: KW is this needed? GetDocument()->Session()->UpdateGags();	
}
void CNewWindowView::OnConfigurationSubs() 
{
	CSubListDlg dlg;
	
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("pDoc is NULL!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("pSession is null!"));
		ATLASSERT(FALSE);
		return;
	}

	dlg.m_pSubList = pSession->GetSubs();
	dlg.DoModal();		
	//TODO: KW is this needed? GetDocument()->Session()->UpdateSubs();	
}

void CNewWindowView::OnInputMsgFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	UINT nState = 0;
	CPoint point;
	CString strText;
	*pResult = 0;
	switch(pMsgFilter->msg)
	{
	case WM_KEYDOWN:
		switch(pMsgFilter->wParam)
		{
		case VK_TAB:
			DoTabCompletion();
			*pResult = 1;
			return;
			break;
		case VK_RETURN:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				*pResult = 0;
				return;
			}
			Input().GetWindowText(strText);
			GetDocument()->Session()->DebugStack().Push(0, STACK_TYPE_USER_INPUT, strText);
			HandleInput();
			GetDocument()->Session()->DebugStack().Pop();
			*pResult = 1;
			return;
			break;
		case 0x0A:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				*pResult = 0;
				return;
			}
			Input().GetWindowText(strText);
			GetDocument()->Session()->DebugStack().Push(0, STACK_TYPE_USER_INPUT, strText);
			HandleInput();
			GetDocument()->Session()->DebugStack().Pop();
			*pResult = 1;
			return;
			break;
		case VK_UP:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				HandleScreenScrollLineUp();
				*pResult = 1;
				return;
			}
			HandleCommandScrollUp();
			*pResult = 1;
			return;
			break;
		case VK_DOWN:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				HandleScreenScrollLineDown();
				*pResult = 1;
				return;
			}
			HandleCommandScrollDown();
			*pResult = 1;
			return;
			break;
		case VK_PRIOR:
			HandleScreenScrollPageUp();
			*pResult = 1;
			return;
			break;
		case VK_NEXT:
			HandleScreenScrollPageDown();
			*pResult = 1;
			return;
		case VK_ESCAPE:

			//GetDocument()->Session()->KillSpeedwalk();
			Terminal().EndScrollBack();
			*pResult = 1;
			return;
		case VK_BACK:
			if(Input().GetTextLength() == 0)
			{
				*pResult =1;
			}
			return;
			break;
		case VK_HOME:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				//GetDocument()->Session()->ScrollHome(this);
				Terminal().ScrollHome();
				*pResult = 1;
				return;
			}
			return;
		case VK_END:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				//GetDocument()->Session()->EndScrollBack(this);
				Terminal().EndScrollBack();
				*pResult = 1;
				return;
			}
			return;
		}

		break;
//	case WM_RBUTTONDOWN:
//		SendMessage(pMsgFilter->msg, pMsgFilter->wParam, pMsgFilter->lParam);
//		*pResult = 1;
//		return;
//		break;
	case WM_LBUTTONDOWN:
		GetParentFrame()->ActivateFrame();
		break;
	case WM_MOUSEWHEEL:
		break;
	case WM_CHAR:
		switch(pMsgFilter->wParam)
		{
		case VK_RETURN:
			*pResult=1;
			return;
		case VK_TAB:
			*pResult=1;
			return;
		case 0x0A:
			nState = GetKeyState(VK_CONTROL);
			if((nState & ~(~0U >> 1)) != 0)
			{
				*pResult = 0;
				return;
			}
			Input().GetWindowText(strText);
			GetDocument()->Session()->DebugStack().Push(0, STACK_TYPE_USER_INPUT, strText);
			HandleInput();
			GetDocument()->Session()->DebugStack().Pop();
			*pResult = 1;
			return;
		}
		break;
	}
	*pResult = 0;
}

void CNewWindowView::DoTabCompletion()
{
	CString strText;
	Input().GetWindowText(strText);
	if (strText.IsEmpty())
		return;

	CString strCommand = strText;

	//if(strText.GetAt(0) == '!')  //command tab completion based on command history
	//{
//	}

	int nLen = strText.GetLength();
	int nIndex = nLen-1;
	while(nIndex && strText.GetAt(nIndex) != ' ')
		nIndex--;

	int nCharsToCopy = nLen-nIndex;
	if (nIndex)
	{
		nIndex++;
		nCharsToCopy--;
	}

	if (nCharsToCopy)
	{

		CString strTab(strText.Mid(nIndex,nCharsToCopy));
		CTab *pTab = GetDocument()->Session()->GetTabList()->Lookup(strTab);
		if (pTab != NULL)
		{

			nLen = pTab->Text().GetLength()-nCharsToCopy;
			CString strAdd(pTab->Text().Mid(nCharsToCopy,nLen));
			Input().ReplaceSel(strAdd);	
			return;
		}
	}
	//search and retrieve from history...set as most recent command if found
		CString strCompleteCommand;
		SearchAndRetrieveCommandHistory(strCommand, strCompleteCommand);
		if(!strCompleteCommand.IsEmpty())
		{
			Input().SetWindowText(strCompleteCommand);
			return;
		}

}

void CNewWindowView::HandleInput()
{
		// track timing 
	//QPerformanceTimer qpTimer("HandleInput");
		

	CString strText;
	Input().GetWindowText(strText);

	CString strCommand = strText;
	
	CSession *pSession = GetDocument()->Session();

	if(!pSession->GetOptions().inputOptions_.RetainCommands())
	{
		Input().SetWindowText("");
		Input().RequestResize();
	}
	else
		Input().SendMessage(EM_SETSEL, 0, -1);

	if(!strText.IsEmpty() && strText.GetAt(0) == '!')
	{
		//search and retrieve from history...set as most recent command if found
		CString strCompleteCommand;
		SearchAndRetrieveCommandHistory(strCommand, strCompleteCommand);
		if(strCompleteCommand.IsEmpty())
		{
			strText = strCommand;
		}
		else
		{
			strText = strCompleteCommand;
		}
	}
	else if(!strText.IsEmpty())
	{
		//search history, if command found in history, set as most recent command
		//else add to history as most recent command
		AddCommandToHistory(strCommand);
	}

	CommandScrollPos(NULL);
	ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);

	if(Verbatim() && strText[0] != CGlobals::GetCommandCharacter())
	{
		SendTextToMud((LPCTSTR)strText);
		//TODO:KW Sending input so reset the time for tracking time since input to now.
		//pSession->SetTimeSinceLastInput();

		if(pSession->GetOptions().terminalOptions_.LocalEcho())
		{
			std::string command = static_cast<LPCTSTR>( strText );            
			PrintCommand(command);
		}
		else
		{
			Terminal().AvertAppendNext();
		}
	}
	else
	{
		CString command = strText;
		//TODO: KW onInputLine processing for dll support
		CString onReceive = "onInputLine";
		pSession->SetHandledByDll(FALSE);
		pSession->GetDlls().CallAll( pSession, onReceive, strText );
		//TODO: KW END test calling all dlls
		// do not process input here if dll says it handled it
		if(!pSession->GetHandledByDll())
			try
			{
		if(strText.MakeLower() == "/walkstop")
			pSession->KillSpeedwalk();  //don't delay executing this
			pSession->ExecuteCommand( command );
			}
			catch(...)
			{
				::OutputDebugString(_T("Error in NewWindowView::HandleInput calling ExecuteCommand\n"));
			}
		//always save input in history
		if( !strText.IsEmpty() ) LastCommand( strText );
	}
	//UpdateAllOptions();
}

void CNewWindowView::HandleScreenScrollLineUp()
{
	//GetDocument()->Session()->ScrollLineUp(this);
	Terminal().ScrollLineUp();
}

void CNewWindowView::HandleCommandScrollUp()
{
	CSession *pSession = GetDocument()->Session();

	if(pSession->GetCommandHistoryBuffer().IsEmpty())
		return;

	if(CommandScrollPos() == NULL)
	{
		switch(ScrollDirection())
		{
		case SCROLL_DIRECTION_NOTSCROLLING:
			CommandScrollPos(pSession->GetCommandHistoryBuffer().GetHeadPosition());

			if(CommandScrollPos() == NULL)
			{
				ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
				return;
			}
			break;
		case SCROLL_DIRECTION_SCROLLUP:
			ScrollDirection(SCROLL_DIRECTION_SCROLLUP);
			return;
			break;
		case SCROLL_DIRECTION_SCROLLDOWN:
			CommandScrollPos(pSession->GetCommandHistoryBuffer().GetHeadPosition());
			if(CommandScrollPos() == NULL)
			{
				ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
				return;
			}
			break;
		}
	}
	else
	{
		if(ScrollDirection() == SCROLL_DIRECTION_SCROLLDOWN)
		{
			pSession->GetCommandHistoryBuffer().GetNext(CommandScrollPos());
			if(CommandScrollPos() == NULL)
			{
				ScrollDirection(SCROLL_DIRECTION_SCROLLUP);
				return;
			}
		}
	}

	ScrollDirection(SCROLL_DIRECTION_SCROLLUP);

	CString strCommand = pSession->GetCommandHistoryBuffer().GetNext(CommandScrollPos());

	Input().SetWindowText(strCommand);
	Input().SetSel(Input().GetTextLength(),Input().GetTextLength()); //put cursor at the end of the retrieved command

}

void CNewWindowView::HandleScreenScrollLineDown()
{
	//GetDocument()->Session()->ScrollLineDown(this);
	Terminal().ScrollLineDown();
}

void CNewWindowView::HandleCommandScrollDown()
{
	CSession *pSession = GetDocument()->Session();

	if(pSession->GetCommandHistoryBuffer().IsEmpty())
	{
		Input().SetWindowText("");
		ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
		return;
	}

	if(CommandScrollPos() == NULL)
	{
		switch(ScrollDirection())
		{
		case SCROLL_DIRECTION_NOTSCROLLING:
			Input().SetWindowText("");
			return;
			break;
		case SCROLL_DIRECTION_SCROLLUP:
			CommandScrollPos(pSession->GetCommandHistoryBuffer().GetTailPosition());
			pSession->GetCommandHistoryBuffer().GetPrev(CommandScrollPos());
			if(CommandScrollPos() == NULL)
			{
				Input().SetWindowText("");
				ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
				return;
			}
			break;
		case SCROLL_DIRECTION_SCROLLDOWN:
			Input().SetWindowText("");
			ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
			return;
			break;
		}
	}
	else
	{
		if(ScrollDirection() == SCROLL_DIRECTION_SCROLLUP)
		{
			pSession->GetCommandHistoryBuffer().GetPrev(CommandScrollPos());
			if(CommandScrollPos() == NULL)
			{
				Input().SetWindowText("");
				ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
				return;
			}
			pSession->GetCommandHistoryBuffer().GetPrev(CommandScrollPos());
			if(CommandScrollPos() == NULL)
			{
				Input().SetWindowText("");
				ScrollDirection(SCROLL_DIRECTION_NOTSCROLLING);
				return;
			}
		}
	}

	CString strCommand = pSession->GetCommandHistoryBuffer().GetPrev(CommandScrollPos());
	ScrollDirection(SCROLL_DIRECTION_SCROLLDOWN);
	Input().SetWindowText(strCommand);
	Input().SetSel(Input().GetTextLength(),Input().GetTextLength()); //put cursor at the end of the retrieved command
}

void CNewWindowView::HandleScreenScrollPageUp()
{
	//GetDocument()->Session()->ScrollPageUp(this);
	Terminal().ScrollPageUp();
}

void CNewWindowView::HandleScreenScrollPageDown()
{
	//GetDocument()->Session()->ScrollPageDown(this);
	Terminal().ScrollPageDown();
}

void CNewWindowView::SearchAndRetrieveCommandHistory(CString &strLookFor, CString &strResult)
{
	if(strLookFor.GetLength() < 1)
	{
		if(GetDocument()->Session()->GetCommandHistoryBuffer().IsEmpty())
		{
			strResult = "";
			return;
		}
		
		strResult = LastCommand();
		return;
	}

	if(GetDocument()->Session()->GetCommandHistoryBuffer().IsEmpty())
		return;

	CString strSearchString = strLookFor.Right(strLookFor.GetLength()/*-1 ! isn't used anymore*/);

	POSITION posFound;

	for(POSITION pos = GetDocument()->Session()->GetCommandHistoryBuffer().GetHeadPosition();(posFound = pos) != NULL;)
	{
		CString strString = GetDocument()->Session()->GetCommandHistoryBuffer().GetNext(pos);
		if(strString.GetLength() < strSearchString.GetLength())
			continue;

		CString strCompare = strString.Left(strSearchString.GetLength());
		if(strSearchString.Compare(strCompare) == 0)
		{
			// Ozymandias said this is wrong, !x should not change most recent command
			//GetDocument()->Session()->GetCommandHistoryBuffer().RemoveAt(posFound);
			//GetDocument()->Session()->GetCommandHistoryBuffer().AddHead(strString);
			strResult = strString;
			return;
		}
	}
}

void CNewWindowView::AddCommandToHistory(CString &strCommand)
{
	if(GetDocument()->Session()->GetCommandHistoryBuffer().IsEmpty())
	{
		GetDocument()->Session()->GetCommandHistoryBuffer().AddHead(strCommand);
		return;
	}


	POSITION posFound;

	for(POSITION pos = GetDocument()->Session()->GetCommandHistoryBuffer().GetHeadPosition();(posFound = pos) != NULL;)
	{
		CString strString = GetDocument()->Session()->GetCommandHistoryBuffer().GetNext(pos);
		if(strString.Compare(strCommand) == 0)
		{
			GetDocument()->Session()->GetCommandHistoryBuffer().RemoveAt(posFound);
			GetDocument()->Session()->GetCommandHistoryBuffer().AddHead(strString);
			return;;
		}
	}

	GetDocument()->Session()->GetCommandHistoryBuffer().AddHead(strCommand);

	if((UINT)GetDocument()->Session()->GetCommandHistoryBuffer().GetCount() > (UINT)GetDocument()->Session()->GetOptions().inputOptions_.CommandHistoryBufferSize())
	{
		GetDocument()->Session()->GetCommandHistoryBuffer().RemoveTail();
	}
}

void CNewWindowView::OnRequestresizeInputCtrl(NMHDR *pNMHDR, LRESULT * /*pResult*/)
{
	REQRESIZE *pReqResize = reinterpret_cast<REQRESIZE *>(pNMHDR);
	int nHeight = pReqResize->rc.bottom - pReqResize->rc.top;
	if(GetDocument()->Session()->GetOptions().inputOptions_.ShowBorder())
		nHeight += 5;
	if(nHeight != InputHeight())
	{
		InputHeight(nHeight);
		RecalcLayout();

	}
}

void CNewWindowView::RecalcLayout()
{	
	// track timing of RecalcLayout
//	QPerformanceTimer QPTimer("NewWindowView RecalcLayout");

	CRect rect;
	GetClientRect(&rect);

	int sbDesiredHeight = 0;
	if(StatusBarVisible())
		sbDesiredHeight = StatusWindow().DesiredHeight();

	CRect sbRect;
	sbRect.left = 0;
	sbRect.right = rect.right;

	CRect inpRect;
	inpRect.left = 0;
	inpRect.right = rect.right;

	if(CStatusBarOptions::STATUS_POS_BELOW == 
		GetDocument()->Session()->GetOptions().statusOptions_.GetStatusbarPos())
	{
		sbRect.bottom = rect.bottom;
		sbRect.top = sbRect.bottom - sbDesiredHeight;

		inpRect.bottom = sbRect.top;
		inpRect.top = inpRect.bottom - InputHeight();
	}
	else
	{
		inpRect.bottom = rect.bottom;
		inpRect.top = inpRect.bottom - InputHeight();

		sbRect.bottom = inpRect.top;
		sbRect.top = sbRect.bottom - sbDesiredHeight;
	}

	StatusWindow().MoveWindow(sbRect);
	Input().MoveWindow(inpRect);

	Terminal().MoveWindow(0, rect.top, rect.right, rect.bottom - sbDesiredHeight - InputHeight(),TRUE);
	if(GetDocument()->Session()->GetTelnetNAWS())
	{
        _pMudSocket->Send(GetDocument()->Session()->GetTelnetNAWSmessage(),9);
	}
}

BOOL CNewWindowView::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CNewWindowView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	Input().SetFocus();	
}

BOOL CNewWindowView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CNewWindowView::OnConfigurationStatusbaritems() 
{
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("document is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("session is null!"));
		ATLASSERT(FALSE);
		return;
	}

	CBarItemListDlg dlg(pSession->GetOptions(), this);
	dlg.m_pBarItemList = pSession->GetStatusBar();
	dlg.DoModal();	
	StatusWindow().RedrawStatusBar(GetDocument()->Session());
}

void CNewWindowView::OnDestroy() 
{
	CView::OnDestroy();

	unhookEvent(GetDocument()->Session());
	unhookEvent(&Terminal());
}

void CNewWindowView::OnInputcontextCopy() 
{
	Input().Copy();	
}

void CNewWindowView::OnInputcontextCut() 
{
	Input().Cut();	
}

void CNewWindowView::OnInputcontextPaste() 
{
	Input().Paste();
}

void CNewWindowView::OnEditCopy() 
{
	Input().Copy();	
}

void CNewWindowView::OnEditCut() 
{
	Input().Cut();
}

void CNewWindowView::OnEditPaste() 
{
	Input().Paste();	
	CString strText;
	long nStartChar, nEndChar;

	Input().GetSel(nStartChar, nEndChar); 

	Input().GetWindowText(strText);
	if (strText.IsEmpty())
		return;

	CString strCommand = strText;
//	strCommand.Replace("\r\n","\n");
	strCommand.Replace("\r"," ");
	Input().SetWindowText(strCommand);
	Input().SetSel(nEndChar, nEndChar);

}

void CNewWindowView::InitializeDirectSound()
{
///#ifndef MM2K6_NODIRECTSOUND
	ASSERT(NULL == _pGlobalSoundManager.get());
	_pGlobalSoundManager.reset(new CGlobalSoundManager);
	HRESULT hr = _pGlobalSoundManager->Initialize(GetSafeHwnd());
	if(FAILED(hr))
	{
		ASSERT(FALSE);
	}
	hr = SetSoundPath();
//#endif /* MM2K6_NODIRECTSOUND */
}

HRESULT CNewWindowView::SetSoundPath()
{
//#ifndef MM2K6_NODIRECTSOUND
	ASSERT(_pGlobalSoundManager.get() != NULL);

	HRESULT hr = E_UNEXPECTED;
	try
	{
		std::string path;
		if(FAILED(GetDocument()->Session()->GetOptions().pathOptions_.Get_SoundPath(path)))
		{
			ATLASSERT(FALSE);
		}

		if(path.empty())
		{
			CString folder;
			if( MMFileUtils::getPersonalFolder( folder ) )
				path = folder;
		}

		_pGlobalSoundManager->SetSearchDirectory(path.c_str());

		hr = S_OK;
	}
	catch(_com_error &e)
	{
		hr = e.Error();
	}
	return hr;
//#else
//	return S_OK;
//#endif
}

void CNewWindowView::UpdateStatusBarOptions()
{
	CSession *pSession = GetDocument()->Session();

	RecalcLayout();

	StatusWindow().SetStatusBarInfo(pSession->GetStatusBar());

	StatusWindow().SetColorArrays(
		pSession->GetOptions().terminalOptions_.TerminalWindowForeColors(),
		pSession->GetOptions().terminalOptions_.TerminalWindowForeColors());

	CStatusBarOptions &options = pSession->GetOptions().statusOptions_;

	StatusWindow().SetFont(options.GetStatusBarFont());
	StatusWindow().SetBackColor(options.GetStatusBarBackColor());
	StatusWindow().SetForeColor(options.GetStatusBarForeColor());
	StatusWindow().SetStatusBarLines(options.GetStatusBarLines());
	StatusWindow().SetStatusBarWidth(options.GetStatusBarWidth());

	StatusWindow().UpdateAllOptions();
	StatusBarVisible(options.IsVisible() ? true : false);
	RecalcLayout();
	

}


void CNewWindowView::OnSpeedwalk()
{
	KillTimer(SpeedWalkTimer());
	CIfxDoc *pDoc = GetDocument();
	if(NULL == pDoc)
	{
		TRACE(_T("NULL == pDoc"));
		KillTimer(SpeedWalkTimer());
		SpeedWalkTimer(NULL);
		return;
	}

	CSession *pSession = pDoc->Session();
	if(NULL == pSession)
	{
		TRACE(_T("NULL == pSession"));
		KillTimer(SpeedWalkTimer());
		SpeedWalkTimer(NULL);
		return;
	}

	if(pSession->SpeedwalkDone())
	{
		SpeedWalkTimer(NULL);
		speedWalking_ = false;
		unloadSpeedWalkQueue();
		return;
	}

	TCHAR szSend[2] = {0};
	szSend[0] = pSession->GetNextSpeedwalkStep();

	if (GetDocument()->Session()->GetOptions().terminalOptions_.LocalEcho())
		PrintCommand(szSend);

	SendTextToMud(szSend);

	int nPace = pSession->GetOptions().sessionInfo_.Pace();
	SpeedWalkTimer(SetTimer(IDT_SPEEDWALK, nPace, NULL));
}

void CNewWindowView::queueLine(const CString &line)
{
	speedWalkQueue_.push_back( line );
}

void CNewWindowView::unloadSpeedWalkQueue()
{
	std::vector<CString> li;
	li.assign(speedWalkQueue_.begin(), speedWalkQueue_.end());

	speedWalkQueue_.clear();

	std::for_each(li.begin(), li.end(), HandleSingleLineFunc(this));
}

void CNewWindowView::hookEvent(CSession *pSession)
{
	__hook(&CSession::FullScreen,        pSession, &CNewWindowView::HandleFullScreen);
	__hook(&CSession::ScrollGrep,        pSession, &CNewWindowView::handleScrollGrep);
	__hook(&CSession::PrintAnsi,         pSession, &CNewWindowView::PrintAnsi);
    __hook(&CSession::SetBackground,     pSession, &CNewWindowView::SetBackground);
    __hook(&CSession::ScreenShot,        pSession, &CNewWindowView::ScreenShot);
	__hook(&CSession::ExecuteCommand,    pSession, &CNewWindowView::HandleCommandLine);
	__hook(&CSession::ShowStatusBar,     pSession, &CNewWindowView::ShowStatusBar);
	__hook(&CSession::ProcessLineNoChat, pSession, &CNewWindowView::ProcessLineNoChat);

}

void CNewWindowView::unhookEvent(CSession *pSession)
{
	__unhook(&CSession::FullScreen,        pSession, &CNewWindowView::HandleFullScreen);
	__unhook(&CSession::ScrollGrep,        pSession, &CNewWindowView::handleScrollGrep);
	__unhook(&CSession::PrintAnsi,         pSession, &CNewWindowView::PrintAnsi);
	__unhook(&CSession::SetBackground,     pSession, &CNewWindowView::SetBackground);
	__unhook(&CSession::ScreenShot,        pSession, &CNewWindowView::ScreenShot);
	__unhook(&CSession::ExecuteCommand,    pSession, &CNewWindowView::HandleCommandLine);
	__unhook(&CSession::ShowStatusBar,     pSession, &CNewWindowView::ShowStatusBar);
	__unhook(&CSession::ProcessLineNoChat, pSession, &CNewWindowView::ProcessLineNoChat);
}

void CNewWindowView::hookEvent(CSplitterBar *pBar)
{
	__hook(&CSplitterBar::ErrorMessage, pBar, &CNewWindowView::handleTermWindowErrorMessage);
	__hook(&CSplitterBar::ScrollGrepMatch, pBar, &CNewWindowView::handleScrollGrepMatch);
}

void CNewWindowView::unhookEvent(CSplitterBar *pBar)
{
	__unhook(&CSplitterBar::ErrorMessage, pBar, &CNewWindowView::handleTermWindowErrorMessage);
	__unhook(&CSplitterBar::ScrollGrepMatch, pBar, &CNewWindowView::handleScrollGrepMatch);
}

void CNewWindowView::handleScrollGrep(ULONG nLines, const CString &searchCriteria)
{
	this->Terminal().ScrollGrep(nLines, searchCriteria);
}

void CNewWindowView::handleTermWindowErrorMessage(const CString &errorMessage)
{
	PrintMessage(errorMessage);
}

void CNewWindowView::handleScrollGrepMatch(const CStringList &grepMatch)
{
	int nResult = 0;
	CString strMessage(_T("Matches Found\n"));
	POSITION pos = grepMatch.GetTailPosition();
	while(pos)
	{
		CString strText = grepMatch.GetPrev(pos);
		strText.Format("%s%03d:%s %s\n",
			ANSI_F_BOLDWHITE,
			nResult + 1,
			ANSI_RESET,
			strText);
		nResult++;
		strMessage += strText;
	}

	PrintAnsi(strMessage, false);
}

void CNewWindowView::UpdateChatName(const CString &strOldChatname)
{
	CIfxDoc *pDoc = GetDocument();
	ATLASSERT(NULL != pDoc);

	CSession *pSession = pDoc->Session();
	ATLASSERT(NULL != pSession);

	CString strName = pSession->GetOptions().chatOptions_.ChatName();
	if(!strName.Compare(strOldChatname))
	{
		// only "change" it if its different
		return;
	}

	int nLen = strName.GetLength();
	int i = 0;
	for (; i < nLen; ++i)
		if ((BYTE)strName[i] < 32 || (BYTE)strName[i]>122)
			break;
	if (i != nLen)
	{
		pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, "You can only use letters and numbers in your chat name.",FALSE);
		return;
	}

	pSession->GetOptions().chatOptions_.ChatName(strName);

	CString strText;
	strText.Format("Chat name changed: %s",(LPCSTR)strName);
	pSession->QueueMessage(CMessages::MM_CHAT_MESSAGE, strText,FALSE);
	pSession->GetChat().SetChatName(strName);
}
void CNewWindowView::HandleFullScreen()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->OnFullScreen();
}

LRESULT CNewWindowView::OnPrintMessage(WPARAM /*wParam*/, LPARAM lParam)
{
	PrintMessage((LPCTSTR)lParam);
	return 0;
}
void CNewWindowView::OnSessionStopchatserver() 
{
	GetDocument()->Session()->StopChatServer();	
}

void CNewWindowView::ShowStatusBar()
{
	StatusBarVisible(true);
	RecalcLayout();
}

bool CNewWindowView::LastLineGagged() const
{
	return _lastLineGagged;
}

void CNewWindowView::LastLineGagged(bool gagged)
{
	_lastLineGagged = gagged;
}

const std::string &CNewWindowView::LogFileName() const
{
	return _logFileName;
}

void CNewWindowView::LogFileName(const std::string &name)
{
	_logFileName = name;
}

bool CNewWindowView::Logging() const
{
	return _logging;
}

void CNewWindowView::Logging(bool logging)
{
	_logging = logging;
}
const std::string &CNewWindowView::RawMudLogFileName() const
{
	return _RawMudlogFileName;
}

void CNewWindowView::RawMudLogFileName(const std::string &name)
{
	_RawMudlogFileName = name;
}

bool CNewWindowView::RawMudLogging() const
{
	return _RawMudlogging;
}

void CNewWindowView::RawMudLogging(bool RawMudlogging)
{
	_RawMudlogging = RawMudlogging;
}

bool CNewWindowView::ChangesTimer() const
{
	return _changesTimer;
}
void CNewWindowView::CheckShowmeViaWebQueue()
{
	CString command("/showmeviaweb ");
	if(!GetDocument()->Session()->GetShowmeViaWebBuffer().IsEmpty())
		{
        	command.Append(GetDocument()->Session()->GetShowmeViaWebBuffer().GetHead());
			GetDocument()->Session()->GetShowmeViaWebBuffer().RemoveHead();
			GetDocument()->Session()->ExecuteCommand( command );
		}
}
void CNewWindowView::ChangesTimer(bool changes)
{
	_changesTimer = changes;
}

POSITION &CNewWindowView::CommandScrollPos()
{
	return _commandScrollPos;
}

void CNewWindowView::CommandScrollPos(POSITION pos)
{
	_commandScrollPos = pos;
}

int CNewWindowView::ScrollDirection() const
{
	return _scrollDirection;
}

void CNewWindowView::ScrollDirection(int direction)
{
	_scrollDirection = direction;
}

int CNewWindowView::InputHeight() const
{
	return _inputHeight;
}

void CNewWindowView::InputHeight(int height)
{
	_inputHeight = height;
}

const CString &CNewWindowView::LastCommand() const
{
	return _lastCommand;
}

void CNewWindowView::LastCommand(const CString &command)
{
	_lastCommand = command;
}

bool CNewWindowView::StatusBarVisible() const
{
	return _statusBarVisible;
}

void CNewWindowView::StatusBarVisible(bool visible)
{
	_statusBarVisible = visible;
}

bool CNewWindowView::Verbatim() const
{
	return _verbatim;
}

void CNewWindowView::Verbatim(bool verbatim)
{
	_verbatim = verbatim;
}

INT_PTR CNewWindowView::SpeedWalkTimer() const
{
	return _speedWalkTimer;
}

void CNewWindowView::SpeedWalkTimer(INT_PTR timer)
{
	_speedWalkTimer = timer;
}

CStatusWindow &CNewWindowView::StatusWindow()
{
	return _wndStatus;
}

CSplitterBar &CNewWindowView::Terminal()
{
	return _wndTerm;
}

CMyInputControl &CNewWindowView::Input()
{
	return _wndInput;
}
//void CNewWindowView::OnWindowPosChanged(WINDOWPOS* /*lpwndpos*/)
//{
//	//if(GetDocument()->Session()->OnTop()) 
//		//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
//	if(GetDocument()->Session()->OnTop()) 
//		GetParentFrame()->BringWindowToTop();
//		
//}