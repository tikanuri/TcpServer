#ifndef COMBOBOXUPDATEEVENTFILTER_H
#define COMBOBOXUPDATEEVENTFILTER_H

#include <QObject>

class ComboBoxUpdateEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit ComboBoxUpdateEventFilter(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void clicked();
};

#endif // COMBOBOXUPDATEEVENTFILTER_H
