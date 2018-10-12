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

#define ARRAY_ELEMENTS(ptr) ((ptr->SingleDim()) ? (ptr->Dim(0)) : (ptr->Dim(0)*ptr->Dim(1)))
#define ARRAY_INDEX_VALID(ptr,row,col) ((ptr->SingleDim()) ? (row>0&&row<=ptr->Dim(0)) : (row>0&&row<=ptr->Dim(0)&&col>0&&col<=ptr->Dim(1)))
#define ARRAY_INDEX(ptr,row,col) ((ptr->SingleDim()) ? (row-1) : ((row-1)*ptr->Dim(1)+(col-1)))

#include "ScriptEntity.h"

class CSession;

namespace MMScript
{
	class CMMArray : public CScriptEntity
	{
	public:
		void MapToDisplayText(CString &strText, CSession *pSession);
		void MapToText(CString &strText, BOOL bIncludeGrou);
		void MapToCommand(CString &strCommand);

		BOOL CreateArray(int nRows, int nCols);

		CMMArray();
		virtual ~CMMArray();

		BOOL GetValue(int nRow, int nCol, CString& strValue);
		BOOL SetValue(int nRow, int nCol, const CString& strValue);

		//ACCESSORS
		BOOL SingleDim()				{return m_bSingleDim;}
		int Dim(int nElement)			{return m_nDim[nElement];}
		const CString& Group() const	{return m_strGroup;}
		CString& Name()					{return m_strName;}

		//MUTATORS
		void SingleDim(BOOL bSingleDim)		{m_bSingleDim = bSingleDim;}
		void Dim(int nElement, int nValue)	{m_nDim[nElement] = nValue;}
		void Group(const CString& strGroup)	{m_strGroup = strGroup;}
		void Name(const CString& strName)	{m_strName = strName;}

	private:
		BOOL		m_bSingleDim;		// TRUE when single dimensional.
		int			m_nDim[2];			// Dimensions of the array.
		int			m_nUpperBound[2];	// Index of the highest OCCUPIED index of the array.
		CObArray	m_Array;
		CString		m_strGroup;			// Name of the group.
		CString		m_strName;			// Name of the array.
	};
}