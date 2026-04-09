/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterDefinitions.h"
#include <iostream>
#include <chrono>
using namespace std;



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
context(nullptr),
parameters(*this, nullptr, "OrbishState", createParameterLayout()),
allocator(context, activeTrack, trackToAdd)
{
    //_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

    context = std::make_shared<OrbishContext>();

	context->buffer = make_shared<AudioBuffer<float> >();
    context->inputBuffer = make_shared<AudioBuffer<float> >();

    context->feedback = Decibels::decibelsToGain(float(-0.0));
    context->mix = Decibels::decibelsToGain(std::stof(parameters.getParameter("globalMix")->getCurrentValueAsText().toStdString()));
    context->clickLevel = parameters.getParameter("clickLevel")->getValue();

    context->xchange = new DataExchange();
    
    initGroups();
    auto formatMgr = make_unique<AudioFormatManager>();
    
    formatMgr->registerBasicFormats();
    unique_ptr<AudioFormatReader> reader(formatMgr->createReaderFor(make_unique<MemoryInputStream>( BinaryData::Low_click_aif, BinaryData::Low_click_aifSize, false)));
    context->clickBuffer = make_unique<AudioSampleBuffer>(reader->numChannels, (int)reader->lengthInSamples);
    reader->read(context->clickBuffer.get(), 0, int(reader->lengthInSamples), 0, true, true);
    
    unique_ptr<AudioFormatReader> reader2(formatMgr->createReaderFor(make_unique<MemoryInputStream>( BinaryData::High_Click_aif, BinaryData::High_Click_aifSize, false)));
    context->barStartClickBuffer = make_unique<AudioSampleBuffer>(reader2->numChannels, (int)reader2->lengthInSamples);
    reader2->read(context->barStartClickBuffer.get(), 0, int(reader2->lengthInSamples), 0, true, true);

    primarySynchronizer = make_unique<HostSynchronizer>(context);
    secondarySynchronizer =  make_unique<InternalSynchronizer>(context,nullptr);
    allocator.start();
}

OrbishAudioProcessor::~OrbishAudioProcessor()
{
    allocator.stop();
    context->lockForStateUpdate(true);
    cleanup();
    context->lockForStateUpdate(false);
    //context->logger = nullptr;
    context = nullptr;
}

