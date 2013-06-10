/*
 * fiberselector.cpp
 *
 * Created on: Feb 26, 2013
 * @author schurade
 */

#include "fiberselector.h"

#include "../models.h"
#include "../enums.h"
#include "../vptr.h"
#include "../roi.h"
#include "../roiarea.h"

#include <QDebug>

#include <math.h>

FiberSelector::FiberSelector() :
    m_numLines( 0 ),
    m_numPoints( 0 ),
    m_isInitialized( false )
{
    m_boxMin.resize( 3 );
    m_boxMax.resize( 3 );
}

FiberSelector::~FiberSelector()
{
}

QVector<bool>* FiberSelector::getSelection()
{
    return &m_rootfield;
}


void FiberSelector::init( QVector< QVector< float > >& data )
{
    qDebug() << "start creating kdtree";
    m_numLines = data.size();
    int ls = 0;
    for ( int i = 0; i < m_numLines; ++i )
    {
        m_lineStarts.push_back( ls );
        m_lineLengths.push_back( data[i].size() / 3 );
        for( int k = 0; k < data[i].size() / 3; ++k )
        {
            m_kdVerts.push_back( data[i][k * 3    ] );
            m_kdVerts.push_back( data[i][k * 3 + 1] );
            m_kdVerts.push_back( data[i][k * 3 + 2] );
            m_reverseIndexes.push_back( i );
            ++m_numPoints;
            ++ls;
        }
    }
    m_kdTree = new KdTree( m_numPoints, m_kdVerts.data() );
    qDebug() << "end creating kdtree";

    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( roiChanged( QModelIndex, QModelIndex ) ) );
    connect( Models::r(), SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( roiInserted( QModelIndex, int, int ) ) );
    connect( Models::r(), SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( roiDeleted( QModelIndex, int, int ) ) );

    m_rootfield.resize( m_numLines );
    for ( int i = 0; i < m_numLines; ++i )
    {
        m_rootfield[i] = true;
    }

    updatePresentRois();

}

void FiberSelector::updatePresentRois()
{
    qDebug() << "updatePresentRois";
    int numBranches = Models::r()->rowCount( QModelIndex() );

    for ( int i = 0; i < numBranches; ++i )
    {
        QList<QVector<bool> >newBranch;
        QVector<bool>newLeaf( m_numLines );
        newBranch.push_back( newLeaf );
        m_branchfields.push_back( newLeaf );
        m_bitfields.push_back( newBranch );
        updateROI( m_bitfields.size() - 1, 0 );

        int leafCount = Models::r()->rowCount( createIndex( i, 0, 0 ) );

        for ( int k = 0; k < leafCount; ++k )
        {
            // inserted child roi
            QVector<bool>newLeaf( m_numLines );
            m_bitfields[i].push_back( newLeaf );
            updateROI( i, k + 1 );
        }
    }
}

void FiberSelector::roiChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight )
{
    if ( topLeft.row() == -1 ) return;
    //qDebug() << "roi changed" << topLeft.row() << topLeft.column() << topLeft.internalId();
    int branch = 0;
    int pos = 0;
    if ( topLeft.internalId() == -1 )
    {
        // top level box
        branch = topLeft.row();
    }
    else
    {
        // child box
        branch = topLeft.internalId();
        pos = topLeft.row() + 1;
    }
    updateROI( branch, pos );

}

void FiberSelector::roiInserted( const QModelIndex &parent, int start, int end )
{
    //qDebug() << "roi inserted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        // inserted top level roi
        QList<QVector<bool> >newBranch;
        QVector<bool>newLeaf( m_numLines );
        newBranch.push_back( newLeaf );
        m_branchfields.push_back( newLeaf );
        m_bitfields.push_back( newBranch );
        updateROI( m_bitfields.size() - 1, 0 );
    }
    else
    {
        // inserted child roi
        QVector<bool>newLeaf( m_numLines );
        m_bitfields[parent.row()].push_back( newLeaf );
        updateROI( parent.row(), m_bitfields[parent.row()].size() - 1 );
    }
}

