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
	bool isInterestedInDragSource(const SourceDetails& dragSourceDetails);
	void itemDragEnter(const SourceDetails& dragSourceDetails);
	void itemDragMove(const SourceDetails& dragSourceDetails);
	void itemDragExit(const SourceDetails& dragSourceDetails);
	void itemDropped(const SourceDetails& dragSourceDetails);
    bool shouldDrawDragImageWhenOver();
    int getIndex();
    std::atomic<float>& getProgress();

    void mouseDrag();

    private:
        std::atomic<float>& progress;
        int buttonSize;
        int margin;
        bool active = false;
        bool muted = false;
        int index;
};
#endif

