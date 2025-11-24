#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// C implementation function from separate file
extern float* imgCvtGrayInttoFloat_C(int n, int *a);

// High-resolution timer function
double get_time() {
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
#endif
}

// Check correctness of conversion
int check_correctness(int *int_array, float *float_array, int n) {
    for (int i = 0; i < n; i++) {
        float expected = (float)int_array[i] / 255.0f;
        float diff = float_array[i] - expected;
        if (diff < 0) diff = -diff; // absolute value
        if (diff > 0.001f) { // Allow small floating point error
            printf("Error at index %d: expected %.6f, got %.6f\n", 
                   i, expected, float_array[i]);
            return 0;
        }
    }
    return 1;
}

// Manual input mode
void manual_mode() {
    int height, width;
    
    // Collect input - height and width
    printf("Enter height: ");
    scanf("%d", &height);
    printf("Enter width: ");
    scanf("%d", &width);
    
    int total_elements = height * width;
    
    // Allocate memory for 2D array (flattened as 1D)
    int *array = (int *)malloc(total_elements * sizeof(int));
    
    if (array == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    
    // Collect array input
    printf("Enter %d pixel values (0-255):\n", total_elements);
    for (int i = 0; i < total_elements; i++) {
        printf("Pixel [%d]: ", i);
        scanf("%d", &array[i]);
        // Clamp to valid range
        if (array[i] < 0) array[i] = 0;
        if (array[i] > 255) array[i] = 255;
    }
    
    // Time the C function
    double start_time = get_time();
    float *float_array = imgCvtGrayInttoFloat_C(total_elements, array);
    double end_time = get_time();
    
    if (float_array == NULL) {
        printf("Memory allocation failed\n");
        free(array);
        return;
    }
    
    double elapsed = end_time - start_time;
    double elapsed_ms = elapsed * 1000.0;
    
    // Check correctness before outputting
    int correctness_passed = check_correctness(array, float_array, total_elements);
    if (!correctness_passed) {
        printf("\nError: Correctness check failed. Output may be incorrect.\n");
        free(array);
        free(float_array);
        return;
    }
    
    // Print outputs
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Conversion Results (C Implementation)\n");
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Correctness check: PASSED\n");
    printf("Execution time: %.6f ms (%.9f seconds)\n", elapsed_ms, elapsed);
    printf("\nConverted grayscale values (2D array):\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%.2f ", float_array[i * width + j]);
        }
        printf("\n");
    }
    
    // Free allocated memory
    free(array);
    free(float_array);
}

// Automated mode
void automated_mode() {
    int size_choice;
    int height, width;
    
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Automated Grayscale Image Conversion Test (C Implementation)\n");
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Choose image size:\n");
    printf("1. 10x10\n");
    printf("2. 100x100\n");
    printf("3. 1000x1000\n");
    printf("Enter your choice (1, 2, or 3): ");
    scanf("%d", &size_choice);
    
    // Set dimensions based on choice
    switch(size_choice) {
        case 1:
            height = 10;
            width = 10;
            break;
        case 2:
            height = 100;
            width = 100;
            break;
        case 3:
            height = 1000;
            width = 1000;
            break;
        default:
            printf("Invalid choice. Exiting.\n");
            return;
    }
    
    int total_elements = height * width;
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    printf("\nTesting image size: %dx%d (%d pixels)\n", height, width, total_elements);
    
    // Allocate memory for input array
    int *array = (int *)malloc(total_elements * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed for size %dx%d\n", height, width);
        return;
    }
    
    // Generate random pixel values (0-255)
    for (int i = 0; i < total_elements; i++) {
        array[i] = rand() % 256; // Random value between 0 and 255
    }
    
    // Display generated inputs
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Generated Input (Integer Pixel Values)\n");
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", array[i * width + j]);
        }
        printf("\n");
    }
    
    // Time the C function
    double start_time = get_time();
    float *float_array = imgCvtGrayInttoFloat_C(total_elements, array);
    double end_time = get_time();
    
    if (float_array == NULL) {
        printf("Memory allocation failed in C function\n");
        free(array);
        return;
    }
    
    double elapsed = end_time - start_time;
    double elapsed_ms = elapsed * 1000.0;
    
    // Check correctness before outputting
    int correctness_passed = check_correctness(array, float_array, total_elements);
    if (!correctness_passed) {
        printf("\nError: Correctness check failed. Output may be incorrect.\n");
        free(array);
        free(float_array);
        return;
    }
    
    // For 1000x1000, output to text file
    if (height == 1000 && width == 1000) {
        FILE *file = fopen("output_1000x1000_c.txt", "w");
        if (file != NULL) {
            fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            fprintf(file, "Grayscale Conversion Results (1000x1000) - C Implementation\n");
            fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            fprintf(file, "Correctness check: PASSED\n");
            fprintf(file, "Execution time: %.6f ms (%.9f seconds)\n", elapsed_ms, elapsed);
            fprintf(file, "\nGenerated Input (Integer Pixel Values):\n");
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    fprintf(file, "%d ", array[i * width + j]);
                }
                fprintf(file, "\n");
            }
            fprintf(file, "\nConverted Output (Float Pixel Values):\n");
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    fprintf(file, "%.2f ", float_array[i * width + j]);
                }
                fprintf(file, "\n");
            }
            fclose(file);
            printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            printf("Results for %dx%d\n", height, width);
            printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            printf("Correctness check: PASSED\n");
            printf("Execution time: %.6f ms (%.9f seconds)\n", elapsed_ms, elapsed);
            printf("Full output saved to: output_1000x1000_c.txt\n");
        } else {
            printf("Error: Could not create output file\n");
            // Fall back to console output
            printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            printf("Results for %dx%d\n", height, width);
            printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
            printf("Correctness check: PASSED\n");
            printf("Execution time: %.6f ms (%.9f seconds)\n", elapsed_ms, elapsed);
            printf("\nSample output (first 5x5 pixels):\n");
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    printf("%.2f ", float_array[i * width + j]);
                }
                printf("\n");
            }
        }
    } else {
        // Display results for 10x10 and 100x100
        printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        printf("Results for %dx%d\n", height, width);
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        printf("Correctness check: PASSED\n");
        printf("Execution time: %.6f ms (%.9f seconds)\n", elapsed_ms, elapsed);
        
        // Display full output for smaller sizes
        printf("\nConverted Output (Float Pixel Values):\n");
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                printf("%.2f ", float_array[i * width + j]);
            }
            printf("\n");
        }
    }
    
    // Free memory
    free(array);
    free(float_array);
    
    printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Automated Test Complete\n");
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
}

int main() {
    int choice;
    
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("Grayscale Image Conversion Program (C Implementation)\n");
    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    printf("1. Manual input mode\n");
    printf("2. Automated mode (10x10, 100x100, 1000x1000)\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        printf("\n");
        manual_mode();
    } else if (choice == 2) {
        automated_mode();
    } else {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }
    
    return 0;
}

