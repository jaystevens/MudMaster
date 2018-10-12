#pragma once

class IGroupable
{
public:
	virtual const std::string &GetGroup() const = 0;
	virtual void SetGroup( const std::string &groupName ) = 0;
};
