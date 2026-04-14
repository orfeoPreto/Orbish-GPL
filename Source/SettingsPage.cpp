#include "SettingsPage.h"

SettingsPage::SettingsPage() {}

SettingsPage::SettingsPage(bool monitoring, bool loggingActive, int maxHistory, int tracksPerRow, int latency, int numVisibleLayers, int themeId) {
	addAndMakeVisible(settingsCentre);
	activateLoggingButton.setClickingTogglesState(true);
	activateLoggingButton.addListener(this);
	activateLoggingButton.setToggleState(loggingActive, NotificationType::sendNotification);
	settingsCentre.addAndMakeVisible(activateLoggingButton);
    monitoringButton.setTooltip("When active, the channels input volume setting is applied to the monitoring signal");
    monitoringButton.setClickingTogglesState(true);
    monitoringButton.addListener(this);
    monitoringButton.setToggleState(monitoring, NotificationType::sendNotification);
    settingsCentre.addAndMakeVisible(monitoringButton);
	settingsCentre.addAndMakeVisible(closeSettingsButton);
	settingsTitle.setText("Settings", NotificationType::dontSendNotification);
	settingsTitle.setFont(juce::Font(16.0f, juce::Font::bold));
	settingsTitle.setColour(Label::textColourId, juce::Colour(0xffd4af37));
	settingsCentre.addAndMakeVisible(settingsTitle);
	addAndMakeVisible(right);
	addAndMakeVisible(left);
	addAndMakeVisible(top);
	addAndMakeVisible(bottom);
	closeSettingsButton.addListener(this);
	closeSettingsButton.setButtonText("Close");
	latencySlider.setRange(-500, 500);
	latencySlider.setValue(latency);
	latencySlider.setNumDecimalPlacesToDisplay(1);
    latencySlider.setDoubleClickReturnValue(true, 0);

	latencySlider.setTextBoxIsEditable(true);
	latencySlider.setTextValueSuffix(" ms");
	latencySlider.addListener(this);




	latencySlider.setSliderStyle(Slider::LinearHorizontal);
//	latencySlider.getLookAndFeel().setColour(Slider::trackColourId, Colours::black);
//	latencySlider.getLookAndFeel().setColour(Slider::backgroundColourId, Colours::white);
	latencySlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
	latencySlider.setPopupDisplayEnabled(true, false, this);
	settingsCentre.addAndMakeVisible(latencySlider);
	latencyLabel.setText("Latency Compensation", NotificationType::dontSendNotification);
	latencyLabel.setFont(juce::Font(11.0f));
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
    maxUndoHistorySlider.setDoubleClickReturnValue(true, 0);
	settingsCentre.addAndMakeVisible(maxUndoHistorySlider);
	maxUndoHistoryLabel.setText("Max Undo History", NotificationType::dontSendNotification);
	maxUndoHistoryLabel.setFont(juce::Font(11.0f));
	settingsCentre.addAndMakeVisible(maxUndoHistoryLabel);
    
    tracksPerRowSlider.setSliderStyle(Slider::LinearHorizontal);
    tracksPerRowSlider.setValue(tracksPerRow);
    tracksPerRowSlider.setRange(2, 8);
    tracksPerRowSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
    tracksPerRowSlider.setPopupDisplayEnabled(true, false, this);
    tracksPerRowSlider.addListener(this);
    tracksPerRowSlider.setNumDecimalPlacesToDisplay(0);
    tracksPerRowSlider.setDoubleClickReturnValue(true, 0);
    settingsCentre.addAndMakeVisible(tracksPerRowSlider);
    tracksPerRowLabel.setText("Tracks Per Row", NotificationType::dontSendNotification);
    tracksPerRowLabel.setFont(juce::Font(11.0f));
    settingsCentre.addAndMakeVisible(tracksPerRowLabel);
    
    visibleLayersSlider.setSliderStyle(Slider::LinearHorizontal);
    visibleLayersSlider.setValue(numVisibleLayers);
    visibleLayersSlider.setRange(2, 50);
    visibleLayersSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0, 0);
    visibleLayersSlider.setPopupDisplayEnabled(true, false, this);
    visibleLayersSlider.addListener(this);
    visibleLayersSlider.setNumDecimalPlacesToDisplay(0);
    visibleLayersSlider.setDoubleClickReturnValue(true, 0);
    settingsCentre.addAndMakeVisible(visibleLayersSlider);
    visibleLayersLabel.setText("Visible Layers", NotificationType::dontSendNotification);
    visibleLayersLabel.setFont(juce::Font(11.0f));
    settingsCentre.addAndMakeVisible(visibleLayersLabel);
    
	activateLoggingLabel.setText("Logging", NotificationType::dontSendNotification);
	activateLoggingLabel.setFont(juce::Font(11.0f));
	settingsCentre.addAndMakeVisible(activateLoggingLabel);
    monitoringLabel.setText("Post Mix Monitoring", NotificationType::dontSendNotification);
    monitoringLabel.setFont(juce::Font(11.0f));
    settingsCentre.addAndMakeVisible(monitoringLabel);

    themeLabel.setText("Theme", NotificationType::dontSendNotification);
    themeLabel.setFont(juce::Font(11.0f));
    settingsCentre.addAndMakeVisible(themeLabel);
    for (int i = 0; i < static_cast<int>(OrbishThemeId::COUNT); ++i)
        themeCombo.addItem(orbishThemeName(static_cast<OrbishThemeId>(i)), i + 1);
    themeCombo.setSelectedId(themeId + 1, NotificationType::dontSendNotification);
    themeCombo.addListener(this);
    settingsCentre.addAndMakeVisible(themeCombo);
}
SettingsPage::~SettingsPage() {
}

