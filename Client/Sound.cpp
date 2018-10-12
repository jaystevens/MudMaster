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
//#include "Extern.h"
#include "Sound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef MM2K6_NODIRECTSOUND

CSound::CALLBACKDATA CSound::_cbdSoundDone;
BOOL CSound::sm_bMidiOpen = FALSE;

void CALLBACK SoundCallback(
	HWAVEOUT /*hwo*/, 
	UINT uMsg, 
	DWORD dwInstance,
	DWORD /*dwParam1*/, 
	DWORD /*dwParam2*/)
{
	if (uMsg == WOM_DONE)
	{
		CSound::CALLBACKDATA *pData = 
			reinterpret_cast<CSound::CALLBACKDATA *>(dwInstance);

		ASSERT(pData != NULL);

		if (pData)
			pData->bSoundDone = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////
// CWave

CWave::CWave()
{
	m_pData = NULL;
	m_pWfx = NULL;
	m_lSize = 0;
	m_bPlaying = FALSE;
	m_nPriority = 100;
	m_nRepeat = 1;
	m_nHits = 0;
}

CWave::~CWave()
{
	if (m_bPlaying)
		Stop();

	if (m_pWfx)
	{
		delete [] m_pWfx;
		m_pWfx = NULL;
	}

	if (m_pData)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

BOOL CWave::Load(LPCSTR pszFilename)
{
	HMMIO       hmmio;              
	MMCKINFO    mmckinfoParent;     
	MMCKINFO    mmckinfoSubchunk;
	LONG			lFmtSize;
	char			szFilename[MAX_PATH+1];

	strcpy(szFilename,pszFilename);

	// Open the MM file and get a handle.
	hmmio = mmioOpen(szFilename,NULL,MMIO_READ|MMIO_ALLOCBUF);
	if (hmmio == NULL)
		return(FALSE);

	// Locate a "RIFF" chunk with a "WAVE" form type to make 
	// sure the file is a waveform-audio file. 
	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent,NULL,MMIO_FINDRIFF)) 
	{ 
		mmioClose(hmmio,0); 
		return(FALSE);
	} 

	// Find the "FMT" chunk (form type "FMT"); it must be 
	// a subchunk of the "RIFF" chunk. 
	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
	if (mmioDescend(hmmio,&mmckinfoSubchunk,&mmckinfoParent,MMIO_FINDCHUNK)) 
	{ 
		mmioClose(hmmio, 0); 
		return(FALSE); 
	} 

	// Allocate the memory for the wave header.
	lFmtSize = mmckinfoSubchunk.cksize; 
	m_pWfx = new char[lFmtSize];
	if (!m_pWfx)
	{
		mmioClose(hmmio, 0);
		return(FALSE);
	}
	// Read the "FMT" chunk. 
	if (mmioRead(hmmio, (HPSTR)m_pWfx, lFmtSize) != lFmtSize)
	{ 
		mmioClose(hmmio, 0); 
		return(FALSE); 
	} 

	// Ascend out of the "FMT" subchunk. 
	mmioAscend(hmmio, &mmckinfoSubchunk, 0); 
 
	// Find the data subchunk. The current file position should be at 
	// the beginning of the data chunk; however, you should not make 
	// this assumption. Use mmioDescend to locate the data chunk. 
	mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a'); 
	if (mmioDescend(hmmio,&mmckinfoSubchunk,&mmckinfoParent,MMIO_FINDCHUNK)) 
	{ 
		mmioClose(hmmio, 0); 
		return(FALSE); 
	} 

	m_lSize = mmckinfoSubchunk.cksize;

	// Get the size of the data subchunk. 
	if (m_lSize == 0)
	{ 
		mmioClose(hmmio, 0); 
		return(FALSE); 
	} 

	m_pData = new char[m_lSize+10];
	ASSERT(m_pData != NULL);

	// Read the waveform-audio data subchunk. 
	if(mmioRead(hmmio,(HPSTR)m_pData,m_lSize) != m_lSize)
	{ 
		mmioClose(hmmio, 0); 
		return(FALSE); 
	} 

	// Close the file. 
	mmioClose(hmmio, 0); 

	m_strFilename = pszFilename;

	memset(&m_hdr,0,sizeof(WAVEHDR));
	m_hdr.lpData = m_pData;
	m_hdr.dwBufferLength = m_lSize;
	m_hdr.dwFlags = WHDR_DONE;

	return(TRUE);
}

BOOL CWave::Play()
{
	ASSERT(m_pData != NULL);
	ASSERT(m_lSize != 0);

	CSound::_cbdSoundDone.bSoundDone = FALSE;
	CSound::_cbdSoundDone.pWave = this;

	MMRESULT mmResult;
	mmResult = waveOutOpen(
		&m_hwo,
		WAVE_MAPPER,
		(WAVEFORMATEX*)m_pWfx,
		(DWORD)(SoundCallback),
		(DWORD)(&CSound::_cbdSoundDone),
		CALLBACK_FUNCTION);

	if (mmResult != MMSYSERR_NOERROR)
		return(FALSE);

	mmResult = waveOutPrepareHeader(m_hwo,&m_hdr,sizeof(WAVEHDR));
	if (mmResult != MMSYSERR_NOERROR)
		return(FALSE);

	// Set this before the call to waveOutWrite is made on purpose.
	m_bPlaying = TRUE;
	waveOutSetVolume(m_hwo,m_dwVolume);
	mmResult = waveOutWrite(m_hwo,&m_hdr,sizeof(WAVEHDR));
	if (mmResult != MMSYSERR_NOERROR)
	{
		m_bPlaying = FALSE;
		waveOutUnprepareHeader(m_hwo,&m_hdr,sizeof(WAVEHDR));
		waveOutClose(m_hwo);
		return(FALSE);
	}

	m_nHits++;

	return(TRUE);
}

void CWave::Stop()
{
	if (m_bPlaying)
	{
		ASSERT(m_pData != NULL);

		// waveOutReset causes the callback to be called if the
		// wave file is playing.  So set the _bSoundDone flag to
		// false after calling it.
		waveOutReset(m_hwo);
		CSound::_cbdSoundDone.bSoundDone = FALSE;
		CSound::_cbdSoundDone.pWave = NULL;

		waveOutUnprepareHeader(m_hwo,&m_hdr,sizeof(WAVEHDR));
		waveOutSetVolume(m_hwo,m_dwOriginalVolume);
		waveOutClose(m_hwo); 

		m_bPlaying = FALSE;
	}
}

//
/////////////////////////////////////////////////////////////////////

CSound::CSound()
{
	m_bWaveIsValid = FALSE;
	m_bMidiIsValid = FALSE;

	m_nWaveOutDevices = waveOutGetNumDevs();
	if (m_nWaveOutDevices < 1)
		return;

	// If there are more than one -- too bad.  Going to select
	// the first output device.
	if (waveOutGetDevCaps(WAVE_MAPPER,&m_waveOutCaps,sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
		m_bWaveIsValid = TRUE;
	else
		m_bWaveIsValid = FALSE;

	// Number of sound bytes currently buffered.
	m_lCurrentBuffer = 0;

	m_nCount = 0;

	_cbdSoundDone.bSoundDone = FALSE;
	_cbdSoundDone.pWave = NULL;

	m_nMaxVolume = 100;

	// Take a guess at the volume bits.
	DWORD dwFormat = m_waveOutCaps.dwFormats;
	if (dwFormat & WAVE_FORMAT_1M16 || dwFormat & WAVE_FORMAT_1S16 ||
		 dwFormat & WAVE_FORMAT_2M16 || dwFormat & WAVE_FORMAT_2S16 ||
		 dwFormat & WAVE_FORMAT_4M16 || dwFormat & WAVE_FORMAT_4S16)
		m_nVolumeBits = 16;
	else
		m_nVolumeBits = 8;
	CalculateVolume();

	// Figure out the default setting of the volume.
	GetVolumeSetting();

	m_nMidiOutDevices = midiOutGetNumDevs();
	if (m_nMidiOutDevices < 1)
		return;

	if (midiOutGetDevCaps(0,&m_midiOutCaps,sizeof(MIDIOUTCAPS)) == MMSYSERR_NOERROR)
		m_bMidiIsValid = TRUE;
	else
		m_bMidiIsValid = FALSE;
}

CSound::~CSound()
{
	CWave *pWave;
	for (int i=0;i<m_nCount;i++)
	{
		pWave = (CWave *)m_ptrList.GetAt(i);
		ASSERT(pWave != NULL);
		delete pWave;
		pWave = NULL;
	}
	m_ptrList.RemoveAll();
}

void CSound::SetVolumeBits(int nBits)
{
	if (!m_bWaveIsValid)
		return;

	if (nBits != 16)
		m_nVolumeBits = 8;
	else
		m_nVolumeBits = 16;
	CalculateVolume();
}

void CSound::SetMaxVolume(int nVol)
{
	if (!m_bWaveIsValid)
		return;

	if (nVol < 1 || nVol > 100)
		nVol = 100;

	m_nMaxVolume = nVol;
	CalculateVolume();
}

void CSound::CalculateVolume()
{
	// Figure out the maximum value the card can support based on
	// the number of bits of the volume control.
	WORD wBitVolume;
	if (m_nVolumeBits == 16)
		wBitVolume = 0xFFFF;
	else
		wBitVolume = 0x8000;

	// Based on the max volume of the card, figure out what volume
	// the user wants to be max.
	WORD wTopVolume;
	wTopVolume = (WORD)(m_nMaxVolume / 100.0 * wBitVolume);

	// Use this new value for calculating the range of the volume.
	for (int i=0;i<101;i++)
	{
		m_dwLeftVolume[i] = (DWORD)(i / 100.0 * wTopVolume);
		m_dwRightVolume[i] = m_dwLeftVolume[i] << 16;
	}
}

void CSound::GetWaveDeviceCaps(WAVEOUTCAPS *pwaveOutCaps)
{
	if (m_bWaveIsValid)
		memcpy(pwaveOutCaps,&m_waveOutCaps,sizeof(WAVEOUTCAPS));
	else
		memset(pwaveOutCaps,0,sizeof(WAVEOUTCAPS));
}

void CSound::GetMidiDeviceCaps(MIDIOUTCAPS *pmidiOutCaps)
{
	if (m_bMidiIsValid)
		memcpy(pmidiOutCaps,&m_midiOutCaps,sizeof(MIDIOUTCAPS));
	else
		memset(pmidiOutCaps,0,sizeof(MIDIOUTCAPS));
}

BOOL CSound::PlayMidi(LPCSTR pszFilename)
{
	if (!m_bMidiIsValid)
		return(FALSE);

	// An empty string means we want to stop the current midi
	// file from playing.
	if (!pszFilename || !strlen(pszFilename))
	{
		if (sm_bMidiOpen)
		{
			mciSendString("close MudMaster", NULL, NULL, NULL);
			sm_bMidiOpen = FALSE;
		}
		return(TRUE);
	}

	CString strMidi(pszFilename);

	// Make sure to close it if we already have one open.
	if (sm_bMidiOpen)
	{
		mciSendString("close MudMaster", NULL, NULL, NULL);
		sm_bMidiOpen = FALSE;
	}

	// Create the send string for opening the file.
	CString strSend;
	strSend.Format(_T("open %s type sequencer alias MudMaster"),
		(LPCSTR )strMidi);

	// Returns 0 if successful.
	if (mciSendString(strSend, NULL, NULL, NULL))
		return(FALSE);

	if (mciSendString("play MudMaster", NULL, NULL, NULL))
	{
		mciSendString("close MudMaster", NULL, NULL, NULL);
		return(FALSE);
	}

	sm_bMidiOpen = TRUE;
	return(TRUE);
}

BOOL CSound::PlayWave(LPCSTR pszFilename, int nLeftVol, int nRightVol, int nPriority, int nRepeat)
{
	// Make sure there is something to use for playback.
	if (!m_bWaveIsValid)
		return(FALSE);

	if (_cbdSoundDone.bSoundDone)
		SoundDone();

	// If a NULL or empty string is passed in, stop any file from playing.
	if (!pszFilename || !strlen(pszFilename))
	{
		if (_cbdSoundDone.pWave)
			_cbdSoundDone.pWave->Stop();
		return(TRUE);
	}

	// A repeat value of 0 is not valid.
	if (!nRepeat)
		nRepeat = 1;

	// Searches the cache for this file.
	int nIndex;
	CWave *pWave = FindWave(pszFilename, nIndex);

	if (!pWave)
	{
		// Not in the cache.
		pWave = new CWave;
		ASSERT(pWave != NULL);

		pWave->m_bPlaying = FALSE;

		// Load it up and add it to the cache.
		if (!pWave->Load(pszFilename))
		{
			delete pWave;
			pWave = NULL;
			return(FALSE);
		}

		// Need to let it know what the original volume level was.
		pWave->m_dwOriginalVolume = m_dwOriginalVolume;

		// Keep track of the number of bytes buffered.
		m_lCurrentBuffer += pWave->m_lSize;

		// Add this to our buffered list.
		m_ptrList.SetAtGrow(m_nCount,pWave);
		m_nCount++;

		// If we have gone over our buffer limit, throw some files
		// out of memory until we are back to normal.
		CheckBufferLimit();
	}
	else
	{
		// Remove the item from the array.
		m_ptrList.RemoveAt(nIndex);

		// Stick it back in at the end.
		m_ptrList.SetAtGrow(m_nCount-1,pWave);
	}

	// The priority of a file existing in the cache can be changed.
	pWave->m_nPriority = nPriority;
	pWave->m_nRepeat = nRepeat;

	// I think sometimes the waves to get closed up as a result of the
	// callback.  Hopefully this will catch it.
	if (pWave->m_bPlaying && pWave->m_hdr.dwFlags & WHDR_DONE)
		pWave->Stop();

	// If the wave found is playing, don't do anything.  This will keep it
	// fairly smooth with repetitive use of the same wave file.
	if (pWave->m_bPlaying)
		return(FALSE);

	// Make the volume.
	if (nLeftVol < 0 || nLeftVol > 100 || nRightVol < 0 || nRightVol > 100)
		pWave->m_dwVolume = 0xFFFFFFFF;
	else
		pWave->m_dwVolume = m_dwLeftVolume[nLeftVol] | m_dwRightVolume[nRightVol];

	// Check for a sound being played.  Check priorities to see which file
	// should be playing.
	if (_cbdSoundDone.pWave)
	{
		if (_cbdSoundDone.pWave->m_nPriority < nPriority)
			_cbdSoundDone.pWave->Stop();
		else
			return(TRUE);
	}

	if (!pWave->Play())
		return(FALSE);

	return(TRUE);
}

CWave* CSound::FindWave(LPCSTR pszFilename, int &nIndex)
{
	CWave *pWave;
	for (int i=0;i<m_nCount;i++)
	{
		pWave = (CWave *)m_ptrList.GetAt(i);
		ASSERT(pWave != NULL);
		if (pWave->m_strFilename == pszFilename)
		{
			nIndex = i;
			return(pWave);
		}
	}
	return(NULL);
}

void CSound::CheckBufferLimit()
{
	if (m_nCount < 2 || m_lCurrentBuffer < m_lSoundCache)
		return;

	CWave *pWave;
	while(m_nCount && m_lCurrentBuffer > m_lSoundCache)
	{
		pWave = (CWave *)m_ptrList.GetAt(0);
		ASSERT(pWave != NULL);
		m_lCurrentBuffer -= pWave->m_lSize;
		delete pWave;
		pWave = NULL;
		m_ptrList.RemoveAt(0);
		m_nCount--;
	}
}

CWave* CSound::GetFirst()
{
	if (!m_bWaveIsValid)
		return(NULL);
	m_nFindIndex = 0;
	if (!m_nCount)
		return(NULL);
	return((CWave *)m_ptrList.GetAt(m_nFindIndex));
}

CWave* CSound::GetNext()
{
	if (!m_bWaveIsValid)
		return(NULL);
	m_nFindIndex++;
	if (m_nFindIndex == m_nCount)
		return(NULL);
	return((CWave *)m_ptrList.GetAt(m_nFindIndex));
}

void CSound::GetVolumeSetting()
{
	if (!m_bWaveIsValid)
		return;

	CWave wave;

	// Try to load a wave file.  Ring.wav should be there unless they moved 
	// it.  If unable to open, just going to pick a volume level.
	if (!wave.Load("ring.wav"))
	{
		m_dwOriginalVolume = m_dwLeftVolume[50] | m_dwRightVolume[50];
		return;
	}
		
	MMRESULT mmResult;
	mmResult = waveOutOpen(&wave.m_hwo,WAVE_MAPPER,(WAVEFORMATEX*)wave.m_pWfx,0,0,CALLBACK_NULL);
	if (mmResult != MMSYSERR_NOERROR)
	{
		m_dwOriginalVolume = m_dwLeftVolume[50] | m_dwRightVolume[50];
		return;
	}

	mmResult = waveOutGetVolume(wave.m_hwo,&m_dwOriginalVolume);
	if (mmResult != MMSYSERR_NOERROR)
		m_dwOriginalVolume = m_dwLeftVolume[50] | m_dwRightVolume[50];

	waveOutClose(wave.m_hwo);
}

void CSound::SoundDone()
{
	ASSERT(_cbdSoundDone.pWave != NULL);

	if (_cbdSoundDone.pWave == NULL)
		return;

	//MessageBeep(0);

	waveOutUnprepareHeader(_cbdSoundDone.pWave->m_hwo,&_cbdSoundDone.pWave->m_hdr,sizeof(WAVEHDR));
	waveOutSetVolume(_cbdSoundDone.pWave->m_hwo,_cbdSoundDone.pWave->m_dwOriginalVolume);
	waveOutClose(_cbdSoundDone.pWave->m_hwo);

	_cbdSoundDone.pWave->m_nRepeat--;

	if (_cbdSoundDone.pWave->m_nRepeat)
		_cbdSoundDone.pWave->Play();
	else
	{
		_cbdSoundDone.pWave->m_bPlaying = FALSE;
		_cbdSoundDone.pWave = NULL;
	}
	_cbdSoundDone.bSoundDone = FALSE;
}

#endif /* MM2K6_NODIRECTSOUND */
