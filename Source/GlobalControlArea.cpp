/*
  ==============================================================================

    GlobalControlArea.cpp
    Created: 3 Aug 2020 2:16:39pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GlobalControlArea.h"
#include "PluginEditor.h"

//==============================================================================
GlobalControlArea::GlobalControlArea(){
    globalLabel.setText("Global", NotificationType::dontSendNotification);
    addAndMakeVisible(globalLabel);

    muteAllButton.addListener(this);
    muteAllButton.setTooltip("Mute all tracks");
    muteAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::muteicon_png, BinaryData::muteicon_pngSize));
    addAndMakeVisible(muteAllButton);

    startAllButton.setTooltip("Start playing on all tracks");
    startAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::playicon_png, BinaryData::playicon_pngSize));
    addAndMakeVisible(startAllButton);

    stopAllButton.setTooltip("Stop playing on all tracks");
    stopAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::stopicon_png, BinaryData::stopicon_pngSize));
    addAndMakeVisible(stopAllButton);

    clearAllButton.addListener(this);
    clearAllButton.setTooltip("Clear all tracks");
    clearAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::clearicon_png, BinaryData::clearicon_pngSize));
    addAndMakeVisible(clearAllButton);

    pauseAllButton.setTooltip("Pause all tracks");
    pauseAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::pauseicon_png, BinaryData::pauseicon_pngSize));
    addAndMakeVisible(pauseAllButton);

    // Tempo icons: angular chevrons - time domain shape language
    tempoDownButton.addListener(this);
    tempoDownButton.setTooltip("Decrease tempo by 10 BPM");
    {
        const int sz = 24;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        Path p;
        p.startNewSubPath(6.0f, 8.0f);
        p.lineTo(12.0f, 18.0f);
        p.lineTo(18.0f, 8.0f);
        g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::mitered, PathStrokeType::rounded));
        tempoDownButton.setIcon(img);
    }
    addAndMakeVisible(tempoDownButton);

    tempoUpButton.addListener(this);
    tempoUpButton.setTooltip("Increase tempo by 10 BPM");
    {
        const int sz = 24;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        Path p;
        p.startNewSubPath(6.0f, 16.0f);
        p.lineTo(12.0f, 6.0f);
        p.lineTo(18.0f, 16.0f);
        g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::mitered, PathStrokeType::rounded));
        tempoUpButton.setIcon(img);
    }
    addAndMakeVisible(tempoUpButton);

    bpmReadout.setJustificationType(Justification::centred);
    bpmReadout.setFont(Font(11.0f, Font::plain));
    bpmReadout.setText("120.0", NotificationType::dontSendNotification);
    bpmReadout.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(bpmReadout);

    pitchReadout.setJustificationType(Justification::centred);
    pitchReadout.setFont(Font(11.0f, Font::plain));
    pitchReadout.setText("0 st", NotificationType::dontSendNotification);
    pitchReadout.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(pitchReadout);

    // Pitch icons: curved sine waves - shape-language distinction from tempo chevrons.
    // Angular = time domain (tempo), curved = frequency domain (pitch).
    pitchDownButton.addListener(this);
    pitchDownButton.setTooltip("Shift pitch down by 1 semitone");
    {
        // Descending sine: M 3 8 Q 6 14 9 11 Q 12 8 15 14 Q 18 20 21 17
        const int sz = 24;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        Path p;
        p.startNewSubPath(3.0f, 8.0f);
        p.quadraticTo(6.0f, 14.0f, 9.0f, 11.0f);
        p.quadraticTo(12.0f, 8.0f, 15.0f, 14.0f);
        p.quadraticTo(18.0f, 20.0f, 21.0f, 17.0f);
        g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
        pitchDownButton.setIcon(img);
    }
    addAndMakeVisible(pitchDownButton);

    pitchUpButton.addListener(this);
    pitchUpButton.setTooltip("Shift pitch up by 1 semitone");
    {
        // Ascending sine: M 3 16 Q 6 10 9 13 Q 12 16 15 10 Q 18 4 21 7
        const int sz = 24;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        Path p;
        p.startNewSubPath(3.0f, 16.0f);
        p.quadraticTo(6.0f, 10.0f, 9.0f, 13.0f);
        p.quadraticTo(12.0f, 16.0f, 15.0f, 10.0f);
        p.quadraticTo(18.0f, 4.0f, 21.0f, 7.0f);
        g.strokePath(p, PathStrokeType(1.8f, PathStrokeType::curved, PathStrokeType::rounded));
        pitchUpButton.setIcon(img);
    }
    addAndMakeVisible(pitchUpButton);

    midiLearnButton.addListener(this);
    midiLearnButton.setTooltip("Left-click: enter MIDI map mode. Right-click: manage mappings.");
    midiLearnButton.setClickingTogglesState(true);
    midiLearnButton.midiLearnExcluded = true;
    midiLearnButton.addMouseListener(this, false);
    addAndMakeVisible(midiLearnButton);

    // Time signature controls
    // Load common time icon from BinaryData
    tsLabelIcon = ImageFileFormat::loadFrom(BinaryData::ctime_png, BinaryData::ctime_pngSize);
    tsLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(tsLabel);

    tsNumeratorButton.addListener(this);
    tsNumeratorButton.setTooltip("Cycle time signature numerator (1-13)");
    tsNumeratorButton.setClickingTogglesState(false);
    addAndMakeVisible(tsNumeratorButton);

    tsPrimeButton.addListener(this);
    tsPrimeButton.setTooltip("Cycle subdivision (prime series: 2-31)");
    tsPrimeButton.setClickingTogglesState(false);
    addAndMakeVisible(tsPrimeButton);

    tsDenominatorButton.addListener(this);
    tsDenominatorButton.setTooltip("Cycle time signature denominator (1, 2, 4, 8, 16)");
    tsDenominatorButton.setClickingTogglesState(false);
    addAndMakeVisible(tsDenominatorButton);

    tsReadout.setJustificationType(Justification::centred);
    tsReadout.setFont(Font(11.0f, Font::plain));
    tsReadout.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(tsReadout);
    updateTimeSigReadout();

    createTracksLayoutButton();
}

GlobalControlArea::~GlobalControlArea(){
}

void GlobalControlArea::paint (juce::Graphics& g){
}

void GlobalControlArea::resized(){
    auto bounds = getLocalBounds().reduced(10);
    globalLabel.setBounds(bounds.removeFromTop(15));
    bounds.removeFromTop(4);

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    Component trackLayoutButtonArea{};

    juce::Grid grid;
    grid.setGap(juce::Grid::Px(4));

    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(2)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(muteAllButton), juce::GridItem(startAllButton),
        juce::GridItem(stopAllButton), juce::GridItem(clearAllButton),
        juce::GridItem(pauseAllButton), juce::GridItem(trackLayoutButtonArea),
        juce::GridItem(tempoDownButton), juce::GridItem(tempoUpButton),
        juce::GridItem(pitchDownButton), juce::GridItem(pitchUpButton),
        juce::GridItem(midiLearnButton).withArea({}, juce::GridItem::Span(2))
    };

    grid.performLayout(bounds);
    
    auto trackLayoutButtonBounds = trackLayoutButtonArea.getBoundsInParent();
    tracksLayoutLeft.setBounds(trackLayoutButtonBounds.removeFromLeft(trackLayoutButtonBounds.getWidth() / 2));
    tracksLayoutRight.setBounds(trackLayoutButtonBounds);
    
}

void GlobalControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}

void GlobalControlArea::createTracksLayoutButton(){
    // Left button = horizontal layout (rows icon: 3 horizontal bars)
    {
        const int sz = 32;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        float barH = 4.0f, gap = 3.0f;
        float startY = (sz - (3 * barH + 2 * gap)) * 0.5f;
        for (int i = 0; i < 3; ++i)
            g.fillRoundedRectangle(sz * 0.2f, startY + i * (barH + gap), sz * 0.6f, barH, 1.5f);
        tracksLayoutLeft.setIcon(img);
    }
    tracksLayoutLeft.setTooltip("Horizontal layout - tracks in rows");
    tracksLayoutLeft.setToggleState(true, NotificationType::dontSendNotification);
    tracksLayoutLeft.addListener(this);
    tracksLayoutLeft.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnRight);
    addAndMakeVisible(tracksLayoutLeft);

    // Right button = vertical layout (columns icon: 3 vertical bars)
    {
        const int sz = 32;
        Image img(Image::ARGB, sz, sz, true);
        Graphics g(img);
        g.setColour(Colours::white);
        float barW = 4.0f, gap = 3.0f;
        float startX = (sz - (3 * barW + 2 * gap)) * 0.5f;
        for (int i = 0; i < 3; ++i)
            g.fillRoundedRectangle(startX + i * (barW + gap), sz * 0.2f, barW, sz * 0.6f, 1.5f);
        tracksLayoutRight.setIcon(img);
    }
    tracksLayoutRight.setTooltip("Vertical layout - tracks in columns");
    tracksLayoutRight.setToggleState(false, NotificationType::dontSendNotification);
    tracksLayoutRight.addListener(this);
    tracksLayoutRight.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnLeft);
    addAndMakeVisible(tracksLayoutRight);
}

void GlobalControlArea::buttonClicked(Button* button){
    if (editor == nullptr) return;

    // In MIDI learn mode, block all normal actions except the MIDI Learn button itself
    if (editor->isMidiLearnActive() && button != &midiLearnButton) {
        return;
    }

    if (button == &muteAllButton) {
        toggleMuteAll((bool)muteAllButton.getToggleStateValue().getValue());
    }
    if (button == &clearAllButton) {
        editor->toggleClear();
    }
    if (button == &tracksLayoutLeft) {
        if (!editor->isTrackLayoutHorizontal()){
            editor->toggleLayout();
            tracksLayoutLeft.setToggleState(true, NotificationType::dontSendNotification);
            tracksLayoutRight.setToggleState(false, NotificationType::dontSendNotification);
        }

        editor->setTracksDirty();
        editor->repaint();
        editor->resized();
    }
    if (button == &tracksLayoutRight) {
        if (editor->isTrackLayoutHorizontal()) {
            editor->toggleLayout();
            tracksLayoutLeft.setToggleState(false, NotificationType::dontSendNotification);
            tracksLayoutRight.setToggleState(true, NotificationType::dontSendNotification);
        }

        editor->setTracksDirty();
        editor->repaint();
        editor->resized();
    }
    if (button == &tempoDownButton) {
        editor->tempoDown();
    }
    if (button == &tempoUpButton) {
        editor->tempoUp();
    }
    if (button == &pitchDownButton) {
        editor->pitchDown();
    }
    if (button == &pitchUpButton) {
        editor->pitchUp();
    }
    if (button == &midiLearnButton) {
        editor->toggleMidiLearn();
    }
    if (button == &tsNumeratorButton) {
        tsNumeratorIndex = (tsNumeratorIndex + 1) % 13;
        int val = kTsNumerators[tsNumeratorIndex];
        auto ctx = editor->getProcessor()->context;
        ctx->timeSigTop = val;
        AudioPlayHead::TimeSignature sig;
        sig.numerator = val;
        sig.denominator = ctx->timeSigBottom;
        ctx->info->setTimeSignature(sig);
        updateTimeSigReadout();
    }
    if (button == &tsPrimeButton) {
        tsPrimeIndex = (tsPrimeIndex + 1) % 11;
        int val = kTsPrimes[tsPrimeIndex];
        auto ctx = editor->getProcessor()->context;
        ctx->timeSigTop = val;
        AudioPlayHead::TimeSignature sig;
        sig.numerator = val;
        sig.denominator = ctx->timeSigBottom;
        ctx->info->setTimeSignature(sig);
        updateTimeSigReadout();
    }
    if (button == &tsDenominatorButton) {
        tsDenominatorIndex = (tsDenominatorIndex + 1) % 5;
        int val = kTsDenominators[tsDenominatorIndex];
        auto ctx = editor->getProcessor()->context;
        ctx->timeSigBottom = val;
        AudioPlayHead::TimeSignature sig;
        sig.numerator = ctx->timeSigTop;
        sig.denominator = val;
        ctx->info->setTimeSignature(sig);
        updateTimeSigReadout();
    }
}

void GlobalControlArea::setMidiLearnActive(bool active) {
    midiLearnButton.setToggleState(active, NotificationType::dontSendNotification);
}

void GlobalControlArea::mouseDown(const MouseEvent& e) {
    if (e.eventComponent == &midiLearnButton && e.mods.isPopupMenu()) {
        if (editor) editor->showMidiLearnMenu();
    }
}

void GlobalControlArea::toggleMuteAll(bool) {
    muteAllButton.setState(Button::buttonNormal);
}

void GlobalControlArea::updateTimeSigReadout() {
    int num = kTsNumerators[tsNumeratorIndex];
    int den = kTsDenominators[tsDenominatorIndex];
    int prime = kTsPrimes[tsPrimeIndex];
    tsNumeratorButton.setButtonText(String(num));
    tsPrimeButton.setButtonText(String(prime));
    tsDenominatorButton.setButtonText(String(den));
    // Show the actual current numerator from context (set by either button)
    int activeNum = (editor != nullptr) ? editor->getProcessor()->context->timeSigTop : num;
    tsReadout.setText(String(activeNum) + "/" + String(den), NotificationType::dontSendNotification);
}
