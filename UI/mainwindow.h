#if !defined(MAINWINDOW_H)
#define MAINWINDOW_H

namespace Ui{class MainWindow;} // namespace Ui

#include <QMainWindow>
#include <QScopedPointer>
#include <QMap>
#include <QMapIterator>

class MountpointSettingDialog;
class InputDevice;
class QThread;
class MountpointWidget;
class QCircularBuffer;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public slots:
    void on_deviceOptions_currentIndexChanged(int);
    void on_addMountpoint_clicked();
    void mountpoint_settings_received(int);
    void close_mountpoint(QString);
    void display_error(QString, QString, QString);

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
    QScopedPointer<MountpointSettingDialog> m_mountpointSettingDialog;
    QThread *m_inputThread = nullptr;
    InputDevice *m_inputDevice = nullptr;
    QCircularBuffer *m_inputBuffer = nullptr;
    QMap<QString,MountpointWidget*> m_mountpoints;
};

#endif // MAINWINDOW_H