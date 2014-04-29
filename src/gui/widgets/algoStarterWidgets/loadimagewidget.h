/*
 * loadimagewidget.h
 *
 *  Created on: 26.04.2014
 *      Author: Ralph
 */

#ifndef LOADIMAGEWIDGET_H_
#define LOADIMAGEWIDGET_H_

#include <QtGui>

class CheckBox;
class SelectWithLabel;
class SliderWithEditInt;

class LoadImageWidget  : public QDialog
{
    Q_OBJECT

public:
    LoadImageWidget( QWidget* parent = 0 );
    virtual ~LoadImageWidget();

    int getOrient();

    SliderWithEditInt* m_thickness;
    SliderWithEditInt* m_thickness2;
    SliderWithEditInt* m_offset;
    SliderWithEditInt* m_paddingLeftRight;
    SliderWithEditInt* m_paddingTopBottom;
    CheckBox*  m_whiteTransparent;

private:
    QVBoxLayout* m_layout;

    SelectWithLabel* m_orientSelect;

    QPushButton* m_okButton;

signals:

public slots:
    void ok();
};

#endif /* LOADIMAGEWIDGET_H_ */
