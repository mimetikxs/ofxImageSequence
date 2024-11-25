/**
 *  ofxImageSequence.h
 *
 * Created by James George, http://www.jamesgeorge.org
 * in collaboration with Flightphase http://www.flightphase.com
 *		- Updated for 0.8.4 by James George on 12/10/2014 for Specular (http://specular.cc) (how time flies!) 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 *  ofxImageSequence is a class for easily loading a series of image files
 *  and accessing them like you would frames of a movie.
 *  
 *  This class loads only textures to the graphics card and does not store pixel data in memory. This helps with
 *  fast, random access drawing of seuqences
 *  
 *  Why would you use this instead of a movie file? A few reasons,
 *  If you want truly random frame access with no lag on large images, ofxImageSequence is a good way to go
 *  If you need a movie with alpha channel the only readily available codec is Animation (PNG) which is slow at large resolutions, so this class can help with that
 *  If you want to easily access frames based on percents this class makes that easy
 * 
 * //TODO: Extend ofBaseDraws
 * //TODO: experiment with storing pixels intead of textures and doing upload every frame
 * //TODO: integrate ofDirectory to API
 */

#pragma once

#include "ofMain.h"

template <class T>
class ImageSequenceLoader;

template<class T>
class ofxImageSequence : public ofBaseHasTexture {
public:
    ofxImageSequence();
    ~ofxImageSequence();

    //sets an extension, like png or jpg
    void setExtension(string prefix);
    void setMaxFrames(int maxFrames); //set to limit the number of frames. 0 or less means no limit
    void enableThreadedLoad(bool enable);

    /**
     *	use this method to load sequences formatted like:
     *	path/to/images/myImage8.png
     *	path/to/images/myImage9.png
     *	path/to/images/myImage10.png
     *
     *	for this sequence the parameters would be:
     *	prefix		=> "path/to/images/myImage"
     *	filetype	=> "png"
     *	startIndex	=> 8
     *	endIndex	=> 10
     */
    bool loadSequence(string prefix, string filetype, int startIndex, int endIndex);

    /**
     *	Use this function to load sequences formatted like
     *
     *	path/to/images/myImage004.jpg
     *	path/to/images/myImage005.jpg
     *	path/to/images/myImage006.jpg
     *	path/to/images/myImage007.jpg
     *
     *	for this sequence the parameters would be:
     *	prefix		=> "path/to/images/myImage"
     *	filetype	=> "jpg"
     *	startIndex	=> 4
     *	endIndex	=> 7
     *	numDigits	=> 3
     */
    bool loadSequence(string prefix, string filetype, int startIndex, int endIndex, int numDigits);
    bool loadSequence(string folder);

    void cancelLoad();
    void preloadAllFrames(); //immediately loads all frames in the sequence, memory intensive but fastest scrubbing
    void unloadSequence(); //clears out all frames and frees up memory

    void setFrameRate(float rate); //used for getting frames by time, default is 30fps

    ofTexture & getTextureForFrame(int index); //returns a frame at a given index
    ofTexture & getTextureForTime(float time); //returns a frame at a given time, used setFrameRate to set time
    ofTexture & getTextureForPercent(float percent); //returns a frame at a given time, used setFrameRate to set time

    // pixels version (mimetikxs)
    T & getPixelsForFrame(int index);
    T & getPixelsForTime(float time);
    T & getPixelsForPercent(float percent);

    //if usinsg getTextureRef() use these to change the internal state
    void setFrame(int index);
    void setFrameForTime(float time);
    void setFrameAtPercent(float percent);

    string getFilePath(int index);

    virtual ofTexture & getTexture();
    virtual const ofTexture & getTexture() const;

    virtual void setUseTexture(bool bUseTex) { /* not used */ };
    virtual bool isUsingTexture() const { return true; }

    int getFrameIndexAtPercent(float percent); //returns percent (0.0 - 1.0) for a given frame
    float getPercentAtFrameIndex(int index); //returns a frame index for a percent

    int getCurrentFrame() { return currentFrame; };
    int getTotalFrames(); //returns how many frames are in the sequence
    float getLengthInSeconds(); //returns the sequence duration based on frame rate

    float getWidth(); //returns the width/height of the sequence
    float getHeight();
    bool isLoaded(); //returns true if the sequence has been loaded
    bool isLoading(); //returns true if loading during thread
    void loadFrame(int imageIndex); //allows you to load (cache) a frame to avoid a stutter when loading. use this to "read ahead" if you want

    void setMinMagFilter(int minFilter, int magFilter);

    //Do not call directly
    //called internally from threaded loader
    void completeLoading();
    bool preloadAllFilenames(); //searches for all filenames based on load input
    float percentLoaded();

protected:
    ImageSequenceLoader<T> * threadLoader;

