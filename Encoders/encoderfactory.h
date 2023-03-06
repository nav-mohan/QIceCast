#if !defined(ENCODERFACTORY_H)
#define ENCODERFACTORY_H
#include <string>
#include "encoder.h"
#include "mpegencoder.h"
#include "aacencoder.h"
#include <QDebug>

class Encoder;

class EncoderFactory {

public:
    static EncoderFactory *getInstance();
    Encoder *make_encoder(const char *mime);

private:
    static EncoderFactory *_factory;
    const char m_mimeTypeMPEG[11] = "audio/mpeg";
    const char m_mimeTypeAAC[10] = "audio/aac";
};

#endif // ENCODERFACTORY_H