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
// MusicSegment.cpp: implementation of the CMusicSegment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ifx.h"
#include "MusicSegment.h"
#include "dxutil.h"

//#ifndef MM2K6_NODIRECTSOUND
//
//#define DEPRECATE_MUSIC
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMusicSegment::CMusicSegment(IDirectMusicPerformance8* pPerformance, IDirectMusicLoader8* pLoader, IDirectMusicSegment8* pSegment)
{
//#ifndef DEPRECATE_MUSIC
    m_pPerformance          = pPerformance;
    m_pLoader               = pLoader;
    m_pSegment              = pSegment;
    m_pEmbeddedAudioPath    = NULL;
    m_bDownloaded           = FALSE;
    
    // Try to pull out an audio path from the segment itself if there is one.
    // This embedded audio path will be used instead of the default
    // audio path if the app doesn't wish to use an overriding audio path.
    IUnknown* pConfig = NULL;
    if( SUCCEEDED( m_pSegment->GetAudioPathConfig( &pConfig ) ) )
    {
        m_pPerformance->CreateAudioPath( pConfig, TRUE, &m_pEmbeddedAudioPath );
        SAFE_RELEASE( pConfig );
    } 
//#endif // DEPRECATE_MUSIC
}

CMusicSegment::~CMusicSegment()
{

    if( m_pSegment )
    {
        // Tell the loader that this object should now be released
        if( m_pLoader )
            m_pLoader->ReleaseObjectByUnknown( m_pSegment );

        if( m_bDownloaded )
        {
            if( m_pEmbeddedAudioPath )
                m_pSegment->Unload( m_pEmbeddedAudioPath );
            else
                m_pSegment->Unload( m_pPerformance );
        }

        SAFE_RELEASE( m_pEmbeddedAudioPath ); 
        SAFE_RELEASE( m_pSegment ); 
    }

    m_pPerformance = NULL;

}

HRESULT CMusicSegment::Download( IDirectMusicAudioPath8* pAudioPath )
{
    HRESULT hr;
//#ifndef DEPRECATE_MUSIC    
    if( m_pSegment == NULL )
        return CO_E_NOTINITIALIZED;

    // If no audio path was passed in, then download
    // to the embedded audio path if it exists 
    // else download to the performance
    if( pAudioPath == NULL )
    {
        if( m_pEmbeddedAudioPath )
            hr = m_pSegment->Download( m_pEmbeddedAudioPath );
        else    
            hr = m_pSegment->Download( m_pPerformance );
    }
    else
    {
        hr = m_pSegment->Download( pAudioPath );
    }
    
    if ( SUCCEEDED( hr ) )
        m_bDownloaded = TRUE;
        
    return hr;
//#else
//	return 0;
//#endif
}

HRESULT CMusicSegment::Unload( IDirectMusicAudioPath8* pAudioPath )
{
    HRESULT hr;
//#ifndef DEPRECATE_MUSIC
    if( m_pSegment == NULL )
        return CO_E_NOTINITIALIZED;

    // If no audio path was passed in, then unload 
    // from the embedded audio path if it exists 
    // else unload from the performance
    if( pAudioPath == NULL )
    {
        if( m_pEmbeddedAudioPath )
            hr = m_pSegment->Unload( m_pEmbeddedAudioPath );
        else    
            hr = m_pSegment->Unload( m_pPerformance );
    }
    else
    {
        hr = m_pSegment->Unload( pAudioPath );
    }
        
    if ( SUCCEEDED( hr ) )
        m_bDownloaded = FALSE;

    return hr;
//#else
//	return 0;
//#endif
}

HRESULT CMusicSegment::Play( DWORD dwFlags,	IDirectMusicAudioPath8* 	pAudioPath )
{

    if( m_pSegment == NULL || m_pPerformance == NULL )
        return CO_E_NOTINITIALIZED;

    if( !m_bDownloaded )
        return E_FAIL;

    // If an audio path was passed in then use it, otherwise
    // use the embedded audio path if there was one.
    if( pAudioPath == NULL && m_pEmbeddedAudioPath != NULL )
        pAudioPath = m_pEmbeddedAudioPath;
        
    // If pAudioPath is NULL then this plays on the default audio path.
    return m_pPerformance->PlaySegmentEx( m_pSegment, 0, NULL, dwFlags, 
                                          0, 0, NULL, pAudioPath );

}

HRESULT CMusicSegment::SetRepeats( DWORD dwRepeats )
{
//#ifndef DEPRECATE_MUSIC
    if( m_pSegment == NULL )
        return CO_E_NOTINITIALIZED;

    return m_pSegment->SetRepeats( dwRepeats );
//#else
//	return 0;
//#endif
}

BOOL CMusicSegment::IsPlaying()
{
//#ifndef DEPRECATE_MUSIC
    if( m_pSegment == NULL || m_pPerformance == NULL )
        return CO_E_NOTINITIALIZED;

    return ( m_pPerformance->IsPlaying( m_pSegment, NULL ) == S_OK );
//#else
//	return false;
//#endif
}

HRESULT CMusicSegment::Stop( DWORD dwFlags )
{
//#ifndef DEPRECATE_MUSIC
    if( m_pSegment == NULL || m_pPerformance == NULL )
        return CO_E_NOTINITIALIZED;

    return m_pPerformance->Stop( m_pSegment, NULL, 0, dwFlags );;
//#else
//	return false;
//#endif
}

//#endif /* MM2K6_NODIRECTSOUND */