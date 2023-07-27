#include "vsingleapp.h"

VSingleApp::VSingleApp()
{
    QSettings setting("setting.ini", QSettings::IniFormat);
    contructObject(setting, QUuid(), QUuid());
}

VSingleApp::VSingleApp(const QString &fileName)
{
    QSettings setting(fileName, QSettings::IniFormat);
    contructObject(setting, QUuid(), QUuid());
}

VSingleApp::VSingleApp(QSettings &setting)
{
    contructObject(setting, QUuid(), QUuid());
}

VSingleApp::VSingleApp(QSettings &setting, QUuid &semUuid, QUuid &memUuid)
{
    contructObject(setting, semUuid, memUuid);
}

VSingleApp::~VSingleApp()
{
    delete semaphore;
    delete sharedMem;
}

QUuid VSingleApp::readSetting(QSettings &setting, const QString &nameSetting, const QUuid &uuid)
{
    if(uuid.isNull())
    {
        QVariant variant = setting.value(nameSetting);
        if(variant.isNull())
        {
            QUuid uuidLocal = QUuid::createUuid();
            setting.setValue(nameSetting,uuidLocal.toString());
            return uuidLocal;
        }
        else
        {
            return QUuid::fromString(variant.toString());
        }
    }
    else
    {
        setting.setValue(nameSetting, uuid.toString());
        return uuid;
    }
}

void VSingleApp::contructObject(QSettings &setting, const QUuid &semUuid, const QUuid &memUuid)
{
    programmIsRun = false;
    semaphoreUuid = readSetting(setting,"Uuid/Semaphore",semUuid);
    sharedMemUuid = readSetting(setting,"Uuid/SharedMemory",memUuid);

    semaphore = new QSystemSemaphore(semaphoreUuid.toString(QUuid::WithoutBraces),1);
    semaphore->acquire();

    sharedMem = new QSharedMemory(sharedMemUuid.toString(QUuid::WithoutBraces));
#ifdef Q_OS_UNIX
    if(sharedMem->attach())
    {
        sharedMem->detach();
    }
#endif

    if(sharedMem->attach())
    {
        programmIsRun = true;
    }
    else
    {
        sharedMem->create(1);
    }

    semaphore->release();
}

