#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

extern float* imgCvtGrayInttoFloat(int n, int *a);  // Assembly version
extern float* imgCvtGrayInttoFloat_C(int n, int *a);  // C version

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

// Check if two float arrays produce the same output
int check_outputs_match(float *array1, float *array2, int n) {
    for (int i = 0; i < n; i++) {
        float diff = array1[i] - array2[i];
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
    int num_iterations = 30;
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Open performance results file
    FILE *file = fopen("performance_test_results.txt", "w");
    if (file == NULL) {
        printf("Error: Could not create performance test results file\n");
        return 1;
    }
    
    // Open inputs/outputs file
    FILE *io_file = fopen("test_inputs_outputs.txt", "w");
    if (io_file == NULL) {
        printf("Error: Could not create inputs/outputs file\n");
        fclose(file);
        return 1;
    }
    
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(file, "Performance Test Results\n");
    fprintf(file, "Comparing Assembly vs C Implementation\n");
    fprintf(file, "Running %d iterations for each image dimension\n", num_iterations);
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
    
    fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(io_file, "Test Inputs and Outputs\n");
    fprintf(io_file, "Comparing Assembly vs C Implementation\n");
    fprintf(io_file, "Running %d iterations for each image dimension\n", num_iterations);
    fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
    
    printf("Running performance tests...\n");
    printf("Comparing Assembly vs C implementation\n");
    printf("This may take a while for larger image sizes.\n\n");
    
    // Test each image size
    for (int size_idx = 0; size_idx < 3; size_idx++) {
        int height = test_sizes[size_idx][0];
        int width = test_sizes[size_idx][1];
        int total_elements = height * width;
        
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        fprintf(file, "Test Image Dimension: %dx%d (%d pixels)\n", height, width, total_elements);
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        
        fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        fprintf(io_file, "Test Image Dimension: %dx%d (%d pixels)\n", height, width, total_elements);
        fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
        if (height == 1000) {
            fprintf(io_file, "Note: Inputs and outputs are not included for 1000x1000 due to file size.\n");
            fprintf(io_file, "Performance timing data is still recorded in performance_test_results.txt\n");
        }
        fprintf(io_file, "\n");
        
        printf("Testing %dx%d...\n", height, width);
        
        double total_time_asm = 0.0;
        double total_time_c = 0.0;
        int passed_count_asm = 0;
        int failed_count_asm = 0;
        int passed_count_c = 0;
        int failed_count_c = 0;
        int outputs_match_count = 0;
        int outputs_mismatch_count = 0;
        
        // Run iterations
        for (int iteration = 0; iteration < num_iterations; iteration++) {
            // Allocate memory for input array
            int *array = (int *)malloc(total_elements * sizeof(int));
            if (array == NULL) {
                fprintf(file, "Iteration %d: Memory allocation failed for input\n", iteration + 1);
                fprintf(io_file, "Iteration %d: Memory allocation failed for input\n", iteration + 1);
                continue;
            }
            
            // Generate random pixel values (0-255) - same input for both versions
            for (int i = 0; i < total_elements; i++) {
                array[i] = rand() % 256;
            }
            
            // Write input to IO file (once per iteration) - only for 10x10 and 100x100
            if (height <= 100) {
                fprintf(io_file, "Iteration %d:\n", iteration + 1);
                fprintf(io_file, "Input (Integer Pixel Values):\n");
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        fprintf(io_file, "%d ", array[i * width + j]);
                    }
                    fprintf(io_file, "\n");
                }
                fprintf(io_file, "\n");
            }
            
            // Test Assembly version
            double start_time_asm = get_time();
            float *float_array_asm = imgCvtGrayInttoFloat(total_elements, array);
            double end_time_asm = get_time();
            
            double elapsed_asm = 0.0;
            int correctness_asm = 0;
            if (float_array_asm == NULL) {
                fprintf(file, "Iteration %d (Assembly): FAILED - Memory allocation failed\n", iteration + 1);
                if (height <= 100) {
                    fprintf(io_file, "Assembly Output: FAILED - Memory allocation failed\n");
                }
                failed_count_asm++;
            } else {
                elapsed_asm = end_time_asm - start_time_asm;
                total_time_asm += elapsed_asm;
                correctness_asm = check_correctness(array, float_array_asm, total_elements);
                if (correctness_asm) {
                    passed_count_asm++;
                } else {
                    failed_count_asm++;
                }
                
                // Write assembly output to IO file - only for 10x10 and 100x100
                if (height <= 100) {
                    fprintf(io_file, "Assembly Output (Float Pixel Values):\n");
                    for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            fprintf(io_file, "%.2f ", float_array_asm[i * width + j]);
                        }
                        fprintf(io_file, "\n");
                    }
                    fprintf(io_file, "\n");
                }
            }
            
            // Test C version
            double start_time_c = get_time();
            float *float_array_c = imgCvtGrayInttoFloat_C(total_elements, array);
            double end_time_c = get_time();
            
            double elapsed_c = 0.0;
            int correctness_c = 0;
            if (float_array_c == NULL) {
                fprintf(file, "Iteration %d (C): FAILED - Memory allocation failed\n", iteration + 1);
                if (height <= 100) {
                    fprintf(io_file, "C Output: FAILED - Memory allocation failed\n");
                }
                failed_count_c++;
            } else {
                elapsed_c = end_time_c - start_time_c;
                total_time_c += elapsed_c;
                correctness_c = check_correctness(array, float_array_c, total_elements);
                if (correctness_c) {
                    passed_count_c++;
                } else {
                    failed_count_c++;
                }
                
                // Write C output to IO file - only for 10x10 and 100x100
                if (height <= 100) {
                    fprintf(io_file, "C Output (Float Pixel Values):\n");
                    for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            fprintf(io_file, "%.2f ", float_array_c[i * width + j]);
                        }
                        fprintf(io_file, "\n");
                    }
                    fprintf(io_file, "\n");
                }
            }
            
            // Check if outputs match (both must have succeeded)
            int outputs_match = 0;
            if (float_array_asm != NULL && float_array_c != NULL) {
                outputs_match = check_outputs_match(float_array_asm, float_array_c, total_elements);
                if (outputs_match) {
                    outputs_match_count++;
                } else {
                    outputs_mismatch_count++;
                }
            }
            
            // Write iteration results to performance file
            fprintf(file, "Iteration %d:\n", iteration + 1);
            fprintf(file, "  Assembly: %s - Time: %.6f ms (%.9f seconds)\n", 
                   correctness_asm ? "PASSED" : "FAILED", 
                   elapsed_asm * 1000.0, elapsed_asm);
            fprintf(file, "  C:        %s - Time: %.6f ms (%.9f seconds)\n", 
                   correctness_c ? "PASSED" : "FAILED", 
                   elapsed_c * 1000.0, elapsed_c);
            if (float_array_asm != NULL && float_array_c != NULL) {
                fprintf(file, "  Outputs Match: %s\n", outputs_match ? "YES" : "NO");
            }
            fprintf(file, "\n");
            
            // Write separator to IO file - only for 10x10 and 100x100
            if (height <= 100) {
                fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
            }
            
            // Free memory
            free(array);
            if (float_array_asm != NULL) free(float_array_asm);
            if (float_array_c != NULL) free(float_array_c);
        }
        
        // Calculate average times
        double avg_time_asm = total_time_asm / num_iterations;
        double avg_time_asm_ms = avg_time_asm * 1000.0;
        double avg_time_c = total_time_c / num_iterations;
        double avg_time_c_ms = avg_time_c * 1000.0;
        
        fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        
        printf("  Assembly: %d passed, %d failed, Avg: %.6f ms\n", 
               passed_count_asm, failed_count_asm, avg_time_asm_ms);
        printf("  C:        %d passed, %d failed, Avg: %.6f ms\n", 
               passed_count_c, failed_count_c, avg_time_c_ms);
        printf("  Outputs Match: %d, Mismatch: %d\n\n", outputs_match_count, outputs_mismatch_count);
    }
    
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(file, "Performance Test Complete\n");
    fprintf(file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    
    fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    fprintf(io_file, "Test Inputs and Outputs Complete\n");
    fprintf(io_file, "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
    
    fclose(file);
    fclose(io_file);
    
    printf("Performance test complete!\n");
    printf("Results saved to: performance_test_results.txt\n");
    printf("Inputs/Outputs saved to: test_inputs_outputs.txt\n");
    
    return 0;
}
