#include "stylesheets.h"

#include <QDebug>

#include "mountpointwidget.h"
#include "encoder.h"
#include "encoderfactory.h"
#include "mpegencoder.h"
#include "aacencoder.h"
#include "ui_mountpointwidget.h"
#include <QThread>
#include "socket.h"
#include "constants.h"

MountpointWidget::~MountpointWidget()
{
    if(m_workerThread!=nullptr && m_workerThread->isRunning())
        m_workerThread->quit();
    m_encoder->deleteLater();
    m_socket->deleteLater();
    m_workerThread->deleteLater();
    delete m_ui;
}

MountpointWidget::MountpointWidget(
    QWidget* parent, 
    QString url,
    QString port,
    QString endpoint,
    QString password,
    QString metadata,
    QString mime,
    QCircularBuffer *inputBuffer) : 
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

    m_socket = new Socket(m_url,m_port,m_endpoint,m_metadata,m_password,m_mime);
    connect(m_workerThread,SIGNAL(started()),m_socket,SLOT(initialize()));
    connect(m_socket,SIGNAL(signal_socketState(int)),this,SLOT(update_socketState(int)));
    connect(m_workerThread,&QObject::destroyed,m_socket,&Socket::abort);
    m_socket->moveToThread(m_workerThread);

    EncoderFactory *_factory = EncoderFactory::getInstance();
    m_encoder = _factory->make_encoder(m_mime.toUtf8().constData());
    m_encoder->registerInputBuffer(inputBuffer);
    m_encoder->moveToThread(m_workerThread);
    QObject::connect(m_workerThread,SIGNAL(started()),m_encoder,SLOT(initialize()));
    QObject::connect(m_encoder,SIGNAL(finished(const char*, qint64)),m_socket,SLOT(transmit_buffer(const char*, qint64)));
    m_workerThread->start();
}

void MountpointWidget::on_startStopStream_clicked()
{
    QString socketState =  m_ui->socketState->text();
    if(socketState == "Disconnected")
        m_socket->connectToHost();
    else if(socketState=="Connected")
        m_socket->disconnectFromHost();
    else
        m_socket->abort();
}

void MountpointWidget::on_closeMountpoint_clicked()
{
    if(m_workerThread!=nullptr && m_workerThread->isRunning())
        m_workerThread->quit();
    emit close_mountpoint(m_ui->mountpointAddress->text());
}

void MountpointWidget::update_socketState(int socketState)
{
    switch (socketState)
    {
    case SOCKET_STATE_DISCONNECTED:
        m_ui->socketState->setText("Disconnected");
        m_ui->startStopStream->setText("Start");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__START);
        break;
    case SOCKET_STATE_CONNECTING:
        m_ui->socketState->setText("Connecting...");
        m_ui->startStopStream->setText("Abort");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
    case SOCKET_STATE_AUTHENTICATING:
        m_ui->socketState->setText("Authenticating...");
        m_ui->startStopStream->setText("Abort");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__AWAIT);
        break;
    case SOCKET_STATE_CONNECTED:
        m_ui->socketState->setText("Connected");
        m_ui->startStopStream->setText("Stop");
        m_ui->startStopStream->setStyleSheet(STARTSTOPSTREAM__STOP);
        m_encoder->encode();
        break;
    
    case SOCKET_STATE_CLOSING:
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
