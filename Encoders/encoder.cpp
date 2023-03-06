#include "encoder.h"

Encoder::~Encoder()
{
    if(m_pcmBuffer)
        delete m_pcmBuffer;
    if(m_encodeBuffer)
        delete m_encodeBuffer;
}

void Encoder::registerInputBuffer(QCircularBuffer *inputBuffer)
{
    qDebug("Encoder Registering inputBUffer");
    m_inputBuffer=inputBuffer;
}
void Encoder::registerOutputSocket(Socket *outputSocket)
{
    qDebug("Encoder Registering outputSocket");
    m_outputSocket=outputSocket;
}