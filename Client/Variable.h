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

#include "IGroupable.h"

namespace MMScript
{
	class CVariable
		: public IGroupable
	{
	public:
		void MapToCommand(CString &strCommand);
		std::string MapToCommand();

		void MapToText(CString &strText, BOOL bIncludeGroup);
		std::string MapToText( bool includeGroup );

		CVariable();
		CVariable(
			const std::string &name,
			const std::string &value,
			const std::string &group,
			int type);

		CVariable(const CVariable& src);
		virtual ~CVariable();
		CVariable& operator=(const CVariable& src);

		const std::string& GetName() const;
		const std::string& GetValue() const;
		int GetType() const;

		void SetName( const std::string &name );
		void SetValue( const std::string &value );
		void SetType( int type );

		// IGroupable
		virtual void SetGroup( const std::string &groupName );
		virtual const std::string &GetGroup() const;

	private:
		void Swap( CVariable &src );

		std::string	_name;		// User define name of the var.
		std::string _value;		// Value of the var.
		std::string _group;		// Group the var belongs to.
		int			_type;		// Type of var.
	};
}