void OrbishAudioProcessor::initBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages){
    if (context->init)context->init = false;
    if (midiMessages.data.size() > 0) {
        processMidi(midiMessages);
    }
    ScopedNoDenormals noDenormals;
    {
        AudioPlayHead* head = getPlayHead();
        Optional<AudioPlayHead::PositionInfo> info;
        if (head != nullptr)
            info = head->getPosition();

        // Detect real host transport: standalone wrapper may provide PositionInfo
        // with default BPM but no PPQ position. Require both BPM and PPQ to
        // distinguish a real DAW host from the standalone wrapper.
        bool hasHostTransport = info.hasValue()
            && info->getBpm().hasValue()
            && info->getPpqPosition().hasValue();

        if (hasHostTransport)
        {
            context->timeSigBottom = info->getTimeSignature().hasValue()
                ? info->getTimeSignature()->denominator : 4;
            context->timeSigTop = info->getTimeSignature().hasValue()
                ? info->getTimeSignature()->numerator : 4;
        }
        else
        {
            // Standalone mode: use sensible defaults
            if (context->bpm <= 0) context->bpm = 120.0;
            if (context->timeSigBottom <= 0) context->timeSigBottom = 4;
            if (context->timeSigTop <= 0) context->timeSigTop = 4;
        }

        context->samplesPerBeat = int(context->beatsToSamples(1.0));
        context->samplesPerQuarter = int(context->quartersToSamples(1.0));

        double hostBpm = hasHostTransport ? info->getBpm().orFallback(context->bpm) : context->bpm;
        if (context->bpm != hostBpm
            || context->timeSigTop != context->timeSigBottom) {
            context->bpm = hostBpm;
            processTempoChange(context->bpm);
            context->samplesPerBeat = int(std::round(context->beatsToSamples(1.0)));
            context->samplesPerQuarter = int(context->quartersToSamples(1.0));
        }
        context->maxBlockSize = buffer.getNumSamples();

        if (hasHostTransport)
        {
            context->info->setBpm(info->getBpm());
            context->info->setEditOriginTime(info->getEditOriginTime());
            context->info->setIsPlaying(info->getIsPlaying());
            context->info->setPpqPosition(info->getPpqPosition());

            context->info->setTimeInSamples(int64(context->quartersToSamples(info->getPpqPosition().orFallback(0))));
            AudioPlayHead::TimeSignature sig;
            sig.denominator = info->getTimeSignature().hasValue() ? info->getTimeSignature()->denominator : 4;
            sig.numerator = info->getTimeSignature().hasValue() ? info->getTimeSignature()->numerator : 4;
            context->info->setTimeSignature(sig);
            context->info->setPpqPosition(info->getPpqPositionOfLastBarStart());

            if (info->getLoopPoints().hasValue()) {
                AudioPlayHead::LoopPoints lp;
                lp.ppqEnd = info->getLoopPoints()->ppqEnd;
                lp.ppqStart = info->getLoopPoints()->ppqStart;
                context->info->setLoopPoints(lp);
            }
            context->info->setIsLooping(info->getIsLooping());
        }
        else
        {
            // Standalone: enable click on first block
            if (syncState.standaloneSamplePosition == 0) {
                context->clickEnabled = true;
            }
            // Standalone: advance beat clock based on accumulated samples
            syncState.standaloneSamplePosition += context->maxBlockSize;

            context->info->setBpm(context->bpm);
            context->info->setIsPlaying(true);

            // Derive PPQ from accumulated samples
            double currentPpq = context->samplesToQuarters(double(syncState.standaloneSamplePosition));

            // Set timeInSamples from actual position (matches host path)
            context->info->setTimeInSamples(syncState.standaloneSamplePosition);

            // Compute bar start PPQ, then set ppqPosition to bar start (matches host path)
            double qPerBar = context->quartersPerBar();
            double barStartPpq = (qPerBar > 0) ? floor(currentPpq / qPerBar) * qPerBar : 0.0;
            context->info->setPpqPosition(barStartPpq);
            context->info->setPpqPositionOfLastBarStart(barStartPpq);

            AudioPlayHead::TimeSignature sig;
            sig.denominator = context->timeSigBottom;
            sig.numerator = context->timeSigTop;
            context->info->setTimeSignature(sig);
            context->info->setIsLooping(false);
        }
    }
#if DEBUG_LOG
    int diff = context->maxBlockSize - context->samplesPerBlock;
    if(diff != 0){
        //context->logMessage("buffer excess:" + String(diff));
    }
#endif
    if (context->info->getIsPlaying()) {
        syncState.trackHostSamples += context->maxBlockSize;
    }
}


void OrbishAudioProcessor::captureTrigger(int& startRecordingSyncPoint){
    // Auto Trigger mode: check if there is any sound before starting new activeTrack->Recording
    if (!activeTrack->Recording
        && activeTrack->isRecordingArmed()
        && activeTrack->isAutoTrigger()
        && !activeTrack->Triggered)
    {
        
         const float* const* samples = context->buffer->getArrayOfReadPointers();
        for (uint32 channel = 0; channel < context->audioInputsCount && startRecordingSyncPoint == -1; channel++)
            
            for (int i = 0; i < context->maxBlockSize && startRecordingSyncPoint == -1; i++)
            {
                if (abs(samples[channel][i]) > context->triggerThreshold)
                {
                    startRecordingSyncPoint = i;
                    activeTrack->Triggered = true;
                }
            }
    }
}


