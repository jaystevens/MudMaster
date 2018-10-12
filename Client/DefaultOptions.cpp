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
#include "stdafx.h"
#include ".\defaultoptions.h"
#include "ConfigFileConstants.h"
#include "FileUtils.h"

using namespace std;

COptions::COptions(void)
{
}

COptions::~COptions(void)
{
}

void COptions::Load(LPCTSTR szPath)
{
	chatOptions_.Read(szPath);
	colorOptions_.Read(szPath);
	debugOptions_.Read(szPath);
	inputOptions_.Read(szPath);
	messageOptions_.Read(szPath);
	pathOptions_.Read(szPath);
	statusOptions_.Read(szPath);
	terminalOptions_.Read(szPath);
	triggerOptions_.Read(szPath);
	sessionInfo_.Read(szPath);
	//TODO: KW added read for sound options
	mspOptions_.Read(szPath);
}

void COptions::Save(LPCTSTR szPath)
{
	chatOptions_.Save(szPath);
	colorOptions_.Save(szPath);
	debugOptions_.Save(szPath);
	inputOptions_.Save(szPath);
	messageOptions_.Save(szPath);
	pathOptions_.Save(szPath);
	statusOptions_.Save(szPath);
	terminalOptions_.Save(szPath);
	triggerOptions_.Save(szPath);
	sessionInfo_.Save(szPath);
	//TODO: KW added save for sound options
	mspOptions_.Save(szPath);
}
void COptions::SaveSessionPosition()
{
	sessionInfo_.SaveSessionPosition();

}

void COptions::LoadDefaults()
{
	CString dir;

		dir = MMFileUtils::fixPath(INI_FILE_NAME);

		Load( dir );
	
}

void COptions::SaveDefaults()
{
	CString dir;
	dir = MMFileUtils::fixPath(INI_FILE_NAME);

	Save( dir );

}