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

namespace MMScript
{
	/**
	* \class CScriptEntity ScriptEntity.h 
	* \brief an interface for all script entities.
	*/
	class CScriptEntity : public CObject
	{
	public:
		/**
		* method description for mapping this entity to a mudmaster command
		* @param strCommand a buffer for holding the generated command
		*/
		virtual void MapToCommand(CString& strCommand) = 0;

		/**
		* method for maping the script entity to a display string
		* @param strText a buffer for holding the display string
		* @param bIncludeGroup TRUE if the method should include this entities group in the display string
		*/
		virtual void MapToText(CString &strText, BOOL bIncludeGroup) = 0;

		/**
		* method for setting the group that this entity belongs to
		* @param strGroup the string that contains the name of the group for this entity
		*/
		virtual void Group(const CString& strGroup) = 0;

		/**
		* method for retrieving the group that this entity belongs to
		* @return a string containing the name of the group for this script entity
		*/
		virtual const CString &Group() const = 0;
	};
}