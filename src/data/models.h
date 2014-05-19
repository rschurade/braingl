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

class Dataset;
class ROI;
class ROIWidget;
class ROIArea;

class Models
{
public:
    static void init();

    static void setGlobal( int var, QVariant value );
    static void setGlobal( Fn::Property var, QVariant value );

    static QVariant getGlobal( Fn::Property var );
    static QVariant getGlobal( int var );

    static ROI* getRoi( int branch, int pos );
    static void setROIProp( int branch, int pos, Fn::Property prop, QVariant value );
    static void setROIProp( int branch, int pos, int prop, QVariant value );

    static QAbstractItemModel* globalModel();
    static QAbstractItemModel* dataModel();
    static QAbstractItemModel* RoiModel();

    static QAbstractItemModel* g();
    static QAbstractItemModel* d();
    static QAbstractItemModel* r();

    static QModelIndex createRoiIndex( int branch, int pos, int column );

    static Dataset* getDataset( int id );
    static QList<Dataset*>getDatasets( Fn::DatasetType filter, bool ignoreInactive = true );
    static QList<Dataset*>getDatasets( int filter, bool ignoreInactive = true );

    static void setRoiWidget( ROIWidget* rw );
    static void addROIArea( ROIArea* roi );

    static void addDataset( Dataset* ds );

private:
    static QAbstractItemModel* m_globalModel;
    static QAbstractItemModel* m_dataModel;
    static QAbstractItemModel* m_roiModel;
    static ROIWidget* m_roiWidget;

    Models() {};
    virtual ~Models() {};
};

#endif /* MODELS_H_ */
