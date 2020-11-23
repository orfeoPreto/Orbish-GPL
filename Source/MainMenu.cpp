#include "MainMenu.h"

MainMenu::MainMenu() : MenuBarModel() {}
MainMenu::MainMenu(MenuManager* mm) : MenuBarModel() {
	menuManager = mm;
}
MainMenu::~MainMenu() {}

PopupMenu MainMenu::getMenuForIndex(int topLevelMenuIndex, const String& menuName) {
	PopupMenu menu;
	menuManager->createMenu(menu, menuName);
	return menu;
}

StringArray MainMenu::getMenuBarNames() { return { "File", "Settings"}; }
void MainMenu::menuItemSelected(int menuItemID, int topLevelMenuIndex) {}
void MainMenu::menuBarActivated(bool isActive) {}