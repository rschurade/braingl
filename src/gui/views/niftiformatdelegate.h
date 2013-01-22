/*
 * niftiformatdelegate.h
 *
 *  Created on: Jan 22, 2013
 *      Author: schurade
 */

#ifndef NIFTIFORMATDELEGATE_H_
#define NIFTIFORMATDELEGATE_H_

#include <QtGui/QStyledItemDelegate>

class NiftiFormatDelegate : public QStyledItemDelegate
{
public:
    NiftiFormatDelegate( QObject *parent = 0 );
    virtual ~NiftiFormatDelegate();

    virtual QString displayText(const QVariant & value, const QLocale & locale ) const;
};

#endif /* NIFTIFORMATDELEGATE_H_ */
