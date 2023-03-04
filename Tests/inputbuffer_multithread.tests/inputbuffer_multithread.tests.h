#if !defined(INPUTBUFFER_MULTITHREAD_TESTS_H)
#define INPUTBUFFER_MULTITHREAD_TESTS_H

#include <QTest>
#include <QDebug>
#include "inputbuffer.h"
#include <QObject>
#include <QSignalSpy>
#include <QThread>

#include "producer.h"

class TestInputBufferMultipleThread : public QObject {
Q_OBJECT

public:
    TestInputBufferMultipleThread(QObject *parent = nullptr){}

private:
    InputBuffer *m_inputBuffer;
    QSignalSpy *m_signalSpy;
    Producer *m_producer;
    QThread *m_producerThread;

private slots:
    void initTestCase()
    {
        m_inputBuffer = new InputBuffer();
        m_inputBuffer->initialize();
        m_producerThread = new QThread();
        m_producer = new Producer(m_inputBuffer);
        m_producer->moveToThread(m_producerThread);
        QObject::connect(m_producerThread,SIGNAL(started()),m_producer,SLOT(startWork()));
        m_signalSpy = new QSignalSpy(m_inputBuffer, SIGNAL(readyRead()));
        m_producerThread->start();
    }
    
    void verify_readyReadSignal_count()
    {
        QTest::qWait(1);
        QCOMPARE(m_signalSpy->count(),10);
    }

    void verify_bytesWritten()
    {
            
    }
};



#endif // INPUTBUFFER_MULTITHREAD_TESTS_H
