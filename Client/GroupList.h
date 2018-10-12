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
	class CAction;
	class CAlias;
	class CGag;
	class CHighlight;
	class CSub;
	class CMMArray;
	class CVariable;
	class CMacro;
	class CEvent;
	class CUserList;
	class CScriptEntity;
}

class CGroup;

class CGroupList 
{
	static const int DEFAULT_GROUP_ARRAY_SIZE	= 150;
	static const int DEFAULT_GROUP_GROW_SIZE	= 20;
public:

	// Create a new group and add it to the list
	CGroup *Add(const CString& strGroup);

	// Add a script entity to its appropriate group.
	// If the group doesn't exist, it will be created and added to the group list.
	CGroup *Add(const CString& strGroup, MMScript::CScriptEntity *pEntity);

	// Given a group name, find the group
	CGroup *FindByName(const CString& strGroup);

	// Remove a group from the group list
	BOOL Remove(CGroup *pGroup);
	BOOL Remove(int nIndex);

	// Get a specific group indexed by its element index
	CGroup *GetAt(int nIndex);

	// Iteration 
	CGroup *GetNext();

	// Iteration
	CGroup *GetFirst();

	// Clear the list
	void RemoveAll();

	CGroupList();
	virtual ~CGroupList();

private:
	CObArray m_list;
	int m_nCount;
	int m_nGetIndex;
};
