#include <stdio.h>
#include <stdlib.h>

extern float* imgCvtGrayInttoFloat(int n, int *a);

int main(){
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
        return 1;
    }
    
    // Collect array input
    printf("Enter %d values:\n", total_elements);
    for (int i = 0; i < total_elements; i++) {
        printf("Element [%d]: ", i);
        scanf("%d", &array[i]);
    }
    
    // Execute ASM function - converts int array to float array
    float *float_array = imgCvtGrayInttoFloat(total_elements, array);
    
    if (float_array == NULL) {
        printf("Memory allocation failed\n");
        free(array);
        return 1;
    }
    
    // Print outputs as 2D array
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
    
    return 0;
}