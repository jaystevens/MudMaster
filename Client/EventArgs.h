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

class EventArgs
{
public:
	EventArgs(void);
	virtual ~EventArgs(void);
};

typedef enum EventType
{
	eSetEditText,
	eOpenLog,
	eCloseLog,
	eLoadScript,
	eStartChatServer,
	eStopChatServer,
	eShowStatusBar,
	eHideStatusBar,
	eUpdateBarItem,
} EventType;

class Observer
{
public:
	virtual void notify(EventType type, EventArgs *args) = 0;
};

struct ObserverItem
{
	EventType _event;
	Observer &_observer;

	ObserverItem(EventType type, Observer &o)
		: _event(type)
		, _observer(o)
	{
	}

};

class Subject
{
	CAtlList<ObserverItem> _observers;

public:
	void advise(EventType e, Observer &o)
	{
		bool alreadyRegistered = false;

		POSITION pos = _observers.GetHeadPosition();
		while(pos)
		{
			ObserverItem &item = _observers.GetNext(pos);
			if(item._event == e && &item._observer == &o)
			{
				alreadyRegistered = true;
				TRACE(_T("Observer already registered"));
				break;
			}
		}

		if(!alreadyRegistered)
		{
			ObserverItem item(e, o);
			_observers.AddTail(item);
		}
	}

	void notify(EventType type, EventArgs &args)
	{
		POSITION pos = _observers.GetHeadPosition();
		while(pos)
		{
			ObserverItem &item = _observers.GetNext(pos);
			if(item._event == type)
			{
				item._observer.notify(type, &args);
			}
		}
	}
};