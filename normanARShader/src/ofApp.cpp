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

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Setting the orientation is important or everything will be flipped on the y axis
    // OF has the origin coordinates at top-left by default, while usually opengl has the origin in the center of the screen or bottom-left corner with y growing upwards.
    // Here we're setting vflip = false in order to use opengl coordinates where origin is in the bottom-left of the screen.
    ofSetOrientation(OF_ORIENTATION_DEFAULT, false);
    ofSetupScreenPerspective(-1,-1,60,0.00001,100);

    // Calculate screen heights/widths
    height = ofGetHeight();
    width = ofGetWidth();

    // Setup ARCore
    arcore.setup();
    initialized = false; // We have not initialized arcore

    // The next two lines are equivalent ways of loading shaders
    // shader.load("shaders/noise");
    shader.load("shaders/noise.vert", "shaders/noise.frag");

    // Load our animation files from the bin/data/animations folder
    files.push_back("animations/empty.json");
    files.push_back("animations/brumpled-frump-glops.json"); // norman
    files.push_back("animations/brumpled-shift-hops.json"); // nice
    files.push_back("animations/clumbied-brine-hunguses.json"); // nice
    files.push_back("animations/mulgy-limp-donks.json"); // nice

    // To use these other animations move the json files from the animations folder in the root
    // of the repo to the bin/data/animations
//    files.push_back("animations/brumpled-bung-hinges.json");
//    files.push_back("animations/brumpled-tap-shanks.json");
//    files.push_back("animations/clumbied-frump-squeefs.json");
//    files.push_back("animations/clumbied-prunt-glops.json");
//    files.push_back("animations/fropley-clam-glops.json");
//    files.push_back("animations/fropley-shift-clumps.json");
//    files.push_back("animations/gildered-plex-squeefs.json");
//    files.push_back("animations/gildered-ront-flops.json");
//    files.push_back("animations/gildered-shift-glops.json");
//    files.push_back("animations/lorgussy-brine-shanks.json");
//    files.push_back("animations/lorgussy-crank-clumps.json");
//    files.push_back("animations/lorgussy-frump-hunguses.json");
//    files.push_back("animations/lorgussy-ront-flops.json");
//    files.push_back("animations/marbled-limp-clumps.json");
//    files.push_back("animations/mulgy-frump-donks.json");
//    files.push_back("animations/mulgy-limp-clamps.json");
//    files.push_back("animations/mulgy-prunt-clamps.json");
//    files.push_back("animations/mulgy-ront-hops.json");
//    files.push_back("animations/mulgy-tap-donks.json");
//    files.push_back("animations/shingled-groft-glops.json");
//    files.push_back("animations/shingled-shift-clumps.json");
//    files.push_back("animations/trulmy-bung-donks.json");
//    files.push_back("animations/trulmy-bung-squeefs.json");
//    files.push_back("animations/trulmy-clam-lumps.json");
//    files.push_back("animations/trulmy-crank-hops.json");
//    files.push_back("animations/trulmy-plex-glops.json");

    // Load animation meshes
    for(auto fileStr : files) {
        ofJson json = ofLoadJson(fileStr);

        auto animation = vector< vector< vector< ofMesh > > >();
        for(auto timeline : json["compData"]) {

            vector< vector<ofMesh> > timelines;
            for (auto frame : timeline) {
                vector<ofMesh> meshes;
                for (auto l : frame) {
                    ofMesh mesh;
                    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
                    for (auto p : l) {
                        mesh.addVertex(ofVec3f(3) * ofVec3f(p["x"], p["y"], p["z"]));
                    }
                    meshes.push_back(mesh);
                }

                timelines.push_back(meshes);
            }
            animation.push_back(timelines);
        }
        animations.push_back(animation);
    }

    // Record the size of our animations array
    animationsSize = animations.size();
}

