#include "encoder.h"

Encoder::~Encoder()
{
    if(m_pcmBuffer)
        delete m_pcmBuffer;
    if(m_encodeBuffer)
        delete m_encodeBuffer;
}