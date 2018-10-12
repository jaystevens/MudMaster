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
// MusicSegment.h: interface for the CMusicSegment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUSICSEGMENT_H__5E6A152B_1A02_44C5_AE14_509F66139FB0__INCLUDED_)
#define AFX_MUSICSEGMENT_H__5E6A152B_1A02_44C5_AE14_509F66139FB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* Deprecated in 2007 
#ifndef DEPRECATE_SOUND
#endif
*/
#include <dmusici.h>
//#ifndef MM2K6_NODIRECTSOUND
//
class CMusicSegment  
{
public:
	CMusicSegment(IDirectMusicPerformance8* pPerformance, IDirectMusicLoader8* pLoader, IDirectMusicSegment8* pSegment);
	virtual ~CMusicSegment();

	HRESULT Download( IDirectMusicAudioPath8* pAudioPath = NULL );
	HRESULT Unload( IDirectMusicAudioPath8* pAudioPath = NULL );
	HRESULT Play( DWORD dwFlags = DMUS_SEGF_BEAT, IDirectMusicAudioPath8* pAudioPath = NULL );
	HRESULT SetRepeats( DWORD dwRepeats );
	BOOL IsPlaying();
	HRESULT Stop( DWORD dwFlags = DMUS_SEGF_BEAT);

protected:
	IDirectMusicSegment8 *m_pSegment;
	IDirectMusicLoader8 *m_pLoader;
	IDirectMusicPerformance8 *m_pPerformance;
	IDirectMusicAudioPath8 *m_pEmbeddedAudioPath;
	BOOL m_bDownloaded;

};

//#endif /* MM2K6_NODIRECTSOUND */
#endif // !defined(AFX_MUSICSEGMENT_H__5E6A152B_1A02_44C5_AE14_509F66139FB0__INCLUDED_)
