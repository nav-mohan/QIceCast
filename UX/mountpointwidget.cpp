#include "mountpointwidget.h"
#include "ui_mountpointwidget.h"

MountpointWidget::~MountpointWidget()
{
    if(m_workerThread!=nullptr && m_workerThread->isRunning())
        m_workerThread->quit();
    m_encoder->deleteLater();
    m_socket->deleteLater();
    m_workerThread->deleteLater();
    delete m_ui;
}

// slot
MountpointWidget::MountpointWidget(
    QWidget* parent, 
    QString url,
    QString port,
    QString endpoint,
    QString password,
    QString metadata,
    QString mime
    ) : 
    m_ui(new Ui::MountpointWidget), 
    m_url(url),
    m_port(port),
    m_endpoint(endpoint),
    m_password(password),
    m_metadata(metadata),
    m_mime(mime),QWidget(parent)
{
    qDebug() << "Creating MountpointWidget";
    m_ui->setupUi(this);
    m_ui->mountpointAddress->setText(m_url + ":" + m_port + m_endpoint);
    m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
    m_workerThread = new QThread();

// Socket
    m_socket = new Socket(m_url,m_port,m_endpoint,m_metadata,m_password,m_mime);
    // m_socket->moveToThread(m_workerThread);
    connect(m_workerThread,SIGNAL(started()),m_socket,SLOT(initialize()));
    connect(m_socket,&Socket::stateChanged,this, &MountpointWidget::on_socketStateChanged);
    connect(m_workerThread,&QObject::destroyed,m_socket,&Socket::on_threadDestroyed);

// Encoder
    EncoderFactory *_factory = EncoderFactory::getInstance();
    m_encoder = _factory->make_encoder(m_mime.toUtf8().constData());
    m_encoder->moveToThread(m_workerThread);
    connect(m_workerThread,SIGNAL(started()),m_encoder,SLOT(initialize()));
    connect(this,&MountpointWidget::readyRead, m_encoder,&Encoder::encode);
    
    m_workerThread->start();
}

// slot
void MountpointWidget::on_startStopStream_clicked()
{
    QAbstractSocket::SocketState socketState =  m_socket->m_tcpSocket->state();
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        m_socket->m_tcpSocket->connectToHost(m_url.toUtf8().data(),m_port.toInt());
        break;
    case QAbstractSocket::ConnectedState:
        m_socket->m_tcpSocket->disconnectFromHost();
    default:
        m_socket->m_tcpSocket->abort();
        break;
    }
}

// slot
void MountpointWidget::on_closeMountpoint_clicked()
{
    if(m_workerThread!=nullptr && m_workerThread->isRunning())
        m_workerThread->quit();
    emit close_mountpoint(m_ui->mountpointAddress->text());
}

// slot
void MountpointWidget::on_socketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "Socket State : " << socketState;
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        m_ui->socketState->setText("Disconnected");
        m_ui->startStopStream->setText("Start");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
        break;
    case QAbstractSocket::ConnectingState:
        m_ui->socketState->setText("Connecting...");
        m_ui->startStopStream->setText("Abort");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
    case QAbstractSocket::ConnectedState:
        m_ui->socketState->setText("Connected");
        m_ui->startStopStream->setText("Stop");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__STOP);
        break;
    
    case QAbstractSocket::ClosingState:
        m_ui->socketState->setText("Closing...");
        m_ui->startStopStream->setText("Await");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
    
    default:
        m_ui->socketState->setText("Disconnected");
        m_ui->startStopStream->setText("Start");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
        break;
    }

}


void MountpointWidget::registerInputBuffer(QCircularBuffer *inputBuffer,qint64 timestamp)
{
    m_inputBuffer = inputBuffer;
    m_consumerID = timestamp;
}

void MountpointWidget::on_readyRead()
{
    if(m_inputBuffer==nullptr){
        qDebug() << "undefined m_inputBuffer";
        return;
    }
    else {
        qDebug() << "m_inputBuffer!";
    }

    qint64 bytes_read = m_inputBuffer->readTail(m_encoder->m_pcmBuffer,PCM_BUFFERSIZE,m_consumerID);
    emit readyRead(bytes_read);
}