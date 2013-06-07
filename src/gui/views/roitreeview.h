/*
 * roitreeview.h
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#ifndef ROITREEVIEW_H_
#define ROITREEVIEW_H_

#include <QMouseEvent>
#include <QTreeView>

class ROIArea;

class ROITreeView : public QTreeView
{
    Q_OBJECT

public:
    ROITreeView( QWidget* parent = 0 );
    virtual ~ROITreeView();

    void addBox();
    void addROIArea( ROIArea* roi );

private:
    virtual void mousePressEvent( QMouseEvent *event );

private slots:
    void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );
    void rowsInserted (const QModelIndex &parent, int start, int end );

signals:
    void itemSelectionChanged( const QItemSelection &selected );
    void nothingSelected();

};

#endif /* ROITREEVIEW_H_ */
