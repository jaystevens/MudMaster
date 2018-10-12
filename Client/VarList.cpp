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
#include "Globals.h"
#include "VarList.h"
#include "Variable.h"
#include "ErrorHandling.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utilities;

namespace MMScript
{
	CVarList::CVarList()
	{
		SetCompareFunction(Compare);
	}

	CVarList::~CVarList()
	{
	}

	int CVarList::AddToList(CVariable &add)
	{
		return OrderedInsert(add);
	}

	int CVarList::Add(
		const std::string& name, 
		const std::string& value, 
		const std::string& group, 
		VariableType type /* = VariableTypeString */ )
	{
		CVariable var( name, value, group, type );
		int nIndex = Find(var);
		if(-1 == nIndex)
		{
			return AddToList(var);
		}
		else
		{
			CVariable &v = GetAt(nIndex);
			v = var;
			return nIndex;
		}
	}

	BOOL CVarList::RemoveByName(const std::string& name)
	{
		CVariable v( name, _T(""), _T(""), 0 );
		int nIndex = Find(v);
		if(-1 == nIndex)
		{
			return FALSE;
		}

		RemoveAt(nIndex);
		return TRUE;
	}


	BOOL CVarList::FindByName( const std::string &name, CVariable &var )
	{
		CVariable v( name, _T(""), _T(""), VariableTypeString );
		int nIndex = Find(v);
		if(-1 == nIndex)
		{
			return FALSE;
		}

		var = GetAt(nIndex);
		return TRUE;
	}

	size_t CVarList::RemoveGroup( const std::string &groupName )
	{
		std::vector<std::string> to_delete;

		for(int i = 0; i < GetSize(); ++i)
		{
			CVariable &v = GetAt(i);
			if( v.GetGroup() == groupName )
			{
				to_delete.push_back( v.GetName() );
			}
		}

		std::vector<std::string>::iterator it = to_delete.begin();
		while(it != to_delete.end())
		{
			RemoveByName(*it);
			++it;
		}

		return to_delete.size();
	}

	int CVarList::Compare( const CVariable &v1, const CVariable &v2 )
	{
		//CString n1(v1.Name());
		//n1.MakeLower();
		//CString n2(v2.Name());
		//n2.MakeLower();
		//return n1.Compare(n2);
		return v1.GetName().compare( v2.GetName() );
	}

	HRESULT CVarList::SendGroup(
		MMChatServer::ScriptCommandType cmd, 
		const std::string &groupName, 
		const std::string &personName, 
		MMChatServer::CChatServer &server,
		int &nSent)
	{
		HRESULT hr = E_UNEXPECTED;
		try
		{
			for(int i = 0; i < GetSize(); ++i)
			{
				CVariable v = GetAt(i);
				if ( v.GetGroup() == groupName )
				{
					ErrorHandlingUtils::TESTHR(server.SendCommand(
						static_cast<BYTE>(cmd),
						personName,
						v.MapToCommand() ) );

					++nSent;
				}
			}

			hr = S_OK;
		}
		catch(_com_error)
		{
			::OutputDebugString("Error in SendGroup\n");
		}

		return hr;
	}

}