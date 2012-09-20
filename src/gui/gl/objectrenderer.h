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
	void setSceneStats( float zoom, int moveX, int moveY, float bbX, float bbY );

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
    QString createSettingsString( int x, int y, int z, int orient,
                                  int lx, int ux, int ly, int uy, int lz, int uz,
                                  bool scaling, int bValue );
    int getMaxLod( int orient, int lx, int ux, int ly, int uy, int lz, int uz );

    void calcBounds( int nx, int ny, int nz, float dx, float dy, float dz, int orient );

    QString m_previousSettings;

    float m_zoom;
    int m_moveX;
    int m_moveY;
    float m_bbX;
    float m_bbY;

    QVector<int>m_visibleArea;
};

#endif /* OBJECTRENDERER_H_ */
