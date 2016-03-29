#include "circularbuffer.h"

//Carson
void CircularBufferInitialize(CircularBuffer * circularBuffer, int maxSize, int elementSize) {
    circularBuffer->buffer = malloc(MaxSize * elementSize);
    if (circularBuffer->buffer == NULL) {
        return;
    }
    circularBuffer->bufferEnd = (char *)circularBuffer->buffer + maxSize * elementSize;
    circularBuffer->maxSize = maxSize;
    circularBuffer->length = 0;
    circularBuffer->elementSize = elementSize;
    circularBuffer->front = circularBuffer->buffer;
    circularBuffer->back = circularBuffer->buffer;
}

void CircularBufferFree(CircularBuffer * circularBuffer) {
    free(circularBuffer->buffer);
}

void CircularBufferPushBack(CircularBuffer * circularBuffer, const void *item) {
    /* Comment this out if we want the head to overwrite the tail */
    if (circularBuffer->length == circularBuffer->maxSize) {
        return;
    }

    memcpy(circularBuffer->front, item, circularBuffer->elementSize);
    circularBuffer->front = (char *)circularBuffer->front + circularBuffer->elementSize;
    if (circularBuffer->front == circularBuffer->bufferEnd) {
        circularBuffer->front = circularBuffer->buffer;
    }
    circularBuffer->length++;
}

void CircularBufferPop(CircularBuffer * circularBuffer, void * item) {
    if (circularBuffer->length == 0) {
        return;
    }
    memcpy(item, circularBuffer->back, circularBuffer->elementSize);
    circularBuffer->back = (char *)circularBuffer->back + circularBuffer->elementSize;
    if (circularBuffer->back == circularBuffer->bufferEnd) {
        circularBuffer->back = circularBuffer->buffer;
    }
    circularBuffer->length--;
}
