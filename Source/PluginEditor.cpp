/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "../ffAudio/ff_meters.h"


//==============================================================================
OrbishAudioProcessorEditor::OrbishAudioProcessorEditor (OrbishAudioProcessor& p, AudioProcessorValueTreeState& apvts)
: AudioProcessorEditor (&p),                              // [4]
projectXml("<project />"), processor (p), thumbnailCache (5), thumbnail (32, formatManager, thumbnailCache), valueTreeState(apvts)
{
	//processor.logger->logMessage("begin gui constructor");
    openGLContext.attachTo(*getTopLevelComponent());
	project = Project();
	setProjectName(project.name);
	headerComp.addAndMakeVisible(projectLabel);
	initCommandManager();
	mainMenu = std::make_unique<MainMenu>(this);
	MenuBarModel* mm = mainMenu.get();
	menuBar = std::make_unique<MenuBarComponent>(mm);
	addAndMakeVisible(menuBar.get());
	addAndMakeVisible(headerComp);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    auto globalLnF = new LookAndFeel_V3();
    setLookAndFeel(globalLnF);
    getLookAndFeel().setColour(ResizableWindow::backgroundColourId, Colours::black);
    globalLnF->setColour(Label::textColourId, Colours::white);

    updateInputBuffer = &Observer::updateInputVisualiser;
    updateLoopBuffer = &Observer::updateLoopVisualiser;
    updatePlayPosition = &Observer::updatePlayHead;
    handleMidi = &Observer::handleMidiMessages;
    newTrack = &Observer::askToCreateTrack;
    trackChange = &Observer::updateNextTrackNumber;
    trackRemoval = &Observer::removeTrack;
    
    newLoop = &Observer::askToCreateLoop;
    loopChange = &Observer::updateNextLoopNumber;
    loopRemoval = &Observer::removeLoop;

    
    processor.context->observer = this;
    thumbnail.addChangeListener (this);
    startTimer (20);
   
    lnf = std::make_shared<FFAU::LevelMeterLookAndFeel>();
    lnf->setColour (FFAU::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    inputMeter = std::make_shared<FFAU::LevelMeter> (); // See FFAU::LevelMeter::MeterFlags for options
    inputMeter->setLookAndFeel (lnf.get());
    inputMeter->setMeterSource (processor.getInputMeterSource());
    inputMeter->setMeterFlags(FFAU::LevelMeter::Minimal);
    
    outputMeter = std::make_shared<FFAU::LevelMeter>(); // See FFAU::LevelMeter::MeterFlags for options
    outputMeter->setLookAndFeel (lnf.get());
    outputMeter->setMeterSource (processor.getOutputMeterSource());
    outputMeter->setMeterFlags(FFAU::LevelMeter::Minimal);
   for (auto track : processor.tracks) {
       doCreateTrack(track->Index);
    }
    timeSigLabel.setTooltip("Current time signature\nComes from the host");

    
	Array<StringArray> buttonRecModeNames;
	buttonRecModeNames.add({ "Overdub", "Record multiple layers within same loop" });
	buttonRecModeNames.add({ "Repeat","Record additional layer while extending loop size repeating previous layers" });
	buttonRecModeNames.add({ "Append","Record new layer while extending without repeating previous layers" });
	buttonRecModeNames.add({ "Overwrite", "overwrite content of current layer with new material, and extend original loop until recording ends" });
	buttonRecModeNames.add({ "Punch","Record while replacing previous material on current layer" });

	Array<StringArray> buttonSnapModeNames;
	buttonSnapModeNames.add({ "No Sync", "Snap disabled, functionality goes into effect instantly" });
	buttonSnapModeNames.add({ "Bar","Snaps to bar" });
	buttonSnapModeNames.add({ "Beat","Snaps to the beat (bottom of time signature)" });

    recModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "mode", recModeCombo));
    snapModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "snap", snapModeCombo));

	for (int i = 0; i < buttonRecModeNames.size(); ++i)
	{
		recModeCombo.addItem(buttonRecModeNames[i][0], i + 1);

	}
    for (int i = 0; i < buttonSnapModeNames.size(); ++i)
    {
        snapModeCombo.addItem(buttonSnapModeNames[i][0], i+1);

    }
    recModeCombo.setSelectedId(1);
     String str = String("Overdub: Record multiple layers within same loop\n")    +
                  String("Repeat: Record additional layer while extending loop size repeating previous layers\n") +
                  String("Append: Record new layer while extending without repeating previous layers") +
                  String("Overwrite: overwrite content of current layer with new material, and extend original loop until recording ends\n") +
                  String("Punch: Record while replacing previous material on current layer\n");
    recModeCombo.setTooltip(str);
    snapModeCombo.setSelectedId(2);
    str = String("No Sync: Snap disabled, functionality goes into effect instantly\n")    +
                 String("Bar: Snaps to bar\n") +
                 String("Beat: Snaps to the beat (bottom of time signature)");
   snapModeCombo.setTooltip(str);
    
    activeLabel.setText("Active Track - OpenGL mode", NotificationType::dontSendNotification);
    recordButton.setClickingTogglesState (true);
    recordButton.setToggleState(false, NotificationType::sendNotification);
    recordButton.addListener(this);
    recordAttachment.reset (new ButtonAttachment (valueTreeState, "record", recordButton));
    recordButton.setTooltip("Start/Stop recording on the active track");
    playButton.setToggleState(false, NotificationType::sendNotification);
    playButton.setClickingTogglesState (true);
    playButton.addListener(this);
    playAttachment.reset (new ButtonAttachment (valueTreeState, "play", playButton));
    playButton.setTooltip("Play/Pause the recorded material on the active track or group");

    globalLabel.setText("Global", NotificationType::dontSendNotification);

    muteAllButton.setToggleState(false, NotificationType::sendNotification);
    muteAllButton.setClickingTogglesState (true);
    muteAllButton.addListener(this);
    muteAllAttachment.reset (new ButtonAttachment (valueTreeState, "muteAll", muteAllButton));
    muteAllButton.setTooltip("Mute all tracks");

    
    stopAllButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    stopAllButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    stopAllButton.setToggleState(false, NotificationType::sendNotification);
    stopAllButton.setClickingTogglesState (true);
    stopAllButton.addListener(this);
    stopAllAttachment.reset (new ButtonAttachment (valueTreeState, "stopAll", stopAllButton));
    stopAllButton.setTooltip("Stop playing on all tracks");

 
    startAllButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    startAllButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    startAllButton.setToggleState(false, NotificationType::sendNotification);
    startAllButton.setClickingTogglesState (true);
    startAllButton.addListener(this);
    startAllAttachment.reset (new ButtonAttachment (valueTreeState, "startAll", startAllButton));
    startAllButton.setTooltip("Start playing on all tracks");

    pauseAllButton.setToggleState(false, NotificationType::sendNotification);
    pauseAllButton.setClickingTogglesState (true);
    pauseAllButton.addListener(this);
    pauseAllAttachment.reset (new ButtonAttachment (valueTreeState, "pauseAll", pauseAllButton));
    pauseAllButton.setTooltip("Pause all tracks");

  
    clearAllButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    clearAllButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    clearAllButton.setToggleState(false, NotificationType::sendNotification);
    clearAllButton.setClickingTogglesState (true);
    clearAllButton.addListener(this);
    clearAllAttachment.reset (new ButtonAttachment (valueTreeState, "resetAll", clearAllButton));
    clearAllButton.setTooltip("Clear all tracks");


	addToGroupButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
	addToGroupButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
	addToGroupButton.setToggleState(false, NotificationType::sendNotification);
	addToGroupButton.setClickingTogglesState(true);
	addToGroupButton.addListener(this);
	addToGroupAttachment.reset(new ButtonAttachment(valueTreeState, "addToGroup", addToGroupButton));
    addToGroupButton.setTooltip("Add the active track to the selected group");


	removeFromGroupButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
	removeFromGroupButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
	removeFromGroupButton.setToggleState(false, NotificationType::sendNotification);
	removeFromGroupButton.setClickingTogglesState(true);
	removeFromGroupButton.addListener(this);
	removeFromGroupAttachment.reset(new ButtonAttachment(valueTreeState, "removeFromGroup", removeFromGroupButton));
    removeFromGroupButton.setTooltip("Remove the active track from the selected group");

    horizontalOutline = new DrawableComposite();
    for(int i=0;i<3;++i){
        Path pth;
        auto* line = new DrawablePath();
        pth.addLineSegment(Line<float>{2,float(i) * 8,30,float(i) * 8}, 4);
        line->setPath(pth);
        horizontalOutline->addChildComponent(line);
    }
    
    verticalOutline = new DrawableComposite();
    for(int i=0;i<6;++i){
        Path pth;
        auto* block = new DrawablePath();
        pth.addRectangle(Rectangle<int>{i%3*10,i/3 * 13,5,7});
        block->setPath(pth);
        verticalOutline->addChildComponent(block);
    }
    
    tracksLayoutButton = new DrawableButton( "Layout", DrawableButton::ImageFitted);
    tracksLayoutButton->setColour(DrawableButton::backgroundOnColourId, Colour(0x40FFFFFF));
    tracksLayoutButton->setColour(DrawableButton::backgroundColourId, Colour(0x40FFFFFF));

    tracksLayoutButton->setImages( horizontalOutline, horizontalOutline, horizontalOutline,nullptr, verticalOutline, verticalOutline, verticalOutline, nullptr);
    tracksLayoutButton->setTooltip ("Toggles between horizontal and vertical layout of the tracks");
    tracksLayoutButton->setToggleState(true, NotificationType::dontSendNotification);
    tracksLayoutButton->setClickingTogglesState (true);
    tracksLayoutButton->addListener(this);
    
    stopButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    stopButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    stopButton.setToggleState(false, NotificationType::sendNotification);
    stopButton.setClickingTogglesState (true);
    stopButton.addListener(this);
    stopAttachment.reset (new ButtonAttachment (valueTreeState, "stop", stopButton));
    stopButton.setTooltip("Stop playing on the active track or group");
    
    clearButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    clearButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    clearButton.setToggleState(false, NotificationType::sendNotification);
    clearButton.setClickingTogglesState (true);
    clearButton.addListener(this);
    clearAttachment.reset (new ButtonAttachment (valueTreeState, "reset", clearButton));
    clearButton.setTooltip("Clear the active track");
    
    muteButton.setToggleState(false, NotificationType::sendNotification);
    muteButton.setClickingTogglesState (true);
    muteButton.addListener(this);
    muteAttachment.reset (new ButtonAttachment (valueTreeState, "mute", muteButton));
    muteButton.setTooltip("Mute the active track or group");
    
    soloButton.setToggleState(false, NotificationType::sendNotification);
    soloButton.setClickingTogglesState (true);
    soloButton.addListener(this);
    soloAttachment.reset (new ButtonAttachment (valueTreeState, "solo", soloButton));
    soloButton.setTooltip("Play exclusively the active track or group");
    
    monitorButton.setToggleState(false, NotificationType::sendNotification);
    monitorButton.setClickingTogglesState (true);
    monitorButton.addListener(this);
    monitorAttachment.reset (new ButtonAttachment (valueTreeState, "monitor", monitorButton));
    monitorButton.setTooltip("Listen to input audio when this track is active");
    
    reverseButton.setToggleState(false, NotificationType::sendNotification);
    reverseButton.setClickingTogglesState (true);
    reverseButton.addListener(this);
    reverseAttachment.reset (new ButtonAttachment (valueTreeState, "reverse", reverseButton));
    reverseButton.setTooltip("Reverse the audio on the active track");

    
    undoButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    undoButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    undoButton.setToggleState(true, NotificationType::sendNotification);
    undoButton.setClickingTogglesState (true);
    undoButton.addListener(this);
    undoAttachment.reset (new ButtonAttachment (valueTreeState, "undo", undoButton));
    undoButton.setTooltip("Undo latest overdub recording");

    
    redoButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    redoButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    redoButton.setToggleState(true, NotificationType::sendNotification);
    redoButton.setClickingTogglesState (true);
    redoButton.addListener(this);
    redoAttachment.reset (new ButtonAttachment (valueTreeState, "redo", redoButton));
    redoButton.setTooltip("Redo latest overdub recording");

    bounceButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    bounceButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    bounceButton.setClickingTogglesState(true);
    bounceButton.setToggleState(true, NotificationType::sendNotification);
    bounceAttachment.reset(new ButtonAttachment(valueTreeState, "bounce", bounceButton));
    bounceButton.setTooltip("Flatten all overdub layers to one");
    
    autoTriggerButton.setClickingTogglesState (true);
    autoTriggerButton.setToggleState(false, NotificationType::sendNotification);
    triggerAttachment.reset (new ButtonAttachment (valueTreeState, "trigger", autoTriggerButton));
    autoTriggerButton.setTooltip("Start recording when input signal exceeds the predefined threshold");

    loopLabel.setText("Loops: ", NotificationType::dontSendNotification);
    
    previousLoopButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    previousLoopButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    previousLoopButton.setToggleState(false, NotificationType::sendNotification);
    previousLoopButton.setClickingTogglesState(true);
    previousLoopButton.addListener(this);
    previousLoopAttachment.reset (new ButtonAttachment (valueTreeState, "previousLoop", previousLoopButton));
    previousLoopButton.setTooltip("Go to previous loop on active track or group");

    
    activeLoopLabel.setText(String(activeLoop + 1), NotificationType::dontSendNotification);
    
    nextLoopButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    nextLoopButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    nextLoopButton.setToggleState(false, NotificationType::sendNotification);
    nextLoopButton.setClickingTogglesState(true);
    nextLoopButton.addListener(this);
    nextLoopAttachment.reset (new ButtonAttachment (valueTreeState, "nextLoop", nextLoopButton));
    nextLoopButton.setTooltip("Go to next loop on active track or group");

    
    newLoopButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    newLoopButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    newLoopButton.setToggleState(false, NotificationType::dontSendNotification);
    newLoopButton.setClickingTogglesState(true);
    newLoopAttachment.reset (new ButtonAttachment (valueTreeState, "newLoop", newLoopButton));
    newLoopButton.setTooltip("Create new loop on active track");

    
    removeLoopButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    removeLoopButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    removeLoopButton.setToggleState(false, NotificationType::dontSendNotification);
    removeLoopButton.setClickingTogglesState(true);
    removeLoopAttachment.reset (new ButtonAttachment (valueTreeState, "removeLoop", removeLoopButton));
    removeLoopButton.setTooltip("Remove latest loop from active track");

    
    globalVolumeSlider.setValue(0);
    
    loopConfigArea.addAndMakeVisible(loopLabel);
    loopConfigArea.addAndMakeVisible(previousLoopButton);
    loopConfigArea.addAndMakeVisible(activeLoopLabel);
    loopConfigArea.addAndMakeVisible(nextLoopButton);
    loopConfigArea.addAndMakeVisible(newLoopButton);
    loopConfigArea.addAndMakeVisible(removeLoopButton);
    
    trackLabel.setText("Tracks: ", NotificationType::dontSendNotification);
    
    previousTrackButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    previousTrackButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    previousTrackButton.setToggleState(false, NotificationType::sendNotification);
    previousTrackButton.setClickingTogglesState(true);
    previousTrackButton.addListener(this);
    previousTrackAttachment.reset (new ButtonAttachment (valueTreeState, "previousTrack", previousTrackButton));
    previousTrackButton.setTooltip("Go to previous track");

    
    activeTrackLabel.setText(String(activeTrack + 1), NotificationType::dontSendNotification);
    
    nextTrackButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    nextTrackButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    nextTrackButton.setToggleState(false, NotificationType::sendNotification);
    nextTrackButton.setClickingTogglesState(true);
    nextTrackButton.addListener(this);
    nextTrackAttachment.reset (new ButtonAttachment (valueTreeState, "nextTrack", nextTrackButton));
    nextTrackButton.setTooltip("Go to next track");

    
    newTrackButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    newTrackButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    newTrackButton.setToggleState(false, NotificationType::dontSendNotification);
    newTrackButton.setClickingTogglesState(true);
    newTrackAttachment.reset (new ButtonAttachment (valueTreeState, "newTrack", newTrackButton));
    newTrackButton.setTooltip("Create new track");

    
    removeTrackButton.setColour(TextButton::textColourOnId, globalLnF->findColour(TextButton::textColourOffId));
    removeTrackButton.setColour(TextButton::buttonOnColourId, globalLnF->findColour(TextButton::buttonColourId));
    removeTrackButton.setToggleState(false, NotificationType::dontSendNotification);
    removeTrackButton.setClickingTogglesState(true);
    removeTrackAttachment.reset (new ButtonAttachment (valueTreeState, "removeTrack", removeTrackButton));
    removeTrackButton.setTooltip("Remove latest track");

    globalVolumeSlider.setValue(0);
    
    
    transportButtonArea.addAndMakeVisible(activeLabel);
    transportButtonArea.addAndMakeVisible(recordButton);
    transportButtonArea.addAndMakeVisible(playButton);
    transportButtonArea.addAndMakeVisible(stopButton);
    transportButtonArea.addAndMakeVisible(clearButton);
    transportButtonArea.addAndMakeVisible(muteButton);
    transportButtonArea.addAndMakeVisible(soloButton);
    transportButtonArea.addAndMakeVisible(monitorButton);
    transportButtonArea.addAndMakeVisible(reverseButton);
    transportButtonArea.addAndMakeVisible(undoButton);
    transportButtonArea.addAndMakeVisible(redoButton);
    transportButtonArea.addAndMakeVisible(autoTriggerButton);
	transportButtonArea.addAndMakeVisible(bounceButton);

    transportButtonArea.addAndMakeVisible(loopLabel);
    transportButtonArea.addAndMakeVisible(previousLoopButton);
    transportButtonArea.addAndMakeVisible(activeLoopLabel);
    transportButtonArea.addAndMakeVisible(nextLoopButton);
    transportButtonArea.addAndMakeVisible(newLoopButton);
    transportButtonArea.addAndMakeVisible(removeLoopButton);

    loopConfigArea.addAndMakeVisible(globalLabel);
    loopConfigArea.addAndMakeVisible(muteAllButton);
    loopConfigArea.addAndMakeVisible(stopAllButton);
    loopConfigArea.addAndMakeVisible(startAllButton);
    loopConfigArea.addAndMakeVisible(pauseAllButton);
    loopConfigArea.addAndMakeVisible(clearAllButton);
	loopConfigArea.addAndMakeVisible(addToGroupButton);
	loopConfigArea.addAndMakeVisible(removeFromGroupButton);

    loopConfigArea.addAndMakeVisible(trackLabel);
    loopConfigArea.addAndMakeVisible(previousTrackButton);
    loopConfigArea.addAndMakeVisible(activeTrackLabel);
    loopConfigArea.addAndMakeVisible(nextTrackButton);
    loopConfigArea.addAndMakeVisible(newTrackButton);
    loopConfigArea.addAndMakeVisible(removeTrackButton);
    loopConfigArea.addAndMakeVisible(tracksLayoutButton);
    
    inputDisplay.setSamplesPerBlock(processor.context->maxBlockSize);
    inputDisplay.setBufferSize(processor.context->samplesPerBlock);
    inputDisplay.setColours(Colours::darkgrey, Colours::indianred);

	groupLabel.setText("Group select: ", NotificationType::dontSendNotification);
	headerComp.addAndMakeVisible(groupLabel);
	groupAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(valueTreeState, "selectGroup", groupCombo));

	groupColours.add(Colours::aqua);
	groupColours.add(Colours::coral);
	groupColours.add(Colours::crimson);
	groupColours.add(Colours::lemonchiffon);
	groupColours.add(Colours::darkturquoise);
	groupColours.add(Colours::fuchsia);
	groupColours.add(Colours::orange);
	groupColours.add(Colours::darksalmon);
	groupColours.add(Colours::violet);
	groupColours.add(Colours::cornflowerblue);

    for (auto group: processor.groups)
	{
		groupCombo.addItem(group->Name, group->Index + 1);
	}
	groupCombo.setSelectedId(1);
	headerComp.addAndMakeVisible(groupCombo);
    groupCombo.setTooltip("Select a group, then add or remove tracks. \nAll tracks in the same group will act simultaneously for certain commands");
    inputLevelSlider.setNumDecimalPlacesToDisplay(1);
    
    inputLevelSlider.setTextBoxIsEditable(true);
    inputLevelSlider.setTextValueSuffix(" db");
    inputLevelSlider.addListener(this);
    inputLevelAttachment.reset (new SliderAttachment (valueTreeState, "inputLevel", inputLevelSlider));
    inputLevelSlider.setSliderStyle (Slider::LinearBarVertical);
    inputLevelSlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
    inputLevelSlider.getLookAndFeel().setColour(Slider::backgroundColourId, Colours::white);
    inputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    inputLevelSlider.setPopupDisplayEnabled (true, false, this);
    leftInnerSide.addAndMakeVisible(inputLevelSlider);
    inputLevelLabel.setText("Input Level", NotificationType::dontSendNotification);
    inputLevelLabel.attachToComponent(&inputLevelSlider, false);
    inputLevelSlider.setTooltip("Adjust the level of the input signal for the active track");
    leftInnerSide.addAndMakeVisible(inputSliderComp);
    
    inputLevelSlider.textFromValueFunction = [this] (double val)
        {
                return String (val, 1);
        };

    outputLevelSlider.setRange(-60, 6);
    outputLevelSlider.setNumDecimalPlacesToDisplay(1);
    outputLevelSlider.setTextBoxIsEditable(true);
    outputLevelSlider.setTextValueSuffix(" db");
    outputLevelSlider.addListener(this);
    outputLevelAttachment.reset (new SliderAttachment (valueTreeState, "outputLevel", outputLevelSlider));
    
    outputLevelSlider.setSliderStyle (Slider::LinearBarVertical);
    outputLevelSlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
    outputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    outputLevelSlider.setPopupDisplayEnabled (true, false, this);
    rightInnerSide.addAndMakeVisible(outputLevelSlider);
    outputLevelLabel.setText("Output Level", NotificationType::dontSendNotification);
    outputLevelLabel.attachToComponent(&outputLevelSlider, false);
    outputLevelSlider.textFromValueFunction = [this] (double val)
        {
                return String (val, 1);
        };
    outputLevelSlider.setTooltip("Adjust the level of the output signal for the active track");

    rightInnerSide.addAndMakeVisible(outputSliderComp);
    
    globalVolumeSlider.setRange(-120, 6);
    globalVolumeSlider.setNumDecimalPlacesToDisplay(2);
    globalVolumeSlider.setTextBoxIsEditable(true);
    globalVolumeSlider.setTextValueSuffix(" db");
    globalVolumeSlider.addListener(this);
    globalMixAttachment.reset (new SliderAttachment (valueTreeState, "globalMix", globalVolumeSlider));
    globalVolumeSlider.setSliderStyle (Slider::LinearBarVertical);
    globalVolumeSlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
    globalVolumeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    globalVolumeSlider.setPopupDisplayEnabled (true, false, this);
    rightInnerSide.addAndMakeVisible(globalVolumeSlider);
    globalVolumeLabel.setText("Global Level", NotificationType::dontSendNotification);
    globalVolumeSlider.textFromValueFunction = [this] (double val)
        {
                return String (val, 1);
        };
    globalVolumeLabel.attachToComponent(&globalVolumeSlider, false);
    globalVolumeSlider.setTooltip("Adjust the level of the general output signal");

    rightInnerSide.addAndMakeVisible(globalSliderComp);
    leftSide.addAndMakeVisible(inputMeter.get());
    rightSide.addAndMakeVisible(outputMeter.get());
    headerComp.addAndMakeVisible(timeSigLabel);
    headerComp.addAndMakeVisible(bpmLabel);
    headerComp.addAndMakeVisible(progressLabel);
    headerComp.addAndMakeVisible(trackNumberLabel);
    headerComp.addAndMakeVisible(loopNumberLabel);
    headerComp.addAndMakeVisible(layerNumberLabel);
    headerComp.addAndMakeVisible(groupNumberLabel);
    headerComp.addAndMakeVisible(barWitness);
    headerComp.addAndMakeVisible(beatWitness);
    transportButtonArea.addAndMakeVisible(recModeCombo);
    transportButtonArea.addAndMakeVisible(snapModeCombo);
    transportButtonArea.addAndMakeVisible(recModeLabel);
    transportButtonArea.addAndMakeVisible(snapModeLabel);
    setSize (1100, 620);

}


