#if !defined(MAINWINDOW_TESTS_H)
#define MAINWINDOW_TESTS_H

#include "mainwindow.h"
#include "mountpointsettingdialog.h"
#include <QObject>
#include <QTest>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QApplication>

class TestMainWindow : public QObject {
Q_OBJECT

public:
    TestMainWindow(QObject *parent = nullptr){}

private:
    MainWindow *m_win;
    QComboBox *m_deviceOptions;
    QPushButton *m_addMountpoint;
    QListWidget *m_mountpointList;

private slots:
    void initTestCase()
    {
        qInfo("Initializing MainWindow");
        m_win = new MainWindow();
        qDebug() << QApplication::activeWindow();
    }

    void verify_windowTitle_value()
    {
        QString windowTitle = m_win->property("windowTitle").toString();
        QVERIFY(windowTitle == "QIceCast");
    }

    void verify_deviceOptions_initialized()
    {
        m_deviceOptions = m_win->findChild<QComboBox*>("deviceOptions");
        if(m_deviceOptions->count()==0)
            qWarning() << "No input devices. Some test results might be unrealiable.";
        QVERIFY(m_deviceOptions->property("placeholderText")=="Audio Input Device");
    }
    
    void verify_addMountpoint_initialized()
    {
        m_addMountpoint = m_win->findChild<QPushButton*>("addMountpoint");
        QString buttonText = m_addMountpoint->property("text").toString();
        QVERIFY(buttonText == "Add Mount Point");
    }

    void verify_mountpointList_initialized()
    {
        m_mountpointList = m_win->findChild<QListWidget*>("mountpointList");
        QVERIFY(m_mountpointList->count()==0);
    }

    void verify_mountpointSettingDialog_popup()
    {
        QTest::mouseClick(m_addMountpoint,Qt::LeftButton);
    }

};

#endif // MAINWINDOW_TESTS_H
