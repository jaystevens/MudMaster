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
#include "AnsiParser.h"
#include "ColorLine.h"
#include "Colors.h"
#include "Telnet.h"
#include "Sess.h"
#include "TermWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MudmasterColors;
using namespace Telnet;

static const char *parse_states[] = 
{
	"PARSE_START",
	"PARSE_ANSI",
	"PARSE_CODE",
	"PARSE_IAC",
	"PARSE_WILL",
	"PARSE_WONT",
	"PARSE_DO",
	"PARSE_DONT"
};

CAnsiParser::CAnsiParser()
{
	ZeroMemory(m_AnsiArg, ANSI_ARG_LEN);
	m_nArgLen = 0;
	m_nParseState = PARSE_START;
}

CAnsiParser::~CAnsiParser()
{
}

void CAnsiParser::ResetParser()
{

}

 CCharFormat CAnsiParser::MakeBold(  CCharFormat _cfCurrentFormat)
{
	if(_cfCurrentFormat.Fore()==COLOR_RED)
					{
						_cfCurrentFormat.Fore(COLOR_LIGHTRED);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_BLACK)
					{
						_cfCurrentFormat.Fore(COLOR_DARKGRAY);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_GREEN)
					{
						_cfCurrentFormat.Fore(COLOR_LIGHTGREEN);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_BROWN)
					{
						_cfCurrentFormat.Fore(COLOR_YELLOW);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_BLUE)
					{
						_cfCurrentFormat.Fore(COLOR_LIGHTBLUE);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_CYAN)
					{
						_cfCurrentFormat.Fore(COLOR_LIGHTCYAN);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_MAGENTA)
					{
						_cfCurrentFormat.Fore(COLOR_LIGHTMAGENTA);
					}
					else if(_cfCurrentFormat.Fore()==COLOR_GRAY)
					{
						_cfCurrentFormat.Fore(COLOR_WHITE);
					}
	return _cfCurrentFormat;
}

