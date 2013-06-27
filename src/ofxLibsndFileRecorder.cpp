#include "ofxLibsndFileRecorder.h"

ofxLibsndFileRecorder::ofxLibsndFileRecorder(){
	initialized=false;
	recordingBuffer=NULL;
}


void ofxLibsndFileRecorder::setup(string fileName, int lengthMs, int sampleRate_, int numChannels_){
	
	sampleRate=sampleRate_;
	numChannels = numChannels_;
	recordingSize=0;
	audioFormat=SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	
	//cout << "SND PATH: "<<ofToDataPath(fileName,true)<<"\n";
	outFile=new SndfileHandle(fileName, SFM_RDWR, audioFormat, numChannels, sampleRate);
	if(!outFile){
		cout<<"SndFileHandle outfile creation error\n";
		initialized=false;
	}
	else{
		//recordingBuffer=(float*)realloc(recordingBuffer, sizeof(float));
		totalRecordingSize = sampleRate * numChannels * (int)((float)lengthMs / 1000.f);
		recordingBuffer = (float*)malloc(sizeof(float) * totalRecordingSize);
		initialized=true;	
	}
}
void  ofxLibsndFileRecorder::close(){
	if(recordingBuffer!=NULL)
		free(recordingBuffer);
	recordingBuffer=NULL;
	initialized=false;
	recordingSize=0;
}

void  ofxLibsndFileRecorder::setFormat(int format){
	/*****EXAMPLES:  (see sndfile.h for more information)
		SF_FORMAT_AIFF | SF_FORMAT_PCM_16;
		SF_FORMAT_RAW | SF_FORMAT_PCM_16;
		SF_FORMAT_AU | SF_FORMAT_FLOAT;
	*/
	audioFormat=format;
}

int ofxLibsndFileRecorder::addSamples(float* &input, int numSamples){
	if(initialized){
		int new_samples = numSamples * numChannels;
		if (recordingSize + new_samples > totalRecordingSize)
			return 1; // no space left to record. TODO better error code 

		memcpy(&recordingBuffer[recordingSize], input, new_samples*sizeof(float));
		recordingSize+=new_samples;

		return 0;

		//// using realloc over and over here seems like a bad idea	
		//// TODO switch to using buffers that gradually get filled up
		//recordingBuffer=(float*)realloc(recordingBuffer, recordingSize*sizeof(float));
		////cout << recordingSize <<" "<<numSamples <<" "<<numChannels << ""<< recordingSize*sizeof(float)<<"\n";
		//memcpy(&recordingBuffer[recordingSize-numSamples*numChannels], input, numSamples*numChannels*sizeof(float)); 	
	}
	return 1; //  not initiliazed TODO err code
}

void ofxLibsndFileRecorder::finalize(){
	if(initialized){
		int res=outFile->write(recordingBuffer, recordingSize);
		sf_close(outFile->takeOwnership());
		initialized=false;
		recordingSize=0;
		delete outFile;
	}
}