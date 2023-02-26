#include <iostream>
#include <windows.h>
#include <fstream>
#include <iterator>
#include <vector>

#pragma comment(lib, "winmm.lib")

// g++ .\main.cpp -l "ws2_32" -l "winmm"

char*     AUDIO_BUFFER;
const int THRESHOLD_KNOB  = 10292;
const int BLEND_KNOB      = 3;
const int SELECTOR_SWITCH = 0;
const int HEADER_LEN      = 112;

int analyseWAV()
{
    std::ofstream outFile;
    outFile.open("./Samples/Snare_Mod16.wav", std::ios::binary);

    std::ifstream input;
    input.open("./InputAudio/Snare_Recording16.wav", std::ios::binary);
    std::vector<char> inputBuffer(std::istreambuf_iterator<char>(input), {});
    std::cout << "Input File Size: " << inputBuffer.size() << std::endl;
    
    std::ifstream sampleFile;
    sampleFile.open("./Samples/Snare_Sample16.wav", std::ios::binary);
    std::vector<char> sampleBuffer(std::istreambuf_iterator<char>(sampleFile), {});
    std::cout << "Sample File Size: " << sampleBuffer.size() << std::endl;

    AUDIO_BUFFER = new char[inputBuffer.size()];

    // Write Headers
    for (int i = 0; i < HEADER_LEN; i++)
    {
        AUDIO_BUFFER[i] = inputBuffer[i];
    }

    SHORT inputSample = 0;
    SHORT audio = 0;
    SHORT merge = 0;
    int sampleptr = HEADER_LEN;
    // 0x00 0x01 0x02 0x03
    //std::cout << sizeof(int) << std::endl;
    for (int i = HEADER_LEN; i < inputBuffer.size(); i+=2)
    {
        memcpy(&audio, &inputBuffer[i], 2);
        if (audio > THRESHOLD_KNOB || audio < -THRESHOLD_KNOB || sampleptr > HEADER_LEN){
            if(sampleptr < sampleBuffer.size()-99000){
                memcpy(&inputSample, &sampleBuffer[sampleptr], 2);
                sampleptr +=2;
            }else
            {
                inputSample = 0;
                sampleptr = HEADER_LEN;
            }
        }
            audio /= 1;
            inputSample /= BLEND_KNOB;
            merge = audio+inputSample;
            memcpy(AUDIO_BUFFER+i, &merge, 2);
    }
    outFile.write(AUDIO_BUFFER, inputBuffer.size());
    outFile.close();

    return 0;
}

int main()
{
    analyseWAV();
    PlaySound((LPCSTR)AUDIO_BUFFER, NULL, SND_MEMORY);

    return 0;
}