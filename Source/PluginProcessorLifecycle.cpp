// PluginProcessorLifecycle.cpp
// Split compilation unit for OrbishAudioProcessor - lifecycle, boilerplate,
// JUCE overrides, state persistence, and utility methods.

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace std;

//==============================================================================
const String OrbishAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrbishAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool OrbishAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool OrbishAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double OrbishAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrbishAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int OrbishAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrbishAudioProcessor::setCurrentProgram(int )
{

}

const String OrbishAudioProcessor::getProgramName(int )
{
    return {};
}

void OrbishAudioProcessor::changeProgramName(int, const String& )
{
}

FFAU::LevelMeterSource* OrbishAudioProcessor::getInputMeterSource()
{
    return &inputMeterSource;
}

FFAU::LevelMeterSource* OrbishAudioProcessor::getOutputMeterSource()
{
    return &outputMeterSource;
}

void OrbishAudioProcessor::askTrackChange(int trackNumber) {
    auto p = parameters.getParameter("trackSelect");
    p->setValueNotifyingHost(p->convertTo0to1(trackNumber));
}

void OrbishAudioProcessor::askLoopChange(int loopNumber) {
    auto p = parameters.getParameter("loopSelect");
    p->setValueNotifyingHost(p->convertTo0to1(float(loopNumber)));
}

void OrbishAudioProcessor::addTrack(bool active) {
    auto track = new Track(tracks.size(), active, parameters, context, guiAlive);
    context->progress.add(track->Progress);
    // Initialize time stretcher for new track
    if (context->sampleRate > 0 && context->audioInputsCount > 0) {
        track->timeStretcher = std::make_unique<TimeStretchRubberBand>();
        track->timeStretcher->prepare(context->sampleRate, context->audioInputsCount,
                                       context->samplesPerBlock);
        track->timeStretcher->primeWithSilence();
        track->stretcherStartDelay = track->timeStretcher->getStartDelay();
        track->stretcherDelayConsumed = 0;
        track->stretchInputBuffer = std::make_unique<AudioBuffer<float>>(
            int(context->audioInputsCount), context->samplesPerBlock * 4);
    }
    tracks.add(track);
}

void OrbishAudioProcessor::cleanup() {
    while (tracks.size() > 0) {
        tracks.removeLast();
    }
    context->progress.clear();
    activeTrack  = nullptr;
}

void OrbishAudioProcessor::initParams() {

}

//==============================================================================
void OrbishAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
       //context->logMessage("begin prepareToPlay");
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if (context == nullptr) {
        context = std::make_shared<OrbishContext>();
    }
    if (getTotalNumOutputChannels() != context->audioOutputsCount
        || getTotalNumInputChannels() != context->audioInputsCount) {
        context->audioInputsCount = getTotalNumInputChannels();
        context->audioOutputsCount = getTotalNumOutputChannels();
        context->buffer->setSize(context->audioInputsCount, samplesPerBlock);
        context->inputBuffer->setSize(context->audioInputsCount, samplesPerBlock);
        context->xchange->resetBuffers = true;
        init();
    }

    context->allocatedLength = int(sampleRate * context->defaultLoopLength);
    //context->samplesPerBlock = samplesPerBlock;
    context->fadeInc = float(context->fadeTime * sampleRate);
    context->samplesPerMinute = float(sampleRate * 60.0);

    context->secondsPerSample = 1.0f / sampleRate;

    if (samplesPerBlock != context->samplesPerBlock) {
        context->samplesPerBlock = samplesPerBlock;
        context->maxDelta = sampleRate * context->samplesPerBlock / 2000000000;
        context->buffer->setSize(context->audioInputsCount, samplesPerBlock);
        context->inputBuffer->setSize(context->audioInputsCount, samplesPerBlock);
        if(nullptr == activeTrack){
            init();
        }
        for (auto track : tracks) {
            track->EndFadeOffset = min(samplesPerBlock, track->EndFadeOffset);
            track->BeginFadeOffset = min(samplesPerBlock, track->BeginFadeOffset);
            track->realignment->setBlockSize(samplesPerBlock);
            track->realignment->setRealigned(true);
            // Initialize or re-initialize the time stretcher
            if (!track->timeStretcher)
                track->timeStretcher = std::make_unique<TimeStretchRubberBand>();
            track->timeStretcher->prepare(int(sampleRate), int(context->audioInputsCount), samplesPerBlock);
            track->timeStretcher->primeWithSilence();
            track->stretcherStartDelay = track->timeStretcher->getStartDelay();
            track->stretcherDelayConsumed = 0;
            track->stretchInputBuffer = std::make_unique<AudioBuffer<float>>(
                int(context->audioInputsCount), samplesPerBlock * 4);
        }
        context->xchange->resetBuffers = true;
    }
    if (sampleRate != context->sampleRate) {
        context->sampleRate = int(sampleRate);
        init();
    }
    context->samplesPerBlock = samplesPerBlock;
    context->fadeTime = int(min(float(samplesPerBlock), float(sampleRate) * .01f));
    context->sampleRate = int(sampleRate);
    // Initialize delay compensation from the latency parameter (ms → samples)
    if (auto* p = parameters.getRawParameterValue("latency"))
        context->delayCompensation = int(p->load() * sampleRate * 0.001f);
      //context->logMessage("end prepareToPlay");
