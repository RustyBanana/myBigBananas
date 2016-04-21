//By: William Chen and Tommy Lee
//thu18kora Cameron
//Date: 14/04/2016
//mandelbrot set task


/*
 *  based on bmpServer.c
 *  1917 serve that 3x3 bmp from lab3 Image activity
 *
 *  Created by Tim Lambert on 02/04/12.
 *  Containing code created by Richard Buckland on 28/01/11.
 *  Copyright 2012 Licensed under Creative Commons SA-BY-NC 3.0. 
 *
 */
 
 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "mandelbrot.h"
#include "pixelColor.h"

#define LIMIT 255
#define SIZE 512
#define HEADER_SIZE 54
#define BYTES_PER_PIXEL 3

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 7191
#define NUMBER_OF_PAGES_TO_SERVE 10

typedef struct _complex {
    double a;
    double b;
} complex;

typedef struct _color {
    unsigned char red;
    unsigned char blue;
    unsigned char green;
} color;

static complex squareComplex(complex complexNum);
static double modulus(complex complexNum);
static void unitTest(void);
static int waitForConnection (int serverSocket);
static int makeServerSocket (int portno);
static double stringToDouble(char *numString);
static void serveBMP (int socket, complex centre, int zoom);
static void serveHTML (int socket);
complex pixelPosition(complex centrePixel, int zoom, int col, int row);

int main (int argc, char *argv[]) {
   unitTest();   
   printf ("************************************\n");
   printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
   printf ("Serving bmps since 2012\n");   
   
   int serverSocket = makeServerSocket (DEFAULT_PORT);   
   printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
   printf ("************************************\n");
   
   char request[REQUEST_BUFFER_SIZE];
   
   int numberServed = 0;
   while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {
      
      printf ("*** So far served %d pages ***\n", numberServed);
      
      int connectionSocket = waitForConnection (serverSocket);
      // wait for a request to be sent from a web browser, open a new
      // connection for this conversation
      
      // read the first line of the request sent by the browser  
      int bytesRead;
      bytesRead = read (connectionSocket, request, (sizeof request)-1);
      assert (bytesRead >= 0); 
      // were we able to read any data from the connection?
            
      // print entire request to the console 
      printf (" *** Received http request ***\n %s\n", request);
      
      //send the browser a simple html page using http
      printf (" *** Sending http response ***\n");
      
      //analyse request
      int i = 0;
      while (request[i] != '/') {
         i++;
      }
      if (request[i+1] != ' ') {
         //start of request
         while (request[i] != 'x') {
            i++;
         }
         //start of x
         char numString[50];
         i++; //i is index of start of x number
         int numIndex = 0;
         while (request[numIndex+i] != '_') {
            numString[numIndex] = request[numIndex+i];
            numIndex++;
         }
         numString[numIndex] = '\0';
         double x = stringToDouble(numString);
         
         i = numIndex + i + 2; //i is index of start of y number
         numIndex=0;
         while (request[numIndex+i] != '_') {
            numString[numIndex] = request[numIndex+i];
            numIndex++;
         }
         numString[numIndex] = '\0';
         double y = stringToDouble(numString);
         
         i = numIndex + i + 2; //i is index of start of z number
         numIndex=0;
         while (request[numIndex+i] != '.') {
            numString[numIndex] = request[numIndex+i];
            numIndex++;
         }      
         numString[numIndex] = '\0';
         double tempZoom = stringToDouble(numString);
         int zoom = tempZoom;

         
         printf("x: %lf, y: %lf, z: %d here we go",x,y,zoom);
         complex centre;
         centre.a = x;
         centre.b = y;
         serveBMP(connectionSocket, centre, zoom);
      } else {
         //empty request; send back script
         serveHTML(connectionSocket);
      }
      // close the connection after sending the page- keep aust beautiful
      close(connectionSocket);
      
      numberServed++;
   } 
   
   // close the server connection after we are done- keep aust beautiful
   printf ("** shutting down the server **\n");
   close (serverSocket);
   
   return EXIT_SUCCESS; 
}

static void serveHTML (int socket) {
   char* message;
 
   // first send the http response header
   message =
      "HTTP/1.0 200 Found\n"
      "Content-Type: text/html\n"
      "\n";
   printf ("about to send=> %s\n", message);
   write (socket, message, strlen (message));
 
   message =
      "<!DOCTYPE html>\n"
      "<script src=\"http://almondbread.cse.unsw.edu.au/tiles.js\"></script>"
      "\n";      
   write (socket, message, strlen (message));
}


static double stringToDouble(char *numString) {
   int i = 0;
   int pointerIndex = 0;
   double result = 0;
   while (numString[i] != '\0') {
      if (numString[i] == '.') {
         pointerIndex = i;
      }
      i++;
   }
   //i = index of end of NULL
   i = i - 1;
   int endIndex = i;
   if (pointerIndex == 0) {
      pointerIndex = endIndex + 1;
   }
   while (i>0) {
      if (i < pointerIndex) {
         //num is before the decimal point
         result += (numString[i] - '0') * pow(10, pointerIndex - i - 1);
      } else if (i > pointerIndex) {
         //num is after the decimal point
         result += (numString[i] - '0') * pow(10, pointerIndex - i);
      }
      i = i - 1;
   }
   if (numString[i] == '-') {
      result = 0 - result;
   } else {
      if (i < pointerIndex) {
         //num is before the decimal point
         result += (numString[i] - '0') * pow(10, pointerIndex - i - 1);
      } else if (i > pointerIndex) {
         //num is after the decimal point
         result += (numString[i] - '0') * pow(10, pointerIndex - i);
      }
   }
   return result;
}

