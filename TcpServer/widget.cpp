#include "widget.h"
#include "ui_widget.h"
#include "comboboxupdateeventfilter.h"

#include <QMenu>
#include <QStyle>
#include <QCloseEvent>

Widget::Widget(VSingleApp &singleApp, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , trayIcon(new QSystemTrayIcon(this))
    , closeType(CloseType::DefaultX)
    , path(QDir::current())
{
    ui->setupUi(this);
    QIcon icon(":icon/Proycontec-Robots-Robot-documents.ico");
    setWindowIcon(icon);
    writeLogBrowser(QString("Uuid/Semaphore = %1").arg(singleApp.getSemUuid().toString()));
    writeLogBrowser(QString("Uuid/SharedMemory = %1").arg(singleApp.getSharedMemUuid().toString()));

    //init tray menu
    QMenu *menu = new QMenu(this);
    QAction *viewWindow = new QAction(tr("Развернуть окно"), this);
    QAction *quitAction = new QAction(tr("Выход"), this);
    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    //init tray
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(this->windowTitle());
    trayIcon->setContextMenu(menu);
    trayIcon->show();

    ComboBoxUpdateEventFilter *serverIpEventFilter = new ComboBoxUpdateEventFilter(this);
    ui->serverIpComboBox->installEventFilter(serverIpEventFilter);

    QAbstractItemView *serverIpView = ui->serverIpComboBox->view();
    serverIpView->setTextElideMode(Qt::ElideNone);
    //serverIpView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    updateServerIp();

    if(!path.exists("logFiles"))
    {
        path.mkdir("logFiles");
    }
    path.cd("logFiles");

    //init servertable


    //connect
    connect(viewWindow, &QAction::triggered, this, &Widget::show);
    connect(quitAction, &QAction::triggered, [=](){
        closeType = CloseType::TrayMenuAction; this->close();});
    connect(ui->exit, &QPushButton::clicked, [=](){
        closeType = CloseType::ButtonAction; this->close();});
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Widget::trayIconActivated);
    connect(serverIpEventFilter, &ComboBoxUpdateEventFilter::clicked, this, &Widget::updateServerIp);
    //button connect
    connect(ui->addServerButton, &QPushButton::clicked, this, &Widget::addServer);
    connect(ui->removeServerButton, &QPushButton::clicked, this, &Widget::removeServer);
    connect(ui->startTcpServerButton, &QPushButton::clicked, this, &Widget::startServer);
    connect(ui->stopTcpServerButton, &QPushButton::clicked, this, &Widget::stopServer);
}

Widget::~Widget()
{
    delete ui;
    delete trayIcon;
}

void Widget::closeEvent(QCloseEvent *event)
{
    switch (closeType) {
    case CloseType::DefaultX:
        event->ignore();
        this->hide();
        {
            QSystemTrayIcon::MessageIcon icon(QSystemTrayIcon::Information);
            trayIcon->showMessage(this->windowTitle(),
                                  tr("Приложение свернуто в трей. Для того чтобы, развернуть окно приложения, щелкните по иконке приложения в трее"),
                                  icon, 2000);
        }
        break;
    case CloseType::TrayMenuAction:
    case CloseType::ButtonAction:
        event->accept();
        emit quit();
    default:
        break;
    }
    closeType = CloseType::DefaultX;
}

void Widget::addServer()
{
    int serverIndex = ui->serverIpComboBox->currentIndex();
    if(serverIndex > -1)
    {
        quint16 port = ui->serverPortEdit->text().toUShort();
        QString dirName = QString("%1-%2")
                            .arg(hostAddressList.at(serverIndex).toString())
                            .arg(port);
        if(!path.exists(dirName))
        {
            path.mkdir(dirName);
        }
        QDir dirLog(path);
        dirLog.cd(dirName);
        VTcpServer *server = new VTcpServer(hostAddressList.at(serverIndex),port,dirLog,this);
        int row = serverList.length();
        QTableWidgetItem *nameItem = new QTableWidgetItem(server->name());
        QTableWidgetItem *statusItem = new QTableWidgetItem("Stop");
        QTableWidgetItem *connectionCountItem = new QTableWidgetItem("0");
        ui->serverTable->setRowCount(row+1);
        ui->serverTable->setItem(row, 0, nameItem);
        ui->serverTable->setItem(row, 1, statusItem);
        ui->serverTable->setItem(row, 2, connectionCountItem);

        serverList.append(server);
    }
    else
    {
        writeLogBrowser("Ошибка добавления сервера");
    }
}

void Widget::removeServer()
{
    int i = ui->serverTable->currentRow();
    VTcpServer *serv = serverList[i];
    if(serv->isListening())
    {
        serv->stop();
    }
    ui->serverTable->removeRow(i);
    serverList.removeAt(i);
    delete serv;
}

void Widget::startServer()
{
    int i = ui->serverTable->currentRow();
    if(serverList[i]->start())
    {
        QTableWidgetItem *item = ui->serverTable->item(i,1);
        item->setText("Listening");
    }
}

void Widget::stopServer()
{
    int i = ui->serverTable->currentRow();
    serverList[i]->stop();
}

void Widget::writeLogBrowser(const QString &str)
{
    ui->logBrowser->append(str);
}

void Widget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        if(!this->isVisible()){
            this->show();
        } else {
            this->hide();
        }
        break;
    default:
        break;
    }
}

void Widget::updateServerIp()
{
    hostAddressList = QNetworkInterface::allAddresses();
    ui->serverIpComboBox->clear();
    if(!hostAddressList.isEmpty())
    {
        for(auto addr : hostAddressList)
        {
            ui->serverIpComboBox->addItem(addr.toString());
        }
    }
}

