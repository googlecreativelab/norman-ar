// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxARCore.h"

class ofApp : public ofxAndroidApp{
	
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();
        bool _touchDown;

        // ARCore variables
        ofxARCore arcore;
        bool initialized;
        ofMatrix4x4 projectionMatrix;
		ofVec3f translationOffset;
		ofVec3f rotateOffset;

        // Animation variables
        bool showAnimation;
		int animationsSize = 0;
		vector<string> files; // Store the names of all the animation files
        vector< vector< vector< vector< ofMesh > > > > animations; // Contains all the animation meshes

		// Animations placed in space
        vector< vector <ofVec3f>> placedAnimationsArray; // Store the translation and rotation offset of each animation we place in space
        vector< int > placedAnimationsTypeArray; // Store which animation from the files array we're placing
        int current = 0;

        ofShader shader;
		ofMatrix4x4 modelViewProjectionMatrix;
		ofMatrix4x4 modelview;

        float height;
        float width;
};