    vector<T> sequence;
    vector<string> filenames;
    vector<bool> loadFailed;
    int currentFrame;
    ofTexture texture;
    string extension;

    string folderToLoad;
    int curLoadFrame;
    int maxFrames;
    bool useThread;
    bool loaded;

    float width, height;
    int lastFrameLoaded;
    float frameRate;

    int minFilter;
    int magFilter;
};



// -------------------------------------------------------------
// IMPLEMENTATION
// -------------------------------------------------------------


/**
 *  ofxImageSequence.cpp
 *
 * Created by James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *		- Updated for 0.8.4 by James George on 12/10/2014 for Specular (http://specular.cc) (how time flies!) 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 *  ofxImageSequence is a class for easily loading a series of image files
 *  and accessing them like you would frames of a movie.
 *  
 *  This class loads only textures to the graphics card and does not store pixel data in memory. This helps with
 *  fast, random access drawing of seuqences
 *  
 *  Why would you use this instead of a movie file? A few reasons,
 *  If you want truly random frame access with no lag on large images, ofxImageSequence allows it
 *  If you need a movie with alpha channel the only readily available codec is Animation (PNG) which is slow at large resolutions, so this class can help with that
 *  If you want to easily access frames based on percents this class makes that easy
 *  
 */

#include "ofxImageSequence.h"

template<class T>
class ImageSequenceLoader : public ofThread {
public:
    bool loading;
    bool cancelLoading;
    ofxImageSequence<T> & sequenceRef;

    ImageSequenceLoader(ofxImageSequence<T> * seq)
        : sequenceRef(*seq)
        , loading(true)
        , cancelLoading(false) {
        startThread(true);
    }

    ~ImageSequenceLoader() {
        cancel();
    }

    void cancel() {
        if (loading) {
            ofRemoveListener(ofEvents().update, this, &ImageSequenceLoader::updateThreadedLoad);
            lock();
            cancelLoading = true;
            unlock();
            loading = false;
            waitForThread(true);
        }
    }

    void threadedFunction() {

        ofAddListener(ofEvents().update, this, &ImageSequenceLoader::updateThreadedLoad);

        if (!sequenceRef.preloadAllFilenames()) {
            loading = false;
            return;
        }

        if (cancelLoading) {
            loading = false;
            cancelLoading = false;
            return;
        }

        sequenceRef.preloadAllFrames();

        loading = false;
    }

    void updateThreadedLoad(ofEventArgs & args) {
        if (loading) {
            return;
        }
        ofRemoveListener(ofEvents().update, this, &ImageSequenceLoader::updateThreadedLoad);

        if (sequenceRef.getTotalFrames() > 0) {
            sequenceRef.completeLoading();
        }
    }
};

template<class T>
ofxImageSequence<T>::ofxImageSequence() {
    loaded = false;
    useThread = false;
    frameRate = 30.0f;
    lastFrameLoaded = -1;
    currentFrame = 0;
    maxFrames = 0;
    curLoadFrame = 0;
    threadLoader = NULL;
}

template<class T>
ofxImageSequence<T>::~ofxImageSequence() {
    unloadSequence();
}

template<class T>
bool ofxImageSequence<T>::loadSequence(string prefix, string filetype, int startDigit, int endDigit) {
    return loadSequence(prefix, filetype, startDigit, endDigit, 0);
}

template<class T>
bool ofxImageSequence<T>::loadSequence(string prefix, string filetype, int startDigit, int endDigit, int numDigits) {
    unloadSequence();

    char imagename[1024];
    stringstream format;
    int numFiles = endDigit - startDigit + 1;
    if (numFiles <= 0) {
        ofLogError("ofxImageSequence<T>::loadSequence") << "No image files found.";
        return false;
    }

    if (numDigits != 0) {
        format << prefix << "%0" << numDigits << "d." << filetype;
    } else {
        format << prefix << "%d." << filetype;
    }

    for (int i = startDigit; i <= endDigit; i++) {
        sprintf(imagename, format.str().c_str(), i);
        filenames.push_back(imagename);

        auto pixelType = typeid(T).name();
		if (pixelType == typeid(ofPixels).name()) {
			sequence.push_back(ofPixels());
		} else if (pixelType == typeid(ofShortPixels).name()) {
			sequence.push_back(ofShortPixels());
		} else if (pixelType == typeid(ofFloatPixels).name()) {
			sequence.push_back(ofFloatPixels());
		} else {       
            ofLogError("ofxImageSequence<T>::loadSequence") << "Unrecognised type. Only ofPixels, ofShortPixels, ofFloatPixels are valid.";
            return false;
        }

		loadFailed.push_back(false);
    }

    loaded = true;

    lastFrameLoaded = -1;
    loadFrame(0);

    width = sequence[0].getWidth();
    height = sequence[0].getHeight();
    return true;
}

