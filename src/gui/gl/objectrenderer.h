/*
 * objectrenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef OBJECTRENDERER_H_
#define OBJECTRENDERER_H_

#include "GL/glew.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QAbstractItemView>

#include <initializer_list>

class PropertyGroup;

class ObjectRenderer : public QAbstractItemView
{
public:
	ObjectRenderer();
	virtual ~ObjectRenderer();

	virtual void init() = 0;
	virtual void initGeometry() {};
	virtual void setShaderVars() {};
	//virtual void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props );

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
    QString createSettingsString( std::initializer_list<QVariant> settings );
    QString m_previousSettings;
};

#endif /* OBJECTRENDERER_H_ */