ApplicationCommandTarget* OrbishAudioProcessorEditor::getNextCommandTarget() { return nullptr; };

void OrbishAudioProcessorEditor::getAllCommands(Array< CommandID >& commands) {
	const CommandID ids[] = { CommandIDs::newProject,
							  CommandIDs::open,
							  CommandIDs::saveProjectAs,
							  CommandIDs::saveProject,
							  CommandIDs::showProjectSettings
	};



	commands.addArray(ids, numElementsInArray(ids));
}

void OrbishAudioProcessorEditor::showSettingsPage() {
	settingsPage = std::make_shared<SettingsPage>(processor.loggingActive
		, processor.context->maxUndoHistory
        , nbrTracksInARow
		, processor.context->delayCompensation );
	settingsPage->addListener(this);
	addAndMakeVisible(*settingsPage);
	resized();
}

void OrbishAudioProcessorEditor::closeSettingsPage() {
	settingsPage.reset();
}


void OrbishAudioProcessorEditor::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) {
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

bool OrbishAudioProcessorEditor::perform(const InvocationInfo& info) {
	switch (info.commandID) {
	case CommandIDs::newProject:                createNewProject();
		break;
	case CommandIDs::open:                      askUserToOpenFile();
		break;
	case CommandIDs::saveProject:               saveProject();
		break;
	case CommandIDs::saveProjectAs:              project.newProject = true; saveProject();
		break;
	case CommandIDs::showProjectSettings:     showSettingsPage();
		break;
	default:                                    return false;
	}
	return true;
}

void OrbishAudioProcessorEditor::createNewProject() {
	if (project.dirty) {
		bool save = showDialogWindow("Save current project!"
			, "Do you want save the current project?"
			, AlertWindow::AlertIconType::QuestionIcon
			, "Yes", "No");
		if (save) { saveProject(); };
	}
	const juce::String title = "New Orbish Project";
	const juce::String msg = juce::String("Give your new project a name!");
	AlertWindow* alert = new AlertWindow(title, msg, AlertWindow::AlertIconType::InfoIcon, this);
	alert->addTextEditor("projectNameBox", "Untitled");
	auto box = alert->getTextEditor("projectNameBox");
	box->setWantsKeyboardFocus(true);
	alert->setBoundsRelative(0.2f, 0.2f, 0.2f, 0.2f);
	alert->setLookAndFeel(lnf.get());
	alert->addButton("Ok", 1, juce::KeyPress(KeyPress::returnKey));
	alert->isDropShadowEnabled();
	alert->enterModalState(true, nullptr, false);
	addAndMakeVisible(alert);
	alert->centreWithSize(300, 200);
	if (alert->runModalLoop() != 0) // is they picked 'ok'
	{
		setProjectName(alert->getTextEditorContents("projectNameBox"));
		alert->exitModalState(0);
		alert->setVisible(false);
	}
	int zero = 0;
	processor.changeTrack(zero);
	changeTrack();
	for (auto i = tracks.size() - 1;i > 0;i--) {
		processor.removeTrack(i);
		doRemoveTrack();
		makeTracks();
	}
	processor.activeTrack->ChangeLoopBefore(zero);
	processor.activeTrack->ChangeLoopAfter();
    while (processor.tracks[0]->loops.size() > 1){
		processor.tracks[0]->RemoveLoopAfter();
		makeTracks();
	}
    while (processor.tracks[0]->Layers->size() > 1){
		processor.tracks[0]->RemoveTopLayer();
		makeTracks();
	}
	processor.initGroups();
	processor.activeTrack->processResetChange();
    groupCombo.setSelectedId(1);
	project.newProject = true;
	project.dirty = true;
}
void OrbishAudioProcessorEditor::setProjectName(String name) {
	projectLabel.setText(name, NotificationType::dontSendNotification);
	project.name = name;
}
void OrbishAudioProcessorEditor::saveProject() {
	if (!project.dirty && !project.newProject)return;
	if (project.newProject || !project.directory.exists()) {
		File dir = File(File::getSpecialLocation(File::userHomeDirectory));
		if (dir.getChildFile("Orbish").exists()) {
			dir = dir.getChildFile("Orbish");
		}
		FileChooser fc("Save As", dir,"",true);
		if (fc.showDialog(
            FileBrowserComponent::FileChooserFlags::canSelectDirectories
            | FileBrowserComponent::FileChooserFlags::saveMode
			, nullptr))
			project.directory = fc.getResult();
        setProjectName(project.directory.getFileName());	//	
		if (!project.directory.exists()) {
			juce::Result result = project.directory.createDirectory();
			if (result.failed()) {
				processor.logMessage(result.getErrorMessage());
			}
		}
	}

	loopTree = std::make_unique<ValueTree>(String("project"));
	loopTree->setProperty("name", project.name, nullptr);

	std::unique_ptr<ValueTree> tsv(std::make_unique<ValueTree>(String("tracks")));
	for (auto track: tracks) {
		ValueTree tv(String("track")); 
		tv.setProperty("index", track->getIndex(), nullptr);
		tv.setProperty("name", track->getName(), nullptr);
		for (auto loop: track->Loops) {
			ValueTree lv(String("loop"));
			for (auto k = 0;k <= track->getAudioTrack()->loops[loop->getIndex()]->CurrentTop;++k) {
				auto result = saveBuffer(track->getIndex(), loop->getIndex(), k, project.directory
					, track->getName() + "_" + String(loop->getIndex()) + "_" + String(k));
				ValueTree lyv(String("layer"));
				lyv.setProperty("index", k, nullptr);
				lyv.setProperty("file", result , nullptr);
				lv.addChild(lyv, k, nullptr);
			}
			lv.setProperty("index", loop->getIndex(), nullptr);
			tv.addChild(lv, loop->getIndex(), nullptr);
		}
		tv.appendChild(*track->getAudioTrack()->state, nullptr);
		tsv->addChild(tv, track->getIndex(), nullptr);
	}
	loopTree->appendChild(*tsv, nullptr);
	std::unique_ptr<ValueTree> gsv(std::make_unique<ValueTree>(String("groups")));
	for (auto grp: processor.groups) {
		//auto grp = processor.groups[i];
        std::unique_ptr<ValueTree> g(std::make_unique<ValueTree>(String("group")));
        g->setProperty("name", grp->Name, nullptr);
        g->setProperty("index", grp->Index, nullptr);
        gsv->appendChild(*g, nullptr);
		for (auto trk: *grp) {
			//auto trk = (*grp)[j];
			ValueTree gtv(String("groupedTrack"));
			gtv.setProperty("index", int(trk->Index), nullptr);
			g->appendChild(gtv, nullptr);
		}
	}
	loopTree->appendChild(*gsv, nullptr);
	std::unique_ptr<XmlElement> el = loopTree->createXml();
	el->writeTo(project.directory.getChildFile(project.name + ".xml")
                , XmlElement::TextFormat());
	project.newProject = false;
	project.dirty = false;
}


bool OrbishAudioProcessorEditor::showDialogWindow(
	String title
	, String message
	, AlertWindow::AlertIconType icon
	, String firstButtonText
	, String secondButtonText)
{
	AlertWindow* alert = new AlertWindow(title, message, icon, this);
	alert->setLookAndFeel(lnf.get());
	alert->addButton(firstButtonText, 1, juce::KeyPress(KeyPress::returnKey));
	alert->addButton(secondButtonText, 0, juce::KeyPress(KeyPress::escapeKey));
	alert->isDropShadowEnabled();
	alert->enterModalState(true, nullptr, false);
	addAndMakeVisible(alert);
	alert->centreWithSize(400, 200);
	if (alert->runModalLoop() != 0) // is they picked 'ok'
	{
		alert->exitModalState(0);
		alert->setVisible(false);
		return true;
	}
	else {
		alert->exitModalState(0);
		alert->setVisible(false);
		return false;
	}
}

void OrbishAudioProcessorEditor::askUserToOpenFile() {
	if (project.dirty) {
		bool save = showDialogWindow("Save current project!"
			, "Do you want to save the current project?"
			, AlertWindow::AlertIconType::QuestionIcon
			, "Yes", "No");
		if (save) { saveProject(); };
	}
	int zero = 0;
	processor.changeTrack(zero);
	changeTrack();
    
	for (auto i = processor.tracks.size() - 1;i > 0;i--) {
		processor.removeTrack(i);
		doRemoveTrack();
		makeTracks();
	}
	processor.activeTrack->ChangeLoopBefore(zero);
	processor.activeTrack->ChangeLoopAfter();
    while (processor.tracks[0]->loops.size() > 1){
		processor.tracks[0]->RemoveLoopAfter();
		makeTracks();
	}
    while (processor.tracks[0]->Layers->size() > 1){
		processor.tracks[0]->RemoveTopLayer();
		makeTracks();
	}
	processor.initGroups();
    groupCombo.setSelectedId(1);
	processor.activeTrack->processResetChange();
	File dir = File(File::getSpecialLocation(File::userHomeDirectory));
	if (dir.getChildFile("Orbish").exists()) {
		dir = dir.getChildFile("Orbish");
	}
	FileChooser fc("Open Project", dir, "*.xml");
	if (fc.browseForFileToOpen())
		openFile(fc.getResult());
	project.newProject = false;
	project.dirty = false;
}

bool OrbishAudioProcessorEditor::openFile(const File& file) {
	loadingTracks = true;
	loopTree = std::make_shared<ValueTree>(String("project"));
	if (auto xml = parseXML(file)) {
		loopTree = std::make_shared<ValueTree>(ValueTree::fromXml(*xml));
		setProjectName(loopTree->getProperty("name"));
		try {
			processor.loadFromValueTree(loopTree.get());
		}
		catch (...) {
			processor.logMessage("Problem loading file");
		}
		for (int i = 0;i < processor.context->trackCount; ++i) {
			doCreateTrack(i);
			tracks[activeTrack]->setActive(true);
		}
	}
	return false;
}

void  OrbishAudioProcessorEditor::initCommandManager() {
	commandManager.reset(new ApplicationCommandManager());
	commandManager->registerAllCommandsForTarget(this);
	commandManager->setFirstCommandTarget(this);
}
MenuBarModel* OrbishAudioProcessorEditor::getMenuModel() {
	return mainMenu.get();
}

StringArray OrbishAudioProcessorEditor::getMenuNames() { return { "File",  "Settings" }; }

void OrbishAudioProcessorEditor::createMenu(PopupMenu& menu, const String& menuName) {
	if (menuName == "File")             createFileMenu(menu);
	//else if (menuName == "Edit")        createEditMenu(menu);
	else if (menuName == "Settings")        createSettingsMenu(menu);
	else                                jassertfalse; // names have changed?
}

void OrbishAudioProcessorEditor::createFileMenu(PopupMenu& menu) {
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

void OrbishAudioProcessorEditor::createEditMenu(PopupMenu& menu) {
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

void OrbishAudioProcessorEditor::createSettingsMenu(PopupMenu& menu)
{
	menu.addCommandItem(commandManager.get(), CommandIDs::showProjectSettings);
}

void OrbishAudioProcessorEditor::handleMidiMessages(const MidiBuffer& midiMessages){
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;
    String s;
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isNoteOn())
        {
            DBG(m.getNoteNumber());
        }
        else if (m.isNoteOff())
        {
        }
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
        }
        else if (m.isController())
        {
            int controllerNumber = m.getControllerNumber();
            DBG(controllerNumber);
            std::stringstream ss;
            ss  << "Controller Number: " << controllerNumber << std::endl;
            int controllerValue = m.getControllerValue();
            DBG(controllerValue);
            ss  << "Controller Value: " << controllerValue << std::endl;
  
            int channel = m.getChannel();
            DBG(channel);
            ss  << "Channel: " << channel << std::endl;
            if(m.isChannelPressure()){
                int channelPressureValue = m.getChannelPressureValue();
                DBG(channelPressureValue);
                ss  << "Channel Pressure: " << channelPressureValue << std::endl;
            }
            int velocity = m.getVelocity();
            DBG(velocity);
            ss  << "Velocity: " << velocity << std::endl;

            if(m.getControllerName(controllerNumber) != NULL){
                const char* controllerName = m.getControllerName(controllerNumber);
                DBG(controllerName);
                ss  << "ControllerName: " << controllerName << std::endl;
            }
        }
        
        processedMidi.addEvent (m, time);
    }
}

