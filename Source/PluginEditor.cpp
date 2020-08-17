#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OrbishAudioProcessorEditor::OrbishAudioProcessorEditor (OrbishAudioProcessor& p, AudioProcessorValueTreeState& apvts): 
    AudioProcessorEditor (&p), 
    projectXml("<project />"), 
    processor (p), 
    thumbnailCache (5), 
    thumbnail (32, formatManager, thumbnailCache), 
    valueTreeState(apvts)
{
    openGLContext.attachTo(*getTopLevelComponent());

    // Define the Look and Feel of the application
    auto editorLookAndFeel = new OrbishLookAndFeel();
    setLookAndFeel(editorLookAndFeel);
    setResizable(true, false);

    // Setup project header
	project = Project();
	setProjectName(project.name);
    
    setUpObserverCallbacks();
    
    processor.context->observer = this;
    thumbnail.addChangeListener (this);
    startTimer (20);
   

    auto buttonControlArea = &infoAndControlArea.controlArea.buttonControlArea;

    // Setup level meters
    buttonControlArea->outputControlArea.setEditor(this);
    outputLevelAttachment.reset(new SliderAttachment(valueTreeState, "outputLevel", buttonControlArea->outputControlArea.outputLevelSlider));

    buttonControlArea->inputControlArea.setEditor(this);
    inputLevelAttachment.reset(new SliderAttachment(valueTreeState, "inputLevel", buttonControlArea->inputControlArea.inputLevelSlider));
    globalMixAttachment.reset(new SliderAttachment(valueTreeState, "globalMix", buttonControlArea->inputControlArea.globalVolumeSlider));

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
    
    // Grouping buttons attachments

    auto groupControlArea = &infoAndControlArea.controlArea.thumbnailAndGroupArea.groupControlArea;
    addToGroupAttachment.reset(new ButtonAttachment(valueTreeState, "addToGroup", groupControlArea->addToGroupButton));
    removeFromGroupAttachment.reset(new ButtonAttachment(valueTreeState, "removeFromGroup", groupControlArea->removeFromGroupButton));
    groupAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(valueTreeState, "selectGroup", groupControlArea->groupCombo));

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
    for (auto group : processor.groups){
        groupControlArea->groupCombo.addItem(group->Name, group->Index + 1);
    }

    auto navigationControlArea = &buttonControlArea->modeAndNavigationControlArea.navigationControlArea;

    // Loop button attachments
    previousLoopAttachment.reset (new ButtonAttachment (valueTreeState, "previousLoop", navigationControlArea->previousLoopButton));
    nextLoopAttachment.reset (new ButtonAttachment (valueTreeState, "nextLoop", navigationControlArea->nextLoopButton));
    newLoopAttachment.reset (new ButtonAttachment (valueTreeState, "newLoop", navigationControlArea->newLoopButton));
    removeLoopAttachment.reset (new ButtonAttachment (valueTreeState, "removeLoop", navigationControlArea->removeLoopButton));
    
    // Track button attachments
    previousTrackAttachment.reset (new ButtonAttachment (valueTreeState, "previousTrack", navigationControlArea->previousTrackButton));
    nextTrackAttachment.reset (new ButtonAttachment (valueTreeState, "nextTrack", navigationControlArea->nextTrackButton));
    newTrackAttachment.reset (new ButtonAttachment (valueTreeState, "newTrack", navigationControlArea->newTrackButton));
    removeTrackAttachment.reset (new ButtonAttachment (valueTreeState, "removeTrack", navigationControlArea->removeTrackButton));

    // Audio Display
    infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.setEditor(this);
    headerArea.setEditor(this);

    addAndMakeVisible(headerArea);
    addAndMakeVisible(infoAndControlArea);
    addAndMakeVisible(tracksArea);

    tracksViewport.setViewedComponent(&trackArea, false);
    tracksViewport.setScrollBarsShown(true, false);
    tracksViewport.setScrollBarThickness(20);
    tracksViewport.setScrollBarPosition(false, true);
    addAndMakeVisible(tracksViewport);

    // Create tracks
    for (auto track : processor.tracks) {
        doCreateTrack(track->Index);
    }

    setSize (1300, 800);
}


void OrbishAudioProcessorEditor::showSettingsPage() {
	settingsPage = std::make_shared<SettingsPage>(processor.loggingActive, processor.context->maxUndoHistory, nbrTracksInARow, processor.context->delayCompensation );
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
	doChangeTrack();
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
    infoAndControlArea.controlArea.thumbnailAndGroupArea.groupControlArea.groupCombo.setSelectedId(1);
	project.newProject = true;
	project.dirty = true;
}
void OrbishAudioProcessorEditor::setProjectName(String name) {
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
	el->writeTo(project.directory.getChildFile(project.name + ".xml"), XmlElement::TextFormat());
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
	doChangeTrack();
    
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
    infoAndControlArea.controlArea.thumbnailAndGroupArea.groupControlArea.groupCombo.setSelectedId(1);
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


OrbishAudioProcessorEditor::~OrbishAudioProcessorEditor(){
	processor.guiAlive = false;
    Thread::sleep(200);
    setLookAndFeel(nullptr);
}

void OrbishAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* ){
           
}

