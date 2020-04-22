
#pragma once

#ifndef SetPg_h
#define SetPg_h

#include "../JuceLibraryCode/JuceHeader.h"



class SettingsPage:public Component,
	public ChangeListener,
	public Button::Listener,
	public Slider::Listener,
	public Label::Listener {
public:
	SettingsPage();
	SettingsPage(bool loggingActive, int maxHistory, int tracksPerRow, int latency);

	
	~SettingsPage();


	void paint(Graphics&) override;
	void resized() override;
	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void labelTextChanged(Label* label) override;
	void parentSizeChanged() override;
	Component settingsCentre;
		Component right;
		Component left;
		Component top;
		Component bottom;
	TextButton activateLoggingButton;
	Label activateLoggingLabel;
	TextButton closeSettingsButton;
	Label latencyLabel;
	Slider latencySlider;
	Label maxUndoHistoryLabel{ "History" };
	Slider maxUndoHistorySlider{ "History" };
    Label tracksPerRowLabel{ "Tracks per row" };
    Slider tracksPerRowSlider{ "Tracks per row" };

	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() = default;

		/** Called when a button is clicked. */
		virtual void clicked(Button*) = 0;

		/** Called when a button is clicked. */
		virtual void sliderChanged(Slider*) = 0;
	};

	void addListener(Listener* newListener);

	/** Removes a previously-registered button listener
		@see addListener
	*/
	void removeListener(Listener* listener);

	//==============================================================================
	/** You can assign a lambda to this callback object to have it called when the button is clicked. */
	std::function<void()> onClick;
private:
	float margin = 5;
	float buttonSize = 15;
	ListenerList<SettingsPage::Listener> settingsPageListeners;

};


#endif
