/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <chrono>
using namespace std;

//#include "Track.h"
unique_ptr<RangedAudioParameter> OrbishAudioProcessor::createParamFromBool(AudioParameterBool* boolParam, bool defaultValue) {// return
    auto p = make_unique<AudioProcessorValueTreeState::Parameter>(boolParam->paramID,
                                                                       boolParam->name,
                                                                       boolParam->label,
                                                                       boolParam->getNormalisableRange(),
                                                                       defaultValue,
                                                                       [](bool v) { return v ? TRANS("On") : TRANS("Off"); },
                                                                       nullptr,
                                                                       true,
                                                                       true,
                                                                       true,
                                                                       AudioProcessorParameter::genericParameter,
                                                                       true);
    return move(p);
}

unique_ptr<RangedAudioParameter> OrbishAudioProcessor::createParamFromInt(AudioParameterInt* intParam, int defaultValue) {
    auto p = make_unique<AudioProcessorValueTreeState::Parameter>(intParam->paramID,
                                                                       intParam->name,
                                                                       intParam->label,
                                                                       intParam->getNormalisableRange(),
                                                                       defaultValue,
                                                                       [](float v) { return String(int(v)); },
                                                                       nullptr,
                                                                       true,
                                                                       true,
                                                                       true,
                                                                       AudioProcessorParameter::genericParameter,
                                                                       true);
    return move(p);
}

//==============================================================================
OrbishAudioProcessor::OrbishAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
               .withInput("Input", AudioChannelSet::stereo(), true)
#endif
               .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
               ),
#endif
context(new OrbishContext()),
parameters(*this, nullptr, "OrbishState", {
    make_unique<AudioParameterFloat>("globalMix", "GlobalMix"
                                          ,NormalisableRange<float>(
                                                                    -120.0f
                                                                    , 6.0f
                                                                    , [](float start, float end, float gain) {
                                                                                                                return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start);
                                                                                                             }
                                                                    , [](float start, float end, float dB) {
                                                                                                                float n = Decibels::decibelsToGain(dB, start);
                                                                                                                float d = Decibels::decibelsToGain(end);
                                                                                                                float r = n / d;
                                                                                                                return r;
                                                                                                            }
                                                                    )
                                         , 0.5f
                                         , "db")
    , make_unique<AudioParameterFloat>("clickLevel", "clickLevel"
                                       , NormalisableRange<float>(-120.0f, 6.0f
                                                                  , [](float start, float end, float gain) {
                                                                                                                return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start);
        
                                                                                                            }
                                                                  , [](float start, float end, float dB) {
                                                                                                                return Decibels::decibelsToGain(dB, start) / Decibels::decibelsToGain(end);
                                                                                                        }
                                                                  )
                                       , 0.5f
                                       , "db")
    , make_unique<AudioParameterFloat>("latency", "Latency"
                                            ,NormalisableRange<float>(-500.0f, 500.0f), 0, "ms")
    , make_unique<AudioParameterFloat>("inputLevel", "InputLevel"
                                            , NormalisableRange<float>(-60.0f, 6.0f
                                                                       , [](float start, float end, float gain) {
                                                                                                                    return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start);
                                                                                                                }
                                                                       , [](float start, float end, float dB) {
                                                                                                                    float n = Decibels::decibelsToGain(dB, start);
                                                                                                                    float d = Decibels::decibelsToGain(end);
                                                                                                                    float r = n / d;
                                                                                                                    return r;
                                                                                                                }
                                                                       )
                                       , 0.5f
                                       , "db")
    , make_unique<AudioParameterFloat>("outputLevel", "outputLevel"
                                            , NormalisableRange<float>(-60.0f, 6.0f
                                                                       , [](float start, float end, float gain) {
                                                                                                                    return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start);
                                                                                                                }
                                                                       , [](float start, float end, float dB) {
                                                                                                                    return Decibels::decibelsToGain(dB, start) / Decibels::decibelsToGain(end);
                                                                                                            }
                                                                       )
                                       , 0.5f
                                       , "db")
    , createParamFromBool(new AudioParameterBool("click", "click", false), false)
    , createParamFromBool(new AudioParameterBool("record", "Record", false), false)
    , createParamFromBool(new AudioParameterBool("play", "Play", false), false)
    , createParamFromBool(new AudioParameterBool("stop", "Stop", false), false)
    , createParamFromBool(new AudioParameterBool("mute", "Mute", false), false)
    , createParamFromBool(new AudioParameterBool("solo", "Solo", false), false)
    , make_unique<AudioParameterBool>("monitor", "Monitor", false)
    , make_unique<AudioParameterBool>("reverse", "Reverse", false)
    , make_unique<AudioParameterBool>("undo", "Undo", false)
    , make_unique<AudioParameterBool>("redo", "Redo", false)
    , createParamFromBool(new AudioParameterBool("reset", "Reset", false), false)
    , createParamFromBool(new AudioParameterBool("nextTrack", "NextTrack", false), false)
    , createParamFromBool(new AudioParameterBool("previousTrack", "PreviousTrack", false), false)
    , make_unique<AudioParameterBool>("newTrack", "NewTrack", false)
    , createParamFromBool(new AudioParameterBool("removeTrack", "RemoveTrack", false), false)
    , make_unique<AudioParameterBool>("trigger", "Trigger", false)
    , createParamFromBool(new AudioParameterBool("muteAll", "MuteAll", false), false)
    , createParamFromBool(new AudioParameterBool("stopAll", "StopAll", false), false)
    , createParamFromBool(new AudioParameterBool("startAll", "StartAll", false), false)
    , createParamFromBool(new AudioParameterBool("pauseAll", "PauseAll", false), false)
    , createParamFromBool(new AudioParameterBool("resetAll", "ResetAll", false), false)
    , make_unique<AudioParameterChoice>("snap", "Snap", StringArray("No Sync","Measure","Beat"), 0.5)
    , make_unique<AudioParameterChoice>("mode", "Mode", StringArray("Loop","Repeat","Append","Overwrite","Punch","Clear"), 0)
    , createParamFromBool(new AudioParameterBool("bounce", "Bounce", false), false)
    , createParamFromInt(new AudioParameterInt("trackSelect", "Track", 0, 100, 0), 0)
    , createParamFromBool(new AudioParameterBool("nextLoop", "NextLoop", false), false)
    , createParamFromBool(new AudioParameterBool("previousLoop", "PreviousLoop", false), false)
    , make_unique<AudioParameterBool>("newLoop", "NewLoop", false)
    , createParamFromBool(new AudioParameterBool("removeLoop", "RemoveLoop", false), false)
    , createParamFromInt(new AudioParameterInt("loopSelect", "Loop", 0, 100, 0), 0)
    , createParamFromInt(new AudioParameterInt("fixed", "fixedSizeLength", 1, 16, 0), 0)
    // , createParamFromInt(new AudioParameterInt("selectGroup", "SelectGroup", 0, 10, 0), 0)
    , make_unique<AudioParameterChoice>("selectGroup", "SelectGroup", StringArray("A","B","C","D","E","F","G","H","I","J"), 0)
    , createParamFromBool(new AudioParameterBool("addToGroup", "AddToGroup", false), false)
    , createParamFromBool(new AudioParameterBool("removeFromGroup", "RemoveFromGroup", false), false)
})
{
    context = new OrbishContext();
    context->buffer = make_shared<AudioBuffer<float> >();
    context->inputBuffer = make_shared<AudioBuffer<float> >();

    context->feedback = Decibels::decibelsToGain(float(-0.3));
    context->mix = Decibels::decibelsToGain(parameters.getParameter("globalMix")->getValue());
    context->clickLevel = Decibels::decibelsToGain(parameters.getParameter("clickLevel")->getValue());

    context->xchange = new DataExchange();
    
    initGroups();
    auto formatMgr = make_unique<AudioFormatManager>();
    
    formatMgr->registerBasicFormats();
    unique_ptr<AudioFormatReader> reader(formatMgr->createReaderFor(make_unique<MemoryInputStream>( BinaryData::low_dry_click_aif, BinaryData::low_dry_click_aifSize, false)));
    context->clickBuffer = make_unique<AudioSampleBuffer>(reader->numChannels, (int)reader->lengthInSamples);
    reader->read(context->clickBuffer.get(), 0, int(reader->lengthInSamples), 0, true, true);
    
    unique_ptr<AudioFormatReader> reader2(formatMgr->createReaderFor(make_unique<MemoryInputStream>( BinaryData::high_dry_click_aif,BinaryData::high_dry_click_aifSize, false)));
    context->barStartClickBuffer = make_unique<AudioSampleBuffer>(reader2->numChannels, (int)reader2->lengthInSamples);
    reader2->read(context->barStartClickBuffer.get(), 0, int(reader2->lengthInSamples), 0, true, true);

    primarySynchronizer = make_unique<HostSynchronizer>(context);
    secondarySynchronizer =  make_unique<InternalSynchronizer>(context,nullptr);
    context->allocatorThread = thread(
                                           [this] {
        File file = File(File::getSpecialLocation(File::userHomeDirectory)).getChildFile("Orbish").getChildFile("Orbish.log");
        auto result = file.create();
        if (result.wasOk()) {
            context->logger = make_shared<FileLogger>(file, "Hi");
        }
        while (keepRunning) {
            if (context->audioInputsCount > 0) {
                if(context->xchange->resetBuffers.get()){
                    context->xchange->layerQueue->reset();
                    context->xchange->readBufferQueue->reset();
                    context->xchange->resetBuffers = false;
                }
                if (context->xchange->layerQueue->write_available() > 0) {
                    auto l = make_shared<Layer>(context->audioInputsCount, context->allocatedLength);
                    if (l->Buffer->getNumChannels() == 0
                        || l->Buffer->getNumSamples() < context->sampleRate) {
                        continue;
                    }
                    context->xchange->layerQueue->push(l);
                }
                if (context->xchange->writeVisualisationBufferQueue->write_available() > 0) {
                    auto b = make_shared<BufferForVisualisation>();
                    b->buffer = nullptr;
                    context->xchange->writeVisualisationBufferQueue->push(b);
                }
                else {
                    this_thread::sleep_for(chrono::milliseconds(1));
                }
            }
            for (auto track : tracks) {
                if (track != nullptr && *track->LoopDuration > 0) {
                    if (track->doBounce) {
                        track->BounceAllHistory();
                        track->doBounce = false;
                    }
                    for (auto layer:*track->Layers){
                        if (layer->dirty)break;
                        auto length = 0;
                        if (track->loopToBeExtended && layer->Buffer->getNumSamples() < context->allocatedLength) {
                            length = context->allocatedLength;
                        }
                        else if (layer->Buffer->getNumSamples() > ((*track->LoopDuration) + context->samplesPerBlock)
                                 && !(track->getRecordMode() > 0 && track->getRecordMode() < 5)) {
                            length = (*track->LoopDuration) + context->samplesPerBlock;
                        }
                        if (length > 0) {
                            layer->Buffer->setSize(context->audioOutputsCount, length, true, true, false);
                        }
                    }
                    track->loopToBeExtended = false;
                }
            }
            if(context->loggingActive){
                int64 stamp = Time::getApproximateMillisecondCounter();
                if (stamp - context->timestamp > 2000) {
                    context->timestamp = stamp;
                    context->flushLogs();
                }
            }
            while (context->xchange->readBufferQueue->write_available() > 0){
                auto rb = make_shared<AudioBuffer<float> >();
                //  String s = String(pointer_sized_int(b));
                rb->clear();
                rb->setSize(context->audioInputsCount, context->samplesPerBlock);
                if (rb->getNumChannels() == 0
                    || rb->getReadPointer(0) == NULL
                    || rb->getNumSamples() == 0) {
                    continue;
                }
                context->xchange->readBufferQueue->push(rb);
            };
            while (context->xchange->writeGainModifierQueue->write_available() > 0){
                auto gm = make_shared<GainModifier>();
                context->xchange->writeGainModifierQueue->push(gm);
            };
            while (context->xchange->readGainModifierQueue->read_available() > 0){
                auto gm = context->xchange->readGainModifierQueue->front();
                context->xchange->readGainModifierQueue->pop();
                gm->applyGain();
                gm->buffer = nullptr;
            };
            while (context->xchange->writeMeasureBufferQueue->write_available() > 0){
                auto mb = make_shared<MeasureBuffer>();
                mb->buffer = nullptr;
                context->xchange->writeMeasureBufferQueue->push(mb);
            };
            while (context->xchange->readMeasureBufferQueue->read_available() > 0){
                auto mb = context->xchange->readMeasureBufferQueue->front();
                context->xchange->readMeasureBufferQueue->pop();
                mb->measure();
            };
        }
    }
                                           );
    //   logMessage("end constructor");
}

