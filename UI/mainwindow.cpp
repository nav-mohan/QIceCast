// DEBUG
#include <QDebug>

// UI
#include "mainwindow.h"
#include "mountpointsettingdialog.h"
#include "audiolevel.h"
#include <QMessageBox>
#include "mountpointwidget.h"

// Layout
#include "ui_mainwindow.h"

// Constants
#include "constants.h"

// Audio Stuff
#include <QAudio>
#include <QAudioDeviceInfo>
#include "circularbuffer.h"
#include "inputdevice.h"
#include "encoder.h"


#include <QThread>
#include <string>
#include <QWidget>
#include <QDateTime>

MainWindow::MainWindow() : 
    QMainWindow(),
    m_ui(new Ui::MainWindow) 
{
    m_ui->setupUi(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    m_ui->deviceOptions->addItem(defaultDeviceInfo.deviceName(), QVariant::fromValue(defaultDeviceInfo));
    for (auto &deviceInfo: QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_ui->deviceOptions->addItem(deviceInfo.deviceName(), QVariant::fromValue(deviceInfo));
    }
}

MainWindow::~MainWindow(){}

void MainWindow::on_deviceOptions_currentIndexChanged(int index) {
    QAudioDeviceInfo audioDeviceInfo = m_ui->deviceOptions->currentData().value<QAudioDeviceInfo>();
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(PCM_SAMPLERATE);
    audioFormat.setChannelCount(PCM_CHANNELS);
    audioFormat.setSampleSize(PCM_BITDEPTH);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(PCM_SAMPLETYPE);

    // if the thread doesnt exist, create it first
    if(m_inputThread == nullptr)
        m_inputThread = new QThread();
    
    // if the thread exists and is running, then stop it
    if(m_inputThread->isRunning())
        m_inputThread->quit();

    m_inputBuffer = new QCircularBuffer(PCM_BUFFERSIZE);
    m_inputBuffer->moveToThread(m_inputThread);
    connect(m_inputThread,SIGNAL(started()),m_inputBuffer,SLOT(initialize()));
    connect(m_inputThread,SIGNAL(finished()),m_inputBuffer,SLOT(closeBuffer()));

    // if the inputDevice exists, stop it and delete it 
    if(m_inputDevice != nullptr) {
        m_inputDevice->stopRecording();
        delete m_inputDevice;
    }
    m_inputDevice = new InputDevice(audioDeviceInfo, audioFormat,m_inputBuffer);
    m_inputDevice->moveToThread(m_inputThread);
    connect(m_inputThread,SIGNAL(started()),m_inputDevice,SLOT(initialize()));

    QMapIterator <QString,MountpointWidget*> mpwItr(m_mountpoints);
    while(mpwItr.hasNext()) {
        mpwItr.next();
        mpwItr.value()->m_encoder->registerInputBuffer(m_inputBuffer);
        connect(m_inputBuffer,&QIODevice::readyRead,mpwItr.value(),&MountpointWidget::readyRead);
    }

    qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    m_ui->audioLevel->registerInputBuffer(m_inputBuffer,timestamp);
    m_inputBuffer->registerConsumer(timestamp);
    connect(m_inputBuffer,&QIODevice::readyRead,m_ui->audioLevel,&AudioLevel::calculateLevels);

    m_inputThread->start();
}

void MainWindow::on_addMountpoint_clicked() {
    m_mountpointSettingDialog.reset(new MountpointSettingDialog());
    QObject::connect(
        m_mountpointSettingDialog.get(), SIGNAL(finished(int)),
        this, SLOT(mountpoint_settings_received(int))
    );
    m_mountpointSettingDialog.get()->exec();
}

void MainWindow::display_error(QString messageWindowTitle, QString messageText, QString informativeText) {
    qDebug("ERROR!");
    QMessageBox msgBox;
    msgBox.setWindowTitle(messageWindowTitle);
    msgBox.setText(messageText);
    msgBox.setInformativeText(informativeText);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    int ret = msgBox.exec();
}

void MainWindow::mountpoint_settings_received(int result) {
    if(result) {
        qDebug("Getting setting value");
        QString url                 = m_mountpointSettingDialog.get()->m_url;
        QString port                = m_mountpointSettingDialog.get()->m_port;
        QString endpoint            = m_mountpointSettingDialog.get()->m_endpoint;
        QString password            = m_mountpointSettingDialog.get()->m_password;
        QString mime                = m_mountpointSettingDialog.get()->m_mime;
        QString metadata            = m_mountpointSettingDialog.get()->m_metadata;
        QString mountpointAddress   = url+":"+port+endpoint;
        if(m_mountpoints.contains(mountpointAddress)) {
            display_error("ERROR","Duplicate Mountpoint","A mountpoint with the same Url, Port, and Endpoint is already configured here");
            return ;
        }

        QListWidgetItem *newListWidgetItem = new QListWidgetItem();

        MountpointWidget *newMountpointWidget = new MountpointWidget(
            this,
            url,
            port,
            endpoint,
            password,
            metadata,
            mime,
            m_inputBuffer
        );
        
        if(m_inputBuffer) {
            qDebug() << "Registering m_inputBuffer to mountpointwidget's encoder";
            newMountpointWidget->m_encoder->registerInputBuffer(m_inputBuffer);
            connect(m_inputBuffer,&QIODevice::readyRead,newMountpointWidget,&MountpointWidget::readyRead);
        }

        newListWidgetItem->setSizeHint(newMountpointWidget->sizeHint());
        QObject::connect(newMountpointWidget,SIGNAL(close_mountpoint(QString)),this,SLOT(close_mountpoint(QString)));
        QObject::connect(newMountpointWidget,SIGNAL(announce_error(QString,QString,QString)),this,SLOT(display_error(QString,QString,QString)));
        m_ui->mountpointList->addItem(newListWidgetItem);
        m_ui->mountpointList->setItemWidget(newListWidgetItem,newMountpointWidget);
        m_mountpoints.insert(mountpointAddress,newMountpointWidget);
    }
    m_mountpointSettingDialog.get()->close();
    m_mountpointSettingDialog.reset(nullptr);
}

void MainWindow::close_mountpoint(QString mountpointAddress) {
    qDebug() << "Closing " << mountpointAddress;
    for (int i = 0; i < m_ui->mountpointList->count(); ++i) {
        auto item = m_ui->mountpointList->item(i);
        auto mountpointWidget = qobject_cast<MountpointWidget*>(m_ui->mountpointList->itemWidget(item));
        if (mountpointWidget->m_url+":"+mountpointWidget->m_port+mountpointWidget->m_endpoint == mountpointAddress) {
            delete m_ui->mountpointList->takeItem(i);//update sourceListWidget
            m_mountpoints.remove(mountpointAddress);// update QMap of SourceWidgets
            break;
        }
    }
}
