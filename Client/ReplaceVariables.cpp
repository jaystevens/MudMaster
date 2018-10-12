#include "stdafx.h"
#include ".\replacevariables.h"
#include "Globals.h"
#include "ParseUtils.h"
#include "Variable.h"
#include "Sess.h"
#include "VarList.h"
#include "ProcedureTable.h"
#include <sstream>
#include <strsafe.h>
// Tintin seems to work like this:  If you type a $<text>
// if looks it up in the variable list, if nothing is there
// "$<text>" prints, otherwise replaces it with the variable
// value.  It treats the $0 vars differently than I plan to
// though.  The $vars are global in MudMaster.  You can use
// $0 to print what is in that variable at any time.  Tintin
// only allows you to use that var it in a tintin statement
// I think.

// Variables names start with a $ always.  They are alpha
// only.  If the $ character is what is wanted instead of 
// representing a variable, the escape character should be
// used.

// Add procedures to the variable replacement list.  The only
// procedure that should be handled during var replacement are
// the ones that evaultate as a BOOL.

// Going to do this a as quick check to see if there are
// any possible vars in the string.

using namespace MMScript;

CReplaceVariables::CReplaceVariables( 
	CSession *_pSession,
	CString &line,
	bool quoteStrings )
: _pSession( _pSession )
, _line( line )
, _quoteStrings( quoteStrings )
, _foundVar( false )
, _foundProc( false )
, _foundProcParen( false )
, _procParenCount( 0 )
, _index( 0 )
, _length( line.GetLength()	)
{
}

CReplaceVariables::~CReplaceVariables(void)
{
	ATLASSERT( NULL != _pSession );
}

void CReplaceVariables::DoParseStart( ParseState &state )
{
	if( CGlobals::DOLLAR_SIGN == _line[_index] )
	{
		state = ParseVar;
		_varName.clear();
	}
	else if( CGlobals::GetProcedureCharacter() == _line[ _index ] )
	{
		state = ParseProc;
		_procName.clear();
	}
	else if( CGlobals::GetEscapeCharacter() == _line[ _index ] )
	{
		state = ParseEscape;
	}
	else
	{
		_temp += _line[ _index ];
	}

	++_index;
}

void CReplaceVariables::DoParseEscape( ParseState &state )
{
	if( CGlobals::DOLLAR_SIGN != _line[ _index ] && 
		CGlobals::GetProcedureCharacter() != _line[ _index ] )
	{
		_temp += CGlobals::GetEscapeCharacter();
	}

	_temp += _line[ _index ];
	++_index;
	state = ParseStart;
}

void CReplaceVariables::QuoteVar( const std::string &var )
{
	if( _quoteStrings && !CParseUtils::IsNumber( var.c_str() ) )
	{
		CString strQuoted;
		CParseUtils::EscapeQuote( var.c_str(), strQuoted );
		_temp += _T('"');
		_temp += static_cast<LPCTSTR>( strQuoted );
		_temp += _T('"');
	}
	else
	{
		_temp += var;
	}
}

void CReplaceVariables::DoGlobalVar()
{
	CString globalVar = CGlobals::GetGlobalVar( _line[_index] - '0' );
	QuoteVar( static_cast<LPCTSTR>( globalVar ) );
}

void CReplaceVariables::DoParseVar( ParseState &state )
{
	if( ::isdigit( _line[ _index ] ) )
	{
		DoGlobalVar();
		state = ParseStart;
	}
	else if( ::isalpha( _line[ _index ] ) )
	{
		_varName += _line[ _index ];
		state = ParseVarName;
	}

	++_index;
}

void CReplaceVariables::ProcessVar()
{
	CVariable v;
	if( _pSession->GetVariables()->FindByName( _varName, v ) )
	{
		QuoteVar( v.GetValue() );
	}
	else if ( _pSession->SystemVar( _varName, _text ) )
	{
		_temp += _text;
	}
	else
	{
		_temp += CGlobals::DOLLAR_SIGN;
		_temp += _varName;
	}
}

