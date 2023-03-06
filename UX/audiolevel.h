#if !defined(AUDIOLEVEL_H)
#define AUDIOLEVEL_H

#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include "circularbuffer.h"
#include <QMainWindow>

class AudioLevel : public QWidget {
Q_OBJECT
public:
    AudioLevel(QWidget *parent = nullptr);
    ~AudioLevel();

public slots:
    void calculateLevels();
    void registerInputBuffer(QCircularBuffer *circularBuffer, qint64 consumerID);

private:
    qreal m_leftLevel = 0.0;
    qreal m_rightLevel = 0.0;
    qint16 m_maxAmplitude = 32767;
    QCircularBuffer *m_circularBuffer;
    char *m_pcmBuffer = nullptr;
    qint64 m_consumerID;

protected:
    virtual void paintEvent(QPaintEvent* event);

};

#endif // AUDIOLEVEL_H