void OrbishAudioProcessor::handleReverseEvent(int startReverseSyncPoint, int stopReverseSyncPoint){
    // manage activeTrack->Reverse state
    
    if (primarySynchronizer->isSyncEventImminent(startReverseSyncPoint)) {
        if (nullptr != getTrackGroup(activeTrack)) {
            for(auto groupedTrack:*CurrentGroup){
                groupedTrack->StartReverse();
            }
        }
        else {
            activeTrack->StartReverse();
        }
    }
    else if (primarySynchronizer->isSyncEventImminent(stopReverseSyncPoint)) {
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

void OrbishAudioProcessor::handleRecordingEvent(int startRecordingSyncPoint, int stopRecordingSyncPoint){
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
    ////context->logMessage("startRecordingSample: " + String(startRecordingSample));
    if (primarySynchronizer->isSyncEventImminent(startRecordingSyncPoint)) {
        activeTrack->StartRecordingBefore();
    }
    else if (primarySynchronizer->isSyncEventImminent(stopRecordingSyncPoint)) {
        // switch recording off but still allow for the last samples to be written + fade
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::handlePlaybackEvent(int startPlayingSyncPoint, int stopPlayingSyncPoint){
    if (primarySynchronizer->isSyncEventImminent(startPlayingSyncPoint))
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
    else if (primarySynchronizer->isSyncEventImminent(stopPlayingSyncPoint))
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


void OrbishAudioProcessor::handleMuteEvent(int toggleMuteSyncPoint){
    if (primarySynchronizer->isSyncEventImminent(toggleMuteSyncPoint)) {
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

bool OrbishAudioProcessor::handleSoloEvent(int toggleSoloSyncPoint){
    auto toggleSolo = primarySynchronizer->isSyncEventImminent(toggleSoloSyncPoint);
    if (toggleSolo) {
        if (activeTrack->isSoloArmed()) {
            mixState.aTrackIsSoloed = true;
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
            mixState.aTrackIsSoloed = false;
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

void OrbishAudioProcessor::handleLoopChangeEvent(int startLoopChangeSyncPoint){
        if (primarySynchronizer->isSyncEventImminent(startLoopChangeSyncPoint)) {
        if (nullptr != getTrackGroup(activeTrack)) {
            int diff = activeTrack->nextLoop - activeTrack->ActiveLoop->Index;
            for(auto groupedTrack:*CurrentGroup){
                auto newIdx = groupedTrack->ActiveLoop->Index + diff % groupedTrack->loops.size();
//                if (newIdx >=0 && newIdx < groupedTrack->loops.size()) {
                    groupedTrack->ChangeLoopBefore(newIdx);
//                }
            }
        }
        else {
            activeTrack->ChangeLoopBefore(activeTrack->nextLoop);
        }
    }
}



void OrbishAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
#if DEBUG_LOG
    int64 beginMark = Time::getHighResolutionTicks();
    int64 startBeginning = Time::getHighResolutionTicks();
#endif
    //   logMessage("begin processBlock");
    if(buffer.getNumSamples() == 0)return;
    initBlock(buffer, midiMessages);
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = context->audioInputsCount; i < context->audioOutputsCount; ++i)
        buffer.clear(i, 0, context->maxBlockSize);
    
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
    }
//    DBG("inputBuffer: " + String(context->inputBuffer->getMagnitude(0, 0, context->maxBlockSize)));
    if(context->xchange->writeMeasureBufferQueue->read_available() > 0 &&
       context->xchange->readMeasureBufferQueue->write_available() > 0){
        auto mb = context->xchange->writeMeasureBufferQueue->front();
        mb->buffer = context->inputBuffer;
        mb->source = &inputMeterSource;
#if DEBUG_LOG
        auto peak = mb->buffer->getMagnitude(0, mb->buffer->getNumSamples());
        int64 startFinal = Time::getHighResolutionTicks();
        int64 startMeasure = Time::getHighResolutionTicks();
        //context->logMessage("peak: "+ String(peak));
#endif
        context->xchange->writeMeasureBufferQueue->pop();
        context->xchange->readMeasureBufferQueue->push(mb);
    }else{
        inputMeterSource.measureBlock(*context->inputBuffer);
    }
#if DEBUG_LOG
    int64 endMeasure = Time::getHighResolutionTicks();
#endif
//    if(activeTrack->refresh){
//        activeTrack->RefreshLoopVisualizer();
//    }
    // no action if host doesn't play
    if (/* PLUGIN_MODE && */ !context->info->getIsPlaying()) {
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
            Realignment::realign(context, &tracks, syncState.hostHasPlayed);
        }
        Events e;
        if (!trackChange.changing) {
            TrackEventHandler::handleEvents(e, activeTrack);
        }
#if DEBUG_LOG
        int64 endBeginning = Time::getHighResolutionTicks();
        ////context->logMessage("Beginning section:" + String(endBeginning - startBeginning));
#endif
        if (activeTrack->Recording || (activeTrack->LastRecordingBuffer)) {
#if DEBUG_LOG
            int64 startRec = Time::getHighResolutionTicks();
#endif
            handleRecordBlock(e.startRecordingSample, e.stopRecordingSample);
#if DEBUG_LOG
            int64 endRec = Time::getHighResolutionTicks();
            ////context->logMessage("nano secs spent recording:" + String(endRec - startRec));
#endif
        }
        if (!activeTrack->isMonitoring()) {
            // prevent input from ending up in the output buffer
            context->inputBuffer->clear();
        }
        context->buffer->clear();
        if (/* not !PLUGIN_MODE || */ context->info->getIsPlaying()) {
#if DEBUG_LOG
            int64 startPlay = Time::getHighResolutionTicks();
#endif
            handlePlaybackBlock((e.startReverseSample >= 0) ? e.startReverseSample : e.startPlayingSample, (e.stopReverseSample >= 0) ? e.stopReverseSample : e.stopPlayingSample);
#if DEBUG_LOG
            int64 endPlay = Time::getHighResolutionTicks();
            ////context->logMessage("nano secs spent playing back:" + String(endPlay - startPlay));
#endif
        }
        
        if(e.toggleSolo){
            for(auto track: tracks){
                track->FirstMuteBuffer  = false;
                track->LastMuteBuffer  = false;
            }
        }
    }

    if (/*  PLUGIN_MODE && */ !context->info->getIsPlaying()) {
        // prevent input from ending up in the output buffer
        context->buffer->clear();
        if(!activeTrack->isMonitoring()){
            context->inputBuffer->clear();
        }
    }
#if DEBUG_LOG
    if(activeTrack->isAutoTrigger() && context->firstRun){
        printBuffer(&buffer, "system buffer");
        printBuffer((context->inputBuffer).get(), "context->inputBuffer");
        printBuffer((context->buffer).get(), "context->buffer");
        printBuffer((activeTrack->getActivePlaybackLayer()->Buffer).get(), "context->buffer");
        context->firstRun = false;
    }
#endif

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
         smoothVolume(mixState.previousMixLevel, context->mix, context->maxBlockSize, context->buffer.get(), &buffer, c);
//        DBG("outputBuffer: " + String(buffer.getMagnitude(0, 0, context->maxBlockSize)));

    }
    // Mix click into output buffer
    if (context->clickBuffer != nullptr && context->barStartClickBuffer != nullptr) {
        clickGen.process(context, &buffer);
    } else {
        DBG("Click buffers not loaded: clickBuffer=" + String(context->clickBuffer != nullptr ? "yes" : "no")
            + " barStartClickBuffer=" + String(context->barStartClickBuffer != nullptr ? "yes" : "no"));
    }
#if DEBUG_LOG
    int64 endFinal = Time::getHighResolutionTicks();
#endif
    // run the methods defined to be executed after the block
    for(auto t :tracks){
        ////context->logMessage(String(t->RunAfters.size()));
        while (t->RunAfters.size() > 0) {
            (t->*(t->RunAfters).back()) ();
            t->RunAfters.pop_back();
        }
    }
    if (trackChange.nextTrack > -1) {
        changeTrack(trackChange.nextTrack);
    }
    if (trackChange.trackToRemove > 0) {
        removeTrack(trackChange.trackToRemove);
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
#if DEBUG_LOG
    ////context->logMessage("Final section:" + String(endFinal - startFinal));
    ////context->logMessage("Measure section:" + String(endMeasure - startMeasure));
    //  logMessage("begin iock");
    int64 diff, endMark = Time::getHighResolutionTicks();
    diff = endMark - beginMark;

    //context->logMessage("nano secs spent in callback:" + String(diff = endMark - beginMark));
    //context->logMessage("buffer size:" + String(context->maxBlockSize));
    int64 expectedDiff = (double(context->maxBlockSize) / context->sampleRate) * 1000000000;
    //context->logMessage("delay:" + String(expectedDiff - diff));
    ;
    if(expectedDiff - diff < 0){
        //context->logMessage("ALARM!! Buffer Underrun!!!");
    }
#endif
    if (guiAlive)
        if (context->observer != nullptr) {
            (context->observer->*(context->observer->hostPositionChanged)) (int(context->info->getTimeInSamples().orFallback(0)));
        }
   // context->buffer.reset();
   // context->inputBuffer.reset();
	//context->logMessage("end of processBlock");
}

void OrbishAudioProcessor::handleRecordBlock(int start, int stop) {
    int resetCurrentIndex = false;
    int indexUpdate = 0;
    // Recording or recording has been stopped but we want to write current buffer with fade out
#if DEBUG_LOG
    int64 start0 = 0, end0 = 0, start1 = 0, end1 = 0, start2 = 0, end2 = 0;
#endif
    // in destructive mode make sure we write within bounds
    if (activeTrack->getRecordMode() > 3 && *activeTrack->CurrentTop < 0) {
        if (!activeTrack->Layers->empty())
            *activeTrack->CurrentTop = 0;
    }
    // start should always be 0.  Pre-snap samples are used for crossfade
    start = 0;
    // stop should always == buffer size (except when output buffer size is exceeded). Post-snap samples are used for crossfade
    activeTrack->EndFadeOffset = min(max(stop,0), context->allocatedLength - activeTrack->CurrentRecordingIndex);
    stop = min(context->maxBlockSize, context->allocatedLength - activeTrack->CurrentRecordingIndex);
    DBG(String("CurrentRecordingIndex at entry handle record")+String(activeTrack->CurrentRecordingIndex));
    int samplesToRead = stop;
    int fadeIn = 0;
    int fadeOut = 0;
    int adjustedFadeTime = min(context->fadeTime, context->maxBlockSize);
#if DEBUG_LOG
    start0 = Time::getHighResolutionTicks();
#endif
    if (activeTrack->FirstRecordingBuffer) {
        if(*activeTrack->LoopDuration == 0){
            activeTrack->BeginFadeOffset = start = (start >= context->maxBlockSize) ? 0 : max(start, 0);
        }
        // in overdub mode create a new layer
        if ((activeTrack->getRecordMode() < 4)
            && ((activeTrack->CurrentRecordingIndex + context->maxBlockSize < *activeTrack->LoopDuration)
                || (*activeTrack->LoopDuration == 0))){
            for (auto i = 0; i <= *activeTrack->CurrentTop && i < (int)activeTrack->Layers->size(); ++i) {
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
            if (*activeTrack->CurrentTop >= 0 && *activeTrack->CurrentTop < (int)activeTrack->Layers->size() - 1) {
                (*activeTrack->Layers)[*activeTrack->CurrentTop]->dirty = true;
                activeTrack->setActivePlaybackLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
                activeTrack->setActiveRecordingLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
            }else if((*activeTrack->Layers).size() == 0 || activeTrack->getActiveRecordingLayer()->dirty){
                auto l = activeTrack->AddLayer(true);
                if (l == nullptr) return; // layerQueue empty, skip this block
                activeTrack->setActivePlaybackLayer(l);
                activeTrack->setActiveRecordingLayer(l);
                ////context->logMessage("1. adding layer at: " + String(activeTrack->CurrentRecordingIndex));
#if DEBUG_LOG

                end1 = Time::getHighResolutionTicks();
                //} );
                end2 = Time::getHighResolutionTicks();
                //context->logMessage("1. at loop start overdub ");
                
                //context->logMessage("playback layer: " + String(activeTrack->getActivePlaybackLayer()->index));
                //context->logMessage("recording layer: " + String(l->index));
                //context->logMessage("layers size: " + String(activeTrack->Layers->size()));
                ////context->logMessage("Duration outer block: " + String(end2 - start2));
                ////context->logMessage("Duration inner block: " + String(end1 - start1));
#endif
            }
        }else if(activeTrack->getRecordMode() >= kRecAppend){
            int ct = activeTrack->safeCurrentTop();
            if (ct >= 0) {
                activeTrack->setActivePlaybackLayer((*activeTrack->Layers)[ct]);
                activeTrack->setActiveRecordingLayer((*activeTrack->Layers)[ct]);
            }
        }
        fadeIn = adjustedFadeTime;
#if DEBUG_LOG
        //  end0 = Time::getHighResolutionTicks();
        //DBG("Duration block 1: " + String(end0 - start0));
#endif
    }
    // main scenario -> write the whole buffer without fade
    // in overdub/punch mode at the end of the loop apply fade in/out
    // and prepare writing to the next layer previously created
    int tail = 0, samplesBeforeTail = 0;
    // Scenario: writing in non extending mode, we're at the end of an existing loop and the current buffer goes beyond loop duration
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
    // main writing section
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
#if DEBUG_LOG

    end0 = Time::getHighResolutionTicks();
    //DBG("Duration block 2: " + String(end0 - start0));
#endif
    // in overdub mode record the remaining samples to the new layer if the buffer goes beyond the loop length
    if (resetCurrentIndex && tail > 0) {
#if DEBUG_LOG

        start0 = Time::getHighResolutionTicks();
        start2 = Time::getHighResolutionTicks();
#endif
        activeTrack->updateVisualizationBuffers();

#if DEBUG_LOG
        end2 = Time::getHighResolutionTicks();
#endif
            {
            
            for (auto i = 0; i <= *activeTrack->CurrentTop && i < (int)activeTrack->Layers->size(); ++i) {
#if DEBUG_LOG
                start1 = Time::getHighResolutionTicks();
#endif
                if(context->xchange->writeGainModifierQueue->read_available()
                   && context->xchange->readGainModifierQueue->write_available()){
                    auto gm = context->xchange->writeGainModifierQueue->front();
                    context->xchange->writeGainModifierQueue->pop();
                    gm->startLevel = context->feedback;
                    gm->operation = GainModifier::OperationType::All;
                    gm->buffer = (*activeTrack->Layers)[i]->Buffer;
                    context->xchange->readGainModifierQueue->push(gm);
                }
#if DEBUG_LOG
                end1 = Time::getHighResolutionTicks();
#endif
            }
            if (*activeTrack->CurrentTop >= 0 && *activeTrack->CurrentTop < (int)activeTrack->Layers->size() - 1) {
                (*activeTrack->Layers)[*activeTrack->CurrentTop]->dirty = true;
                ++(*activeTrack->CurrentTop);
                activeTrack->setActivePlaybackLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
                activeTrack->setActiveRecordingLayer((*activeTrack->Layers)[*activeTrack->CurrentTop]);
            }else if((*activeTrack->Layers).size() == 0 || activeTrack->getActiveRecordingLayer()->dirty){
#if DEBUG_LOG
                start1 = Time::getHighResolutionTicks();
#endif
                activeTrack->setActivePlaybackLayer(activeTrack->getActiveRecordingLayer());
                auto l = activeTrack->AddLayer(true);
                if (l == nullptr) return; // layerQueue empty, skip this block
                activeTrack->setActiveRecordingLayer(l);
#if DEBUG_LOG

                end1 = Time::getHighResolutionTicks();
                //context->logMessage("2. in overlap overdub ");

                //context->logMessage("playback layer: " + String(activeTrack->getActivePlaybackLayer()->index));
                //context->logMessage("recording layer: " + String(l->index));
                //context->logMessage("layers size: " + String(activeTrack->Layers->size()));
#endif
            }
        }
        activeTrack->CurrentRecordingIndex = 0;
#if DEBUG_LOG
        start2 = Time::getHighResolutionTicks();
#endif
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
#if DEBUG_LOG
            end2 = Time::getHighResolutionTicks();
            end0 = Time::getHighResolutionTicks();
#endif
        }
        indexUpdate = tail;
//        DBG(String("tail:")+String(tail));

        if (activeTrack->FirstRecordingBuffer) {
            activeTrack->FirstRecordingBuffer=false;
        }
    }
    else {
        if (activeTrack->FirstRecordingBuffer) {
            indexUpdate = start + samplesToRead;
//            DBG(String("start + samplesToRead:")+String(start + samplesToRead));

            activeTrack->FirstRecordingBuffer=false;
        }
        else if (activeTrack->LastRecordingBuffer) {
            indexUpdate = activeTrack->EndFadeOffset;
//            DBG(String("activeTrack->EndFadeOffset:")+String(activeTrack->EndFadeOffset));
            activeTrack->LastRecordingBuffer=false;
            activeTrack->updateVisualizationBuffers();

        }
        else {
            indexUpdate = samplesToRead;
//            DBG(String("samplesToRead:")+String(samplesToRead));

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
    for(auto track :tracks){
        // get audio only fro tracks currently playing
        if (((track->isActive() && track->IsPlaying()) || track->Playing)
            && *track->LoopDuration > 0
            && track->Layers->size() > 0)
        {
            // Determine stretcher state from the stretcher's actual ratios.
            // Time ratio is set externally (testTempoChange / processTempoChange).
            // Pitch ratio is updated here from the GUI-facing perTrackPitchRatio.
            bool stretchActive = false;
            double timeRatio = 1.0;
            if (track->timeStretcher && track->stretchInputBuffer) {
                track->timeStretcher->setPitchRatio(track->perTrackPitchRatio);
                timeRatio = track->timeStretcher->getTimeRatio();
                double pitchScale = track->timeStretcher->getPitchScale();
                stretchActive = (std::abs(timeRatio - 1.0) > 0.001
                                 || std::abs(pitchScale - 1.0) > 0.001);
                // On bypass→active transition, mark startup delay as fully
                // consumed so we don't discard real audio output.
                if (stretchActive && !track->stretcherWasActive) {
                    track->stretcherDelayConsumed = track->stretcherStartDelay;
                }
                track->stretcherWasActive = stretchActive;
            }
            int sourceSamples = stretchActive
                ? jmin(track->stretchInputBuffer->getNumSamples(),
                       jmax(1, (int)std::ceil((double)context->maxBlockSize / timeRatio)))
                : context->maxBlockSize;
            int samplesToRead = sourceSamples;

            auto reSync = track->realignment;
            int tail = jmin(sourceSamples, jmax(0, sourceSamples + *track->CurrentPlayingIndex - *track->LoopDuration));
            auto activeLayer = track->getActivePlaybackLayer();
            if ((!(nullptr == activeLayer) &&
                 !(nullptr == activeLayer->Buffer) ) &&
                ((!track->Muted && !mixState.aTrackIsSoloed)
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
                if(activeTrack->Index == track->Index &&
                   track->Recording &&
                   nullptr != track->getActiveRecordingLayer() &&
                   track->getActivePlaybackLayer()->index == track->getActiveRecordingLayer()->index){
                    if ( track  ->getRecordMode() <= RecordMode::kRecAppend &&
                        track->Layers->size()-1 == track->getActiveRecordingLayer()->index ){
                        --currentPlayBuffer;
                    }
                }
                // set start to 0 if out of bounds
                start = (start >= sourceSamples) ? 0 : jmax(start, 0);
                start=0;
                if(*track->CurrentPlayingIndex == 0){
                   start = activeTrack->BeginFadeOffset;
                }
                // set stop to source block size if out of bounds
                stop = jmin(jmax(stop, sourceSamples), sourceSamples);
                stop = jmin(*track->LoopDuration - *track->CurrentPlayingIndex, stop);
                int fadeIn, fadeOut;
                fadeIn = fadeOut = 0;
                int sourceIndex = index + start;
                int targetIndex = start;
                // determine number of samples to read
                samplesToRead = jmax(0,stop - start);
                
                // Clear accumulation buffer when stretching
                if (stretchActive && track->stretchInputBuffer) {
                    track->stretchInputBuffer->clear();
                }
                //                                                //context->logMessage("sourceIndex: " + String(sourceIndex));
                //                                                //context->logMessage("LoopDuration: " + String(*track->LoopDuration));
                //                                                //context->logMessage("CurrentPlayingIndex: " + String(*track->CurrentPlayingIndex));
                //                                                //context->logMessage("index: " + String(index));
                //                                                //context->logMessage("SamplesToRead: " + String(samplesToRead));
                //                                                //context->logMessage("track: " + String(track->Index));
                if (*track->LoopDuration > 0) {
                    int reverseSourceIndex=0;
                    {
                        int tmpIndex = *track->LoopDuration - 1 - index;
                        reverseSourceIndex = max(tmpIndex - stop, 0);
                    }
                    String s = String();
                    for (int l = 0; currentPlayBuffer > -1 && l < currentPlayBuffer + 1 && l < (int)track->Layers->size(); l++) {
                        auto buff = (*track->Layers)[l]->Buffer;
                        int64 start0, end0;
                        start0 = Time::getHighResolutionTicks();
                        
                        shared_ptr<AudioBuffer<float> > temp = nullptr;
                        if(context->xchange->readBufferQueue->read_available() > 0
                           && (!stretchActive || sourceSamples <= context->maxBlockSize)){
                            temp = context->xchange->readBufferQueue->front();
                            context->xchange->readBufferQueue->pop();
                        }else{
                            int tempSize = stretchActive ? sourceSamples : context->maxBlockSize;
                            auto b = make_unique<AudioBuffer<float> >();
                            b->setSize(context->audioInputsCount, tempSize);
                            b->clear();
                            temp = std::move(b);
                        }
                        end0 = Time::getHighResolutionTicks();
                        for (uint32 c = 0; c < context->audioInputsCount;++c) {
                            // crossfade for realignment
                            int adjustedPosition = track->Reverse?reverseSourceIndex:sourceIndex;
                            if(reSync->isFadeInProgress() && !context->skipAlign){
                                // write fade out with signal from unadjusted position
                                 fadeOut = fadeIn = jmin(abs(reSync->getCurrentOffset()), sourceSamples - targetIndex);
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
                                                      , (*track->Layers)[l]->Buffer->getReadPointer(c, track->Reverse?reverseSourceIndex:sourceIndex)
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
                                        samplesToRead = sourceSamples;
                                    }
                                    fadeOut = min(samplesToRead, context->fadeTime);
                                    temp->addFromWithRamp(c
                                                          , targetIndex + samplesToRead - fadeOut
                                                          , (*track->Layers)[l]->Buffer->getReadPointer(c, sourceIndex + samplesToRead - fadeOut)
                                                          , fadeOut, 1.0f, 0.0f);
                                    tail = 0;
                                }
//                                //context->logMessage("targetIndex: " + String(targetIndex));
//                                //context->logMessage("sourceIndex: " + String(sourceIndex));
//                                //context->logMessage("fadeIn: " + String(fadeIn));
//                                //context->logMessage("fadeOut: " + String(fadeOut));
//                                //context->logMessage("samplesToRead: " + String(samplesToRead));
//                                //context->logMessage("temp->getNumSamples(): " + String(temp->getNumSamples()));
                            }
                            temp->addFrom(c
                                          , targetIndex + fadeIn
                                          , (*track->Layers)[l]->Buffer->getReadPointer(c, adjustedPosition + fadeIn)
                                          , jmax(samplesToRead - fadeIn - fadeOut, 0));
                        }
                        // read beginning of the loop to tail of the output buffer
                        if (tail>0) {
                            int tailOffset = stretchActive ? sourceSamples - tail : context->maxBlockSize - tail;
                            for (uint32 c = 0; c < context->audioInputsCount;++c) {
                                temp->addFrom(c
                                              , tailOffset
                                              , (*track->Layers)[l]->Buffer->getReadPointer(c, 0)
                                              , tail);
                            }
                        }
                        
                        int blockSize = stretchActive ? sourceSamples : context->maxBlockSize;
                        if (stretchActive && track->stretchInputBuffer) {
                            // Accumulate into stretch buffer (no volume yet)
                            for (uint32 c = 0; c < context->audioInputsCount;++c) {
                                if (track->Reverse) {
                                    temp->reverse(c, 0, blockSize);
                                }
                                if(temp->getReadPointer(c) != nullptr){
                                    track->stretchInputBuffer->addFrom(c, 0, temp->getReadPointer(c), samplesToRead + tail);
                                }
                            }
                        } else {
                            // Original path: smoothVolume directly to output
                            for (uint32 c = 0; c < context->audioInputsCount;++c) {
                                if (track->Reverse) {
                                    temp->reverse(c, 0, blockSize);
                                }
                                if(temp->getReadPointer(c) != nullptr){
                                    smoothVolume(track->PreviousOutputLevel, track->getOutputLevel(), samplesToRead+tail, temp.get(), context->buffer.get(), c);
                                }
                            }
                        }
                    }
                    
                    // Stretcher pass: process accumulated audio and write to output
                    if (stretchActive && track->timeStretcher && track->stretchInputBuffer) {
                        const float* inPtrs[32];
                        float* outPtrs[32];
                        int numCh = (int)context->audioInputsCount;
                        for (int c = 0; c < numCh; ++c) {
                            inPtrs[c] = track->stretchInputBuffer->getReadPointer(c);
                        }
                        
                        // Feed accumulated source audio into stretcher
                        track->timeStretcher->process(inPtrs, samplesToRead + tail, false);
                        
                        int avail = track->timeStretcher->available();
                        
                        // Handle startup delay (skip initial latency samples)
                        // Reuse stretchInputBuffer (input already consumed by process())
                        if (track->stretcherDelayConsumed < track->stretcherStartDelay && avail > 0) {
                            int toSkip = jmin(avail,
                                              (int)(track->stretcherStartDelay - track->stretcherDelayConsumed));
                            toSkip = jmin(toSkip, track->stretchInputBuffer->getNumSamples());
                            if (toSkip > 0) {
                                for (int c = 0; c < numCh; ++c)
                                    outPtrs[c] = track->stretchInputBuffer->getWritePointer(c);
                                track->timeStretcher->retrieve(outPtrs, toSkip);
                                track->stretcherDelayConsumed += (size_t)toSkip;
                                avail = track->timeStretcher->available();
                            }
                        }
                        
                        int toRetrieve = jmin(avail, context->maxBlockSize);
                        if (toRetrieve > 0) {
                            // Reuse stretchInputBuffer for output (input already consumed)
                            track->stretchInputBuffer->clear();
                            for (int c = 0; c < numCh; ++c) {
                                outPtrs[c] = track->stretchInputBuffer->getWritePointer(c);
                            }
                            int retrieved = track->timeStretcher->retrieve(outPtrs, toRetrieve);
                            
                            // Apply output volume and write to context buffer
                            for (int c = 0; c < numCh; ++c) {
                                smoothVolume(track->PreviousOutputLevel, track->getOutputLevel(),
                                            retrieved, track->stretchInputBuffer.get(),
                                            context->buffer.get(), c);
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
    if(!doublesEqual(diff, 0.0)){
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
