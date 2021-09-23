#include "SettingsPage.h"

SettingsPage::SettingsPage() {}

SettingsPage::SettingsPage(bool monitoring, bool loggingActive, int maxHistory, int tracksPerRow, int latency) {
	addAndMakeVisible(settingsCentre);
	activateLoggingButton.setClickingTogglesState(true);
	activateLoggingButton.addListener(this);
	activateLoggingButton.setToggleState(loggingActive, NotificationType::sendNotification);
	settingsCentre.addAndMakeVisible(activateLoggingButton);
    monitoringButton.setClickingTogglesState(true);
    monitoringButton.addListener(this);
    monitoringButton.setToggleState(monitoring, NotificationType::sendNotification);
    settingsCentre.addAndMakeVisible(monitoringButton);
	settingsCentre.addAndMakeVisible(closeSettingsButton);
	addAndMakeVisible(right);
	addAndMakeVisible(left);
	addAndMakeVisible(top);
	addAndMakeVisible(bottom);
	closeSettingsButton.addListener(this);
	closeSettingsButton.setButtonText("Close");
	latencySlider.setRange(-500, 500);
	latencySlider.setValue(latency);
	latencySlider.setNumDecimalPlacesToDisplay(1);

	latencySlider.setTextBoxIsEditable(true);
	latencySlider.setTextValueSuffix(" ms");
	latencySlider.addListener(this);




	latencySlider.setSliderStyle(Slider::LinearHorizontal);
//	latencySlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
//	latencySlider.getLookAndFeel().setColour(Slider::backgroundColourId, Colours::white);
	latencySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
	latencySlider.setPopupDisplayEnabled(true, false, this);
	settingsCentre.addAndMakeVisible(latencySlider);
	latencyLabel.setText("Latency compensation: ", NotificationType::dontSendNotification);
	settingsCentre.addAndMakeVisible(latencyLabel);

	maxUndoHistorySlider.setSliderStyle(Slider::LinearHorizontal);
	maxUndoHistorySlider.setValue(maxHistory);
    maxUndoHistorySlider.setRange(0,99);
//	maxUndoHistorySlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
//	maxUndoHistorySlider.getLookAndFeel().setColour(Slider::backgroundColourId, Colours::white);
	maxUndoHistorySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
	maxUndoHistorySlider.setPopupDisplayEnabled(true, false, this);
	maxUndoHistorySlider.addListener(this);
    maxUndoHistorySlider.setNumDecimalPlacesToDisplay(0);
	settingsCentre.addAndMakeVisible(maxUndoHistorySlider);
	maxUndoHistoryLabel.setText("Max Undo History: ", NotificationType::dontSendNotification);
	settingsCentre.addAndMakeVisible(maxUndoHistoryLabel);
    
    tracksPerRowSlider.setSliderStyle(Slider::LinearHorizontal);
    tracksPerRowSlider.setValue(tracksPerRow);
    tracksPerRowSlider.setRange(2, 8);
//    tracksPerRowSlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
//    tracksPerRowSlider.getLookAndFeel().setColour(Slider::backgroundColourId, Colours::white);
    tracksPerRowSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
    tracksPerRowSlider.setPopupDisplayEnabled(true, false, this);
    tracksPerRowSlider.addListener(this);
    tracksPerRowSlider.setNumDecimalPlacesToDisplay(0);
    settingsCentre.addAndMakeVisible(tracksPerRowSlider);
    tracksPerRowLabel.setText("Number of tracks per row: ", NotificationType::dontSendNotification);
    settingsCentre.addAndMakeVisible(tracksPerRowLabel);
    
	activateLoggingLabel.setText("Activate Logging: ", NotificationType::dontSendNotification);
	settingsCentre.addAndMakeVisible(activateLoggingLabel);
    monitoringLabel.setText("Monitoring Pre/Post Mix: ", NotificationType::dontSendNotification);
    settingsCentre.addAndMakeVisible(monitoringLabel);
}
SettingsPage::~SettingsPage() {
}

void SettingsPage::addListener(Listener* l) { settingsPageListeners.add(l); }
void SettingsPage::removeListener(Listener* l) { settingsPageListeners.remove(l); }

void SettingsPage::paint(Graphics& g){
	RectangleList<float> rects;

	g.setColour(Colour(0xA0ffffff));
	g.fillRect(right.getBounds());
	g.fillRect(left.getBounds());
	g.fillRect(top.getBounds());
	g.fillRect(bottom.getBounds());
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(settingsCentre.getBounds());
	settingsCentre.setAlpha(1.0f);
}
void SettingsPage::resized(){
	settingsCentre.setBounds(getWidth() * .2f, getHeight() * .2f, getWidth() * .6f, getHeight() * .6f);
	closeSettingsButton.setBounds(settingsCentre.getWidth() - margin - buttonSize*2, margin, buttonSize*2, buttonSize);
	top.setBounds(0, 0, getWidth(), getHeight() * .2f);
    bottom.setBounds(0, getHeight() - getHeight() * .2f, getWidth(), getHeight() * .2f);
    left.setBounds(0, getHeight() * .2f, getWidth() * .2f, getHeight() * .6f);
    right.setBounds(getWidth() - getWidth() * .2f, getHeight() * .2f, getWidth() * .2f, getHeight() * .6f);
    latencyLabel.setBounds(20, 30, 150, 15);
    latencySlider.setBounds(200, 35, 200, 15);
    maxUndoHistoryLabel.setBounds(20, 65, 150, 15);
    maxUndoHistorySlider.setBounds(200, 70, 200, 15);
    tracksPerRowLabel.setBounds(20, 105, 150, 15);
    tracksPerRowSlider.setBounds(200,105,200,15);
    activateLoggingLabel.setBounds(20, 130, 150, 15);
    activateLoggingButton.setBounds(200,135,100,30);
    monitoringLabel.setBounds(20, 165, 150, 15);
    monitoringButton.setBounds(200,170,100,30);
    
}
void SettingsPage::buttonClicked(Button* button){
	for (auto i = 0;i < settingsPageListeners.size();++i) {
		settingsPageListeners.call([this, button](Listener& l) { l.clicked(button); });	
	}
}
void SettingsPage::sliderValueChanged(Slider* slider) {
	for (auto i = 0;i < settingsPageListeners.size();++i) {
		settingsPageListeners.call([this, slider](Listener& l) { l.sliderChanged(slider); });
	}
}
void SettingsPage::changeListenerCallback(ChangeBroadcaster* source){
}
void SettingsPage::labelTextChanged(Label* label){
}
void SettingsPage::parentSizeChanged(){
}
