#ifndef WIDGET_H
#define WIDGET_H

#include "vsingleapp.h"
#include "vtcpserver.h"
#include <QWidget>
#include <QSystemTrayIcon>
#include <QNetworkInterface>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(VSingleApp &singleApp, QWidget *parent = nullptr);
    ~Widget();
    void writeLogBrowser(const QString &str);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void addServer();
    void removeServer();
    void startServer();
    void stopServer();

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void updateServerIp();

signals:
    void quit();

private:
    enum class CloseType
    {
        DefaultX,       //defaul close on 'X' button
        TrayMenuAction,
        ButtonAction
    };

private:
    Ui::Widget *ui;
    QSystemTrayIcon *trayIcon;
    CloseType closeType;
    QList<QHostAddress> hostAddressList;
    QList<VTcpServer*> serverList;
    QDir path;
};
#endif // WIDGET_H
