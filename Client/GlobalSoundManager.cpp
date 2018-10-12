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
// GlobalSoundManager.cpp: implementation of the CGlobalSoundManager class.
//
//////////////////////////////////////////////////////////////////////

/* RLB - Plan is to replace Direct sound with SFML Sound Libraries for reasons
 *  1) Its portable should we ever get to the point MudMaster can be ported to another platform.
 *  2) DirectMusic is deprecated and only works with old Visual Studio and SDK setups.
 */

#include "stdafx.h"
#include "ifx.h"
#include "GlobalSoundManager.h"
#include "SoundManager.h"
#include "MusicSegment.h"
#include "dxutil.h"
//#ifndef MM2K6_NODIRECTSOUND
#include <dxerr8.h>
//#else // Otherwise use SFML for our sound library
//#include <SFML/Audio.hpp>
//#endif // MM2K6_NODIRECTSOUND


// Take this after we finish implementing SFML
#pragma warning (disable : 4100)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct CGlobalSoundManager::GlobalSoundManagerImpl
{
//#ifndef MM2K6_NODIRECTSOUND
	GlobalSoundManagerImpl()
		: _nCurrentFXPriority(0)
	{
	}

	
	std::auto_ptr<CSoundManager> _pSoundManager;
	std::auto_ptr<CMusicSegment> _pMidiSegment;
	std::auto_ptr<CMusicSegment> _pFxSegment;

	CComPtr<IDirectMusicAudioPath> _sp3dAudioPath;

	int _nCurrentFXPriority;
//#else // SFML Sound
//	sf::Sound Music;  // ptr to Music 
//	sf::Sound FX; // ptr to Sound effects
//#endif // MM2K6_NODIRECTSOUND
};

CGlobalSoundManager::CGlobalSoundManager()
: _pImpl(new GlobalSoundManagerImpl)
{
}

CGlobalSoundManager::~CGlobalSoundManager()
{
}

HRESULT CGlobalSoundManager::Initialize(HWND hWnd)
{
//#ifdef MM2K6_NODIRECTSOUND
//	// Use SFML sound libraries
//	return TRUE;
//#else // Otherwise we do have DirectMusic & Friends
	HRESULT hr;

	_pImpl->_pSoundManager.reset(new CSoundManager());
	if(FAILED(hr=_pImpl->_pSoundManager->Initialize(hWnd) ) )
	{
		return DXTRACE_ERR(TEXT("Initializing Global Sound Manager"), hr);
	}

	IDirectMusicPerformance8 *pPerformance = _pImpl->_pSoundManager->GetPerformance();

	if(FAILED(hr=pPerformance->CreateStandardAudioPath(
		DMUS_APATH_DYNAMIC_3D, 128, TRUE, &_pImpl->_sp3dAudioPath) ) )
	{
		return DXTRACE_ERR( TEXT("CreateStandardAudioPath"), hr );
	}

	return TRUE;
//#endif // MM2K6_NODIRECTSOUND
}

HRESULT CGlobalSoundManager::PlayBackgroundMusic(
		LPCTSTR strFilename, 
		int nRVol,				//=100
		int nLVol,				//=100
		int nRepeats,			//=1
		BOOL bContinue,			//=TRUE
		LPCTSTR strType,		//=NULL
		LPCTSTR strURL)			//=NULL
{
//#ifdef MM2K6_NODIRECTSOUND
//	// Use SFML Sound Libraries
//	return TRUE;
//#else // Otherwise use directmusic
	if(NULL != _pImpl->_pMidiSegment.get())
	{
		if(_pImpl->_pMidiSegment->IsPlaying())
			_pImpl->_pMidiSegment->Stop();

		_pImpl->_pMidiSegment->Unload();
		_pImpl->_pMidiSegment.reset();
	}

	CMusicSegment *pMusicSegment = NULL;
	HRESULT hr = _pImpl->_pSoundManager->CreateSegmentFromFile(&pMusicSegment, strFilename, TRUE, TRUE);
	if(FAILED(hr))
	{
		ASSERT(FALSE);
		return hr;
	}
	_pImpl->_pMidiSegment.reset(pMusicSegment);

	hr = _pImpl->_pMidiSegment->SetRepeats(nRepeats);
	if(FAILED(hr))
	{
		ASSERT(FALSE);
		return hr;
	}

	hr = _pImpl->_pMidiSegment->Play(DMUS_SEGF_DEFAULT, _pImpl->_sp3dAudioPath);
	if(FAILED(hr))
	{
		ASSERT(FALSE);
		return hr;
	}

	return S_OK;
//#endif // MM2K6_NODIRECTSOUND
}

HRESULT CGlobalSoundManager::PlayFX(
		LPCTSTR strFilename, 
		int /*nRVol*/,				//=100
		int /*nLVol*/,				//=100
		int nRepeats,			//=1
		int /*pPriority*/,			//=50
		LPCTSTR /*strType*/,		//=NULL
		LPCTSTR /*strURL*/)			//=NULL
{
//#ifdef MM2K6_NODIRECTSOUND
//	// Use SFML Libraries
//	return S_OK;
//#else
	if(NULL != _pImpl->_pFxSegment.get())
	{
		if(_pImpl->_pFxSegment->IsPlaying())
			_pImpl->_pFxSegment->Stop();

		_pImpl->_pFxSegment->Unload();
		_pImpl->_pFxSegment.reset();
	}

	CMusicSegment *pFxSegment = NULL;
	HRESULT hr = _pImpl->_pSoundManager->CreateSegmentFromFile(&pFxSegment, strFilename);
	if(FAILED(hr))
	{
		if(hr == DMUS_E_LOADER_FAILEDOPEN)
			return hr;

		ASSERT(FALSE);
		return hr;
	}

	_pImpl->_pFxSegment.reset(pFxSegment);
	hr = _pImpl->_pFxSegment->SetRepeats(nRepeats);
	if(FAILED(hr))
	{
		ASSERT(FALSE);
		return hr;
	}

	hr = _pImpl->_pFxSegment->Play(
		DMUS_SEGF_DEFAULT | DMUS_SEGF_SECONDARY, _pImpl->_sp3dAudioPath);
	if(FAILED(hr))
	{
		ASSERT(FALSE);
		return hr;
	}

	return S_OK;
//#endif // MM2K6_NODIRECTSOUND
}

HRESULT CGlobalSoundManager::SetSearchDirectory(LPCTSTR strDirectory)
{
//#ifdef MM2K6_NODIRECTSOUND
//	return TRUE;
//#else
	return _pImpl->_pSoundManager->SetSearchDirectory(strDirectory);
//#endif
}

void CGlobalSoundManager::StopFX()
{
//#ifndef MM2K6_NODIRECTSOUND
	_pImpl->_pFxSegment->Stop();
//#endif
}

void CGlobalSoundManager::StopBackgroundMusic()
{
//#ifndef MM2K6_NODIRECTSOUND
	_pImpl->_pMidiSegment->Stop();
//#endif 
}