void CAnsiParser::ParseAnsi(const char *strLine, CColorLine::SpColorLine &line, CMudSocket* pMudSocket, CSession* pSession)
{
	const char *ptr = strLine;
				
	static char ansi_terminators[] = "HFABCDnsuJKmp";

	int nNumSpaces = 0;	//Used by the ansi C command

	line->SetFormat(m_cfCurrentFormat);

	int counter = 0; // used by the \t state

	CString strCommand;    //used in execute command calls

	while(*ptr != '\0' )
	{
		switch(m_nParseState)
		{
		case PARSE_START:
			switch((BYTE)*ptr)
			{
    		case GA:
				//m_nParseState = PARSE_START;
                line->Add(*ptr);
                //return;
				*ptr++;
				break;

			case 0x1B:
				m_nParseState = PARSE_ANSI;
				ptr++;
				break;
			
			case IAC:
				m_nParseState = PARSE_IAC;
				ptr++;
				break;

			case '\t':
				for(counter = 0; counter < 4; counter++)
					line->Add(' ');
				ptr++;
				break;

			case 7: // bell we'll ignore it for now, maybe add a feature for it later
				ptr++;
				break;

			case 8:	//backspace
				ptr++;
				break;

			case '\r':
				ptr++;
				break;

			default:
				// copy the character into the outgoing text buffer
				// using the current text formating scheme
				line->Add(*ptr);
				// increment the pointer to the next character
				ptr++;
			}
			break;

		case PARSE_ANSI:
			if(*ptr != '[')
			{
				m_nParseState = PARSE_START;
			}
			else
			{
				m_nParseState = PARSE_CODE;
				ptr++;
			}
			break;
		case PARSE_SB:
			if(SE == *(ptr+1) && IAC == *ptr ) // need to look for IAC SE marking end of subcommand
			{
				m_nParseState = PARSE_START;
				ptr++;
			}
			else
			{
				if(AUTH == *ptr)
				{
					m_nParseState = PARSE_SB_AUTH;
				}
				else
				{
					m_nParseState = PARSE_START;
					while(!(SE == *(ptr+1) && IAC == *ptr))
					{
						//continue until finding the end of the subcommand we are ignoring
						ptr++;
					}
				}
			}
			ptr++;
			break;
		case PARSE_SB_AUTH:
			if(SE == (BYTE)*(ptr+1) && IAC == (BYTE)*ptr ) // need to look for IAC SE
			{
				m_nParseState = PARSE_START;
				ptr++;
				break;
			}
			else
			{
				switch((BYTE)*ptr)
				{
				case IS:
					ptr++;
					if(AUTHKEY ==  *ptr ) //
					{
						ptr++;
						counter = 5;
						telnetAuthkeyResponse[0] = IAC;
						telnetAuthkeyResponse[1] = SB;
						telnetAuthkeyResponse[2] = AUTH;
						telnetAuthkeyResponse[3] = REPLY;
						telnetAuthkeyResponse[4] = AUTHKEY;
						//parse authkey 
						while(!(SE == (BYTE)*(ptr+1) && IAC == (BYTE)*ptr ))
						{
							telnetAuthkeyResponse[counter] = *ptr;
							ptr++;
							counter++;
						}
						ptr++;
						ptr++;;
						telnetAuthkeyResponse[counter] = IAC;
						counter++;
						telnetAuthkeyResponse[counter] = SE;	
						counter++;
						telnetAuthkeyResponse[counter] = '\0';
						//now store the whole response at the session level.
						pSession->SetTelnetAuthkeyResponse((LPCSTR)telnetAuthkeyResponse);
						//strCommand = "/sessionauthkey {";
						//strCommand += CString((LPCSTR)telnetCommand.GetData(),telnetCommand.GetSize()) +"}";

						//pSession->ExecuteCommand(strCommand);
						m_nParseState = PARSE_START;
						break;
					}
					if(SERVERADDRESS ==  *ptr ) //
					{
						//parse server name and set /sessionaddress
						ptr++;
						counter = 0;
						//parse address
						while(SE != (BYTE)*(ptr+1) && IAC != (BYTE)*ptr && *ptr != '\0')
						{
							telnetCommand[counter] = *ptr;
							ptr++;
							counter++;
							
								
						}
						ptr++;
						ptr++;
						telnetCommand[counter] = '\0';
						strCommand = "/sessionaddress {";
						strCommand += CString((LPCSTR)telnetCommand) +"}";

						pSession->ExecuteCommand(strCommand);
						m_nParseState = PARSE_START;
						break;
					}
					if(PORT ==  *ptr ) //
					{
						//parse port and set /sessionport
						ptr++;
						counter = 0;
						//parse port 
						while(!(SE == (BYTE)*(ptr+1) && IAC == (BYTE)*ptr ))
						{
							telnetCommand[counter] = *ptr;
							ptr++;
							counter++;
						}
						ptr++;
						ptr++;
						telnetCommand[counter] = '\0';
						strCommand = "/sessionport {";
						strCommand += CString((LPCSTR)telnetCommand) +"}";

						pSession->ExecuteCommand(strCommand);
						m_nParseState = PARSE_START;
						break;
					}
					break;
				case SEND:
					ptr++;
					if(AUTHKEY == *ptr)
					{
						ptr++;
						ptr++;
						ptr++; //advance past IAC SE
						
						counter =CString(pSession->GetTelnetAuthkeyResponse()).GetLength();
						
						//grab the session's authkey response
						
						//send the mud the authkey response directly
						pMudSocket->Send(pSession->GetTelnetAuthkeyResponse(), counter);
						m_nParseState = PARSE_START;
					}
					break;
				case REPLY:
				case NAME:
					break;
				}
				break;
			}
			break;

		case PARSE_CODE:
			if(strchr(ansi_terminators, *ptr))
			{
				switch(*ptr)
				{
				case 'C':
					// the argument for this command is the number of spaces
					// to forward the cursor.  We'll just add that many spaces
					// to the line
					nNumSpaces = atoi(m_AnsiArg);
					line->AddChars(nNumSpaces, ' ');					
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					break;
				case 'm':
					// This is where we'll set the graphic rendition
					SetGraphics();
					line->SetFormat(m_cfCurrentFormat);
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					break;
				}
				m_nParseState = PARSE_START;
				ptr++;
			}
			else
			{
				m_AnsiArg[m_nArgLen] = *ptr;
				m_nArgLen++;
				m_AnsiArg[m_nArgLen] = '\0';
				ptr++;
				if(m_nArgLen>98)
				{
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					m_nParseState = PARSE_START;
					//if 99 ANSI args then clearly this is broken so ignore the ANSI command
				}
			}
			break;

		case PARSE_WILL:
					telnetCommand[0] = IAC;
					telnetCommand[1] = DONT;
					telnetCommand[2] = *ptr;
					telnetCommand[3] = 0x00;
					//telentCommand[4] = '\0';
					pMudSocket->Send((LPCSTR)telnetCommand, 3);
					ptr++;
					m_nParseState = PARSE_START;
					break;

		case PARSE_WONT:
		case PARSE_DO:
				switch((BYTE)*ptr)
				{
				case AUTH:
					//send IAC WILL AUTH here
					
					telnetCommand[0] = IAC;
					telnetCommand[1] = WILL;
					telnetCommand[2] = AUTH;
					telnetCommand[3] = 0x00;
					//telentCommand[4] = '\0';
					
					//pMudSocket->Send(&telnetCommand, 4);
					pMudSocket->Send((LPCSTR)telnetCommand,3);
					break;
				case NAWS:
					//send IAC WILL AUTH here
					
					telnetCommand[0] = IAC;
					telnetCommand[1] = WILL;
					telnetCommand[2] = NAWS;
					telnetCommand[3] = 0x00;
					//telentCommand[4] = '\0';
					
					//pMudSocket->Send(&telnetCommand, 4);
					pMudSocket->Send((LPCSTR)telnetCommand,3);
					pSession->SetTelnetNAWS(true);
					pMudSocket->Send(pSession->GetTelnetNAWSmessage(),9);
					break;
				default:
					//send IAC WONT *ptr here
					
					telnetCommand[0] = IAC;
					telnetCommand[1] = WONT;
					telnetCommand[2] = *ptr;
					telnetCommand[3] = 0x00;
					//telentCommand[4] = '\0';
					pMudSocket->Send((LPCSTR)telnetCommand, 3);
					//CString strDoRequest;
					//strDoRequest.Format("do request number %d",telnetCommand.GetAt(2));
					//pSession->QueueMessage(CMessages::MM_GENERAL_MESSAGE, strDoRequest, TRUE);
					break;
				}
				ptr++;
				m_nParseState = PARSE_START;
				break;
		case PARSE_DONT:
			ptr++;
			m_nParseState = PARSE_START;
			break;

		case PARSE_IAC:
			switch((BYTE)*ptr)
			{
			case IAC:
				m_nParseState = PARSE_START;
				break;
			case WILL:
				m_nParseState = PARSE_WILL;
				break;
			case WONT:
				m_nParseState = PARSE_WONT;
				break;
			case DO:
				m_nParseState = PARSE_DO;
				break;
			case DONT:
				m_nParseState = PARSE_DONT;
				break;
			case SB:
				m_nParseState = PARSE_SB;
				break;
    		case GA:
				m_nParseState = PARSE_START;
				line->Add(':');
                //return;
				break;
			default:
				m_nParseState = PARSE_START;
			}
			ptr++;
			break;
		}
	}
}


