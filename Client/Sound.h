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
#ifndef _SOUND_H_
#define _SOUND_H_


#include "mmsystem.h"

#define DEFAULT_SOUND_ARRAY_SIZE	150
#define DEFAULT_SOUND_GROW_SIZE	20

class CWave
{
public:
	CWave();
	~CWave();

	// Loads up a wave file.
	BOOL Load(LPCSTR pszFilename);
	
	// Plays the wave file.
	BOOL Play();

	// Stops playback of the file.
	void Stop();

	CString	m_strFilename;				// Full pathname of the file.
	HWAVEOUT m_hwo;						// Handle to wave output.
	char     *m_pData;					// Sound data.
	LONG		m_lSize;						// Number of sound data bytes.
	WAVEHDR	m_hdr;
	char		*m_pWfx;						// Wave format.  This data at the head of this buffer
												//		is stored as a WAVEFORMATEX header.  There can
												//		can be addiontal data appended to tail of the
												//		buffer so allocating required bytes by hand.
												//		Needs to be typecast back to WAVEFORMATEX everywhere.
	BOOL		m_bPlaying;					// True while being played.
	DWORD		m_dwVolume;					// Volume, left and right channels.
	int		m_nPriority;
	int		m_nRepeat;					// Number of times to repeat the sound.
	int		m_nHits;						// Number of times the sound has been played.
	DWORD		m_dwOriginalVolume;		// I'm puting this in each individual record because
												// the callback function doesn't have access to the
												// CSound class.
};

class CSound
{
	// This is used by the sound callback procedure.  It is set to TRUE when
	// a sound is done playing.
public:
	struct CALLBACKDATA
	{
		BOOL	bSoundDone;
		CWave *pWave;
	};

	static CALLBACKDATA _cbdSoundDone;

public:
	CSound();
	~CSound();

	// Returns true if the sound system is ready to use.
	BOOL WaveIsValid()						{ return(m_bWaveIsValid); }
	BOOL MidiIsValid()						{ return(m_bMidiIsValid); }

	// Fills in a WAVEOUTCAPS structure with the outcap structure
	// of the opened device.
	void GetWaveDeviceCaps(WAVEOUTCAPS *pwaveOutCaps);
	void GetMidiDeviceCaps(MIDIOUTCAPS *pmidiOutCaps);

	// Plays a wave file.  The left and right volumes are percentages of the total
	// volume level.  100 is 100% volume.  Priority is used to determine which
	// sound is more important.  If a sound is playing and a sound with a higher
	// priority comes along, the playing sound is stopped and the new one is started.
	// If a sound is requested for playing and it has a lower priority it is just
	// ignored.
	BOOL PlayWave(LPCSTR pszFilename, int nLeftVol=100, int nRightVol=100, int nPriority=100, int nRepeat=1);

	BOOL PlayMidi(LPCSTR pszFilename);

	// This is called from the main loop when the callback procedure sets the flag
	// to let the app know a sound has just completed.
	void SoundDone();

	void SetMaxVolume(int nVol);
	int  GetMaxVolume()					{ return(m_nMaxVolume); }

	// Sets the number of bits then recacluates the volume values.
	void SetVolumeBits(int nBits);
	int  GetVolumeBits()					{ return(m_nVolumeBits); }

	void SetCacheSize(unsigned long lSize)	{ m_lSoundCache = lSize; }
	unsigned long GetCacheSize()		{ return(m_lSoundCache); }
	unsigned long GetBytesCached()	{ return(m_lCurrentBuffer); }

	CWave* GetFirst();
	CWave* GetNext();
	int	 GetFindIndex()				{ return(m_nFindIndex); }

	int GetCount()							{ return(m_nCount); }

private:

	// Searches the cache for the file.  Returns a pointer to the CWave object if 
	// found, else NULL.  The reference to an integer is used to pass back the
	// index of the item in the array.  This allows the item to be removed an
	// readded to the tail.
	CWave* FindWave(LPCSTR pszFilename, int &nIndex);

	// Checks the bytes cached against the cache size.  Tosses out files in the 
	// cache until the back under the limit.
	void CheckBufferLimit();

	// This determines the sound card's current volume level.  This is used
	// to get the initial sound setting.  A wave file must be opened to
	// get the setting.
	void GetVolumeSetting();

	// Fills in the two arrays that have the sound values based on 16bit or
	// 8bit volume control.
	void CalculateVolume();

private:

	// Used for creating the volume settings.
	int m_nVolumeBits;

	// This is the volume the sound card is set at when MM first runs.
	// After a sound is played the volume is set back to this level.
	DWORD m_dwOriginalVolume;

	// This is a percentage of the sound card's full volume.  This is
	// the level that 100% should be played at.
	int m_nMaxVolume;

	// Precalculated values for setting the volume.  Rather than dividing
	// the max volume each time to find a percentage, the values are
	// calculated in the constructor.
	DWORD m_dwLeftVolume[101];
	DWORD m_dwRightVolume[101];

	// The number of waveform output devices.
	int m_nWaveOutDevices;
	int m_nMidiOutDevices;

	// Output device capabilities.
	WAVEOUTCAPS m_waveOutCaps;
	MIDIOUTCAPS m_midiOutCaps;

	// Number of bytes currently buffered.
	unsigned long m_lCurrentBuffer;

	// Number of bytes max to keep around for the cache.
	unsigned long m_lSoundCache;

	// TRUE if the sound system is ok to use.
	BOOL m_bWaveIsValid;
	BOOL m_bMidiIsValid;

	CPtrArray m_ptrList;
	int m_nCount;
	int m_nFindIndex;

public:
	static BOOL sm_bMidiOpen;
};
#endif
