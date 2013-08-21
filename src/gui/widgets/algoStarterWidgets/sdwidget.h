/*
 * sdwidget.h
 *
 *  Created on: Jul 22, 2013
 *      Author: schurade
 */

#ifndef SDWIDGET_H_
#define SDWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui>
#include <QList>

class Dataset;
class SD;

class SDWidget : public QWidget
{
    Q_OBJECT

public:
    SDWidget( Dataset* ds, QList<QVariant> &dsl, QWidget* parent = 0 );
    virtual ~SDWidget();

    QList<Dataset*>getResult();

private:
    QVBoxLayout* m_layout;

    SD* m_sd;

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

#endif /* SDWIDGET_H_ */