template<class T>
bool ofxImageSequence<T>::loadSequence(string _folder) {
    unloadSequence();

    folderToLoad = _folder;

    if (useThread) {
        threadLoader = new ImageSequenceLoader(this);
        return true;
    }

    if (preloadAllFilenames()) {
        completeLoading();
        return true;
    }

    return false;
}

template<class T>
void ofxImageSequence<T>::completeLoading() {

    if (sequence.size() == 0) {
        ofLogError("ofxImageSequence<T>::completeLoading") << "load failed with empty image sequence";
        return;
    }

    loaded = true;
    lastFrameLoaded = -1;
    loadFrame(0);

    width = sequence[0].getWidth();
    height = sequence[0].getHeight();
}

template<class T>
bool ofxImageSequence<T>::preloadAllFilenames() {
    ofDirectory dir;
    if (extension != "") {
        dir.allowExt(extension);
    }

    if (!ofFile(folderToLoad).exists()) {
        ofLogError("ofxImageSequence<T>::loadSequence") << "Could not find folder " << folderToLoad;
        return false;
    }

    int numFiles;
    if (maxFrames > 0) {
        numFiles = MIN(dir.listDir(folderToLoad), maxFrames);
    } else {
        numFiles = dir.listDir(folderToLoad);
    }

    if (numFiles == 0) {
        ofLogError("ofxImageSequence<T>::loadSequence") << "No image files found in " << folderToLoad;
        return false;
    }

// read the directory for the images
#ifdef TARGET_LINUX
    dir.sort();
#endif

    for (int i = 0; i < numFiles; i++) {

        filenames.push_back(dir.getPath(i));
        
        auto pixelType = typeid(*this).name();
		if (pixelType == typeid(ofPixels).name()) {
			sequence.push_back(ofPixels());
		} else if (pixelType == typeid(ofShortPixels).name()) {
			sequence.push_back(ofShortPixels());
		} else if (pixelType == typeid(ofFloatPixels).name()) {
			sequence.push_back(ofFloatPixels());
		} else {       
            ofLogError("ofxImageSequence<T>::loadSequence") << "Unrecognised type. Only ofPixels, ofShortPixels, ofFloatPixels are valid.";
            return false;
        }

        loadFailed.push_back(false);
    }
    return true;
}

//set to limit the number of frames. negative means no limit
template<class T>
void ofxImageSequence<T>::setMaxFrames(int newMaxFrames) {
    maxFrames = MAX(newMaxFrames, 0);
    if (loaded) {
        ofLogError("ofxImageSequence<T>::setMaxFrames") << "Max frames must be called before load";
    }
}

template<class T>
void ofxImageSequence<T>::setExtension(string ext) {
    extension = ext;
}

template<class T>
void ofxImageSequence<T>::enableThreadedLoad(bool enable) {

    if (loaded) {
        ofLogError("ofxImageSequence<T>::enableThreadedLoad") << "Need to enable threaded loading before calling load";
    }
    useThread = enable;
}

template<class T>
void ofxImageSequence<T>::cancelLoad() {
    if (useThread && threadLoader != NULL) {
        threadLoader->cancel();

        delete threadLoader;
        threadLoader = NULL;
    }
}

template<class T>
void ofxImageSequence<T>::setMinMagFilter(int newMinFilter, int newMagFilter) {
    minFilter = newMinFilter;
    magFilter = newMagFilter;
    texture.setTextureMinMagFilter(minFilter, magFilter);
}

template<class T>
void ofxImageSequence<T>::preloadAllFrames() {
    if (sequence.size() == 0) {
        ofLogError("ofxImageSequence<T>::loadFrame") << "Calling preloadAllFrames on unitialized image sequence.";
        return;
    }

    for (int i = 0; i < sequence.size(); i++) {
        //threaded stuff
        if (useThread) {
            if (threadLoader == NULL) {
                return;
            }
            threadLoader->lock();
            bool shouldExit = threadLoader->cancelLoading;
            threadLoader->unlock();
            if (shouldExit) {
                return;
            }

            ofSleepMillis(15);
        }
        curLoadFrame = i;
        if (!ofLoadImage(sequence[i], filenames[i])) {
            loadFailed[i] = true;
            ofLogError("ofxImageSequence<T>::loadFrame") << "Image failed to load: " << filenames[i];
        }
    }
}

template<class T>
float ofxImageSequence<T>::percentLoaded() {
    if (isLoaded()) {
        return 1.0;
    }
    if (isLoading() && sequence.size() > 0) {
        return 1.0 * curLoadFrame / sequence.size();
    }
    return 0.0;
}