static void serveBMP (int socket, complex centre, int zoom) {
   char* message;
   
   // first send the http response header
   
   // (if you write stings one after another like this on separate
   // lines the c compiler kindly joins them togther for you into
   // one long string)
   message = "HTTP/1.0 200 OK\r\n"
                "Content-Type: image/bmp\r\n"
                "\r\n";
   printf ("about to send=> %s\n", message);
   write (socket, message, strlen (message));
   
   // now send the BMP
   unsigned char bmp[SIZE * SIZE * BYTES_PER_PIXEL + HEADER_SIZE] = 
   { 0x42,0x4d, //BM
     0x36,0x00,0x02,0x00, //size
     0x00,0x00,0x00,0x00,
     0x36,0x00,0x00,0x00, //offset to start of pixels
     0x28,0x00,0x00,0x00, //size of DIB header
     0x00,0x02,0x00,0x00, //width (pixels)
     0x00,0x02,0x00,0x00, //height (pixels)
     0x01,0x00,
     0x18,0x00, //bits per pixel
     0x00,0x00,0x00,0x00, //compression
     0x24,0x00,0x00,0x00, //image size
     0x13,0x0b,0x00,0x00, //horizontal resolution
     0x13,0x0b,0x00,0x00, //vertical resolution
     0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00 //pixels start from here.
   };
   
   complex pixelPos;
   int bmpIndex = HEADER_SIZE;
   int steps;
   int row = 0;
   int col;
   while (row < SIZE) {
      col = 0;
      while (col < SIZE) {
         pixelPos = pixelPosition(centre, zoom, col, row);
         steps = escapeSteps(pixelPos.a,pixelPos.b);
         bmp[bmpIndex] = stepsToBlue(steps);
         bmpIndex++;
         bmp[bmpIndex] = stepsToGreen(steps);
         bmpIndex++;
         bmp[bmpIndex] = stepsToRed(steps);
         bmpIndex++;
         col++;
      }
      row++;
   }
   
   write (socket, bmp, sizeof(bmp));
}


// start the server listening on the specified port number
static int makeServerSocket (int portNumber) { 
   // create socket
   int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
   assert (serverSocket >= 0);   
   // error opening socket
   
   // bind socket to listening port
   struct sockaddr_in serverAddress;
   memset ((char *) &serverAddress, 0,sizeof (serverAddress));
   
   serverAddress.sin_family      = AF_INET;
   serverAddress.sin_addr.s_addr = INADDR_ANY;
   serverAddress.sin_port        = htons (portNumber);
   
   // let the server start immediately after a previous shutdown
   int optionValue = 1;
   setsockopt (
      serverSocket,
      SOL_SOCKET,
      SO_REUSEADDR,
      &optionValue, 
      sizeof(int)
   );

   int bindSuccess = 
      bind (
         serverSocket, 
         (struct sockaddr *) &serverAddress,
         sizeof (serverAddress)
      );
   
   assert (bindSuccess >= 0);
   // if this assert fails wait a short while to let the operating 
   // system clear the port before trying again
   
   return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
static int waitForConnection (int serverSocket) {
   // listen for a connection
   const int serverMaxBacklog = 10;
   listen (serverSocket, serverMaxBacklog);
   
   // accept the connection
   struct sockaddr_in clientAddress;
   socklen_t clientLen = sizeof (clientAddress);
   int connectionSocket = 
      accept (
         serverSocket, 
         (struct sockaddr *) &clientAddress, 
         &clientLen
      );
   
   assert (connectionSocket >= 0);
   // error on accept
   
   return (connectionSocket);
}

static complex squareComplex(complex complexNum) {
    complex result;
    result.a = pow(complexNum.a,2) - pow(complexNum.b,2);
    result.b = 2*(complexNum.a * complexNum.b);
    return result;
}

static double modulus(complex complexNum) {
    double modulus;
    modulus = sqrt(pow(complexNum.a,2)+pow(complexNum.b,2));
    return modulus;
}

int escapeSteps(double x, double y) {
    complex point;
    point.a = x;
    point.b = y;
    int iterations = 1;
    while(iterations < 256 && modulus(point)< 2) {
        complex squared = squareComplex(point);
        point.a = squared.a + x; 
        point.b = squared.b + y; 
        iterations++;
    }
    return iterations;
}

complex pixelPosition(complex centrePixel, int zoom, int col, int row) {
   double distancePixel;
   complex pixelPosition;
   distancePixel = pow(2,-zoom); 
   pixelPosition.a = centrePixel.a - (SIZE * distancePixel/2) + col*distancePixel;
   pixelPosition.b = centrePixel.b - (SIZE * distancePixel/2) + row*distancePixel;
   printf("%lf,%lf\n", centrePixel.a, centrePixel.a);
   printf("%lf,%lf\n", pixelPosition.a, pixelPosition.a);
   return pixelPosition;
}

static void unitTest(void) {
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
