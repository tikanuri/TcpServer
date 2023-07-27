#ifndef VTCPSERVER_H
#define VTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "vtcpsocket.h"

class VTcpServer : public QObject
{
    Q_OBJECT

private:
    QHostAddress address;
    quint16 port;
    QDir path;
    QFile logFile;
    QTextStream log;
    QTcpServer *server;
    QList<VTcpSocket*> sockets;

public:
    explicit VTcpServer(const QHostAddress &addr, qint16 _port, QDir &_path, QObject *parent = nullptr);
    QString name(){return QString("%1:%2").arg(address.toString()).arg(port);};
    bool isListening() const{return server->isListening();};

public slots:
    bool start();
    void stop();

protected:
    void timerEvent(QTimerEvent *event) override;

private:

private slots:
    void newConnection();
    void disconnectSocket();
    void writeLog(const QString &msg);

};

#endif // VTCPSERVER_H
