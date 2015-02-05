#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int Delay(int Milliseconds) {
    usleep(1000 * Milliseconds);
    return true;
}

int SetLEDBrightness(const int LedIndex, const int Brightness) {
    int Result = false;
    char LEDDevicePath[] = "/sys/class/leds/beaglebone:green:usr_/brightness";
    LEDDevicePath[36] = '0' + LedIndex;
    FILE * LEDFileDescriptor;

    if((LEDFileDescriptor = fopen(LEDDevicePath, "r+")) != NULL){
        switch (Brightness) {
            case 0: {
                fwrite("0", sizeof(char), 1, LEDFileDescriptor);
                break;
            }
            case 1: {
                fwrite("1", sizeof(char), 1, LEDFileDescriptor);
                break;
            }
            default: {
		fprintf(stderr, "Brightness out of range\n");
	    }
        }
        fclose(LEDFileDescriptor);
        Result = true;
    }
    else {
        fprintf(stderr, "Unable to open device: %s\n", LEDDevicePath);
        Result = false;
    }
    return Result ;
}

int SetLEDTrigger(const int LedIndex, const char * TriggerType) {
    int Result = false;
    char LEDDevicePath[] = "/sys/class/leds/beaglebone:green:usr_/trigger";
    LEDDevicePath[36] = '0' + LedIndex;
    FILE * LEDFileDescriptor;

    if((LEDFileDescriptor = fopen(LEDDevicePath, "r+")) != NULL){
        fwrite(TriggerType, sizeof(TriggerType), 1, LEDFileDescriptor);
        fclose(LEDFileDescriptor);
        Result = true;
    }
    else {
        fprintf(stderr, "Unable to open device: %s\n", LEDDevicePath);
        Result = false;
    }
    return Result ;
}

int InitializeLEDs() {
    int c;

    for (c = 0; c < 3; c++) {
        SetLEDTrigger(c, "none");
    }

    for (c = 0; c < 4; c++) {
        SetLEDBrightness(c, 1);
        Delay(100);
        SetLEDBrightness(c, 0);
    }

    for (c = 2; c > -1; c--) {
        SetLEDBrightness(c, 1);
        Delay(100);
        SetLEDBrightness(c, 0);
    }
    return true;
}

int main(int argc, char ** argv) {
    double Load[1];
    int LEDState[4];
    int c;

    InitializeLEDs();

    while (true) {
        if (getloadavg(Load, 1) != -1) {
            for (c = 0; c < 4; c++) {
                if (Load[0] > 0.24+0.25*c) {
                    LEDState[c] = 1;
                }
                else {
                    LEDState[c] = 0;
                }
                SetLEDBrightness(c, LEDState[c]);
            }
        }
        else {
            fprintf(stderr, "Error retrieving system load\n");
        }
        Delay(1000);
    }
    return EXIT_FAILURE;
}
