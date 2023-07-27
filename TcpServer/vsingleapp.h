#ifndef VSINGLEAPP_H
#define VSINGLEAPP_H

#include <QUuid>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QSettings>

class VSingleApp
{
private:
    QUuid semaphoreUuid;
    QUuid sharedMemUuid;
    QSystemSemaphore *semaphore;
    QSharedMemory *sharedMem;
    bool programmIsRun;

public:
    VSingleApp();
    VSingleApp(const QString &fileName);
    VSingleApp(QSettings &setting);
    VSingleApp(QSettings &file, QUuid &semUuid, QUuid &memUuid);
    ~VSingleApp();
    inline QUuid getSemUuid(){return semaphoreUuid;};
    inline QUuid getSharedMemUuid(){return sharedMemUuid;};
    inline bool isRun(){return programmIsRun;};

private:
    QUuid readSetting(QSettings &setting, const QString &nameSetting, const QUuid &uuid);
    void contructObject(QSettings &setting, const QUuid &semUuid, const QUuid &memUuid);
};

#endif // VSINGLEAPP_H
