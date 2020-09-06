/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#ifndef Editor_h
#define Editor_h

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Observer.h"
#include "Orbish.h"
#include "TrackComponent.h"
#include "CommandIDs.h"
#include "SettingsPage.h"
#include "../exu/Label.hpp"
#include "OrbishLookAndFeel.h"
#include "CustomButton.h"
#include "HeaderArea.h"
#include "InfoAndControlArea.h"
#include "TrackArea.h"

//==============================================================================
/**
*/
struct Project {
	String name = "Untitled";
	bool newProject = true;
	bool dirty = false;
	File directory;
};
enum ToggleState
{
    On,
    Off
};

enum CallBackFlags {
  shouldUpdateThumbnail    = 0x01,
  shouldUpdatePlayHead     = 0x02,
  shouldHandleMidi   = 0x04,
  shouldCreateTrack  = 0x08,
  shouldChangeTrack    = 0x10,
  shouldRemoveTrack      = 0x20,
  shouldCreateLoop      = 0x40,
  shouldChangeLoop      = 0x80,
  shouldRemoveLoop      = 0x100,
  shouldUpdatePlayState   = 0x200
};


typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


class OrbishAudioProcessorEditor : public AudioProcessorEditor,
	public Button::Listener,
	public Slider::Listener,
	public virtual Observer,
	public ChangeListener,
	public SettingsPage::Listener,
	private Timer,
	public DragAndDropContainer

{
public:
    OrbishAudioProcessorEditor (OrbishAudioProcessor&, AudioProcessorValueTreeState& apvts);
    ~OrbishAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void toggleRecord();
    void toggleStop();
    void toggleClear();
    void toggleReverse();
    void askToRemoveTrack(int) override;
    void removeTrack();
    void askToRemoveLoop() override;
    void removeLoop();
    void doRemoveLoop();
    void askToUpdatePlayState(int trackNumber) override;
    void updatePlayState();
    void doUpdatePlayState();
    void doRemoveTrack();
    void mouseDrag(const MouseEvent& event) override;
    void updateLoopVisualiser(const AudioBuffer<float>& buffer, int numSamples) override;
    void askToUpdatePlayHead(int position, bool reverse) override;
    void updatePlayHead();
    void doHandleMidiMessages(const MidiBuffer& midiMessages);
    void changeTrack() ;
    void doChangeTrack() ;
    void createLoop();
    void doCreateLoop();
    void askToChangeTrack(int trackNumber) override;
    void askToChangeLoop(int trackNumber, int loopNumber) override;
	String saveBuffer(int trackIdx, int loopIdx, int layerIdx, File dir, String name, bool overwrite);
	void clicked(Button*) override;
	void sliderChanged(Slider*) override;
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void timerCallback() override;
    void mouseDown(const MouseEvent &event) override;
	void makeTracks();
    void highlightActiveTrack(Graphics& g);
    void paintInfoSection(Graphics&);
    void askToCreateTrack() override;
    void createTrack();
    void askToCreateLoop() override;
    void updateTrackBounds();
    String saveBufferFromLoop(int, int);
	void createNewProject();
	void saveProject();
	void askUserToOpenFile();
	bool openFile(const File& file);
	Project project;
	void setProjectName(String name);
	void showSettingsPage();
	void closeSettingsPage();
	int getTrackRowHeight(int);
    void setTracksDirty();
    void toggleLayout();
    OrbishAudioProcessor* getProcessor();
    AudioThumbnail* getThumbnailInstance();
    bool getReverseState();
    void setUpObserverCallbacks();
    void askToHandleMidiMessages(const MidiBuffer& midiMessages) override;
    void doUpdatePlayHead();
    void changeLoop();
    void doChangeLoop();
    bool isTrackLayoutHorizontal();
    void logMessage(String);
    
private:
    std::atomic<uint> flags;
    MidiBuffer processedMidi;
    OpenGLContext openGLContext;
    int playHeadPosition=0;
    HeaderArea headerArea{};
    InfoAndControlArea infoAndControlArea{};
    TrackArea tracksArea{};
    bool destructiveSave = true;
    int nbrTracksInARow = 4;
    SharedResourcePointer<TooltipWindow> tooltipWindow;
	std::shared_ptr<SettingsPage> settingsPage;
	bool loadingTracks = false;
	XmlDocument projectXml;
    void doCreateTrack(int);
    OrbishAudioProcessor& processor;
	std::shared_ptr<ValueTree> loopTree;
    bool tracksLayoutHorizontal = true;
	DialogWindow* dialog;
	bool showDialogWindow(String title, String message, AlertWindow::AlertIconType icon, String firstButtonText, String secondButtonText);
    AudioFormatManager formatManager;                    // [3]
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> recModeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> snapModeAttachment;
    Component trackArea {};
    Viewport  tracksViewport{};
    FFAU::LevelMeterSource outputMeterSource;
    bool start = true;
    bool thumbnailDirty = true;

    std::unique_ptr<ButtonAttachment> recordAttachment;
    std::unique_ptr<ButtonAttachment> playAttachment;
    std::unique_ptr<ButtonAttachment> stopAttachment;
    std::unique_ptr<ButtonAttachment> clearAttachment;
    std::unique_ptr<ButtonAttachment> muteAttachment;
    std::unique_ptr<ButtonAttachment> soloAttachment;
    std::unique_ptr<ButtonAttachment> monitorAttachment;
    std::unique_ptr<ButtonAttachment> reverseAttachment;
    ToggleState reverseState = Off;
    std::unique_ptr<ButtonAttachment> undoAttachment;
    std::unique_ptr<ButtonAttachment> redoAttachment;
    std::unique_ptr<ButtonAttachment> bounceAttachment;
    std::unique_ptr<ButtonAttachment> triggerAttachment;
    
    std::unique_ptr<ButtonAttachment> previousLoopAttachment;
    std::unique_ptr<ButtonAttachment> nextLoopAttachment;
    std::unique_ptr<ButtonAttachment> newLoopAttachment;
    std::unique_ptr<ButtonAttachment> removeLoopAttachment;
    
    std::unique_ptr<ButtonAttachment> previousTrackAttachment;   
    std::unique_ptr<ButtonAttachment> nextTrackAttachment;
    std::unique_ptr<ButtonAttachment> newTrackAttachment;
    std::unique_ptr<ButtonAttachment> removeTrackAttachment;

    std::unique_ptr<ButtonAttachment> muteAllAttachment;
    std::unique_ptr<ButtonAttachment> stopAllAttachment;
    std::unique_ptr<ButtonAttachment> startAllAttachment;
    std::unique_ptr<ButtonAttachment> pauseAllAttachment;
	std::unique_ptr<ButtonAttachment> clearAllAttachment;

    Array<Colour> groupColours;
	std::unique_ptr<ButtonAttachment> addToGroupAttachment;
	std::unique_ptr<ButtonAttachment> removeFromGroupAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> groupAttachment;

    std::unique_ptr<SliderAttachment> inputLevelAttachment;
    std::unique_ptr<SliderAttachment> outputLevelAttachment;
    std::unique_ptr<SliderAttachment> globalMixAttachment;
    exu::Label midiInfoLabel;

    AudioProcessorValueTreeState& valueTreeState;
    OwnedArray<TrackComponent> tracks;
    bool tracksDirty = false;
    int activeTrack = 0;
    int activeLoop = 0;
    bool dirty = false;
	
    int nextTrackNumber = 0;
    int nextLoopNumber = -1;
    int trackToRemove = -1;
    int trackToChange = -1;
    int updatedTrackNumber = -1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessorEditor)
};

#endif
