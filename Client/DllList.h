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
#pragma warning(disable: 4267)

#include <boost/shared_ptr.hpp>
#include "Dll.h"

class CSession;

namespace MMSystem
{
	class CDllList
	{
	public:
		typedef std::vector<CDll::PtrType> ListType;
		typedef ListType::iterator ListIter;
		typedef std::pair<ListIter, ListIter> IterPair;

		CDllList();
		~CDllList();

		bool call(CSession *pSession, const CString &lib,const CString &func, const CString &params);
		void CallAll(CSession *pSession, const CString &func, const CString &params);
		bool add(CSession *pSession, const CString &name, const CString &path, const CString &params);
		bool getByIndex(const CString &index, CDll::PtrType &dll);
		bool getByHMOD(HMODULE hMod, CDll::PtrType &dll);
		bool remove(CSession *pSession, const CString &dll);
		void RemoveAll(CSession *pSession);
		int write(CStdioFile &file);
		void print(CSession *pSession);
		unsigned int count() { return _list.size(); }

	private:
		void freeDll(CSession *pSession, CDll::PtrType &dll);
		bool isValidIndex( unsigned int index );
		ListType _list;

	public:
		static char	m_pszDLLResult[CDll::DLL_RESULT_LEN+1];
	};
}