#if !defined(INPUTBUFFER_H)
#define INPUTBUFFER_H

#include <QIODevice>
#include <QObject>
#include "constants.h"
#include<QReadWriteLock>

class InputBuffer : public QIODevice {
Q_OBJECT
public:
    InputBuffer():QIODevice(){};
    ~InputBuffer(){};

public slots:
    void initialize();
    void closeBuffer();
    void resetBuffer();

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

signals:
    void bufferReady(char *data, qint64 maxSize);

private:
    char *m_buffer;
    quint64 m_bufSize = 0;    
    QReadWriteLock m_lock;
};

#endif // INPUTBUFFER_H
