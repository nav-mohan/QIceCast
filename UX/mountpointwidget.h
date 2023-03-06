#if !defined(MOUNTPOINTWIDGET_H)
#define MOUNTPOINTWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QMap>
#include <QAbstractSocket>
#include <QDebug>
#include "stylesheets.h"
#include "encoder.h"
#include "encoderfactory.h"
#include <QThread>
#include "socket.h"
#include "constants.h"
#include "circularbuffer.h"

namespace Ui{class MountpointWidget;} // namespace Ui

class MountpointWidget : public QWidget {
Q_OBJECT
    
public:
    explicit MountpointWidget(
        QWidget* parent, 
        QString url,
        QString port,
        QString endpoint,
        QString metadata,
        QString password,
        QString mime
    );
    ~MountpointWidget();
    QString m_url;
    QString m_port;
    QString m_endpoint;
    QString m_metadata;
    QString m_password;
    QString m_mime;
    QThread *m_workerThread = nullptr;
    Encoder *m_encoder = nullptr;
    Socket *m_socket = nullptr;
    QCircularBuffer *m_inputBuffer;
    void registerInputBuffer(QCircularBuffer*, qint64);
    qint64 m_consumerID;
    

public slots:
    void on_closeMountpoint_clicked();
    void on_startStopStream_clicked();
    void on_socketStateChanged(QAbstractSocket::SocketState);
    void on_readyRead();
    
private:
    Ui::MountpointWidget* m_ui;

signals:
    void readyRead(qint64 bytes_read);
    void close_mountpoint(QString);
    void announce_error(QString,QString,QString);
};

#endif // MOUNTPOINTWIDGET_H