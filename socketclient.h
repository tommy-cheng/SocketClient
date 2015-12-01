#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QDialog>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTcpSocket;
QT_END_NAMESPACE

namespace Ui {
class SocketClient;
}

class SocketClient : public QDialog
{
    Q_OBJECT

public:
    explicit SocketClient(QWidget *parent = 0);
    ~SocketClient();

private slots:
    void on_rbTCP_clicked(bool checked);

    void on_rbUDP_clicked(bool checked);

    void on_pbExit_clicked();

    void on_pbConnect_clicked();

    void on_leIPAddr_textChanged(const QString &arg1);

    void on_lePortNo_textChanged(const QString &arg1);

    void socketConnected();

    void displayError(QAbstractSocket::SocketError socketError);

    void on_pbSend_clicked();

    void on_leInput_textChanged(const QString &arg1);

private:
    Ui::SocketClient *ui;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;

    void SetPBConnect();
};

#endif // SOCKETCLIENT_H
