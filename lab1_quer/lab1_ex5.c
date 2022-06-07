#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int n, i, j;
    int *vet;

    if (argc != 2) {
    	printf("Wrong number of parameters\n");
    	exit(1);
    }
    
    n = atoi(argv[1]);
    vet = (int *) malloc(n * sizeof(int));

    if (vet == NULL) {
        printf("Allocation error\n");
        exit(   1);
    }

    printf("Binary numbers:\n");
    // start of cool part
    for (i=0; i<n; i++) {
        if (fork()) {
            vet[i] = 0;
        } else {
            vet[i] = 1;
        }
    }
    // end of cool part
    for (j=0; j<n; j++) {
        printf("%d", vet[j]);
    }
    printf("\n");
    free(vet);
    return 0;
}

