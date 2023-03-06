#if !defined(MPEGENCODER_H)
#define MPEGENCODER_H

#include <lame/lame.h>
#include <stdlib.h>
#include <QObject>
#include "encoder.h"
#include "circularbuffer.h"
#include "constants.h"
#include <QDebug>

class MpegEncoder : public Encoder{
Q_OBJECT
public:
    MpegEncoder(){};
    ~MpegEncoder()
    {
        lame_close(m_lgf);
    };

public slots:
    void initialize() override;
    void encode() override;

private:
    lame_global_flags *m_lgf;
    QCircularBuffer *m_inputBuffer;
};

#endif // MPEGENCODER_H
