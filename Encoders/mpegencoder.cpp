#include "mpegencoder.h"
#include "logger.h"


namespace mpeg
{
    Logger *loggerinstance = Logger::getInstance();
} 

void MpegEncoder::initialize()
{
    mpeg::loggerinstance->write_log(std::string("Initializing MPEG-Encoder"));
 
    m_lgf = lame_init();
    lame_set_mode(m_lgf,LAME_MODE);
    lame_set_in_samplerate(m_lgf,LAME_SAMPLERATE);
    lame_set_brate(m_lgf,LAME_BITRATE);
    lame_set_num_channels(m_lgf,LAME_CHANNELS);
    lame_init_params(m_lgf);
    m_pcmBuffer = (char*)malloc(PCM_BUFFERSIZE);
    m_encodeBuffer = (unsigned char*)malloc(LAME_BUFFERSIZE);
}

void MpegEncoder::encode(qint64 bytes_read)
{
    short *pcm_stereo = reinterpret_cast<short*>(m_pcmBuffer);
    short pcm_left[bytes_read/4];
    short pcm_right[bytes_read/4];

    for (int i = 0; i < bytes_read/4; i++)
    {
        pcm_left[i] = *(pcm_stereo + 2*i);
        pcm_right[i] = *(pcm_stereo + 2*i + 1);
    }

    int bytes_encoded = lame_encode_buffer(
        m_lgf,
        pcm_left,
        pcm_right,
        bytes_read/4,
        m_encodeBuffer,
        LAME_BUFFERSIZE
    );
    // qDebug() << "Encoded " << bytes_read << " bytes into " << bytes_encoded << " bytes";
    emit finished(const_cast<const char*>((char*)m_encodeBuffer), bytes_encoded);
}