void FiberSelector::roiDeleted( const QModelIndex &parent, int start, int end )
{
    //qDebug() << "roi deleted" << parent.row() << start << end;
    if ( parent.row() == -1 )
    {
        m_bitfields.removeAt( start );
        m_branchfields.removeAt( start );
        updateRoot();
    }
    else
    {
        m_bitfields[parent.row()].removeAt( start + 1 );
        updateBranch( parent.row() );
    }
}

QModelIndex FiberSelector::createIndex( int branch, int pos, int column )
{
    int row;
    QModelIndex parent;
    if ( pos == 0 )
    {
        row = branch;
    }
    else
    {
        row = pos - 1;
        parent = Models::r()->index( branch, 0 );
    }
    return Models::r()->index( row, column, parent );
}

void FiberSelector::updateROI( int branch, int pos )
{
	int shape = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::SHAPE ), Qt::DisplayRole ).toInt();
    if ( Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
    {
        if ( shape == 10 )
        {
            ROIArea* roi = VPtr<ROIArea>::asPtr( Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::POINTER ), Qt::DisplayRole ) );
            float threshold = roi->properties()->get( Fn::ROI::THRESHOLD ).toFloat();
            QVector<float>* data = roi->data();
            int nx = roi->properties()->get( Fn::ROI::NX ).toInt();
            int ny = roi->properties()->get( Fn::ROI::NY ).toInt();
            int nz = roi->properties()->get( Fn::ROI::NZ ).toInt();
            float dx = roi->properties()->get( Fn::ROI::DX ).toFloat();
            float dy = roi->properties()->get( Fn::ROI::DY ).toFloat();
            float dz = roi->properties()->get( Fn::ROI::DZ ).toFloat();
            for ( int i = 0; i < m_numLines; ++i )
            {
                m_bitfields[branch][pos][i] = false;
            }
            for ( int i = 0; i < m_kdVerts.size() / 3; ++i )
            {
                float x = m_kdVerts[i*3];
                float y = m_kdVerts[i*3+1];
                float z = m_kdVerts[i*3+2];

                int px = x / dx;
                int py = y / dy;
                int pz = z / dz;

                px = qMax( 0, qMin( px, nx - 1) );
                py = qMax( 0, qMin( py, ny - 1) );
                pz = qMax( 0, qMin( pz, nz - 1) );

                int id = px + py * nx + pz * nx * ny;

                if( data->at( id) - threshold > 0 )
                {
                    m_bitfields[branch][pos][m_reverseIndexes[ i ]] = true;
                }
            }
        }
        else
        {
            m_x = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::X ), Qt::DisplayRole ).toFloat();
            m_y = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::Y ), Qt::DisplayRole ).toFloat();
            m_z = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::Z ), Qt::DisplayRole ).toFloat();
            m_dx = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::DX ), Qt::DisplayRole ).toFloat() / 2;
            m_dy = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::DY ), Qt::DisplayRole ).toFloat() / 2;
            m_dz = Models::r()->data( createIndex( branch, pos, (int)Fn::ROI::DZ ), Qt::DisplayRole ).toFloat() / 2;
            m_boxMin[0] = m_x - m_dx;
            m_boxMax[0] = m_x + m_dx;
            m_boxMin[1] = m_y - m_dy;
            m_boxMax[1] = m_y + m_dy;
            m_boxMin[2] = m_z - m_dz;
            m_boxMax[2] = m_z + m_dz;

            for ( int i = 0; i < m_numLines; ++i )
            {
                m_bitfields[branch][pos][i] = false;
            }

            boxTest( m_bitfields[branch][pos], 0, m_numPoints - 1, 0 );

            if ( shape == 0 || shape == 1 )
            {
                sphereTest( m_bitfields[branch][pos] );
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_bitfields[branch][pos][i] = true;
        }
    }

    updateBranch( branch );
    if ( m_isInitialized )
    {
        Models::r()->setData( createIndex( branch, pos, (int)Fn::ROI::UPDATED ), true, Qt::DisplayRole );
    }
}

