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
// ProcedureTable.h: interface for the CProcedureTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCEDURETABLE_H__C6526541_7803_11D3_BCD6_00E029482496__INCLUDED_)
#define AFX_PROCEDURETABLE_H__C6526541_7803_11D3_BCD6_00E029482496__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "HashTable.h"

class CProcedureTable;
class CSession;

class CProcedureTable  
{
	typedef BOOL (*PROCEDUREPROC)(CSession *pSession, CString &strParams, CString &strResult);

public:
	struct PROCLIST
	{
		char szProcedure[31];
		PROCEDUREPROC proc;
	};

	static BOOL BaritemBack(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL BaritemFore(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL ChatConnectIP(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL ChatConnectName(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL ChatFlags(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL ChatVersion(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL ForeColor(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL BackColor(CSession *pSession, CString& strParams, CString& strResult);
	static BOOL ReadableTime(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL CommandCount(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL CommandChar(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL ProcedureCount(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL Asc(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL SessionName(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL SessionPath(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL WordCount(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL WordColor(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Word(string,word number)
	// Returns a specific word in a string.
	static BOOL Word(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Upper(text)  @Lower(text)
	static BOOL Upper(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Lower(CSession *pSession,  CString &strParams, CString &strResult);

	static BOOL TimeToYear(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeZone(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToSecond(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToMonth(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToMinute(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToHour(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToDayOfWeek(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TimeToDay(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL TextColor(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @StrStr(string to search, string to search for)
	// Returns the index of where the string to search for starts
	// within the first string.  Returns -1 if not found.
	static BOOL StatusBarSize(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL StrStr(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL StrStrRev(CSession *pSession,  CString &strParams, CString &strResult);
	
	static BOOL StripAnsi(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @LTrim(text).  LTrim trims spaces from the left
	// side of a string, RTrim from the right.
	static BOOL RTrim(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL LTrim(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Right(text,number)
	// Take the right portion of a string.
	static BOOL Right(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Random(<num>)  returns a number between 1 and <num>.
	static BOOL Random(CSession *pSession,  CString &strParams, CString &strResult);
	// Format: @Replace(String,Oldstring,NewString) replaces old with new and returns the resulting string
	static BOOL Replace(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL RegEx(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL RegExArray(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL RegExMatch(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @PreTrans(string)
	// This is used to translate a string in a command (macros, actions,
	// etc...) before the command is executed.  You would use this when
	// you were defining a command like an alias to replace variables
	// at the time of creation instead of time of execution.
	static BOOL PreTrans(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Mid(text,start pos,number)
	static BOOL Mid(CSession *pSession,  CString &strParams, CString &strResult);
	
	// Format: @Len(text)
	// Retuns the string length.
	static BOOL Len(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Left(text,number)
	// Take the left portion of a string.
	static BOOL Left(CSession *pSession,  CString &strParams, CString &strResult);
	
	static BOOL IsNumber(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL IPP(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @IP()
	// Returns the IP address of the machine.
	static BOOL IP(CSession *pSession,  CString &strParams, CString &strResult);
	//Format: @If(condition) returns 1 if true 0 if false
	static BOOL If(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @InList(list,item)
	//g Returns 1 if the item is found in the list.
	static BOOL InList(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @EnumList(list,item)
	//g Returns 1 based index of the item is found in the list otherwise 0 if not found
	static BOOL EnumList(CSession *pSession,  CString &strParams, CString &strResult);

	// Gets an item from a list.  @GetItem(<list name>,<index>)
	static BOOL GetItem(CSession *pSession,  CString &strParams, CString &strResult);

	// Returns the number of items in a list.  @GetCount(<list name>)
	static BOOL GetCount(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @GetArray(Array name,row,col)
	// Returns the item from an array.
	static BOOL GetArray(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL GetArrayRows(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL GetArrayCols(CSession *pSession,  CString &strParams, CString &strResult);
	
	static BOOL ForeWhite(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeYellow(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeRed(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeMagenta(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeGreen(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeCyan(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeBlue(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ForeBlack(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @FileExists(filename)
	// Returns 1 if the file exists.  0 if not found.
	static BOOL FileExists(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Exists(var name)
	// Returns 1 if the variable exists.
	static BOOL Exists(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @EventTime(event name)
	// Returns the time left before an event fires.
	static BOOL EventTime(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Connected()
	// Returns 1 if connected.  For a modem this means there is a 
	// carrier signal.  For winsock it means a socket is connected.
	static BOOL Connected(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @ConCat(target,new text)
	// Concatenates strings.
	static BOOL ConCat(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL CProcedureTable::Comma(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL CProcedureTable::DeComma(CSession *pSession, CString &strParams, CString &strResult);
	static BOOL ChatName(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL ChatConnects(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL CharColor(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL CharBackColor(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackWhite(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackYellow(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Backward(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackRed(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackMagenta(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackGreen(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackCyan(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL BackBlue(CSession *pSession,  CString& strParams, CString& strResult);
	static BOOL BackBlack(CSession *pSession,  CString& strParams, CString& strResult);
	static BOOL AnsiReset(CSession *pSession,  CString& strParams, CString & strResult);
	static BOOL AnsiBold(CSession *pSession,  CString& strParams, CString& strResult);
//TODO: KW add reverse
	static BOOL AnsiRev(CSession *pSession,  CString& strParams, CString& strResult);

	// Format:: @Abs(num)
	// Returns the absolute value of a number.
	static BOOL Abs(CSession *pSession,  CString& strParams, CString& strResult);
	//@Alias(a deefined alias)
	static BOOL Alias(CSession *pSession,  CString& strParams, CString& strResult);


	// Returns the number of seconds elapsed since January 1, 1970
	static BOOL Time(CSession *pSession,  CString &strParams, CString &strResult);

	// Returns the current time.
	static BOOL Hour(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Minute(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Second(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Microsecond(CSession *pSession,  CString &strParams, CString &strResult);

	// Returns the current date.
	static BOOL Day(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Month(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL Year(CSession *pSession, CString &strParams, CString &strResult);
	
	// Returns the current version.
	static BOOL VersionProc(CSession *pSession,  CString &strParams, CString &strResult);

	// Returns the number of items.
	static BOOL NumActions(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumAliases(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumArrays(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumBarItems(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumEvents(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumGags(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumHighlights(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumLists(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumMacros(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumSubstitutes(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumTabList(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL NumVariables(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @PadLeft(string,char,num)
	// Pads string eithe ron the left of right with the the char
	// and num number of them.
	static BOOL PadRight(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL PadLeft(CSession *pSession,  CString &strParams, CString &strResult);
	// these pad to a total of num characters
	static BOOL RightPad(CSession *pSession,  CString &strParams, CString &strResult);
	static BOOL LeftPad(CSession *pSession,  CString &strParams, CString &strResult);

	// Works like Chr$ in basic.  Pass in a number and it
	// returns the character.
	static BOOL Chr(CSession *pSession,  CString &strParams, CString &strResult);

	// format: @IsEmpty(string)
	// returns 1 if th estring is empty
	static BOOL IsEmpty(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Value(text)
	// Returns the string as a number.  This can be useful in 
	// an /if statement where /if might get confused and
	// think a variable is a string rather than a number.
	static BOOL Val(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Math(formula)
	// Returns the result of a math formula.  Does the same thing
	// as /math, only it returns the result instead of puttig it
	// in a var.
	static BOOL Math(CSession *pSession,  CString &strParams, CString &strResult);

	// Format:: @Var(var name)
	// This function returns the value of a variable.  This is used
	// for looking up a variable named by another variable.  Let's
	// say you have a list of variables names.  And you want to see
	// the value of each item in the list.  You could do this:
	// @CommandToList(Names,say @Var($ListItem))
	static BOOL Var(CSession *pSession,  CString &strParams, CString &strResult);

	// Format: @Mid(text,start pos,number)
	static BOOL SubStr(CSession *pSession,  CString &strParams, CString &strResult);

	// Format:: @SubAll(strOrigString, strReplaceString)
	// This function will substitute all occurances of strOrigString in the text
	// About to be printed with the text found in strReplaceString.
	//static BOOL SubAll(CSession *pSession, CString &strParams, CString &strResult);

	// Format:: @SubWord(nWordNumber, strReplaceString)
	// This function will substitue the nWordNumberth word with the string found
	// in strReplaceString
	//static BOOL SubWord(CSession *pSession, CString &strParams, CString& strResult);

	// This function takes a procedure name and the parens enclosing
	// the parameters and returns true if it can be resolved to 
	// return a value.  If it can be the result string is placed in
	// strResult.
	static BOOL ProcedureVar(
		CSession *pSession, 
		const std::string &pszProc, 
		std::string &strResult );

	static bool GetParameters( const std::string &paramText, std::string &params );

	static BOOL DoProcedure(CSession *pSession, LPCSTR pszProc);
	
	static void CommandToList(CSession *pSession, CString &strParams);
	static PROCLIST *FindProcedure(CSession *pSession, const CString& strProc);
	static char *FindProcedureName(CSession *pSession, const CString& strProc);
	static BOOL Hash();
	static int Count();

	CProcedureTable();
	virtual ~CProcedureTable();

private:
	static PROCLIST m_List[];
	static CHashTable m_HashTable;
};

#endif // !defined(AFX_PROCEDURETABLE_H__C6526541_7803_11D3_BCD6_00E029482496__INCLUDED_)
