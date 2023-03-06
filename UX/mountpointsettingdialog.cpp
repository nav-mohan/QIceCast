#include "mountpointsettingdialog.h"
#include "ui_mountpointsettingdialog.h"

#include <QDebug>

MountpointSettingDialog::MountpointSettingDialog()
    :m_ui(new Ui::MountpointSettingDialog)
{
    m_ui->setupUi(this);
    m_ui->mime->insertItem(0,"MP3","audio/mpeg");
    m_ui->mime->insertItem(1,"AAC","audio/aac");
    connect(
        m_ui->url,SIGNAL(textChanged(const QString&)),
        this,SLOT(toggle_acceptSettings_enabled())
    );
    connect(
        m_ui->port,SIGNAL(textChanged(const QString&)),
        this,SLOT(toggle_acceptSettings_enabled())
    );
    connect(
        m_ui->endpoint,SIGNAL(textChanged(const QString&)),
        this,SLOT(toggle_acceptSettings_enabled())
    );
    connect(
        m_ui->mime,SIGNAL(currentIndexChanged(int)),
        this,SLOT(toggle_acceptSettings_enabled())
    );
    toggle_acceptSettings_enabled();
}

MountpointSettingDialog::~MountpointSettingDialog()
{
    qDebug("Deleting MountpointSettingdialog");
}

void MountpointSettingDialog::on_rejectSettings_clicked(){emit finished(0);}

void MountpointSettingDialog::on_acceptSettings_clicked()
{
    m_url = m_ui->url->text();
    m_port = m_ui->port->text();
    m_password = m_ui->password->text();
    m_mime = m_ui->mime->currentData().toString();
    m_endpoint = m_ui->endpoint->text();
    m_metadata = m_ui->metadata->text();
    emit finished(1);
}

void MountpointSettingDialog::toggle_acceptSettings_enabled()
{
    QString url = m_ui->url->text();
    QString port = m_ui->port->text();
    QString endpoint = m_ui->endpoint->text();
    QString metadata = m_ui->metadata->text();
    int mime_index = m_ui->mime->currentIndex();
    if(url.isEmpty() || port.isEmpty() || endpoint.isEmpty() || mime_index == -1)
        m_ui->acceptSettings->setEnabled(false);
    else
        m_ui->acceptSettings->setEnabled(true);
}