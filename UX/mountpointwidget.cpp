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
    m_mime(mime),QWidget(parent),
    m_consumerID(0),
    m_socketState(SOCKET_STATE_DISCONNECTED)
{
    qDebug() << "Creating MountpointWidget";
    m_ui->setupUi(this);
    m_ui->mountpointAddress->setText(m_url + ":" + m_port + m_endpoint);
    m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
    m_workerThread = new QThread();

// Socket
    m_socket = new Socket(m_url,m_port,m_endpoint,m_metadata,m_password,m_mime);
    m_socket->moveToThread(m_workerThread);
    connect(m_workerThread,SIGNAL(started()),m_socket,SLOT(initialize()));
    connect(m_socket,&Socket::stateChanged,this, &MountpointWidget::on_socketStateChanged);
    connect(m_workerThread,&QObject::destroyed,m_socket,&Socket::on_threadDestroyed);
    connect(this,&MountpointWidget::connectToHost,m_socket,&Socket::connectToHost);
    connect(this,&MountpointWidget::disconnectFromHost,m_socket,&Socket::disconnectFromHost);
    connect(this,&MountpointWidget::abort,m_socket,&Socket::abort);
    connect(m_socket,&Socket::socketErrored, this, &MountpointWidget::on_socketErrored);
    
// Encoder
    EncoderFactory *_factory = EncoderFactory::getInstance();
    m_encoder = _factory->make_encoder(m_mime.toUtf8().constData());
    m_encoder->moveToThread(m_workerThread);
    connect(m_workerThread,SIGNAL(started()),m_encoder,SLOT(initialize()));
    connect(this,&MountpointWidget::readyRead, m_encoder,&Encoder::encode);
    connect(m_encoder,&Encoder::finished,m_socket,&Socket::write);
    
    m_workerThread->start();
}

// slot
void MountpointWidget::on_socketErrored(QString errorString)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Socket Error");
    msgBox.setText(errorString);
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.setDefaultButton(QMessageBox::Close);
    int ret = msgBox.exec();
}

// slot
void MountpointWidget::on_startStopStream_clicked()
{
    switch (m_socketState)
    {
    case SOCKET_STATE_DISCONNECTED:
        qDebug() << "on_startStopStream_clicked connectToHost";
        emit connectToHost();
        break;
    case SOCKET_STATE_CONNECTED:
        qDebug() << "on_startStopStream_clicked disconnectFromHost";
        emit disconnectFromHost();
        break;
    default:
        qDebug() << "on_startStopStream_clicked abort";
        emit abort();
        break;
    }
}


// slot
void MountpointWidget::on_socketStateChanged(int socketState)
{
    m_socketState = socketState;
    switch (m_socketState)
    {
    case SOCKET_STATE_DISCONNECTED:
        qDebug() << "on_socketStateChanged UnconnectedState";
        m_ui->socketState->setText("Disconnected");
        m_ui->startStopStream->setText("Start");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
        break;

    case SOCKET_STATE_CONNECTING:
        qDebug() << "on_socketStateChanged ConnectingState";
        m_ui->socketState->setText("Connecting...");
        m_ui->startStopStream->setText("Abort");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
        
    case SOCKET_STATE_CONNECTED:
        qDebug() << "on_socketStateChanged ConnectedState";
        m_ui->socketState->setText("Connected");
        m_ui->startStopStream->setText("Abort");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__STOP);
        break;

    case SOCKET_STATE_AUTHENTICATED:
        qDebug() << "on_socketStateChanged Authenticated";
        m_ui->socketState->setText("Mountpoint Ready!");
        m_ui->startStopStream->setText("Stop");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__STOP);
        break;

    case SOCKET_STATE_AUTHENTICATING:
        qDebug() << "on_socketStateChanged Auhtenticating";
        m_ui->socketState->setText("Authenticating...");
        m_ui->startStopStream->setText("Stop");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__STOP);
        break;
    
    case SOCKET_STATE_CLOSING:
        qDebug() << "on_socketStateChanged ClosingSTate";
        m_ui->socketState->setText("Closing...");
        m_ui->startStopStream->setText("Await");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
    
    default:
        qDebug() << "on_socketStateChanged Default";
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
        return;
    }
    qint64 bytes_read = m_inputBuffer->readTail(m_encoder->m_pcmBuffer,PCM_BUFFERSIZE,m_consumerID);
    emit readyRead(bytes_read);
}

// slot
void MountpointWidget::on_closeMountpoint_clicked()
{
    if(m_workerThread!=nullptr && m_workerThread->isRunning())
        m_workerThread->quit();
    emit close_mountpoint(m_ui->mountpointAddress->text());
}