OrbishAudioProcessor::~OrbishAudioProcessor()
{
    keepRunning = false;
    if (context->allocatorThread.joinable()) {
        context->allocatorThread.join();
    }
    context->lockForStateUpdate(true);
    cleanup();
    context->lockForStateUpdate(false);
}

void OrbishAudioProcessor::parameterChanged(const String& parameterID, float newValue) {
    if (parameterID == "globalMix") {
        auto p = parameters.getParameter("globalMix");
        context->mix =  p->convertTo0to1(newValue);
    }
    if (parameterID == "clickLevel") {
        auto p = parameters.getParameter("clickLevel");
        context->clickLevel =  p->convertTo0to1(newValue);
    }
    if (parameterID == "latency") {
        auto delay = newValue * context->sampleRate * 0.001f;
        context->delayCompensation = int(delay);
    }
    if (parameterID == "muteAll") {
        processMuteAllChange(bool(newValue));
    }
    if (parameterID == "trackSelect") {
        if (newValue >= 0) {
            processTrackChange(uint(newValue));
        }
    }
    if (parameterID == "nextTrack") {
        askTrackChange(activeTrack->Index + 1);
    }
    if (parameterID == "previousTrack") {
        askTrackChange(activeTrack->Index - 1);
    }
    if (parameterID == "newTrack") {
        processNewTrack();
    }
    if (parameterID == "click") {
        processClickToggle();
    }
    if (parameterID == "removeTrack") {
        processRemoveTrack(tracks.size() - 1);
    }
    if (parameterID == "stopAll") {
        processStopAllChange();
    }
    if (parameterID == "startAll") {
        processStartAllChange();
    }
    if (parameterID == "pauseAll") {
        processPauseAllChange(bool(newValue));
    }
    if (parameterID == "resetAll") {
        processResetAllChange();
    }
    if (parameterID == "selectGroup") {
        processGroupSelect(int(newValue));
    }
    if (parameterID == "addToGroup") {
        processAddToGroup(activeTrack->Index);
    }
    if (parameterID == "removeFromGroup") {
        processRemoveFromGroup(activeTrack->Index);
    }
}



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
    tracks.add(track);
    context->trackCount = tracks.size();
    context->progress.add(tracks[tracks.size() - 1]->Progress);
    
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
    //   logMessage("begin prepareToPlay");
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if (context == nullptr) {
        context = new OrbishContext();
    }
    if (getTotalNumOutputChannels() != context->audioOutputsCount) {
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
    //  logMessage("end prepareToPlay");
    context->logMessage("sample rate set to:" + String(context->sampleRate));
    
    
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
    int holdValue = 150;
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
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void OrbishAudioProcessor::startReset()
{
    // start playback of all armed tracks
    for (auto track : tracks) {
        if (*track->LoopDuration > 0) {
            track->StartResetBefore();
        }
    }
}

void OrbishAudioProcessor::startPlayback()
{
    // start playback of all armed tracks
    for (auto track : tracks) {
        if (!track->Playing) {
            track->StartPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::stopPlayback()
{
    // stop playback of all tracks currently playing
    for (auto track : tracks) {
        if (track->Playing)
            track->StopPlaybackBefore();
    }
    if (activeTrack->Recording) {
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::resumePlayback()
{
    // resume playback of all armed tracks
    for (auto track : tracks) {
        track->StartPlaybackBefore();
    }
}

void OrbishAudioProcessor::pausePlayback()
{
    // pause playback of all tracks currently playing
    for (auto track : tracks) {
        if (track->Playing)
            track->PausePlaybackBefore();
    }
}

void OrbishAudioProcessor::doMute() {
    for (auto track : tracks) {
        if (track->isMuteArmed()) {
            track->StartMuteBefore();
        }
        else {
            track->StopMuteBefore();
        }
    }
}

void OrbishAudioProcessor::processNewTrack() {
    addTrack(false);
    if (guiAlive) {
            (context->observer->*(context->observer->newTrack))();
    }
}

void OrbishAudioProcessor::processClickToggle() {
    context->clickEnabled = !context->clickEnabled;
}

void OrbishAudioProcessor::processRemoveTrack(int trackNumber) {
    if (trackNumber > 0 && trackNumber < tracks.size()) {
        if (tracks[trackNumber]->Recording) {
            tracks[trackNumber]->StopRecordingBefore();
        }
        if (tracks[trackNumber]->Playing) {
            tracks[trackNumber]->StopPlaybackBefore();
        }
        trackToRemove = trackNumber;
    }
}

void OrbishAudioProcessor::processStopAllChange() {
    globalAction = kGlobalStop;
    // stop playback of all armed tracks
    for (auto track : tracks) {
        track->setPlayArmed(false);
        if (track->getSnapMode() == kSnapNone)
        {
            track->StopPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::processResetAllChange() {
    startReset();
    if (activeTrack->Recording) {
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::processStartAllChange() {
    globalAction = kGlobalPlay;
    // start playback of all armed tracks
    for (auto track : tracks) {
        track->setPlayArmed(true);
        if (track->getSnapMode() == kSnapNone)
        {
            track->StartPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::processPauseAllChange(bool pause) {
    if (pause) {
        globalAction = kGlobalPause;
    }
    else {
        globalAction = kGlobalResume;
    }
    for (auto track : tracks) {
        if (!pause) {
            if (track->WasPlaying) {
                track->setPlayArmed(true);
                if (track->getSnapMode() == kSnapNone)
                {
                    track->StartPlaybackBefore();
                }
            }
        }
        else {
            if (track->Playing) {
                track->setPlayArmed(false);
                if (track->getSnapMode() == kSnapNone)
                {
                    track->PausePlaybackBefore();
                }
            }
        }
    }
}

void OrbishAudioProcessor::processMuteAllChange(bool mute) {
    globalAction = kGlobalMute;
    for (auto track : tracks) {
        track->setMuteArmed(mute);
        if (track->isActive()) {
            track->setRecordingArmed(track->isRecordingArmed() && !track->isMuteArmed());
            if (track->getSnapMode() == kSnapNone)
            {
                track->Muted = track->isMuteArmed();
                if (track->Recording && track->Muted)
                    track->StopRecordingBefore();
            }
        }
    }
}

void OrbishAudioProcessor::processTrackChange(uint newTrackIndex) {
    if (activeTrack->Index != newTrackIndex && newTrackIndex < uint(tracks.size())) {
        // disable recording on track we're about to leave
        if (activeTrack->Recording) {
            activeTrack->StopRecordingBefore();
        }
        nextTrack = newTrackIndex;
    }
}


void OrbishAudioProcessor::processGroupSelect(int groupIdx) {
    SelectedGroup = groups[groupIdx];
}

void OrbishAudioProcessor::processTempoChange(int bpm) {
    int ratio = context->info->bpm / bpm;
  //  stretcher->setTimeRatio(ratio);
}

void OrbishAudioProcessor::initGroups() {
    while (groups.size() > 0) {
        groups.pop_back();
    }
    for (auto i = 0; i < 10; ++i) {
        groups.push_back(new TrackGroup(i));
    }
    SelectedGroup = CurrentGroup = groups.front();
}


void OrbishAudioProcessor::processAddToGroup(int track) {
    CurrentGroup->RemoveTrack(tracks[track]);
    SelectedGroup->AddTrack(tracks[track]);
    CurrentGroup = SelectedGroup;
}

void OrbishAudioProcessor::processRemoveFromGroup(int track) {
    CurrentGroup->RemoveTrack(tracks[track]);
}


void OrbishAudioProcessor::processMidi(const MidiBuffer& midi) {
    OrbishContext* ctxt = context;
    if (guiAlive) {
            (ctxt->observer->*(context->observer->handleMidi))(midi);
    }
}

void OrbishAudioProcessor::changeTrack(int& nextTrackIndex) {
    changingTrack = true;
    if (nextTrackIndex < 0 || nextTrackIndex >= tracks.size())return;
    activeTrack->setActive(false);
    activeTrack = tracks[nextTrackIndex];
    activeTrack->setActive(true);
    
    if (guiAlive) {
        (context->observer->*(context->observer->trackChange)) (nextTrackIndex);
        activeTrack->refresh = true;
    }
    nextTrack = -1;
    auto g = getTrackGroup(activeTrack);
    if (nullptr != g) {
        CurrentGroup = g;
    }
    changingTrack = false;
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
        --context->trackCount;
        if (guiAlive) {
            (context->observer->*(context->observer->trackRemoval)) (removeTrackIndex);
        }
    }
    trackToRemove = -1;
}

void OrbishAudioProcessor::initBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages){
    if (context->init)context->init = false;
    if (midiMessages.data.size() > 0) {
        processMidi(midiMessages);
    }
    ScopedNoDenormals noDenormals;
    AudioPlayHead* head = getPlayHead();
    AudioPlayHead::CurrentPositionInfo info{};
    head->getCurrentPosition(info);
    context->timeSigBottom = info.timeSigDenominator;
    context->timeSigTop = info.timeSigNumerator;
    context->samplesPerBeat = int(context->beatsToSamples(1.0));
    context->samplesPerQuarter = int(context->quartersToSamples(1.0));

    if (context->bpm != info.bpm
        || context->timeSigTop != info.timeSigDenominator) {
        context->bpm = info.bpm;
        context->samplesPerBeat = int(std::round(context->beatsToSamples(1.0)));
        context->samplesPerQuarter = int(context->quartersToSamples(1.0));
        //logMessage(String("bpm: ") + String(context->bpm));
    }
    
    context->maxBlockSize = buffer.getNumSamples();
    context->info->bpm = info.bpm;
    context->info->editOriginTime = info.editOriginTime;
    context->info->isPlaying = info.isPlaying;
    context->info->ppqPosition = info.ppqPosition;
    context->info->timeInSamples = int64(context->quartersToSamples(float(info.ppqPosition)));
    context->info->timeSigDenominator = info.timeSigDenominator;
    context->info->timeSigNumerator = info.timeSigNumerator;
    context->info->ppqPositionOfLastBarStart = info.ppqPositionOfLastBarStart;
    int diff = context->maxBlockSize - context->samplesPerBlock;
    if(diff != 0){
        context->logMessage("buffer excess:" + String(diff));
    }
    if (context->info->isPlaying) {
        trackHostSamples += context->maxBlockSize;
    }
}

void OrbishAudioProcessor::realign(){
    int df = int(context->quartersToSamples(context->info->ppqPosition)) % context->samplesPerQuarter;
    if (context->info->isPlaying) {
        if (!hostHasPlayed)hostHasPlayed = true;
        for(auto track:tracks){
            if (track->IsPlaying() && !track->Reverse && track->getSnapMode() != kSnapNone){
                auto currentPosition = *track->CurrentPlayingIndex;
                double currentPosInQuarters =0;
                double currentHostPosInQuarters=0;
                double diffQuarters = 0;
                switch (track->getSnapMode()) {
                    case kSnapQuarter:
                         currentPosInQuarters = fmod(context->samplesToQuarters(currentPosition), 4 / context->timeSigBottom);
                         currentHostPosInQuarters = fmod(context->info->ppqPosition, 4 / context->timeSigBottom);
                         diffQuarters = currentHostPosInQuarters - currentPosInQuarters;
                         if (abs(diffQuarters) > (.5 * 4 / context->timeSigBottom)) {
                            if(currentPosInQuarters < currentHostPosInQuarters){
                                diffQuarters = currentHostPosInQuarters - (4 / context->timeSigBottom) - currentPosInQuarters ;
                            }else{
                                diffQuarters = (4 / context->timeSigBottom) - currentPosInQuarters + currentHostPosInQuarters;
                            }
                        }
                        break;
                    case kSnapMeasure:
                        currentPosInQuarters = fmod(context->samplesToQuarters(currentPosition), context->quartersPerBar());
                        currentHostPosInQuarters = fmod(context->info->ppqPosition, context->quartersPerBar());
                        diffQuarters = currentHostPosInQuarters - currentPosInQuarters;
                        if (abs(diffQuarters) > (context->quartersPerBar()*.5)) {
                            if(currentPosInQuarters < currentHostPosInQuarters){
                                diffQuarters = currentHostPosInQuarters - context->quartersPerBar() - currentPosInQuarters ;
                            }else{
                                diffQuarters = context->quartersPerBar() - currentPosInQuarters + currentHostPosInQuarters;
                            }
                        }
                        break;
                    default:
                        break;
                }
                int diff= context->quartersToSamples(diffQuarters);
                if (abs(diff) >= context->sampleRate / 400) {
                    if (!((diff < 0 && *track->CurrentPlayingIndex < abs(diff))
                          || (diff > 0 && *track->CurrentPlayingIndex > * track->LoopDuration - diff))) {
                            if (!track->realignment->isSyncInProgress()){
                                track->realignment->setTotalOffset(diff);
                            }
                    }
                }
                if (diff > 80000){
                    context->logMessage("oops");
                }

//            context->logMessage("#======================================");
//            context->logMessage(String("#current pos in quarters:") + String(currentPosInQuarters));
//            context->logMessage(String("#realign with host at:") + String(context->info->ppqPosition) + String("\tdiff:")+String(diff));
//            context->logMessage(String("#current host pos in quarters:") + String(currentHostPosInQuarters));
//
//            context->logMessage(String("#diff in quarters:") + String(diffQuarters));
//            context->logMessage(String("#hostSamples:") + String(context->info->timeInSamples));
//            context->logMessage(String("#host position converted:") + String(int(context->quartersToSamples(context->info->ppqPosition))));
//            context->logMessage(String("#host Samples tracked by p/i:") + String(trackHostSamples));
//            context->logMessage(String("#host samples gap:") + String(trackHostSamples - context->info->timeInSamples));
//            context->logMessage(String("#gap in quarters:") + String(context->samplesToQuarters(trackHostSamples - context->info->timeInSamples)));
//
//            context->logMessage(String("#plugin pos after host sync:") + String(*track->CurrentPlayingIndex));
//            context->logMessage(String("#samples per beat:") + String(context->samplesPerBeat));
//            context->logMessage(String("#samples per quarter:") + String(context->samplesPerQuarter));
//            context->logMessage(String("#Loop duration:") + String(*track->LoopDuration));
//            context->logMessage(String("#Loop duration in quarters:") + String(context->samplesToQuarters(*track->LoopDuration)));
//            context->logMessage(String("#block in quarters:") + String(context->samplesToQuarters(context->maxBlockSize)));
//            context->logMessage(String("#sample rate:") + String(context->sampleRate));
//            context->logMessage(String("#samples per minute:") + String(context->samplesPerMinute));
//            context->logMessage(String("#bpm:") + String(context->bpm));
//            context->logMessage(String("#denominator:") + String(context->timeSigBottom));
           }
        }
    }
}

void OrbishAudioProcessor::handleClick(OrbishContext* context, AudioSampleBuffer* output){
    int targetOffset = 0, sourceOffset = 0;
    
    auto offsetFromClosestBeat = context->differenceFromClosestBeatInSamples(int(context->info->timeInSamples));
    if (!context->clickEnabled
        || !context->info->isPlaying
        || (abs(offsetFromClosestBeat) > context->maxBlockSize
            && !clickInProcess)) {
        return;
    }
    context->logMessage("$offsetFormClosestBeat:" + String(context->samplesToBeats(offsetFromClosestBeat)));
    context->logMessage("$offsetFormClosestBeat2:" + String(context->samplesToQuarters(offsetFromClosestBeat)));

    auto hostPosInBeats = context->info->ppqPosition * (context->timeSigBottom * .25);
    context->logMessage("$host pos in beats:" + String(hostPosInBeats));
    context->logMessage("$host pos in quarters:" + String(context->info->ppqPosition));



    AudioSampleBuffer* source;
    if (fmod(std::round(hostPosInBeats) , context->timeSigTop) == 0) {
        source = context->barStartClickBuffer.get();
    }else{
        source = context->clickBuffer.get();
    }
    auto b = context->maxBlockSize;
    auto a = source->getNumSamples();
    auto x = a % b;
    auto y = a / b;
    auto samplesToWrite = b;
    // first
    if (!clickInProcess) {
        targetOffset = (offsetFromClosestBeat < 0)
            ? abs(offsetFromClosestBeat)
            : 0;
        samplesToWrite -= targetOffset;
        clickInProcess = true;
        // last
    }else if(numBuffersDoneForClick>=y){
        if (x <= 0 || numBuffersDoneForClick > y) {
            clickInProcess = false;
            context->logMessage("$host pos in quarters:" + String(context->info->ppqPosition));
            numBuffersDoneForClick = 0;
            return;
        }
        samplesToWrite = x;
    }
    sourceOffset = numBuffersDoneForClick * b;
    numBuffersDoneForClick++;

    if(samplesToWrite > 0){
        for (auto c=0; c < source->getNumChannels(); ++c) {
            output->addFrom(c, targetOffset, source->getReadPointer(c, sourceOffset), samplesToWrite, context->clickLevel);
        }
    }
}

void OrbishAudioProcessor::captureTrigger(int& startRecordingSample){
    // Auto Trigger mode: check if there is any sound before starting new activeTrack->Recording
    if (!activeTrack->Recording
        && activeTrack->isRecordingArmed()
        && activeTrack->isAutoTrigger()
        && !activeTrack->Triggered)
    {
        
        const float** samples = context->buffer->getArrayOfReadPointers();
        for (uint32 channel = 0; channel < context->audioInputsCount && startRecordingSample == -1; channel++)
            
            for (int i = 0; i < context->maxBlockSize && startRecordingSample == -1; i++)
            {
                if (abs(samples[channel][i]) > context->triggerThreshold)
                {
                    startRecordingSample = i;
                    activeTrack->Triggered = true;
                }
            }
    }
}


void OrbishAudioProcessor::handleReverseEvent(int startReverseSample, int stopReverseSample){
    // manage activeTrack->Reverse state
    bool doStartReversal = (startReverseSample >= 0) && (context->maxBlockSize > startReverseSample);
    bool doStopReversal = (stopReverseSample >= 0) && (context->maxBlockSize > stopReverseSample);
    
    if (doStartReversal) {
        if (nullptr != getTrackGroup(activeTrack)) {
            for(auto groupedTrack:*CurrentGroup){
                groupedTrack->StartReverse();
            }
        }
        else {
            activeTrack->StartReverse();
        }
    }
    else if (doStopReversal) {
        if (nullptr != getTrackGroup(activeTrack)) {
            for(auto groupedTrack:*CurrentGroup){
                groupedTrack->StopReverse();
            }
        }
        else {
            activeTrack->StopReverse();
        }
        
    }
}

void OrbishAudioProcessor::handleRecordingEvent(int startRecordingSample, int stopRecordingSample){
//    int pluginDiff=-1;
//    if(*activeTrack->LoopDuration > 0 && activeTrack->Playing){
//        pluginDiff = *activeTrack->LoopDuration - *activeTrack->CurrentPlayingIndex;
//    }
//    if(startRecordingSample >= 0 && pluginDiff >= 0){
//        startRecordingSample = max(pluginDiff,0);
//    }
//    if(stopRecordingSample >= 0 && pluginDiff >= 0){
//        stopRecordingSample = max(stopRecordingSample + pluginDiff,0);
//    }
    //context->logMessage("startRecordingSample: " + String(startRecordingSample));
    bool doStartRecording = (startRecordingSample >= 0) && (context->maxBlockSize > startRecordingSample);
    bool doStopRecording = (stopRecordingSample >= 0) && (context->maxBlockSize > stopRecordingSample);
    if (doStartRecording) {
        activeTrack->StartRecordingBefore();
    }
    else if (doStopRecording) {
        // switch recording off but still allow for the last samples to be written + fade
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::handlePlaybackEvent(int startPlayingSample, int stopPlayingSample){
    bool doStartPlaying = (startPlayingSample >= 0) && (context->maxBlockSize > startPlayingSample);
    bool doStopPlaying = (stopPlayingSample >= 0) && (context->maxBlockSize > stopPlayingSample);
    if (doStartPlaying)
    {
        if (globalAction == kGlobalPlay) {
            startPlayback();
        }
        else if (globalAction == kGlobalResume) {
            resumePlayback();
        }
        else {
            if (nullptr != getTrackGroup(activeTrack)) {
                for(auto groupedTrack:*CurrentGroup){
                    groupedTrack->StartPlaybackBefore();
                }
            }
            else {
                activeTrack->StartPlaybackBefore();
            }
        }
        globalAction = kGlobalNone;
    }
    else if (doStopPlaying)
    {
        if (globalAction == kGlobalStop) {
            stopPlayback();
        }
        else if (globalAction == kGlobalPause) {
            pausePlayback();
        }
        else {
            if (activeTrack->isStopArmed())
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StopPlaybackBefore();
                    }
                }
                else {
                    activeTrack->StopPlaybackBefore();
                }
                else
                    if (nullptr != getTrackGroup(activeTrack)) {
                        for(auto groupedTrack:*CurrentGroup){
                            groupedTrack->PausePlaybackBefore();
                        }
                    }
                    else {
                        activeTrack->PausePlaybackBefore();
                    }
        }
        globalAction = kGlobalNone;
    }
}


void OrbishAudioProcessor::handleMuteEvent(int toggleMuteSample){
    bool toggleMute = (toggleMuteSample >= 0) && (context->maxBlockSize > toggleMuteSample);
    if (toggleMute) {
        if (globalAction == kGlobalMute) {
            doMute();
        }
        else {
            if (activeTrack->isMuteArmed()) {
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StartMuteBefore();
                    }
                }
                else {
                    activeTrack->StartMuteBefore();
                }
            }
            else {
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StopMuteBefore();
                    }
                }
                else {
                    activeTrack->StopMuteBefore();
                }
            }
        }
        globalAction = kGlobalNone;
    }
}

bool OrbishAudioProcessor::handleSoloEvent(int toggleSoloSample){
    bool toggleSolo = (toggleSoloSample >= 0) && (context->maxBlockSize > toggleSoloSample);
    if (toggleSolo) {
        if (activeTrack->isSoloArmed()) {
            aTrackIsSoloed = true;
            if (nullptr != getTrackGroup(activeTrack)) {
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if(!CurrentGroup->Contains(*track)){
                        if((*track)->Soloed){
                            (*track)->StopSoloBefore();
                            //  logger->logMessage("unsolo track " + String((*track)->Index));
                        }
                        (*track)->FirstMuteBuffer = true;
                    }
                }
                CurrentGroup->ForEach([](Track* t){ t->StartSoloBefore();} );
            }
            else {
                activeTrack->StartSoloBefore();
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if((*track)->Soloed){
                        (*track)->StopSoloBefore();
                    }
                    (*track)->FirstMuteBuffer = true;
                }
            }
        }
        else {
            aTrackIsSoloed = false;
            if (nullptr != getTrackGroup(activeTrack)) {
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if(!CurrentGroup->Contains(*track)){
                        (*track)->LastMuteBuffer = true;
                    }
                }
                CurrentGroup->ForEach([](Track* t){ t->StopSoloBefore();} );
            }
            else {
                activeTrack->StopSoloBefore();
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    (*track)->LastMuteBuffer = true;
                }
            }
        }
    }
    return toggleSolo;
}

void OrbishAudioProcessor::handleLoopChangeEvent(int startLoopChangeSample){
    
    bool doStartLoopChange = (startLoopChangeSample >= 0) && (context->maxBlockSize > startLoopChangeSample);
    if (doStartLoopChange) {
        if (nullptr != getTrackGroup(activeTrack)) {
            if ((activeTrack->nextLoop - activeTrack->ActiveLoop->Index) == 1) {
                for(auto groupedTrack:*CurrentGroup){
                    groupedTrack->ChangeLoopBefore(groupedTrack->ActiveLoop->Index + 1);
                }
            }
            else
                if ((activeTrack->nextLoop - activeTrack->ActiveLoop->Index) == -1) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->ChangeLoopBefore(groupedTrack->ActiveLoop->Index - 1);
                    }
                }
                else {
                    activeTrack->ChangeLoopBefore(activeTrack->nextLoop);
                    
                }
        }
        else {
            activeTrack->ChangeLoopBefore(activeTrack->nextLoop);
        }
    }
}

void OrbishAudioProcessor::handleEvents(Events& e){
    captureTrigger(e.startRecordingSample);
    
    // actions will be performed in sync based on the snap mode
    // look for the sample corresponding to the snap (bar or beat) and perform action
    // if no sync the actions are performed immediately
    
    //print2("timeSigBottom:", data.transport->timeSigBottom);
    // start position is either buffer start, or startRecordingSample if we are detecting automatically
    int nextSample=-1;
    nextSample = activeTrack->getNextSample(activeTrack->getSnapMode());
    if(nextSample < 0){
        nextSample = primarySynchronizer->getNextSample(activeTrack->getSnapMode());
    }
    //context->logMessage("currentPos: " + String(currentPos));
    //context->logMessage("expectedSamplePositi on: " + String(int64(expectedSamplePosition)));
    //context->logMessage("expectedPos: " + String(expectedPos));
    
    //context->logMessage("nextSample" + String(nextSample));
    //context->logMessage("timeInSamples:" + String(context->info->timeInSamples));
    //context->logMessage("timeInSecs: " + String(context->info->timeInSeconds));
    //context->logMessage("currentRecordingIndex:" + String(activeTrack->CurrentRecordingIndex));
    //context->logMessage("currentPlayingIndex:" + String(*activeTrack->CurrentPlayingIndex));
    
    //context->logMessage("active track loop duration:" + String(*activeTrack->LoopDuration));
    
    // manage activeTrack->Recording
    if (!activeTrack->Recording && activeTrack->isRecordingArmed())
    {
        if((activeTrack->getSnapMode() == kSnapNone
            && activeTrack->isAutoTrigger())){
            if (!activeTrack->Triggered) {
                e.startRecordingSample = -1;
            }
        }else{
            e.startRecordingSample = nextSample;
        }
    }
    else if (activeTrack->Recording && !activeTrack->isRecordingArmed())
    {
        e.stopRecordingSample = nextSample;
    }
    // manage activeTrack->Playing
    if ((!activeTrack->Playing && activeTrack->isPlayArmed()) || globalAction == kGlobalPlay || globalAction == kGlobalResume)
    {
        e.startPlayingSample = nextSample;
    }
    else if ((activeTrack->Playing && !activeTrack->isPlayArmed()) || globalAction == kGlobalStop || globalAction == kGlobalPause)
    {
        e.stopPlayingSample = nextSample;
    }
    
    // manage reversing
    if (!activeTrack->Reverse && activeTrack->isReverseArmed())
    {
        e.startReverseSample = nextSample;
    }
    
    else if (activeTrack->Playing && activeTrack->Reverse && !activeTrack->isReverseArmed()) {
        e.stopReverseSample = nextSample;
    }
    if (activeTrack->loopChangeArmed)
    {
        e.startLoopChangeSample = nextSample;
    }
    // manage Mute
    if ((activeTrack->isMuteArmed() != activeTrack->Muted) || globalAction == kGlobalMute) {
        e.toggleMuteSample = nextSample;
    }
    // manage Solo
    if (activeTrack->isSoloArmed() != activeTrack->Soloed) {
        e.toggleSoloSample = nextSample;
    }
    handlePlaybackEvent(e.startPlayingSample, e.stopPlayingSample);
    handleRecordingEvent(e.startRecordingSample, e.stopRecordingSample);
    // manage activeTrack->Reverse state
    handleReverseEvent(e.startReverseSample, e.stopReverseSample);
    // manage mute state
    handleMuteEvent(e.toggleMuteSample);
    
    handleLoopChangeEvent(e.startLoopChangeSample);
    // manage solo state
    e.toggleSolo = handleSoloEvent(e.toggleSoloSample);
}

void OrbishAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    int64 beginMark = Time::getHighResolutionTicks();
    int64 startBeginning = Time::getHighResolutionTicks();
    //   logMessage("begin processBlock");
    initBlock(buffer, midiMessages);
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = context->audioInputsCount; i < context->audioOutputsCount; ++i)
        buffer.clear(i, 0, context->maxBlockSize);
    
    // logger->logMessage("context buffer size:" + String(context->buffer->));
    // get a copy of the incoming audio to manipulate
    context->buffer->clear();
    context->inputBuffer->clear();
    for (uint c = 0; c < context->audioInputsCount; ++c) {
        smoothVolume(activeTrack->PreviousInputLevel, activeTrack->getInputLevel(), min(context->maxBlockSize, buffer.getNumSamples()), &buffer, context->buffer.get(), c);
        if(context->postMixMonitoring){
            context->inputBuffer->copyFrom(c, 0, context->buffer->getReadPointer(c, 0),  context->maxBlockSize);
        }else{
            context->inputBuffer->copyFrom(c, 0, buffer.getReadPointer(c, 0),  context->maxBlockSize);
        }
        //        const float* pt = buffer.getReadPointer(c);
        //        const float* pt2 = context->buffer->getReadPointer(c);
        
        //      context->logMessage("b\tcb:");
        //        for (auto i=0; i<context->buffer->getNumSamples(); ++i) {
        //            if(pt[i] != pt2[i]){
        //                String s = "Gap at "+String(i)+": "+String(pt[i])+"\t"+String(pt2[i]);
        //            }
        //        }
        //        if (buffer.getNumSamples() != context->buffer->getNumSamples()) {
        //            context->logMessage("buffer size difference:" + String(buffer.getNumSamples()-context->buffer->getNumSamples()));
        //        }
        
    }
    int64 startFinal = Time::getHighResolutionTicks();
    int64 startMeasure = Time::getHighResolutionTicks();
    
    if(context->xchange->writeMeasureBufferQueue->read_available() > 0 &&
       context->xchange->readMeasureBufferQueue->write_available() > 0){
        auto mb = context->xchange->writeMeasureBufferQueue->front();
        mb->buffer = context->inputBuffer;
        mb->source = &inputMeterSource;
        context->xchange->writeMeasureBufferQueue->pop();
        context->xchange->readMeasureBufferQueue->push(mb);
    }else{
        inputMeterSource.measureBlock(*context->inputBuffer);
    }
    int64 endMeasure = Time::getHighResolutionTicks();
    if(activeTrack->refresh){
        activeTrack->RefreshLoopVisualizer();
    }
    // no action if host doesn't play
    if (!context->info->isPlaying) {
        //        stopPlayback();
        if (activeTrack->Recording) {
            activeTrack->Recording = false;
            activeTrack->setRecordingArmed(false);
            activeTrack->StopRecordingAfter();
        }
        for(auto track:tracks){
            if (track->Playing) {
                track->Playing = false;
                track->setPlayArmed(false);
                track->StopPlaybackAfter();
            }
        }
        if (activeTrack->loopChangeArmed) {
            activeTrack->ChangeLoopBefore(activeTrack->nextLoop);
        }
    }
    else {
        // realign every 4 bars (from host perspective)
        if(context->skipAlign){
            context->skipAlign = false;
        }else{
                realign();
        }
        Events e;
        if (!changingTrack) {
            handleEvents(e);
        }
        int64 endBeginning = Time::getHighResolutionTicks();
        //context->logMessage("Beginning section:" + String(endBeginning - startBeginning));
        if (activeTrack->Recording || (activeTrack->LastRecordingBuffer)) {
            int64 startRec = Time::getHighResolutionTicks();
            handleRecordBlock(e.startRecordingSample, e.stopRecordingSample);
            int64 endRec = Time::getHighResolutionTicks();
            //context->logMessage("nano secs spent recording:" + String(endRec - startRec));
        }
        if (!activeTrack->isMonitoring()) {
            // prevent input from ending up in the output buffer
            context->inputBuffer->clear();
        }
        context->buffer->clear();
        if (context->info->isPlaying) {
            int64 startPlay = Time::getHighResolutionTicks();
            handlePlaybackBlock((e.startReverseSample >= 0) ? e.startReverseSample : e.startPlayingSample, (e.stopReverseSample >= 0) ? e.stopReverseSample : e.stopPlayingSample);
            int64 endPlay = Time::getHighResolutionTicks();
            //context->logMessage("nano secs spent playing back:" + String(endPlay - startPlay));
        }
        
        if(e.toggleSolo){
            for(auto track: tracks){
                track->FirstMuteBuffer  = false;
                track->LastMuteBuffer  = false;
            }
        }
    }

    if (!activeTrack->isMonitoring() && !context->info->isPlaying) {
        // prevent input from ending up in the output buffer
        context->buffer->clear();
    }
    // overwrite the output buffer with the processed audio
    for (uint c = 0; c < context->audioInputsCount; ++c) {
        buffer.clear(c, 0, context->maxBlockSize);
        context->buffer->addFrom(c, 0, context->inputBuffer->getReadPointer(c, 0),  context->maxBlockSize);
    //    auto rp= context->buffer->getReadPointer(c);
     //   const float* const *input[1];
     //   input[0]=&rp;
        //stretcher->process((const float* const *)&input, context->buffer->getNumSamples(), false);
      //  auto samps = stretcher->available();
      //  auto wp = context->buffer.get()->getWritePointer(c);
      //  const float* const *output[1];
      //  output[0]=&wp;
      //  stretcher->retrieve((float* const *)&output, samps);
         smoothVolume(previousMixLevel, context->mix, context->maxBlockSize, context->buffer.get(), &buffer, c);
    }
    int64 endFinal = Time::getHighResolutionTicks();
    
    // run the methods defined to be executed after the block
    for(auto t :tracks){
        //context->logMessage(String(t->RunAfters.size()));
        while (t->RunAfters.size() > 0) {
            (t->*(t->RunAfters).back()) ();
            t->RunAfters.pop_back();
        }
    }
    if (nextTrack > -1) {
        changeTrack(nextTrack);
    }
    if (trackToRemove > 0) {
        removeTrack(trackToRemove);
    }

    if(context->xchange->writeMeasureBufferQueue->read_available() > 0 &&
       context->xchange->readMeasureBufferQueue->write_available() > 0){
        auto mb = context->xchange->writeMeasureBufferQueue->front();
        mb->buffer = std::make_shared<AudioBuffer<float>>(buffer);
        mb->source = &outputMeterSource;
        context->xchange->writeMeasureBufferQueue->pop();
        context->xchange->readMeasureBufferQueue->push(mb);
    }
    else{
        outputMeterSource.measureBlock(buffer);
    }
    handleClick(context, &buffer);

    //context->logMessage("Final section:" + String(endFinal - startFinal));
    //context->logMessage("Measure section:" + String(endMeasure - startMeasure));
    
    //  logMessage("begin iock");
    int64 diff, endMark = Time::getHighResolutionTicks();
    diff = endMark - beginMark;
    context->logMessage("nano secs spent in callback:" + String(diff = endMark - beginMark));
    context->logMessage("buffer size:" + String(context->maxBlockSize));
    int64 expectedDiff = (double(context->maxBlockSize) / context->sampleRate) * 1000000000;
    context->logMessage("delay:" + String(expectedDiff - diff));
    ;
    if(expectedDiff - diff < 0){
        context->logMessage("ALARM!! Buffer Underrun!!!");
    }
    if (guiAlive)
        if (context->observer != nullptr) {
            (context->observer->*(context->observer->hostPositionChanged)) (int(context->info->timeInSamples));
        }
    
}

