#if !defined(MAINWINDOW_H)
#define MAINWINDOW_H

namespace Ui{class MainWindow;} // namespace Ui
// DEBUG
#include <QDebug>

// UI
#include "mainwindow.h"
#include "mountpointsettingdialog.h"
#include "audiolevel.h"
#include <QMessageBox>
#include "mountpointwidget.h"

// Layout
#include "ui_mainwindow.h"

// Constants
#include "constants.h"

// Audio Stuff
#include <QAudio>
#include <QAudioDeviceInfo>
#include "circularbuffer.h"
#include "inputdevice.h"
#include "encoder.h"


#include <QThread>
#include <string>
#include <QWidget>
#include <QDateTime>

#include <QMainWindow>
#include <QScopedPointer>
#include <QMap>
#include <QMapIterator>


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
    void prepareAudioDevice();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;
    QScopedPointer<MountpointSettingDialog> m_mountpointSettingDialog;
    QThread *m_inputThread = nullptr;
    InputDevice *m_inputDevice = nullptr;
    QCircularBuffer *m_inputBuffer = nullptr;
    QMap<QString,MountpointWidget*> m_mountpoints;
    QAudioDeviceInfo m_audioDeviceInfo;
    QAudioFormat m_audioFormat;
    
signals:
    void startRecording();
    void stopRecording();
};

#endif // MAINWINDOW_H