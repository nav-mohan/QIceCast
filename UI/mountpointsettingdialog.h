#if !defined(MOUNTPOINTSETTINGDIALOG_H)
#define MOUNTPOINTSETTINGDIALOG_H

#include <QDialog>
#include <QObject>
#include <QString>

namespace Ui{class MountpointSettingDialog;} // namespace Ui

class MountpointSettingDialog : public QDialog {
Q_OBJECT

public:
    MountpointSettingDialog();
    ~MountpointSettingDialog();
    QString m_url;
    QString m_port;
    QString m_endpoint;
    QString m_metadata;
    QString m_password;
    QString m_mime;

public slots:
    void on_acceptSettings_clicked();
    void on_rejectSettings_clicked();
    void toggle_acceptSettings_enabled();

signals:
    void finished(int);

private:
    Ui::MountpointSettingDialog* m_ui;

};

#endif // MOUNTPOINTSETTINGDIALOG_H