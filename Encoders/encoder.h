
#if !defined(ENCODER_H)
#define ENCODER_H

#include <stdio.h>
#include "circularbuffer.h"
#include "socket.h"
#include <QObject>

class QCircularBuffer;
class Socket;

class Encoder : public QObject {
Q_OBJECT

public:
    Encoder() {
        qDebug("construct encoer");
    };
    virtual ~Encoder();

public slots:    
    virtual void initialize(){};
    virtual void encode(){};
    void registerInputBuffer(QCircularBuffer *inputBuffer);
    void registerOutputSocket(Socket *outputSocket);

signals:
    void finished(const char*, qint64);

protected:
    QCircularBuffer *m_inputBuffer;
    Socket *m_outputSocket;
    char *m_pcmBuffer = nullptr;
    unsigned char *m_encodeBuffer = nullptr;
};

#endif // ENCODER_H
