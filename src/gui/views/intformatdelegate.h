/*
 * intformatdelegate.h
 *
 *  Created on: Jan 22, 2013
 *      Author: schurade
 */

#ifndef INTFORMATDELEGATE_H_
#define INTFORMATDELEGATE_H_

#include <QtGui/QStyledItemDelegate>

class IntFormatDelegate : public QStyledItemDelegate
{
public:
    IntFormatDelegate( QObject *parent = 0 );
    virtual ~IntFormatDelegate();

    virtual QString displayText(const QVariant & value, const QLocale & locale ) const;
};

#endif /* INTFORMATDELEGATE_H_ */