void OrbishAudioProcessorEditor::timerCallback(){
    // update thumbnail from ringbuffer
	BufferForVisualisation* b;
	if (processor.context->xchange->readBufferQueue->read_available()) {
		processor.context->xchange->readBufferQueue->pop(b);
		updateLoopVisualiser(*b->buffer, b->numSamples);
		String s = String(pointer_sized_int(b));
		delete b;
	}
    
    // run ACT methods in response to flags set by notification callbacks
    updatePlayHead();
    createTrack();
    changeTrack();
    createLoop();
    removeTrack();
    changeLoop();
    removeLoop();
    updatePlayState();
    
    
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
    infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.inputDisplay.clear();
}



void OrbishAudioProcessorEditor::toggleStop(){
    playHeadPosition = 0;
    reverseState = ToggleState::Off;
    updatePlayHead();
}

void OrbishAudioProcessorEditor::toggleClear(){
    infoAndControlArea.controlArea.buttonControlArea.transportControlArea.playButton.setToggleState(false, NotificationType::dontSendNotification);
    thumbnail.reset(processor.context->audioInputsCount, processor.context->sampleRate, thumbnail.getNumSamplesFinished());
    playHeadPosition = 0;
    reverseState = ToggleState::Off;
    updatePlayHead();}

void OrbishAudioProcessorEditor::toggleReverse(){
    reverseState = (reverseState == On)?Off:On;
}




