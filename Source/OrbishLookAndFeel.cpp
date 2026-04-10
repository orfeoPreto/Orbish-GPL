/*
  ==============================================================================

    OrbishLookAndFeel.cpp
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "OrbishLookAndFeel.h"

OrbishLookAndFeel::OrbishLookAndFeel() {
    setDefaultSansSerifTypefaceName("Bahnschrift");
    applyTheme(OrbishThemeId::ObsidianGold);
}

void OrbishLookAndFeel::applyTheme(OrbishThemeId themeId) {
    auto t = orbishThemeColours(themeId);

    auto bgOuter  = t.backgroundOuter;
    auto bgInner  = t.backgroundInner;
    auto panel    = t.panelSurface;
    auto btnSurf  = t.buttonSurface;
    auto text     = t.textPrimary;
    auto textDim  = t.textSecondary;
    auto accent   = t.accent;
    auto divider  = t.divider;
    auto track    = t.sliderTrack;

    // background - use outer as the main window bg
    setColour(juce::ResizableWindow::backgroundColourId, bgOuter);

    // text
    setColour(juce::Label::textColourId, textDim);

    // buttons - accent is used for active/on state border, not fill
    setColour(juce::TextButton::ColourIds::textColourOffId, text);
    setColour(juce::TextButton::ColourIds::textColourOnId, accent);
    setColour(juce::TextButton::ColourIds::buttonColourId, btnSurf);
    setColour(juce::TextButton::ColourIds::buttonOnColourId, accent);

    setColour(DrawableButton::ColourIds::backgroundColourId, btnSurf);
    setColour(DrawableButton::ColourIds::backgroundOnColourId, btnSurf);

    // tracks
    setColour(TrackComponent::ColourIds::backgroundColourId, bgInner);
    setColour(TrackComponent::ColourIds::activeBackgroundColourId, panel);
    setColour(TrackComponent::ColourIds::outlineColourId, accent);

    // level meters
    setupDefaultMeterColours();
    setColour(FFAU::LevelMeter::lmMeterGradientLowColour, t.accentDim);
    setColour(FFAU::LevelMeter::lmBackgroundColour, bgOuter);
    setColour(FFAU::LevelMeter::lmMeterBackgroundColour, t.meterBg);

    // combo boxes
    setColour(juce::ComboBox::backgroundColourId, btnSurf);
    setColour(juce::ComboBox::arrowColourId, textDim);
    setColour(juce::ComboBox::textColourId, text);
    setColour(juce::ComboBox::focusedOutlineColourId, accent);
    setColour(juce::ComboBox::buttonColourId, btnSurf);
    setColour(juce::ComboBox::outlineColourId, divider);

    // menu bar / popup menus
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, panel);
    setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, accent.withAlpha(0.15f));
    setColour(juce::PopupMenu::ColourIds::textColourId, text);
    setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, accent);

    // sliders
    setColour(juce::Slider::backgroundColourId, track);
    setColour(juce::Slider::trackColourId, accent.withAlpha(0.4f));
    setColour(juce::Slider::thumbColourId, accent);
    setColour(juce::Slider::textBoxOutlineColourId, btnSurf);

    // progress bars
    setColour(juce::ProgressBar::ColourIds::backgroundColourId, bgInner);
    setColour(juce::ProgressBar::ColourIds::foregroundColourId, accent);

    // scroll bars
    setColour(juce::ScrollBar::ColourIds::trackColourId, bgInner);
    setColour(juce::ScrollBar::ColourIds::thumbColourId, textDim.withAlpha(0.3f));
    setColour(juce::ScrollBar::ColourIds::backgroundColourId, bgOuter);
}

OrbishLookAndFeel::~OrbishLookAndFeel() {
}

//==============================================================================
// Button rendering - uniform, restrained
//==============================================================================

static void drawCodeButton(juce::Graphics& g, juce::Button& button,
                           juce::Colour fillColour, juce::Colour borderColour,
                           float borderWidth, float cornerRadius) {
    auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);
    g.setColour(fillColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    if (borderWidth > 0.0f) {
        g.setColour(borderColour);
        g.drawRoundedRectangle(bounds.reduced(borderWidth * 0.5f), cornerRadius, borderWidth);
    }
}

void OrbishLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                              const juce::Colour& backgroundColour,
                                              bool isHovering, bool isButtonDown) {
    auto bgOuter = findColour(juce::ResizableWindow::backgroundColourId);
    auto btnSurf = findColour(juce::TextButton::ColourIds::buttonColourId);
    auto accent  = findColour(juce::TextButton::ColourIds::buttonOnColourId);
    auto text    = findColour(juce::TextButton::ColourIds::textColourOffId);

    bool isSquare = isSquareButton(&button);
    float corner = isSquare ? 5.0f : 7.0f;

    // Clear bg behind button
    g.setColour(bgOuter);
    g.fillRect(button.getLocalBounds());

    if (isPushButton(&button)) {
        if (isButtonDown) {
            drawCodeButton(g, button, btnSurf.contrasting(0.08f), accent.withAlpha(0.5f), 0.5f, corner);
        } else if (isHovering) {
            drawCodeButton(g, button, btnSurf.contrasting(0.06f), text.withAlpha(0.12f), 0.5f, corner);
        } else {
            drawCodeButton(g, button, btnSurf, text.withAlpha(0.08f), 0.5f, corner);
        }
    } else {
        // Toggle button
        bool on = button.getToggleState();
        if (on && isButtonDown) {
            drawCodeButton(g, button, accent.withAlpha(0.12f), accent.withAlpha(0.7f), 0.5f, corner);
        } else if (isButtonDown) {
            drawCodeButton(g, button, btnSurf.contrasting(0.08f), accent.withAlpha(0.5f), 0.5f, corner);
        } else if (on && isHovering) {
            drawCodeButton(g, button, accent.withAlpha(0.10f), accent, 0.5f, corner);
        } else if (isHovering) {
            drawCodeButton(g, button, btnSurf.contrasting(0.06f), text.withAlpha(0.12f), 0.5f, corner);
        } else if (on) {
            drawCodeButton(g, button, accent.withAlpha(0.06f), accent, 0.5f, corner);
        } else {
            drawCodeButton(g, button, btnSurf, text.withAlpha(0.08f), 0.5f, corner);
        }
    }
}

void OrbishLookAndFeel::drawProgressBar(Graphics& g, ProgressBar& progressBar, int width, int height, double progress, const String& textToShow){
    auto background = progressBar.findColour(ProgressBar::backgroundColourId);
    auto foreground = progressBar.findColour(ProgressBar::foregroundColourId);

    auto barBounds = progressBar.getLocalBounds().toFloat();

    g.setColour(background);
    g.fillRoundedRectangle(barBounds, (float)progressBar.getHeight() * 0.1f);

    if (progress >= 0.0f && progress <= 1.0f)
    {
        Path p;
        p.addRoundedRectangle(barBounds, (float)progressBar.getHeight() * 0.1f);
        g.reduceClipRegion(p);

        barBounds.setWidth(barBounds.getWidth() * (float)progress);
        g.setColour(foreground);
        g.fillRoundedRectangle(barBounds, (float)progressBar.getHeight() * 0.1f);
    }
    else
    {
        g.setColour(background);

        auto stripeWidth = height * 2;
        auto position = static_cast<int> (Time::getMillisecondCounter() / 15) % stripeWidth;

        Path p;

        for (auto x = static_cast<float> (-position); x < (float)(width + stripeWidth); x += (float)stripeWidth)
            p.addQuadrilateral(x, 0.0f,
                x + (float)stripeWidth * 0.5f, 0.0f,
                x, static_cast<float> (height),
                x - (float)stripeWidth * 0.5f, static_cast<float> (height));

        Image im(Image::ARGB, width, height, true);

        {
            Graphics g2(im);
            g2.setColour(foreground);
            g2.fillRoundedRectangle(barBounds, (float)progressBar.getHeight() * 0.1f);
        }

        g.setTiledImageFill(im, 0, 0, 0.85f);
        g.fillPath(p);
    }

    if (textToShow.isNotEmpty())
    {
        g.setColour(Colour::contrasting(background, foreground));
        g.setFont((float)height * 0.6f);

        g.drawText(textToShow, 0, 0, width, height, Justification::centred, false);
    }
}

void OrbishLookAndFeel::drawPushButton(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool, ButtonShape) {}
void OrbishLookAndFeel::drawToggleButton(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool, ButtonShape) {}

bool OrbishLookAndFeel::isPushButton(juce::Button* button){
    if (auto orbishButton = dynamic_cast<CustomButton*>(button)) {
        if (orbishButton->isPushButton()) {
            return true;
        }
    }
    return false;
}

bool OrbishLookAndFeel::isSquareButton(juce::Button* button) {
    if (auto orbishButton = dynamic_cast<CustomButton*>(button)) {
        if (orbishButton->isSquareButton()) {
            return true;
        }
    }
    return false;
}

Image OrbishLookAndFeel::getImageForButton(ButtonShape, ButtonState){
    return {};
}

//==============================================================================
// Slider rendering
//==============================================================================

int OrbishLookAndFeel::getSliderPopupPlacement (Slider&)
{
    return BubbleComponent::below;
}

void OrbishLookAndFeel::drawLinearSliderThumb(Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, float minSliderPos, float maxSliderPos,
                                               const Slider::SliderStyle style, Slider& slider) {
    if (width == 0 || height == 0) return;

    auto thumbColour = slider.findColour(Slider::thumbColourId);
    const float dotRadius = 3.5f;

    if (style == Slider::LinearBarVertical) {
        float cx = (float)x + (float)width * 0.5f;
        g.setColour(thumbColour);
        g.fillEllipse(cx - dotRadius, sliderPos - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    } else if (style == Slider::LinearHorizontal) {
        float cy = (float)y + (float)height * 0.5f;
        g.setColour(thumbColour);
        g.fillEllipse(sliderPos - dotRadius, cy - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
    }
}

void OrbishLookAndFeel::drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height,
                                                 float sliderPos,
                                                 float minSliderPos,
                                                 float maxSliderPos,
                                                 const Slider::SliderStyle style, Slider& slider)
{
    auto trackBg = slider.findColour(Slider::backgroundColourId);
    auto trackFill = slider.findColour(Slider::trackColourId);
    const float trackThickness = 2.0f;

    if (style == Slider::LinearBarVertical) {
        float cx = (float)x + (float)width * 0.5f;
        float top = (float)y;
        float bottom = (float)(y + height);
        // Empty (above thumb)
        g.setColour(trackBg);
        g.fillRoundedRectangle(cx - trackThickness * 0.5f, top, trackThickness, sliderPos - top, 1.0f);
        // Filled (below thumb)
        g.setColour(trackFill);
        g.fillRoundedRectangle(cx - trackThickness * 0.5f, sliderPos, trackThickness, bottom - sliderPos, 1.0f);
    } else if (style == Slider::LinearHorizontal) {
        float cy = (float)y + (float)height * 0.5f;
        float left = (float)x;
        float right = (float)(x + width);
        // Filled (left of thumb)
        g.setColour(trackFill);
        g.fillRoundedRectangle(left, cy - trackThickness * 0.5f, sliderPos - left, trackThickness, 1.0f);
        // Empty (right of thumb)
        g.setColour(trackBg);
        g.fillRoundedRectangle(sliderPos, cy - trackThickness * 0.5f, right - sliderPos, trackThickness, 1.0f);
    }
}


void OrbishLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

int OrbishLookAndFeel::getSliderThumbRadius (Slider& slider)
{
    return 4;
}

//==============================================================================
// Popup menu overrides
//==============================================================================

int OrbishLookAndFeel::getPopupMenuColumnSeparatorWidthWithOptions(PopupMenu::Options const&)
{
	return 0;
}

int OrbishLookAndFeel::getPopupMenuColumnSeparatorWithOptions(Graphics&, Rectangle<int> const&, PopupMenu::Options const&)
{
	return 0;
}

int OrbishLookAndFeel::getPopupMenuBorderSizeWithOptions(juce::PopupMenu::Options const&)
{
	return 0;
}

void OrbishLookAndFeel::getIdealPopupMenuItemSizeWithOptions(juce::String const&, bool, int, int&, int&, juce::PopupMenu::Options const&)
{
}

void OrbishLookAndFeel::drawPopupMenuUpDownArrowWithOptions(juce::Graphics&, int, int, bool, juce::PopupMenu::Options const&)
{
}

void OrbishLookAndFeel::drawPopupMenuSectionHeaderWithOptions(juce::Graphics&, juce::Rectangle<int> const&, juce::String const&, juce::PopupMenu::Options const&)
{
}

void OrbishLookAndFeel::drawPopupMenuItemWithOptions(juce::Graphics& g, juce::Rectangle<int> const& r, bool b, juce::PopupMenu::Item const& item, juce::PopupMenu::Options const& options)
{
	const auto colour = item.colour != Colour() ? &item.colour : nullptr;
	const auto hasSubMenu = item.subMenu != nullptr
		&& (item.itemID == 0 || item.subMenu->getNumItems() > 0);

	drawPopupMenuItem(g,
		r,
		item.isSeparator,
		item.isEnabled,
		b,
		item.isTicked,
		hasSubMenu,
		item.text,
		"",
		item.image.get(),
		colour);
}

void OrbishLookAndFeel::drawMenuBarItem(Graphics& g, int width, int height,
	int itemIndex,
	const String& itemText,
	bool isMouseOverItem,
	bool isMenuOpen,
	bool isMouseOverBar,
	MenuBarComponent& menuBar) {
	LookAndFeel_V2::drawMenuBarItem(g, width, height, itemIndex, itemText, isMouseOverItem, isMenuOpen, isMouseOverBar, menuBar);
}

void OrbishLookAndFeel::drawPopupMenuBackgroundWithOptions(juce::Graphics& g, int x, int y, juce::PopupMenu::Options const& options)
{
	LookAndFeel_V2::drawPopupMenuBackgroundWithOptions(g, x, y, options);
}

//==============================================================================
// Shiny button fallback (required by LookAndFeel_V3)
//==============================================================================

void OrbishLookAndFeel::drawShinyButtonShape(Graphics& g,
    float x, float y, float w, float h, float maxCornerSize,
    const Colour& baseColour, float strokeWidth,
    bool flatOnLeft, bool flatOnRight, bool flatOnTop, bool flatOnBottom) noexcept
{
    auto bounds = Rectangle<float>(x, y, w, h).reduced(1.0f);
    float corner = jmin(maxCornerSize, bounds.getWidth() * 0.5f, bounds.getHeight() * 0.5f);
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, corner);
    if (strokeWidth > 0.0f) {
        g.setColour(baseColour.darker(0.2f));
        g.drawRoundedRectangle(bounds, corner, strokeWidth);
    }
}
