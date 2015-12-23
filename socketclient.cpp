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
    bStartTimer = bTCPConnected = false;
    timer = new QTimer(this);
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
    ui->pbConnect->setText("Disconnect");
    ui->pbConnect->setEnabled(true);
    bTCPConnected = true;
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
    if (bTCPConnected) {
        tcpSocket->disconnectFromHost();
        ui->pbConnect->setText("Connect");
        bTCPConnected = false;
        ui->leStatus->setText("");
    }
    else {
        tcpSocket->connectToHost(ui->leIPAddr->text(), ui->lePortNo->text().toInt());
        ui->pbConnect->setText("Connecting");
        ui->pbConnect->setEnabled(false);
    }
}

void SocketClient::on_leIPAddr_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    SetPBConnect();
}


void SocketClient::on_lePortNo_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
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
                                    "Make sure the server is running, "
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
    QHostAddress ip;

    outPacket.append(ui->leInput->text());
    ip = QHostAddress(ui->leIPAddr->text());
    if (ui->rbTCP->isChecked()) {
         tcpSocket->write(outPacket);
    }
    else {
        udpSocket->writeDatagram(outPacket.data(), outPacket.size(),ip, ui->lePortNo->text().toInt());
    }
}

void SocketClient::on_leInput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(ui->leInput->text().isEmpty())
        ui->pbSend->setEnabled(false);
    else
        ui->pbSend->setEnabled(true);
}

void SocketClient::on_cbEnableTimer_clicked()
{
    if (ui->cbEnableTimer->isChecked()) {
        ui->cbBroadcast->setEnabled(true);
        ui->leTimerValue->setEnabled(true);
        ui->leTimerCommand->setEnabled(true);
        ui->pbTimerTest->setEnabled(true);
        ui->pbTimerTest->setText("TimerTest On");
    }
    else {
        ui->cbBroadcast->setEnabled(false);
        ui->leTimerValue->setEnabled(false);
        ui->leTimerCommand->setEnabled(false);
        ui->pbTimerTest->setEnabled(false);
    }
}

void SocketClient::SendTimerData()
{
    QByteArray outPacket;
    QHostAddress ip;

    outPacket.append(ui->leTimerCommand->text());
    ip = QHostAddress(ui->leIPAddr->text());
    if (ui->cbBroadcast->isChecked())
         udpSocket->writeDatagram(outPacket.data(), outPacket.size(),QHostAddress::Broadcast, ui->lePortNo->text().toInt());
    else
         udpSocket->writeDatagram(outPacket.data(), outPacket.size(),ip, ui->lePortNo->text().toInt());
}

void SocketClient::on_pbTimerTest_clicked()
{
    int tmValue;

    if (bStartTimer) {
        bStartTimer = false;
        timer->stop();
        ui->pbTimerTest->setText("TimerTest On");
    }
    else {
        if (ui->rbUDP->isChecked()) {
            if (ui->lePortNo->text().isEmpty()) {
                QMessageBox::information(this, tr("Socket Client"), "Port number is empty");
                return;
            }
            if (ui->leIPAddr->text().isEmpty()) {
                if (!ui->cbBroadcast->isChecked()) {
                    QMessageBox::information(this, tr("Socket Client"), "Dest IP address is empty");
                    return;
                }
            }
            if (ui->leTimerCommand->text().isEmpty()) {
                QMessageBox::information(this, tr("Socket Client"), "Timer command is empty");
                return;
            }
            ui->pbTimerTest->setText("TimerTest Off");
            bStartTimer = true;
            SendTimerData();
            connect(timer, SIGNAL(timeout()), this, SLOT(MyTimerSlot()));
            tmValue = ui->leTimerValue->text().toInt() * 1000;
            timer->start(tmValue);
        }
        else {
            QMessageBox::information(this, tr("Socket Client"), "Set protocol to UDP");
            return;
        }
    }
}

void SocketClient::MyTimerSlot()
{
    SendTimerData();
}
