#include "circularbuffer.h"

//Carson
bool CircularBufferInitialize(CircularBuffer * circularBuffer, int maxLength, int elementLength) {
    circularBuffer->buffer = malloc(MaxLength * elementLength);
    if (circularBuffer->buffer == NULL) {
        return false;
    }
    circularBuffer->bufferEnd = (char *)circularBuffer->buffer + maxLength * elementLength;
    circularBuffer->maxLength = maxLength;
    circularBuffer->length = 0;
    circularBuffer->elementLength = elementLength;
    circularBuffer->front = circularBuffer->buffer;
    circularBuffer->back = circularBuffer->buffer;
    return true;
}

void CircularBufferFree(CircularBuffer * circularBuffer) {
    free(circularBuffer->buffer);
}

void CircularBufferPushBack(CircularBuffer * circularBuffer, const void *item) {
    /* Comment this out if we want the head to overwrite the tail */
    if (circularBuffer->length == circularBuffer->maxLength) {
        return;
    }

    memcpy(circularBuffer->front, item, circularBuffer->elementLength);
    circularBuffer->front = (char *)circularBuffer->front + circularBuffer->elementLength;
    if (circularBuffer->front == circularBuffer->bufferEnd) {
        circularBuffer->front = circularBuffer->buffer;
    }
    circularBuffer->length++;
}

void CircularBufferPop(CircularBuffer * circularBuffer, void * item) {
    if (circularBuffer->length == 0) {
        return;
    }
    memcpy(item, circularBuffer->back, circularBuffer->elementLength);
    circularBuffer->back = (char *)circularBuffer->back + circularBuffer->elementLength;
    if (circularBuffer->back == circularBuffer->bufferEnd) {
        circularBuffer->back = circularBuffer->buffer;
    }
    circularBuffer->length--;
}