void OrbishAudioProcessor::handleRecordBlock(int start, int stop) {
    int resetCurrentIndex = false;
    int indexUpdate = 0;
    // Recording or recording has been stopped but we want to write current buffer with fade out
    int64 start0 = 0, end0 = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
    // don't record if no layers in track
    // in destructive mode make sure we write within bounds
    if (activeTrack->getRecordMode() > 3 && *activeTrack->CurrentTop < 0) {
        *activeTrack->CurrentTop = 0;
    }
    // start should always be 0.  Pre-snap samples are used for crossfade
    start = 0;
    // stop should always == buffer size (except when output buffer size is exceeded). Post-snap samples are used for crossfade
    activeTrack->EndFadeOffset = min(stop, context->allocatedLength - activeTrack->CurrentRecordingIndex);
    stop = min(context->samplesPerBlock, context->allocatedLength - activeTrack->CurrentRecordingIndex);
    int samplesToRead = stop;
    int fadeIn = 0;
    int fadeOut = 0;
    int adjustedFadeTime = min(context->fadeTime, context->maxBlockSize);
    
    start0 = Time::getHighResolutionTicks();
    if (activeTrack->FirstRecordingBuffer) {
        if(*activeTrack->LoopDuration == 0){
            activeTrack->BeginFadeOffset = start = (start >= context->samplesPerBlock) ? 0 : max(start, 0);
        }
        // in overdub mode create a new layer
        if ((activeTrack->getRecordMode() < 4)
            && ((activeTrack->CurrentRecordingIndex + context->maxBlockSize < *activeTrack->LoopDuration)
                || (*activeTrack->LoopDuration == 0))){
            for (auto i = 0;i<=*activeTrack->CurrentTop;++i) {
                if(context->xchange->writeGainModifierQueue->read_available()
                   && context->xchange->readGainModifierQueue->write_available()){
                    auto gm = context->xchange->writeGainModifierQueue->front();
                    context->xchange->writeGainModifierQueue->pop();
                    gm->startLevel = context->feedback;
                    gm->operation = GainModifier::OperationType::All;
                    gm->buffer = (*activeTrack->Layers)[i]->Buffer;
                    context->xchange->readGainModifierQueue->push(gm);
                }
            }
            if (*activeTrack->CurrentTop < (*activeTrack->Layers).size() - int(1)) {
                (*activeTrack->Layers)[*activeTrack->CurrentTop]->dirty = true;
                activeTrack->setActivePlaybackLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
                activeTrack->setActiveRecordingLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
            }else if((*activeTrack->Layers).size() == 0 || activeTrack->getActiveRecordingLayer()->dirty){
                auto l = activeTrack->AddLayer(true);
                activeTrack->setActivePlaybackLayer(l);
                activeTrack->setActiveRecordingLayer(l);
                //context->logMessage("1. adding layer at: " + String(activeTrack->CurrentRecordingIndex));
                end1 = Time::getHighResolutionTicks();
                //} );
                end2 = Time::getHighResolutionTicks();
                context->logMessage("1. at loop start overdub ");
                
                context->logMessage("playback layer: " + String(activeTrack->getActivePlaybackLayer()->index));
                context->logMessage("recording layer: " + String(l->index));
                context->logMessage("layers size: " + String(activeTrack->Layers->size()));
                //context->logMessage("Duration outer block: " + String(end2 - start2));
                //context->logMessage("Duration inner block: " + String(end1 - start1));
            }
        }
        fadeIn = adjustedFadeTime;
        //  end0 = Time::getHighResolutionTicks();
        //DBG("Duration block 1: " + String(end0 - start0));
    }
    // main scenario -> write the whole buffer without fade
    // in overdub/punch mode at the end of the loop apply fade in/out
    // and prepare writing to the next layer previously created
    int tail = 0, samplesBeforeTail = 0;
    if ((activeTrack->getRecordMode() == kRecLoopOver || activeTrack->getRecordMode() == kRecFixedLoopOver || activeTrack->getRecordMode() == kRecPunch)
        && *activeTrack->LoopDuration > 0
        && activeTrack->CurrentRecordingIndex + context->maxBlockSize >= *activeTrack->LoopDuration) {
        samplesToRead = min(max(*activeTrack->LoopDuration - activeTrack->CurrentRecordingIndex - start, 0), samplesToRead);
        samplesBeforeTail = start + samplesToRead;
        if (activeTrack->FirstRecordingBuffer) samplesToRead = 0;
        tail = context->buffer->getNumSamples() - samplesBeforeTail;
        resetCurrentIndex = true;
        fadeOut = adjustedFadeTime;
        if(*activeTrack->LoopDuration == activeTrack->CurrentRecordingIndex){
            activeTrack->CurrentRecordingIndex = 0;
        }
    }
    if (activeTrack->CurrentRecordingIndex == 0) {
        fadeIn = adjustedFadeTime;
    }
    if (activeTrack->LastRecordingBuffer) {
        fadeOut = adjustedFadeTime;
    }
    //            if (samplesToRead > 0 && samplesToRead <= 2 * context->fadeTime) {
    //                fadeIn = min(fadeIn, samplesToRead);
    //                fadeOut = min(fadeOut, samplesToRead);
    //            }
    //                DBG("samples to read: " + String(samplesToRead));
    //                DBG("fi: " + String(fadeIn));
    //                DBG("fo: " + String(fadeOut));
    // fade in
    //  start0 = Time::getHighResolutionTicks();
    for (uint c = 0; c < context->audioInputsCount; ++c)
    {
                        activeTrack->getActiveRecordingLayer()->Buffer->copyFrom(c
                                                                                 , activeTrack->CurrentRecordingIndex + start
                                                                                 , context->buffer->getReadPointer(c, start)
                                                                                 , samplesToRead);
        if (fadeIn > 0 && samplesToRead > 0) {
            activeTrack->getActiveRecordingLayer()->Buffer->copyFromWithRamp(c
                                                                     , activeTrack->CurrentRecordingIndex + start
                                                                     , context->buffer->getReadPointer(c, start)
                                                                     , fadeIn
                                                                     , .0f
                                                                     , 1.0f);
        }
        // fade out
        if (fadeOut > 0) {
            activeTrack->getActiveRecordingLayer()->dirty = true;
            if (resetCurrentIndex || activeTrack->LastRecordingBuffer) {
                if(!resetCurrentIndex && *activeTrack->LoopDuration > 0){
                    activeTrack->getActiveRecordingLayer()->Buffer->copyFromWithRamp(c
                                                                             , activeTrack->CurrentRecordingIndex + start + samplesToRead - fadeOut
                                                                             , context->buffer->getReadPointer(c, context->maxBlockSize - fadeOut)
                                                                             , fadeOut
                                                                             , 1.0f
                                                                             , .0f);
                }else{
                    if (activeTrack->CurrentRecordingIndex > fadeOut && !activeTrack->FirstRecordingBuffer) {
                        if(context->xchange->writeGainModifierQueue->read_available()
                           && context->xchange->readGainModifierQueue->write_available()){
                            auto gm = context->xchange->writeGainModifierQueue->front();
                            context->xchange->writeGainModifierQueue->pop();
                            auto samplesTillEnd = (activeTrack->EndFadeOffset>0)?activeTrack->EndFadeOffset:samplesToRead;
                            gm->startIndex = activeTrack->CurrentRecordingIndex + samplesTillEnd - context->fadeTime;
                            gm->numberOfSamples = context->fadeTime;
                            gm->startLevel = 1.0f;
                            gm->endLevel = .0f;
                            gm->operation = GainModifier::OperationType::RampChannelRegion;
                            gm->buffer = activeTrack->getActiveRecordingLayer()->Buffer;
                            context->xchange->readGainModifierQueue->push(gm);
                        }
                    }
                    if(context->xchange->writeGainModifierQueue->read_available()
                       && context->xchange->readGainModifierQueue->write_available()){
                        auto gm = context->xchange->writeGainModifierQueue->front();
                        context->xchange->writeGainModifierQueue->pop();
                        gm->startIndex = activeTrack->BeginFadeOffset;
                        gm->numberOfSamples = context->fadeTime;
                        gm->startLevel = .0f;
                        gm->endLevel = 1.0f;
                        gm->operation = GainModifier::OperationType::RampChannelRegion;
                        gm->buffer = activeTrack->getActiveRecordingLayer()->Buffer;
                        context->xchange->readGainModifierQueue->push(gm);
                    }
                }
            }
        }
    }
    // update the current recording index
    end0 = Time::getHighResolutionTicks();
    //DBG("Duration block 2: " + String(end0 - start0));
    // in overdub mode record the remaining samples to the new layer if the buffer goes beyond the loop length
    if (resetCurrentIndex && tail > 0) {
        start0 = Time::getHighResolutionTicks();
        start2 = Time::getHighResolutionTicks();
        if(guiAlive){
            if (context->xchange->writeVisualisationBufferQueue->read_available() && context->xchange->readVisualisationBufferQueue->write_available()) {
                shared_ptr<BufferForVisualisation> b;
                context->xchange->writeVisualisationBufferQueue->pop(b);
                b->numSamples = *activeTrack->LoopDuration;
                b->buffer = activeTrack->getActiveRecordingLayer()->Buffer;
                b->layerIndex = activeTrack->getActiveRecordingLayer()->index;
                (context->observer->*(context->observer->layerChange)) ( activeTrack->Index,b->layerIndex);
                context->xchange->readVisualisationBufferQueue->push(b);
            }
        }
        end2 = Time::getHighResolutionTicks();
        if (true) {
            
            for (auto i = 0;i<=*activeTrack->CurrentTop;++i) {
                start1 = Time::getHighResolutionTicks();
                if(context->xchange->writeGainModifierQueue->read_available()
                   && context->xchange->readGainModifierQueue->write_available()){
                    auto gm = context->xchange->writeGainModifierQueue->front();
                    context->xchange->writeGainModifierQueue->pop();
                    gm->startLevel = context->feedback;
                    gm->operation = GainModifier::OperationType::All;
                    gm->buffer = (*activeTrack->Layers)[i]->Buffer;
                    context->xchange->readGainModifierQueue->push(gm);
                }
                end1 = Time::getHighResolutionTicks();
            }
            if (*activeTrack->CurrentTop < (*activeTrack->Layers).size() - int(1)) {
                (*activeTrack->Layers)[*activeTrack->CurrentTop]->dirty = true;
                ++(*activeTrack->CurrentTop);
                activeTrack->setActivePlaybackLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
                activeTrack->setActiveRecordingLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
            }else if((*activeTrack->Layers).size() == 0 || activeTrack->getActiveRecordingLayer()->dirty){
                start1 = Time::getHighResolutionTicks();
                activeTrack->setActivePlaybackLayer(activeTrack->getActiveRecordingLayer());
                auto l = activeTrack->AddLayer(true);
                activeTrack->setActiveRecordingLayer(l);
                end1 = Time::getHighResolutionTicks();
                context->logMessage("2. in overlap overdub ");

                context->logMessage("playback layer: " + String(activeTrack->getActivePlaybackLayer()->index));
                context->logMessage("recording layer: " + String(l->index));
                context->logMessage("layers size: " + String(activeTrack->Layers->size()));
            }
        }
        activeTrack->CurrentRecordingIndex = 0;
        start2 = Time::getHighResolutionTicks();
        
        fadeIn = min(tail, context->fadeTime);
        for (uint c = 0; c < context->audioInputsCount; ++c){
            activeTrack->getActiveRecordingLayer()->Buffer->copyFromWithRamp(c
                                                                             , 0
                                                                             , context->buffer->getReadPointer(c, samplesBeforeTail)
                                                                             , fadeIn
                                                                             , 0.0f
                                                                             , 1.0f);
            
            // main chunk
            if (tail > fadeIn) {
                activeTrack->getActiveRecordingLayer()->Buffer->copyFrom(c
                                                                         , fadeIn
                                                                         , context->buffer->getReadPointer(c, samplesBeforeTail + fadeIn)
                                                                         , tail - fadeIn);
            }
            end2 = Time::getHighResolutionTicks();
            end0 = Time::getHighResolutionTicks();
        }
        indexUpdate = tail;
        if (activeTrack->FirstRecordingBuffer) {
            activeTrack->FirstRecordingBuffer=false;
        }
    }
    else {
        if (activeTrack->FirstRecordingBuffer) {
            indexUpdate = start + samplesToRead;
            activeTrack->FirstRecordingBuffer=false;
        }
        else if (activeTrack->LastRecordingBuffer) {
            indexUpdate = activeTrack->EndFadeOffset;
        }
        else {
            indexUpdate = samplesToRead;
        }
    }
    activeTrack->CurrentRecordingIndex += indexUpdate;
    if (activeTrack->CurrentRecordingIndex >= context->allocatedLength)
        // stop activeTrack->Recording if reached the end of the buffer
    {
        activeTrack->StopRecordingAfter();
        activeTrack->Recording = false;
        activeTrack->setRecordingArmed(false);
    }
}


