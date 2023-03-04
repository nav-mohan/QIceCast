#if !defined(AACENCODER_H)
#define AACENCODER_H

#include <fdk-aac/aacenc_lib.h>
#include <QObject>
#include "encoder.h"

class AACEncoder : public Encoder{
Q_OBJECT
public:
    AACEncoder(){};
    ~AACEncoder()
    {
        free(m_pcmBufDesc);
        free(m_aacBufDesc);
        free(m_pcmArgs);
        free(m_aacArgs);
    }

public slots:
    void initialize() override;
    void encode() override;
    void state_change(uint8_t socketState);

private:
    HANDLE_AACENCODER   *m_handle;
    AACENC_BufDesc      *m_pcmBufDesc;
    AACENC_InArgs       *m_pcmArgs;
    AACENC_BufDesc      *m_aacBufDesc;
    AACENC_OutArgs      *m_aacArgs;
    
    int m_pcmBufId;
    int m_pcmBufElSize;
    uint32_t m_pcmBufSize;

    int m_aacBufId;
    int m_aacBufElSize;
    int m_aacBufSize;
    
    uint8_t m_socketState;
    
    void allocateMemory();
    void configEncoder();
    void initializePcmBuffer();
    void initializeAacBuffer();

};

#endif // AACENCODER_H