void CReplaceVariables::DoHandleVarName()
{
	if( _varName.empty() )
	{
		_temp += CGlobals::DOLLAR_SIGN;
	}
	else 
	{
		ProcessVar();
	}
}

void CReplaceVariables::DoParseVarName( ParseState &state )
{
	if( ::isalnum( _line[_index] ) )
	{
		_varName += _line[ _index ];
		++_index;
	}
	else
	{
		DoHandleVarName();
		state = ParseStart;
	}
}

void CReplaceVariables::DoParseProc( ParseState &state )
{
	if( CGlobals::PROC_OPEN_PAREN == _line[ _index ] )
	{
		state = ParseProcParams;
		++_procParenCount;
		_procName += _line[ _index ];
	} 
	else if( CGlobals::GetEscapeCharacter() == _line[ _index ] )
	{
		state = ParseProcEscape;
	}
	else if( !::isalnum( _line[_index] ) )   //if it gets here it isn't really a procedure so stop proc processing
	{
		_procName += _line[ _index ];
		_temp += _T("@");
		_temp += _procName;

		state = ParseStart;
	}
	else
	{
		_procName += _line[ _index ];
	}

	++_index;
}

void CReplaceVariables::DoParseProcEscape( ParseState &state )
{
	if( CGlobals::PROC_CLOSE_PAREN == _line[ _index ] )
	{
		_procName += CGlobals::GetEscapeCharacter() ;
	}

	_procName += CGlobals::PROC_CLOSE_PAREN;
	state = ParseProcParams;
	++_index;
}

void CReplaceVariables::ProcessProc()
{
	std::string procReturn;
	if( CProcedureTable::ProcedureVar( _pSession, _procName.c_str(), procReturn ) )
	{
		_temp += procReturn;
	}
	else
	{
		if(_pSession->GetCmdDepth()<15)
		{
			_pSession->SetCmdDepth(_pSession->GetCmdDepth() + 1);
			_temp += _T("@");
		}
		else
            _pSession->SetCmdDepth(0);
		_temp += _procName;
	}
}

void CReplaceVariables::DoParseProcParams( ParseState &state )
{
	_procName += _line[ _index ];
	if( CGlobals::PROC_OPEN_PAREN == _line[ _index ] )
	{
		++_procParenCount;
	}
	else if( CGlobals::PROC_CLOSE_PAREN == _line[ _index ] )
	{
		--_procParenCount;
		if( 0 == _procParenCount )
		{
			ProcessProc();
			state = ParseStart;
		}
	}


	++_index;
}

void CReplaceVariables::execute()
{
	ParseState state = ParseStart;
	try
	{
		if( -1 != _line.Find( CGlobals::DOLLAR_SIGN ) || -1 != _line.Find( CGlobals::GetProcedureCharacter() ) )
		{
			while( _index < _length )
			{
				switch( state )
				{
				case ParseStart:
					DoParseStart( state );
					break;
				case ParseEscape:
					DoParseEscape( state );
					break;
				case ParseVar:
					DoParseVar( state );
					break;
				case ParseVarName:
					DoParseVarName( state );
					break;
				case ParseProc:
					DoParseProc( state );
					break;
				case ParseProcParams:
					DoParseProcParams( state );
					break;
				case ParseProcEscape:
					DoParseProcEscape( state );
					break;
				default:
					ASSERT( _T(!"Invalid ParseState!") );
				}
			}

			switch( state )
			{
			case ParseStart:
				break;
			case ParseVarName:
				DoHandleVarName();
				break;
			default:
				ASSERT( _T(!"HMMMMMM") );
			}

			_line = _temp.c_str();
		}
	}
	catch(...)
	{
		::OutputDebugString(_T("Error executing ReplaceVariables.Execute\n"));
	}

}
