MenuManager::~MenuManager(){
    
}

OrbishAudioProcessorEditor::~OrbishAudioProcessorEditor()
{
	processor.guiAlive = false;
    Thread::sleep(200);
    inputMeter->setLookAndFeel(nullptr);
    outputMeter->setLookAndFeel(nullptr);
}

void OrbishAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &thumbnail)        
		thumbnailChanged();
}

void OrbishAudioProcessorEditor::thumbnailChanged()
{
    //repaint();
}

void OrbishAudioProcessorEditor::timerCallback()
{
    changeTrack();
    if(tracksDirty){
       resized();
       tracksViewport.repaint();
    }
    tracksDirty = false;
    transportInfoArea.repaint();
	BufferForVisualisation* b;
	if (processor.context->xchange->readBufferQueue->read_available()) {
		processor.context->xchange->readBufferQueue->pop(b);
		updateLoopVisualiser(*b->buffer, b->numSamples);
		String s = String(pointer_sized_int(b));
		//DBG("Deleting ptr:" + s);
		delete b;
	}
	if (processor.activeTrack->Recording || processor.activeTrack->Playing) {
		recModeCombo.setEnabled(false);
	}
	else {
		recModeCombo.setEnabled(true);
	}
}

String OrbishAudioProcessorEditor::saveBuffer(int trackIdx
								, int loopIdx
								, int layerIdx
								, File dir
								, String name){
	auto t = processor.tracks[trackIdx];
	if (*t->LoopDuration == 0) {
		return String();
	}
	auto l = t->loops[loopIdx];
	if (l->Layers.size() <= layerIdx) { return String(); }
	auto layer = l->Layers[layerIdx];
	if (layer->Buffer->getNumSamples() == 0) {
		return String();
	}
	if (!dir.exists()) { 
		juce::Result result = dir.createDirectory(); 
		if (result.failed()) {
			DBG(result.getErrorMessage());
		}
	}
	File file = dir.getChildFile(name + ".wav");
	WavAudioFormat form;
	std::unique_ptr<AudioFormatWriter> writer;
	writer.reset(form.createWriterFor(new FileOutputStream(file),
		processor.context->sampleRate,
		layer->Buffer->getNumChannels(),
		24,
		{},
		0));

	writer->writeFromAudioSampleBuffer(*layer->Buffer, 0, layer->Buffer->getNumSamples());

	return file.getFullPathName();
}

