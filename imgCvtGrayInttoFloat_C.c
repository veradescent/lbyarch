#include <stdlib.h>

// C implementation of the grayscale conversion function
// Converts integer pixel values (0-255) to float pixel values (0.0-1.0)
// by dividing each value by 255.0
float* imgCvtGrayInttoFloat_C(int n, int *a) {
    // Check for invalid input
    if (n <= 0 || a == NULL) {
        return NULL;
    }
    
    // Allocate memory for float array (n * 4 bytes)
    float *float_array = (float *)malloc(n * sizeof(float));
    if (float_array == NULL) {
        return NULL;
    }
    
    // Convert each int value to float by dividing by 255.0
    for (int i = 0; i < n; i++) {
        float_array[i] = (float)a[i] / 255.0f;
    }
    
    return float_array;
}


