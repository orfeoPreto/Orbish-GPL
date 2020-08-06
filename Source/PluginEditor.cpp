/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OrbishAudioProcessorEditor::OrbishAudioProcessorEditor (OrbishAudioProcessor& p, AudioProcessorValueTreeState& apvts)
: AudioProcessorEditor (&p),                              // [4]
projectXml("<project />"), processor (p), thumbnailCache (5), thumbnail (32, formatManager, thumbnailCache), valueTreeState(apvts)
{
	//processor.logger->logMessage("begin gui constructor");
    openGLContext.attachTo(*getTopLevelComponent());

    // Define the Look and Feel of the application
    auto editorLookAndFeel = new OrbishLookAndFeel();
    setLookAndFeel(editorLookAndFeel);
    setResizable(true, false);

    // Setup project header
	project = Project();
	setProjectName(project.name);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
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

    playChanged = &Observer::updatePlaying;

    processor.context->observer = this;
    thumbnail.addChangeListener (this);
    startTimer (20);
   
    // setup level meters
    inputMeter = std::make_shared<FFAU::LevelMeter> (); // See FFAU::LevelMeter::MeterFlags for options
    inputMeter->setLookAndFeel (editorLookAndFeel);
    inputMeter->setMeterSource (processor.getInputMeterSource());
    inputMeter->setMeterFlags(FFAU::LevelMeter::Minimal);

    for (auto track : processor.tracks) {
       doCreateTrack(track->Index);
    }

    auto buttonControlArea = &infoAndControlArea.controlArea.buttonControlArea;
    buttonControlArea->outputControlArea.setEditor(this);

    // Mode control attachments
    auto modeControlArea = &buttonControlArea->modeAndNavigationControlArea.modeControlArea;

    recModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "mode", modeControlArea->recModeCombo));
    snapModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "snap", modeControlArea->snapModeCombo));

    // transport Button attachments
    auto transportControlArea = &buttonControlArea->transportControlArea;
    transportControlArea->setEditor(this);

    recordAttachment.reset (new ButtonAttachment (valueTreeState, "record", transportControlArea->recordButton));
    playAttachment.reset (new ButtonAttachment (valueTreeState, "play", transportControlArea->playButton));
    stopAttachment.reset(new ButtonAttachment(valueTreeState, "stop", transportControlArea->stopButton));
    clearAttachment.reset(new ButtonAttachment(valueTreeState, "reset", transportControlArea->clearButton));
    muteAttachment.reset(new ButtonAttachment(valueTreeState, "mute", transportControlArea->muteButton));
    soloAttachment.reset(new ButtonAttachment(valueTreeState, "solo", transportControlArea->soloButton));
    monitorAttachment.reset(new ButtonAttachment(valueTreeState, "monitor", transportControlArea->monitorButton));
    reverseAttachment.reset(new ButtonAttachment(valueTreeState, "reverse", transportControlArea->reverseButton));
    undoAttachment.reset(new ButtonAttachment(valueTreeState, "undo", transportControlArea->undoButton));
    redoAttachment.reset(new ButtonAttachment(valueTreeState, "redo", transportControlArea->redoButton));
    bounceAttachment.reset(new ButtonAttachment(valueTreeState, "bounce", transportControlArea->bounceButton));
    triggerAttachment.reset(new ButtonAttachment(valueTreeState, "trigger", transportControlArea->autoTriggerButton));
    
    // Global Button attachments
    auto globalControlArea = &buttonControlArea->globalControlArea;
    globalControlArea->setEditor(this);
    
    muteAllAttachment.reset (new ButtonAttachment (valueTreeState, "muteAll", globalControlArea->muteAllButton));
    stopAllAttachment.reset (new ButtonAttachment (valueTreeState, "stopAll", globalControlArea->stopAllButton));
    startAllAttachment.reset (new ButtonAttachment (valueTreeState, "startAll", globalControlArea->startAllButton));
    pauseAllAttachment.reset (new ButtonAttachment (valueTreeState, "pauseAll", globalControlArea->pauseAllButton));
    clearAllAttachment.reset (new ButtonAttachment (valueTreeState, "resetAll", globalControlArea->clearAllButton));
    
    // Grouping buttons
    addToGroupAttachment.reset(new ButtonAttachment(valueTreeState, "addToGroup", addToGroupButton));
    addToGroupButton.setTooltip("Add the active track to the selected group");

    removeFromGroupAttachment.reset(new ButtonAttachment(valueTreeState, "removeFromGroup", removeFromGroupButton));
    removeFromGroupButton.setTooltip("Remove the active track from the selected group");

    auto navigationControlArea = &infoAndControlArea.controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;

    // Loop button attachments
    previousLoopAttachment.reset (new ButtonAttachment (valueTreeState, "previousLoop", navigationControlArea->previousLoopButton));
    nextLoopAttachment.reset (new ButtonAttachment (valueTreeState, "nextLoop", navigationControlArea->nextLoopButton));
    newLoopAttachment.reset (new ButtonAttachment (valueTreeState, "newLoop", navigationControlArea->newLoopButton));
    removeLoopAttachment.reset (new ButtonAttachment (valueTreeState, "removeLoop", navigationControlArea->removeLoopButton));
    
    // Track buttons
    previousTrackAttachment.reset (new ButtonAttachment (valueTreeState, "previousTrack", navigationControlArea->previousTrackButton));
    nextTrackAttachment.reset (new ButtonAttachment (valueTreeState, "nextTrack", navigationControlArea->nextTrackButton));
    newTrackAttachment.reset (new ButtonAttachment (valueTreeState, "newTrack", navigationControlArea->newTrackButton));
    removeTrackAttachment.reset (new ButtonAttachment (valueTreeState, "removeTrack", navigationControlArea->removeTrackButton));
    
	loopConfigArea.addAndMakeVisible(addToGroupButton);
	loopConfigArea.addAndMakeVisible(removeFromGroupButton);

    inputDisplay.setSamplesPerBlock(processor.context->maxBlockSize);
    inputDisplay.setBufferSize(processor.context->samplesPerBlock);
    inputDisplay.setColours(Colours::darkgrey, Colours::indianred);

	loopInfoArea.addAndMakeVisible(groupLabel);
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
	loopInfoArea.addAndMakeVisible(groupCombo);
    groupCombo.setTooltip("Select a group, then add or remove tracks. \nAll tracks in the same group will act simultaneously for certain commands");


    globalVolumeSlider.setValue(0);
    inputLevelSlider.setNumDecimalPlacesToDisplay(1);
    
    inputLevelSlider.setTextBoxIsEditable(true);
    inputLevelSlider.setTextValueSuffix(" db");
    inputLevelSlider.addListener(this);
    inputLevelAttachment.reset (new SliderAttachment (valueTreeState, "inputLevel", inputLevelSlider));
    inputLevelSlider.setSliderStyle (Slider::LinearBarVertical);
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

    auto outputControlArea = &buttonControlArea->outputControlArea;
    outputLevelAttachment.reset (new SliderAttachment (valueTreeState, "outputLevel", outputControlArea->outputLevelSlider));
    
    globalVolumeSlider.setRange(-120, 6);
    globalVolumeSlider.setNumDecimalPlacesToDisplay(2);
    globalVolumeSlider.setTextBoxIsEditable(true);
    globalVolumeSlider.setTextValueSuffix(" db");
    globalVolumeSlider.addListener(this);
    globalMixAttachment.reset (new SliderAttachment (valueTreeState, "globalMix", globalVolumeSlider));
    globalVolumeSlider.setSliderStyle (Slider::LinearBarVertical);
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

    headerArea.setEditor(this);
    addAndMakeVisible(headerArea);
    addAndMakeVisible(infoAndControlArea);
    addAndMakeVisible(tracksArea);

    setSize (1400, 650);
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
	alert->addButton("Ok", 1, juce::KeyPress(KeyPress::returnKey));
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
	//projectLabel.setText(name, NotificationType::dontSendNotification);
    infoAndControlArea.infoArea.setProjectName(name);
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
        setProjectName(project.directory.getFileName());	
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
	alert->addButton(firstButtonText, 1, juce::KeyPress(KeyPress::returnKey));
	alert->addButton(secondButtonText, 0, juce::KeyPress(KeyPress::escapeKey));
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

