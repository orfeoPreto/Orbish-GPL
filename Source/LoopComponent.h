//
//  LoopComponent.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 11/08/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef LoopComponent_hpp
#define LoopComponent_hpp

#include <stdio.h>
#include "JuceHeader.h"
#include "OpenGLComponents.h"


class LoopComponent:
public DragAndDropTarget,
public Component{

    public:
    std::atomic<float> bidon{0};

        LoopComponent();
        LoopComponent(std::atomic<float>& p, int idx);
    
        ~LoopComponent();

        static void test();
    void setActive(bool a);
    bool isActive();
    void resized() override;
    void paint(Graphics&) override;
	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails);
	void itemDragEnter(const SourceDetails& dragSourceDetails);
	void itemDragMove(const SourceDetails& dragSourceDetails);
	void itemDragExit(const SourceDetails& dragSourceDetails);
	void itemDropped(const SourceDetails& dragSourceDetails);
    bool shouldDrawDragImageWhenOver();
    int getIndex();
    std::atomic<float>& getProgress();
    std::unique_ptr<OpenGLAudioThumbnail> thumbnail{nullptr};
    void setMargin(int);
	void highlightBecomingActive();
	void unHighlightBecomingActive();

	void mouseDrag();

    private:
        std::atomic<float>& progress;
        int buttonSize = 15;
        int margin = 3;
        bool active = false;
        bool muted = false;
        int index = 0;
		bool becomingActive = false;
};
#endif

