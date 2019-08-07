//
//  Button.h
//  Sequencer
//
//  Created by Brian Eschrich on 28.12.17.
//
//

#ifndef Button_h
#define Button_h

#include "ofMain.h"


class Button{
public:
    
    Button(string name, int x, int y, int w, int h);
    
    void draw();
    void onPressed(int x, int y);
    void onReleased();
    
    ofEvent<string> buttonPressedEvent;
    ofEvent<string> buttonReleasedEvent;
    
private:
    int x,y,w,h;
    string name;
    bool isActive;
};

#endif /* Button_h */