String OrbishAudioProcessorEditor::saveBufferFromLoop(int trackIdx, int loopIdx) {
	auto track = tracks[trackIdx];
	auto layerIdx = processor.tracks[trackIdx]->CurrentTop;
	File dir(File::getSpecialLocation(File::tempDirectory));
	return saveBuffer(trackIdx, loopIdx, *processor.tracks[trackIdx]->CurrentTop
		, dir, track->getName() + "_" + String(loopIdx) + "_" + String(*layerIdx));
}

void OrbishAudioProcessorEditor::toggleRecord(){
	project.dirty = true;
    if(recordButton.getToggleState() && !processor.activeTrack->isRecordingArmed()){
        processor.activeTrack->setRecordingArmed(true);
    }
    if(!recordButton.getToggleState() && processor.activeTrack->isRecordingArmed()){
        processor.activeTrack->setRecordingArmed(false);
    }
    inputDisplay.clear();
}

void OrbishAudioProcessorEditor::togglePlay(){
    if(playButton.getToggleState() && !processor.activeTrack->isPlayArmed()){
        processor.activeTrack->setPlayArmed(true);
    }
    if(!playButton.getToggleState() && processor.activeTrack->isPlayArmed()){
        processor.activeTrack->setPlayArmed(false);
    }
}

