#if !defined(QCIRCULARBUFFER_H)
#define QCIRCULARBUFFER_H

#include <QDebug>

#include <QObject>
#include <QIODevice>
#include <QReadWriteLock>
#include <QMap>

class QCircularBuffer : public QIODevice {
Q_OBJECT
public:
    QCircularBuffer(qint64 bufferSize) : m_bufferSize(bufferSize), QIODevice(){};
    ~QCircularBuffer();

    QReadWriteLock m_lock;
    char *m_buffer;
    qint64 m_writeHead;
    qint64 m_bufferSize;
    QMap<qint64,qint64> m_readTails;
    QMap<qint64,qint64> m_validData;

public slots:
    void initialize();
    void registerConsumer(qint64 consumerID);
    void removeConsumer(qint64 consumerID);
    void resetBuffer();
    qint64 readTail(char *data, qint64 maxSize, qint64 consumerID);
    void closeBuffer();

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);
};

#endif // QCIRCULARBUFFER_H
