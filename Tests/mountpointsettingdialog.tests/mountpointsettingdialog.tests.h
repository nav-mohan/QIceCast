#if !defined(TESTMOUNTPOINTSETTINGDIALOG_H)
#define TESTMOUNTPOINTSETTINGDIALOG_H
#include <QTest>

#include <QString>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QListView>

#include "mountpointsettingdialog.h"

class TestMountpointSettingDialog : public QObject {
Q_OBJECT

public:
    TestMountpointSettingDialog(QObject *parent = nullptr){}

private slots:
    void initTestCase()
    {
        qDebug("initializing mountpointsettingdialog");
        m_dialog = new MountpointSettingDialog();
        m_url = m_dialog->findChild<QLineEdit*>("url");
        m_port = m_dialog->findChild<QLineEdit*>("port");
        m_endpoint = m_dialog->findChild<QLineEdit*>("endpoint");
        m_password = m_dialog->findChild<QLineEdit*>("password");
        m_metadata = m_dialog->findChild<QLineEdit*>("metadata");
        m_mime = m_dialog->findChild<QComboBox*>("mime");
        m_acceptSettings = m_dialog->findChild<QPushButton*>("acceptSettings");
        m_rejectSettings = m_dialog->findChild<QPushButton*>("rejectSettings");
    }   

    void test__window_initialized()
    {
        QVERIFY("Mountpoint Settings" == m_dialog->property("windowTitle").toString());
        QVERIFY("localhost" == m_url->property("placeholderText").toString());
        QVERIFY("8000" == m_port->property("placeholderText").toString());
        QVERIFY("/stream" == m_endpoint->property("placeholderText").toString());
        QVERIFY("hackme" == m_password->property("placeholderText").toString());
        QVERIFY("Encoder" == m_mime->property("placeholderText").toString());
        QVERIFY("Ok" == m_acceptSettings->property("text").toString());
        QVERIFY("Cancel" == m_rejectSettings->property("text").toString());
        test__acceptSettings_state(0);
    }

    void test__url_updates()
    {
        QTest::keyClicks(m_url, "localhost");
        QCOMPARE(m_url->text(), QString("localhost"));
        test__acceptSettings_state(0);
    }

    void test__port_updates()
    {
        QTest::keyClicks(m_port, "8000");
        QCOMPARE(m_port->text(), QString("8000"));
        test__acceptSettings_state(0);
    }

    void test__endpoint_updates()
    {
        QTest::keyClicks(m_endpoint, "/stream");
        QCOMPARE(m_endpoint->text(), QString("/stream"));
        test__acceptSettings_state(0);
    }

    void test__mime_updates()
    {
        QTest::mouseClick(m_mime,Qt::LeftButton);
        QTest::qWait(1000);
        QListView* lw = m_mime->findChild<QListView *>();
        if(lw)
        {
            qDebug() << " " << lw->size() << " " << lw->gridSize();
            for (int r = 0; r < lw->model()->rowCount(); r++)
            {
                QModelIndex idx = lw->model()->index(r,0);
                lw->scrollTo(idx);
                QPoint itemPt = lw->visualRect(idx).center();
                QString str = lw->model()->index(r,0).data(Qt::DisplayRole).toString();
                qDebug() << "clicking on " << str << itemPt;
                if(!itemPt.isNull())
                {
                    QTest::mouseClick(lw->viewport(),Qt::LeftButton,0,itemPt);
                    QTest::qWait(1000);
                    QTest::mouseClick(m_mime,Qt::LeftButton);
                    QTest::qWait(1000);
                }
            }
        }
        test__acceptSettings_state(1);
    }

private:
    MountpointSettingDialog *m_dialog;
    QLineEdit *m_url;
    QLineEdit *m_port;
    QLineEdit *m_password;
    QComboBox *m_mime;
    QLineEdit *m_endpoint;
    QLineEdit *m_metadata;
    QPushButton *m_acceptSettings;
    QPushButton *m_rejectSettings;
    void test__acceptSettings_state(bool state)
    {
        QVERIFY(m_acceptSettings->property("enabled") == state);
    }
};


#endif // TESTMOUNTPOINTSETTINGDIALOG_H
