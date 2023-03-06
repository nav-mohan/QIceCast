#if !defined(MOUNTPOINTWIDGET_H)
#define MOUNTPOINTWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QMap>
#include <QAbstractSocket>

class QCircularBuffer;
class QThread;
class Encoder;
class Socket;

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
        QString mime,
        QCircularBuffer *inputBuffer
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
    

public slots:
    void on_closeMountpoint_clicked();
    void on_startStopStream_clicked();
    void on_socketStateChanged(QAbstractSocket::SocketState);
    
private:
    Ui::MountpointWidget* m_ui;

signals:
    void readyRead();
    void close_mountpoint(QString);
    void announce_error(QString,QString,QString);
};

#endif // MOUNTPOINTWIDGET_H