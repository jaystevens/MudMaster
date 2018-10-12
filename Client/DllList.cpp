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
#include "DebugStack.h"
#include "DllList.h"
#include "Notifications.h"
#include "Globals.h"
#include "Messages.h"
#include "Sess.h"
#include "SeException.h"
#include "GlobalVariables.h"
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "colors.h"
#include "ErrorHandling.h"
#include "StringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace Utilities;

namespace MMSystem
{
	class SameHMOD
	{
	public:
		SameHMOD( HMODULE h )
			: _h(h)
		{
		}

		bool operator() (CDll::PtrType dll)
		{
			return dll->matchesHMOD(_h);
		}

	private:
		HMODULE _h;
	};

	class WriteToFileFunc
	{
	public:
		WriteToFileFunc(CStdioFile &file)
			: _file(file)
			, _count(0)
		{
		}

		void operator()(CDll::PtrType dll)
		{
			std::stringstream buffer;
			buffer
				<< CGlobals::GetCommandCharacter()
				<< "loadlibrary {"
				<< dll->name()
				<< "} {"
				<< dll->Path()
				<< "}"
				<< std::endl;

			_file.WriteString(buffer.str().c_str());
			++_count;
		}

		int count() { return _count; }

	private:
		CStdioFile &_file;
		int _count;
	};

	char CDllList::m_pszDLLResult[CDll::DLL_RESULT_LEN+1];

	CDllList::CDllList()
	{
	}

	CDllList::~CDllList()
	{
		RemoveAll(NULL);
	}

	void CDllList::RemoveAll(CSession *pSession)
	{
		CDll::PtrType dll;
		ListIter it = _list.begin();
		while ( it != _list.end() )
		{
			dll = *it;
			freeDll(pSession, dll);
//			_list.erase( it );
			it++;
		}

		_list.clear();
		ListType().swap(_list);
	}
	void CDllList::CallAll(CSession *pSession, 
		const CString &func, 
		const CString &params)
	{
		CDll::PtrType dll;
		ListIter it = _list.begin();
		while ( it != _list.end() )
		{
			dll = *it;
			CDllList::call(pSession,dll->name(),func,params);
			it++;
		}
	}

	int CDllList::write(CStdioFile &file)
	{
		WriteToFileFunc f = std::for_each( _list.begin(), _list.end(), WriteToFileFunc(file) );
		return f.count();
	}

	bool CDllList::add(CSession *pSession, const CString &name, const CString &path, const CString &params)
	{
		bool result = false;

		CString fixedName(name);
		fixedName.Trim();			

		IterPair res = std::equal_range(_list.begin(), _list.end(), fixedName);
		if(res.first == res.second)
		{
			CDll::PtrType dll(new CDll(fixedName, path));
			HRESULT hr = dll->Load();
			if(SUCCEEDED(hr))
			{
				_list.insert(res.first, dll);

				// Call OnLoad function if it exists.
				CDll::MYPROC proc = NULL;
				if(dll->getProcAddress("onLoad", proc)) {
					GlobalVariables::g_pCommandHandler = pSession;
					*CDllList::m_pszDLLResult = '\x0';
					proc(params, CDllList::m_pszDLLResult);

				}				 
				result = true;
			}
		}

		return result;
	}

	bool CDllList::remove(CSession *pSession, const CString &index)
	{
		bool result = false;

		CDll::PtrType dll;
		if(getByIndex(index, dll))
		{				
			ListIter it = std::remove( _list.begin(), _list.end(), dll );
			freeDll(pSession, dll);
			_list.erase(it);
			result = true;			
		}

		return result;
	}

	void CDllList::freeDll(CSession *pSession, CDll::PtrType &dll)
	{
		// Call a destructor function if it exists
		// Call OnLoad function if it exists.
		CDll::MYPROC proc = NULL;
		if(pSession != NULL && dll->getProcAddress("onUnload", proc)) {
			GlobalVariables::g_pCommandHandler = pSession;
			*CDllList::m_pszDLLResult = '\x0';
			proc("", CDllList::m_pszDLLResult);
		}

		dll->CDll::Free();
	}

