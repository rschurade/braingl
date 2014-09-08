/*
 * fiberbundlewidget.h
 *
 * Created on: 04.11.2013
 * @author Ralph Schurade
 */

#ifndef FIBERBUNDLEWIDGET_H_
#define FIBERBUNDLEWIDGET_H_

#include "../../../data/enums.h"

#include <QtWidgets>

class QProgressBar;
class QPushButton;
class Dataset;
class Bundle;
class SliderWithEditInt;

class FiberBundleWidget : public QWidget
{
    Q_OBJECT

public:
    FiberBundleWidget( Dataset* ds, QWidget* parent = 0 );
    virtual ~FiberBundleWidget();

    QList<Dataset*> getFibs();

private:
    QVBoxLayout* m_layout;

    Bundle* m_bundler;

    QPushButton* m_startButton;
    QProgressBar* m_progressBar;
    int m_progress;

    SliderWithEditInt* m_iterations;

private slots:
    void start();
    void slotProgress();
    void slotFinished();

signals:
    void finished();
};

#endif /* FIBERBUNDLEWIDGET_H_ */
