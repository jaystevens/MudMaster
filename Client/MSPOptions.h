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

namespace SerializedOptions
{
	/**
	* @class CMSPOptions MSPOptions.h
	* @brief Class that holds the options for the general sound settings for a session
	* @author Kevin Cook
	*/
	class CMspOptions
	{
	public:
		/**
		* Default Constructor
		*/
		CMspOptions();

		/**
		* Copy Constructor
		* @param src a reference to another CMSPOption variable to use a a template
		*/
		CMspOptions(const CMspOptions& src);

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		CMspOptions &operator=(const CMspOptions &src);

		/**
		* Implicit desctruction
		*/
		virtual ~CMspOptions();

		HRESULT Read(LPCTSTR lpszPathName);
		HRESULT Save(LPCTSTR lpszPathName);

		////////////////////////////////////////////////////////////////////////////////
		//ACCESSORS
		////////////////////////////////////////////////////////////////////////////////
		/**
		* Accessor for msp status
		* @return the status of MSP
		* @retval TRUE process MSP signals
		* @retval FALSE ignore MSP signals
		*/
		BOOL On(void)						{return m_bOn;}

		/**
		* Accessor for the UsePlaySound setting
		* @return the status the UsePlaySound setting
		* @retval TRUE use playsound for playing sound files
		* @retval FALSE use directx for playing sound files
		*/
		BOOL UsePlaySound(void)				{return m_bUsePlaySound;}

		/**
		* Accessor for the ProcessMidi setting
		* @return the status of the Process Midi setting
		* @retval TRUE if MSP !!MUSIC() signals should be processed
		* @retval FALSE if MSP !!MUSIC() signals should be ignored
		*/
		BOOL ProcessMidi(void)				{return m_bProcessMidi;}

		/**
		* Accessor for the ProcessFX setting
		* @return the status of the ProcessFX setting
		* @retval TRUE if MSP !!SOUND() signals should be processed
		* @retval FALSE if MSP !!SOUND() signals should be ignored
		*/
		BOOL ProcessFX(void)				{return m_bProcessFX;}

		/**
		* Accessor for the ShowMSPTriggers
		* @attention This setting is ignored of MSP is off.
		* @return the status of the ShowMSPTriggers setting
		* @retval TRUE if MM should display MSP Triggers even if they've been processed.
		* @retval FALSE if MM should gag the display of MSP Triggers after they've been processed.
		*/
		BOOL ShowMSPTriggers(void)			{return m_bShowMSPTriggers;}

		////////////////////////////////////////////////////////////////////////////////
		//MUTATORS
		////////////////////////////////////////////////////////////////////////////////

		/**
		* Mutator for the MSP base status
		* @param bOn TRUE if MSP signals should be processed
		*/
		void On(BOOL bOn)						{m_bOn = bOn;}

		/**
		* Mutator for the UsePlaySound Setting
		* @param bUsePlaySound TRUE if PlaySound should be used, False if DirectX should be used
		*/
		void UsePlaySound(BOOL bUsePlaySound)	{m_bUsePlaySound = bUsePlaySound;}

		/**
		* Mutator for the ProcessMidi setting
		* @param bProcessMidi TRUE if !!MUSIC() msp signals should be processed FALSE if !!MUSIC() signals should be ignored
		*/
		void ProcessMidi(BOOL bProcessMidi)		{m_bProcessMidi = bProcessMidi;}

		/**
		* Mutator for the ProcessFX setting
		* @param bProcessFX TRUE if !!SOUND() msp signals should be processed, FALSE if !!SOUND() signals should be ignored
		*/
		void ProcessFX(BOOL bProcessFX)			{m_bProcessFX = bProcessFX;}

		/**
		* Mutator for the ShowMSPTriggers Setting
		* @param bShowMSPTriggers TRUE if MSP Triggers should be displayed, FALSE if they should be ignored
		*/
		void ShowMSPTriggers(BOOL bShowMSPTriggers)	{m_bShowMSPTriggers = bShowMSPTriggers;}

	private:
		void Swap(CMspOptions &src);

		/**
		* Value holder for the process midi setting
		*/
		BOOL m_bProcessMidi;

		/**
		* Value holder for the process FX setting
		*/
		BOOL m_bProcessFX;

		/**
		* Value holder for the process msp setting
		*/
		BOOL m_bOn;

		/**
		* Value holder for the use playsound setting
		*/
		BOOL m_bUsePlaySound;

		/**
		* Value holder for the show msp triggers setting
		*/
		BOOL m_bShowMSPTriggers;
	};
}