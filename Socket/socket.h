#if !defined(SOCKET_H)
#define SOCKET_H

#include <QTcpSocket>
#include <QAbstractSocket>
#include "constants.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <QAbstractSocket>
#include <QTcpSocket>

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

    QTcpSocket *m_tcpSocket;
    QString m_url;
    QString m_port;
    QString m_endpoint;
    QString m_metadata;
    QString m_password;
    QString m_mime;
    QString m_authHeader;
    void prepareAuthHeader();
    char *m_readBuffer;

public slots:
    void initialize();
    void on_readyRead();
    void on_errorOccurred(QAbstractSocket::SocketError);
    void write(const char *data, qint64 maxSize);
    void on_threadDestroyed();
    void connectToHost();
    void disconnectFromHost();
    void abort();
    void authenticate();
    void reset();

signals:
    void stateChanged(int); // this triggers mountpointwidget's socket
    void socketErrored(QString);

};


#endif // SOCKET_H
