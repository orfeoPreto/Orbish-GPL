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
#include "MainMenu.h"
#include "CommandIDs.h"
#include "SettingsPage.h"
#include "../exu/Label.hpp"

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
	public DragAndDropContainer,
	public ApplicationCommandTarget,
	public MenuManager

{
public:
    OrbishAudioProcessorEditor (OrbishAudioProcessor&, AudioProcessorValueTreeState& apvts);
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
    void highlightActiveTrack(Graphics& g);
    void paintInfoSection(Graphics&);
    void askToCreateTrack() override;
    void askToCreateLoop() override;
	String saveBufferFromLoop(int, int);
	ApplicationCommandTarget* getNextCommandTarget() override;
	void getAllCommands(Array< CommandID >& commands) override;
	void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
	bool perform(const InvocationInfo& info) override;
	void initCommandManager();
	MenuBarModel* getMenuModel();
	StringArray getMenuNames();
	void createMenu(PopupMenu&, const String& menuName) override;
	void createFileMenu(PopupMenu& menu);
	void createEditMenu(PopupMenu& menu);
	void createSettingsMenu(PopupMenu& menu);
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
	std::unique_ptr<MainMenu> mainMenu;
	std::unique_ptr<MenuBarComponent> menuBar;
	std::unique_ptr<ApplicationCommandManager> commandManager;
	std::shared_ptr<ValueTree> loopTree;
	Component headerComp;
    Component inputSliderComp;
    Component outputSliderComp;
    Component globalSliderComp;
    bool tracksLayoutHorizontal = true;
    DrawableButton* tracksLayoutButton;
    DrawableComposite* horizontalOutline;
    DrawableComposite* verticalOutline;
    FlexBox generalInfoArea {};
    Component transportInfoArea {};
    Component loopInfoArea {};
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
    Component transportButtonArea {};
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
    Grid transportButtonGrid {};
	exu::Label projectLabel{};
    TextButton recordButton { "Record" };
    std::unique_ptr<ButtonAttachment> recordAttachment;
    TextButton playButton { "Play" };
    std::unique_ptr<ButtonAttachment> playAttachment;
    TextButton stopButton { "Stop" };
    std::unique_ptr<ButtonAttachment> stopAttachment;
    TextButton clearButton { "Clear" };
    std::unique_ptr<ButtonAttachment> clearAttachment;
    TextButton muteButton { "Mute" };
    std::unique_ptr<ButtonAttachment> muteAttachment;
    TextButton soloButton { "Solo" };
    std::unique_ptr<ButtonAttachment> soloAttachment;
    TextButton monitorButton { "Monitor" };
    std::unique_ptr<ButtonAttachment> monitorAttachment;
    TextButton reverseButton { "Revrs" };
    std::unique_ptr<ButtonAttachment> reverseAttachment;
    ToggleState reverseState = Off;
    TextButton undoButton { "Undo" };
    std::unique_ptr<ButtonAttachment> undoAttachment;
    TextButton redoButton { "Redo" };
    std::unique_ptr<ButtonAttachment> redoAttachment;
    TextButton autoTriggerButton { "Trigger" };
    std::unique_ptr<ButtonAttachment> triggerAttachment;
    
    TextButton previousLoopButton { "<" };
    std::unique_ptr<ButtonAttachment> previousLoopAttachment;
    TextButton nextLoopButton { ">" };
    std::unique_ptr<ButtonAttachment> nextLoopAttachment;
    TextButton newLoopButton { "+" };
    std::unique_ptr<ButtonAttachment> newLoopAttachment;
    TextButton removeLoopButton { "-" };
    std::unique_ptr<ButtonAttachment> removeLoopAttachment;
    
    TextButton previousTrackButton { "<" };
    std::unique_ptr<ButtonAttachment> previousTrackAttachment;
    TextButton nextTrackButton { ">" };
    std::unique_ptr<ButtonAttachment> nextTrackAttachment;
    TextButton newTrackButton { "+" };
    std::unique_ptr<ButtonAttachment> newTrackAttachment;
    TextButton removeTrackButton { "-" };
    std::unique_ptr<ButtonAttachment> removeTrackAttachment;
    TextButton muteAllButton { "Mute" };
    std::unique_ptr<ButtonAttachment> muteAllAttachment;
    TextButton stopAllButton { "Stop" };
    std::unique_ptr<ButtonAttachment> stopAllAttachment;
    TextButton startAllButton { "Start" };
    std::unique_ptr<ButtonAttachment> startAllAttachment;
    TextButton pauseAllButton { "Pause" };
    std::unique_ptr<ButtonAttachment> pauseAllAttachment;
    TextButton clearAllButton { "Clear" };
	std::unique_ptr<ButtonAttachment> clearAllAttachment;
	TextButton addToGroupButton{ "Group" };
	std::unique_ptr<ButtonAttachment> addToGroupAttachment;
	TextButton removeFromGroupButton{ "UnGroup" };
	std::unique_ptr<ButtonAttachment> removeFromGroupAttachment;

	TextButton bounceButton{ "Bounce" };
	std::unique_ptr<ButtonAttachment> bounceAttachment;

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

    exu::Label activeLabel { "Active Track" };
    exu::Label globalLabel { "Global" };
    AudioProcessorValueTreeState& valueTreeState;
    OwnedArray<TrackComponent> tracks;
    bool tracksDirty = false;
    int activeTrack = 0;
    int activeLoop = 0;
    bool dirty = false;
    exu::Label timeSigLabel;
    exu::Label bpmLabel;
    exu::Label progressLabel;
    exu::Label trackNumberLabel;
    exu::Label loopNumberLabel;
    exu::Label layerNumberLabel;
	exu::Label groupNumberLabel;
	DrawablePath barWitness {};
    DrawablePath beatWitness {};
	exu::Label groupLabel;
	ComboBox groupCombo;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> groupAttachment;
    int nextTrackNumber = 0;
    int nextLoopNumber = -1;

    bool trackNumberUpdated = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessorEditor)
};

#endif