void CAnsiParser::ParseAnsi(const char *strLine, CColorLine::SpColorLine &line)
{
	const char *ptr = strLine;
				
	static char ansi_terminators[] = "HFABCDnsuJKmp";

	int nNumSpaces = 0;	//Used by the ansi C command

	line->SetFormat(m_cfCurrentFormat);

	int counter = 0; // used by the \t state

	while(*ptr != '\0' )
	{
		switch(m_nParseState)
		{
		case PARSE_START:
			switch((BYTE)*ptr)
			{
    		case GA:
				//m_nParseState = PARSE_START;
                line->Add(*ptr);
                //return;
				*ptr++;
				break;

			case 0x1B:
				m_nParseState = PARSE_ANSI;
				ptr++;
				break;
			
			case IAC:
				m_nParseState = PARSE_IAC;
				ptr++;
				break;

			case '\t':
				for(counter = 0; counter < 4; counter++)
					line->Add(' ');
				ptr++;
				break;

			case 7: // bell we'll ignore it for now, maybe add a feature for it later
				ptr++;
				break;

			case 8:	//backspace
				ptr++;
				break;

			case '\r':
				ptr++;
				break;

			default:
				// copy the character into the outgoing text buffer
				// using the current text formating scheme
				line->Add(*ptr);
				// increment the pointer to the next character
				ptr++;
			}
			break;

		case PARSE_ANSI:
			if(*ptr != '[')
			{
				m_nParseState = PARSE_START;
			}
			else
			{
				m_nParseState = PARSE_CODE;
				ptr++;
			}
			break;
		case PARSE_SB:
			if(SE == (BYTE)*(ptr+1) && IAC == (BYTE)*ptr ) // need to look for IAC SE marking end of subcommand
			{
				m_nParseState = PARSE_START;
				ptr++;
			}
			else
			{
				if(AUTH == *ptr)
				{
					m_nParseState = PARSE_SB_AUTH;
				}
				else
				{
					m_nParseState = PARSE_START;
					while(!(SE == (BYTE)*(ptr+1) && IAC == (BYTE)*ptr))
					{
						//continue until finding the end of the subcommand we are ignoring
						ptr++;
					}
				}
			}
			ptr++;
			break;
		case PARSE_SB_AUTH:
			if(SE == *(ptr+1) && IAC == *ptr ) // need to look for IAC SE
			{
				m_nParseState = PARSE_START;
				ptr++;
			}
			else
			{
				switch((BYTE)*ptr)
				{
				case AUTHKEY:
				case SERVERADDRESS:
				case PORT:
					break;
				}
			}
			break;

		case PARSE_CODE:
			if(strchr(ansi_terminators, *ptr))
			{
				switch(*ptr)
				{
				case 'C':
					// the argument for this command is the number of spaces
					// to forward the cursor.  We'll just add that many spaces
					// to the line
					nNumSpaces = atoi(m_AnsiArg);
					line->AddChars(nNumSpaces, ' ');					
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					break;
				case 'm':
					// This is where we'll set the graphic rendition
					SetGraphics();
					line->SetFormat(m_cfCurrentFormat);
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					break;
				}
				m_nParseState = PARSE_START;
				ptr++;
			}
			else
			{
				m_AnsiArg[m_nArgLen] = *ptr;
				m_nArgLen++;
				m_AnsiArg[m_nArgLen] = '\0';
				ptr++;
				if(m_nArgLen>98)
				{
					ZeroMemory(m_AnsiArg, 100);
					m_nArgLen = 0;
					m_nParseState = PARSE_START;
					//if 99 ANSI args then clearly this is broken so ignore the ANSI command
				}
			}
			break;

		case PARSE_WILL:
		case PARSE_WONT:
		case PARSE_DO:
				switch((BYTE)*ptr)
				{
				case AUTH:
					//send IAC WILL AUTH here
					break;
				default:
					//send IAC WONT *ptr here
					break;
				}
				ptr++;
				m_nParseState = PARSE_START;
				break;
		case PARSE_DONT:
			ptr++;
			m_nParseState = PARSE_START;
			break;

		case PARSE_IAC:
			switch((BYTE)*ptr)
			{
			case IAC:
				m_nParseState = PARSE_START;
				break;
			case WILL:
				m_nParseState = PARSE_WILL;
				break;
			case WONT:
				m_nParseState = PARSE_WONT;
				break;
			case DO:
				m_nParseState = PARSE_DO;
				break;
			case DONT:
				m_nParseState = PARSE_DONT;
				break;
			case SB:
				m_nParseState = PARSE_SB;
				break;
    		case GA:
				m_nParseState = PARSE_START;
				line->Add(':');
                //return;
				break;
			default:
				m_nParseState = PARSE_START;
			}
			ptr++;
			break;
		}
	}
}

