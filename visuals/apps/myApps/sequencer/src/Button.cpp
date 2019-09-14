//
//  Button.cpp
//  Sequencer
//
//  Created by Brian Eschrich on 28.12.17.
//
//

#include "Button.h"

Button::Button(string name_, int x_, int y_, int w_, int h_){
    name = name_;
    x = x_;
    y = y_;
    w = w_;
    h = h_;
    isActive = false;
}

void Button::draw(){
    isActive ? ofSetColor(180) : ofSetColor(100);
    ofDrawRectangle(x, y, w, h);
    ofSetColor(255);
    ofDrawBitmapString(name, x+5,y+10);
}

void Button::onPressed(int x_, int y_){
    if (x_ > x && x_ < x+w && y_>y && y_<y+h){
        isActive = true;
        ofNotifyEvent(buttonPressedEvent,name);
    }
}
void Button::onReleased(){
    isActive = false;
    ofNotifyEvent(buttonReleasedEvent,name);
}