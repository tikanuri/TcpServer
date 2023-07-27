#include "widget.h"
#include "vsingleapp.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VSingleApp sApp;
    if(sApp.isRun())
    {
        QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText(QObject::tr("Приложение уже запущено.\n"
                                "Вы можете запустить только один экземпляр приложения."));
                msgBox.exec();
                return 1;
    }

    Widget w(sApp);
    QObject::connect(&w, &Widget::quit, &a, &QApplication::quit);
    w.show();
    return a.exec();
}
