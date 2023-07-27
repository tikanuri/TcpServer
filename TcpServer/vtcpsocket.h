#ifndef VTCPSOCKET_H
#define VTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTimerEvent>

class VTcpSocket : public QObject
{
    Q_OBJECT

private:
    enum ErrorMessageType
    {
        Timeout,
        NotEqual
    };

    enum LoggingMessageType
    {
        Send,
        Receive,
        Error
    };

    bool run;
    int timerId;
    QByteArray message;
    QTcpSocket *socket;

public:
    explicit VTcpSocket(QTcpSocket *_socket, QObject *parent = nullptr);
    QTcpSocket* getQTcpSocket(){return socket;};
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void generateMessage();
    QString generateLoggingMessage(const LoggingMessageType &type, const QString &msg);
    QString generateLoggingMessage(const LoggingMessageType &type){return generateLoggingMessage(type, message);};
    void sendData();
    void errorMessageHandler(const ErrorMessageType &type);

private slots:
    void readData();
    void disconnectSocket();

signals:
    void disconnected();
    void logging(const QString &msg);
};

#endif // VTCPSOCKET_H
