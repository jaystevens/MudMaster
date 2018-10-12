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
#include "Alias.h"
#include <sstream>

namespace MMScript
{
	bool CAlias::CreateAlias(
		const CString &name, 
		const CString &action, 
		const CString &group, 
		PtrType &alias )
	{
		// Empty strings are not valid.
		if(name.IsEmpty())
			throw std::invalid_argument("an alias must have a name");

		if(action.IsEmpty())
			throw std::invalid_argument("an alias must have an action");

		// Aliases cannot start with a variable.
		if (name[0] == '%')
			throw std::invalid_argument("an alias cannot begin with a variable");

		bool hasVariable = false;
		CString fixedName(name);
		int varNum = 0;

		int n = name.Find('%');
		if(n != -1)
		{
			if(n == name.GetLength() - 1)
				throw std::invalid_argument("an alias cannot end with a % sign");

			hasVariable = true;
			fixedName = name.Left(n-1);
			varNum = name[n + 1] - '0';
		}

		alias.reset( new CAlias(fixedName, action, group, hasVariable, varNum) );

		return true;
	}

	CAlias::CAlias( 
		const CString &name,
		const CString &action,
		const CString &group,
		bool hasVariable,
		int varNum)
		: _enabled(true)
		, _hasVar(hasVariable)
		, _varNum(varNum)
		, _action(action)
		, _group(group)
		, _name(name)
	{
	}

	CAlias::CAlias(const CAlias& src)
		: _enabled(src._enabled)
		, _hasVar(src._hasVar)
		, _varNum(src._varNum)
		, _action(src._action)
		, _group(src._group)
		, _name(src._name)
	{
	}

	CAlias& CAlias::operator =(const CAlias& src)
	{
		CAlias temp(src);
		Swap(temp);
		return *this;
	}

	CString CAlias::Text(bool includeGroup) const
	{
		std::stringstream buffer;
		buffer
			<< "{" << name();

		if(hasVar())
			buffer << " %" << varNum();

		buffer << "} {" << action() << "}";

		if (includeGroup)
		{
			buffer << "{" << group() << "}";
		}

		CString ret(buffer.str().c_str());
		return ret;
	}

	const CString &CAlias::GetAction()	const
		{
			return _action;
		}


		const std::string &CAlias::GetGroup() const
		{
		return _group;
		}

	const CString &CAlias::GetName() const
		{
			return _name ;
		}
	const bool &CAlias::IsEnabled() const
		{
			return _enabled;
		}


	std::string CAlias::Command(char commandChar) const
	{
		std::stringstream buffer;

		buffer 
			<< commandChar 
			<< "alias " 
			<< Text(true);

		return buffer.str();
	}

	CString CAlias::DisplayName() const
	{
		std::stringstream buffer;

		buffer << name();

		if(hasVar())
			buffer << " %" << varNum();

		return buffer.str().c_str();
	}

	void CAlias::Swap(CAlias &other)
	{
		std::swap(_enabled, other._enabled);
		std::swap(_hasVar, other._hasVar);
		std::swap(_varNum, other._varNum);
		std::swap(_action, other._action);
		std::swap(_group, other._group);
		std::swap(_name, other._name);
	}
}