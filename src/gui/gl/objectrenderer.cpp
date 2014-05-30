/*
 * objectrenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "objectrenderer.h"

#include "../../data/enums.h"

#include <QDebug>

ObjectRenderer::ObjectRenderer() :
	m_previousSettings( "" )
{
}

ObjectRenderer::~ObjectRenderer()
{
}

QString ObjectRenderer::createSettingsString( std::initializer_list<QVariant>settings )
{
    QString result("");

    for ( auto i = settings.begin(); i != settings.end(); ++i )
    {
        result += (*i).toString();
    }
    return result;
}