void OrbishAudioProcessorEditor::toggleStop(){
    updatePlayHead(0, false);

}

void OrbishAudioProcessorEditor::toggleClear(){
        //getFromProcessor
    playButton.setToggleState(false, NotificationType::dontSendNotification);
    thumbnail.reset(processor.context->audioInputsCount, thumbnail.getNumSamplesFinished());
    updatePlayHead(0,false);
}



void OrbishAudioProcessorEditor::toggleMute(){
    //processor.processMuteChange();
}

void OrbishAudioProcessorEditor::toggleMonitor(){
    //processor.processMonitorChange();
}

void OrbishAudioProcessorEditor::toggleReverse(){
        //getFromProcessor
    reverseState = (reverseState == On)?Off:On;
    //processor.processReverseChange();
}

void OrbishAudioProcessorEditor::toggleUndo(){
//   processor.processPreviousChange();
}

void OrbishAudioProcessorEditor::toggleRedo(){
//   processor.processNextChange();
}

void OrbishAudioProcessorEditor::toggleAutoTrigger(){
   //processor.processTriggerModeChange();
}

void OrbishAudioProcessorEditor::toggleMuteAll(bool mute){
        //getFromProcessor
    muteAllButton.setState(Button::buttonNormal);
//    processor.processMuteAllChange(mute);
}

void OrbishAudioProcessorEditor::changeInputLevel(){
    //processor.processInputLevelChange(inputLevelSlider.getValue());
}

void OrbishAudioProcessorEditor::changeOutputLevel(){
    //processor.processOutputLevelChange(outputLevelSlider.getValue());
}

void OrbishAudioProcessorEditor::changeGlobalMix(){
  //  processor.processMixChange(globalVolumeSlider.getValue());
}

void OrbishAudioProcessorEditor::changeRecordingMode(){

}

void OrbishAudioProcessorEditor::changeSnapMode(){
}

void OrbishAudioProcessorEditor::paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    //DropShadow ds{Colour(0xFF2e2b23), 30, Point<int>(0,0)};
    Path pth{};
    pth.addRectangle(thumbnailBounds.withSizeKeepingCentre(thumbnailBounds.getWidth()+10, thumbnailBounds.getHeight()+10));
    //ds.drawForPath(g, pth);
    g.setColour (Colours::steelblue);
    auto audioLength (thumbnail.getTotalLength());                                      // [12]
    thumbnail.drawChannels (g,
                            thumbnailBounds,
                            0.0,
                            audioLength,
                            1.0f);
    g.setColour (Colours::white);
	playHead.setBounds(std::max(playHeadPosition - 2.0f, float(transportInfoArea.getX())), float(transportInfoArea.getY()), 2.0f,
                        float(transportInfoArea.getHeight()));
    g.setOpacity(0.4);
	if (playHead.getX() < 0)playHead.setX(0);
    g.fillRect(playHead);
    auto c1 = Colour(0x00FFFFFF);
    auto c2 = Colour(0x8FFFFF00);
    processor.logMessage("Calculating denominator for playhead trail size");
    double tmpDenom = processor.samplesToBeats(float(*processor.activeTrack->LoopDuration));
    int denominator = std::ceil(tmpDenom);
    processor.logMessage("1st stage: " + String(denominator));
    denominator = denominator / (processor.context->timeSigBottom * .25) ;
    processor.logMessage("2nd stage: " + String(denominator));
    denominator = denominator / processor.context->timeSigTop * (60 / processor.context->info->bpm);
    processor.logMessage("timesigTop: " + String(processor.context->timeSigTop));
    processor.logMessage("timeRatio: " + String(60 / processor.context->info->bpm));
    processor.logMessage("bpm: " + String(processor.context->info->bpm));

    processor.logMessage("3rd stage: " + String(denominator));
    denominator = jmin(jmax(denominator*2, 8),40);
    if(processor.activeTrack->Playing){
    if(reverseState == On){
        float tailWidth =
                std::min(transportInfoArea.getWidth() / denominator, transportInfoArea.getWidth() - (playHead.getX() - transportInfoArea.getX()));
        auto tail = Rectangle<float>(playHead.getX() , transportInfoArea.getY(),tailWidth, transportInfoArea.getHeight());
        g.setGradientFill( ColourGradient::horizontal( c2, c1, tail ));
        g.fillRect(tail);
    }else{
        float tailWidth =
                std::min(transportInfoArea.getWidth() / denominator, playHead.getX()-transportInfoArea.getX());
        auto tail = Rectangle<float>(playHead.getX() - tailWidth, transportInfoArea.getY()
                                    , tailWidth
                                    , transportInfoArea.getHeight());
        g.setGradientFill( ColourGradient::horizontal( c1, c2, tail ));
        g.fillRect(tail);
        }
    }
}

void OrbishAudioProcessorEditor::paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds)
{
    //DropShadow ds{Colour(0xFF2e2b23), 30, Point<int>(0,0)};
    g.setColour (Colours::white);
    Path pth{};
    pth.addRectangle(thumbnailBounds.withSizeKeepingCentre(thumbnailBounds.getWidth()+10, thumbnailBounds.getHeight()+10));
    //ds.drawForPath(g, pth);
    g.setColour (Colours::white);

    g.drawFittedText ("No Loop", thumbnailBounds, Justification::centred, 1.0f);
}

