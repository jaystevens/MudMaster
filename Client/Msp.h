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

// This is the maximum number of wildcard matches to find before
// selecting a random string.
#define MAX_WILDCARD_MATCHES		100

// Defines used to pass into the MatchWildcard function.
#define MATCH_MIDI					0
#define MATCH_WAVE					1

class CMsp
{
public:
	CMsp();
	~CMsp();

	// Pass in a path it will gather all the files that exist in the directory
	// and any sub directories.
	void GatherFiles(LPCSTR pszPath);

	// Clears out all the file into and MSP path.
	void Clear();

	// Pass in a string with wildcards, get back a file matching the wildcard.
	// If more than one file matches the wildcard, a random file is picked
	// from the matches.  The string can contain one *, OR (not both) a number
	// of questions marks.
	CString MatchWildcard(LPCSTR pszMask, int nMatchType);

	CString GetPath()				{ return(m_strPath); }

	CString GetFirstWave();
	CString GetNextWave();

	CString GetFirstMidi();
	CString GetNextMidi();

private:
	// Recursive funtion that reads all the files in the directories.
	// Root is the root path to build from, the root path is not included in
	// the filepath saved int he list.
	void FindFiles(LPCSTR pszRoot, LPCSTR pszPath, LPCSTR pszFileType, CStringList &list);

	CString MatchAsterisk(LPCSTR pszMask, CStringList &list);
	CString MatchQuestion(LPCSTR pszMask, CStringList &list);

	// This is the root path to the MSP files.
	CString m_strPath;

	// List of all the files found in the MSP directories.
	CStringList	m_listWave;
	CStringList m_listMidi;

	// Holds the resulting positions from the lists for picking a random match.
	POSITION m_posWild[MAX_WILDCARD_MATCHES];

	POSITION posFind;
};
