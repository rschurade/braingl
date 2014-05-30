/*
 * tensortrackwidget.h
 *
 * Created on: 11.09.2013
 * @author Ralph Schurade
 */

#ifndef CORRELATIONWIDGET_H_
#define CORRELATIONWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui>

class QProgressBar;
class QPushButton;
class Dataset;
class DatasetMeshTimeSeries;
class Correlation;

class CorrelationWidget : public QWidget
{
    Q_OBJECT

public:
    CorrelationWidget( Dataset* ds, QWidget* parent = 0 );
    virtual ~CorrelationWidget();

    QList<Dataset*> getResult();

private:
    DatasetMeshTimeSeries* m_dataset;

    QVBoxLayout* m_layout;


    QPushButton* m_startButton;
    QProgressBar* m_progressBar;
    int m_progress;

    Correlation* m_correlation;

private slots:
    void start();
    void slotProgress();
    void slotFinished();

signals:
    void finished();
};

#endif /* TENSORTRACKWIDGET_H_ */
