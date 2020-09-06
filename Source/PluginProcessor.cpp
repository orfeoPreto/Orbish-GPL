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

//#include "Track.h"
std::unique_ptr<RangedAudioParameter> OrbishAudioProcessor::createParamFromBool(AudioParameterBool* boolParam, bool defaultValue) {// return
    auto p = std::make_unique<AudioProcessorValueTreeState::Parameter>(boolParam->paramID,
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
    return std::move(p);
}

std::unique_ptr<RangedAudioParameter> OrbishAudioProcessor::createParamFromInt(AudioParameterInt* intParam, int defaultValue) {
    auto p = std::make_unique<AudioProcessorValueTreeState::Parameter>(intParam->paramID,
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
    return std::move(p);
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
        std::make_unique<AudioParameterFloat>("globalMix", "GlobalMix"
                        ,NormalisableRange<float>(-120.0f, 6.0f
                            , [](float start, float end, float gain) { return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start); }
                                                  , [](float start, float end, float dB) {
                                                      float n = Decibels::decibelsToGain(dB, start);
                                                      float d = Decibels::decibelsToGain(end);
                                                      float r = n / d;
                                                      return r;
                                                  }), 0.5f, "db")
            , std::make_unique<AudioParameterFloat>("latency", "Latency"
                                                  ,NormalisableRange<float>(-300.0f, 300.0f), 0, "ms")
            , std::make_unique<AudioParameterFloat>("inputLevel", "InputLevel"
                            , NormalisableRange<float>(-60.0f, 6.0f
                                , [](float start, float end, float gain) { return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start); }
                                                       , [](float start, float end, float dB) {
                                                           float n = Decibels::decibelsToGain(dB, start);
                                                           float d = Decibels::decibelsToGain(end);
                                                           float r = n / d;
                                                           return r;
                                }), 0.5f, "db")
            , std::make_unique<AudioParameterFloat>("outputLevel", "outputLevel"
                            , NormalisableRange<float>(-60.0f, 6.0f
                                , [](float start, float end, float gain) { return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end) , start); }
                                , [](float start, float end, float dB) { return Decibels::decibelsToGain(dB, start) / Decibels::decibelsToGain(end); }), 0.5f, "db")
            , createParamFromBool(new AudioParameterBool("record", "Record", false), false)
            , createParamFromBool(new AudioParameterBool("play", "Play", false), false)
            , createParamFromBool(new AudioParameterBool("stop", "Stop", false), false)
            , createParamFromBool(new AudioParameterBool("mute", "Mute", false), false)
            , createParamFromBool(new AudioParameterBool("solo", "Solo", false), false)
            , std::make_unique<AudioParameterBool>("monitor", "Monitor", false)
            , std::make_unique<AudioParameterBool>("reverse", "Reverse", false)
            , std::make_unique<AudioParameterBool>("undo", "Undo", false)
            , std::make_unique<AudioParameterBool>("redo", "Redo", false)
            , createParamFromBool(new AudioParameterBool("reset", "Reset", false), false)
            , createParamFromBool(new AudioParameterBool("nextTrack", "NextTrack", false), false)
            , createParamFromBool(new AudioParameterBool("previousTrack", "PreviousTrack", false), false)
            , std::make_unique<AudioParameterBool>("newTrack", "NewTrack", false)
            , createParamFromBool(new AudioParameterBool("removeTrack", "RemoveTrack", false), false)
            , std::make_unique<AudioParameterBool>("trigger", "Trigger", false)
            , createParamFromBool(new AudioParameterBool("muteAll", "MuteAll", false), false)
            , createParamFromBool(new AudioParameterBool("stopAll", "StopAll", false), false)
            , createParamFromBool(new AudioParameterBool("startAll", "StartAll", false), false)
            , createParamFromBool(new AudioParameterBool("pauseAll", "PauseAll", false), false)
            , createParamFromBool(new AudioParameterBool("resetAll", "ResetAll", false), false)
            , std::make_unique<AudioParameterChoice>("snap", "Snap", StringArray("No Sync","Measure","Beat"), 0.5)
            , std::make_unique<AudioParameterChoice>("mode", "Mode", StringArray("Loop","Repeat","Append","Overwrite","Punch","Clear"), 0)
            , createParamFromBool(new AudioParameterBool("bounce", "Bounce", false), false)
            , createParamFromInt(new AudioParameterInt("trackSelect", "Track", 0, 100, 0), 0)
            , createParamFromBool(new AudioParameterBool("nextLoop", "NextLoop", false), false)
            , createParamFromBool(new AudioParameterBool("previousLoop", "PreviousLoop", false), false)
            , std::make_unique<AudioParameterBool>("newLoop", "NewLoop", false)
            , createParamFromBool(new AudioParameterBool("removeLoop", "RemoveLoop", false), false)
            , createParamFromInt(new AudioParameterInt("loopSelect", "Loop", 0, 100, 0), 0)
           // , createParamFromInt(new AudioParameterInt("selectGroup", "SelectGroup", 0, 10, 0), 0)
			, std::make_unique<AudioParameterChoice>("selectGroup", "SelectGroup", StringArray("A","B","C","D","E","F","G","H","I","J"), 0)
            , createParamFromBool(new AudioParameterBool("addToGroup", "AddToGroup", false), false)
            , createParamFromBool(new AudioParameterBool("removeFromGroup", "RemoveFromGroup", false), false)




        })
{
    
    context = new OrbishContext();
    context->buffer = new AudioBuffer<float>();
    context->feedback = Decibels::decibelsToGain(float(-0.1));
    context->mix = Decibels::decibelsToGain(parameters.getParameter("globalMix")->getValue());
    context->layerQueue = new boost::lockfree::spsc_queue<Layer*, boost::lockfree::capacity<3> >;
    context->xchange = new DataExchange();
    initGroups();
    context->allocatorThread = std::thread(
        [this] {
            File file = File(File::getSpecialLocation(File::userHomeDirectory)).getChildFile("Orbish").getChildFile("Orbish.log");
            auto result = file.create();
            if (result.wasOk()) {
                context->logger = std::make_shared<FileLogger>(file, "Hi");
            }
            while (keepRunning) {
                if (context->audioInputsCount > 0) {
                    if (context->layerQueue->write_available() > 0) {
                        auto l = new Layer();
                        l->Buffer = new AudioBuffer<float>();
                        l->Buffer->clear();
                        l->Buffer->setSize(context->audioInputsCount, context->allocatedLength);
                        if (l->Buffer->getNumChannels() == 0) {
                            continue;
                        }
                        context->layerQueue->push(l);
                    }
                    else {
                        if (context->xchange->writeBufferQueue->write_available() > 0) {
                            BufferForVisualisation* b = new BufferForVisualisation();
                            b->buffer = new AudioBuffer<float>();
                            String s = String(pointer_sized_int(b));
                            b->buffer->clear();
                            b->buffer->setSize(context->audioInputsCount, context->allocatedLength);
                            context->xchange->writeBufferQueue->push(b);
                        }
                        else {
                            std::this_thread::sleep_for(std::chrono::milliseconds(3));
                        }
                    }
                }
                for (auto track : tracks) {
                    if (track != nullptr && *track->LoopDuration > 0) {
                        if (track->doBounce) {
                            track->BounceAllHistory();
                            track->doBounce = false;
                        }
                        for (auto layer:*track->Layers){
                            if (layer->Dirty)break;
                            auto length = 0;
                            if (track->loopToBeExtended && layer->Buffer->getNumSamples() < context->allocatedLength) {
                                length = context->allocatedLength;
                            }
                            else if (layer->Buffer->getNumSamples() > ((*track->LoopDuration) + context->samplesPerBlock)
                                && !(track->getRecordMode() > 0 && track->getRecordMode() < 4)) {
                                length = (*track->LoopDuration) + context->samplesPerBlock;
                            }
                            if (length > 0) {
                                layer->Buffer->setSize(context->audioOutputsCount, length, true, true, false);
                            }
                        }
                        track->loopToBeExtended = false;
                    }
                }
                context->flushLogs();
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
                                        context->mix = Decibels::decibelsToGain(newValue);
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
                                        context->audioInputsCount = getTotalNumInputChannels();
                                        context->audioOutputsCount = getTotalNumOutputChannels();
                                    if (samplesPerBlock != context->samplesPerBlock
                                        || sampleRate != context->sampleRate) {
                                        context->allocatedLength = int(context->sampleRate * context->defaultLoopLength);
                                        context->samplesPerBlock = samplesPerBlock;
                                        context->sampleRate = int(sampleRate);
                                        context->fadeTime = (samplesPerBlock < 256) ? samplesPerBlock : int(samplesPerBlock * 0.5);
                                        context->fadeInc = float(context->fadeTime * context->sampleRate);
                                        samplesPerMinute = float(context->sampleRate * 60.0);
                                        secondsPerSample = 1.0f / context->sampleRate;


                                        cleanup();
                                        Thread::sleep(100);
                                         //create tracks
                                        addTrack(true);
                                        activeTrack = tracks.getLast();
                                        *activeTrack->LoopDuration = 0;
                                        context->fadeTime = int(context->sampleRate * .05);
                                        context->buffer->setSize(context->audioInputsCount, samplesPerBlock);

//                                        logMessage("outputchannels:" + String(context->audioInputsCount));
//                                        logMessage("context buffer channels:" + String(context->buffer->getNumChannels()));
                                        int holdValue = 50;
                                        inputMeterSource.setMaxHoldMS(holdValue);
                                        outputMeterSource.setMaxHoldMS(holdValue);
                                        inputMeterSource.resize(context->audioInputsCount, holdValue);
                                        outputMeterSource.resize(context->audioInputsCount, holdValue);

                                        messenger = MessageManager::getInstance();
                                        context->init = true;
                                        parameters.addParameterListener("globalMix", this);
                                        parameters.addParameterListener("latency", this);
                                        parameters.addParameterListener("muteAll", this);
                                        parameters.addParameterListener("trackSelect", this);
                                        parameters.addParameterListener("nextTrack", this);
                                        parameters.addParameterListener("previousTrack", this);
                                        parameters.addParameterListener("newTrack", this);
                                        parameters.addParameterListener("removeTrack", this);
                                        parameters.addParameterListener("stopAll", this);
                                        parameters.addParameterListener("startAll", this);
                                        parameters.addParameterListener("pauseAll", this);
                                        parameters.addParameterListener("resetAll", this);
                                        parameters.addParameterListener("selectGroup", this);
                                        parameters.addParameterListener("addToGroup", this);
                                        parameters.addParameterListener("removeFromGroup", this);

                                    }
                                 //   logMessage("end prepareToPlay");
                                  //  logMessage("sample block size:" + String(context->samplesPerBlock));

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
                                        MessageManager::callAsync(
                                            [this]() {
                                                (context->observer->*(context->observer->newTrack))();
                                            });
                                    }
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
                                    // start playback of all armed tracks
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
                                    if (isEditorActive()) {
                                        MessageManager::callAsync(
                                            [this, midi, ctxt]()
                                            {
                                                (ctxt->observer->*(context->observer->handleMidi))(midi);
                                            }
                                        );
                                    }
                                }

                                void OrbishAudioProcessor::changeTrack(int& nextTrackIndex) {
                                    if (nextTrackIndex < 0 || nextTrackIndex >= tracks.size())return;
                                    activeTrack->setActive(false);
                                    activeTrack = tracks[nextTrackIndex];
                                    activeTrack->setActive(true);

                                    if (guiAlive) {
                                        (context->observer->*(context->observer->trackChange)) (nextTrackIndex);
                                        activeTrack->UpdateLoopVisualizer();
                                    }
                                    nextTrack = -1;
                                    auto g = getTrackGroup(activeTrack);
                                    if (nullptr != g) {
                                        CurrentGroup = g;
                                    }
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
									if (context->bpm != info.bpm
										|| context->timeSigTop != info.timeSigDenominator) {
										context->bpm = int(info.bpm);
										context->samplesPerBeat = int(beatsToSamples(1.0));
										//logMessage(String("bpm: ") + String(context->bpm));
                                    }

                                    context->maxBlockSize = buffer.getNumSamples();
                                    context->info->bpm = info.bpm;
                                    context->info->editOriginTime = info.editOriginTime;
                                    context->info->isPlaying = info.isPlaying;
                                    context->info->ppqPosition = info.ppqPosition;
                                    context->info->timeInSamples = int64(beatsToSamples(float(info.ppqPosition)));
                                    context->info->timeSigDenominator = info.timeSigDenominator;
                                    context->info->timeSigNumerator = info.timeSigNumerator;
                                    context->info->ppqPositionOfLastBarStart = info.ppqPositionOfLastBarStart;
                                }

                                void OrbishAudioProcessor::realign(){
                                 int df = int(quartersToSamples(context->info->ppqPosition)) % (context->samplesPerBeat * context->timeSigTop * 4);
                                 if (context->info->isPlaying && df < context->samplesPerBeat) {
                                  //                                        logMessage(String("spb:") + String(context->samplesPerBeat) + String("\nNum: ") + String(context->timeSigTop)
                                  //                                            + String("\nsamplesPerBlock: ") + String(context->samplesPerBlock)
                                  //                                            + String("\nmaxBlockSize: ") + String(context->maxBlockSize)
                                  //
                                  //                                            + String("\timeInSamplesFromPpq: ") + String(int(quartersToSamples(context->info->ppqPosition)))
                                  //                                            + String("\nfps: ") + String(context->sampleRate)
                                   //                                           logMessage(String("\ndf: ") + String(df));
                                                                           if (!hostHasPlayed)hostHasPlayed = true;
                                  auto diffHost = differenceFromClosestBeatInSamples(int(quartersToSamples(context->info->ppqPosition)));
                                  //                                        if (abs(diffHost) < context->samplesPerBlock * 0.5f) {
                                  if (abs(diffHost) < context->samplesPerBlock * 0.5f) {
                                                                               for(auto track:tracks){
                                                                                   auto diffPlugin = differenceFromClosestBeatInSamples(*track->CurrentPlayingIndex);
                                  //                                                logMessage(String("diffHost:") + String(diffHost) + String("\ndiffPlugin: ") + String(diffPlugin));
                                 int diff=0;
                                   if(((diffHost<0)&&(diffPlugin>0)) || ((diffPlugin<0)&&(diffHost>0))){
                                       if(abs(diffHost)+abs(diffPlugin) > context->samplesPerBeat*0.5){
                                           diff = context->samplesPerBeat - int(abs(diffHost)) - int(abs(diffPlugin));
                                       }
                                  }else{
                                      diff = int(diffHost - diffPlugin);
                                  }
                                  if (abs(diff) > 0) {
                                      if (!((diff < 0 && *track->CurrentPlayingIndex < abs(diff))
                                        || (diff > 0 && *track->CurrentPlayingIndex > * track->LoopDuration - diff))) {
                                  //  if(abs(diff)<context->samplesPerBlock){
                                  auto sign = diff/abs(diff);
                                  diff = std::min(abs(diff),context->samplesPerBlock);
                                          track->RealignOffset = diff * sign;
                                  //                                                            logMessage(String("realign at:") + String(conte xt->info->ppqPosition) + String("\ndiff: ")+String(diff));
                                  // }
                                                                                       }
                                  //                                                }
                                                                               }
                                                                           }
                                                                       }
                                                                   }
                                }

                                void OrbishAudioProcessor::captureTrigger(int startRecordingSample){
                                    // Auto Trigger mode: check if there is any sound before starting new activeTrack->Recording
                                    if (!activeTrack->Recording
                                        && *activeTrack->LoopDuration == 0
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
                                        if (startRecordingSample == -1)
                                            startRecordingSample = context->maxBlockSize;
                                    }
                                }

                                void OrbishAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
                                {
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
                                    for (uint c = 0; c < context->audioInputsCount; ++c) {
                                        context->buffer->copyFrom(c, 0, buffer, c, 0, buffer.getNumSamples());
                                     //   logMessage("channel to write:" + String(c));
                                    }
                                    
                                    // add input gain to input
                                    context->buffer->applyGain(activeTrack->getInputLevel());
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
                                            realign();
                                        int startRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be started
                                        int stopRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be stopped
                                        int startPlayingSample = -1; // sample number in buffer when playback should be started
                                        int stopPlayingSample = -1; // sample number in buffer when playback should be stopped
                                        int startReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be started
                                        int stopReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be stopped
                                        int toggleMuteSample = -1; //sample number in buffer when Mute state should be toggled
                                        int toggleSoloSample = -1; //sample number in buffer when Solo state should be toggled
                                        int startLoopChangeSample = -1;
                                        captureTrigger(startRecordingSample);

                                        // actions will be performed in sync based on the snap mode
                                        // look for the sample corresponding to the snap (bar or beat) and perform action
                                        // if no sync the actions are performed immediately

                                            //print2("timeSigBottom:", data.transport->timeSigBottom);
                                            // start position is either buffer start, or startRecordingSample if we are detecting automatically
                                        double currentPos = context->info->ppqPosition;
                                        if (startRecordingSample >= 0) {
                                            currentPos += samplesToBeats(startRecordingSample);
                                        }
                                        double expectedPos = 0;
                                        // expected position is next down beat (except if buffer is exactly on beat
                                        if (activeTrack->getSnapMode() == kSnapMeasure)
                                        {
                                            expectedPos = context->info->ppqPositionOfLastBarStart;
                                            while (expectedPos < currentPos)
                                                expectedPos += double(context->timeSigTop) / double(context->timeSigBottom) * 4.0;
                                        }
                                        // snap to next quarter note
                                        else if (activeTrack->getSnapMode() == kSnapQuarter)
                                        {
                                            expectedPos = floor(currentPos);
                                            if (expectedPos != currentPos)
                                                expectedPos++;
                                        }
                                        else if (activeTrack->getSnapMode() == kSnapNone)
                                        {
                                            expectedPos = currentPos;
                                        }
										
                                        double expectedSamplePosition = beatsToSamples(expectedPos);
                                        int nextSample = int(floor(expectedSamplePosition + .5)) - int(context->info->timeInSamples);
//                                        logMessage("nextSample" + String(nextSample));
//                                        logMessage("timeInSamples:" + String(context->info->timeInSamples));
//                                        logMessage("ppq: " + String(context->info->ppqPosition));
//                                        logMessage("timeInSecs: " + String(context->info->timeInSeconds));
//                                        logMessage("currentPlayingIndex:" + String(*activeTrack->CurrentPlayingIndex));
                                        // manage activeTrack->Recording
                                        if (!activeTrack->Recording && activeTrack->isRecordingArmed())
                                        {
                                            startRecordingSample = nextSample;
                                        }
                                        else if (activeTrack->Recording && !activeTrack->isRecordingArmed())
                                        {
                                            stopRecordingSample = nextSample;
                                        }
                                        // manage activeTrack->Playing
                                        if ((!activeTrack->Playing && activeTrack->isPlayArmed()) || globalAction == kGlobalPlay || globalAction == kGlobalResume)
                                        {
                                            startPlayingSample = nextSample;
                                        }
                                        else if ((activeTrack->Playing && !activeTrack->isPlayArmed()) || globalAction == kGlobalStop || globalAction == kGlobalPause)
                                        {
                                            stopPlayingSample = nextSample;
                                        }

                                        // manage reversing
                                        if (!activeTrack->Reverse && activeTrack->isReverseArmed())
                                        {
                                            startReverseSample = nextSample;
                                        }

                                        else if (activeTrack->Playing && activeTrack->Reverse && !activeTrack->isReverseArmed()) {
                                            stopReverseSample = nextSample;
                                        }
                                        if (activeTrack->loopChangeArmed)
                                        {
                                            startLoopChangeSample = nextSample;
                                        }
                                        // manage Mute
                                        if ((activeTrack->isMuteArmed() != activeTrack->Muted) || globalAction == kGlobalMute) {
                                            toggleMuteSample = nextSample;
                                        }
                                        // manage Solo
                                        if (activeTrack->isSoloArmed() != activeTrack->Soloed) {
                                            toggleSoloSample = nextSample;
                                        }
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

                                        DBG("startRecordingSample: " + String(startRecordingSample));
                                        bool doStartRecording = (startRecordingSample >= 0) && (context->maxBlockSize > startRecordingSample);
                                        bool doStopRecording = (stopRecordingSample >= 0) && (context->maxBlockSize > stopRecordingSample);
                                        if (doStartRecording) {
                                            activeTrack->StartRecordingBefore();
                                        }
                                        else if (doStopRecording) {
                                            // switch recording off but still allow for the last samples to be written + fade
                                            activeTrack->StopRecordingBefore();
                                        }

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

                                        // manage mute state
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
                                        // manage solo state
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
                                                        for(auto groupedTrack:*CurrentGroup){ groupedTrack->ChangeLoopBefore(groupedTrack->ActiveLoop->Index - 1);
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
                                        if (activeTrack->Recording || (activeTrack->LastRecordingBuffer)) {
                                            handleRecordBlock(startRecordingSample, stopRecordingSample);
                                        }
                                        if (!activeTrack->isMonitoring()) {
                                            // prevent input from ending up in the output buffer
                                            context->buffer->clear();
                                        }
                                        if (context->info->isPlaying) {
                                            handlePlaybackBlock((startReverseSample >= 0) ? startReverseSample : startPlayingSample, (stopReverseSample >= 0) ? stopReverseSample : stopPlayingSample);
                                        }
                                    }
                                    inputMeterSource.measureBlock(buffer);
                                    if (!activeTrack->isMonitoring() && !context->info->isPlaying) {
                                        // prevent input from ending up in the output buffer
                                        context->buffer->clear();
                                    }
                                    // overwrite the output buffer with the processed audio
                                    for (uint c = 0; c < context->audioInputsCount; ++c) {
                                        buffer.clear(c, 0, context->maxBlockSize);
                                        buffer.copyFrom(c, 0, context->buffer->getReadPointer(c), context->maxBlockSize, context->mix);
                                    }

                                    // run the methods defined to be executed after the block
                                    for(auto t :tracks){
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
                                    outputMeterSource.measureBlock(buffer);
                                  //  logMessage("begin processBlock");

                                }

                                void OrbishAudioProcessor::handleRecordBlock(int start, int stop) {
                                    int resetCurrentIndex = false;
                                    int indexUpdate = 0;
                                    // Recording or recording has been stopped but we want to write current buffer with fade out
                                    for (uint c = 0; c < context->audioInputsCount; ++c)
                                    {
                                        int64 start0 = 0, end0 = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
                                        // don't record if no layers in track
                                        if ((*activeTrack->Layers).size() > 0) {
                                            // in destructive mode make sure we write within bounds
                                            if (activeTrack->getRecordMode() > 2 && *activeTrack->CurrentTop < 0) {
                                                *activeTrack->CurrentTop = 0;
                                            }
                                            // start should always be 0.  Pre-snap samples are used for crossfade
                                            activeTrack->BeginFadeOffset = (start >= context->samplesPerBlock) ? 0 : std::max(start, 0);
                                            start = 0;
                                            // stop should always == buffer size (except when output buffer size is exceeded). Post-snap samples are used for crossfade
                                            activeTrack->EndFadeOffset = std::min(stop, context->allocatedLength - activeTrack->CurrentRecordingIndex);
                                            stop = std::min(context->samplesPerBlock, context->allocatedLength - activeTrack->CurrentRecordingIndex);
                                            int samplesToRead = std::min(stop, context->samplesPerBlock);
                                            int fadeIn = 0;
                                            int fadeOut = 0;
                                            start0 = Time::getHighResolutionTicks();
                                            // we are past the beginning of the loop
                                            if (activeTrack->FirstRecordingBuffer) {
                                                // in overdub mode create a new layer
                                                if ((activeTrack->getRecordMode() < 3)) {
                                                    if (c == context->audioInputsCount - 1) {
                                                      //  start2 = Time::getHighResolutionTicks();
                                                        (*activeTrack->Layers)[*activeTrack->CurrentTop]->Dirty = true;
                                                        if (int(*activeTrack->CurrentTop < (*activeTrack->Layers).size()) - 1) {
                                                            ++(*activeTrack->CurrentTop);
                                                        }

                                                    //    start1 = Time::getHighResolutionTicks();
                                                        activeTrack->AddLayer(false);
                                                     //   end1 = Time::getHighResolutionTicks();
                                                        //} );
                                                     //   end2 = Time::getHighResolutionTicks();
                                                    }
                                                }
                                                fadeIn = context->fadeTime;
                                              //  end0 = Time::getHighResolutionTicks();
                                                //DBG("Duration block 1: " + String(end0 - start0));
                                            }
                                            // main scenario -> write the whole buffer without fade

                                        // in overdub/punch mode at the end of the loop apply fade out
                                        // and prepare writing to the next layer previously created 
                                            int tail = 0;
                                            if ((activeTrack->getRecordMode() == kRecLoopOver || activeTrack->getRecordMode() == kRecPunch)
                                                && *activeTrack->LoopDuration > 0
                                                && activeTrack->CurrentRecordingIndex + context->buffer->getNumSamples() >= *activeTrack->LoopDuration) {
                                                samplesToRead = std::min(std::max(*activeTrack->LoopDuration - activeTrack->CurrentRecordingIndex - start, 0), samplesToRead);
                                                tail = context->buffer->getNumSamples() - (start + samplesToRead);
                                                resetCurrentIndex = true;
                                                fadeOut = context->fadeTime;
                                            }
                                            if (activeTrack->LastRecordingBuffer) {
                                                fadeOut = context->fadeTime;
                                            }
                                            if (samplesToRead > 0 && samplesToRead <= 2 * context->fadeTime) {
                                                fadeIn = std::min(fadeIn, samplesToRead);
                                                fadeOut = std::min(fadeOut, samplesToRead);
                                            }
                                            //                DBG("samples to read: " + String(samplesToRead));
                                            //                DBG("fi: " + String(fadeIn));
                                            //                DBG("fo: " + String(fadeOut));
                                            // fade in
                                          //  start0 = Time::getHighResolutionTicks();
                                            if (fadeIn > 0) {
                                                (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->copyFromWithRamp(c
                                                    , activeTrack->CurrentRecordingIndex + start
                                                    , context->buffer->getReadPointer(c, start)
                                                    , fadeIn
                                                    , 0.0f
                                                    , 1.0f);
                                                //                    DBG("fade in: " + String(samplesToRead));
                                            }
                                             // fade out
                                            if (fadeOut > 0) {
                                                if (samplesToRead == 0) {
                                                    if (activeTrack->CurrentRecordingIndex > fadeOut) {
                                                        fadeOut = context->fadeTime;
                                                        (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->applyGainRamp(c
                                                            , activeTrack->CurrentRecordingIndex - fadeOut
                                                            , fadeOut
                                                            , 1.0f
                                                            , 0.0f);
                                                    }
                                                }
                                                else {
                                                    (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->copyFromWithRamp(c
                                                        , activeTrack->CurrentRecordingIndex + samplesToRead - fadeOut
                                                        , context->buffer->getReadPointer(c, samplesToRead - fadeOut)
                                                        , fadeOut
                                                        , 1.0f
                                                        , 0.0f);
                                                    //                    DBG("fade out" + String(samplesToRead));
                                                }
                                            }
                                            if (samplesToRead > (fadeIn + fadeOut)) {
                                                (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->copyFrom(c
                                                    , activeTrack->CurrentRecordingIndex + start + fadeIn
                                                    , context->buffer->getReadPointer(c, start + fadeIn)
                                                    , samplesToRead - fadeIn - fadeOut);
                                            }

                                            // update the current recording index
                                            end0 = Time::getHighResolutionTicks();
                                            //DBG("Duration block 2: " + String(end0 - start0));
                                            // in overdub mode record the remaining samples to the new layer if the buffer goes beyond the loop length
                                            if (resetCurrentIndex && tail > 0) {
                                                if (c == context->audioInputsCount - 1) {
                                                    end0 = Time::getHighResolutionTicks();
                                                    start2 = Time::getHighResolutionTicks();
                                                    if (context->xchange->writeBufferQueue->read_available() && context->xchange->readBufferQueue->write_available()) {
                                                        BufferForVisualisation* b;
                                                        context->xchange->writeBufferQueue->pop(b);
                                                        for (uint i = 0; i < context->audioInputsCount; ++i) {
                                                            b->buffer->copyFrom(i, 0, *((*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer), c, 0, *activeTrack->LoopDuration);
                                                        }
                                                        b->numSamples = *activeTrack->LoopDuration;
                                                        context->xchange->readBufferQueue->push(b);
                                                    }
                                                    end2 = Time::getHighResolutionTicks();

                                                    if (true) {  //activeTrack->getRecordMode() == kRecLoopOver){
                                                        if (*activeTrack->CurrentTop < (*activeTrack->Layers).size() - int(1)) {
                                                            ++(*activeTrack->CurrentTop);
                                                            (*activeTrack->Layers)[*activeTrack->CurrentTop]->Dirty = true;
                                                        }

                                                        if (*activeTrack->CurrentTop == (*activeTrack->Layers).size() - int(1)) {

                                                          //  start1 = Time::getHighResolutionTicks();
                                                            activeTrack->AddLayer(false);
                                                           // end1 = Time::getHighResolutionTicks();
                                                        }
                                                    }
                                                    //                        auto tick = Time::getHighResolutionTicks();
                                                    //                        DBG("hires per sec:\n");
                                                    //                        DBG(Time::getHighResolutionTicksPerSecond());
                                                    //                        DBG("start\n");
                                                    //                        DBG(tick);
                                                                            //DBG("Duration addLayer 2: " + String(end1 - start1));
                                                                            //DBG("Duration updateLoopBuffer 2: " + String(end2 - start2));

                                                    //                        DBG("end\n");
                                                    //                        tick = Time::getHighResolutionTicks();
                                                    //                        DBG(tick);
                                                    activeTrack->CurrentRecordingIndex = 0;
                                                }

                                                if (tail < context->fadeTime) {
                                                    fadeIn = tail;
                                                }
                                                (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->copyFromWithRamp(c
                                                    , 0
                                                    , context->buffer->getReadPointer(c, start + samplesToRead)
                                                    , fadeIn
                                                    , 0.0f
                                                    , 1.0f);

                                                // main chunk
                                                if (tail > fadeIn) {
                                                    (*activeTrack->Layers)[*activeTrack->CurrentTop]->Buffer->copyFrom(c
                                                        , fadeIn
                                                        , context->buffer->getReadPointer(c, start + samplesToRead + fadeIn)
                                                        , tail - fadeIn);
                                                }
                                                indexUpdate = tail;
                                                //                        for(int i = 0; i<context->samplesPerBlock; ++i){
                                                //                            std::cout << activeTrack->Layers[*activeTrack->CurrentTop].Buffer->getSample(c, i) << std::endl;
                                                //                        }
                                                end0 = Time::getHighResolutionTicks();
                                                //DBG("Duration block 3: " + String(end0 - start0));
                                            }
                                            else {
                                                if (activeTrack->FirstRecordingBuffer) {
                                                    indexUpdate = samplesToRead - activeTrack->BeginFadeOffset;
                                                }
                                                else if (activeTrack->LastRecordingBuffer) {
                                                    indexUpdate = samplesToRead - activeTrack->EndFadeOffset;
                                                }
                                                else {
                                                    indexUpdate = samplesToRead;
                                                }
                                            }
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

                                    DBG("Start: " + String(start));
                                    DBG("Stop: " + String(stop));
                                    for(auto track :tracks){
                                        // get audio only fro tracks currently playing
                                        if (((track->isActive() && track->IsPlaying()) || track->Playing) && *track->LoopDuration > 0)
                                        {
                                            if (*activeTrack->CurrentPlayingIndex == 0) {
                                                //for (int i = 0;i < *activeTrack->LoopDuration;i++) {
                                                //	DBG(String(activeTrack->Layers[0][0]->Buffer->getSample(0, i)));
                                                //	DBG(String(activeTrack->Layers[0][0]->Buffer->getSample(1, i)));
                                                //}
                                            }
                                            if ((!track->Muted && !aTrackIsSoloed) || (track->Soloed)) {
                                                // read loop content
                                                int index = 0;
                                                index = *track->CurrentPlayingIndex;
                                                // compute the index of the position to read from taking into account latency compensation
                                                if ((index - context->delayCompensation) >= 0) {
                                                    if ((index - context->delayCompensation) < *track->LoopDuration) {
                                                        index = *track->CurrentPlayingIndex - context->delayCompensation;
                                                    }
                                                    else {
                                                        index = *track->CurrentPlayingIndex - context->delayCompensation - *track->LoopDuration;
                                                    }
                                                }
                                                else {
                                                    index = *track->LoopDuration + *track->CurrentPlayingIndex - context->delayCompensation;
                                                }
                                                int currentPlayBuffer = *track->CurrentTop;
                                                if (track->Recording) {
                                                    if (track->getRecordMode() == 0) {
                                                        currentPlayBuffer--;
                                                    }
                                                }
                                                start = (start >= context->maxBlockSize) ? 0 : std::max(start, 0);
                                                stop = (stop < 0) ? context->maxBlockSize : std::min(stop, context->maxBlockSize);
                                                int fadeIn, fadeOut;
                                                fadeIn = fadeOut = 0;
                                                int sourceIndex = index + start;
                                                int targetIndex = start;
                                                // determine number of samples to read
                                                samplesToRead = stop - start;

                                                DBG("sourceIndex: " + String(sourceIndex));
                                                DBG("LoopDuration: " + String(*track->LoopDuration));
                                                DBG("CurrentPlayingIndex: " + String(*track->CurrentPlayingIndex));
                                                DBG("index: " + String(index));
                                                DBG("SamplesToRead: " + String(samplesToRead));
                                                DBG("track: " + String(track->Index));


                                                if (*track->LoopDuration > 0) {
                                                    if (track->Reverse) {
                                                        index = *track->LoopDuration - 1 - index;
                                                        sourceIndex = std::max(index - stop, 0);
                                                    }
                                                    String s = String();

                                                    for (int l = 0; currentPlayBuffer > -1 && l < currentPlayBuffer + 1; l++) {
                                                        for (uint32 c = 0; c < context->audioInputsCount;++c) {
                                                            AudioBuffer<float> temp{};
                                                            temp.setSize(context->audioInputsCount, context->maxBlockSize);
                                                            temp.clear();
                                                            if (track->FirstPlaybackBuffer || track->FirstSoloBuffer || track->LastMuteBuffer || track->Realigned) {
                                                                fadeIn = std::min(samplesToRead, context->fadeTime);
                                                                temp.addFromWithRamp(c
                                                                    , targetIndex
                                                                    , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex)
                                                                    , fadeIn, 0.0f, 1.0f);
                                                                if (c == context->audioInputsCount - 1)track->Realigned = false;
                                                            }

                                                            if (track->LastPlaybackBuffer || track->LastSoloBuffer ||track->FirstMuteBuffer || realignOffset > 0) {
                                                                fadeOut = std::min(samplesToRead, context->fadeTime);
                                                                temp.addFromWithRamp(c
                                                                    , targetIndex + samplesToRead - fadeOut
                                                                    , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex + samplesToRead - fadeOut)
                                                                    , fadeOut, 1.0f, 0.0f);
                                                                if (c == context->audioInputsCount - 1 && track->RealignOffset > 0) {
                                                                    track->Realigned = true;
                                                                }
                                                            }
                                                            if (*track->CurrentPlayingIndex == 0 && track->EndFadeOffset > 0) {
                                                                temp.addFrom(c
                                                                    , 0
                                                                    , (*track->Layers)[l]->Buffer->getReadPointer(c, *track->LoopDuration - 1)
                                                                    , track->EndFadeOffset);
                                                            }
                                                            if (*track->CurrentPlayingIndex == *track->LoopDuration - context->maxBlockSize && track->BeginFadeOffset > 0) {
                                                                temp.addFrom(c
                                                                    , context->maxBlockSize - track->BeginFadeOffset
                                                                    , (*track->Layers)[l]->Buffer->getReadPointer(c, 0)
                                                                    , track->BeginFadeOffset);
                                                            }
                                                            temp.addFrom(c
                                                                , targetIndex + fadeIn
                                                                , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex + fadeIn)
                                                                , jmax(samplesToRead - fadeIn - fadeOut, 0));

                                                            if (track->Reverse) {
                                                                temp.reverse(c, 0, context->maxBlockSize);
                                                            }
                                                            context->buffer->addFrom(c
                                                                , 0
                                                                , temp.getReadPointer(c)
                                                                , samplesToRead
                                                                , track->getOutputLevel());
                                                        }
                                                    }
                                                }
                                            }

                                            // update playback index while playing
                                            // for active track also check rec mode | don't bother for the others
                                                // update index
                                            *track->CurrentPlayingIndex += samplesToRead + track->RealignOffset;
                                            track->RealignOffset = 0;
                                            if (*track->CurrentPlayingIndex >= *track->LoopDuration)
                                                * track->CurrentPlayingIndex = 0;
                                            if (guiAlive && track->isActive())
                                                if (context->observer != nullptr) {
                                                    (context->observer->*(context->observer->updatePlayPosition)) (*track->CurrentPlayingIndex, track->Reverse);
                                                }
                                            *track->Progress = (double(*track->CurrentPlayingIndex) / double(*track->LoopDuration));
                                        }
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
                                    std::unique_ptr<XmlElement> xml(state.createXml());
                                    copyXmlToBinary(*xml, destData);
                                }

                                void OrbishAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
                                {
                                    // You should use this method to restore your parameters from this memory block,
                                    // whose contents will have been created by the getStateInformation() call.
                                    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
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
                                    ;									return true;
                                }

                                bool OrbishAudioProcessor::loadTrackFromValueTree(ValueTree* trackTree, Track* track) {
                                    int numLoopsToLoad = trackTree->getNumChildren();
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
                                            if (layerIdx == loop->Layers.size()) {
                                                loop->AddLayer(true, context);
                                            }
                                            Layer* layer(loop->Layers[layerIdx]);
                                            String filePath = lyChild.getProperty("file");
                                            if (filePath.isEmpty())return false;
                                            File file(filePath);
                                            auto formatMgr = std::make_unique<AudioFormatManager>();
                                            formatMgr->registerBasicFormats();
                                            std::unique_ptr<AudioFormatReader> reader(formatMgr->createReaderFor(file));
                                            if (reader.get() != nullptr)
                                            {
                                                try {
                                                    std::unique_ptr<AudioSampleBuffer> buffer = std::make_unique<AudioSampleBuffer>(reader->numChannels, (int)reader->lengthInSamples);
                                                    reader->read(buffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
                                                    layer->Buffer->setSize(buffer->getNumChannels(), buffer->getNumSamples());
                                                    layer->Buffer->clear();
                                                    layer->Buffer->copyFrom(0, 0, *buffer, 0, 0, buffer->getNumSamples());
                                                    layer->Buffer->copyFrom(1, 0, *buffer, 1, 0, buffer->getNumSamples());
                                                    loop->LoopDuration = buffer->getNumSamples();
                                                }
                                                catch (...) {
                                                    context->logMessage("write of audio file failed: " + String(filePath));
                                                }
                                            }
                                            layer->Dirty = true;
                                        }
                                    }
                                    return true;
                                }
