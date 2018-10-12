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

#include <boost/shared_ptr.hpp>

class CCharFormat
{
public:
	typedef boost::shared_ptr<CCharFormat> PtrType;

	CCharFormat(BYTE fore, BYTE back);
	CCharFormat();

	BYTE Back()	
	{
		return (m_attr & 0xE) >> 1;
	}

	BYTE Fore() 
	{
		return (m_attr & 0xF0)>>4;
	}

	BOOL Bold()	{return m_attr & 1;}

	void Back(BYTE back)
	{
		m_attr = (m_attr & 0xF1) | ( back << 1 );
	}

	void Fore(BYTE fore)
	{
		//m_attr &= 0xF;
		//m_attr |= (fore << 4);
		m_attr = (m_attr & 0xF) | (fore << 4);
// change by Steven Jackson
	}

	void Bold(bool bBold)		{ m_attr = (m_attr & 0xFE) | (bBold ? 1 : 0);}

private:
	BYTE m_attr;
};
