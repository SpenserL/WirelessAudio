#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <stdlib.h>
#include <string.h>

//Carson
struct CircularBuffer
{
    int	maxSize;
    int	length;
    int	elementSize;
    void * buffer;
    void * bufferEnd;
    void * front;
    void * back;
};

void CircularBufferInitialize(CircularBuffer * circularBuffer, int maxSize, int elementSize);
void CircularBufferFree(CircularBuffer * circularBuffer);
void CircularBufferPushBack(CircularBuffer * circularBuffer, const void * item);
void CircularBufferPop(CircularBuffer * circularBuffer, void * item);

#endif // CIRCULARBUFFER_H
