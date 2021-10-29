/*
  ==============================================================================

    ImageViewPort.cpp
    Created: 8 Jun 2021 10:20:31pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ImageViewport.h"

//==============================================================================
ImageViewport::ImageViewport()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    background.setImage(ImageFileFormat::loadFrom(BinaryData::viewport_background_png, BinaryData::viewport_background_pngSize));
    background.setOpaque(false);
    background.toBack();
    background.setAlpha(0.07);
}

ImageViewport::~ImageViewport()
{
}

void ImageViewport::paint (juce::Graphics& g)
{
    Viewport::paint(g);
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

//    g.setColour (juce::Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("ImageViewPort", getLocalBounds(),
//                juce::Justification::centred, true);   // draw some placeholder textoweb

    
}

void ImageViewport::viewedComponentChanged (Component *newComponent) {}

void ImageViewport::visibleAreaChanged (const Rectangle< int > &newVisibleArea) {}

void ImageViewport::resized()
{
    Viewport::resized();
    addAndMakeVisible(background);
    auto ratio = background.getImage().getHeight() / std::max(getHeight(),1);
    background.setBounds(10,20, std::max(getWidth(),1)*0.3, std::max(getWidth(),1)*0.3);
    background.setCentrePosition(getWidth() /2, getHeight()/2);
}
