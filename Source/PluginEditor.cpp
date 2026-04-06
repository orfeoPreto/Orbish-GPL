#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdexcept>

// Debug: track which callback is currently executing
static const char* g_currentCallback = "none";


//==============================================================================
OrbishAudioProcessorEditor::OrbishAudioProcessorEditor (OrbishAudioProcessor& p, AudioProcessorValueTreeState& apvts): 
AudioProcessorEditor (&p),
flags(0),
projectXml("<project />"),
processor (p),
valueTreeState(apvts),
nextLayerNumber(-1)
{
#if DEBUG_LOG
    //logMessage("HiRes ticks per sec:" + String(Time::getHighResolutionTicksPerSecond()));
#endif
    //logMessage("entering editor constructor");
    openGLContext = makeOpenGLContext(true, true, 1);
    //    setAlpha(0.5);
    setOpaque(true);
    
    openGLContext->setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext->setRenderer (this);
    
    // Define the Look and Feel of the application
    auto editorLookAndFeel = new OrbishLookAndFeel();
    setLookAndFeel(editorLookAndFeel);
    thumbnail = setupThumbnail();
    infoAndControlArea = std::make_unique<InfoAndControlArea>();
    setupWitness();
    infoAndControlArea->infoArea.setWitness(witness);
    infoAndControlArea->infoArea.setSubDivs(hostPosition);
    // Add custom OpenGL Components
    references.push_back (std::make_shared<OpenGLComponentReference>(thumbnail.get()));
    references.push_back (std::make_shared<OpenGLComponentReference>(infoAndControlArea->infoArea.witness.get()));
    infoAndControlArea->controlArea.thumbnailAndGroupArea.thumbnailArea.setEditor(this);
    references.push_back(std::make_shared<OpenGLComponentReference>( infoAndControlArea->controlArea.buttonControlArea.inputControlArea.inputMeter.meterDisplay.get()));
    references.push_back(std::make_shared<OpenGLComponentReference>(infoAndControlArea->controlArea.buttonControlArea.outputControlArea.outputMeter.meterDisplay.get()));
    headerArea.setEditor(this);
    addAndMakeVisible(headerArea);
    addAndMakeVisible(*infoAndControlArea);
    infoAndControlArea->infoArea.setOpenGLContext(openGLContext);
    setResizable(true, true);
    // Setup project header
    project = Project();
    setProjectName(project.name);
    startTimer (20);
    setUpObserverCallbacks();
    processor.context->observer = this;
    setupTracks();
    
    auto buttonControlArea = &infoAndControlArea->controlArea.buttonControlArea;
    setupGlobalButtons();
    setupModeControls(buttonControlArea);
    setupGroupingButtons();
    setupTransportControls(buttonControlArea);
    setupNavigationControls(buttonControlArea);
    setSize (1300, 800);
    markActiveTrackForRefresh(true);
    //logMessage("leaving editor constructor");
    
    // Attach OpenGL context AFTER all child components are set up,
    // to avoid texture creation before the GL context is ready
    openGLContext->attachTo (*this);
    
}

std::shared_ptr<OpenGLContext> OrbishAudioProcessorEditor::getOpenGLContext(){
    return openGLContext;
}

//void OrbishAudioProcessorEditor::setLookAndFeel (LookAndFeel* const newLookAndFeel){
//    Component::setLookAndFeel(newLookAndFeel);
//    headerArea.setLookAndFeel(newLookAndFeel);
//}

void OrbishAudioProcessorEditor::setupTracks(){
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
    if (processor.activeTrack != nullptr && (int)processor.activeTrack->Index < tracks.size()) {
        auto t = tracks[processor.activeTrack->Index];
        t->setActive(true);
    }
}


void OrbishAudioProcessorEditor::setupNavigationControls(ButtonControlArea* buttonControlArea){
    auto navigationControlArea = &buttonControlArea->modeAndNavigationControlArea.navigationControlArea;
    navigationControlArea->nextLoopButton.addListener(this);
    navigationControlArea->previousLoopButton.addListener(this);
    navigationControlArea->nextTrackButton.addListener(this);
    navigationControlArea->previousTrackButton.addListener(this);
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
}

void OrbishAudioProcessorEditor::setupTransportControls(ButtonControlArea* buttonControlArea){
    // transport Button attachments
    auto transportControlArea = &buttonControlArea->transportControlArea;
    transportControlArea->setEditor(this);
    
    recordAttachment.reset (new ButtonAttachment (valueTreeState, "record", transportControlArea->recordButton));
    clickAttachment.reset (new ButtonAttachment (valueTreeState, "click", infoAndControlArea->infoArea.clickButton));
    
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
}

void OrbishAudioProcessorEditor::setupModeControls(ButtonControlArea *buttonControlArea){
    // Mode control attachments
    auto modeControlArea = &buttonControlArea->modeAndNavigationControlArea.modeControlArea;
    
    recModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "mode", modeControlArea->recModeCombo));
    snapModeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "snap", modeControlArea->snapModeCombo));
    fixedSizeAttachment.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "fixed", modeControlArea->fixedSizeBeatsCombo));
}

std::unique_ptr<OpenGLAudioThumbnail> OrbishAudioProcessorEditor::setupThumbnail(){
    // Audio Display
    auto tn = std::make_unique<OpenGLAudioThumbnail>(playHeadPosition, false);
    tn->setOpenGLContext(openGLContext, false);
    tn->setLookAndFeel(&getLookAndFeel());
    //    tn->setTopLevelComponent(this);
    tn->setDisplayType(WaveDisplayType::kLayered);
    tn->setName("main");
    tn->setContext(processor.context);
    tn->setNumberVisibleLayers(processor.context->numVisibleLayers);
    return tn;
}

void OrbishAudioProcessorEditor::setupWitness(){
    witness = std::make_unique<OpenGLClickWitness>(hostPosition);
    witness->setContext(processor.context);
}

