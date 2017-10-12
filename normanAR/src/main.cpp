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

#include "ofMain.h"
#include "ofApp.h"

int main(){
    ofSetLogLevel(OF_LOG_VERBOSE);

//    for GL ES 2
    ofGLESWindowSettings settings;
    settings.setGLESVersion(2); // Set the version of openGL ES we want to use
    ofCreateWindow(settings);
    ofSetupOpenGL(1024,768, OF_WINDOW); // setup the GL context


    // This kicks off the app
	ofRunApp( new ofApp() );
	return 0;
}


#ifdef TARGET_ANDROID
void ofAndroidApplicationInit()
{
    //application scope init
}

void ofAndroidActivityInit()
{
    //activity scope init
    main();
}
#endif
