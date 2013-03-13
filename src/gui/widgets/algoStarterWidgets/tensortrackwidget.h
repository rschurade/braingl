/*
 * tensortrackwidget.h
 *
 *  Created on: 26.01.2013
 *      Author: Ralph Schurade
 */

#ifndef TENSORTRACKWIDGET_H_
#define TENSORTRACKWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui>

class QProgressBar;
class QPushButton;
class Dataset;
class Track;

class TensorTrackWidget : public QWidget
{
    Q_OBJECT

public:
    TensorTrackWidget( Dataset* ds, QList<QVariant> &dsl, QWidget* parent = 0 );
    virtual ~TensorTrackWidget();

    QList<Dataset*> getFibs();

private:
    QVBoxLayout* m_layout;

    Track* m_tracker;

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

#endif /* TENSORTRACKWIDGET_H_ */