void OrbishAudioProcessorEditor::setupGlobalButtons(){
    auto buttonControlArea = &infoAndControlArea->controlArea.buttonControlArea;
    
    // Setup level meters
    buttonControlArea->outputControlArea.setEditor(this);
    outputLevelAttachment.reset(new SliderAttachment(valueTreeState, "outputLevel", buttonControlArea->outputControlArea.outputLevelSlider));
    globalMixAttachment.reset(new SliderAttachment(valueTreeState, "globalMix", buttonControlArea->outputControlArea.globalVolumeSlider));
    
    buttonControlArea->inputControlArea.setEditor(this);
    inputLevelAttachment.reset(new SliderAttachment(valueTreeState, "inputLevel", buttonControlArea->inputControlArea.inputLevelSlider));
    
    // Global Button attachments
    auto globalControlArea = &buttonControlArea->globalControlArea;
    globalControlArea->setEditor(this);
    
    muteAllAttachment.reset (new ButtonAttachment (valueTreeState, "muteAll", globalControlArea->muteAllButton));
    stopAllAttachment.reset (new ButtonAttachment (valueTreeState, "stopAll", globalControlArea->stopAllButton));
    startAllAttachment.reset (new ButtonAttachment (valueTreeState, "startAll", globalControlArea->startAllButton));
    pauseAllAttachment.reset (new ButtonAttachment (valueTreeState, "pauseAll", globalControlArea->pauseAllButton));
    clearAllAttachment.reset (new ButtonAttachment (valueTreeState, "resetAll", globalControlArea->clearAllButton));
}

void OrbishAudioProcessorEditor::setupGroupingButtons(){
    // Grouping buttons attachments
    auto groupControlArea = &infoAndControlArea->controlArea.groupControlArea;
    groupControlArea->setEditor(this);
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
    groupControlArea->groupCombo.setSelectedId(1);
}

void OrbishAudioProcessorEditor::newOpenGLContextCreated()
{
    for (auto reference : references){
        reference->asRenderer->newOpenGLContextCreated();
        //        auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
    }
    
}

void OrbishAudioProcessorEditor::openGLContextClosing()
{
    
    for (auto reference : references)
        reference->asRenderer->openGLContextClosing();
}

void OrbishAudioProcessorEditor::renderOpenGL()
{
    g_currentCallback = "renderOpenGL";
    
    counter++;
    stamp = Time::getApproximateMillisecondCounter();
    if (stamp - startStamp > 1000) {
        startStamp = stamp;
        frameRate = counter;
        counter = 0;
    }
    
    //    auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
    lock.enterRead();
    for (auto reference : references){
        try {
        if (!reference->asOpenGLComponent->isInitialized()) {
            reference->asOpenGLComponent->init();
        }
        if(nullptr == reference ||
           nullptr == reference->asOpenGLComponent->shader)continue;
        if(reference->asComponent->isVisible()){
            //logMessage("component type: "+ String(reference->asComponent->getName()));
            //logMessage("component id: " + String(reference->asComponent->getComponentID()));
            //logMessage("before if");
            
            //            auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
            if (reference->asOpenGLComponent->shaderName == "thumbnail-playhead"){
                //logMessage(" if shader name: thumbnail-playhead");
                
                if (flags & CallBackFlags::shouldRemoveLoop
                    || flags & CallBackFlags::shouldRemoveTrack) {
                    continue;
                }
                if(reference->asComponent->getName() != "main"){
                    //logMessage(" if >getName() != main");
                    
                    auto widgetPosition = reference->asComponent->getPosition();
                    auto widgetPosRelativeToViewport = reference->asComponent->getLocalPoint(&tracksViewport,Point<float>(widgetPosition.getX(), widgetPosition.getY()));
                    //logMessage("2component type: " + String(reference->asComponent->getName()));
                    //logMessage("2component id: " + String(reference->asComponent->getComponentID()));
                    if(tracksViewport.getWidth() < widgetPosRelativeToViewport.getX()
                       || tracksViewport.getHeight() < widgetPosRelativeToViewport.getY()
                       || 0 < widgetPosRelativeToViewport.getX() - reference->asComponent->getWidth()
                       || 0 < widgetPosRelativeToViewport.getY() - reference->asComponent->getHeight()
                       ){
                        continue;
                    }
                }
            }
            
            //             grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
            //            if(reference->asComponent->getComponentID() == "Loop1"){
            //                DBG(reference->asOpenGLComponent->getTotalLength());
            //            }
            //logMessage("befor renderopengl of component");
            
            reference->asRenderer->renderOpenGL();
            //             grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
            
        }
        } catch (const std::exception& e) {
            DBG("Exception in renderOpenGL component: " + String(e.what()));
        } catch (...) {
            DBG("Unknown exception in renderOpenGL component");
        }
        
    }
    lock.exitRead();
    auto afterStamp = Time::getApproximateMillisecondCounter();
    if (afterStamp-stamp < 33) {
        //        DBG("time in render:" + String(afterStamp-stamp));
        std::this_thread::sleep_for(std::chrono::milliseconds(33 - (afterStamp-stamp)));
    }
    //logMessage("leaving renderOpenGL");
    
}

void OrbishAudioProcessorEditor::removeReference(OpenGLComponent* r){
    for(auto i=0;i<references.size();i++){
        if(r == (references)[i]->asComponent){
            lock.enterWrite();
            references.std::vector<std::shared_ptr<OpenGLComponentReference> >::erase(references.std::vector<std::shared_ptr<OpenGLComponentReference> >::begin() + i);
            //                DBG(String((uint64)references[i]->asComponent));
            lock.exitWrite();
        }
    }
}

void OrbishAudioProcessorEditor::showSettingsPage() {
    //    thumbnail->stop();
    //    thumbnail->setVisible(false);
    settingsPage = std::make_shared<SettingsPage>(processor.context->postMixMonitoring, processor.context->loggingActive, processor.context->maxUndoHistory, nbrTracksInARow, processor.context->delayCompensation, processor.context->numVisibleLayers );
    settingsPage->addListener(this);
    settingsPage->setLookAndFeel(&getLookAndFeel());
    addAndMakeVisible(*settingsPage);
    resized();
}

void OrbishAudioProcessorEditor::closeSettingsPage() {
    settingsPage.reset();
    //    thumbnail->start();
    //    thumbnail->setVisible(true);
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
//    alert->show
//    if (alert->runModalLoop() != 0) // is they picked 'ok'
//        {
//        setProjectName(alert->getTextEditorContents("projectNameBox"));
//        alert->exitModalState(0);
//        alert->setVisible(false);
//        }
    totalReset();
    infoAndControlArea->controlArea.groupControlArea.groupCombo.setSelectedId(1);
    project.newProject = true;
    project.dirty = true;
    
}

void OrbishAudioProcessorEditor::totalReset(){
    int zero = 0;
    processor.processStopAllChange();
    processor.changeTrack(zero);
    doChangeTrack();
    
    for (auto i = processor.tracks.size() - 1;i > 0;i--) {
        askToRemoveTrack(i);
        processor.removeTrack(i);
        removeTrack();
    }
    processor.activeTrack->ChangeLoopBefore(zero);
    processor.activeTrack->ChangeLoopAfter();
    while (processor.tracks[0]->loops.size() > 1){
        askToRemoveLoop();
        processor.tracks[0]->RemoveLoopAfter();
        removeLoop();
    }
    processor.initGroups();
    infoAndControlArea->controlArea.groupControlArea.groupCombo.setSelectedId(1);
    processor.activeTrack->processResetChange();
    doRefreshThumbnail(false);
}

