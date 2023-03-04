#include "audiolevel.h"
#include <QDebug>
#include <QPainter>
#include <QMainWindow>
#include "constants.h"

AudioLevel::AudioLevel(QWidget *parent)
    :QWidget(parent)
{
    setMinimumHeight(20);
    setMinimumWidth(400);
    setMaximumHeight(40);
    setMaximumWidth(800);
}

AudioLevel::~AudioLevel() {
    qDebug() << "Deleting AudioLEvel";
}

void AudioLevel::registerInputBuffer(QCircularBuffer *circularBuffer, qint64 consumerID) {
    m_circularBuffer = circularBuffer;
    m_consumerID = consumerID;
    if(m_pcmBuffer != nullptr)
        delete m_pcmBuffer;
    m_pcmBuffer = (char*)malloc(PCM_BUFFERSIZE);
}

void AudioLevel::calculateLevels() {
    qint64 bytes_read = m_circularBuffer->readTail(m_pcmBuffer,PCM_BUFFERSIZE,m_consumerID);

    qint64 numSamples = bytes_read/2;
    qint16 max_left_value = 0;
    qint16 max_right_value = 0;
    for (int i = 0; i < numSamples; i+=2) {
        qint16 left_value = *reinterpret_cast<qint16*>(m_pcmBuffer + 2*i);
        qint16 right_value = *reinterpret_cast<qint16*>(m_pcmBuffer + 2*i + 2);
        max_left_value = qMax(max_left_value,left_value);
        max_right_value = qMax(max_right_value,right_value);
    }
    max_left_value = qMin(max_left_value,m_maxAmplitude);
    max_right_value = qMin(max_right_value,m_maxAmplitude);

    m_leftLevel     = qreal(max_left_value)/m_maxAmplitude;
    m_rightLevel    = qreal(max_right_value)/m_maxAmplitude;

    update();
}

void AudioLevel::paintEvent(QPaintEvent*) {
    // qDebug() << m_leftLevel << " " << m_rightLevel;

    QPainter painter(this);
    
    // boundary box
    painter.setPen(Qt::green); 
    painter.drawRect(QRect(
        painter.viewport().left(),
        painter.viewport().top(),
        painter.viewport().right(),
        painter.viewport().bottom()
    ));

    int pos_left = ((painter.viewport().right()) - (painter.viewport().left()))*m_leftLevel;
    int pos_right = ((painter.viewport().right()) - (painter.viewport().left()))*m_rightLevel;

// actual levels drawing
    // left channel
    painter.fillRect(
        painter.viewport().left(),
        painter.viewport().top(),
        pos_left,
        painter.viewport().height()/2,
        Qt::red
    );
    // right channel
    painter.fillRect(
        painter.viewport().left(),
        painter.viewport().top() + painter.viewport().height()/2,
        pos_right,
        painter.viewport().height()/2,
        Qt::yellow
    );
}