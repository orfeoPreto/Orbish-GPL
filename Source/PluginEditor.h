/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#ifndef Editor_h
#define Editor_h


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
#include "OpenGLComponents.h"
#include "ImageViewPort.h"
#include <algorithm>
#include <mutex>
#include <chrono>


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
  shouldUpdatePlayState   = 0x200,
  shouldChangeLayer      = 0x400,
  shouldUpdateHostPosition  = 0x800,
  shouldRefreshThumbnail = 0x1000,

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
    ,public OpenGLRenderer


{
public:
    OrbishAudioProcessorEditor (OrbishAudioProcessor&, AudioProcessorValueTreeState& apvts);
    ~OrbishAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void toggleRecord();
//    void setLookAndFeel (LookAndFeel* const newLookAndFeel);
    void toggleStop();
    void toggleClear();
    void toggleReverse();
    void askToRemoveTrack(int) override;
	void askToRefreshThumbnail() override;
    void removeTrack();
    void askToRemoveLoop() override;
    void removeLoop();
    void doRemoveLoop();
    void askToUpdatePlayState(int trackNumber) override;
    void updatePlayState();
    void doUpdatePlayState();
    void doRemoveTrack();
    void mouseDrag(const MouseEvent& event) override;
    void updateLoopVisualiser(std::shared_ptr<BufferForVisualisation>) override;
    void askToUpdatePlayHead(int position, bool reverse) override;
    void updatePlayHead();
    void updateHostPosition();
    void doHandleMidiMessages(const MidiBuffer& midiMessages);
    void doUpdateHostPosition();
    void changeTrack() ;
    void doChangeTrack() ;
    void createLoop();
    void doCreateLoop();
	void refreshThumbnail();
	void doRefreshThumbnail(bool);
    void askToChangeTrack(int trackNumber) override;
    void askToChangeLoop(int trackNumber, int loopNumber) override;
    void askToChangeLayer(int trackNumber, int layerNumber) override;
    void askToUpdateHostPosition(int ) override;
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
    void paintInfoSection();
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
    std::shared_ptr<OpenGLAudioThumbnail> getThumbnailInstance();
    bool getReverseState();
    void setUpObserverCallbacks();
    void askToHandleMidiMessages(const MidiBuffer& midiMessages) override;
    void doUpdatePlayHead();
    void changeLoop();
    void doChangeLoop();
    void changeLayer();
    void doChangeLayer();
    bool isTrackLayoutHorizontal();
    void logMessage(String);
    void setupGlobalButtons();
    void setupGroupingButtons();
    std::unique_ptr<OpenGLAudioThumbnail> setupThumbnail();
    void setupModeControls(ButtonControlArea*);
    void setupTransportControls(ButtonControlArea*);
    void setupNavigationControls(ButtonControlArea*);
    void setupTracks();
    void setupWitness();
    void markActiveTrackForRefresh(bool);
    void renderOpenGL() override;
    
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    std::shared_ptr<OpenGLContext> getOpenGLContext();
    void removeReference(OpenGLComponent*);
    void totalReset();
    
private:
    std::mutex rendererVectorMutex;
    std::atomic<uint> flags;
    MidiBuffer processedMidi;
    std::shared_ptr<OpenGLContext> openGLContext;
    std::atomic<float> playHeadPosition;
    std::atomic<float> hostPosition;
    HeaderArea headerArea{};
    std::unique_ptr<InfoAndControlArea> infoAndControlArea;
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
    std::shared_ptr<OpenGLAudioThumbnail> thumbnail;
    std::shared_ptr<OpenGLClickWitness> witness;
    //AudioThumbnail thumbnail;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> recModeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> snapModeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> fixedSizeAttachment;
    Component trackArea {};
    ImageViewport  tracksViewport{};
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
    std::unique_ptr<ButtonAttachment> clickAttachment;

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
    int counter = 0;
    int64 stamp=0, startStamp=0;
    int frameRate = 0;
    AudioProcessorValueTreeState& valueTreeState;
    OwnedArray<TrackComponent> tracks;
    bool tracksDirty = false;
    int activeTrackIdx = 0;
    int activeLoop = 0;
    bool dirty = false;
    int nextTrackNumber = 0;
    int nextLoopNumber = -1;
    std::atomic<int> nextLayerNumber;
    int trackToRemove = -1;
    int trackToChange = -1;
    int updatedTrackNumber = -1;
    std::vector<std::shared_ptr<OpenGLComponentReference>> references;
    std::mutex renderingTargetsLock;
    ReadWriteLock lock{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessorEditor)
};

#endif
