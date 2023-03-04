#if !defined(INPUTBUFFER_SINGLETHREAD_TESTS_H)
#define INPUTBUFFER_SINGLETHREAD_TESTS_H

#include <QTest>
#include <QDebug>
#include "inputbuffer.h"
#include <QObject>
#include <QSignalSpy>

class TestInputBufferSingleThread : public QObject {
Q_OBJECT

public:
    TestInputBufferSingleThread(QObject *parent = nullptr){}

private:
    InputBuffer *m_inputBuffer;
    QSignalSpy *m_signalSpy;

private slots:
    void initTestCase()
    {
        m_inputBuffer = new InputBuffer();
        QCOMPARE(m_inputBuffer->isOpen(),false);
        m_inputBuffer->initialize();
        QCOMPARE(m_inputBuffer->isOpen(),true);
        QCOMPARE(m_inputBuffer->isReadable(),true);
        QCOMPARE(m_inputBuffer->isWritable(),true);
        m_signalSpy = new QSignalSpy(m_inputBuffer, SIGNAL(readyRead()));
        QCOMPARE(true,m_signalSpy->isValid());
    }

// prepare data[PCM_BUFFERSIZE] and verify accurate write and read
    void verify_write_read_1()
    {
        char *write_buffer = (char*)malloc(PCM_BUFFERSIZE);
        memset(write_buffer,'w',PCM_BUFFERSIZE);
        int bytes_written = m_inputBuffer->write(const_cast<const char*>(write_buffer),PCM_BUFFERSIZE);
        QCOMPARE(bytes_written,PCM_BUFFERSIZE);
        QCOMPARE(m_signalSpy->count(),1);

        char *read_buffer = (char*)malloc(PCM_BUFFERSIZE);
        memset(read_buffer,'r',PCM_BUFFERSIZE);
        int bytes_read = m_inputBuffer->read(read_buffer,PCM_BUFFERSIZE);
        QCOMPARE(bytes_read,PCM_BUFFERSIZE);
        for (int i = 0; i < PCM_BUFFERSIZE; i++)
            QCOMPARE(read_buffer[i],'w');
    }

// prepare data[1024] and verify read_buffer is limited to 1024
    void verify_write_read_2()
    {
        char *write_buffer = (char*)malloc(1024);
        memset(write_buffer,'w',1024);
        int bytes_written = m_inputBuffer->write(const_cast<const char*>(write_buffer),1024);
        QCOMPARE(bytes_written,1024);
        QCOMPARE(m_signalSpy->count(),2);

        char *read_buffer = (char*)malloc(PCM_BUFFERSIZE);
        memset(read_buffer,'r',PCM_BUFFERSIZE);
        int bytes_read = m_inputBuffer->read(read_buffer,PCM_BUFFERSIZE);
        QCOMPARE(bytes_read,1024);
        for (int i = 0; i < 1024; i++)
            QCOMPARE(read_buffer[i],'w');
        for (int i = 1024; i < PCM_BUFFERSIZE; i++)
            QCOMPARE(read_buffer[i],'r');
    }

// prepare data[2*PCM_BUFFERSIZE] and verify latest data has been written while old data has been overwritten
    void verify_write_read_3()
    {
        char *write_buffer = (char*)malloc(2*PCM_BUFFERSIZE);
        memset(write_buffer,'o',PCM_BUFFERSIZE);
        memset(write_buffer+PCM_BUFFERSIZE,'n',PCM_BUFFERSIZE);
        int bytes_written = m_inputBuffer->write(const_cast<const char*>(write_buffer),2*PCM_BUFFERSIZE);
        QCOMPARE(bytes_written,PCM_BUFFERSIZE);
        QCOMPARE(m_signalSpy->count(),3);

        char *read_buffer = (char*)malloc(PCM_BUFFERSIZE);
        memset(read_buffer,'r',PCM_BUFFERSIZE);
        int bytes_read = m_inputBuffer->read(read_buffer,PCM_BUFFERSIZE);
        QCOMPARE(bytes_read,PCM_BUFFERSIZE);
        for (int i = 0; i < PCM_BUFFERSIZE; i++)
            QCOMPARE(read_buffer[i],'n');
    }

// prepare data[1.5*PCM_BUFFERSIZE] and verify latest data has been written while old data has been overwritten
    void verify_write_read_4()
    {
        char *write_buffer = (char*)malloc(PCM_BUFFERSIZE + PCM_BUFFERSIZE/2);
        memset(write_buffer,'o',PCM_BUFFERSIZE);
        memset(write_buffer+PCM_BUFFERSIZE,'n',PCM_BUFFERSIZE/2);
        int bytes_written = m_inputBuffer->write(const_cast<const char*>(write_buffer),(PCM_BUFFERSIZE+PCM_BUFFERSIZE/2));
        QCOMPARE(bytes_written,PCM_BUFFERSIZE);
        QCOMPARE(m_signalSpy->count(),4);

        char *read_buffer = (char*)malloc(PCM_BUFFERSIZE);
        memset(read_buffer,'r',PCM_BUFFERSIZE);
        int bytes_read = m_inputBuffer->read(read_buffer,PCM_BUFFERSIZE);
        QCOMPARE(bytes_read,PCM_BUFFERSIZE);
        for (int i = 0; i < PCM_BUFFERSIZE/2; i++)
            QCOMPARE(read_buffer[i],'o');
        for (int i = PCM_BUFFERSIZE/2; i < PCM_BUFFERSIZE; i++)
            QCOMPARE(read_buffer[i],'n');
    }
};

#endif // INPUTBUFFER_SINGLETHREAD_TESTS_H