#if DEBUG_LOG
    //context->logMessage("sample rate set to:" + String(context->sampleRate));
#endif

}

void OrbishAudioProcessor::init(){
    cleanup();
    Thread::sleep(100);
    //create tracks
    addTrack(true);
    activeTrack = tracks.getLast();
    // activeTrack->ActiveLoop->activeLayer = activeTrack->Layers->back();
    *activeTrack->LoopDuration = 0;
    if (guiAlive) {
        (context->observer->*(context->observer->trackRemoval)) (-1);
    }
   // stretcher = make_unique<RubberBandStretcher>(context->sampleRate, context->audioOutputsCount, RubberBandStretcher::OptionProcessRealTime | RubberBandStretcher::OptionTransientsCrisp);
    //context->fadeTime = int(context->samplesPerBlock * .05);
    //context->buffer->setSize(context->audioInputsCount, samplesPerBlock);

    //                                        logMessage("outputchannels:" + String(context->audioInputsCount));
    //                                        logMessage("context buffer channels:" + String(context->buffer->getNumChannels()));
    int holdValue = 30;
    inputMeterSource.setMaxHoldMS(holdValue);
    outputMeterSource.setMaxHoldMS(holdValue);
    inputMeterSource.resize(context->audioInputsCount, holdValue);
    outputMeterSource.resize(context->audioInputsCount, holdValue);

    messenger = MessageManager::getInstance();
    context->init = true;
    parameters.addParameterListener("globalMix", this);
    parameters.addParameterListener("clickLevel", this);
    parameters.addParameterListener("latency", this);
    parameters.addParameterListener("muteAll", this);
    parameters.addParameterListener("trackSelect", this);
    parameters.addParameterListener("nextTrack", this);
    parameters.addParameterListener("previousTrack", this);
    parameters.addParameterListener("newTrack", this);
    parameters.addParameterListener("click", this);
    parameters.addParameterListener("removeTrack", this);
    parameters.addParameterListener("stopAll", this);
    parameters.addParameterListener("startAll", this);
    parameters.addParameterListener("pauseAll", this);
    parameters.addParameterListener("resetAll", this);
    parameters.addParameterListener("selectGroup", this);
    parameters.addParameterListener("addToGroup", this);
    parameters.addParameterListener("removeFromGroup", this);
}

void OrbishAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OrbishAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    // Allow zero inputs (e.g. standalone mode with no mic input)
    if (layouts.getMainInputChannelSet() != AudioChannelSet::disabled()
        && layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void OrbishAudioProcessor::processMidi(const MidiBuffer& midi) {
    std::shared_ptr<OrbishContext> ctxt = context;
    if (guiAlive) {
            (ctxt->observer->*(context->observer->handleMidi))(midi);
    }

}

void OrbishAudioProcessor::changeTrack(int& nextTrackIndex) {
    trackChange.changing = true;
    if (nextTrackIndex < 0 || nextTrackIndex >= tracks.size())return;
    activeTrack->setActive(false);
    activeTrack = tracks[nextTrackIndex];
    activeTrack->setActive(true);

    if (guiAlive) {
        (context->observer->*(context->observer->trackChange)) (nextTrackIndex);
        activeTrack->refresh = true;
    }
    trackChange.nextTrack = -1;
    auto g = getTrackGroup(activeTrack);
    if (nullptr != g) {
        CurrentGroup = g;
    }
    trackChange.changing = false;
}

TrackGroup* OrbishAudioProcessor::getTrackGroup(Track* t) {
    for(auto g : groups){
        if (nullptr == t && g->size() == 0) {
            return g;
        }
        for(auto track : *g){
            if (track == t) {
                return g;
            }
        }
    }
    return nullptr;
}

void OrbishAudioProcessor::removeTrack(int& removeTrackIndex) {
    if (!tracks[removeTrackIndex]->isActive()) {
        tracks.remove(removeTrackIndex);
        context->progress.remove(removeTrackIndex);
        for (int i = removeTrackIndex + 1;i < tracks.size();++i) {
            --tracks[i]->Index;
        }
    }
    trackChange.trackToRemove = -1;
}

bool OrbishAudioProcessor::isEditorActive()
{
    return true;
}

bool OrbishAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* OrbishAudioProcessor::createEditor()
{
    editor = new OrbishAudioProcessorEditor(*this, parameters);
    guiAlive = true;
    return editor;
}

//==============================================================================
void OrbishAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    auto state = parameters.copyState();
    unique_ptr<XmlElement> xml(state.createXml());

    // Persist theme
    xml->setAttribute("themeId", context->themeId.load(std::memory_order_relaxed));

    // Persist MIDI mappings
    auto* midiMappingsXml = xml->createNewChildElement("MidiMappings");
    int count = midiMappings.count.load(std::memory_order_acquire);
    for (int i = 0; i < count; ++i) {
        auto& e = midiMappings.entries[i];
        auto* mapXml = midiMappingsXml->createNewChildElement("Map");
        mapXml->setAttribute("ch", (int)e.channel);
        mapXml->setAttribute("type", (int)e.type);
        mapXml->setAttribute("num", (int)e.number);
        mapXml->setAttribute("action", (int)e.action);
        mapXml->setAttribute("flags", (int)e.flags);
    }

    copyXmlToBinary(*xml, destData);
}

void OrbishAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr) {
        // Restore theme
        if (xmlState->hasAttribute("themeId"))
            context->themeId.store(xmlState->getIntAttribute("themeId", 0), std::memory_order_relaxed);

        // Restore MIDI mappings
        if (auto* midiMappingsXml = xmlState->getChildByName("MidiMappings")) {
            midiMappings.clear();
            for (auto* mapXml : midiMappingsXml->getChildIterator()) {
                MidiMappingEntry entry{};
                entry.channel = (uint8_t)mapXml->getIntAttribute("ch", 0);
                entry.type = (uint8_t)mapXml->getIntAttribute("type", 0);
                entry.number = (uint8_t)mapXml->getIntAttribute("num", 0);
                entry.action = (uint16_t)mapXml->getIntAttribute("action", 0);
                entry.flags = (uint16_t)mapXml->getIntAttribute("flags", 0);
                midiMappings.add(entry);
            }
            xmlState->removeChildElement(midiMappingsXml, true);
        }
        xmlState->removeAttribute("themeId");

        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
    }
}

void OrbishAudioProcessor::printBuffer(AudioBuffer<float> *buffer, String name){
    context->logMessage("Buffer to print: " + name);
    for(auto i=0;i< buffer->getNumSamples();++i){
     auto p= buffer->getReadPointer(0, i);
        context->logMessage(String(i) + ":"+ String(*p));
    }
    context->logMessage("Done printing buffer " + name);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrbishAudioProcessor();
}
