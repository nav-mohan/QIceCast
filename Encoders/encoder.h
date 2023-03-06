
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
    Encoder() {qDebug("construct encoer");};
    virtual ~Encoder();
    QCircularBuffer *m_inputBuffer;
    Socket *m_outputSocket;
    char *m_pcmBuffer = nullptr;
    unsigned char *m_encodeBuffer = nullptr;

public slots:    
    virtual void initialize(){};
    virtual void encode(qint64 bytes_read){};
    void registerOutputSocket(Socket *outputSocket);

signals:
    void finished(const char*, qint64);
};

#endif // ENCODER_H
