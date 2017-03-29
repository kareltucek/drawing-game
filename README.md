This is a simple game-like thing written as a motorics exercise. You may like to use this to exercise dexterity with your pointing device, e.g., mouse, trackball or graphic tablet.

The game draws some objects on screen and the user is supposed to interact with them by mouse. There are currently three types of objects:
- dots - for standard click operation
- line segments - for simple strokes (e.g, hatching with a stylus) (i.e., hold left button and follow the line from point A to point B)
- (poly) beziere curves - for precise contour drawing (i.e., hold left button and follow a nontrivial curve)

Graphics design is clean "modern" vector graphics (green on black), with smooth transitions and growing animations. It has no levels - its just one endless room. Its very relaxing, almost addictively.

Installation
============
Dependencies: g++, gtkmm, cairomm. 

- Clone this repository ('git clone https://github.com/kareltucek/drawing-game').
- Build the source code ('cd drawing-game' and 'make').
- Run the game from the repository root ('./thegame'). 
- Optionally, copy the executable into your path, e.g., 'sudo cp thegame /usr/local/bin/'.



