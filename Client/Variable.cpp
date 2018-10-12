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
#include "Variable.h"
#include "Globals.h"
#include "Sess.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace MMScript
{
	CVariable::CVariable()
		: _type( 0 )
	{
	}

	CVariable::CVariable(
		const std::string &name,
		const std::string &value,
		const std::string &group,
		int type)
		: _name( name )
		, _value( value )
		, _group( group )
		, _type(type)
	{
	}

	CVariable::CVariable(const CVariable& src) 
		: _name( src._name )
		, _value( src._value )
		, _group( src._group )
		, _type( src._type )
	{
	}

	CVariable::~CVariable()
	{
	}

	CVariable& CVariable::operator =(const CVariable& src)
	{
		CVariable temp( src );

		Swap( temp );

		return *this;
	}

	std::string CVariable::MapToText( bool includeGroup )
	{
		std::stringstream buffer;
		buffer
			<< CGlobals::GetBlockStartCharacter()
			<< GetName()
			<< CGlobals::GetBlockEndCharacter()
			<< " "
			<< CGlobals::GetBlockStartCharacter()
			<< GetValue()
			<< CGlobals::GetBlockEndCharacter();

		if( includeGroup )
		{
			buffer
				<< " "
				<< CGlobals::GetBlockStartCharacter()
				<< GetGroup()
				<< CGlobals::GetBlockEndCharacter();
		}

		return buffer.str();
	}

	void CVariable::MapToText(CString &strText, BOOL bIncludeGroup)
	{
		if(bIncludeGroup)
		{
			if ( GetValue().empty() )
			{
				strText.Format( 
					"{%s} {} {%s}", 
					GetName().c_str(), 
					GetGroup().c_str() );
			}
			else
			{
				CString strPrepare;
				CGlobals::PrepareForWrite( GetValue().c_str(), strPrepare );
				strText.Format(
					"{%s} {%s} {%s}", 
					GetName().c_str(), 
					strPrepare, 
					GetGroup().c_str() );
			}
		} 
		else
		{
			if ( GetValue().empty() )
			{
				strText.Format( "{%s}", GetName().c_str() );
			}
			else
			{
				CString strPrepare;
				CGlobals::PrepareForWrite( GetValue().c_str(), strPrepare );
				strText.Format( "{%s} {%s}", GetName().c_str(), strPrepare );
			}
		}
	}

	std::string CVariable::MapToCommand()
	{
		std::stringstream buffer;

		buffer 
			<< CGlobals::GetCommandCharacter()
			<< ( GetValue().empty() ? "empty" : "variable" )
			<< MapToText( true );

		return buffer.str();
	}

	void CVariable::MapToCommand(CString &strCommand)
	{
		CString strFormat;
		if( GetValue().empty() )
			strFormat = "%cempty %s";
		else
			strFormat = "%cvariable %s";

		CString strText;
		MapToText(strText, TRUE);

		strCommand.Format(strFormat, CGlobals::GetCommandCharacter(), strText);
	}

	const std::string &CVariable::GetName() const
	{
		return _name;
	}

	const std::string &CVariable::GetValue() const
	{
		return _value;
	}

	int CVariable::GetType() const
	{
		return _type;
	}

	void CVariable::SetName( const std::string &name )
	{
		_name = name;
	}

	void CVariable::SetValue( const std::string &value )
	{
		_value = value;
	}

	void CVariable::SetType( int type )
	{
		_type = type; 
	}

	void CVariable::SetGroup( const std::string &groupName )
	{
		_group = groupName;
	}

	const std::string &CVariable::GetGroup() const
	{
		return _group;
	}

	void CVariable::Swap( CVariable &src )
	{
		std::swap( _type, src._type );
		std::swap( _name, src._name );
		std::swap( _value, src._value );
		std::swap( _group, src._group );
	}
}