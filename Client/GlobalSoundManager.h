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


//#ifndef MM2K6_NODIRECTSOUND
#include <dmusici.h>
//#endif

class CGlobalSoundManager  
{
public:
	void StopBackgroundMusic();
	void StopFX();
	HRESULT SetSearchDirectory(LPCTSTR strDirectory);

	// Play a wave file as a sound effect
	HRESULT PlayFX(
		LPCTSTR strFilename, 
		int nRVol		= 100, 
		int nLVol		= 100, 
		int nRepeats	= 1, 
		int nPriority	= 50, 
		LPCTSTR strType = NULL, 
		LPCTSTR strURL	= NULL);


	// Play a midi file as background music
	HRESULT PlayBackgroundMusic(
		LPCTSTR strFilename, 
		int nRVol		= 100, 
		int nLVol		= 100, 
		int nRepeats	= 1, 
		BOOL bContinue	= TRUE, 
		LPCTSTR strType = NULL, 
		LPCTSTR strURL	= NULL);

	HRESULT Initialize(HWND hWnd);
	CGlobalSoundManager();
	virtual ~CGlobalSoundManager();

private:
	struct GlobalSoundManagerImpl;
	std::auto_ptr<GlobalSoundManagerImpl> _pImpl;
//#ifndef MM2K6_NODIRECTSOUND
//	sf::Sound Sound; // Required SFML Sound
//#endif 
};