//--------------------------------------------------------------
void ofApp::update(){
    arcore.update();
    if(!initialized && arcore.isInitialized()){
        initialized = true;
        projectionMatrix = arcore.getProjectionMatrix(0.01f,100.0);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Color should be white and alpha should be 255 to draw correctly
    ofSetColor(255, 255, 255, 255);

    if(initialized) {
        arcore.draw();

        // Weird hack to get OF to do something in the background that then renders shader colors. Trying to find out why...
        ofDrawRectangle(0, 0, 0, 0);

        // A computer monitor is a 2D surface, which means our 3D scene must be projected onto the screen as a 2D image
        // We need to use a "Projection" matrix for that transformation and we set it in the next two lines
        ofSetMatrixMode(OF_MATRIX_PROJECTION); // Can retreive this matrix anytime using "ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_PROJECTION)"
        ofLoadMatrix(projectionMatrix);

        // In the next two lines we set our "ModelView" matrix.
        // This matrix helps us represent our camera space, in other words it helps us define all vertices relative to our camera
        // The location of our phone represents the AR "camera", which is why we're setting the ModelView matrix to the ViewMatrix of ARCore
        // Any calls to ofTranslate, ofRotate, or ofScale will affect the "ModelView" matrix
        ofSetMatrixMode(OF_MATRIX_MODELVIEW); // Can retrieve this matrix anytime using "ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_MODELVIEW)"
        ofLoadMatrix(arcore.getViewMatrix());

        ofSetLineWidth(5);

        // DRAWING PART 1: Draw all the animations that we've placed in space
        int index = 0;
        int frameOffset = 0; // Change the starting frame of each animation for variety

        for (auto placedAnimation : placedAnimationsArray) {
            ofPushMatrix();

            // In these next lines we transform our ModelView matrix then retrieve a modelViewProjectionMatrix to send to our shader.
            // We need to retrieve stored translation and rotation matrices to move our animations
            // to the original place in space where they were placed
            modelview = arcore.getViewMatrix();
            modelview.glTranslate(placedAnimation[0]);       // Alternative method: ofTranslate(translationOffset);
            modelview.glRotate(placedAnimation[1].x, 1,0,0); // Alternative method: ofRotateXDeg(rotateOffset.x);
            modelview.glRotate(placedAnimation[1].y, 0,1,0); // Alternative method: ofRotateYDeg(rotateOffset.y);
            modelview.glRotate(placedAnimation[1].z, 0,0,1); // Alternative method: ofRotateZDeg(rotateOffset.z);
            modelview.glTranslate(0,0,-1.5);                 // Alternative method: ofTranslate(0, 0, -1.5);
            modelViewProjectionMatrix = modelview*arcore.getProjectionMatrix(0.01f,100.0); // Alternative method: ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_PROJECTION)*ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_MODELVIEW);

            shader.begin();
            shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);

            auto & animation = animations[placedAnimationsTypeArray[index]];

            // This is really where the drawing magic happens
            // Within one single animation...
            // For each timeline/subcomponent of the 3D drawing/animation...
            for( auto timeline : animation) {
                auto frameIndex = ((30 * ofGetElapsedTimeMillis() / 1000) + frameOffset) % timeline.size();
                auto frame = timeline[frameIndex];

                // Draw the mesh representing that particular frame
                for (auto mesh : frame) {
                    mesh.draw();
                }
            }

            shader.end();

            ofPopMatrix();

            index = index + 1;
            frameOffset = frameOffset + 4;
        }

        // DRAWING PART 2: Draw all the animations we can cycle through
        ofPushMatrix();

        // In these next lines we transform our ModelView matrix then retrieve a modelViewProjectionMatrix to send to our shader.
        modelview = arcore.getViewMatrix();
        modelview.glTranslate(translationOffset);  // Alternative method: ofTranslate(translationOffset);
        modelview.glRotate(rotateOffset.x, 1,0,0); // Alternative method: ofRotateXDeg(rotateOffset.x);
        modelview.glRotate(rotateOffset.y, 0,1,0); // Alternative method: ofRotateYDeg(rotateOffset.y);
        modelview.glRotate(rotateOffset.z, 0,0,1); // Alternative method: ofRotateZDeg(rotateOffset.z);
        modelview.glTranslate(0,0,-1.5);           // Alternative method: ofTranslate(0, 0, -1.5);
        modelViewProjectionMatrix = modelview*arcore.getProjectionMatrix(0.01f,100.0); // Alternative method: ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_PROJECTION)*ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_MODELVIEW);

        shader.begin();
        shader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);

        auto & animation = animations[current];

        // This is really where the drawing magic happens
        // Within one single animation...
        // For each timeline/subcomponent of the 3D drawing/animation...
        for( auto timeline : animation) {

            // Get a different frame of the timeline/subcomponent on each OF draw loop
            auto frameIndex = (30 * ofGetElapsedTimeMillis() / 1000) % timeline.size();
            auto frame = timeline[frameIndex];

            // Draw the mesh representing that particular frame
            for (auto mesh : frame) {
                mesh.draw(OF_MESH_FILL); // Comment this line and uncomment the next if you want to change the color of the mesh
                // mesh.draw();
            }
        }

        shader.end();

        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){
    showAnimation = true;

    // Store the current translation matrix
    // Note that we need to invert the ViewMatrix before retrieving the translation
    translationOffset = arcore.getViewMatrix().getInverse().getTranslation();

    // Store the current rotation matrix
    ofQuaternion quat = arcore.getViewMatrix().getInverse().getRotate();
    rotateOffset = quat.getEuler();

    // If pressing the top two thirds of the screen --> intent is to place an animation in space
    if (y < ((height/3.f) * 2)) {
        // Save the current animation translation and rotation matrices, and position within our files array
        vector<ofVec3f> temp;
        temp.push_back(translationOffset);
        temp.push_back(rotateOffset);

        placedAnimationsArray.push_back(temp);
        placedAnimationsTypeArray.push_back(current);
    } // If pressing bottom third --> intent is to cycle through available animation
    else {
        // If pressing bottom left of screen, cycle to previous animation
        if (x < (width/2.f) ) {
            if (current == 0) {
                current = animationsSize - 1;
            }
            else {
                current = current - 1;
            }
        } // If pressing bottom right of screen, cycle to next animation
        else {
            current = current + 1;
        }

        if (current == animationsSize) {
            current = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){

}

//--------------------------------------------------------------
void ofApp::stop(){

}

//--------------------------------------------------------------
void ofApp::resume(){

}

//--------------------------------------------------------------
void ofApp::reloadTextures(){
}

//--------------------------------------------------------------
bool ofApp::backPressed(){
    return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

};

//--------------------------------------------------------------
void ofApp::cancelPressed(){

};