void FiberSelector::boxTest( QVector<bool>& workfield, int left, int right, int axis )
{
    // abort condition
    if ( left > right )
        return;

    int root = left + ( ( right - left ) / 2 );
    int axis1 = ( axis + 1 ) % 3;
    int pointIndex = m_kdTree->m_tree[root] * 3;

    if ( m_kdVerts[pointIndex + axis] < m_boxMin[axis] )
    {
        boxTest( workfield, root + 1, right, axis1 );
    }
    else if ( m_kdVerts[pointIndex + axis] > m_boxMax[axis] )
    {
        boxTest( workfield, left, root - 1, axis1 );
    }
    else
    {
        int axis2 = ( axis + 2 ) % 3;
        if ( m_kdVerts[pointIndex + axis1] <= m_boxMax[axis1] && m_kdVerts[pointIndex + axis1]
                >= m_boxMin[axis1] && m_kdVerts[pointIndex + axis2] <= m_boxMax[axis2]
                && m_kdVerts[pointIndex + axis2] >= m_boxMin[axis2] )
        {
            workfield[m_reverseIndexes[ m_kdTree->m_tree[root] ]] = true;
        }
        boxTest( workfield, left, root - 1, axis1 );
        boxTest( workfield, root + 1, right, axis1 );
    }
}

void FiberSelector::sphereTest( QVector<bool>& workfield )
{
    float vx,vy,vz;
    bool hit;
    for ( int i = 0; i < m_numLines; ++i )
    {
        int ls = m_lineStarts[i] * 3;
        if ( workfield[i] )
        {
            hit = false;
            for ( int k = 0; k < m_lineLengths[i]; ++k )
            {
                vx = ( m_kdVerts[ ls + k*3    ] - m_x ) / m_dx;
                vy = ( m_kdVerts[ ls + k*3 + 1] - m_y ) / m_dy;
                vz = ( m_kdVerts[ ls + k*3 + 2] - m_z ) / m_dz;
                float r = sqrt( vx * vx + vy * vy + vz * vz );
                if ( r < 1.0 )
                {
                    hit = true;
                    break;
                }
            }
            workfield[i] = hit;
        }
    }
}

void FiberSelector::updateBranch( int branch )
{
    //qDebug() << "update branch" << branch;
    int current = 0;

    bool neg = Models::r()->data( createIndex( branch, current, (int)Fn::ROI::NEG ), Qt::DisplayRole ).toBool();
    if ( neg )
    {
        for ( int k = 0; k < m_numLines; ++k )
        {
            m_branchfields[branch][k] = !m_bitfields[branch][current][k];
        }
    }
    else
    {
        for ( int k = 0; k < m_numLines; ++k )
        {
            m_branchfields[branch][k] = m_bitfields[branch][current][k];
        }
    }
    ++current;

    while ( current < m_bitfields[branch].size() )
    {
        if ( Models::r()->data( createIndex( branch, current, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
        {
            bool neg = Models::r()->data( createIndex( branch, current, (int)Fn::ROI::NEG ), Qt::DisplayRole ).toBool();
            if ( neg )
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_branchfields[branch][k] &= !m_bitfields[branch][current][k];
                }
            }
            else
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_branchfields[branch][k] &= m_bitfields[branch][current][k];
                }
            }
            ++current;
        }
        else
        {
            ++current;
        }
    }
    updateRoot();
}

void FiberSelector::updateRoot()
{
    //qDebug() << "update root";
    if ( m_branchfields.size() > 0 )
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_rootfield[i] = false;
        }

        for ( int i = 0; i < m_branchfields.size(); ++i )
        {
            if ( Models::r()->data( createIndex( i, 0, (int)Fn::ROI::ACTIVE ), Qt::DisplayRole ).toBool() )
            {
                for ( int k = 0; k < m_numLines; ++k )
                {
                    m_rootfield[k] |= m_branchfields[i][k];
                }
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_rootfield[i] = true;
        }
    }
    emit( changed() );
}
