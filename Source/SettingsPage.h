
#pragma once

#ifndef SetPg_h
#define SetPg_h

#include <JuceHeader.h>
#include "OrbishSlider.h"
#include "CustomButton.h"


class SettingsPage:public Component,
	public ChangeListener,
	public Button::Listener,
	public Slider::Listener,
	public Label::Listener {
public:
	SettingsPage();
	SettingsPage(bool, bool , int , int , int );

	
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
    CustomButton activateLoggingButton{"Logging", false };
	Label activateLoggingLabel;
    CustomButton monitoringButton{"Post Mix", false };
    Label monitoringLabel;
    CustomButton closeSettingsButton{"Close", false };
	Label latencyLabel;
    OrbishSlider latencySlider;
	Label maxUndoHistoryLabel{ "History" };
    OrbishSlider maxUndoHistorySlider{ "History" };
    Label tracksPerRowLabel{ "Tracks per row" };
    OrbishSlider tracksPerRowSlider{ "Tracks per row" };
        
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
