#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <stdlib.h>
#include <string.h>

//Carson
struct CircularBuffer {
    int	maxLength;
    int	length;
    int	elementLength;
    void* buffer;
    void* bufferEnd;
    void* front;
    void* back;
};

bool CircularBufferInitialize(CircularBuffer* circularBuffer, int maxLength, int elementLength);
void CircularBufferFree(CircularBuffer* circularBuffer);
void CircularBufferPushBack(CircularBuffer* circularBuffer, const void* item);
void CircularBufferPop(CircularBuffer* circularBuffer, void* item);

#endif // CIRCULARBUFFER_H
