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

#include <boost/shared_ptr.hpp>
#include <string>

namespace MMScript
{
	class CAlias
	{
	public:
		typedef boost::shared_ptr<CAlias> PtrType;

		static bool CreateAlias(
			const CString &name, 
			const CString &action, 
			const CString &group,
			PtrType &alias);

		CString DisplayName() const;
		std::string Command(char commandChar) const;
		CString Text(bool includeGroup) const;

		//ACCESSORS
		bool enabled() const				{return _enabled;}
		bool hasVar() const					{return _hasVar;}
		int  varNum() const					{return _varNum;}
		const CString &action() const	{return _action;}
		const std::string &group() const	{return _group;}
		const CString &name() const		{return _name;}

		//MUTATORS
		void enabled(bool enabled)				{_enabled = enabled;}
		void hasVar(bool hasVar)				{_hasVar = hasVar;}
		void varNum(int varNum)					{_varNum = varNum;}
		void action(const CString &action)	{_action = action;}
		void group(const CString &group)	{_group = group;}
		void name(const CString &name)		{_name = name;}
//TODO: KW gets based on Alias pointer
		const CString &CAlias::GetAction()	const;
		const std::string &CAlias::GetGroup() const;
		const CString &CAlias::GetName() const;
		const bool &CAlias::IsEnabled() const;
		

	private:
		CAlias();
		CAlias(
			const CString &name, 
			const CString &action, 
			const CString &group, 
			bool hasVar, 
			int varNum);

		CAlias(const CAlias& src);
		CAlias& operator=(const CAlias& src);

		void Swap(CAlias &other);

		bool	_enabled;
		bool	_hasVar;		// TRUE if %0-%9 is part of the alias.
		int		_varNum;		// The replaceable var num.
		CString _action;	// The text to replace the alias.
		std::string _group;
		CString _name;		// The alias.
	};

	inline bool operator==(const CAlias::PtrType alias, const CString &name)
	{
		return alias->name() == name;
	}

	inline bool operator<(const CAlias::PtrType alias, const CString &name)
	{
		return alias->name() < name;
	}

	inline bool operator<(const CString &name, const CAlias::PtrType alias)
	{
		return name < alias->name();
	}
}
