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
// SoundManager.h: interface for the CSoundManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDMANAGER_H__D82B6364_80F2_410E_BAA1_C89458EA270F__INCLUDED_)
#define AFX_SOUNDMANAGER_H__D82B6364_80F2_410E_BAA1_C89458EA270F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dmusicc.h>
#include <dsound.h>

#ifndef MM2K6_NODIRECTSOUND

typedef char IDirectMusicAudioPath8;

//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CMusicManager;
class CMusicSegment;
class CMusicScript;

class CSoundManager  
{
public:
	CSoundManager();
	virtual ~CSoundManager();

	
	inline IDirectMusicLoader8			*GetLoader()			{return m_pLoader;		}
	inline IDirectMusicPerformance8	*GetPerformance()		{return m_pPerformance;	}

	IDirectMusicAudioPath8		*GetDefaultAudioPath();

	HRESULT Initialize(HWND hWnd, DWORD dwPChannels = 128, DWORD dwDefaultPathType = DMUS_APATH_SHARED_STEREOPLUSREVERB);
	HRESULT SetSearchDirectory( LPCTSTR strMediaPath );
	VOID CollectGarbage();
	HRESULT CreateSegmentFromFile(CMusicSegment **ppSegment, LPCTSTR strFileName, BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE);
	HRESULT CreateScriptFromFile(CMusicScript **ppScript, TCHAR *strFileName);
	HRESULT CreateChordMapFromFile(IDirectMusicChordMap8 **ppChordMap, TCHAR *strFileName);
	HRESULT CreateStyleFromFile(IDirectMusicStyle8 **ppStyle, TCHAR *strFileName);
	HRESULT GetMotifFromStyle(IDirectMusicSegment8 **ppMotif8, TCHAR *strStyle, TCHAR *strMotif);

	HRESULT CreateSegmentFromResource(CMusicSegment **ppSegment, TCHAR *strResource, TCHAR *strResourceType, BOOL bDownloadNow = TRUE, BOOL bIsMidiFile = FALSE);
private:
	IDirectMusicLoader8			*m_pLoader;
	IDirectMusicPerformance8	*m_pPerformance;
};
#endif /* MM2K6_NODIRECTSOUND */
#endif // !defined(AFX_SOUNDMANAGER_H__D82B6364_80F2_410E_BAA1_C89458EA270F__INCLUDED_)