template<class T>
void ofxImageSequence<T>::loadFrame(int imageIndex) {
    if (lastFrameLoaded == imageIndex) {
        return;
    }

    if (imageIndex < 0 || imageIndex >= sequence.size()) {
        ofLogError("ofxImageSequence<T>::loadFrame") << "Calling a frame out of bounds: " << imageIndex;
        return;
    }

    if (!sequence[imageIndex].isAllocated() && !loadFailed[imageIndex]) {
        if (!ofLoadImage(sequence[imageIndex], filenames[imageIndex])) {
            loadFailed[imageIndex] = true;
            ofLogError("ofxImageSequence<T>::loadFrame") << "Image failed to load: " << filenames[imageIndex];
        }
    }

    if (loadFailed[imageIndex]) {
        return;
    }

    texture.loadData(sequence[imageIndex]);

    lastFrameLoaded = imageIndex;
}

template<class T>
float ofxImageSequence<T>::getPercentAtFrameIndex(int index) {
    return ofMap(index, 0, sequence.size() - 1, 0, 1.0, true);
}

template<class T>
float ofxImageSequence<T>::getWidth() {
    return width;
}

template<class T>
float ofxImageSequence<T>::getHeight() {
    return height;
}

template<class T>
void ofxImageSequence<T>::unloadSequence() {
    if (threadLoader != NULL) {
        delete threadLoader;
        threadLoader = NULL;
    }

    sequence.clear();
    filenames.clear();
    loadFailed.clear();

    loaded = false;
    width = 0;
    height = 0;
    curLoadFrame = 0;
    lastFrameLoaded = -1;
    currentFrame = 0;
}

template<class T>
void ofxImageSequence<T>::setFrameRate(float rate) {
    frameRate = rate;
}

template<class T>
string ofxImageSequence<T>::getFilePath(int index) {
    if (index > 0 && index < filenames.size()) {
        return filenames[index];
    }
    ofLogError("ofxImageSequence<T>::getFilePath") << "Getting filename outside of range";
    return "";
}

template<class T>
int ofxImageSequence<T>::getFrameIndexAtPercent(float percent) {
    if (percent < 0.0 || percent > 1.0) percent -= floor(percent);

    return MIN((int)(percent * sequence.size()), sequence.size() - 1);
}

template<class T>
ofTexture & ofxImageSequence<T>::getTextureForFrame(int index) {
    setFrame(index);
    return getTexture();
}

template<class T>
ofTexture & ofxImageSequence<T>::getTextureForTime(float time) {
    setFrameForTime(time);
    return getTexture();
}

template<class T>
ofTexture & ofxImageSequence<T>::getTextureForPercent(float percent) {
    setFrameAtPercent(percent);
    return getTexture();
}

template<class T>
T & ofxImageSequence<T>::getPixelsForFrame(int index) {
    setFrame(index);
    return sequence[lastFrameLoaded];
}

template<class T>
T & ofxImageSequence<T>::getPixelsForTime(float time) {
    setFrameForTime(time);
    return sequence[lastFrameLoaded];
}

template<class T>
T & ofxImageSequence<T>::getPixelsForPercent(float percent) {
    setFrameAtPercent(percent);
    return sequence[lastFrameLoaded];
}

template<class T>
void ofxImageSequence<T>::setFrame(int index) {
    if (!loaded) {
        ofLogError("ofxImageSequence<T>::setFrame") << "Calling getFrame on unitialized image sequence.";
        return;
    }

    if (index < 0) {
        ofLogError("ofxImageSequence<T>::setFrame") << "Asking for negative index.";
        return;
    }

    index %= getTotalFrames();

    loadFrame(index);
    currentFrame = index;
}

template<class T>
void ofxImageSequence<T>::setFrameForTime(float time) {
    float totalTime = sequence.size() / frameRate;
    float percent = time / totalTime;
    return setFrameAtPercent(percent);
}

template<class T>
void ofxImageSequence<T>::setFrameAtPercent(float percent) {
    setFrame(getFrameIndexAtPercent(percent));
}

template<class T>
ofTexture & ofxImageSequence<T>::getTexture() {
    return texture;
}

template<class T>
const ofTexture & ofxImageSequence<T>::getTexture() const {
    return texture;
}

template<class T>
float ofxImageSequence<T>::getLengthInSeconds() {
    return getTotalFrames() / frameRate;
}

template<class T>
int ofxImageSequence<T>::getTotalFrames() {
    return sequence.size();
}

template<class T>
bool ofxImageSequence<T>::isLoaded() { //returns true if the sequence has been loaded
    return loaded;
}

template<class T>
bool ofxImageSequence<T>::isLoading() {
    return threadLoader != NULL && threadLoader->loading;
}
