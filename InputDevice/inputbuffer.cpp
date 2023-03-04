#include "inputbuffer.h"

#include <QDebug>

void InputBuffer::initialize()
{
    qDebug("initialized input buffer");
    m_buffer = (char*)malloc(PCM_BUFFERSIZE);
    open(QIODevice::ReadWrite);
}

void InputBuffer::closeBuffer()
{
    m_lock.unlock();
    close();
}

void InputBuffer::resetBuffer()
{
    memset(m_buffer,0,PCM_BUFFERSIZE);
}

qint64 InputBuffer::readData(char *data, qint64 maxSize)
{
    // resetBuffer(); this works! it mutes the microphone!
    qDebug("readData %lld",maxSize);
    m_lock.lockForRead();
    qDebug("READ LOCK APPLIED");
    if(m_bufSize)
    {
        if(maxSize > m_bufSize)
        {
            memcpy(data,m_buffer,m_bufSize);
            qDebug("Copying %lld",m_bufSize);
            m_lock.unlock();
            return m_bufSize;
        }
        if(m_bufSize >= maxSize)
        {
            memcpy(data,m_buffer+m_bufSize-maxSize,maxSize);
            m_lock.unlock();
            return maxSize;
        }
    }
    m_lock.unlock();
    qDebug("READ LOCK REMOVED");
    return 0;
}

qint64 InputBuffer::writeData(const char *data, qint64 maxSize)
{
    qint64 start_pos = 0;
    qint64 bytes_written = maxSize;
    m_lock.lockForWrite();
    if(maxSize > PCM_BUFFERSIZE)
    {
        start_pos = maxSize-PCM_BUFFERSIZE;
        bytes_written = PCM_BUFFERSIZE;
    }
    memcpy(m_buffer,data+start_pos,bytes_written);
    m_bufSize = bytes_written;
    m_lock.unlock();
    emit QIODevice::readyRead();
    return bytes_written;
}