void OrbishAudioProcessor::handlePlaybackBlock(int start, int stop) {
    int samplesToRead = context->maxBlockSize;
    for(auto track :tracks){
        // get audio only fro tracks currently playing
        if (((track->isActive() && track->IsPlaying()) || track->Playing)
            && *track->LoopDuration > 0
            && track->Layers->size() > 0)
        {
            //context->logMessage("now playing");
            auto reSync = track->realignment;
            int tail = min(context->maxBlockSize,max(samplesToRead + *track->CurrentPlayingIndex - *track->LoopDuration,0));
            auto activeLayer = track->getActivePlaybackLayer();
            if ((!(nullptr == activeLayer) &&
                 !(nullptr == activeLayer->Buffer) ) &&
                ((!track->Muted && !aTrackIsSoloed)
                || (track->Soloed))) {
                // read loop content
                int index = *track->CurrentPlayingIndex;
                int indexMinusDelayComp = index - context->delayCompensation;
                // compute the index of the position to read from taking into account latency compensation
                if ((indexMinusDelayComp) >= 0) {
                    if ((indexMinusDelayComp) < *track->LoopDuration) {
                        index = indexMinusDelayComp;
                    }
                    else {
                        index = indexMinusDelayComp - *track->LoopDuration;
                    }
                }
                else {
                    index = *track->LoopDuration + indexMinusDelayComp;
                }
                int currentPlayBuffer = activeLayer->index;
                // set start to 0 if out of bounds
                start = (start >= context->maxBlockSize) ? 0 : max(start, 0);
                start=0;
                if(*track->CurrentPlayingIndex == 0){
                   start = activeTrack->BeginFadeOffset;
                }
                // set stop to block size if out of bounds
                stop = min(max(stop, context->maxBlockSize), context->maxBlockSize);
                stop = min(*track->LoopDuration - *track->CurrentPlayingIndex, stop);
                int fadeIn, fadeOut;
                fadeIn = fadeOut = 0;
                int sourceIndex = index + start;
                int targetIndex = start;
                // determine number of samples to read
                samplesToRead = max(0,stop - start);
                //                                                context->logMessage("sourceIndex: " + String(sourceIndex));
                //                                                context->logMessage("LoopDuration: " + String(*track->LoopDuration));
                //                                                context->logMessage("CurrentPlayingIndex: " + String(*track->CurrentPlayingIndex));
                //                                                context->logMessage("index: " + String(index));
                //                                                context->logMessage("SamplesToRead: " + String(samplesToRead));
                //                                                context->logMessage("track: " + String(track->Index));
                if (*track->LoopDuration > 0) {
                    int reverseSourceIndex=0;
                    {
                        int tmpIndex = *track->LoopDuration - 1 - index;
                        reverseSourceIndex = max(tmpIndex - stop, 0);
                    }
                    String s = String();
                    for (int l = 0; currentPlayBuffer > -1 && l < currentPlayBuffer + 1; l++) {
                        auto buff = (*track->Layers)[l]->Buffer;
                        int64 start0, end0;
                        start0 = Time::getHighResolutionTicks();
                        
                        shared_ptr<AudioBuffer<float> > temp = nullptr;
                        if(context->xchange->readBufferQueue->read_available() > 0){
                            temp = context->xchange->readBufferQueue->front();
                            context->xchange->readBufferQueue->pop();
                        }else{
                            auto b = make_unique<AudioBuffer<float> >();
                            b->setSize(context->audioInputsCount, context->samplesPerBlock);
                            b->clear();
                            temp = move(b);
                        }
                        end0 = Time::getHighResolutionTicks();
                        for (uint32 c = 0; c < context->audioInputsCount;++c) {
                            // crossfade for realignment
                            int adjustedPosition = track->Reverse?reverseSourceIndex:sourceIndex;
                            if(reSync->isFadeInProgress() && !context->skipAlign){
                                // write fade out with signal from unadjusted position
                                 fadeOut = fadeIn = min(abs(reSync->getCurrentOffset()), context->samplesPerBlock - targetIndex);
                                int vAdjustedPosition = track->getAdjustedLoopPosition(index, reSync->getTotalOffset());
                                if (track->Reverse) {
                                    int tmpIndex = *track->LoopDuration - 1 - vAdjustedPosition;
                                    adjustedPosition = max(tmpIndex - stop, 0);
                                }else{
                                    adjustedPosition = vAdjustedPosition;
                                }
                                fadeIn = max(0,min(*track->LoopDuration - adjustedPosition - 1, fadeIn));
                                temp->addFromWithRamp(c
                                                      , targetIndex
                                                      , (*track->Layers)[l]->Buffer->getReadPointer(c, reverseSourceIndex)
                                                      , fadeOut, 1 - reSync->getCurrentFadeLevelStart(), 1 - reSync->getCurrentFadeLevelEnd());
                                // write fade in with signal from adjusted position
                                temp->addFromWithRamp(c
                                                      , targetIndex
                                                      , (*track->Layers)[l]->Buffer->getReadPointer(c, adjustedPosition)
                                                      , fadeIn, reSync->getCurrentFadeLevelStart(), reSync->getCurrentFadeLevelEnd());
                                if (c == context->audioInputsCount - 1
                                        && std::abs(reSync->getCurrentOffset()) > 0
                                        && l == currentPlayBuffer) {
                                    reSync->BufferProcessed();
                                    if(reSync->isRealigned()){
                                        *track->CurrentPlayingIndex = vAdjustedPosition;
                                        tail = max(samplesToRead + *track->CurrentPlayingIndex - *track->LoopDuration,0);
                                        samplesToRead = samplesToRead-tail;
                                    }
                                }
                                fadeOut = 0;
                            }else{
                                if (track->FirstPlaybackBuffer
                                    || track->FirstSoloBuffer
                                    || track->LastMuteBuffer
                                    || (track->getActivePlaybackLayer()->index == l && track->getActivePlaybackLayer()->FirstLayerBuffer)) {
                                    int diffToEnd = adjustedPosition - *track->LoopDuration;
                                    fadeIn = min(abs(diffToEnd),min(samplesToRead, context->fadeTime));
                                    temp->addFromWithRamp(c
                                                          , targetIndex
                                                          , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex)
                                                          , fadeIn, 0.0f, 1.0f);
                                    if(c == context->audioInputsCount-1
                                       && l == currentPlayBuffer){
                                        track->getActivePlaybackLayer()->FirstLayerBuffer = false;
                                        track->FirstPlaybackBuffer = false;
                                    }
                                }
                                if (track->LastPlaybackBuffer
                                    || track->LastSoloBuffer
                                    ||track->FirstMuteBuffer
                                    || (track->getActivePlaybackLayer()->index == l && track->getActivePlaybackLayer()->LastLayerBuffer)) {
                                    if(track->getActivePlaybackLayer()->LastLayerBuffer){
                                        samplesToRead = context->maxBlockSize;
                                    }
                                    fadeOut = min(samplesToRead, context->fadeTime);
                                    temp->addFromWithRamp(c
                                                          , targetIndex + samplesToRead - fadeOut
                                                          , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex + samplesToRead - fadeOut)
                                                          , fadeOut, 1.0f, 0.0f);
                                    tail = 0;
                                }
//                                context->logMessage("targetIndex: " + String(targetIndex));
//                                context->logMessage("sourceIndex: " + String(sourceIndex));
//                                context->logMessage("fadeIn: " + String(fadeIn));
//                                context->logMessage("fadeOut: " + String(fadeOut));
//                                context->logMessage("samplesToRead: " + String(samplesToRead));
//                                context->logMessage("temp->getNumSamples(): " + String(temp->getNumSamples()));
                            }
                            temp->addFrom(c
                                          , targetIndex + fadeIn
                                          , (*track->Layers)[l]->Buffer->getReadPointer(c, adjustedPosition + fadeIn)
                                          , jmax(samplesToRead - fadeIn - fadeOut, 0));
                        }
                        bool noNullPtrInTmpBuffer = true;
                        // read beginning of the loop to tail of the output buffer
                        if (tail>0) {
                            for (uint32 c = 0; c < context->audioInputsCount;++c) {
                                temp->addFrom(c
                                              //, context->maxBlockSize - tail + min(tail, context->fadeTime)
                                              , context->maxBlockSize - tail
                                              //, (*track->Layers)[l]->Buffer->getReadPointer(c, min(tail, context->fadeTime))
                                              , (*track->Layers)[l]->Buffer->getReadPointer(c, 0)
                                              , tail); //max(0,tail- context->fadeTime));
                            }
                        }
                        if(temp->getReadPointer(0)==nullptr){
                            noNullPtrInTmpBuffer = false;
                        }
                        for (uint32 c = 0; c < context->audioInputsCount;++c) {
                            if (track->Reverse) {
                                temp->reverse(c, 0, context->maxBlockSize);
                            }
                            if(temp->getReadPointer(c)==nullptr){
                                context->logMessage("Nullpointer in temp buffer");
                                if(noNullPtrInTmpBuffer){
                                    context->logMessage("Previously ok");
                                }else{
                                    context->logMessage("Previously nok");
                                }
                            }else{
                                smoothVolume(track->PreviousOutputLevel, track->getOutputLevel(), samplesToRead+tail, temp.get(), context->buffer.get(), c);
                            }
                        }
                    }
                }
            }
            // update playback index while playing
            // for active track also check rec mode | don't bother for the others
            // update index
            if (tail) {
                * track->CurrentPlayingIndex = tail;
            }else{
                *track->CurrentPlayingIndex += samplesToRead;
            }
            if (guiAlive && track->isActive())
                if (context->observer != nullptr) {
                    (context->observer->*(context->observer->updatePlayPosition)) (*track->CurrentPlayingIndex, track->Reverse);
                }
            *track->Progress = (double(*track->CurrentPlayingIndex) / double(*track->LoopDuration));
        }
    }
}

