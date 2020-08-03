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
    void createTracksLayoutButton();
    ~OrbishAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void toggleRecord();
    void togglePlay();
    void toggleStop();
    void toggleClear();
    void toggleMute();
    void toggleMonitor();
    void toggleReverse();
    void toggleUndo();
    void toggleMuteAll(bool mute);
    void toggleRedo();
    void toggleAutoTrigger();
    void changeInputLevel();
    void changeOutputLevel();
    void changeGlobalMix();
    void handleInputLevel();
    void handleOutputLevel();
    void handleGlobalVolume();
    void createTrack();
    void removeTrack(int) override;
    void removeLoop() override;
    void updatePlaying(int trackNumber) override;
    void doRemoveTrack();
    void mouseDrag(const MouseEvent& event) override;
    void updateInputVisualiser(const AudioBuffer<float>& buffer, int numSamples) override;
    void updateLoopVisualiser(const AudioBuffer<float>& buffer, int numSamples) override;
    void updatePlayHead(int position, bool reverse) override;
    void handleMidiMessages(const MidiBuffer& midiMessages) override;
    void changeTrack() ;
    void updateNextTrackNumber(int trackNumber) override;
    void updateNextLoopNumber(int trackNumber, int loopNumber) override;
	String saveBuffer(int trackIdx, int loopIdx, int layerIdx, File dir, String name);
	void clicked(Button*) override;
	void sliderChanged(Slider*) override;
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void changeRecordingMode();
    void changeSnapMode();
    void thumbnailChanged();
    void paintIfFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds);
    void paintIfNoFileLoaded (Graphics& g, const Rectangle<int>& thumbnailBounds);
    void timerCallback() override;
    void mouseDown(const MouseEvent &event) override;
	void makeTracks();
    void updateTrackAreaSize();
    void highlightActiveTrack(Graphics& g);
    void paintInfoSection(Graphics&);
    void askToCreateTrack() override;
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

private:
    OpenGLContext openGLContext;

    HeaderArea headerArea{};
    InfoAndControlArea infoAndControlArea{};
    TrackArea tracksArea{};

    int nbrTracksInARow = 4;
    SharedResourcePointer<TooltipWindow> tooltipWindow;
	std::shared_ptr<SettingsPage> settingsPage;
	bool loadingTracks = false;
	XmlDocument projectXml;
    void doCreateTrack(int);
    OrbishAudioProcessor& processor;
    int trackToRemove = 0;
    bool shouldCreateTrack = false;
    bool shouldCreateLoop = false;
    bool shouldRemoveLoop = false;
    Rectangle<int> toolCanvas { 10, 50, 980, 440 };
    Rectangle<int> playHead;
	std::shared_ptr<ValueTree> loopTree;
    Component inputSliderComp;
    Component outputSliderComp;
    Component globalSliderComp;
    bool tracksLayoutHorizontal = true;
    DrawableButton* tracksLayoutButton;
    DrawableComposite* horizontalOutline;
    DrawableComposite* verticalOutline;
    Component transportInfoArea {};
    Component loopInfoArea{};
    Component loopDisplayArea { };
	Array<Colour> groupColours;
	DialogWindow* dialog;
	bool showDialogWindow(String title, String message, AlertWindow::AlertIconType icon, String firstButtonText, String secondButtonText);
    AudioVisualiserComponent inputDisplay {2};
    std::shared_ptr<FFAU::LevelMeter> inputMeter;
    std::shared_ptr<FFAU::LevelMeter> outputMeter;
    std::shared_ptr<FFAU::LevelMeterLookAndFeel> lnf;
    AudioFormatManager formatManager;                    // [3]
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;
    OwnedArray<TextButton> recModeButtons;
    OwnedArray<TextButton> snapModeButtons;
    exu::Label recModeLabel= { {}, {"Record Mode"}};
    exu::Label snapModeLabel = { {}, {"Snap Mode"}};
    ComboBox recModeCombo;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> recModeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> snapModeAttachment;

    ComboBox snapModeCombo;
    Component loopConfigArea {};
    Component trackArea {};
    Viewport  tracksViewport;
    Component leftSide {};
    Component leftInnerSide {};
    Component rightInnerSide {};
    float playHeadPosition=0;
    FFAU::LevelMeterSource inputMeterSource;
    FFAU::LevelMeterSource outputMeterSource;
    bool start = true;
    bool thumbnailDirty = true;

    Component rightSide {};
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
    
    CustomButton previousLoopButton { "<", true, this };
    std::unique_ptr<ButtonAttachment> previousLoopAttachment;
    CustomButton nextLoopButton { ">", true, this };
    std::unique_ptr<ButtonAttachment> nextLoopAttachment;
    CustomButton newLoopButton { "+", true, this };
    std::unique_ptr<ButtonAttachment> newLoopAttachment;
    CustomButton removeLoopButton { "-", true, this };
    std::unique_ptr<ButtonAttachment> removeLoopAttachment;
    
    CustomButton previousTrackButton { "<", true, this };
    std::unique_ptr<ButtonAttachment> previousTrackAttachment;
    CustomButton nextTrackButton { ">", true, this };
    std::unique_ptr<ButtonAttachment> nextTrackAttachment;
    CustomButton newTrackButton { "+", true, this };
    std::unique_ptr<ButtonAttachment> newTrackAttachment;
    CustomButton removeTrackButton { "-", true, this };
    std::unique_ptr<ButtonAttachment> removeTrackAttachment;

    CustomButton muteAllButton { "Mute", false, this };
    std::unique_ptr<ButtonAttachment> muteAllAttachment;
    CustomButton stopAllButton { "Stop", true, this };
    std::unique_ptr<ButtonAttachment> stopAllAttachment;
    CustomButton startAllButton { "Start", true, this };
    std::unique_ptr<ButtonAttachment> startAllAttachment;
    CustomButton pauseAllButton { "Pause", false, this };
    std::unique_ptr<ButtonAttachment> pauseAllAttachment;
    CustomButton clearAllButton { "Clear", true, this };
	std::unique_ptr<ButtonAttachment> clearAllAttachment;
    CustomButton addToGroupButton{ "Group", true, this };
	std::unique_ptr<ButtonAttachment> addToGroupAttachment;
    CustomButton removeFromGroupButton{ "UnGroup", true, this };
	std::unique_ptr<ButtonAttachment> removeFromGroupAttachment;

    exu::Label activeTrackLabel { "1" };
    exu::Label activeLoopLabel { "1" };

    Slider inputLevelSlider { "Input Level"};
    std::unique_ptr<SliderAttachment> inputLevelAttachment;
    exu::Label inputLevelLabel { "Input Level" };
    Slider outputLevelSlider { "Output Level"};
    std::unique_ptr<SliderAttachment> outputLevelAttachment;
    exu::Label outputLevelLabel { "Output Level" };
    Slider globalVolumeSlider { "Global Volume"};
    std::unique_ptr<SliderAttachment> globalMixAttachment;
    exu::Label globalVolumeLabel { "Global Volume" };
    exu::Label midiInfoLabel;
    exu::Label trackLabel { "Tracks: " };
    exu::Label loopLabel { "Loops: " };

    exu::Label globalLabel { "Global" };
    AudioProcessorValueTreeState& valueTreeState;
    OwnedArray<TrackComponent> tracks;
    bool tracksDirty = false;
    int activeTrack = 0;
    int activeLoop = 0;
    bool dirty = false;
	exu::Label groupLabel;
	ComboBox groupCombo;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> groupAttachment;
    int nextTrackNumber = 0;
    int nextLoopNumber = -1;

    bool trackNumberUpdated = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessorEditor)
};

#endif