//==============================================================================
void OrbishAudioProcessorEditor::paint (Graphics& g){
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    if (thumbnail.getNumChannels() == 0 || thumbnail.getTotalLength() <= 0) {
        infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.setFileLoaded(false);
    }
    else {
        infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.setFileLoaded(true);
    }

    paintInfoSection(g);
    g.setColour(Colours::black);
    g.drawRoundedRectangle(tracksViewport.getBoundsInParent().expanded(5).toFloat(), 4.0f, 1.0f);

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

void OrbishAudioProcessorEditor::paintInfoSection(Graphics&){
    String timeSig = String(processor.context->info->timeSigNumerator) + "/" + String(processor.context->info->timeSigDenominator);
    if(timeSig != infoAndControlArea.infoArea.getTimeSignature()){
        infoAndControlArea.infoArea.setTimeSignature(timeSig);
    }
    auto bpmStr = "bpm: " + String(processor.context->info->bpm, 1);
    if(bpmStr != infoAndControlArea.infoArea.getBeatsPerMinute()){
        infoAndControlArea.infoArea.setBeatsPerMinute(bpmStr);
    }
    
    float totalSubDiv = float(processor.samplesToBeats(*processor.activeTrack->CurrentPlayingIndex));
    float garbage;
    int bars = int(totalSubDiv / processor.context->info->timeSigNumerator + 1);
    int beats = int(fmod(totalSubDiv,processor.context->info->timeSigNumerator) + 1);
    float rest = std::modf(totalSubDiv, &garbage);
    int subSubDiv = int(rest) * 4 + 1;
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

void OrbishAudioProcessorEditor::resized() {
	if (nullptr != settingsPage.get()) {
		settingsPage->setBounds(getX(), getY(), getWidth(), getHeight());
	}

    auto bounds = getLocalBounds();
    auto headerHeight = 30;
    headerArea.setBounds(bounds.removeFromTop(headerHeight));
    infoAndControlArea.setBounds(bounds.removeFromTop(juce::jmax(80, bounds.getHeight()/2)));
    tracksViewport.setBounds(bounds.reduced(15));
    
    makeTracks();
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

OrbishAudioProcessor* OrbishAudioProcessorEditor::getProcessor(){
    return &processor;
}

AudioThumbnail* OrbishAudioProcessorEditor::getThumbnailInstance(){
    return &thumbnail;
}

bool OrbishAudioProcessorEditor::getReverseState(){
    if (reverseState == Off ){
        return true;
    }
    else {
        return false;
    }
}

bool OrbishAudioProcessorEditor::isTrackLayoutHorizontal(){
    return tracksLayoutHorizontal;
}


void OrbishAudioProcessorEditor::makeTracks(){
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
}

void OrbishAudioProcessorEditor::updateLoopVisualiser(const AudioBuffer<float>& buffer, int numSamples){
        thumbnail.reset (processor.context->audioInputsCount, processor.context->sampleRate, numSamples);
    if (numSamples > 0) {
        thumbnail.addBlock (0, buffer, 0, numSamples);
    }
    infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.repaint();
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
		 processor.context->maxUndoHistory = int(slider->getValue()); 
	}
	if (slider == &settingsPage->latencySlider) {
		processor.context->delayCompensation = int(slider->getValue());
	}
    if (slider == &settingsPage->tracksPerRowSlider) {
         nbrTracksInARow = int(slider->getValue());
	}
}

void OrbishAudioProcessorEditor::buttonClicked(Button* button){
	if (button == &settingsPage->closeSettingsButton) {
		closeSettingsPage();
	}
}

void OrbishAudioProcessorEditor::sliderValueChanged(Slider* ){
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
						doChangeTrack();
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





void OrbishAudioProcessorEditor::updateTrackBounds(){
    auto tracksHeight = 0; 
    if (tracksLayoutHorizontal){
        tracksHeight = tracks.size() * 55 + 10;
    }
    else {
        std::map<int, int> rowHeights;
        for (auto track : tracks){
            auto row = track->getIndex() / nbrTracksInARow + 1;
            auto rowMaxLoops = rowHeights.find(row);
            if (rowMaxLoops == rowHeights.end()){
                rowHeights.insert(rowHeights.end(), std::pair<int, int>(row, track->Loops.size()));
            }
            else{
                if (rowMaxLoops->second < track->Loops.size()){
                    rowHeights.erase(rowMaxLoops);
                    rowHeights.insert(rowHeights.end(), std::pair<int, int>(row, track->Loops.size()));
                }
            }
        }
        for (auto row: rowHeights){
            tracksHeight += row.second*18+20;
        }
        tracksHeight += 10;
    }
    trackArea.setBounds(tracksViewport.getX(), tracksViewport.getY(), tracksViewport.getWidth(), tracksHeight);

    for (auto track : tracks) {
        track->horizontalLayout = tracksLayoutHorizontal;
        auto index = track->getIndex();
        if (!tracksLayoutHorizontal) {
            track->setBounds(5 + (index % nbrTracksInARow * (tracksViewport.getMaximumVisibleWidth() / nbrTracksInARow)), (getTrackRowHeight(index / nbrTracksInARow)) + 5, tracksViewport.getMaximumVisibleWidth() / nbrTracksInARow - 10, 20 + 18 * track->Loops.size());
        }
        else {
            track->setBounds(5, index * (55) + 5, tracksViewport.getMaximumVisibleWidth() - 10, 50);
        }
    }
}


// Setup observers
void OrbishAudioProcessorEditor::setUpObserverCallbacks(){
    updatePlayPosition = &Observer::askToUpdatePlayHead;
    handleMidi = &Observer::askToHandleMidiMessages;
    newTrack = &Observer::askToCreateTrack;
    trackChange = &Observer::askToChangeTrack;
    newLoop = &Observer::askToCreateLoop;
    trackRemoval = &Observer::askToRemoveTrack;
    loopChange = &Observer::askToChangeLoop;
    loopRemoval = &Observer::askToRemoveLoop;
    playChanged = &Observer::askToUpdatePlayState;
}

// ============= ASK TO ACT ==============
void OrbishAudioProcessorEditor::askToUpdatePlayHead(int position, bool reverse){
    reverseState = (reverse) ? ToggleState::On : ToggleState::Off;
    playHeadPosition = position;
    flags |= CallBackFlags::shouldUpdatePlayHead;
}

void OrbishAudioProcessorEditor::askToHandleMidiMessages(const MidiBuffer& midiMessages){
    processedMidi = midiMessages;
    flags |= CallBackFlags::shouldHandleMidi;
}

void OrbishAudioProcessorEditor::askToCreateTrack(){
    flags |= CallBackFlags::shouldCreateTrack;
}

void OrbishAudioProcessorEditor::askToChangeTrack(int trackNumber){
    nextTrackNumber = trackNumber;
    flags |= CallBackFlags::shouldChangeTrack;
}

void OrbishAudioProcessorEditor::askToCreateLoop(){
    flags |= CallBackFlags::shouldCreateLoop;
}

void OrbishAudioProcessorEditor::askToRemoveTrack(int trackNumber){
    trackToRemove = trackNumber;
    flags |= CallBackFlags::shouldRemoveTrack;
}

void OrbishAudioProcessorEditor::askToChangeLoop(int trackNumber, int loopNumber){
    trackToChange = trackNumber;
    nextLoopNumber = loopNumber;
    flags |= CallBackFlags::shouldChangeLoop;
}

void OrbishAudioProcessorEditor::askToRemoveLoop() {
    flags |= CallBackFlags::shouldRemoveLoop;
}


void OrbishAudioProcessorEditor::askToUpdatePlayState(int trackNumber)
{
    updatedTrackNumber = trackNumber;
    flags |= CallBackFlags::shouldUpdatePlayState;
}

// =============== ACT ===================

void OrbishAudioProcessorEditor::updatePlayHead(){
    if(flags & CallBackFlags::shouldUpdatePlayHead){
        doUpdatePlayHead();
        flags ^= CallBackFlags::shouldUpdatePlayHead;
    }
}

void OrbishAudioProcessorEditor::createTrack(){
    if(flags & CallBackFlags::shouldCreateTrack){
        doCreateTrack(tracks.size());
        makeTracks();
        project.dirty = true;
        flags ^= CallBackFlags::shouldCreateTrack;
    }
}

void OrbishAudioProcessorEditor::changeTrack(){
    if(flags & CallBackFlags::shouldChangeTrack){
        doChangeTrack();
        flags ^= CallBackFlags::shouldChangeTrack;

    }
}

void OrbishAudioProcessorEditor::createLoop(){
    if(flags & CallBackFlags::shouldCreateLoop){
        doCreateLoop();
        flags ^= CallBackFlags::shouldCreateLoop;
    }
}

void OrbishAudioProcessorEditor::removeTrack(){
    if(flags & CallBackFlags::shouldRemoveTrack){
        project.dirty = true;
        doRemoveTrack();
        makeTracks();
        flags ^= CallBackFlags::shouldRemoveTrack;
    }
}

void OrbishAudioProcessorEditor::changeLoop(){
    if(flags & CallBackFlags::shouldChangeLoop){
        doChangeLoop();
        flags ^= CallBackFlags::shouldChangeLoop;
    }
}

void OrbishAudioProcessorEditor::removeLoop() {
    if(flags & CallBackFlags::shouldRemoveLoop){
        doRemoveLoop();
        flags ^= CallBackFlags::shouldRemoveLoop;
    }
}

void OrbishAudioProcessorEditor::updatePlayState()
{
    if(flags & CallBackFlags::shouldUpdatePlayState){
        doUpdatePlayState();
        flags ^= CallBackFlags::shouldUpdatePlayState;
    }
}

//================ DO ACT ==============

void OrbishAudioProcessorEditor::doUpdatePlayHead(){
    infoAndControlArea.controlArea.thumbnailAndGroupArea.thumbnailArea.updatePlayHead(playHeadPosition);
}


void OrbishAudioProcessorEditor::doHandleMidiMessages(const MidiBuffer& midiMessages){
    MidiBuffer myMidi;
    int time;
    MidiMessage m;
    String s;
    if(!(flags & CallBackFlags::shouldHandleMidi)){
        return;
    }
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
        
        myMidi.addEvent (m, time);
    }
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
        trackArea.addAndMakeVisible(t);
    }
    else {
        for(auto l : *loops){
            tracks[trackNumber]->addLoop(l->Progress);
        }
    }
}

void OrbishAudioProcessorEditor::doChangeTrack(){
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
}

void OrbishAudioProcessorEditor::doCreateLoop(){
    auto currentTrack = tracks[activeTrack];
    currentTrack->addLoop(currentTrack->getAudioTrack()->loops.getLast()->Progress);

    makeTracks();
    project.dirty = true;
}


void OrbishAudioProcessorEditor::doRemoveTrack(){
    if(trackToRemove > 0 && trackToRemove < tracks.size()){
        if(!tracks[trackToRemove]->isActive()){
            tracks.remove(trackToRemove);
        }
    }
    trackToRemove = 0;
}

void OrbishAudioProcessorEditor::doChangeLoop(){
    if (tracks.size() > trackToChange && trackToChange >= 0 && nextLoopNumber >= 0) {
        auto t = tracks[trackToChange];
        t->setActiveLoop(nextLoopNumber);
        infoAndControlArea.infoArea.setLoopNumber(String(tracks[activeTrack]->getActiveLoop() + 1));
        auto navigationControlArea = &infoAndControlArea.controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
        navigationControlArea->setActiveLoop(String(tracks[activeTrack]->getActiveLoop() + 1));
        
        tracksDirty = true;
    }
}

void OrbishAudioProcessorEditor::doRemoveLoop(){
    auto currentTrack = tracks[activeTrack];
        currentTrack->removeLoop();
        makeTracks();
        project.dirty = true;
}

void OrbishAudioProcessorEditor::doUpdatePlayState()
{
    auto updatedTrack = tracks[updatedTrackNumber];
	if (nullptr == updatedTrack)return;
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