void OrbishAudioProcessorEditor::updateInputVisualiser(const AudioBuffer<float>& buffer, int numSamples){
}

void OrbishAudioProcessorEditor::updateLoopVisualiser(const AudioBuffer<float>& buffer, int numSamples){
        thumbnail.reset (processor.context->audioInputsCount, processor.context->sampleRate, numSamples);
    if (numSamples > 0) {
        thumbnail.addBlock (0, buffer, 0, numSamples);
    }
}


void OrbishAudioProcessorEditor::updatePlayHead(int position, bool reverse){
    reverseState = (reverse)?ToggleState::On:ToggleState::Off;
       if(reverseState == On){
        if(thumbnail.getTotalLength() > 0){
            float audioPosition = jmax(.0f,float(thumbnail.getNumSamplesFinished() - position));
            playHeadPosition = ((audioPosition / thumbnail.getNumSamplesFinished()) * transportInfoArea.getWidth() + transportInfoArea.getX());
        }else{
            playHeadPosition = thumbnail.getNumSamplesFinished()-1;
        }
    }else{
        if(thumbnail.getTotalLength() > 0){
            float audioPosition = position;
            playHeadPosition = ((audioPosition / thumbnail.getNumSamplesFinished()) * transportInfoArea.getWidth() + transportInfoArea.getX());
        }else{
            playHeadPosition = 0;
        }
    }
    playHeadPosition = jmax(playHeadPosition,.0f);
}

//==============================================================================
void OrbishAudioProcessorEditor::paint (Graphics& g)
{

   // processor.logMessage(String(ms));
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    if(start){
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        g.setColour (Colours::white);
        g.setFont (15.0f);

        g.drawFittedText ("Orbish", getLocalBounds(), Justification::top, 1);
        g.setColour(Colours::black);
        g.fillRect(toolCanvas);
        
        leftSide.paint(g);
        g.fillRect(leftSide.getBounds());
        addAndMakeVisible(leftSide);

        rightSide.paint(g);
        g.fillRect(rightSide.getBounds());
        addAndMakeVisible(rightSide);
        rightInnerSide.paint(g);
        g.fillRect(rightInnerSide.getBounds());
        addAndMakeVisible(rightInnerSide);
        leftInnerSide.paint(g);
        g.fillRect(leftInnerSide.getBounds());
        addAndMakeVisible(leftInnerSide);
        
        g.setGradientFill(ColourGradient(Colours::black,(float) transportInfoArea.getX(),(float) transportInfoArea.getY(), Colours::transparentBlack, (float) transportInfoArea.getX() + (float) transportInfoArea.getWidth(), (float) transportInfoArea.getY() + (float) transportInfoArea.getHeight(), false));
        transportInfoArea.paint(g);
        g.fillRect(transportInfoArea.getBounds());
        addAndMakeVisible(transportInfoArea);
        loopDisplayArea.paint(g);
        g.fillRect(loopDisplayArea.getBounds());
        addAndMakeVisible(loopDisplayArea);
        transportButtonArea.paint(g);
        g.fillRect(transportButtonArea.getBounds());
        addAndMakeVisible(transportButtonArea);
        loopConfigArea.paint(g);
        g.fillRect(loopConfigArea.getBounds());
        addAndMakeVisible(loopConfigArea);
        g.fillRect(tracksViewport.getBounds());

        tracksViewport.setViewedComponent(&trackArea, false);
        tracksViewport.setScrollBarsShown(true, false);
        tracksViewport.setScrollBarThickness(20);
        addAndMakeVisible(tracksViewport);
            start = false;
    }

    if (thumbnail.getNumChannels() == 0 || thumbnail.getTotalLength() <= 0) {
            paintIfNoFileLoaded (g, transportInfoArea.getBounds());
    }
    else {
            paintIfFileLoaded (g, transportInfoArea.getBounds());
    }
    g.setColour(Colours::greenyellow);
    highlightActiveTrack(g);
    paintInfoSection(g);
    if(processor.activeTrack->Recording){
        recordButton.setColour(TextButton::textColourOnId, Colours::white);
        recordButton.setColour(TextButton::buttonOnColourId, Colour(0x8FFC0B0B));
    }else{
        auto t = this->getTimerInterval();
        if(processor.activeTrack->isRecordingArmed() && (t%1000)%2 > 0){
            recordButton.setColour(TextButton::textColourOnId, Colour(0xFFFC0B0B));
        }else{
            recordButton.setColour(TextButton::textColourOnId, Colour(0x8FFC0B0B));
        }
    }
    recordButton.setColour(TextButton::textColourOffId, Colour(0xAFFC0B0B));

    if(processor.activeTrack->Playing){
        if(processor.activeTrack->isPlayArmed()){
            if(!playButton.getToggleState()){
                playButton.triggerClick();
            }
            playButton.setColour(TextButton::textColourOnId, Colours::white);
        }else{
            playButton.setColour(TextButton::textColourOnId, Colour(0xFF2ACD01));
            
        }
        playButton.setColour(TextButton::buttonOnColourId, Colour(0x8F2ACD01));
    }else{
        if(processor.activeTrack->isPlayArmed()){
            playButton.setColour(TextButton::textColourOnId, Colour(0xFF2ACD01));
        }else{
            playButton.setColour(TextButton::textColourOnId, Colour(Colours::darkgreen));
            if(playButton.getToggleState()){
                playButton.setToggleState(false, NotificationType::sendNotification);
                playButton.setState(Button::buttonNormal);
            }
        }
    }
    if(processor.activeTrack->Muted != tracks[activeTrack]->isMuted()) tracksDirty = true;
    if(processor.activeTrack->Soloed != tracks[activeTrack]->isSoloed()) tracksDirty = true;
    if(processor.activeTrack->Recording != tracks[activeTrack]->isRecording()) tracksDirty = true;
    if(processor.activeTrack->Playing != tracks[activeTrack]->isPlaying()) tracksDirty = true;
    if(processor.activeTrack->isPlayArmed() != tracks[activeTrack]->isPlayArmed()) tracksDirty = true;
    if(processor.activeTrack->isMuteArmed() != tracks[activeTrack]->isMutedArmed()) tracksDirty = true;
    if(processor.activeTrack->isSoloArmed() != tracks[activeTrack]->isSoloArmed()) tracksDirty = true;
    if(processor.activeTrack->isRecordingArmed() != tracks[activeTrack]->isRecordingArmed()) tracksDirty = true;
    if (tracksDirty) {
        repaint();
    }
    playButton.setColour(TextButton::textColourOffId, Colour(0xAF2ACD01));
    

    if(trackToRemove > 0){
        doRemoveTrack();
    }
    if(shouldCreateTrack){
        createTrack();

    }
	projectLabel.setColour(Label::backgroundColourId, Colour(0x50ffffff));
	projectLabel.setFont(Font(18, Font::bold));
    projectLabel.setJustificationType(Justification::centred);
}

void OrbishAudioProcessorEditor::paintInfoSection(Graphics& g){
    String timeSig = String(processor.context->info->timeSigNumerator) + "/" + String(processor.context->info->timeSigDenominator);
    if(timeSig != timeSigLabel.getText()){
        timeSigLabel.setText(timeSig, NotificationType::dontSendNotification);
        timeSigLabel.touch();
    }
    auto bpmStr = "bpm: " + String(processor.context->info->bpm, 1);
    if(bpmStr != bpmLabel.getText()){
        bpmLabel.setText(bpmStr, NotificationType::dontSendNotification);
        bpmLabel.touch();
    }
    
    float totalSubDiv = processor.samplesToBeats(*processor.activeTrack->CurrentPlayingIndex);
    double garbage;
    int bars = totalSubDiv / processor.context->info->timeSigNumerator + 1;
    int beats = fmod(totalSubDiv,processor.context->info->timeSigNumerator) + 1;
    float rest = std::modf(totalSubDiv, &garbage);
    int subSubDiv = rest * 4 + 1;
    String progress = String(bars) + ". " + String(beats) + ". " + String(subSubDiv);
    if (progress !=  progressLabel.getText()) {
    progressLabel.setText(progress, NotificationType::dontSendNotification);
    }
    progressLabel.setTooltip("Shows the progress of the loop while playing\n Time progress is synced to the host");
    auto trackNbr = "Track: "+String(processor.activeTrack->Index + 1);
    if (trackNbr != trackNumberLabel.getText()) {
        trackNumberLabel.setText(trackNbr, NotificationType::dontSendNotification);
    }
    auto loopNbr = "Loop: "+String(tracks[activeTrack]->getActiveLoop() + 1);
    if (loopNbr != trackNumberLabel.getText()) {
        trackNumberLabel.setText(loopNbr, NotificationType::dontSendNotification);
    }
    auto layerNbr = "Layer: " + String(*processor.activeTrack->CurrentTop + 1);
    if (layerNbr != trackNumberLabel.getText()) {
        layerNumberLabel.setText(layerNbr, NotificationType::dontSendNotification);
    }
	auto grp = processor.getTrackGroup(processor.activeTrack);
	String groupName = "";
	Colour grpCol(Colours::black);
	if (nullptr != grp) {
		groupName = grp->Name;
		grpCol = groupColours[grp->Index];
	}
    groupName = "Group: " + String(groupName);
    if (groupNumberLabel.getText() != groupName){
        groupNumberLabel.setText(groupName, NotificationType::dontSendNotification);
	groupNumberLabel.setColour(Label::textColourId,grpCol);
    }
    if (processor.activeTrack->Playing) {
        Path path;
        path.addEllipse(300,35,10,10);
        barWitness.setPath (path);
        barWitness.setFill (Colours::green);
        barWitness.setStrokeFill (Colours::greenyellow);
        if((beats == 1 && subSubDiv == 1 && rest < (processor.context->bpm * 0.003f))){
            barWitness.setAlpha(1);
        }else{
            barWitness.setAlpha(0.2);
        }
        barWitness.setStrokeThickness (2.0f);
        
        Path path2;
        path2.addEllipse(320,35,10,10);
        beatWitness.setPath (path2);
        beatWitness.setFill (Colours::orangered);
        beatWitness.setStrokeFill (Colours::orange);
        if(processor.activeTrack->Playing && subSubDiv == 1 && rest < (processor.context->bpm * 0.003f)){
            beatWitness.setAlpha(1);
        }else{
            beatWitness.setAlpha(0.2);
        }
        beatWitness.setStrokeThickness (2.0f);
    }


}

