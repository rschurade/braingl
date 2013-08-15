/*
 * models.h
 *
 * Created on: Mar 18, 2013
 * @author schurade
 */

#ifndef MODELS_H_
#define MODELS_H_

#include "enums.h"

#include <QAbstractItemModel>

class Models
{
public:
    static void init();

    static void setGlobal( Fn::Property var, QVariant value );
    static QVariant getGlobal( Fn::Property var );

    static QAbstractItemModel* globalModel();
    static QAbstractItemModel* dataModel();
    static QAbstractItemModel* RoiModel();

    static QAbstractItemModel* g();
    static QAbstractItemModel* d();
    static QAbstractItemModel* r();

private:
    static QAbstractItemModel* m_globalModel;
    static QAbstractItemModel* m_dataModel;
    static QAbstractItemModel* m_roiModel;

    Models() {};
    virtual ~Models() {};
};

#endif /* MODELS_H_ */