void OrbishAudioProcessor::smoothVolume(double& origin, double destination, int samplesToRead, AudioBuffer<float>* source, AudioBuffer<float>* target, int channel){
    double sign=0;
    double diff = double(destination) - double(origin);
    if(diff!=0){
        sign=diff/abs(diff);
        double delta = min(abs(diff),double(context->maxDelta));
        double intermediateDestination = double(origin) + delta*sign;
        target->addFromWithRamp(channel
                             , 0
                             , source->getReadPointer(channel)
                             , samplesToRead
                             , origin
                             , intermediateDestination);
        origin = intermediateDestination;
    }else {
        target->addFrom(channel
                             , 0
                             , source->getReadPointer(channel)
                             , samplesToRead
                             , destination);
                                 
    }
}
//==============================================================================


bool OrbishAudioProcessor::isEditorActive()
{
    //logMessage("Returning editor: " + editor->getName());
    return true; // (change this to false if you choose to not supply an editor)
}

bool OrbishAudioProcessor::hasEditor() const
{
    //logMessage("Returning editor: " + editor->getName());
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OrbishAudioProcessor::createEditor()
{
    editor = new OrbishAudioProcessorEditor(*this, parameters);
    //  logMessage("editor: " + editor->getName());
    guiAlive = true;
    return editor;
}

//==============================================================================
void OrbishAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OrbishAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrbishAudioProcessor();
}

