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

class ObjectRenderer : public QAbstractItemView
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init() = 0;
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
    QString createSettingsString( int x, int y, int z,
                                  int lod, int orient,
                                  int lx, int ux, int ly, int uy, int lz, int uz,
                                  bool scaling, int bValue );
    int getMaxLod( int orient, int lx, int ux, int ly, int uy, int lz, int uz );

    QString m_previousSettings;
};

#endif /* OBJECTRENDERER_H_ */
