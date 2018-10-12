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

namespace MMScript
{
	class CScriptEntity;

	class CScriptEntityList
	{
		static const int ARRAY_SIZE = 50;
		static const int GROW_SIZE = 5;
	public:
		CScriptEntityList();
		virtual ~CScriptEntityList();

		CScriptEntity *GetFirst() const;
		CScriptEntity *GetNext() const;
		CScriptEntity *GetAt(int nIndex) const;
		CScriptEntity *FindExact(const CString& strSearch) const;

		virtual CScriptEntity *FindExactImpl(const CString& strSearch) const = 0;
		virtual CScriptEntity *AddToList(CScriptEntity *pEntity) = 0;
		virtual BOOL RemoveImpl(const CString& strSearch) = 0;

		virtual BOOL Remove(int nIndex);
		virtual BOOL Remove(CScriptEntity *pEntity);
		virtual BOOL Remove(const CString& strSearch);

		virtual BOOL RemoveGroup(const CString& strGroup);

		BOOL AddScriptEntity(CScriptEntity *pEntity, int nIndex);

		void RemoveAll();

		int GetCount() const		{return m_nCount;}
		int GetFindIndex() const	{return m_nGetIndex;}

	protected:
		CObArray m_list;
		int m_nCount;
		mutable int m_nGetIndex;
	};
}