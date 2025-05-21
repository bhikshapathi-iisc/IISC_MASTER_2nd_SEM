
#ifndef SCORE_H
#define SCORE_H

#include <Arduino.h>

// Function to classify activity based on input features
void score(const float input[], float output[]) {
    // Initialize output array with zeros
    for (int i = 0; i < 5; i++) {
        output[i] = 0.0;
    }

    if (input[0] <= -0.9092099964618683) {
        output[1] = 1.0; // Second class
    } else {
        if (input[2] <= 0.7516500055789948) {
            if (input[2] <= -0.752670019865036) {
                output[2] = 1.0; // Third class
            } else {
                if (input[0] <= -0.36774998903274536) {
                    output[4] = 1.0; // Fifth class
                } else {
                    output[3] = 1.0; // Fourth class
                }
            }
        } else {
            output[0] = 1.0; // First class
        }
    }
}

#endif // SCORE_H