bool OrbishAudioProcessor::loadFromValueTree(ValueTree* tree) {
    context->logMessage("loading tracks");
    if (tree == nullptr)return false;
    if (!tree->hasType("project"))return false;
    auto tracksElement = tree->getChildWithName("tracks");
    int numTracksToLoad = tracksElement.getNumChildren();
    for (auto i = 0;i < numTracksToLoad;++i) {
        ValueTree tChild(tracksElement.getChild(i));
        auto trackIdx = int(tChild.getProperty("index"));
        if (trackIdx >= 0) {
            if (trackIdx == tracks.size()) {
                processNewTrack();
            }
            Track* t(tracks[trackIdx]);
            if (!loadTrackFromValueTree(&tChild, t)) { context->logMessage("File load failed"); }
        }
    }
    //    context->logMessage("loading groups");
    auto groupTree = tree->getChildWithName("groups");
    for (auto groupNode: groupTree) {
        processGroupSelect(groupNode.getProperty("index"));
        for (auto j = 0;j < groupNode.getNumChildren(); ++j) {
            auto groupedTrack = groupNode.getChild(j);
            processAddToGroup(groupedTrack.getProperty("index"));
        }
    }
    if (guiAlive) {
        (context->observer->*(context->observer->updatePlayPosition)) (0, activeTrack->Reverse);
    }
    activeTrack->RegisterLoop(0);
    activeTrack->UpdateLoopVisualizer();
    return true;
}

