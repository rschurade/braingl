/*
 * crossingtrackwidget.h
 *
 * Created on: 26.01.2013
 * @author Ralph Schurade
 */

#ifndef CROSSINGTRACKWIDGET_H_
#define CROSSINGTRACKWIDGET_H_

#include "../../../data/enums.h"

#include <QtWidgets>

class QProgressBar;
class QPushButton;
class SelectWithLabel;
class Dataset;
class TrackWithCrossings;

class CrossingTrackWidget : public QWidget
{
    Q_OBJECT

public:
    CrossingTrackWidget( Dataset* ds, QList<QVariant> &dsl, QWidget* parent = 0 );
    virtual ~CrossingTrackWidget();

    QList<Dataset*> getFibs();

private:
    Dataset* m_ds;
    QList<QVariant> m_dsl;
    TrackWithCrossings* m_tracker;

    QVBoxLayout* m_layout;

    SelectWithLabel* m_maskSelect;
    SelectWithLabel* m_tensor2Select;
    SelectWithLabel* m_tensor3Select;

    QPushButton* m_startButton;
    QProgressBar* m_progressBar;
    int m_progress;

private slots:
    void start();
    void slotProgress();
    void slotFinished();

signals:
    void finished();
};

#endif /* CROSSINGTRACKWIDGET_H_ */
