#include "ui_mainwindow.h"
#include "mainwindow.h"

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

void MainWindow::prepareAudioDevice() {
    m_audioDeviceInfo = m_ui->deviceOptions->currentData().value<QAudioDeviceInfo>();
    m_audioFormat.setSampleRate(PCM_SAMPLERATE);
    m_audioFormat.setChannelCount(PCM_CHANNELS);
    m_audioFormat.setSampleSize(PCM_BITDEPTH);
    m_audioFormat.setCodec("audio/pcm");
    m_audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    m_audioFormat.setSampleType(PCM_SAMPLETYPE);
}

void MainWindow::on_deviceOptions_currentIndexChanged(int index) {
    prepareAudioDevice();

    if(m_inputThread == nullptr)
        m_inputThread = new QThread();
    
    if(m_inputThread->isRunning())
        m_inputThread->quit();

    m_inputBuffer = new QCircularBuffer(PCM_BUFFERSIZE);
    m_inputBuffer->moveToThread(m_inputThread);
    connect(m_inputThread,SIGNAL(started()),m_inputBuffer,SLOT(initialize()));
    connect(m_inputThread,SIGNAL(finished()),m_inputBuffer,SLOT(closeBuffer()));

    if(m_inputDevice != nullptr) {
        emit stopRecording();
        m_inputDevice->deleteLater();
    }

    m_inputDevice = new InputDevice(m_audioDeviceInfo, m_audioFormat,m_inputBuffer);
    m_inputDevice->moveToThread(m_inputThread);
    connect(m_inputThread,SIGNAL(started()),m_inputDevice,SLOT(initialize()));
    connect(this,SIGNAL(startRecording()),m_inputDevice,SLOT(startRecording()));
    connect(this,SIGNAL(stopRecording()),m_inputDevice,SLOT(stopRecording()));
    
    m_inputThread->start();

    QMapIterator <QString,MountpointWidget*> mpwItr(m_mountpoints);
    while(mpwItr.hasNext()) {
        mpwItr.next();
        qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
        m_inputBuffer->registerConsumer(timestamp);
        mpwItr.value()->registerInputBuffer(m_inputBuffer,timestamp);
        connect(m_inputBuffer,&QIODevice::readyRead,mpwItr.value(),&MountpointWidget::on_readyRead);
    }

    qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    m_ui->audioLevel->registerInputBuffer(m_inputBuffer,timestamp);
    m_inputBuffer->registerConsumer(timestamp);
    connect(m_inputBuffer,&QIODevice::readyRead,m_ui->audioLevel,&AudioLevel::calculateLevels);

    emit startRecording();

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
            mime
        );
        
        if(m_inputBuffer) {
            qDebug() << "Registering m_inputBuffer to mountpointwidget's encoder";
            qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
            m_inputBuffer->registerConsumer(timestamp);
            newMountpointWidget->registerInputBuffer(m_inputBuffer,timestamp);
            connect(m_inputBuffer,&QIODevice::readyRead,newMountpointWidget,&MountpointWidget::on_readyRead);
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
