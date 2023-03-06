#include "socketutils.h"
#include "socket.h"

Socket::Socket (
    QString url,
    QString port,
    QString endpoint,
    QString metadata,
    QString password,
    QString mime
    ) :
    m_url(url),
    m_port(port),
    m_endpoint(endpoint),
    m_metadata(metadata),
    m_password(password),
    m_mime(mime)
    {}
Socket::~Socket()
{
    qDebug() << "Destroyinh Socket";
    m_tcpSocket->abort();
    m_tcpSocket->deleteLater();
    free(m_readBuffer);
}

void Socket::initialize()
{
    qDebug() << "Initializing Socket";
    m_tcpSocket = new QTcpSocket();
    m_tcpSocket->open(QIODevice::ReadWrite);
    m_readBuffer = (char*)malloc(MOUNTPOINT_READ_BUFFERSIZE);
    m_tcpSocket->setReadBufferSize(MOUNTPOINT_READ_BUFFERSIZE);
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),this, SLOT(on_errorOccurred(QAbstractSocket::SocketError)));
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    // connect(m_tcpSocket,&QAbstractSocket::stateChanged,this, &Socket::stateChanged);
    connect(m_tcpSocket,&QAbstractSocket::connected,this,&Socket::authenticate);
}

void Socket::authenticate()
{
    prepareAuthHeader();
    m_tcpSocket->write(m_authHeader.toUtf8().constData(),(qint64)(m_authHeader.size()));
}

void Socket::on_stateChanged(QAbstractSocket::SocketState socketState) {
    emit stateChanged(socketState);
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        qDebug("Disconnected!");
        m_authHeader = "";
        m_tcpSocket->flush();
        break;
    case QAbstractSocket::ConnectedState:
        qDebug("Connected!");
        prepareAuthHeader();
        m_tcpSocket->write(m_authHeader.toUtf8().constData(),(qint64)(m_authHeader.size()));
    default:
        m_tcpSocket->abort();
        break;
    }
}

void Socket::on_errorOccurred(QAbstractSocket::SocketError)
{   
    qDebug("TCPSOCKET ERROR!!!");
    qDebug() << m_tcpSocket->errorString();
}

void Socket::prepareAuthHeader()
{
    qDebug("PrepareAuthHeader!");
    QString unhashed_http_auth = "source:"+m_password;
    char *hashed_http_auth = util_base64_encode(unhashed_http_auth.toUtf8().data());
    m_authHeader = "PUT " + m_endpoint + " HTTP/1.1\n";
    m_authHeader += "User-Agent: Lavf/59.27.100\n";
    m_authHeader += "Accept: */*\n";
    m_authHeader += "Expect: 100-continue\n";
    m_authHeader += "Connection: close\n";
    m_authHeader += "Host: " + m_url + ":" + m_port + "\n";
    m_authHeader += "Content-Type: " + m_mime + "\n";
    m_authHeader += "Icy-Metadata: " + m_metadata + "\n";
    m_authHeader += "Ice-Public: 0\n";
    m_authHeader += "Authorization: Basic ";
    m_authHeader.append(hashed_http_auth);
    m_authHeader += "\n\n";
    qDebug() << "authHeader ready";
}

void Socket::on_readyRead()
{
    qDebug("DATA RECEIVED!");
    memset(m_readBuffer,0,MOUNTPOINT_READ_BUFFERSIZE);
    qint64 bytes_received = m_tcpSocket->read(m_readBuffer,MOUNTPOINT_READ_BUFFERSIZE);
    qDebug() << m_readBuffer;
}

void Socket::write(const char *data, qint64 maxSize)
{
    if(m_tcpSocket->state() == QAbstractSocket::ConnectedState){
        qDebug() << "sending " << maxSize << " bytes";
        m_tcpSocket->write(data,maxSize);
    }
}

void Socket::on_threadDestroyed() {
    if(m_tcpSocket)
        m_tcpSocket->abort();
}

void Socket::connectToHost()
{
    qDebug() << "Starting Connectinon";
    m_tcpSocket->connectToHost(m_url.toUtf8().data(),m_port.toInt());
}

void Socket::disconnectFromHost()
{
    qDebug() << "Closing Connectinon";
    m_tcpSocket->disconnectFromHost();
}

void Socket::abort()
{
    qDebug() << "Aborting Connectinon";
    m_tcpSocket->abort();
}

QAbstractSocket::SocketState Socket::getState()
{
    return m_tcpSocket->state();
}