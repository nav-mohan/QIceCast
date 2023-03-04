#include "inputdevice.h"
#include <QDebug>

InputDevice::InputDevice(
    QAudioDeviceInfo audioDeviceInfo, 
    QAudioFormat audioFormat, 
    QCircularBuffer *circularBuffer
    )
{
    m_circularBuffer = circularBuffer;
    m_audioDeviceInfo = audioDeviceInfo;
    m_audioFormat = audioFormat;
}

InputDevice::~InputDevice()
{
    delete m_audioInput;
    delete m_circularBuffer;
}

void InputDevice::initialize()
{
    m_audioInput = new QAudioInput(m_audioDeviceInfo,m_audioFormat);
    startRecording();
}

void InputDevice::startRecording()
{
    m_audioInput->start(m_circularBuffer);
}

void InputDevice::stopRecording()
{
    m_audioInput->stop();
}