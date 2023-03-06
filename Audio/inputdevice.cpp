#include "inputdevice.h"

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
    m_audioInput->stop();
    m_audioInput->deleteLater();
    m_circularBuffer->deleteLater();
}

void InputDevice::initialize()
{
    m_audioInput = new QAudioInput(m_audioDeviceInfo,m_audioFormat);
}

void InputDevice::startRecording()
{
    m_audioInput->start(m_circularBuffer);
}

void InputDevice::stopRecording()
{
    m_audioInput->stop();
}