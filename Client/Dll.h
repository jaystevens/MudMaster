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

class CSession;
#include <boost/shared_ptr.hpp>

namespace MMSystem
{
	class CDll
	{
	public:
		typedef boost::shared_ptr<CDll> PtrType;
		typedef void (*MYPROC)(LPCSTR pszParams, LPSTR pszResult);

		enum Constants
		{
			DLL_RESULT_LEN	= 5000
		};

		CDll();
		CDll(const CString &name, const CString &path);
		CDll(const CDll& src);

		CDll &operator=(const CDll &src);

		bool operator==(LPCTSTR pszName);
		bool operator==(HMODULE hModule);
		bool operator<(const CDll &dll);

		bool getProcAddress(const CString &func, MYPROC &proc);
		bool matchesHMOD( HMODULE hMod ) { return _h == hMod; }		

		virtual ~CDll();

		HRESULT Load();
		HRESULT Free();

		const CString &name()	 const	{return _name;}
		const CString &Version() const	{return _version;}
		const CString &Path()	 const  {return _path;}

	private:		
		void Swap(CDll &src);

		CString	_name;				// DLL name
		CString _path;				// path to the dll
		CString	_version;			// Version information
		HMODULE		_h;					// Instance handle
	};
}

inline bool operator<( const MMSystem::CDll::PtrType &lhs, const MMSystem::CDll::PtrType &rhs )
{
	return lhs->name().Compare(rhs->name()) < 0;
}

inline bool operator<(const MMSystem::CDll::PtrType dll, const CString &name)
{
	return dll->name().Compare(name) < 0;
}

inline bool operator<(const CString &name, const MMSystem::CDll::PtrType dll)
{
	return name.Compare(dll->name()) < 0;
}
