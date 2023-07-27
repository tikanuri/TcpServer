#include <QEvent>
#include "comboboxupdateeventfilter.h"

ComboBoxUpdateEventFilter::ComboBoxUpdateEventFilter(QObject *parent)
    : QObject{parent}
{

}

bool ComboBoxUpdateEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Type::MouseButtonPress)
    {
        emit clicked();
    }
    return QObject::eventFilter(watched, event);
}
