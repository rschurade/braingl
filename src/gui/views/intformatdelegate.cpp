/*
 * intformatdelegate.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: schurade
 */
#include "intformatdelegate.h"

IntFormatDelegate::IntFormatDelegate( QObject *parent ) :
    QStyledItemDelegate(parent)
{
}

IntFormatDelegate::~IntFormatDelegate()
{
}

QString IntFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
 {
    Q_UNUSED(locale);
    QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );
    return QString( "%L1" ).arg( value.toInt() );
 }
