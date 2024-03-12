#ifndef _WAV_WRITE
#define _WAV_WRITE

#include <iostream>
#include <fstream>

using namespace std;

class wavWrite {
public:
	typedef struct WAV_HEADER {
  		/* RIFF Chunk Descriptor */
  		uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
  		uint32_t ChunkSize;                     // RIFF Chunk Size
  		uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
  		/* "fmt" sub-chunk */
  		uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
  		uint32_t subChunk1Size = 16;           // Size of the fmt chunk
  		uint16_t audioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
  		                          // Mu-Law, 258=IBM A-Law, 259=ADPCM
  		uint16_t numOfChan;   // Number of channels 1=Mono 2=Sterio
  		uint32_t samplesRate;   // Sampling Frequency in Hz
  		uint32_t bytesPerSec = (sampleRate * bitsPerSample * numfChan / 8); // bytes per second - (sampleRate * bitsPerSample * Channels / 8)
  		uint16_t blockAlign;          // 2=16-bit mono, 4=16-bit stereo
  		uint16_t bitsPerSample;      // Number of bits per sample, probably 16 bit
  		/* "data" sub-chunk */
  		uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
  		uint32_t SubChunk2Size;                        // Sampled data length
	} wav_hdr;

	//void writeToFile(ofstream &file, uint16_t value, uint16_t size){
	//	file.write(reinterpret_cast<const char*> (&value), size);
	//}

	void writeDefault(char fname){
		// instantiates header
		wav_hdr wav;
		
		wav.numOfChan = 2;
		wav.sampleRate = 44100;
		wav.blockAlign = 4;
		wav.bitsPerSample = 16;



	};
	//~wavWrite();

private:
	

}

int main () {
	ofstream audioFile; // file output stream audioFile
	audioFile.open(s:"waveform.wav", ios::binary); //
						       //
	
	




}



#endif