bool OrbishAudioProcessor::loadTrackFromValueTree(ValueTree* trackTree, Track* track) {
    int numLoopsToLoad = trackTree->getNumChildren() - 1;
    for (auto i = 0;i < numLoopsToLoad;++i) {
        ValueTree lChild(trackTree->getChild(i));
        auto loopIdx = int(lChild.getProperty("index", -1));
        if (loopIdx >= 0) {
            if (loopIdx == track->loops.size()) {
                track->processNewLoop();
            }
            if (!loadLoopFromValueTree(&lChild, track->loops[loopIdx]))return false;
        }
    }
    auto params = trackTree->getChildWithName("TrackParams");
    *track->state = params;
    return true;
}

bool OrbishAudioProcessor::loadLoopFromValueTree(ValueTree* loopTree, Loop* loop) {
    int numLayersToLoad = loopTree->getNumChildren();
    for (auto i = 0;i < numLayersToLoad;++i) {
        auto lyChild = loopTree->getChild(i);
        auto layerIdx = int(lyChild.getProperty("index", -1));
        if (layerIdx >= 0) {
            if (layerIdx == loop->Layers->size()) {
                loop->AddLayer(true, context);
            }
            shared_ptr<Layer> layer(loop->Layers.get()->at(layerIdx));
            String filePath = lyChild.getProperty("file");
            if (filePath.isEmpty())return false;
            File file(filePath);
            auto formatMgr = make_unique<AudioFormatManager>();
            formatMgr->registerBasicFormats();
            unique_ptr<AudioFormatReader> reader(formatMgr->createReaderFor(file));
            if (reader.get() != nullptr)
            {
                try {
                    unique_ptr<AudioSampleBuffer> buffer = make_unique<AudioSampleBuffer>(reader->numChannels, (int)reader->lengthInSamples);
                    reader->read(buffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
                    layer->Buffer->setSize(buffer->getNumChannels(), buffer->getNumSamples());
                    layer->Buffer->clear();
                    for(auto i=0;i<buffer->getNumChannels();++i){
                        layer->Buffer->copyFrom(i, 0, *buffer, i, 0, buffer->getNumSamples());
                    }
                    loop->LoopDuration = buffer->getNumSamples();
                }
                catch (...) {
                    context->logMessage("read of audio file failed: " + String(filePath));
                }
            }
            layer->dirty = true;
        }
    }
    loop->activePlaybackLayer = loop->Layers.get()->at(loop->CurrentTop) ;
    return true;
}
