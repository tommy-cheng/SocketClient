#include <QtNetwork>
#include <QString>
#include <QMessageBox>
#include "socketclient.h"
#include "ui_socketclient.h"

SocketClient::SocketClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SocketClient)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

}

SocketClient::~SocketClient()
{

    delete ui;
}

void SocketClient::on_rbTCP_clicked(bool checked)
{
    if (checked) {
        if (!ui->leIPAddr->text().isEmpty() && !ui->lePortNo->text().isEmpty()) {
            ui->pbConnect->setEnabled(true);
            return;
        }
    }
    ui->pbConnect->setEnabled(false);
}


void SocketClient::socketConnected()
{
    ui->leStatus->setText(tr("socket connected"));
}

void SocketClient::on_rbUDP_clicked(bool checked)
{
    ui->pbConnect->setEnabled(!checked);
}

void SocketClient::on_pbExit_clicked()
{
    close();
}

void SocketClient::on_pbConnect_clicked()
{
    tcpSocket->connectToHost(ui->leIPAddr->text(), ui->lePortNo->text().toInt());
}

void SocketClient::on_leIPAddr_textChanged(const QString &arg1)
{
    SetPBConnect();
}


void SocketClient::on_lePortNo_textChanged(const QString &arg1)
{
    SetPBConnect();
}

void SocketClient::SetPBConnect()
{
    if (!ui->leIPAddr->text().isEmpty() && !ui->lePortNo->text().isEmpty()) {
        if (ui->rbTCP->isChecked()) {
            ui->pbConnect->setEnabled(true);
            return;
        }
    }
    ui->pbConnect->setEnabled(false);
}

void SocketClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;

    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Socket Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Socket Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Socket Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}

void SocketClient::on_pbSend_clicked()
{
    QByteArray outPacket;

    outPacket.append(ui->leInput->text());
    if (ui->rbTCP->isChecked()) {
         tcpSocket->write(outPacket);
    }
    else {
        udpSocket->writeDatagram(outPacket.data(), outPacket.size(),QHostAddress::QHostAddress(ui->leIPAddr->text()), ui->lePortNo->text().toInt());
    }
}

void SocketClient::on_leInput_textChanged(const QString &arg1)
{
    if(ui->leInput->text().isEmpty())
        ui->pbSend->setEnabled(false);
    else
        ui->pbSend->setEnabled(true);
}
