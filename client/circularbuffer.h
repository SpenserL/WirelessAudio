#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <stdlib.h>
#include <string.h>
#include <QBuffer>

#define BUFFERSIZE 512
#define CIRCULARBUFFERSIZE 100000

//Carson
class CircularBuffer {
public:
    int	maxLength;
    int	length;
    int	elementLength;
    void* buffer;
    void* bufferEnd;
    void* front;
    void* back;
    QObject* parent;

    CircularBuffer(int maxLength, int elementLength, QObject* parent);
    ~CircularBuffer();
    bool pushBack(void* item);
    bool pop(QBuffer* buffer);
};



#endif // CIRCULARBUFFER_H
