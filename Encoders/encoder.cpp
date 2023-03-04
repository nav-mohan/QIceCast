#include "encoder.h"
#include <stdio.h>
#include "circularbuffer.h"
#include "socket.h"

Encoder::~Encoder()
{
    if(m_pcmBuffer)
        delete m_pcmBuffer;
    if(m_encodeBuffer)
        delete m_encodeBuffer;
}

void Encoder::registerInputBuffer(QCircularBuffer *inputBuffer)
{
    m_inputBuffer=inputBuffer;
}
void Encoder::registerOutputSocket(Socket *outputSocket)
{
    m_outputSocket=outputSocket;
}