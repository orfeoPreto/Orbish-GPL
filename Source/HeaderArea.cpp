/*
  ==============================================================================

    HeaderArea.cpp
    Created: 1 Aug 2020 12:47:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HeaderArea.h"
#include "GlobalControlArea.h"
#include "PluginEditor.h"

//==============================================================================
HeaderArea::HeaderArea(){
    initCommandManager();
    mainMenu = std::make_unique<MainMenu>(this);
    MenuBarModel* mm = mainMenu.get();
    menuBar = std::make_unique<MenuBarComponent>(mm);
    addAndMakeVisible(menuBar.get());

    // Brand logo
    auto logoImage = juce::ImageFileFormat::loadFrom(BinaryData::orbishlogowebdown_png, BinaryData::orbishlogowebdown_pngSize);
    brandLogo.setImage(logoImage, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    brandLogo.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(brandLogo);

    // Project name below logo
    projectNameLabel.setText("Untitled", juce::NotificationType::dontSendNotification);
    projectNameLabel.setFont(juce::Font(9.0f, juce::Font::plain));
    projectNameLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.45f));
    projectNameLabel.setJustificationType(juce::Justification::centred);
    projectNameLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(projectNameLabel);

    // Readout titles
    auto setupTitle = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };
    setupTitle(tempoTitleLabel, "TEMPO");
    setupTitle(meterTitleLabel, "METER");
    setupTitle(positionTitleLabel, "POSITION");

    // Readout values
    auto setupValue = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };
    setupValue(tempoValueLabel, "120.0");
    setupValue(meterValueLabel, "4/4");
    setupValue(positionValueLabel, "1.1.1");
}

HeaderArea::~HeaderArea(){
}

MenuManager::~MenuManager(){
}

void HeaderArea::adoptGlobalControls(GlobalControlArea* globalArea) {
    globalControls = globalArea;
    if (globalControls == nullptr) return;

    // Reparent the global transport buttons into this header
    addAndMakeVisible(globalControls->pauseAllButton);
    addAndMakeVisible(globalControls->startAllButton);
    addAndMakeVisible(globalControls->stopAllButton);
    addAndMakeVisible(globalControls->muteAllButton);
    addAndMakeVisible(globalControls->clearAllButton);
}

void HeaderArea::updateReadouts(const juce::String& sessionName, double tempo,
                                int tsNum, int tsDen, const juce::String& position) {
    projectNameLabel.setText(sessionName, juce::NotificationType::dontSendNotification);
    tempoValueLabel.setText(juce::String(tempo, 1), juce::NotificationType::dontSendNotification);
    meterValueLabel.setText(juce::String(tsNum) + "/" + juce::String(tsDen), juce::NotificationType::dontSendNotification);
    positionValueLabel.setText(position, juce::NotificationType::dontSendNotification);
}

void HeaderArea::paint (juce::Graphics& g){
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto accent = findColour(juce::TextButton::ColourIds::buttonOnColourId);
    auto btnSurf = findColour(juce::TextButton::ColourIds::buttonColourId);

    // Bottom divider
    g.setColour(accent.withAlpha(0.08f));
    g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());

    // Transport pill background (right side)
    if (globalControls != nullptr) {
        // Find the pill bounds from the first to last global button
        auto firstBtn = globalControls->pauseAllButton.getBoundsInParent();
        auto lastBtn  = globalControls->clearAllButton.getBoundsInParent();
        auto pillBounds = firstBtn.getUnion(lastBtn).expanded(4, 2).toFloat();

        g.setColour(btnSurf.withAlpha(0.3f));
        g.fillRoundedRectangle(pillBounds, 8.0f);
        g.setColour(accent.withAlpha(0.10f));
        g.drawRoundedRectangle(pillBounds, 8.0f, 0.5f);
    }
}

void HeaderArea::resized()
{
    auto bounds = getLocalBounds();

    // Menu bar at top (compact, 22px)
    menuBar->setBounds(bounds.removeFromTop(22));

    // Readout row below menu
    auto readoutRow = bounds;

    // Brand logo + project name to its right
    auto brandCol = readoutRow.removeFromLeft(200);
    brandLogo.setBounds(brandCol.reduced(0, 1));
    auto projNameCol = readoutRow.removeFromLeft(80);
    projectNameLabel.setBounds(projNameCol.withTrimmedTop(projNameCol.getHeight() - 16).reduced(2, 0));
    readoutRow.removeFromLeft(6);

    // Readout columns: Tempo, Meter, Position
    auto colWidth = 64;
    auto setupColumn = [&](juce::Label& title, juce::Label& value) {
        auto col = readoutRow.removeFromLeft(colWidth);
        auto top = col.removeFromTop(col.getHeight() / 2);
        title.setBounds(top.reduced(2, 0));
        value.setBounds(col.reduced(2, 0));
        readoutRow.removeFromLeft(4);
    };

    setupColumn(tempoTitleLabel, tempoValueLabel);
    setupColumn(meterTitleLabel, meterValueLabel);
    setupColumn(positionTitleLabel, positionValueLabel);

    // Global transport pill - right-aligned
    if (globalControls != nullptr) {
        auto btnW = juce::jmin(36, readoutRow.getWidth() / 9);

        // Transport buttons: right-aligned
        globalControls->clearAllButton.setBounds(readoutRow.removeFromRight(btnW).reduced(1, 2));
        globalControls->muteAllButton.setBounds(readoutRow.removeFromRight(btnW).reduced(1, 2));
        globalControls->stopAllButton.setBounds(readoutRow.removeFromRight(btnW).reduced(1, 2));
        globalControls->startAllButton.setBounds(readoutRow.removeFromRight(btnW).reduced(1, 2));
        globalControls->pauseAllButton.setBounds(readoutRow.removeFromRight(btnW).reduced(1, 2));
    }
}

void HeaderArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}

void HeaderArea::initCommandManager(){
    commandManager.reset(new ApplicationCommandManager());
    commandManager->registerAllCommandsForTarget(this);
    commandManager->setFirstCommandTarget(this);
}

ApplicationCommandTarget* HeaderArea::getNextCommandTarget(){
    return nullptr;
}

void HeaderArea::getAllCommands(Array<CommandID>& commands){
    const CommandID ids[] = { CommandIDs::newProject,
                              CommandIDs::open,
                              CommandIDs::saveProjectAs,
                              CommandIDs::saveProject,
                              CommandIDs::showProjectSettings
    };
    commands.addArray(ids, numElementsInArray(ids));
}

void HeaderArea::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result){
    switch (commandID)
    {
    case CommandIDs::newProject:
        result.setInfo("New", "Creates a new Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::ctrlModifier, 0));
        result.setActive(true);
        break;
    case CommandIDs::open:
        result.setInfo("Open", "Opens an existing Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('o', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::saveProject:
        result.setInfo("Save", "Saves the current Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('s', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::saveProjectAs:
        result.setInfo("Save As", "Saves the current Loopable project under a different name", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('a', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::showProjectSettings:
        result.setInfo("Settings", "Show the project settings", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('p', ModifierKeys::ctrlModifier, 0));
        break;

    default:
        break;
    }
}

bool HeaderArea::perform(const InvocationInfo& info){
    switch (info.commandID) {
    case CommandIDs::newProject:
        editor->createNewProject();
        break;
    case CommandIDs::open:
        editor->askUserToOpenFile();
        break;
    case CommandIDs::saveProject:
        editor->saveProject();
        break;
    case CommandIDs::saveProjectAs:
        editor->project.newProject = true;
        editor->saveProject();
        break;
    case CommandIDs::showProjectSettings:
        editor->showSettingsPage();
        break;
    default:
        return false;
    }
    return true;
}

MenuBarModel* HeaderArea::getMenuModel(){
    return mainMenu.get();
}

StringArray HeaderArea::getMenuNames(){
    return { "File",  "Settings" };
}

void HeaderArea::createMenu(PopupMenu& menu, const String& menuName){
    if (menuName == "File") createFileMenu(menu);
    else if (menuName == "Settings") createSettingsMenu(menu);
    else jassertfalse;
}

void HeaderArea::createFileMenu(PopupMenu& menu){
    menu.addCommandItem(commandManager.get(), CommandIDs::newProject);
    menu.addCommandItem(commandManager.get(), CommandIDs::open);
    menu.addCommandItem(commandManager.get(), CommandIDs::saveProjectAs);
    menu.addCommandItem(commandManager.get(), CommandIDs::saveProject);
    menu.addSeparator();
#if ! JUCE_MAC
#endif
}

void HeaderArea::createEditMenu(PopupMenu& menu){
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::undo);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::redo);
    menu.addSeparator();
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::cut);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::copy);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::paste);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::del);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::selectAll);
    menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::deselectAll);
}

void HeaderArea::createSettingsMenu(PopupMenu& menu){
    menu.addCommandItem(commandManager.get(), CommandIDs::showProjectSettings);
}