	bool CDllList::getByIndex(const CString &index, CDll::PtrType &dll)
	{
		bool result = false;

		try
		{
			int n = boost::lexical_cast<int>(index);
			if( isValidIndex(n) )
			{
				dll = _list[n - 1];
				result = true;
			}
		}
		catch( boost::bad_lexical_cast ble )
		{
		}

		if(false == result)
		{
			IterPair res = std::equal_range( _list.begin(), _list.end(), index );
			if( res.first != res.second )
			{
				dll = *res.first;
				result = true;
			}
		}

		return result;
	}

	void CDllList::print( CSession *pSession )
	{
		std::stringstream buf;
		if(_list.empty())
		{
			buf << "  ==================================\n"
				<< "  There are currently no DLLs loaded\n"
				<< "  ----------------------------------\n";
			pSession->PrintAnsiNoProcess(buf.str().c_str());
		}
		else
		{
			buf << "     DLL Name             DLL Version Information\n"
				<< "     ==================== ==================================================\n";

			pSession->PrintAnsiNoProcess(buf.str().c_str());
			ListIter it = _list.begin();
			for(;it != _list.end();++it)
			{
				CDll::PtrType dll = *it;

				buf.str("");
				buf << ANSI_F_BOLDWHITE
					<< setiosflags(ios::right)
					<< std::setw(3) << std::distance(_list.begin(), it) + 1
					<< resetiosflags(ios::right)
					<< ":"
					<< ANSI_RESET
					<< " "
					<< setiosflags(ios::left) << std::setw(20) << dll->name()
					<< " "
					<< setiosflags(ios::left) << std::setw(50) << dll->Version()
					<< "\n";

				pSession->PrintAnsiNoProcess(buf.str().c_str());
			}

			buf.str("");
			buf << "     -----------------------------------------------------------------------\n"
				<< "     (" << _list.size() << ") "
				<< ((_list.size() > 1) ? "dlls" : "dll")
				<< " currently loaded\n\n";

			pSession->PrintAnsiNoProcess(buf.str().c_str());
		}
	}

	extern "C" void DllCallback(LPCSTR pszText)
	{
		CString strText    = pszText;
		CString strCommand = pszText;
		GlobalVariables::g_pCommandHandler->DebugStack().Push(0, STACK_TYPE_DLL_CALLBACK, strCommand);
		GlobalVariables::g_pCommandHandler->ExecuteCommand( strText );
		GlobalVariables::g_pCommandHandler->DebugStack().Pop();
	}

	bool CDllList::getByHMOD( HMODULE hMod, CDll::PtrType &dll )
	{
		bool result = false;
		
		ListIter it = std::find_if( _list.begin(), _list.end(), SameHMOD(hMod) );
		if( it != _list.end() )
		{
			dll = *it;
			result = true;
		}

		return result;
	}

	bool CDllList::call( 
		CSession *pSession, 
		const CString &lib, 
		const CString &func, 
		const CString &params)
	{
		bool result = false;

		CDll::PtrType dll;
		if( getByIndex(lib, dll) )
		{
			// Make it a 0 length string so the proc called can just leave
			// it blank if it wants to.
			GlobalVariables::g_pCommandHandler = pSession;
			*CDllList::m_pszDLLResult = '\x0';
			CDll::MYPROC proc = NULL;
			if( dll->getProcAddress( func, proc ) )
			{
				proc(params, CDllList::m_pszDLLResult);

				if (strlen(CDllList::m_pszDLLResult))
				{
					CString strTemp(CDllList::m_pszDLLResult);
					pSession->DebugStack().Push(0,STACK_TYPE_DLL_RETURN, strTemp);
					pSession->ExecuteCommand( strTemp );

					pSession->DebugStack().Pop();
				}

				result = true;
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format( _T("DLL [%s] not found."), lib );
			pSession->QueueMessage(CMessages::MM_DLL_MESSAGE, strMessage);
		}

		return result;
	}

	bool CDllList::isValidIndex( unsigned int index )
	{
		return index > 0 && index <= _list.size();
	}
}