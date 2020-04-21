#pragma once

#include <JuceHeader.h>



class MenuManager 
{
public:
	 MenuManager(){}
    virtual ~MenuManager();

	 virtual void createMenu(PopupMenu&, const String& menuName) = 0;
};

