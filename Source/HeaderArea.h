/*
  ==============================================================================

    HeaderArea.h
    Created: 1 Aug 2020 12:47:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MainMenu.h"
#include "CommandIDs.h"
#include "exu/Label.hpp"
#include "CustomButton.h"
class OrbishAudioProcessorEditor;
class GlobalControlArea;
//==============================================================================

class HeaderArea  : public juce::Component, public MenuManager, public ApplicationCommandTarget{
public:
    HeaderArea();
    ~HeaderArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setEditor(OrbishAudioProcessorEditor* editor);

    // Adopt global control buttons into the header's transport pill
    void adoptGlobalControls(GlobalControlArea* globalArea);

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

    // Brand logo and project name
    juce::ImageComponent brandLogo;
    juce::Label projectNameLabel;

    // Readout labels for the top bar
    juce::Label tempoTitleLabel;
    juce::Label tempoValueLabel;
    juce::Label meterTitleLabel;
    juce::Label meterValueLabel;
    juce::Label positionTitleLabel;
    juce::Label positionValueLabel;

    void updateReadouts(const juce::String& sessionName, double tempo,
                        int tsNum, int tsDen, const juce::String& position);

private:
    OrbishAudioProcessorEditor* editor = nullptr;
    GlobalControlArea* globalControls = nullptr;

    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<MenuBarComponent> menuBar;
    std::unique_ptr<ApplicationCommandManager> commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderArea)
};
