/*
 * niftiformatdelegate.cpp
 *
 *  Created on: Jan 22, 2013
 *      Author: schurade
 */

#include "niftiformatdelegate.h"

NiftiFormatDelegate::NiftiFormatDelegate( QObject *parent ) :
    QStyledItemDelegate(parent)
{
}

NiftiFormatDelegate::~NiftiFormatDelegate()
{
}

QString NiftiFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
 {
    Q_UNUSED(locale);
    switch ( value.toInt() )
    {
        case 0:
            return QString( "unknown" );
            break;
        case 1:
            return QString( "binary" );
            break;
        case 2:
            return QString( "unsigned char" );
            break;
        case 4:
            return QString( "signed short" );
            break;
        case 8:
            return QString( "signed int" );
            break;
        case 16:
            return QString( "float" );
            break;
        case 32:
            return QString( "complex" );
            break;
        case 64:
            return QString( "double" );
            break;
        case 128:
            return QString( "RGB" );
            break;
        default:
            return QString( "unknown" );
            break;
    }
    return QString( "unknown" );
 }
