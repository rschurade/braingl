/*
 * objectrenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph Schurade
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

#include "GL/glew.h"

#include <QtGui/QVector3D>
#include <QtGui/QAbstractItemView>

class ObjectRenderer : public QAbstractItemView
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init() = 0;
	virtual void initGeometry() = 0;
	virtual void setShaderVars() = 0;
	//virtual void draw() = 0;

	// methods that must be implemented for QAbstractItemView
	QRect visualRect( const QModelIndex &index ) const;
    void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );
    QModelIndex indexAt( const QPoint &point ) const;
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden( const QModelIndex &index ) const;
    void setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags );
    QRegion visualRegionForSelection( const QItemSelection &selection ) const;

protected:
    QString createSettingsString( QVector<QVariant>settings );
    QString m_previousSettings;
};

#endif /* OBJECTRENDERER_H_ */
