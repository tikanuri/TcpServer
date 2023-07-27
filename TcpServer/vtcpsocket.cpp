#include "vtcpsocket.h"

VTcpSocket::VTcpSocket(QTcpSocket *_socket, QObject *parent)
    : QObject{parent},
      run(false),
      socket(_socket)
{
   if(socket != nullptr)
   {
        connect(socket, &QTcpSocket::readyRead, this, &VTcpSocket::readData);
        connect(socket, &QTcpSocket::disconnected, this, &VTcpSocket::disconnectSocket);
   }
}

void VTcpSocket::start()
{
    run = true;
    sendData();
}

void VTcpSocket::stop()
{
    run = false;
}

void VTcpSocket::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    errorMessageHandler(Timeout);
    sendData();
}

void VTcpSocket::generateMessage()
{
    message = "Test message";
}

void VTcpSocket::sendData()
{
    if(run)
    {
        generateMessage();
        socket->write(message);
        emit logging(generateLoggingMessage(Send));
        timerId = startTimer(5000);
    }
}

void VTcpSocket::readData()
{
    killTimer(timerId);
    QByteArray readMsg = socket->readAll();
    emit logging(generateLoggingMessage(Receive,readMsg));
    if(readMsg != message)
    {
        errorMessageHandler(NotEqual);
    }
    sendData();
}

void VTcpSocket::disconnectSocket()
{
    run = false;
    disconnect(socket, &QTcpSocket::readyRead, this, &VTcpSocket::readData);
    disconnect(socket, &QTcpSocket::disconnected, this, &VTcpSocket::disconnectSocket);
    emit disconnected();
}

void VTcpSocket::errorMessageHandler(const ErrorMessageType &type)
{
    QString msg;
    switch (type) {
    case Timeout:
        msg = QString("Timeout");
        break;
    case NotEqual:
        msg = QString("Receive message not equal send message");
        break;
    }

    emit logging(generateLoggingMessage(Error,msg));
}

QString VTcpSocket::generateLoggingMessage(const LoggingMessageType &type, const QString &msg)
{
    QString log;
    switch (type) {
    case Send:
        log = QString("[%1:%2] -> [%3:%4]:\t\"%5\"")
                .arg(socket->localAddress().toString())
                .arg(socket->localPort())
                .arg(socket->peerAddress().toString())
                .arg(socket->peerPort())
                .arg(msg);
        break;
    case Receive:
        log = QString("[%1:%2] <- [%3:%4]:\t\"%5\"")
                .arg(socket->localAddress().toString())
                .arg(socket->localPort())
                .arg(socket->peerAddress().toString())
                .arg(socket->peerPort())
                .arg(msg);
        break;
    case Error:
        log = QString("Error: %1").arg(msg);
        break;
    }
    return log;
}
