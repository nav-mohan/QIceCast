#include <QObject>
#include <QString>
#include <QMap>
#include <QAbstractSocket>

class QTcpSocket;

enum socket_state 
{
    SOCKET_STATE_DISCONNECTED   = 0,
    SOCKET_STATE_CONNECTING      = 1,
    SOCKET_STATE_AUTHENTICATING  = 2,
    SOCKET_STATE_CONNECTED  = 3,
    SOCKET_STATE_CLOSING  = 4,
};

class Socket : public QObject {
Q_OBJECT
public:
    explicit Socket(
        QString url,
        QString port,
        QString endpoint,
        QString password,
        QString metadata,
        QString mime
    );
    ~Socket();
    
public slots:
    void initialize();
    void connectToHost();
    void connected();
    void dataReceived();
    void errorOccurred(QAbstractSocket::SocketError);
    void disconnectFromHost();
    void abort();
    void disconnected();
    void transmit_buffer(const char *data, qint64 maxSize);

private:
    QTcpSocket *m_tcpSocket = nullptr;
    QString m_url;
    QString m_port;
    QString m_endpoint;
    QString m_metadata;
    QString m_password;
    QString m_mime;
    QString m_authHeader;
    void prepareAuthHeader();
    char *m_readBuffer;

signals:
    void signal_socketError(QString);
    void signal_socketState(int);
    void signal_transferRate(qreal);
};

