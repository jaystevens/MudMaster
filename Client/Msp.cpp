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
#include "Msp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMsp::CMsp()
{
	posFind = NULL;
}

CMsp::~CMsp()
{

}

void CMsp::Clear()
{
	m_strPath.Empty();
	m_listWave.RemoveAll();
	m_listMidi.RemoveAll();
}

void CMsp::GatherFiles(LPCSTR pszPath)
{
	m_strPath = pszPath;
	int nLen = m_strPath.GetLength();
	for (int i=0;i<nLen;i++)
		if (m_strPath.GetAt(i) == '\\')
			m_strPath.SetAt(i,'/');
	if (m_strPath.Right(1) != "/")
		m_strPath += '/';
	m_strPath.MakeLower();

	m_listWave.RemoveAll();
	m_listMidi.RemoveAll();

	FindFiles(m_strPath,"",".wav",m_listWave);
	FindFiles(m_strPath,"",".mid",m_listMidi);
}

void CMsp::FindFiles(LPCSTR pszRoot, LPCSTR pszPath, LPCSTR pszFileType, CStringList &list)
{
	CString strFilename;
	CString strSearch(pszRoot);
	strSearch += pszPath;
	strSearch += "*.*";
	strSearch.MakeLower();

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strSearch,&fd);
	BOOL bMore = (hFind != INVALID_HANDLE_VALUE);
	while(bMore)
	{
		strFilename = fd.cFileName;
		strFilename.MakeLower();
		if (strFilename != "." && strFilename != "..")
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CString strPath(pszPath);
				strPath += strFilename;
				strPath += '/';
				FindFiles(pszRoot,strPath,pszFileType,list);
			}
			else
				if (strFilename.GetLength() > 4 && strFilename.Right(4) == pszFileType)
					list.AddTail(CString(pszPath)+strFilename);

		bMore = FindNextFile(hFind,&fd);
	}
	FindClose(hFind);
}

CString CMsp::GetFirstWave()
{
	posFind = m_listWave.GetHeadPosition();
	if (posFind)
		return(m_listWave.GetNext(posFind));
	return("");
}

CString CMsp::GetNextWave()
{
	if (posFind)
		return(m_listWave.GetNext(posFind));
	return("");
}

CString CMsp::GetFirstMidi()
{
	posFind = m_listMidi.GetHeadPosition();
	if (posFind)
		return(m_listMidi.GetNext(posFind));
	return("");
}

CString CMsp::GetNextMidi()
{
	if (posFind)
		return(m_listMidi.GetNext(posFind));
	return("");
}

CString CMsp::MatchWildcard(LPCSTR pszMask, int nMatchType)
{
	// Determine which kind of wildcard search needs to be done.
	if (strchr(pszMask,'*'))
		return(MatchAsterisk(pszMask,(nMatchType == MATCH_MIDI ? m_listMidi : m_listWave)));

	if (strchr(pszMask,'?'))
		return(MatchQuestion(pszMask,(nMatchType == MATCH_MIDI ? m_listMidi : m_listWave)));

	return(pszMask);
}

CString CMsp::MatchAsterisk(LPCSTR pszMask, CStringList &list)
{
	// Separate the mask string into the separate parts on each side of the asterisk.
	CString strMask(pszMask);
	CString strMaskLeft, strMaskRight;
	int nLeftLen, nRightLen;

	int nMaskLen = strMask.GetLength();

	// Special cases are the asterisk at head or tail of the buffer.
	if (strMask[0] == '*')
	{
		// Left side is empty.
		nLeftLen = 0;
		strMaskRight = strMask.Right(nMaskLen-1);
		nRightLen = strMaskRight.GetLength();
	}
	else
		if (strMask[nMaskLen-1] == '*')
		{
			// Right side is empty.
			nRightLen = 0;
			strMaskLeft = strMask.Left(nMaskLen-1);
			nLeftLen = strMaskLeft.GetLength();
		}
		else
		{
			int nIndex = strMask.Find('*');
			strMaskLeft = strMask.Left(nIndex);
			strMaskRight = strMask.Right(nMaskLen-nIndex-1);
			nLeftLen = strMaskLeft.GetLength();
			nRightLen = strMaskRight.GetLength();
		}

	int nMatchCount = 0;

	CString strFile;
	POSITION pos, posCurrent;
	for (pos=list.GetHeadPosition();pos!=NULL;)
	{
		posCurrent = pos;
		strFile = list.GetNext(pos);

		// Depending on the wildcard match, compare the strings.
		if (!nLeftLen)
		{
			if (strFile.GetLength() >= nRightLen && strFile.Right(nRightLen) == strMaskRight)
			{
				m_posWild[nMatchCount] = posCurrent;
				nMatchCount++;
			}
		}
		else
			if (!nRightLen)
			{
				if (strFile.GetLength() >= nLeftLen && strFile.Left(nLeftLen) == strMaskLeft)
				{
					m_posWild[nMatchCount] = posCurrent;
					nMatchCount++;
				}
			}
			else
			{
				if (strFile.GetLength() >= nLeftLen + nRightLen && strFile.Left(nLeftLen) == strMaskLeft && strFile.Right(nRightLen) == strMaskRight)
				{
					m_posWild[nMatchCount] = posCurrent;
					nMatchCount++;
				}
			}

		if (nMatchCount == MAX_WILDCARD_MATCHES)
			break;
	}

	// No matches return an empty string.
	if (!nMatchCount)
		return("");

	// Return a random match.
	return(list.GetAt(m_posWild[rand()%nMatchCount]));
}

CString CMsp::MatchQuestion(LPCSTR pszMask, CStringList &list)
{
	int nMatchCount = 0;

	// Find all the strings that are of equal length, then compare them based on
	// the question marks.
	int nIndex;
	int nMaskLen = strlen(pszMask);
	CString strFile;
	POSITION pos, posCurrent;
	for(pos=list.GetHeadPosition();pos != NULL;)
	{
		posCurrent = pos;
		strFile = list.GetNext(pos);
		if (nMaskLen == strFile.GetLength())
		{
			nIndex = 0;
			while(nIndex < nMaskLen)
			{
				if (pszMask[nIndex] != '?' && pszMask[nIndex] != strFile[nIndex])
					break;
				nIndex++;
			}

			// If made it all the way thru then they match.
			if (nIndex == nMaskLen)
			{
				m_posWild[nMatchCount] = posCurrent;
				nMatchCount++;
				if (nMatchCount == MAX_WILDCARD_MATCHES)
					break;
			}
		}
	}

	// No matches return an empty string.
	if (!nMatchCount)
		return("");

	// Return a random match.
	return(list.GetAt(m_posWild[rand()%nMatchCount]));
}