OrbishAudioProcessorEditor::~OrbishAudioProcessorEditor()
{
	processor.guiAlive = false;
    Thread::sleep(200);
    inputMeter->setLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
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
    transportInfoArea.repaint();
	BufferForVisualisation* b;
	if (processor.context->xchange->readBufferQueue->read_available()) {
		processor.context->xchange->readBufferQueue->pop(b);
		updateLoopVisualiser(*b->buffer, b->numSamples);
		String s = String(pointer_sized_int(b));
		delete b;
	}

    auto modeControlArea = &infoAndControlArea.controlArea.buttonControlArea.modeAndNavigationControlArea.modeControlArea;
	if (processor.activeTrack->Recording || processor.activeTrack->Playing) {
        modeControlArea->recModeCombo.setEnabled(false);
	}
	else {
        modeControlArea->recModeCombo.setEnabled(true);
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
    inputDisplay.clear();
}



void OrbishAudioProcessorEditor::toggleStop(){
    updatePlayHead(0, false);

}

void OrbishAudioProcessorEditor::toggleClear(){
    infoAndControlArea.controlArea.buttonControlArea.transportControlArea.playButton.setToggleState(false, NotificationType::dontSendNotification);
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

void OrbishAudioProcessorEditor::changeInputLevel(){
    //processor.processInputLevelChange(inputLevelSlider.getValue());
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
    Path pth{};
    pth.addRectangle(thumbnailBounds.withSizeKeepingCentre(thumbnailBounds.getWidth()+10, thumbnailBounds.getHeight()+10));

    g.setColour (findColour(TextButton::ColourIds::textColourOnId));
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
   // processor.logMessage("Calculating denominator for playhead trail size");
    double tmpDenom = processor.samplesToBeats(float(*processor.activeTrack->LoopDuration));
    int denominator = std::ceil(tmpDenom);
  //  processor.logMessage("1st stage: " + String(denominator));
    denominator = denominator / (processor.context->timeSigBottom * .25) ;
  //  processor.logMessage("2nd stage: " + String(denominator));
    denominator = denominator / processor.context->timeSigTop * (60 / processor.context->info->bpm);
 //   processor.logMessage("timesigTop: " + String(processor.context->timeSigTop));
 //   processor.logMessage("timeRatio: " + String(60 / processor.context->info->bpm));
  //  processor.logMessage("bpm: " + String(processor.context->info->bpm));

 //   processor.logMessage("3rd stage: " + String(denominator));
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
    g.setColour (Colours::white);
    Path pth{};
    pth.addRectangle(thumbnailBounds.withSizeKeepingCentre(thumbnailBounds.getWidth()+10, thumbnailBounds.getHeight()+10));

    g.setColour (findColour(Label::textColourId));
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
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    if(start){
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
        //loopDisplayArea.paint(g);/*
        //g.fillRect(loopDisplayArea.getBounds());
        //addAndMakeVisible(loopDisplayArea);*/
        /*transportButtonArea.paint(g);
        g.fillRect(transportButtonArea.getBounds());
        addAndMakeVisible(transportButtonArea);*/
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

    auto transportControlArea = &infoAndControlArea.controlArea.buttonControlArea.transportControlArea;

    if(processor.activeTrack->Playing){
        if(processor.activeTrack->isPlayArmed()){
            if(!transportControlArea->playButton.getToggleState()) {
                transportControlArea->playButton.triggerClick();
            }
        }
    }
    else {
        if (!processor.activeTrack->isPlayArmed()) {
            if (transportControlArea->playButton.getToggleState()) {
                transportControlArea->playButton.setToggleState(false, NotificationType::sendNotification);
                transportControlArea->playButton.setState(Button::buttonNormal);
            }
        }
    }
    if(processor.activeTrack->Muted != tracks[activeTrack]->isMuted()) tracksDirty = true;
    if(processor.activeTrack->Soloed != tracks[activeTrack]->isSoloed()) tracksDirty = true;
    if(processor.activeTrack->isMuteArmed() != tracks[activeTrack]->isMutedArmed()) tracksDirty = true;
    if(processor.activeTrack->isSoloArmed() != tracks[activeTrack]->isSoloArmed()) tracksDirty = true;

    if (tracksDirty) {
        trackArea.repaint();
        tracksDirty = false;
    }
}

void OrbishAudioProcessorEditor::paintInfoSection(Graphics& g){
    String timeSig = String(processor.context->info->timeSigNumerator) + "/" + String(processor.context->info->timeSigDenominator);
    if(timeSig != infoAndControlArea.infoArea.getTimeSignature()){
        infoAndControlArea.infoArea.setTimeSignature(timeSig);
    }
    auto bpmStr = "bpm: " + String(processor.context->info->bpm, 1);
    if(bpmStr != infoAndControlArea.infoArea.getBeatsPerMinute()){
        infoAndControlArea.infoArea.setBeatsPerMinute(bpmStr);
    }
    
    float totalSubDiv = processor.samplesToBeats(*processor.activeTrack->CurrentPlayingIndex);
    double garbage;
    int bars = totalSubDiv / processor.context->info->timeSigNumerator + 1;
    int beats = fmod(totalSubDiv,processor.context->info->timeSigNumerator) + 1;
    float rest = std::modf(totalSubDiv, &garbage);
    int subSubDiv = rest * 4 + 1;
    String progress = String(bars) + ". " + String(beats) + ". " + String(subSubDiv);
    if (progress !=  infoAndControlArea.infoArea.getProgress()) {
        infoAndControlArea.infoArea.setProgress(progress);
    }
    auto trackNbr = "Track: "+String(processor.activeTrack->Index + 1);
    if (trackNbr != infoAndControlArea.infoArea.getTrackNumber()) {
        infoAndControlArea.infoArea.setTrackNumber(trackNbr);
    }
    auto loopNbr = "Loop: "+String(tracks[activeTrack]->getActiveLoop() + 1);
    if (loopNbr != infoAndControlArea.infoArea.getLoopNumber()) {
        infoAndControlArea.infoArea.setLoopNumber(loopNbr);
    }
    auto layerNbr = "Layer: " + String(*processor.activeTrack->CurrentTop + 1);
    if (layerNbr != infoAndControlArea.infoArea.getLayerNumber()) {
        infoAndControlArea.infoArea.setLayerNumber(layerNbr);
    }
	auto grp = processor.getTrackGroup(processor.activeTrack);
	String groupName = "";
	if (nullptr != grp) {
		groupName = grp->Name;
	}
    groupName = "Group: " + String(groupName);
    if (infoAndControlArea.infoArea.getGroupNumber() != groupName){
        infoAndControlArea.infoArea.setGroupNumber(groupName);
    }
    if (processor.activeTrack->Playing) {
        if ((beats == 1 && subSubDiv == 1 && rest < (processor.context->bpm * 0.003f))){
            infoAndControlArea.infoArea.updateBarWitness(1);
        } else {
            infoAndControlArea.infoArea.updateBarWitness(0.2);   
        }
        if(processor.activeTrack->Playing && subSubDiv == 1 && rest < (processor.context->bpm * 0.003f)){
            infoAndControlArea.infoArea.updateBeatWitness(1);
        }else{
            infoAndControlArea.infoArea.updateBeatWitness(0.2);
        }
    }
}

void OrbishAudioProcessorEditor::resized()
{
	if (nullptr != settingsPage.get()) {
		settingsPage->setBounds(getX(), getY(), getWidth(), getHeight());
	}
    toolCanvas.setBounds(getWidth()/100, getHeight()/12 + 30, getWidth()*.98 , getHeight() - getHeight()/12-50 );

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int containerMargin = 1;
    int controlMargin = 2;
    
	addToGroupButton.setBounds(190, 40, 50, 20);
	removeFromGroupButton.setBounds(190, 65, 50, 20);

    leftSide.setBounds(toolCanvas.removeFromLeft(50).reduced(containerMargin));
    rightSide.setBounds(toolCanvas.removeFromRight(50).reduced(containerMargin));
    leftInnerSide.setBounds(toolCanvas.removeFromLeft(50).reduced(containerMargin));
    rightInnerSide.setBounds(toolCanvas.removeFromRight(50).reduced(containerMargin));
    transportInfoArea.setBounds(toolCanvas.removeFromTop(100).reduced(containerMargin));
    loopDisplayArea.setBounds(toolCanvas.removeFromTop(40).reduced(containerMargin));
    auto rect = toolCanvas.removeFromTop(120).reduced(containerMargin);
    loopConfigArea.setBounds(rect.removeFromRight(rect.getWidth() * .33f).reduced(containerMargin,0));
    rect = toolCanvas;
    rect.removeFromBottom(containerMargin + controlMargin + 1);
    tracksViewport.setBounds(rect.reduced(containerMargin));

    auto r = leftInnerSide.getLocalBounds();
    auto s = r.removeFromTop(r.getHeight()/2);
    inputLevelSlider.setBounds (r.removeFromLeft(r.getWidth() * .5f).reduced(controlMargin + 3));
    r = rightInnerSide.getLocalBounds();
    s = r.removeFromLeft(r.getWidth() * .5f);

    globalVolumeSlider.setBounds(r.removeFromBottom(r.getHeight() * .5f).reduced(controlMargin + 3));
    r = leftSide.getLocalBounds().removeFromBottom(leftSide.getHeight() * .5f).reduced(5, 0);
    inputMeter->setBounds(r.withBottomY(inputLevelSlider.getBottom()));
    r = rightSide.getLocalBounds().removeFromBottom(rightSide.getHeight() * .5f).reduced(5, 0);

    makeTracks();
	groupLabel.setBounds(860, 5, 100, 15);
	groupCombo.setBounds(965, 5, 50, 25);

    auto bounds = getLocalBounds();
    auto headerHeight = 30;
    headerArea.setBounds(bounds.removeFromTop(headerHeight));
    infoAndControlArea.setBounds(bounds.removeFromTop(juce::jmax(80, bounds.getHeight()/2)));
    tracksArea.setBounds(bounds);

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

void OrbishAudioProcessorEditor::setTracksDirty(){
    tracksDirty = true;
}

void OrbishAudioProcessorEditor::toggleLayout(){
    tracksLayoutHorizontal = !tracksLayoutHorizontal;
}

OrbishAudioProcessor& OrbishAudioProcessorEditor::getProcessor(){
    return processor;
}


void OrbishAudioProcessorEditor::makeTracks(){
    
    trackArea.setBounds(tracksViewport.getBounds());
    updateTrackBounds();
    for (auto t : tracks) {
        auto indexOfActiveTrack = t->getIndex();
        if (activeTrack == indexOfActiveTrack){
            infoAndControlArea.infoArea.setGroupNumber(t->Group);
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
    }
    repaint(tracksViewport.getBounds());
    updateTrackAreaSize();
}

void OrbishAudioProcessorEditor::updateTrackAreaSize()
{
    std::vector<int> height; int sum = 0;
    height.reserve(20);
    for (auto track : tracks) {
        auto i = track->getIndex();
        if (i % nbrTracksInARow == 0) { height.push_back(0); }
        height[i / nbrTracksInARow] = jmax(height[i / nbrTracksInARow], track->getHeight());
    }
    for (int i = 0; i < height.size(); ++i) {
        sum += height[i];
    }
    int totalHeight = (tracksLayoutHorizontal) ? tracks.size() * (55) + 5 : (sum)+5;
    int diff = trackArea.getHeight() - totalHeight;
    trackArea.setSize(trackArea.getWidth(), std::max(tracksViewport.getHeight(), totalHeight));
}

void OrbishAudioProcessorEditor::sliderValueChanged (Slider* slider) {
    if(slider == &inputLevelSlider){
        inputLevelSlider.onValueChange = [this]() { changeInputLevel(); };
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
	if (button == &addToGroupButton) {
		button->onClick = [this] {
			makeTracks();
		};
	}
	if (button == &removeFromGroupButton) {
		button->onClick = [this] {
			makeTracks();
		};
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
        infoAndControlArea.infoArea.setLoopNumber(String(tracks[activeTrack]->getActiveLoop() + 1));
        auto navigationControlArea = &infoAndControlArea.controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
        navigationControlArea->setActiveLoop(String(tracks[activeTrack]->getActiveLoop() + 1));
        
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

    infoAndControlArea.infoArea.setTrackNumber(String(activeTrack + 1));
    infoAndControlArea.infoArea.setLoopNumber(String(tracks[activeTrack]->getActiveLoop() + 1));

    auto navigationControlArea = &infoAndControlArea.controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
    navigationControlArea->setActiveTrack(String(activeTrack + 1));
    navigationControlArea->setActiveLoop(String(tracks[activeTrack]->getActiveLoop() + 1));
    trackNumberUpdated = false;
}

void OrbishAudioProcessorEditor::updateTrackBounds()
{
    for(auto track: tracks)
    {
        track->horizontalLayout = tracksLayoutHorizontal;
        auto index = track->getIndex();
        if (!tracksLayoutHorizontal) {
            track->setBounds(5 + (index % nbrTracksInARow * (840 / nbrTracksInARow)), (getTrackRowHeight(index / nbrTracksInARow)) + 5, 840 / nbrTracksInARow, 20 + 18 * track->Loops.size());
        }
        else {
            track->setBounds(5, index * (55) + 5, trackArea.getWidth() - 2 * 5, 50);
        }
    }
}

void OrbishAudioProcessorEditor::askToCreateLoop(){
    auto currentTrack = tracks[activeTrack];
    currentTrack->addLoop(currentTrack->getAudioTrack()->loops.getLast()->Progress);

    makeTracks();
	project.dirty = true;
}

void OrbishAudioProcessorEditor::removeLoop() {
    auto currentTrack = tracks[activeTrack];
    currentTrack->removeLoop();

    makeTracks();
    project.dirty = true;
}

void OrbishAudioProcessorEditor::updatePlaying(int trackNumber)
{
    auto updatedTrack = tracks[trackNumber];
    updatedTrack->setRecording(updatedTrack->getAudioTrack()->Recording);
    updatedTrack->setPlaying(updatedTrack->getAudioTrack()->Playing);
    updatedTrack->setMuted(updatedTrack->getAudioTrack()->Muted);
    updatedTrack->setSoloed(updatedTrack->getAudioTrack()->Soloed);

    updatedTrack->setRecordingArmed(updatedTrack->getAudioTrack()->isRecordingArmed());
    updatedTrack->setPlayArmed(updatedTrack->getAudioTrack()->isPlayArmed());
    updatedTrack->setMutedArmed(updatedTrack->getAudioTrack()->isMuteArmed());
    updatedTrack->setSoloArmed(updatedTrack->getAudioTrack()->isSoloArmed());

    updatedTrack->repaint();
}

void OrbishAudioProcessorEditor::askToCreateTrack(){
    doCreateTrack(tracks.size());
    makeTracks();
    project.dirty = true;
}


void OrbishAudioProcessorEditor::createTrack(){
    // moved to askToCreateTrack
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
        for(auto l : *loops){
            tracks[trackNumber]->addLoop(l->Progress);
		}
	}
}

void OrbishAudioProcessorEditor::removeTrack(int trackNumber){
    trackToRemove = trackNumber;
	project.dirty = true;
    doRemoveTrack();
    makeTracks();
}


void OrbishAudioProcessorEditor::doRemoveTrack(){
    if(trackToRemove > 0 && trackToRemove < tracks.size()){
        if(!tracks[trackToRemove]->isActive()){
            tracks.remove(trackToRemove);
        }
    }
    trackToRemove = 0;
}
