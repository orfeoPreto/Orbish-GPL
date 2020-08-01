/*
  ==============================================================================

    HeaderArea.h
    Created: 1 Aug 2020 12:47:15pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MainMenu.h"
#include "CommandIDs.h"
class OrbishAudioProcessorEditor;
//==============================================================================
/*
*/
class HeaderArea  : public juce::Component, public MenuManager, public ApplicationCommandTarget{
public:
    HeaderArea(OrbishAudioProcessorEditor* editor);
    ~HeaderArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void bindEditor(OrbishAudioProcessorEditor* editor);

    void initCommandManager();
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(Array< CommandID >& commands) override;
    void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    MenuBarModel* getMenuModel();
    StringArray getMenuNames();
    void createMenu(PopupMenu&, const String& menuName) override;
    void createFileMenu(PopupMenu& menu);
    void createEditMenu(PopupMenu& menu);
    void createSettingsMenu(PopupMenu& menu);

private:
    OrbishAudioProcessorEditor* editor;

    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<MenuBarComponent> menuBar;
    std::unique_ptr<ApplicationCommandManager> commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderArea)
};