/*
 * checkboxwithlabel.h
 *
 * Created on: 14.07.2012
 * @author Ralph Schurade
 */

#ifndef CHECKBOXWITHLABEL_H_
#define CHECKBOXWITHLABEL_H_

#include "propertywidget.h"

class QCheckBox;
class QLabel;


class CheckBox  : public PropertyWidget
{
    Q_OBJECT

public:
    CheckBox(  QString label, int id = -1, QWidget* parent = 0 );
    CheckBox(  int id = -1, QWidget* parent = 0 );
    virtual ~CheckBox();

    void setChecked( bool state );
    void check();
    void uncheck();
    bool checked();

private:

signals:
    void stateChanged( int state, int id );

public slots:
    void slotStateChanged( int state );
    void slotSetChecked2( int state, int id );
};

#endif /* CHECKBOXWITHLABEL_H_ */
