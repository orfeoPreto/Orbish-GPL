#pragma once
#ifndef MainMenu_h
#define MainMenu_h

#include "MenuManager.h"

class MainMenu :
	public juce::MenuBarModel
{
public:
	MainMenu();
	MainMenu(MenuManager* mm);

	~MainMenu();

	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	StringArray getMenuBarNames() override;
	void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
	void menuBarActivated(bool isActive) override;

private:
	MenuManager* menuManager;
};

#endif