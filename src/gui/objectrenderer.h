/*
 * objectrenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

#include <QtGui/QVector3D>
#include <QtGui/QAbstractItemView>

struct VertexData
{
    QVector3D position;
    QVector3D texCoord;
};

class ObjectRenderer : public QAbstractItemView
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init() = 0;
	virtual void initShader() = 0;
	virtual void initGeometry() = 0;
	virtual void setShaderVars() = 0;
	//virtual void draw() = 0;

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

};

#endif /* OBJECTRENDERER_H_ */
