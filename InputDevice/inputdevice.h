#if !defined(INPUTDEVICE_H)
#define INPUTDEVICE_H

#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include "circularbuffer.h"
#include <QObject>

class InputDevice : public QObject {
Q_OBJECT

public:
    InputDevice(
        QAudioDeviceInfo, 
        QAudioFormat, 
        QCircularBuffer*
        );
    ~InputDevice();

public slots:
    void initialize();
    void startRecording();
    void stopRecording();

private:
    QAudioInput *m_audioInput;
    QAudioDeviceInfo m_audioDeviceInfo;
    QAudioFormat m_audioFormat;
    QCircularBuffer *m_circularBuffer;
};

#endif // INPUTDEVICE_H
