/*
  ==============================================================================

    HeaderArea.cpp
    Created: 1 Aug 2020 12:47:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HeaderArea.h"
#include "PluginEditor.h"

//==============================================================================
HeaderArea::HeaderArea(){
    initCommandManager();
    mainMenu = std::make_unique<MainMenu>(this);
    MenuBarModel* mm = mainMenu.get();
    menuBar = std::make_unique<MenuBarComponent>(mm);
    addAndMakeVisible(menuBar.get());
}

HeaderArea::~HeaderArea(){
}

MenuManager::~MenuManager(){
}


void HeaderArea::paint (juce::Graphics& g){

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::blue);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("HeaderArea", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void HeaderArea::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    menuBar->setBounds(getLocalBounds());
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
        result.setInfo("New Project...", "Creates a new Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('n', ModifierKeys::ctrlModifier, 0));
        result.setActive(true);
        break;
    case CommandIDs::open:
        result.setInfo("Open Project...", "Opens an existing Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('o', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::saveProject:
        result.setInfo("Save Project...", "Saves the current Loopable project", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('s', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::saveProjectAs:
        result.setInfo("Save Project As...", "Saves the current Loopable project under a different name", CommandCategories::general, 0);
        result.defaultKeypresses.add(KeyPress('a', ModifierKeys::ctrlModifier, 0));
        break;
    case CommandIDs::showProjectSettings:
        result.setInfo("Show Settings...", "Show the project settings", CommandCategories::general, 0);
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
    else jassertfalse; // names have changed?
}


void HeaderArea::createFileMenu(PopupMenu& menu){
    menu.addCommandItem(commandManager.get(), CommandIDs::newProject);
    menu.addCommandItem(commandManager.get(), CommandIDs::open);
    menu.addCommandItem(commandManager.get(), CommandIDs::saveProjectAs);
    menu.addCommandItem(commandManager.get(), CommandIDs::saveProject);
    menu.addSeparator();

#if ! JUCE_MAC
    /*
        menu.addCommandItem(commandManager.get(), CommandIDs::showAboutWindow);
        menu.addCommandItem(commandManager.get(), CommandIDs::showAppUsageWindow);
        menu.addCommandItem(commandManager.get(), CommandIDs::checkForNewVersion);
        menu.addCommandItem(commandManager.get(), CommandIDs::showGlobalPathsWindow);
        menu.addSeparator();
        */
        //menu.addCommandItem(commandManager.get(), StandardApplicationCommandIDs::quit);
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
