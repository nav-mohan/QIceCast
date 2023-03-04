#include <QTcpSocket>
#include <QAbstractSocket>
#include "constants.h"
#include "socket.h"
#include "socketutils.h"

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
    m_readBuffer = (char*)malloc(MOUNTPOINT_READ_BUFFERSIZE);
    m_tcpSocket->setReadBufferSize(MOUNTPOINT_READ_BUFFERSIZE);
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    connect(m_tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),this, SLOT(errorOccurred(QAbstractSocket::SocketError)));
    connect(m_tcpSocket,SIGNAL(connected()),this,SLOT(connected()));
    connect(m_tcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(dataReceived()));
    m_tcpSocket->open(QIODevice::ReadWrite);
}

void Socket::errorOccurred(QAbstractSocket::SocketError)
{   
    qDebug("TCPSOCKET ERROR!!!");
    emit signal_socketError(m_tcpSocket->errorString());
}

void Socket::abort()
{
    emit signal_socketState(SOCKET_STATE_CLOSING);
    m_tcpSocket->abort();
    if (!m_tcpSocket->waitForDisconnected(MOUNTPOINT_SOCKET_TIMEOUT)) {
        emit signal_socketError("Failed to abort the socket:" + m_tcpSocket->errorString());
        qWarning() << "Failed to abortthe socket:" << m_tcpSocket->errorString();
    }
}
void Socket::disconnectFromHost()
{
    emit signal_socketState(SOCKET_STATE_CLOSING);
    m_tcpSocket->disconnectFromHost();
    if (!m_tcpSocket->waitForDisconnected(MOUNTPOINT_SOCKET_TIMEOUT)) {
        emit signal_socketError("Failed to disconnect the socket:" + m_tcpSocket->errorString());
        qWarning() << "Failed to disconnect the socket:" << m_tcpSocket->errorString();
    }
}

void Socket::connectToHost()
{   
    m_tcpSocket->connectToHost(m_url.toUtf8().data(),m_port.toInt());
    emit signal_socketState(SOCKET_STATE_CONNECTING);
    if (!m_tcpSocket->waitForConnected(MOUNTPOINT_SOCKET_TIMEOUT)) {
        emit signal_socketError("Failed to connect the socket:" + m_tcpSocket->errorString());
        qWarning() << "Failed to connect the socket:" << m_tcpSocket->errorString();
    }
}

void Socket::connected()
{
    emit signal_socketState(SOCKET_STATE_AUTHENTICATING);
    prepareAuthHeader();
    m_tcpSocket->write(m_authHeader.toUtf8().constData(),(qint64)(m_authHeader.size()));
}

void Socket::disconnected()
{
    m_authHeader = "";
    m_tcpSocket->flush();
    emit signal_socketState(SOCKET_STATE_DISCONNECTED);
}

void Socket::prepareAuthHeader()
{
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

void Socket::dataReceived()
{
    memset(m_readBuffer,0,MOUNTPOINT_READ_BUFFERSIZE);
    qint64 bytes_received = m_tcpSocket->read(m_readBuffer,MOUNTPOINT_READ_BUFFERSIZE);
    qDebug() << m_readBuffer;
    if(!strcmp(m_readBuffer,"HTTP/1.0 200 OK\r\n\r\n")){
        emit signal_socketState(SOCKET_STATE_CONNECTED);
    }
}

void Socket::transmit_buffer(const char *data, qint64 maxSize)
{
    qDebug() << "sending data";
    m_tcpSocket->write(data,maxSize);
}