void SettingsPage::addListener(Listener* l) { settingsPageListeners.add(l); }
void SettingsPage::removeListener(Listener* l) { settingsPageListeners.remove(l); }

void SettingsPage::paint(Graphics& g){
	auto themeIdx = static_cast<OrbishThemeId>(themeCombo.getSelectedId() - 1);
	auto theme = orbishThemeColours(themeIdx);

	g.setColour(theme.settingsOverlay);
	g.fillRect(right.getBounds());
	g.fillRect(left.getBounds());
	g.fillRect(top.getBounds());
	g.fillRect(bottom.getBounds());

	auto bg = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
	auto accent = getLookAndFeel().findColour(juce::TextButton::ColourIds::buttonOnColourId);
	auto panelBounds = settingsCentre.getBounds().toFloat();

	// Main panel with rounded corners
	g.setColour(bg);
	g.fillRoundedRectangle(panelBounds, 12.0f);
	g.setColour(accent.withAlpha(0.12f));
	g.drawRoundedRectangle(panelBounds, 12.0f, 1.0f);

	// Section card backgrounds
	auto drawSection = [&](juce::Rectangle<int> area, const juce::String& title) {
		auto cardBounds = area.toFloat();
		g.setColour(bg.brighter(0.06f));
		g.fillRoundedRectangle(cardBounds, 8.0f);
		g.setColour(accent.withAlpha(0.06f));
		g.drawRoundedRectangle(cardBounds, 8.0f, 0.5f);

		// Section title
		g.setColour(accent.withAlpha(0.7f));
		g.setFont(juce::Font(10.0f, juce::Font::bold));
		g.drawText(title, cardBounds.withHeight(20.0f).translated(12.0f, 4.0f),
		           juce::Justification::centredLeft);
	};

	// Calculate section bounds relative to settingsCentre
	auto centre = settingsCentre.getBounds();
	int pad = 16;
	int sectionW = (centre.getWidth() - pad * 3) / 2;

	// Left column: Performance section
	drawSection(centre.withPosition(centre.getX() + pad, centre.getY() + 44)
	            .withSize(sectionW, 140), "PERFORMANCE");

	// Right column: Display section
	drawSection(centre.withPosition(centre.getX() + pad * 2 + sectionW, centre.getY() + 44)
	            .withSize(sectionW, 140), "DISPLAY");

	// Bottom row: Advanced section
	drawSection(centre.withPosition(centre.getX() + pad, centre.getY() + 200)
	            .withSize(centre.getWidth() - pad * 2, 80), "ADVANCED");

	settingsCentre.setAlpha(1.0f);
}
void SettingsPage::resized(){
	settingsCentre.setBounds(getWidth() * .2f, getHeight() * .2f, getWidth() * .6f, getHeight() * .6f);
	top.setBounds(0, 0, getWidth(), getHeight() * .2f);
	bottom.setBounds(0, getHeight() - getHeight() * .2f, getWidth(), getHeight() * .2f);
	left.setBounds(0, getHeight() * .2f, getWidth() * .2f, getHeight() * .6f);
	right.setBounds(getWidth() - getWidth() * .2f, getHeight() * .2f, getWidth() * .2f, getHeight() * .6f);

	int cw = settingsCentre.getWidth();
	int pad = 16;
	int sectionW = (cw - pad * 3) / 2;
	int labelW = sectionW - 24;
	int sliderW = labelW;
	int rowH = 32;

	// Title and close button
	settingsTitle.setBounds(pad + 4, 10, 120, 24);
	closeSettingsButton.setBounds(cw - pad - 60, 8, 60, 28);

	// Left column: PERFORMANCE (latency, max undo)
	int lx = pad + 12;
	int ly = 68;
	latencyLabel.setBounds(lx, ly, labelW, 14);
	latencySlider.setBounds(lx, ly + 14, sliderW, 16);
	ly += rowH + 12;
	maxUndoHistoryLabel.setBounds(lx, ly, labelW, 14);
	maxUndoHistorySlider.setBounds(lx, ly + 14, sliderW, 16);

	// Right column: DISPLAY (tracks per row, visible layers, theme)
	int rx = pad * 2 + sectionW + 12;
	int ry = 68;
	tracksPerRowLabel.setBounds(rx, ry, labelW, 14);
	tracksPerRowSlider.setBounds(rx, ry + 14, sliderW, 16);
	ry += rowH + 12;
	visibleLayersLabel.setBounds(rx, ry, labelW, 14);
	visibleLayersSlider.setBounds(rx, ry + 14, sliderW, 16);

	// Theme in right column below visible layers
	ry += rowH + 12;

	// Bottom row: ADVANCED (logging, post mix, theme)
	int bx = pad + 12;
	int by = 224;
	int thirdW = (cw - pad * 2 - 24) / 3;

	activateLoggingLabel.setBounds(bx, by, thirdW, 14);
	activateLoggingButton.setBounds(bx, by + 16, juce::jmin(100, thirdW), 28);

	monitoringLabel.setBounds(bx + thirdW + 12, by, thirdW, 14);
	monitoringButton.setBounds(bx + thirdW + 12, by + 16, juce::jmin(100, thirdW), 28);

	themeLabel.setBounds(bx + (thirdW + 12) * 2, by, thirdW, 14);
	themeCombo.setBounds(bx + (thirdW + 12) * 2, by + 16, juce::jmin(150, thirdW), 24);
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
void SettingsPage::comboBoxChanged(ComboBox* comboBox) {
	for (auto i = 0; i < settingsPageListeners.size(); ++i) {
		settingsPageListeners.call([this, comboBox](Listener& l) { l.comboChanged(comboBox); });
	}
}
void SettingsPage::changeListenerCallback(ChangeBroadcaster* source){
}
void SettingsPage::labelTextChanged(Label* label){
}
void SettingsPage::parentSizeChanged(){
}