void CAnsiParser::SetGraphics()
{
	LPSTR p;
	LPCSTR pp;
	CCharFormat swapTemp;

	if (*m_AnsiArg && m_nArgLen)
	{
		pp = m_AnsiArg;
		do
		{
			p = const_cast<LPSTR> (strchr(pp,';'));
			if (p && *p)
			{
				*p = 0;
				p++;
			}

			switch (atoi(pp))
			{
			case 0: /* all attributes off */
				m_cfCurrentFormat.Bold(FALSE);
				m_cfCurrentFormat.Fore(COLOR_GRAY);
				m_cfCurrentFormat.Back(COLOR_BLACK);
				break;

			case 1: /* bright on */
				if(m_cfCurrentFormat.Bold())
				{
					m_cfCurrentFormat.Fore(COLOR_WHITE);
				}
				else
				{
					m_cfCurrentFormat.Bold(TRUE);
					m_cfCurrentFormat = MakeBold(m_cfCurrentFormat);
					//if(m_cfCurrentFormat.Fore()==COLOR_RED)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_LIGHTRED);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_BLACK)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_DARKGRAY);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_GREEN)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_LIGHTGREEN);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_BROWN)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_YELLOW);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_BLUE)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_LIGHTBLUE);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_CYAN)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_LIGHTCYAN);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_MAGENTA)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_LIGHTMAGENTA);
					//}
					//else if(m_cfCurrentFormat.Fore()==COLOR_GRAY)
					//{
					//	m_cfCurrentFormat.Fore(COLOR_WHITE);
					//}
				}
				break;

			case 2: /* faint on */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Bold(FALSE);
				break;

			case 3: /* italic on */
				break;

			case 5: /* blink on */
				break;

			case 6: /* rapid blink on */
				break;

			case 7: /* reverse video on */
				swapTemp.Back(m_cfCurrentFormat.Fore());
				if(m_cfCurrentFormat.Bold())
					swapTemp.Bold(TRUE);
                m_cfCurrentFormat.Fore( m_cfCurrentFormat.Back());
				m_cfCurrentFormat.Back(swapTemp.Back());
				if(swapTemp.Bold())
					m_cfCurrentFormat = MakeBold(m_cfCurrentFormat);
				break;

			case 8: /* concealed on */
				break;

			case 30: /* black fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_DARKGRAY);
				else
					m_cfCurrentFormat.Fore(COLOR_BLACK);
				break;

			case 31: /* red fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_LIGHTRED);
				else
					m_cfCurrentFormat.Fore(COLOR_RED);
				break;

			case 32: /* green fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_LIGHTGREEN);
				else
					m_cfCurrentFormat.Fore(COLOR_GREEN);
				break;

			case 33: /* yellow fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_YELLOW);
				else
					m_cfCurrentFormat.Fore(COLOR_BROWN);
				break;

			case 34: /* blue fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_LIGHTBLUE);
				else
					m_cfCurrentFormat.Fore(COLOR_BLUE);
				break;

			case 35: /* magenta fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_LIGHTMAGENTA);
				else
					m_cfCurrentFormat.Fore(COLOR_MAGENTA);
				break;

			case 36: /* cyan fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_LIGHTCYAN);
				else
					m_cfCurrentFormat.Fore(COLOR_CYAN);
				break;

			case 37: /* white fg */
				if(m_cfCurrentFormat.Bold())
					m_cfCurrentFormat.Fore(COLOR_WHITE);
				else
					m_cfCurrentFormat.Fore(COLOR_GRAY);
				break;

			case 40: /* black bg */
				m_cfCurrentFormat.Back(COLOR_BLACK);
				break;

			case 41: /* red bg */
				m_cfCurrentFormat.Back(COLOR_RED);
				break;

			case 42: /* green bg */
				m_cfCurrentFormat.Back(COLOR_GREEN);
				break;

			case 43: /* yellow bg */
				m_cfCurrentFormat.Back(COLOR_BROWN);
				break;

			case 44: /* blue bg */
				m_cfCurrentFormat.Back(COLOR_BLUE);
				break;

			case 45: /* magenta bg */
				m_cfCurrentFormat.Back(COLOR_MAGENTA);
				break;

			case 46: /* cyan bg */
				m_cfCurrentFormat.Back(COLOR_CYAN);
				break;

			case 47: /* white bg */
				m_cfCurrentFormat.Back(COLOR_WHITE);
				break;

			case 48: /* subscript bg */
				break;

			case 49: /* superscript bg */
				break;

			default: /* unsupported */
				break;
			}
			pp = p;
		} while (p);
	}
}