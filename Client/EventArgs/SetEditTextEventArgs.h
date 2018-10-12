#pragma once

#include "../EventArgs.h"

class SetEditTextEventArgs
	: public EventArgs
{
	LPCTSTR _text;

public:
	SetEditTextEventArgs(LPCTSTR pszText)
		: _text(pszText) {}

		LPCTSTR GetText() { return _text; }
};