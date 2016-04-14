//By: William Chen and Tommy Lee
//thu18kora Cameron
//Date: 14/04/2016
//mandelbrot set task

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define LIMIT 255

typedef struct _complex {
    double a;
    double b;
} complex;

typedef struct _color {
    unsigned char red;
    unsigned char blue;
    unsigned char green;
} color;

typedef struct _position {
    double x;
    double y;
} position;

complex squareComplex(complex complexNum);
double modulus(complex complexNum);
int escapeSteps(position point);
void unitTest(void);

int main(int argc, char *argv[]) {
    unitTest();
    return EXIT_SUCCESS;
}

complex squareComplex(complex complexNum) {
    complex result;
    result.a = pow(complexNum.a,2) - pow(complexNum.b,2);
    result.b = 2*(complexNum.a * complexNum.b);
    return result;
}

double modulus(complex complexNum) {
    double modulus;
    modulus = sqrt(pow(complexNum.a,2)+pow(complexNum.b,2));
    return modulus;
}

int escapeSteps(position point) {
    int iterations = 0;
    while(iterations < 256 && modulus(point)< 2) {
        point.a = squareComplex(point).a + point.a; 
        point.b = squareComplex(point).b + point.b; 
        iterations++;
    }
    return iterations;
}

void unitTest(void) {
    complex testComplex;
    testComplex.a = 0;
    testComplex.b = 1;
    assert(squareComplex(testComplex).a == -1 && 
        squareComplex(testComplex).b == 0);
    assert(modulus(testComplex) == 1);
    testComplex.a = 2;
    testComplex.b = 0;
    assert(squareComplex(testComplex).a == 4 &&
        squareComplex(testComplex).b == 0);
    assert(modulus(testComplex) == 2);
    
    printf("Success !\n");
}