void OrbishAudioProcessorEditor::resized()
{
	if (nullptr != settingsPage.get()) {
		settingsPage->setBounds(getX(), getY(), getWidth(), getHeight());
	}
	menuBar->setBounds(0, 0, getWidth(), 20);
	headerComp.setBounds(getX(), getY() +20, getWidth(), 60);
    toolCanvas.setBounds(getWidth()/100, getHeight()/12 + 30, getWidth()*.98 , getHeight() - getHeight()/12-50 );

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int containerMargin = 1;
    int controlMargin = 2;
	projectLabel.setBounds(450, 5, 200, 20);
    activeLabel.setBounds(10, 10, 200, 20);
    recordButton.setBounds(10, 40, 50, 20);
    playButton.setBounds(65, 40, 50, 20);
    stopButton.setBounds(120, 40, 50, 20);
    clearButton.setBounds(175, 40, 50, 20);
    muteButton.setBounds(230, 40, 50, 20);
    soloButton.setBounds(285, 40, 50, 20);
    monitorButton.setBounds(10, 65, 50, 20);
    reverseButton.setBounds(65, 65, 50, 20);
    undoButton.setBounds(120, 65, 50, 20);
    redoButton.setBounds(175, 65, 50, 20);
    bounceButton.setBounds(230, 65, 50, 20);
    autoTriggerButton.setBounds(285, 65, 50, 20);

    loopLabel.setBounds(5, 95, 50, 20);
    previousLoopButton.setBounds(60, 95, 25, 20);
    activeLoopLabel.setBounds(95, 95, 30, 20);
    nextLoopButton.setBounds(130, 95, 25, 20);
    newLoopButton.setBounds(160, 95, 25, 20);
    removeLoopButton.setBounds(190, 95, 25, 20);
    
    globalLabel.setBounds(10, 10, 200, 20);
    muteAllButton.setBounds(10, 40, 50, 20);
    stopAllButton.setBounds(10, 65, 50, 20);
    pauseAllButton.setBounds(65, 40, 50, 20);
    startAllButton.setBounds(65, 65, 50, 20);
    clearAllButton.setBounds(120, 40, 50, 20);
	addToGroupButton.setBounds(190, 40, 50, 20);
	removeFromGroupButton.setBounds(190, 65, 50, 20);

    trackLabel.setBounds(5, 95, 50, 20);
    previousTrackButton.setBounds(60, 95, 25, 20);
    activeTrackLabel.setBounds(95, 95, 30, 20);
    nextTrackButton.setBounds(130, 95, 25, 20);
    newTrackButton.setBounds(160, 95, 25, 20);
    removeTrackButton.setBounds(190, 95, 25, 20);
    if(tracksLayoutButton != nullptr){
        tracksLayoutButton->setBounds(250, 90, 25, 25);
    }

    leftSide.setBounds(toolCanvas.removeFromLeft(50).reduced(containerMargin));
    rightSide.setBounds(toolCanvas.removeFromRight(50).reduced(containerMargin));
    leftInnerSide.setBounds(toolCanvas.removeFromLeft(50).reduced(containerMargin));
    rightInnerSide.setBounds(toolCanvas.removeFromRight(50).reduced(containerMargin));
    transportInfoArea.setBounds(toolCanvas.removeFromTop(100).reduced(containerMargin));
    loopDisplayArea.setBounds(toolCanvas.removeFromTop(40).reduced(containerMargin));
    auto rect = toolCanvas.removeFromTop(120).reduced(containerMargin);
    transportButtonArea.setBounds(rect);
    loopConfigArea.setBounds(rect.removeFromRight(rect.getWidth() * .33f).reduced(containerMargin,0));
    rect = toolCanvas;
    rect.removeFromBottom(containerMargin + controlMargin + 1);
    tracksViewport.setBounds(rect.reduced(containerMargin));

    auto r = leftInnerSide.getLocalBounds();
    auto s = r.removeFromTop(r.getHeight()/2);
    inputLevelSlider.setBounds (r.removeFromLeft(r.getWidth() * .5f).reduced(controlMargin + 3));
    r = rightInnerSide.getLocalBounds();
    s = r.removeFromLeft(r.getWidth() * .5f);
    outputLevelSlider.setBounds(s.removeFromBottom(s.getHeight() * .5f).reduced(controlMargin + 3));

    globalVolumeSlider.setBounds(r.removeFromBottom(r.getHeight() * .5f).reduced(controlMargin + 3));
    r = leftSide.getLocalBounds().removeFromBottom(leftSide.getHeight() * .5f).reduced(5, 0);
    inputMeter->setBounds(r.withBottomY(inputLevelSlider.getBottom()));
    r = rightSide.getLocalBounds().removeFromBottom(rightSide.getHeight() * .5f).reduced(5, 0);
    outputMeter->setBounds(r.withBottomY(outputLevelSlider.getBottom()));

    makeTracks();
    
    timeSigLabel.setBounds(105,25,50,28);
    bpmLabel.setBounds(170,25,70,28);
    progressLabel.setBounds(340,25,100,28);
    trackNumberLabel.setBounds(105, 5, 60, 15);
    loopNumberLabel.setBounds(170, 5, 60, 15);
    layerNumberLabel.setBounds(235, 5, 60, 15);
	groupNumberLabel.setBounds(300, 5, 60, 15);
	groupLabel.setBounds(860, 5, 100, 15);
	groupCombo.setBounds(965, 5, 50, 25);
    recModeCombo.setBounds(460, 40, 100, 25);
    snapModeCombo.setBounds(460, 70, 100, 25);
    recModeLabel.setBounds(340, 40, 100, 25);
    snapModeLabel.setBounds(340, 70, 100, 25);

}

void OrbishAudioProcessorEditor::highlightActiveTrack(Graphics& g){
    g.setOrigin(tracksViewport.getBounds().getX(), tracksViewport.getBounds().getY());
    for (auto track : tracks) {
        trackArea.addAndMakeVisible(track);
    }
}

int OrbishAudioProcessorEditor::getTrackRowHeight(int rowIdx) {
	int maxNbrLoops = 0;
	int tmpRowIdx = std::max(rowIdx-1,0);
	for (auto i = tmpRowIdx * nbrTracksInARow;(i < tracks.size()) && (i < tmpRowIdx * nbrTracksInARow + nbrTracksInARow);++i) {
		maxNbrLoops = std::max(tracks[i]->Loops.size(), maxNbrLoops);
	}
	return (rowIdx>0)?maxNbrLoops * tracks[0]->loopHeight + tracks[tmpRowIdx * nbrTracksInARow]->getY() + tracks[0]->loopHeight :0;
}


