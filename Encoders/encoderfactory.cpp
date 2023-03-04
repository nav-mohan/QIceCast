#include <string>
#include "encoderfactory.h"
#include "encoder.h"
#include "mpegencoder.h"
#include "aacencoder.h"
#include <QDebug>

EncoderFactory *EncoderFactory::getInstance()
{
    if(!_factory)
        _factory = new EncoderFactory;
    return _factory;
}

Encoder *EncoderFactory::make_encoder(const char *mime)
{
    if(strcmp(mime,m_mimeTypeMPEG)==0)
        return new MpegEncoder();
    if(strcmp(mime,m_mimeTypeAAC)==0)
        return new AACEncoder();
    return new Encoder();
}

// the singleton instance of the factory
EncoderFactory *EncoderFactory::_factory = nullptr;