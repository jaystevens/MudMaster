#pragma once

#include "../EventArgs.h"

class UpdateBarItemEventArgs
	: public EventArgs
{
	CBarItem *_item;

public:
	UpdateBarItemEventArgs(CBarItem *item)
		: _item(item)
	{
	}

	CBarItem *GetItem() { return _item; }
};