void OrbishAudioProcessorEditor::makeTracks(){
    
    if(tracksDirty){
        trackArea.setBounds(tracksViewport.getBounds());
        for (auto t : tracks) {
            auto i = t->getIndex();
			t->horizontalLayout = tracksLayoutHorizontal;
            if(activeTrack == t->getIndex()){
                if(shouldCreateLoop && t->Loops.size() < processor.activeTrack->loops.size()){
                    t->addLoop(t->getAudioTrack()->loops.getLast()->Progress);
                    shouldCreateLoop = false;
                }
                if(shouldRemoveLoop){
                    t->removeLoop();
                    shouldRemoveLoop = false;
                }
                activeLoopLabel.setText(String(t->getActiveLoop() + 1), NotificationType::dontSendNotification);
                loopNumberLabel.setText(String(t->getActiveLoop() + 1),
                                        NotificationType::dontSendNotification);
            }
			if (!tracksLayoutHorizontal) {
				t->setBounds(5 + (i % nbrTracksInARow * (840/nbrTracksInARow)), (getTrackRowHeight(i / nbrTracksInARow)) + 5, 840/nbrTracksInARow, 20 + 18 * t->Loops.size());
			}
			else {
				t->setBounds(5, i * (55) + 5, trackArea.getWidth() - 2 * 5, 50);
			}
            auto tr = t->getAudioTrack();
			auto grp = processor.getTrackGroup(tr);
			String groupName = "";
			Colour grpCol = Colours::black;
			if (nullptr != grp) {
				groupName = grp->Name;
				grpCol = groupColours[grp->Index];
			}
			t->Group = groupName;
			t->GroupColour = grpCol;
            t->setRecordingArmed(tr->isRecordingArmed());
            t->setMuted(tr->Muted);
            t->setMutedArmed(tr->isMuteArmed());
            t->setSoloed(tr->Soloed);
            t->setSoloArmed(tr->isSoloArmed());
            t->setPlaying(tr->Playing);
            t->setPlayArmed(tr->isPlayArmed());
            t->resized();
        }
        repaint(tracksViewport.getBounds());
        tracksDirty = false;
    }
	std::vector<int> height; int sum = 0;
	height.reserve(20);
    for (auto track : tracks) {
        auto i = track->getIndex();
        if (i % nbrTracksInARow == 0) { height.push_back(0); }
        height[i / nbrTracksInARow] = jmax(height[i/nbrTracksInARow], track->getHeight());
	}
	for (int i = 0;i < height.size(); ++i) {
		sum += height[i];
	}

    int totalHeight = (tracksLayoutHorizontal)
                    ? tracks.size()*(55)+5
                    :(sum)+5;
    int diff = trackArea.getHeight() - totalHeight;
    if(getHeight() < 600 - diff || getHeight() > 900 - diff){
        setSize(getWidth(), std::min(std::max(getHeight() - diff, 600), 900));
    }
    trackArea.setSize(trackArea.getWidth(), std::max(tracksViewport.getHeight(), totalHeight));
}

void OrbishAudioProcessorEditor::sliderValueChanged (Slider* slider) {
    if(slider == &inputLevelSlider){
        inputLevelSlider.onValueChange = [this]() { changeInputLevel(); };
    }
    if(slider == &outputLevelSlider){
        outputLevelSlider.onValueChange = [this]() { changeOutputLevel(); };
    }
    if(slider == &globalVolumeSlider){
        globalVolumeSlider.onValueChange = [this]() { changeGlobalMix(); };
    }
}

void OrbishAudioProcessorEditor::clicked(Button* button) {
	if (button == &settingsPage->closeSettingsButton) {
		closeSettingsPage();
	}
	if (button == &settingsPage->activateLoggingButton) {
			processor.loggingActive = button->getToggleState();
	}
}

void OrbishAudioProcessorEditor::sliderChanged(Slider* slider) {
	if (slider == &settingsPage->maxUndoHistorySlider) {
		 processor.context->maxUndoHistory = slider->getValue(); 
	}
	if (slider == &settingsPage->latencySlider) {
		processor.context->delayCompensation = slider->getValue();
	}
    if (slider == &settingsPage->tracksPerRowSlider) {
         nbrTracksInARow = slider->getValue();
	}
}

void OrbishAudioProcessorEditor::buttonClicked(Button* button){
    if(button == &recordButton){
        recordButton.onClick = [this]() { toggleRecord(); };
        tracksDirty = true;
    }
    if(button == &playButton){
        playButton.onClick = [this]() { togglePlay(); };
        tracksDirty = true;
    }
    if(button == &stopButton){
        stopButton.onClick = [this]() { toggleStop(); };
    }
    if(button == &clearButton || button == &clearAllButton){
        button->onClick = [this]() { toggleClear(); };
    }
    if(button == &muteButton){
        muteButton.onClick = [this]() { toggleMute(); };
        tracksDirty = true;
    }
    if(button == &soloButton){
        tracksDirty = true;
    }
    if(button == &monitorButton){
        monitorButton.onClick = [this]() { toggleMonitor(); };
    }
    if(button == &reverseButton){
        reverseButton.onClick = [this]() { toggleReverse(); };
    }
    if(button == &undoButton){
        undoButton.onClick = [this]() { toggleUndo(); };
    }
    if(button == &redoButton){
        redoButton.onClick = [this]() { toggleRedo(); };
    }
    if(button == &autoTriggerButton){
        autoTriggerButton.onClick = [this]() { toggleAutoTrigger(); };
    }
    if(button == &muteAllButton){
        button->onClick = [this] { toggleMuteAll((bool)muteAllButton.getToggleStateValue().getValue()); };
    }
	if (button == &addToGroupButton) {
		button->onClick = [this] {
			tracksDirty = true;
			makeTracks();
		};
	}
	if (button == &removeFromGroupButton) {
		button->onClick = [this] {
			tracksDirty = true;
			makeTracks();
		};
	}

    if(button == tracksLayoutButton){
        tracksLayoutHorizontal = !tracksLayoutHorizontal;
        tracksDirty = true;
        repaint();
        resized();
    }
	if (button == &settingsPage->closeSettingsButton) {
		closeSettingsPage();
	}
}

void OrbishAudioProcessorEditor::mouseDown(const MouseEvent &event) {
    for (auto track : tracks) {
        if(event.eventComponent->getParentComponent() == track){
			if (event.getNumberOfClicks() > 1) {
				auto lp = track->getActiveLoop();
				saveBufferFromLoop(track->getIndex(), lp);
			}
			else {
                for (auto loop  : track->Loops) {
					if (event.eventComponent == loop) {
						processor.askLoopChange(loop->getIndex());
					}
					else {
						processor.askTrackChange(track->getIndex());
						changeTrack();
					}
				}
			}
        }
    }
}
void OrbishAudioProcessorEditor::mouseDrag(const MouseEvent& event) {
    for (auto track : tracks) {
        for (auto loop  : track->Loops) {
            if (event.eventComponent == loop) {
                    if (!this->isDragAndDropActive()) {
                        this->startDragging("loop", loop, Image(), true);
                    }
                auto s = saveBufferFromLoop(track->getIndex(), loop->getIndex());
                if (s.length() == 0){
                    return;
                }
				StringArray files = { s };															
				performExternalDragDropOfFiles({ s }, false, {  });
			}
        }
	}
}

void OrbishAudioProcessorEditor::updateNextTrackNumber(int trackNumber){
    nextTrackNumber = trackNumber;
    trackNumberUpdated = true;
}

void OrbishAudioProcessorEditor::updateNextLoopNumber(int trackNumber, int loopNumber){
	if (tracks.size() > trackNumber && trackNumber >= 0 && loopNumber >= 0) {
		auto t = tracks[trackNumber];
		t->setActiveLoop(loopNumber);
		tracksDirty = true;
	}
}

void OrbishAudioProcessorEditor::changeTrack(){
    if(!trackNumberUpdated){
        return;
    }
    if(nextTrackNumber >= tracks.size() || nextTrackNumber < 0){
        return;
    }
    for(auto t : tracks){
        t->setActive(nextTrackNumber == t->getIndex());
        t->setActiveLoop(t->getAudioTrack()->ActiveLoop->Index);
    }
    tracksDirty = true;
    activeTrack=nextTrackNumber;
    activeTrackLabel.setText(String(activeTrack + 1), NotificationType::dontSendNotification);
    activeLoopLabel.setText(String(tracks[activeTrack]->getActiveLoop() + 1), NotificationType::dontSendNotification);
    loopNumberLabel.setText(String(tracks[activeTrack]->getActiveLoop() + 1),
                            NotificationType::dontSendNotification);
    //repaint();
    trackNumberUpdated = false;
}

void OrbishAudioProcessorEditor::askToCreateLoop(){
    shouldCreateLoop = true;
    tracksDirty = true;
	project.dirty = true;
}

void OrbishAudioProcessorEditor::askToCreateTrack(){
    shouldCreateTrack = true;
}


void OrbishAudioProcessorEditor::createTrack(){
    doCreateTrack(tracks.size());
    shouldCreateTrack = false;
	project.dirty = true;
}


void OrbishAudioProcessorEditor::doCreateTrack(int trackNumber) {
	if (processor.tracks.size() <= trackNumber) return;
    auto audioTrack = processor.tracks[trackNumber];
	auto loops = &audioTrack->loops;

	std::vector<double*> prg;
    for(auto l : *loops){
        prg.push_back(&(l->Progress));
    }
	if (tracks.size() == trackNumber) {
		auto* t = new TrackComponent(trackNumber, prg, tracksLayoutHorizontal, audioTrack);
		t->addMouseListener(this, true);
		tracks.add(t);
	}
	else {
//		}
        for(auto l : *loops){
            tracks[trackNumber]->addLoop(l->Progress);
		}
	}
    tracksDirty = true;
}

void OrbishAudioProcessorEditor::removeLoop(){
    shouldRemoveLoop = true;
    tracksDirty = true;
	project.dirty = true;
}

void OrbishAudioProcessorEditor::removeTrack(int trackNumber){
    trackToRemove = trackNumber;
	project.dirty = true;
}


void OrbishAudioProcessorEditor::doRemoveTrack(){
    if(trackToRemove > 0 && trackToRemove < tracks.size()){
        if(!tracks[trackToRemove]->isActive()){
            tracks.remove(trackToRemove);
            tracksDirty = true;
        }
    }
    trackToRemove = 0;
}
