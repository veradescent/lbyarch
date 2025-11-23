#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

extern float* imgCvtGrayInttoFloat(int n, int *a);

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
            return 0;
        }
    }
    return 1;
}

int main() {
    // Test sizes: 10x10, 100x100, 1000x1000
    int test_sizes[3][2] = {{10, 10}, {100, 100}, {1000, 1000}};
    int num_iterations = 100;
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Open output file
    FILE *file = fopen("performance_test_results.txt", "w");
    if (file == NULL) {
        printf("Error: Could not create output file\n");
        return 1;
    }
    
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(file, "Performance Test Results\n");
    fprintf(file, "Running %d iterations for each image dimension\n", num_iterations);
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
    
    printf("Running performance tests...\n");
    printf("This may take a while for larger image sizes.\n\n");
    
    // Test each image size
    for (int size_idx = 0; size_idx < 3; size_idx++) {
        int height = test_sizes[size_idx][0];
        int width = test_sizes[size_idx][1];
        int total_elements = height * width;
        
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        fprintf(file, "Test Image Dimension: %dx%d (%d pixels)\n", height, width, total_elements);
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        
        printf("Testing %dx%d...\n", height, width);
        
        double total_time = 0.0;
        int passed_count = 0;
        int failed_count = 0;
        
        // Run iterations
        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // Allocate memory for input array
            int *array = (int *)malloc(total_elements * sizeof(int));
            if (array == NULL) {
                fprintf(file, "Iteration %d: Memory allocation failed\n", iteration + 1);
                continue;
            }
            
            // Generate random pixel values (0-255)
            for (int i = 0; i < total_elements; i++) {
                array[i] = rand() % 256;
            }
            
            // Time the assembly function
            double start_time = get_time();
            float *float_array = imgCvtGrayInttoFloat(total_elements, array);
            double end_time = get_time();
            
            if (float_array == NULL) {
                fprintf(file, "Iteration %d: FAILED - Memory allocation failed in assembly function\n", iteration + 1);
                free(array);
                failed_count++;
                continue;
            }
            
            double elapsed = end_time - start_time;
            double elapsed_ms = elapsed * 1000.0;
            total_time += elapsed;
            
            // Check correctness
            int correctness_passed = check_correctness(array, float_array, total_elements);
            
            if (correctness_passed) {
                passed_count++;
                fprintf(file, "Iteration %d: PASSED - Time: %.6f ms (%.9f seconds)\n", 
                       iteration + 1, elapsed_ms, elapsed);
            } else {
                failed_count++;
                fprintf(file, "Iteration %d: FAILED - Time: %.6f ms (%.9f seconds)\n", 
                       iteration + 1, elapsed_ms, elapsed);
            }
            
            // Free memory
            free(array);
            free(float_array);
        }
        
        // Calculate average time
        double avg_time = total_time / num_iterations;
        double avg_time_ms = avg_time * 1000.0;
        
        // Write summary
        fprintf(file, "\n");
        fprintf(file, "Summary for %dx%d:\n", height, width);
        fprintf(file, "  Total iterations: %d\n", num_iterations);
        fprintf(file, "  Passed: %d\n", passed_count);
        fprintf(file, "  Failed: %d\n", failed_count);
        fprintf(file, "  Average execution time: %.6f ms (%.9f seconds)\n", avg_time_ms, avg_time);
        fprintf(file, "  Total time for all iterations: %.6f ms (%.9f seconds)\n", 
               total_time * 1000.0, total_time);
        fprintf(file, "\n");
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        
        printf("  Completed: %d passed, %d failed\n", passed_count, failed_count);
        printf("  Average time: %.6f ms\n\n", avg_time_ms);
    }
    
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(file, "Performance Test Complete\n");
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    
    fclose(file);
    
    printf("Performance test complete!\n");
    printf("Results saved to: performance_test_results.txt\n");
    
    return 0;
}

