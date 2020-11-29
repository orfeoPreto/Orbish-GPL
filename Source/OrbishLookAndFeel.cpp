/*
  ==============================================================================

    OrbishLookAndFeel.cpp
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "OrbishLookAndFeel.h"


//    static Colour createBaseColour (Colour buttonColour,
//                                    bool hasKeyboardFocus,
//                                    bool shouldDrawButtonAsHighlighted,
//                                    bool shouldDrawButtonAsDown) noexcept
//    {
//        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
//        const Colour baseColour (buttonColour.withMultipliedSaturation (sat));
//
//        if (shouldDrawButtonAsDown)        return baseColour.contrasting (0.2f);
//        if (shouldDrawButtonAsHighlighted) return baseColour.contrasting (0.1f);
//
//        return baseColour;
//    }
//
//    static TextLayout OrbishLookAndFeel::layoutTooltipText (const String& text, Colour colour) noexcept
//    {
//        const float tooltipFontSize = 13.0f;
//        const int maxToolTipWidth = 400;
//
//        AttributedString s;
//        s.setJustification (Justification::topLeft);
//        s.append (text, Font (tooltipFontSize, Font::bold), colour);
//
//        TextLayout tl;
//        tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
//        return tl;
//    }
//}

OrbishLookAndFeel::OrbishLookAndFeel() {
    // colours
    auto deepDarkGrey = juce::Colour(0xff161616);
    auto darkGrey = juce::Colour(0xff262626);
    auto lightGrey = juce::Colour(0xff707070);
    auto yellow = juce::Colour(0xfffed70f);
    auto sliderLowerHalf = juce::Colour(0xff333333);
    
    // background
    setColour(juce::ResizableWindow::backgroundColourId, darkGrey);

    // text
    setColour(juce::Label::textColourId, lightGrey);
    setColour(juce::Slider::backgroundColourId, lightGrey);

    setDefaultSansSerifTypefaceName("Bahnschrift");

    // buttons
    setColour(juce::TextButton::ColourIds::textColourOffId, lightGrey);
    setColour(juce::TextButton::ColourIds::textColourOnId, yellow);
    setColour(juce::TextButton::ColourIds::buttonColourId, darkGrey);
    setColour(juce::TextButton::ColourIds::buttonOnColourId, yellow);

    setColour(DrawableButton::ColourIds::backgroundColourId, darkGrey);
    setColour(DrawableButton::ColourIds::backgroundOnColourId, darkGrey);

    // tracks
    setColour(TrackComponent::ColourIds::backgroundColourId, darkGrey);
    setColour(TrackComponent::ColourIds::activeBackgroundColourId, lightGrey);
    setColour(TrackComponent::ColourIds::outlineColourId, yellow);

    // level meters
    setupDefaultMeterColours();
    setColour(FFAU::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    setColour(FFAU::LevelMeter::lmBackgroundColour, darkGrey);
    setColour(FFAU::LevelMeter::lmMeterBackgroundColour, lightGrey);

    // combo boxes
    setColour(juce::ComboBox::backgroundColourId, darkGrey);
    setColour(juce::ComboBox::arrowColourId, lightGrey);
    setColour(juce::ComboBox::textColourId, lightGrey);
    setColour(juce::ComboBox::focusedOutlineColourId, yellow);
    setColour(juce::ComboBox::buttonColourId, darkGrey);
    setColour(juce::ComboBox::outlineColourId, deepDarkGrey);

    // menu bar
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, darkGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::textColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, yellow);

    // sliders
    setColour(juce::Slider::backgroundColourId, lightGrey);
    setColour(juce::Slider::trackColourId, sliderLowerHalf);
    setColour(juce::Slider::thumbColourId, yellow);
    setColour(juce::Slider::textBoxOutlineColourId, darkGrey);

    
    // progress bars
    setColour(juce::ProgressBar::ColourIds::backgroundColourId, juce::Colour(0xff42403a));
    setColour(juce::ProgressBar::ColourIds::foregroundColourId, yellow);

    // scroll bars
    setColour(juce::ScrollBar::ColourIds::trackColourId, yellow);
    setColour(juce::ScrollBar::ColourIds::thumbColourId, darkGrey);
    setColour(juce::ScrollBar::ColourIds::backgroundColourId, lightGrey);
}

OrbishLookAndFeel::~OrbishLookAndFeel() {

}



void OrbishLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown) {
    g.setColour(juce::Colour(0xff262626));
    g.fillRect(button.getLocalBounds());

    auto buttonShape = isSquareButton(&button) ? ButtonShape::SQUARE : ButtonShape::RECTANGULAR;

    if (isPushButton(&button)){
        if (isHovering) {
            button.setColour(TextButton::textColourOffId, Colours::whitesmoke);
            button.setColour(TextButton::textColourOnId, Colours::whitesmoke);
        }
        else {
            button.setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
            button.setColour(TextButton::textColourOnId, findColour(TextButton::textColourOffId));
        }
        drawPushButton(g, button, backgroundColour, isHovering, isButtonDown, buttonShape);
    }
    else{
        if (isHovering) {
            button.setColour(TextButton::textColourOffId, Colours::whitesmoke);
            button.setColour(TextButton::textColourOnId, Colours::whitesmoke);
        }
        else {
            button.setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
            button.setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));
        }
        drawToggleButton(g, button, backgroundColour, isHovering, isButtonDown, buttonShape);
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
        // spinning bar..
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

void OrbishLookAndFeel::drawPushButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape){
    Image image;
    if (isButtonDown) {
        image = getImageForButton(shape, ButtonState::CLICKED);
    }
    else if (isHovering) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else {
        image = getImageForButton(shape, ButtonState::BASE);
    }
    auto bounds = button.getLocalBounds();
    g.drawImageWithin(image, 0, 0, bounds.getWidth(), bounds.getHeight(), RectanglePlacement(), false);
}

void OrbishLookAndFeel::drawToggleButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape) {
    Image image;
    if ((button.getToggleState() && isButtonDown)) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else if (isButtonDown || (button.getToggleState() && isHovering)) {
        image = getImageForButton(shape, ButtonState::CLICKED);
    }
    else if (isHovering) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else if (button.getToggleState()) {
        image = getImageForButton(shape, ButtonState::ACTIVE);
    }
    else {
        image = getImageForButton(shape, ButtonState::BASE);
    }
    auto bounds = button.getLocalBounds();
    g.drawImageWithin(image, 0, 0, bounds.getWidth(), bounds.getHeight(), RectanglePlacement(), false);
}

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

Image OrbishLookAndFeel::getImageForButton(ButtonShape shape, ButtonState state){
    if (shape == ButtonShape::SQUARE){
        switch (state) {
            case ButtonState::BASE: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbase_png, BinaryData::squarebuttonbase_pngSize);
            case ButtonState::HOVERING: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbasehover_png, BinaryData::squarebuttonbasehover_pngSize);
            case ButtonState::CLICKED: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbaseclicked_png, BinaryData::squarebuttonbaseclicked_pngSize);
            case ButtonState::ACTIVE: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbaseactive_png, BinaryData::squarebuttonbaseactive_pngSize);
        }
    }
    else{
        switch (state) {
        case ButtonState::BASE: return ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
        case ButtonState::HOVERING: return ImageFileFormat::loadFrom(BinaryData::buttonbasehover_png, BinaryData::buttonbasehover_pngSize);
        case ButtonState::CLICKED: return ImageFileFormat::loadFrom(BinaryData::buttonbaseclicked_png, BinaryData::buttonbaseclicked_pngSize);
        case ButtonState::ACTIVE: return ImageFileFormat::loadFrom(BinaryData::buttonbaseactive_png, BinaryData::buttonbaseactive_pngSize);
        }
    }
    return ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
}

int OrbishLookAndFeel::getSliderPopupPlacement (Slider&)
{
    return BubbleComponent::below;
}

void OrbishLookAndFeel::drawLinearSliderThumb (Graphics & g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider & slider) {
    Image image;
    int w, h;
    float ratio;
    if(style == Slider::LinearBarVertical ){
        image = ImageFileFormat::loadFrom(BinaryData::sliderhandle_png, BinaryData::sliderhandle_pngSize);
        ratio = image.getWidth() / width;
        image = image.rescaled(width, image.getHeight()/ratio);
        y = sliderPos - image.getHeight() * .5f;
        w = width;
        h = image.getHeight();
    }else if (style == Slider::LinearHorizontal){
        image = ImageFileFormat::loadFrom(BinaryData::sliderhandle90_png, BinaryData::sliderhandle90_pngSize);
        ratio = image.getHeight() / height;
        image = image.rescaled(image.getWidth()/ratio, height);
        x = sliderPos - image.getWidth() * .5f;
        w = image.getWidth();
        h = height;
    }else{
        w = width;
        h = height;
    }


    g.drawImageWithin(image
                      , x
                      , y
                      , w, h, RectanglePlacement(), false);
}

//==============================================================================
void OrbishLookAndFeel::drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height,
                                                 float sliderPos,
                                                 float minSliderPos,
                                                 float maxSliderPos,
                                                 const Slider::SliderStyle style, Slider& slider)
{
    auto sliderRadius = (float) (getSliderThumbRadius (slider) - 2);
 //   auto gradCol1 = trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f));
 //   auto gradCol2 = trackColour.overlaidWith (Colour (0x14000000));

    Path emptyBarPart;
    Path fullBarPart;
    
    const float ix = (float) x + (float) width * 0.5f - sliderRadius * 0.5f;
    const float iw = sliderRadius;
    const float iy = (float) x + (float) height * 0.5f - sliderRadius * 0.5f;
    const float ih = sliderRadius;

 //       g.setGradientFill (ColourGradient::horizontal (gradCol1, ix, gradCol2, ix + iw));
    if(style == Slider::LinearBarVertical ){
        emptyBarPart.addRoundedRectangle (ix
                                        , (float) y - sliderRadius * 0.5f
                                        ,iw
                                        , (float) sliderPos
                                        , 5.0f);
        fullBarPart.addRoundedRectangle (ix
                                         , sliderPos
                                         , iw
                                         , (float) height - sliderPos
                                         , 5.0f);
    }else if (style == Slider::LinearHorizontal){
        emptyBarPart.addRoundedRectangle ((float) x - sliderRadius * 0.5f
                                          , iy
                                          , (float) sliderPos
                                          , ih
                                          , 5.0f);
        fullBarPart.addRoundedRectangle (sliderPos
                                         , iy
                                         ,(float) width - sliderPos
                                         , ih
                                         , 5.0f);
    }

    g.setColour(slider.findColour(Slider::backgroundColourId));
    g.fillPath (emptyBarPart);
    g.setColour(slider.findColour(Slider::trackColourId));
    g.fillPath (fullBarPart);

}


void OrbishLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{

        drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb (g, x
                               , y
                               , (float) width
                               , (float) height
                               , sliderPos, minSliderPos, maxSliderPos, style, slider);
}

int OrbishLookAndFeel::getSliderThumbRadius (Slider& slider)
{
    return jmin (7,
                 slider.getHeight() / 2,
                 slider.getWidth() / 2) + 2;
}

//void OrbishLookAndFeel::drawBubble (Graphics& g, BubbleComponent& b,
//                         const Point<float>& positionOfTip,
//                                            const Rectangle<float>& body) {
//    if (!b.isCurrentlyModal()) {
//        b.enterModalState();
//    }
//}

