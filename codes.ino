#include <iostream>
#include <wiringPi.h>
#include <vector>
#include <algorithm>
#include <portaudio.h>
#include <ds/ds_c.h>

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 1024
#define BUFFER_SIZE 4096

std::vector<std::vector<char>> createAlphabeticalMatrix() {
    std::vector<std::vector<char>> matrix = {
        {'А', 'Б', 'В'},
        {'Г', 'Д', 'Е'},
        {'Ё', 'Ж', 'З'},
        {'И', 'Й', 'К'},
	{'Л', 'М', 'Н'},	
	{'О', 'П', 'Р'},
	{'С', 'Т', 'У'},
	{'Ф', 'Х', 'Ч'},
	{'Ш', 'Щ', 'Ъ'},
	{'Ы', 'Ь', 'Э'},
	{'Ю', 'Я'}
    };

    return matrix;
}

std::pair<int, int> findCharacterCoordinates(std::vector<std::vector<char>> matrix, char targetChar) {
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[i].size(); j++) {
            if (matrix[i][j] == targetChar) {
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1); 
}

int main() {
    std::vector<std::vector<char>> matrix = createAlphabeticalMatrix();

    char targetChar = '';
    std::pair<int, int> coordinates = findCharacterCoordinates(matrix, targetChar);

    if (coordinates.first != -1 && coordinates.second != -1) {
        std::cout << "Coordinates for character '" << targetChar << "' are: (" << coordinates.first << ", " << coordinates.second << ")" << std::endl;
    } else {
        std::cout << "Character '" << targetChar << "' not found in the matrix." << std::endl;
    }

    return 0;
}

#define X_PIN 9
#define Y_PIN 8

void setupServo(int pin) {
    pinMode(pin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(1920); 
    pwmSetRange(200);
}

void moveServo(int pin, int angle) {
    int pulseWidth = (angle * 10) + 500;
    pwmWrite(pin, pulseWidth);
    delay(1000);  
}

int main() {
    if (wiringPiSetup() == -1) {
        std::cerr << "ошибка" << std::endl;
        return 1;
    }

    setupServo(X_PIN);
    setupServo(Y_PIN);

    int x_coord = 90;  
    int y_coord = 90;  
    moveServo(X_PIN, x_coord);
    moveServo(Y_PIN, y_coord);

    pinMode(X_PIN, INPUT);
    pinMode(Y_PIN, INPUT);

    return 0;
}


static int paRecordCallback(const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData)
{
    DSState *deepspeech = (DSState*)userData;

    if (deepspeech->audio_length + framesPerBuffer > BUFFER_SIZE) {
        std::cerr << "Audio buffer overflow" << std::endl;
        return paAbort;
    }
    
    memcpy(deepspeech->audio_buffer + deepspeech->audio_length, inputBuffer, framesPerBuffer * sizeof(int16_t));
    deepspeech->audio_length += framesPerBuffer;

    return paContinue;
}

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed" << std::endl;
        return 1;
    }

    DSState *deepspeech = CreateModel(modelPath, lmPath, triePath, alphabetPath);

    PaStream *stream;
    err = Pa_OpenDefaultStream(&stream, 1, 0, paInt16, SAMPLE_RATE, FRAMES_PER_BUFFER, paRecordCallback, deepspeech);
    if (err != paNoError) {
        std::cerr << "ошибка" << std::endl;
        return 1;
    }

    deepspeech->audio_buffer = new int16_t[BUFFER_SIZE];
    deepspeech->audio_length = 0;

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "ошибка" << std::endl;
        return 1;
    }
    std::cout << "голос в обработке" << std::endl;

    
    Pa_Sleep(5000);

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "Error stopping PortAudio stream" << std::endl;
        return 1;
    }

 
    std::string transcription = DeepspeechSTT(deepspeech, deepspeech->audio_buffer, deepspeech->audio_length);

    std::cout << "Перевод: " << transcription << std::endl;

    delete [] deepspeech->audio_buffer;
    DestroyModel(deepspeech);

    Pa_Terminate();
    return 0;
}