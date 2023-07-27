#include "vtcpserver.h"

VTcpServer::VTcpServer(const QHostAddress &addr, qint16 _port, QDir &_path, QObject *parent)
    : QObject{parent},
      address(addr),
      port(_port),
      path(_path),
      server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &VTcpServer::newConnection);
    logFile.setFileName(path.filePath(QString("%1.log").arg(QDate::currentDate().toString("dd-MM-yyyy"))));
    if(logFile.exists())
    {
        logFile.open(QIODevice::Append | QIODevice::Text);
    }
    else
    {
        logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    }
    if(!logFile.isOpen())
    {
        qDebug() << "Log file not open\n";

    }
    log.setDevice(&logFile);
    int diffMs = QTime::currentTime().msecsTo(QTime(23,59,59,999))+2;
    startTimer(diffMs);
    qDebug() << "diffMs = " << diffMs <<"\n";
}


void VTcpServer::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    logFile.close();
    logFile.setFileName(path.filePath(QString("%1.log").arg(QDate::currentDate().toString("dd-MM-yyyy"))));
    logFile.open(QIODevice::WriteOnly);
    if(!logFile.isOpen())
    {
        qDebug() << "Log file not open\n";
    }
    log.setDevice(&logFile);
    int diffMs = QTime::currentTime().msecsTo(QTime(23,59,59,999))+2;
    startTimer(diffMs);
    qDebug() << "diffMs = " << diffMs <<"\n";
}

bool VTcpServer::start()
{
    return server->listen(address, port);
}

void VTcpServer::stop()
{
    server->close();
    foreach (VTcpSocket *socket, sockets) {
        socket->deleteLater();
    }
    sockets.clear();
}
void VTcpServer::newConnection()
{
    QTcpSocket *tcpSocket = server->nextPendingConnection();
    VTcpSocket *socket = new VTcpSocket(tcpSocket);
    writeLog(QString("Connect client [%1:%2]")
            .arg(tcpSocket->peerAddress().toString())
            .arg(tcpSocket->peerPort())
            );
    sockets.append(socket);
    connect(socket, &VTcpSocket::logging, this, &VTcpServer::writeLog);
    connect(socket, &VTcpSocket::disconnected, this, &VTcpServer::disconnectSocket);
    socket->start();
}

void VTcpServer::disconnectSocket()
{
    VTcpSocket *socket = dynamic_cast<VTcpSocket*>(sender());
    writeLog(QString("Disconnect client [%1:%2]")
            .arg(socket->getQTcpSocket()->peerAddress().toString())
            .arg(socket->getQTcpSocket()->peerPort())
            );
    sockets.removeOne(socket);
    disconnect(socket, &VTcpSocket::logging, this, &VTcpServer::writeLog);
    disconnect(socket, &VTcpSocket::disconnected, this, &VTcpServer::disconnectSocket );
    socket->deleteLater();
}

void VTcpServer::writeLog(const QString &msg)
{
    log << QDateTime::currentDateTime().toString("dd.MM.yyyy hh.mm.ss.zzz")
             <<"\t"<< msg << Qt::endl << Qt::flush;
}

