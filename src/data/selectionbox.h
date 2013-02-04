/*
 * selectionbox.h
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include "roi.h"

class BoxRenderer;

class SelectionBox : public ROI
{
    Q_OBJECT

public:
    SelectionBox( QString name, QAbstractItemModel* globals );
    virtual ~SelectionBox();

    virtual void draw( QMatrix4x4 mvpMatrix, QMatrix4x4 mvMatrixInverse );

private:
    BoxRenderer* m_renderer;

private slots:
    void propChanged();
};

#endif /* SELECTIONBOX_H_ */