void OrbishAudioProcessorEditor::setProjectName(String name) {
    infoAndControlArea->infoArea.setProjectName(name);
    project.name = name;
}

void OrbishAudioProcessorEditor::saveProject() {
    for(auto track: processor.tracks){
        if(track->IsPlaying() || track->Recording){
            return;
        }
    }
    std::unique_ptr<File> tempDir;
    if (project.newProject || !project.directory.exists()) {
        File dir = File(File::getSpecialLocation(File::userHomeDirectory));
        if (dir.getChildFile("Orbish").exists()) {
            dir = dir.getChildFile("Orbish");
        }
        FileChooser fc("Save As", dir,"*.orb", false);
        {
        project.directory = fc.getResult().getParentDirectory();
        setProjectName(fc.getResult().getFileNameWithoutExtension().replace(".orb","", true));
        if (!project.directory.exists()) {
            juce::Result result = project.directory.createDirectory();
            if (result.failed()) {
                //logMessage(result.getErrorMessage());
                return;
            }
        }
        tempDir = std::make_unique<File>(project.directory.getFullPathName()
                                         + File::getSeparatorString()
                                         + fc.getResult().getFileNameWithoutExtension().replace(".orb","",true));
        if(!fc.getResult().exists()){
            juce::Result result = fc.getResult().createDirectory();
            if (result.failed()) {
                //logMessage(result.getErrorMessage());
                return;
            }
        }
        }
    }else{
        tempDir = std::make_unique<File>(project.directory.getChildFile(project.name));
        juce::Result result = tempDir->createDirectory();
        if (result.failed()) {
            //logMessage(result.getErrorMessage());
            return;
        }
    }
    auto package = project.directory.getChildFile(project.name + ".orb");
    ZipFile::Builder zip{};
    loopTree = std::make_unique<ValueTree>(String("project"));
    loopTree->setProperty("name", project.name, nullptr);
    
    std::unique_ptr<ValueTree> tsv(std::make_unique<ValueTree>(String("tracks")));
    for (auto track: tracks) {
        ValueTree tv(String("track"));
        if(nullptr == track ){
            //logMessage("nullpointer in save project (no track)");
            return;
        }
        tv.setProperty("index", track->getIndex(), nullptr);
        tv.setProperty("name", track->getName(), nullptr);
        for (auto loop: track->Loops) {
            ValueTree lv(String("loop"));
            if(nullptr == loop ){
                //logMessage("nullpointer in save project (no loop)");
                return;
            }
            if(nullptr == track->getAudioTrack()){
                //logMessage("nullpointer in save project (no audio track)");
                //logMessage(String("Track index:") + String(track->getIndex()));
                return;
            }
            if(loop->getIndex() >= track->getAudioTrack()->loops.size()){
                //logMessage("exceeded bounds of audio loop array");
                //logMessage(String("Loop index:") + String(loop->getIndex()));
                return;
            }
            for (auto k = 0;k <= track->getAudioTrack()->loops[loop->getIndex()]->CurrentTop;++k) {
                auto result = saveBuffer(track->getIndex(), loop->getIndex(), k, *tempDir
                                         , track->getName() + "_" + String(loop->getIndex()) + "_" + String(k), true);
                if (result.length() == 0)continue;
                auto file = tempDir->getChildFile(result);
                zip.addFile(file, 1);
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
    loopTree->setProperty("bpm", processor.context->info->getBpm().orFallback(0), nullptr);
    std::unique_ptr<XmlElement> el = loopTree->createXml();
    auto file = tempDir->getChildFile(project.name + ".xml");
    el->writeTo(file, XmlElement::TextFormat());
    zip.addFile(file, 1);
    if (package.existsAsFile()) {
        package.deleteFile();
    }
    auto stream = package.createOutputStream();
    zip.writeToStream(*(stream.get()), nullptr);
    tempDir->deleteRecursively();
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
    thumbnail->setVisible(false);
    AlertWindow* alert = new AlertWindow(title, message, icon, this);
    alert->setColour(alert->backgroundColourId, Colours::black);
    alert->addButton(firstButtonText, 1, juce::KeyPress(KeyPress::returnKey));
    alert->addButton(secondButtonText, 0, juce::KeyPress(KeyPress::escapeKey));
    alert->enterModalState(true, nullptr, false);
    addAndMakeVisible(alert);
    alert->centreWithSize(400, 200);
//    if (alert->runModalLoop() != 0) // is they picked 'ok'
//        {
//        alert->exitModalState(0);
//        alert->setVisible(false);
//        thumbnail->setVisible(true);
//        return true;
//        }
//    else {
//        alert->exitModalState(0);
//        alert->setVisible(false);
//        thumbnail->setVisible(true);
//        return false;
//    }
    return false;
}

void OrbishAudioProcessorEditor::askUserToOpenFile() {
    if (project.dirty) {
        bool save = showDialogWindow("Save current project!"
                                     , "Do you want to save the current project?"
                                     , AlertWindow::AlertIconType::QuestionIcon
                                     , "Yes", "No");
        if (save) { saveProject(); };
    }
    totalReset();
    File dir = File(File::getSpecialLocation(File::userHomeDirectory));
    dir = dir.getChildFile("Orbish");
    FileChooser fc("Open Project", dir, "*.orb",false);
//    if (fc.browseForFileToOpen()){
//        project.directory = fc.getResult().getParentDirectory();
//        openFile(fc.getResult());
//    }
    project.newProject = false;
    project.dirty = false;
    markActiveTrackForRefresh(true);
}

bool OrbishAudioProcessorEditor::openFile(const File& file) {
    ZipFile zip{file};
    File tempDir = project.directory.getChildFile(file.getFileNameWithoutExtension());
    auto result = zip.uncompressTo(tempDir);
    File f = tempDir.getChildFile(file.getFileNameWithoutExtension() + ".xml");
    loadingTracks = true;
    loopTree = std::make_shared<ValueTree>(String("project"));
    if (auto xml = parseXML(f)) {
        loopTree = std::make_shared<ValueTree>(ValueTree::fromXml(*xml));
        setProjectName(loopTree->getProperty("name"));
        try {
            auto loader = std::make_unique<TrackLoader>(processor);
            loader->loadFromValueTree(loopTree.get());
        }
        catch (...) {
            //logMessage("Problem loading file");
        }
        for (int i = 0;i < processor.tracks.size(); ++i) {
            doCreateTrack(i);
            tracks[activeTrackIdx]->setActive(true);
        }
        float bpm = float(loopTree->getProperty("bpm"));
        if(String(bpm, 0) != String(processor.context->info->getBpm().orFallback(0), 0)){
            showDialogWindow("Tempo changed", "The current bpm (" + String(processor.context->info->getBpm().orFallback(0)) + ") doesn't match the one of the loaded project (" + String(bpm, 0) + "). \nPlease change the host's bpm.", AlertWindow::AlertIconType::WarningIcon, "Ok, will change", "Never mind");
        }
    }
    tempDir.deleteRecursively();
    return false;
}


OrbishAudioProcessorEditor::~OrbishAudioProcessorEditor(){
    stopTimer();
    processor.context->observer = nullptr;
    processor.guiAlive = false;
    Thread::sleep(200);
    setLookAndFeel(nullptr);
    openGLContext->setContinuousRepainting (false);
    openGLContext->detach();
    // File dir(File::getSpecialLocation(File::tempDirectory));
    // dir.deleteRecursively();
}

void OrbishAudioProcessorEditor::changeListenerCallback (ChangeBroadcaster* ){
    
}


void OrbishAudioProcessorEditor::updateLoopVisualiser(std::shared_ptr<BufferForVisualisation> b){
    if (processor.activeTrack == nullptr || activeTrackIdx >= tracks.size()) return;
    thumbnail->setSourceLoop(processor.activeTrack->ActiveLoop);
    auto t = tracks[activeTrackIdx];
    t->thumbnail->setSourceLoop(processor.activeTrack->ActiveLoop);
    if(t->Loops.size()>0 && t->getActiveLoopIdx() < t->Loops.size()){
        t->Loops[t->getActiveLoopIdx()]->thumbnail->setSourceLoop(processor.activeTrack->ActiveLoop);
    }
    
    if (nullptr != processor.activeTrack->getActivePlaybackLayer()) {
        thumbnail->setActiveLayer(processor.activeTrack->getActivePlaybackLayer()->index);
        //        tracks[activeTrack]->thumbnail->setActiveLayer(processor.activeTrack->getActivePlaybackLayer()->index);
        //        tracks[activeTrack]->Loops[activeLoop]->thumbnail->setActiveLayer(processor.activeTrack->getActivePlaybackLayer()->index);
    }
    else {
        thumbnail->setActiveLayer(0);
        //        tracks[activeTrack]->thumbnail->setActiveLayer(0);
    }
    thumbnail->sampleRate = processor.context->sampleRate;
    t->thumbnail->sampleRate = processor.context->sampleRate;
    if(t->Loops.size()>0 && t->getActiveLoopIdx() < t->Loops.size()){
        t->Loops[activeLoop]->thumbnail->sampleRate = processor.context->sampleRate;
    }
}

void OrbishAudioProcessorEditor::timerCallback(){
    g_currentCallback = "timerCallback";
    try {
    // run ACT methods in response to flags set by notification callbacks
    updatePlayHead();
    createTrack();
    changeTrack();
    changePitch();
    createLoop();
    removeTrack();
    changeLoop();
    changeLayer();
    removeLoop();
    updatePlayState();
    refreshThumbnail();
    //    DBG(frameRate);
    paintInfoSection();
    if (processor.activeTrack == nullptr) return;
    auto transportControlArea = &infoAndControlArea->controlArea.buttonControlArea.transportControlArea;
    auto modeControlArea = &infoAndControlArea->controlArea.buttonControlArea.modeAndNavigationControlArea.modeControlArea;
    if (processor.activeTrack->Recording || processor.activeTrack->Playing) {
        modeControlArea->recModeCombo.setEnabled(true);
        transportControlArea->bounceButton.setEnabled(false);
    }
    else {
        modeControlArea->recModeCombo.setEnabled(true);
        transportControlArea->bounceButton.setEnabled(true);
    }
    } catch (const std::exception& e) {
        DBG("Exception in timerCallback: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in timerCallback");
    }
}

String OrbishAudioProcessorEditor::saveBuffer(int trackIdx
                                              , int loopIdx
                                              , int layerIdx
                                              , File dir
                                              , String name
                                              , bool overwrite){
    auto t = processor.tracks[trackIdx];
    if (*t->LoopDuration == 0) {
        return String();
    }
    auto l = t->loops[loopIdx];
    if (nullptr == l)return String();
    auto localLayers = l->Layers;
    if (!localLayers || (int)localLayers->size() <= layerIdx) { return String(); }
    std::shared_ptr<Layer> layer = (*localLayers)[layerIdx];
    if (layer->Buffer->getNumSamples() == 0) {
        return String();
    }
    if (!dir.exists()) {
        juce::Result result = dir.createDirectory();
        if (result.failed()) {
            //logMessage(result.getErrorMessage());
            return "";
        }
    }
    File file = dir.getChildFile(name + ".wav");
    if (overwrite && file.exists()) {
        file.deleteFile();
    }
    file = dir.getNonexistentChildFile(name ,".wav");
    WavAudioFormat form;
    std::unique_ptr<AudioFormatWriter> writer;
    writer.reset(form.createWriterFor(new FileOutputStream(file),
                                      processor.context->sampleRate,
                                      layer->Buffer->getNumChannels(),
                                      24,
                                      {},
                                      0));
    
    writer->writeFromAudioSampleBuffer(*layer->Buffer, 0, *t->LoopDuration);
    
    return file.getFullPathName();
}

String OrbishAudioProcessorEditor::saveBufferFromLoop(int trackIdx, int loopIdx) {
    auto track = tracks[trackIdx];
    File dir = File(File::getSpecialLocation(File::userHomeDirectory));
    dir = dir.getChildFile("Orbish");
    dir = dir.getChildFile("UsedByHost");
    std::shared_ptr<AudioBuffer<float> > temp;
    if(processor.context->xchange->readBufferQueue->read_available() > 0){
        temp = processor.context->xchange->readBufferQueue->front();
        temp->setSize(temp->getNumChannels(), *track->getAudioTrack()->LoopDuration);
        for (auto layer: *track->getAudioTrack()->loops[loopIdx]->Layers) {
            for(auto c=0;c<temp->getNumChannels();++c){
                temp->addFrom(c, 0, layer->Buffer->getReadPointer(c),*track->getAudioTrack()->LoopDuration);
            }
        }
        return saveBuffer(temp, dir, track->getName() + "_" + String(loopIdx), false);
    }
    return "";
}

String OrbishAudioProcessorEditor::saveBuffer(std::shared_ptr<AudioBuffer<float> > buffer
    , File dir
    , String name
    , bool overwrite) {

    if (!dir.exists()) {
        juce::Result result = dir.createDirectory();
        if (result.failed()) {
            //logMessage(result.getErrorMessage());
            return "";
        }
    }
    File file = dir.getChildFile(name + ".wav");
    if (overwrite && file.exists()) {
        file.deleteFile();
    }
    file = dir.getNonexistentChildFile(name, ".wav");
    WavAudioFormat form;
    std::unique_ptr<AudioFormatWriter> writer;
    writer.reset(form.createWriterFor(new FileOutputStream(file),
        processor.context->sampleRate,
        buffer->getNumChannels(),
        24,
        {},
        0));

    writer->writeFromAudioSampleBuffer(*buffer, 0, buffer->getNumSamples());

    return file.getFullPathName();
}

void OrbishAudioProcessorEditor::toggleRecord(){
    project.dirty = true;
    infoAndControlArea->controlArea.thumbnailAndGroupArea.thumbnailArea.inputDisplay.clear();
}



void OrbishAudioProcessorEditor::toggleStop(){
    playHeadPosition = 0;
    reverseState = ToggleState::Off;
    updatePlayHead();
}

void OrbishAudioProcessorEditor::toggleClear(){
    infoAndControlArea->controlArea.buttonControlArea.transportControlArea.playButton.setToggleState(false, NotificationType::dontSendNotification);
    //  thumbnail.reset(processor.context->audioInputsCount, processor.context->sampleRate, thumbnail.getNumSamplesFinished());
    thumbnail->clear();
    tracks[activeTrackIdx]->thumbnail->clear();
    playHeadPosition = 0;
    reverseState = ToggleState::Off;
    updatePlayHead();
}

void OrbishAudioProcessorEditor::toggleReverse(){
    reverseState = (reverseState == On)?Off:On;
}


void OrbishAudioProcessorEditor::logMessage(String message){
    processor.context->logMessage(message);
}


//==============================================================================
void OrbishAudioProcessorEditor::paint (Graphics& g){
    g_currentCallback = "paint";
    try {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    
    paintInfoSection();
    g.setColour(Colours::black);
    g.drawRoundedRectangle(tracksViewport.getBoundsInParent().expanded(5).toFloat(), 4.0f, 1.0f);
    
    auto transportControlArea = &infoAndControlArea->controlArea.buttonControlArea.transportControlArea;
    
    if(processor.activeTrack != nullptr && activeTrackIdx < tracks.size()) {
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
        if(processor.activeTrack->Muted != tracks[activeTrackIdx]->isMuted()) tracksDirty = true;
        if(processor.activeTrack->Soloed != tracks[activeTrackIdx]->isSoloed()) tracksDirty = true;
        if(processor.activeTrack->isMuteArmed() != tracks[activeTrackIdx]->isMutedArmed()) tracksDirty = true;
        if(processor.activeTrack->isSoloArmed() != tracks[activeTrackIdx]->isSoloArmed()) tracksDirty = true;
    }
    
    if (tracksDirty) {
        trackArea.repaint();
        tracksDirty = false;
    }
    } catch (const std::exception& e) {
        DBG("Exception in paint: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in paint");
    }
}

void OrbishAudioProcessorEditor::paintInfoSection(){
    int tsNum = processor.context->info->getTimeSignature().hasValue()
        ? processor.context->info->getTimeSignature()->numerator : processor.context->timeSigTop;
    int tsDen = processor.context->info->getTimeSignature().hasValue()
        ? processor.context->info->getTimeSignature()->denominator : processor.context->timeSigBottom;
    if (tsNum <= 0) tsNum = 4;
    if (tsDen <= 0) tsDen = 4;

    String timeSig = String(tsNum) + "/" + String(tsDen);
    if(timeSig != infoAndControlArea->infoArea.getTimeSignature()){
        infoAndControlArea->infoArea.setTimeSignature(timeSig);
    }
    auto bpmStr = "bpm: " + String(processor.context->info->getBpm().orFallback(processor.context->bpm), 1);
    if(bpmStr != infoAndControlArea->infoArea.getBeatsPerMinute()){
        infoAndControlArea->infoArea.setBeatsPerMinute(bpmStr);
    }
    
    if (processor.activeTrack == nullptr || activeTrackIdx >= tracks.size()) return;

    float totalSubDiv = (processor.activeTrack->CurrentPlayingIndex != nullptr)
        ? float(processor.context->samplesToBeats(*processor.activeTrack->CurrentPlayingIndex))
        : 0.0f;
    float garbage;
    int bars = int(totalSubDiv / tsNum + 1);
    int beats = int(fmod(totalSubDiv, tsNum) + 1);
    float rest = std::modf(totalSubDiv, &garbage);
    int subSubDiv = int(rest) * 4 + 1;
    String progress = String(bars) + ". " + String(beats) + ". " + String(subSubDiv);
    if (progress !=  infoAndControlArea->infoArea.getProgress()) {
        infoAndControlArea->infoArea.setProgress(progress);
    }
    auto trackNbr = "Track: "+String(processor.activeTrack->Index + 1);
    if (trackNbr != infoAndControlArea->infoArea.getTrackNumber()) {
        infoAndControlArea->infoArea.setTrackNumber(trackNbr);
    }
    auto loopNbr = "Loop: "+String(tracks[activeTrackIdx]->getActiveLoopIdx() + 1);
    if (loopNbr != infoAndControlArea->infoArea.getLoopNumber()) {
        infoAndControlArea->infoArea.setLoopNumber(loopNbr);
    }
    auto layer = processor.activeTrack->getActivePlaybackLayer();
    auto layerIdx = (nullptr != layer)?layer->index+1:0;
    auto layerNbr = "Layer: " + String(layerIdx);
    if (layerNbr != infoAndControlArea->infoArea.getLayerNumber()) {
        infoAndControlArea->infoArea.setLayerNumber(layerNbr);
    }
    auto grp = processor.getTrackGroup(processor.activeTrack);
    String groupName = "";
    Colour grpCol(findColour(Label::ColourIds::textColourId));
    if (nullptr != grp) {
        groupName = grp->Name;
        grpCol = groupColours[grp->Index];
    }
    groupName = "Group: " + String(groupName);
    if (infoAndControlArea->infoArea.getGroupNumber() != groupName){
        infoAndControlArea->infoArea.setGroupNumber(groupName);
        infoAndControlArea->infoArea.setGroupColour(grpCol);
    }
}

void OrbishAudioProcessorEditor::resized() {
    if (nullptr != settingsPage.get()) {
        settingsPage->setBounds(getX(), getY(), getWidth(), getHeight());
    }
    auto bounds = getLocalBounds();
    auto headerHeight = 30;
    headerArea.setBounds(bounds.removeFromTop(headerHeight));
    infoAndControlArea->setBounds(bounds.removeFromTop(juce::jmax(80, bounds.getHeight()/2)));
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
    auto idx = std::min(tmpRowIdx * nbrTracksInARow, tracks.size()-1);
    return (rowIdx>0)?maxNbrLoops * tracks[0]->loopHeight + tracks[idx]->getY() + tracks[0]->loopHeight :0;
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

std::shared_ptr<OpenGLAudioThumbnail> OrbishAudioProcessorEditor::getThumbnailInstance(){
    return thumbnail;
}

bool OrbishAudioProcessorEditor::getReverseState(){
    switch(reverseState){
        case On:
            return false;
            break;
        case Off:
            return true;
            break;
        default:
            return false;
            break;
    }
}

bool OrbishAudioProcessorEditor::isTrackLayoutHorizontal(){
    return tracksLayoutHorizontal;
}


void OrbishAudioProcessorEditor::makeTracks(){
    updateTrackBounds();
    for (auto t : tracks) {
        auto indexOfActiveTrack = t->getIndex();
        auto tr = t->getAudioTrack();
        auto grp = processor.getTrackGroup(tr);
        String groupName = "";
        Colour grpCol = Colours::black;
        if (nullptr != grp) {
            groupName = grp->Name;
            grpCol = groupColours[grp->Index];
        }
        t->setGroup(groupName, grpCol);
        if (activeTrackIdx == indexOfActiveTrack) {
            infoAndControlArea->infoArea.setGroupNumber(t->getGroup());
        }
    }
}


void OrbishAudioProcessorEditor::clicked(Button* button) {
    if (button == &settingsPage->closeSettingsButton) {
        closeSettingsPage();
    }
    if (button == &settingsPage->activateLoggingButton) {
        processor.context->loggingActive = button->getToggleState();
    }
    if (button == &settingsPage->monitoringButton) {
        processor.context->postMixMonitoring = button->getToggleState();
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
    if (slider == &settingsPage->visibleLayersSlider) {
        thumbnail->setNumberVisibleLayers(processor.context->numVisibleLayers = int(slider->getValue()));
    }
}

void OrbishAudioProcessorEditor::buttonClicked(Button* button){
    g_currentCallback = "buttonClicked";
    try {
    auto navigationControlArea = &infoAndControlArea->controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
    if (button == &settingsPage->closeSettingsButton) {
        closeSettingsPage();
    }

    if (button == &navigationControlArea->nextLoopButton) {
        if (activeTrackIdx >= tracks.size()) return;
        auto t = tracks[activeTrackIdx];
        if (loopBecomingActive >= 0 && loopBecomingActive < t->Loops.size()) {
            t->Loops[loopBecomingActive]->unHighlightBecomingActive();
        }
        else {
            loopBecomingActive = t->getActiveLoopIdx();
        }
        if (t->Loops.size() > 0)
            loopBecomingActive = (loopBecomingActive + 1) % t->Loops.size();
        if (loopBecomingActive < t->Loops.size()) {
            t->Loops[loopBecomingActive]->highlightBecomingActive();
        }
    }
    if (button == &navigationControlArea->previousLoopButton) {
        if (activeTrackIdx >= tracks.size()) return;
        auto t = tracks[activeTrackIdx];
        if (loopBecomingActive >= 0
            && loopBecomingActive < t->Loops.size() ) {
            t->Loops[loopBecomingActive]->unHighlightBecomingActive();
        }
        else {
            loopBecomingActive = t->getActiveLoopIdx();
        }
        if (t->Loops.size() > 0) {
            auto newIdx = (loopBecomingActive - 1) % t->Loops.size();
            if ((loopBecomingActive - 1) < 0) {
                newIdx = newIdx + t->Loops.size();
            }
            loopBecomingActive = newIdx;
        }
        if (loopBecomingActive < t->Loops.size()) {
            t->Loops[loopBecomingActive]->highlightBecomingActive();
        }
    }
    } catch (const std::exception& e) {
        DBG("Exception in buttonClicked: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in buttonClicked");
    }
}

void OrbishAudioProcessorEditor::sliderValueChanged(Slider* ){
}

void OrbishAudioProcessorEditor::mouseDown(const MouseEvent &event) {
    g_currentCallback = "mouseDown";
    try {
    for (auto track : tracks) {
        if(event.eventComponent->getParentComponent() == track){
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
    } catch (const std::exception& e) {
        DBG("Exception in mouseDown: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in mouseDown");
    }
}

void OrbishAudioProcessorEditor::mouseDrag(const MouseEvent& event) {
    g_currentCallback = "mouseDrag";
    try {
    for (auto track : tracks) {
        if (event.eventComponent->getParentComponent() == track) {
            if (activeLoop < 0 || activeLoop >= track->Loops.size()) continue;
            auto l = track->Loops[activeLoop];
            if (!this->isDragAndDropActive()) {
                this->startDragging("track", l, Image(), true);
            }
            auto s = saveBufferFromLoop(track->getIndex(), l->getIndex());
            if (s.length() == 0){
                return;
            }
            StringArray files = { s };
            performExternalDragDropOfFiles({ s }, false, {  });
        }
        for (auto loop  : track->Loops) {
            if (event.eventComponent->getParentComponent() == loop) {
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
    } catch (const std::exception& e) {
        DBG("Exception in mouseDrag: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in mouseDrag");
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
            tracksHeight += row.second*18+25;
        }
        tracksHeight += 20;
    }
    trackArea.setBounds(tracksViewport.getX(), tracksViewport.getY(), tracksViewport.getWidth(), tracksHeight);
    
    for (auto track : tracks) {
        track->horizontalLayout = tracksLayoutHorizontal;
        auto index = track->getIndex();
        if (!tracksLayoutHorizontal) {
            track->setBounds(5 + (index % nbrTracksInARow * (tracksViewport.getMaximumVisibleWidth() / nbrTracksInARow)), (getTrackRowHeight(index / nbrTracksInARow)) + 10, tracksViewport.getMaximumVisibleWidth() / nbrTracksInARow - 10, 25 + 18 * track->Loops.size());
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
    layerChange = &Observer::askToChangeLayer;
    loopRemoval = &Observer::askToRemoveLoop;
    playChanged = &Observer::askToUpdatePlayState;
    pitchChanged = &Observer::askToChangePitch;
    hostPositionChanged = &Observer::askToUpdateHostPosition;
    layersRefreshed = &Observer::askToRefreshThumbnail;
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
    tracksWithLoopChange[trackNumber]=loopNumber;
    flags |= CallBackFlags::shouldChangeLoop;
}

void OrbishAudioProcessorEditor::askToChangeLayer(int trackNumber, int layerNumber){
    trackToChange = trackNumber;
    nextLayerNumber = layerNumber;
    flags |= CallBackFlags::shouldChangeLayer;
}

void OrbishAudioProcessorEditor::askToRemoveLoop() {
    flags |= CallBackFlags::shouldRemoveLoop;
}


void OrbishAudioProcessorEditor::askToUpdatePlayState(int trackNumber)
{
    updatedTrackNumber = trackNumber;
    flags |= CallBackFlags::shouldUpdatePlayState;
}

void OrbishAudioProcessorEditor::askToUpdateHostPosition(int hostPos) {
    ////logMessage("before setting host position");
    hostPosition = float(processor.context->samplesToBeats(hostPos));
    flags |= CallBackFlags::shouldUpdateHostPosition;
};

void OrbishAudioProcessorEditor::askToRefreshThumbnail() {
    ////logMessage("before setting host position");
    flags |= CallBackFlags::shouldRefreshThumbnail;
};

void OrbishAudioProcessorEditor::askToChangePitch(int trackNumber) {
    flags |= CallBackFlags::shouldChangePitch;
};

// =============== ACT ===================

void OrbishAudioProcessorEditor::refreshThumbnail() {
    doRefreshThumbnail(flags& CallBackFlags::shouldRefreshThumbnail);
    flags &= ~CallBackFlags::shouldRefreshThumbnail;
}

void OrbishAudioProcessorEditor::updatePlayHead(){
    if(flags & CallBackFlags::shouldUpdatePlayHead){
        doUpdatePlayHead();
        flags &= ~CallBackFlags::shouldUpdatePlayHead;
    }
}

void OrbishAudioProcessorEditor::updateHostPosition(){
    if(flags & CallBackFlags::shouldUpdateHostPosition){
        doUpdateHostPosition();
        flags &= ~CallBackFlags::shouldUpdatePlayHead;
    }
}

void OrbishAudioProcessorEditor::createTrack(){
    if(flags & CallBackFlags::shouldCreateTrack){
        doCreateTrack(tracks.size());
        project.dirty = true;
        flags &= ~CallBackFlags::shouldCreateTrack;
    }
}

void OrbishAudioProcessorEditor::changeTrack(){
    if(flags & CallBackFlags::shouldChangeTrack){
        doChangeTrack();
        flags &= ~CallBackFlags::shouldChangeTrack;
    }
}

void OrbishAudioProcessorEditor::markActiveTrackForRefresh(bool shouldRefresh){
    if (processor.activeTrack == nullptr) return;
    processor.activeTrack->refresh = shouldRefresh;
}

void OrbishAudioProcessorEditor::createLoop(){
    if(flags & CallBackFlags::shouldCreateLoop){
        doCreateLoop();
        flags &= ~CallBackFlags::shouldCreateLoop;
    }
}


void OrbishAudioProcessorEditor::removeTrack(){
    if(flags & CallBackFlags::shouldRemoveTrack){
        project.dirty = true;
        doRemoveTrack();
        makeTracks();
        flags &= ~CallBackFlags::shouldRemoveTrack;
    }
}

void OrbishAudioProcessorEditor::changeLoop(){
    if(flags & CallBackFlags::shouldChangeLoop){
        doChangeLoop();
        flags &= ~CallBackFlags::shouldChangeLoop;
    }
}

void OrbishAudioProcessorEditor::changeLayer(){
    if(flags & CallBackFlags::shouldChangeLayer){
        doChangeLayer();
        flags &= ~CallBackFlags::shouldChangeLayer;
    }
}

void OrbishAudioProcessorEditor::changePitch(){
    if(flags & CallBackFlags::shouldChangePitch){
        doChangePitch();
        flags &= ~CallBackFlags::shouldChangePitch;
    }
}

void OrbishAudioProcessorEditor::removeLoop() {
    if(flags & CallBackFlags::shouldRemoveLoop){
        doRemoveLoop();
        flags &= (~CallBackFlags::shouldRemoveLoop);
    }
}

void OrbishAudioProcessorEditor::updatePlayState()
{
    if(flags & CallBackFlags::shouldUpdatePlayState){
        doUpdatePlayState();
        flags &= ~CallBackFlags::shouldUpdatePlayState;
    }
}

//================ DO ACT ==============

void OrbishAudioProcessorEditor::doRefreshThumbnail(bool refresh) {
    std::shared_ptr<BufferForVisualisation> b;
    if (refresh) {
        thumbnail->resetVisualizationBuffers();
        playHeadPosition = 0;
        markActiveTrackForRefresh(false);
    }
    updateLoopVisualiser(b);
}
void OrbishAudioProcessorEditor::doUpdatePlayHead(){
    thumbnail->setReverse(getReverseState());
    tracks[activeTrackIdx]->thumbnail->setReverse(getReverseState());
    for (auto l: tracks[activeTrackIdx]->Loops) {
        l->thumbnail->setReverse(getReverseState());
    }
}

void OrbishAudioProcessorEditor::doUpdateHostPosition(){
    infoAndControlArea->infoArea.repaint();
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
    if (processor.tracks.size() <= trackNumber || processor.tracks.size()<=0) return;
    auto audioTrack = processor.tracks[trackNumber];
    auto loops = &audioTrack->loops;
    
    std::vector<std::atomic<float>*> prg;
    for(auto l : *loops){
        prg.push_back(&(l->Progress));
    }
    trackNumber =  std::min(tracks.size(),trackNumber);
    if (tracks.size() <= trackNumber) {
        DBG("doCreateTrack first if branch.\nreferences.size: "  + String(references.size()));
        auto* t = new TrackComponent(trackNumber, prg, tracksLayoutHorizontal, audioTrack);
        t->addMouseListener(this, true);
        t->setOpenGLContext(openGLContext, false);
        tracks.add(t);
        trackArea.addAndMakeVisible(t);
        String id = "Track" + String(t->getIndex());
        t->thumbnail->setComponentID(id);
        t->thumbnail->setContext(processor.context);
        references.push_back (std::make_shared<OpenGLComponentReference>(t->thumbnail.get()));
        for(auto l : *loops){
            if ((int)l->Index >= t->Loops.size()) continue;
            auto lc = t->Loops[l->Index];
            lc->thumbnail->setSourceLoop(l);
            lc->thumbnail->setContext(processor.context);
            references.push_back (std::make_shared<OpenGLComponentReference>(lc->thumbnail.get()));
        }
        t->thumbnail->setSourceLoop(audioTrack->ActiveLoop);
        t->setActiveLoopIdx(t->getAudioTrack()->ActiveLoop->Index);
        DBG("end of branch.\nreferences.size: " + String(references.size()));
        
    }
    else {
        DBG("doCreateTrack second if branch.\nreferences.size: " + String(references.size()));
        auto t = tracks[trackNumber];
        while(t->Loops.size() > 0){
            lock.enterWrite();
            removeReference(t->Loops.getLast()->thumbnail.get());
            t->removeLoop();
            lock.exitWrite();
        }
        for(auto l : *loops){
            t->addLoop(l->Progress);
            String id = "Loop" + String(t->Loops.getLast()->getIndex());
            t->Loops.getLast()->thumbnail->setComponentID(id);
            t->Loops.getLast()->thumbnail->setSourceLoop(l);
            references.push_back (std::make_shared<OpenGLComponentReference>(t->Loops.getLast()->thumbnail.get()));
        }
        t->thumbnail->setSourceLoop(audioTrack->ActiveLoop);
        references.push_back (std::make_shared<OpenGLComponentReference>(t->thumbnail.get()));
        t->setActiveLoopIdx(t->getAudioTrack()->ActiveLoop->Index);
        DBG("end of branch.\nreferences.size: " + String(references.size()));
        
    }
    makeTracks();
    
}

void OrbishAudioProcessorEditor::doChangeTrack(){
    if(nextTrackNumber >= tracks.size() || nextTrackNumber < 0){
        return;
    }
    for(auto t : tracks){
        t->setActive(nextTrackNumber == t->getIndex());
        t->setActiveLoopIdx(t->getAudioTrack()->ActiveLoop->Index);
        
    }
    tracksDirty = true;
    activeTrackIdx=nextTrackNumber;
    
    infoAndControlArea->infoArea.setTrackNumber(String(activeTrackIdx + 1));
    infoAndControlArea->infoArea.setLoopNumber(String(tracks[activeTrackIdx]->getActiveLoopIdx() + 1));
    auto navigationControlArea = &infoAndControlArea->controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
    navigationControlArea->setActiveTrack(String(activeTrackIdx + 1));
    navigationControlArea->setActiveLoop(String(tracks[activeTrackIdx]->getActiveLoopIdx() + 1));
}

void OrbishAudioProcessorEditor::doCreateLoop(){
    auto currentTrack = tracks[activeTrackIdx];
    auto audioTrack = currentTrack->getAudioTrack();
    if (audioTrack->loops.size() <= currentTrack->Loops.size()) return;
    currentTrack->addLoop(audioTrack->loops.getLast()->Progress);
    auto l = currentTrack->Loops.getLast();
    String id = "Loop" + String(l->getIndex());
    l->thumbnail->setComponentID(id);
    l->thumbnail->setSourceLoop(audioTrack->loops.getLast());
    l->thumbnail->setContext(processor.context);
    
    references.push_back (std::make_shared<OpenGLComponentReference>(l->thumbnail.get()));
    
    makeTracks();
    project.dirty = true;
}


void OrbishAudioProcessorEditor::doRemoveTrack(){
    if(trackToRemove > 0 && trackToRemove < tracks.size()){
        auto t = tracks[trackToRemove];
        if(!t->isActive()){
            for (auto i=0; i<references.size(); ++i) {
                if (references[i]->asOpenGLComponent == t->thumbnail.get()) {
                    lock.enterWrite();
                    references.erase(references.begin() + i);
                    lock.exitWrite();
                }
            }
            for(auto l :t->Loops){
                removeReference(l->thumbnail.get());
            }
            tracks.remove(trackToRemove);
        }
    }
    if(trackToRemove == -1){
        while (tracks.size() > 0) {
            tracks.remove(tracks.size()-1);
        }
        doCreateTrack(0);
        activeTrackIdx = 0;
    }
    trackToRemove = 0;
}

void OrbishAudioProcessorEditor::doChangeLoop(){
    loopBecomingActive = -1;
    for(auto tidx:tracksWithLoopChange){
        if (tidx.first < 0 || tidx.first >= tracks.size()){
            return;
        }
        auto t = tracks[tidx.first];
        if (tidx.second < 0 || tidx.second >= t->Loops.size()){
            return;
        }
        t->setActiveLoopIdx(tidx.second);
        t->thumbnail->setOffset(t->audioTrack->ActiveLoop->Progress);
        infoAndControlArea->infoArea.setLoopNumber(String(t->getActiveLoopIdx() + 1));
        auto navigationControlArea = &infoAndControlArea->controlArea.buttonControlArea.modeAndNavigationControlArea.navigationControlArea;
        navigationControlArea->setActiveLoop(String(t->getActiveLoopIdx() + 1));
        tracksDirty = true;
    }
    tracksWithLoopChange.clear();
}

void OrbishAudioProcessorEditor::doChangeLayer(){
    if(nextLayerNumber>=0){
        thumbnail->setActiveLayer(nextLayerNumber);
        tracks[activeTrackIdx]->thumbnail->setActiveLayer(nextLayerNumber);
    }
}

void OrbishAudioProcessorEditor::doRemoveLoop(){
    auto currentTrack = tracks[activeTrackIdx];
    do{
        auto currentLoop = currentTrack->Loops.getLast();
        DBG("audioTrack->loops.size - before: ");
        DBG(currentTrack->getAudioTrack()->loops.size());
        DBG("uiTrack->Loops.size - before: ");
        DBG(currentTrack->Loops.size());
        if (currentLoop->getIndex() < currentTrack->getAudioTrack()->loops.size() ||
            currentLoop->getIndex() == currentTrack->getAudioTrack()->ActiveLoop->Index) {
            return;
        }
        lock.enterWrite();
        currentLoop->thumbnail->setSourceLoop(nullptr);
        removeReference(currentLoop->thumbnail.get());
        currentTrack->removeLoop();
        makeTracks();
        project.dirty = true;
        lock.exitWrite();
    }while (currentTrack->getAudioTrack()->loops.size()<currentTrack->Loops.size());
    DBG("audioTrack->loops.size: ");
    DBG(currentTrack->getAudioTrack()->loops.size());
    DBG("uiTrack->Loops.size: ");
    DBG(currentTrack->Loops.size());
}

void OrbishAudioProcessorEditor::doUpdatePlayState(){
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

void OrbishAudioProcessorEditor::doChangePitch(){
    try {
    for(auto track: processor.tracks){
        if(track->timeStretcher) track->timeStretcher->setPitchRatio(track->perTrackPitchRatio);
    }
    } catch (const std::exception& e) {
        DBG("Exception in doChangePitch: " + String(e.what()));
    } catch (...) {
        DBG("Unknown exception in doChangePitch");
    }
}
