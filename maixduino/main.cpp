#include <I2S.h>
#include <SD.h>
#include <SPI.h>

// Constants
const int recordPin = 10;  // Pin to trigger recording
const int micPin = A0;     // Analog pin for microphone input
const int sampleRate = 16000;  // Sample rate in Hz
const int bitsPerSample = 16;  // 16-bit audio
const int numChannels = 1;    // Mono audio

// Global Variables
File audioFile;

void setup() {
  Serial.begin(115200);
  
  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while(1);
  }
  Serial.println("SD card initialized.");
  
  // Initialize I2S
  I2S.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample);
  
  // Setup the pin for recording
  pinMode(recordPin, INPUT_PULLUP);
}

void writeWAVHeader(File file, int sampleRate, int numChannels, int bitsPerSample, unsigned long dataSize) {
  // RIFF chunk descriptor
  file.write("RIFF", 4);
  unsigned long chunkSize = 36 + dataSize;
  file.write((byte*)&chunkSize, 4);
  file.write("WAVE", 4);
  
  // fmt subchunk
  file.write("fmt ", 4);
  unsigned long subChunk1Size = 16;
  file.write((byte*)&subChunk1Size, 4);
  unsigned short audioFormat = 1;
  file.write((byte*)&audioFormat, 2);
  file.write((byte*)&numChannels, 2);
  file.write((byte*)&sampleRate, 4);
  unsigned long byteRate = sampleRate * numChannels * bitsPerSample / 8;
  file.write((byte*)&byteRate, 4);
  unsigned short blockAlign = numChannels * bitsPerSample / 8;
  file.write((byte*)&blockAlign, 2);
  file.write((byte*)&bitsPerSample, 2);
  
  // data subchunk
  file.write("data", 4);
  file.write((byte*)&dataSize, 4);
}

void updateWAVHeader(File file, unsigned long dataSize) {
  // Update the file size at byte 4 (ChunkSize)
  unsigned long chunkSize = 36 + dataSize;
  file.seek(4);
  file.write((byte*)&chunkSize, 4);
  
  // Update the data size at byte 40 (Subchunk2Size)
  file.seek(40);
  file.write((byte*)&dataSize, 4);
}

void startRecording() {
  Serial.println("Attempting to open file for writing...");
  
  // Open file with truncation mode
  audioFile = SD.open("AUDIO_~1.WAV", O_WRITE | O_CREAT | O_TRUNC);
  if (!audioFile) {
    Serial.println("Error opening file for writing");
    Serial.println("SD card initialization status: ");
    Serial.println(SD.begin());
    Serial.print("Available memory: ");
    while(1);
  }
  Serial.println("File opened successfully");
  
  // Write the initial WAV header (with placeholder sizes)
  writeWAVHeader(audioFile, sampleRate, numChannels, bitsPerSample, 0);
  Serial.println("Recording started");
}

void stopRecording() {
  Serial.println("Recording stopped");
  
  // Get the actual data size
  unsigned long dataSize = audioFile.size() - 44;
  
  // Update WAV header with correct sizes
  updateWAVHeader(audioFile, dataSize);
  audioFile.close();
}

void loop() {
  // Start recording when pin is pulled LOW, stop when released
  if (digitalRead(recordPin) == LOW) {
    startRecording();
    
    int16_t audioBuffer[256];
    while (digitalRead(recordPin) == LOW) {
      I2S.read(audioBuffer, 256);
      audioFile.write((byte*)audioBuffer, 256 * sizeof(int16_t));
    }
    
    stopRecording();
  }
}