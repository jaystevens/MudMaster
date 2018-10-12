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
#pragma	once

#define DEFAULT_VAR_ARRAY_SIZE	150
#define DEFAULT_VAR_GROW_SIZE		20

//#include "ScriptEntityList.h"
#include "SortableArray.h"
#include "Variable.h"
#include "ChatServer.h"

namespace MMScript
{
	class CVariable;

	class CVarList 
		: public MMCollections::CSortedArray<CVariable>
	{
	public:
		enum VariableType
		{
			VariableTypeString = 0,
			VariableTypeNumber = 1,
		};

		CVarList();
		~CVarList();

		int Add(
			const std::string &name, 
			const std::string &value, 
			const std::string &group, 
			VariableType type = VariableTypeString );

		BOOL FindByName( const std::string &name, CVariable &var );
		int AddToList( CVariable &add );
		BOOL RemoveByName( const std::string &name );
		size_t RemoveGroup( const std::string &groupName );

		HRESULT SendGroup(
			MMChatServer::ScriptCommandType cmd, 
			const std::string &group, 
			const std::string &personName, 
			MMChatServer::CChatServer &server,
			int &nSent);

		static int Compare( const CVariable &v1, const CVariable &v2 );
	};
}