#include <string.h>
void score(double * input, double * output) {
    double var0[2];
    if (input[3] <= 33.224544525146484) {
        memcpy(var0, (double[]){0.0, 1.0}, 2 * sizeof(double));
    } else {
        memcpy(var0, (double[]){1.0, 0.0}, 2 * sizeof(double));
    }
    memcpy(output, var0, 2 * sizeof(double));
}
