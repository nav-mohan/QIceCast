#include "aacencoder.h"

void AACEncoder::allocateMemory()
{
	m_handle = (HANDLE_AACENCODER*)malloc(sizeof(HANDLE_AACENCODER));   memset(m_handle,0,sizeof(HANDLE_AACENCODER));
    m_pcmBufDesc = (AACENC_BufDesc*)malloc(sizeof(AACENC_BufDesc));     memset(m_pcmBufDesc,0,sizeof(AACENC_BufDesc));
    m_pcmArgs = (AACENC_InArgs*)malloc(sizeof(AACENC_InArgs));          memset(m_pcmArgs,0,sizeof(AACENC_InArgs));
    m_aacBufDesc = (AACENC_BufDesc*)malloc(sizeof(AACENC_BufDesc));     memset(m_aacBufDesc,0,sizeof(AACENC_BufDesc));
    m_aacArgs = (AACENC_OutArgs*)malloc(sizeof(AACENC_OutArgs));        memset(m_aacArgs,0,sizeof(AACENC_OutArgs));
}

void AACEncoder::configEncoder()
{
	aacEncOpen(m_handle, 0, AAC_CHANNELS);
	aacEncoder_SetParam(*m_handle, AACENC_AOT, AAC_AOT);
	aacEncoder_SetParam(*m_handle, AACENC_SAMPLERATE, AAC_SAMPLERATE);
	aacEncoder_SetParam(*m_handle, AACENC_CHANNELMODE, AAC_CHANNELMODE);
	aacEncoder_SetParam(*m_handle, AACENC_CHANNELORDER, 1);
    aacEncoder_SetParam(*m_handle, AACENC_BITRATE, AAC_BITRATE);
	aacEncEncode(*m_handle, NULL, NULL, NULL, NULL);

}

void AACEncoder::initializePcmBuffer()
{
    m_pcmBuffer = (char*)malloc(PCM_BUFFERSIZE);
    m_pcmBufId = IN_AUDIO_DATA;
    m_pcmBufElSize = 2;
    m_pcmBufDesc->numBufs = 1;
    m_pcmBufDesc->bufs = (void**)&m_pcmBuffer;
    m_pcmBufDesc->bufferIdentifiers = &m_pcmBufId;
    m_pcmBufDesc->bufElSizes = &m_pcmBufElSize;
}

void AACEncoder::initializeAacBuffer()
{
    m_encodeBuffer = (uint8_t*)malloc(AAC_BUFFERSIZE);
    m_aacBufId = OUT_BITSTREAM_DATA;
    m_aacBufSize = AAC_BUFFERSIZE;
    m_aacBufElSize = 1;
    m_aacBufDesc->numBufs = 1;
    m_aacBufDesc->bufs = (void**)&m_encodeBuffer;
    m_aacBufDesc->bufferIdentifiers = &m_aacBufId;
    m_aacBufDesc->bufSizes = &m_aacBufSize;
    m_aacBufDesc->bufElSizes = &m_aacBufElSize;
}

void AACEncoder::initialize()
{
    allocateMemory();
    configEncoder();
    initializePcmBuffer();
    initializeAacBuffer();

}

void AACEncoder::encode(qint64 bytes_read)
{
    int input_bytes = (int)bytes_read;
    m_pcmArgs->numInSamples = input_bytes <= 0 ? -1 : input_bytes/2;
    m_pcmBufDesc->bufSizes = &input_bytes;
    aacEncEncode(*m_handle,m_pcmBufDesc,m_aacBufDesc,m_pcmArgs,m_aacArgs);
    // qDebug("encoder convert %d into %d ",input_bytes, m_aacArgs->numOutBytes);
    if(m_aacArgs->numOutBytes>0){
        // qDebug("Sending %d",m_aacArgs->numOutBytes);
        emit finished(const_cast<const char*>((char*)m_encodeBuffer), (qint64)m_aacArgs->numOutBytes);
    }
}