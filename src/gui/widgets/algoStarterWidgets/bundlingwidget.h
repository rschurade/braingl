/*
 * bundlingwidget.h
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#ifndef BUNDLINGWIDGET_H_
#define BUNDLINGWIDGET_H_

#include "../../../data/enums.h"
#include "../../../algos/connections.h"

#include <QtWidgets>

class QProgressBar;
class QPushButton;
class Dataset;

class BundlingWidget : public QWidget
{
    Q_OBJECT

public:
    //TODO: constructor, pointers to connections
    BundlingWidget( Connections* cons, QWidget* parent = 0 );
    virtual ~BundlingWidget();

    QList<Dataset*> getBundles();

private:
    QVBoxLayout* m_layout;
    QPushButton* m_startButton;
    QProgressBar* m_progressBar;
    int m_progress;

    Connections* m_cons;

private slots:
    void start();
    void slotProgress();
    void slotFinished();

signals:
    void finished();
};

#endif /* BUNDLINGWIDGET_H_ */
