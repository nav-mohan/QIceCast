#if !defined(PRODUCER_H)
#define PRODUCER_H
#include "inputbuffer.h"
#include "constants.h"
#include <QDebug>

class Producer : public QObject {
Q_OBJECT
private:
    InputBuffer *m_inputBuffer;

public:
    Producer(InputBuffer *inputbuffer) : m_inputBuffer(inputbuffer){}
    ~Producer(){}

public slots:
    void startWork()
    {
        char *data = (char*)malloc(PCM_BUFFERSIZE);
        qDebug("starting work");
        for (int i = 0; i < 10; i++)
        {
            memset(data,0,PCM_BUFFERSIZE);
            int bytes_written = m_inputBuffer->write(data,PCM_BUFFERSIZE);
        }
    }
        
};

#endif // PRODUCER_H