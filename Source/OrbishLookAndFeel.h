/*
  ==============================================================================

	OrbishLookAndFeel.h
	Created: 23 Jul 2020 12:22:54pm
	Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "TrackComponent.h"
#include "CustomButton.h"
#include "OrbishTheme.h"

enum class ButtonShape {
	SQUARE,
	RECTANGULAR
};

enum class ButtonState {
	BASE,
	CLICKED,
	HOVERING,
	ACTIVE
};

class OrbishLookAndFeel : public juce::LookAndFeel_V3, public FFAU::LevelMeter::LookAndFeelMethods
{
public:
	OrbishLookAndFeel();
	~OrbishLookAndFeel();
	void applyTheme(OrbishThemeId themeId);

	void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool something, bool isButtonDown) override;
	void drawProgressBar(Graphics&, ProgressBar&, int width, int height, double progress, const String& textToShow) override;
#include "ff_meters/ff_meters_LookAndFeelMethods.h"
private:



	void drawPushButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape);
	void drawToggleButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape);

	bool isPushButton(juce::Button* button);
	bool isSquareButton(juce::Button* button);

	Image getImageForButton(ButtonShape shape, ButtonState state);

	void drawLinearSlider(Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider&) override;

	int getSliderPopupPlacement(Slider&) override;

	void drawLinearSliderBackground(Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider&) override;
	void drawLinearSliderThumb(Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider&) override;
	int getSliderThumbRadius(Slider&) override;
	int getPopupMenuColumnSeparatorWidthWithOptions(PopupMenu::Options const&);
	int getPopupMenuColumnSeparatorWithOptions(Graphics&, Rectangle<int> const&, PopupMenu::Options const&);
	int getPopupMenuBorderSizeWithOptions(class juce::PopupMenu::Options const&);
	void getIdealPopupMenuItemSizeWithOptions(class juce::String const&, bool, int, int&, int&, class juce::PopupMenu::Options const&);
	void drawPopupMenuUpDownArrowWithOptions(class juce::Graphics&, int, int, bool, class juce::PopupMenu::Options const&);
	void drawPopupMenuSectionHeaderWithOptions(class juce::Graphics&, class juce::Rectangle<int> const&, class juce::String const&, class juce::PopupMenu::Options const&);
	void drawPopupMenuItemWithOptions(class juce::Graphics&, class juce::Rectangle<int> const&, bool, struct juce::PopupMenu::Item const&, class juce::PopupMenu::Options const&);
	void drawPopupMenuBackgroundWithOptions(class juce::Graphics&, int, int, class juce::PopupMenu::Options const&);
	void drawMenuBarItem(Graphics&, int width, int height,
		int itemIndex,
		const String& itemText,
		bool isMouseOverItem,
		bool isMenuOpen,
		bool isMouseOverBar,
		MenuBarComponent&) override;
	//	Font getPopupMenuFont() override;

		//
//    void drawRotarySlider (Graphics &, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &) override;
//
//    Button *     createSliderButton (Slider &, bool isIncrement) override;
//
//    Label *     createSliderTextBox (Slider &) override;
//
//    ImageEffectFilter *     getSliderEffect (Slider &) override;
//
//    Font     getSliderPopupFont (Slider &) override;
//
//    int     getSliderPopupPlacement (Slider &) override;
//
//    Slider::SliderLayout     getSliderLayout (Slider &) override;


	void drawShinyButtonShape(Graphics&,
		float x, float y, float w, float h, float maxCornerSize,
		const Colour&, float strokeWidth,
		bool flatOnLeft, bool flatOnRight, bool flatOnTop, bool flatOnBottom) noexcept;

	//    void drawBubble (Graphics&, setNumDecimalPlacesToDisplayt Rectangle<float>& body) override;
};
