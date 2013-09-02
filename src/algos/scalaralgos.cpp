/*
 * scalaralgos.cpp
 *
 * Created on: 09.07.2012
 * @author Ralph Schurade
 */
#include "scalaralgos.h"

#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasetisosurface.h"

#include "../data/models.h"

ScalarAlgos::ScalarAlgos()
{
}

ScalarAlgos::~ScalarAlgos()
{
}

QList<Dataset*> ScalarAlgos::isoSurface( Dataset* ds )
{
    DatasetIsosurface* iso = new DatasetIsosurface( dynamic_cast<DatasetScalar*>( ds ) );
    QList<Dataset*> l;
    l.push_back( iso );
    return l;
}

QList<Dataset*> ScalarAlgos::distanceMap( Dataset* ds )
{
    QVector<float>* data = static_cast<DatasetScalar*>( ds )->getData();
    QVector<float> out( data->size() );

    int b, r, c, bb, rr, r0, b0, c0;
    int i, istart, iend;
    int nz, ny, nx, npixels;
    int d, d1, d2, cc1, cc2;
    float u, dmin, dmax;
    bool *srcpix;
    double g, *array;

    nx = ds->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
    ny = ds->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
    nz = ds->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();

    npixels = qMax( nz, ny );
    array = new double[npixels];

    npixels = nz * ny * nx;

    out.resize( npixels );
    for ( int i = 0; i < npixels; ++i )
    {
        out[i] = 0.0;
    }

    bool* bitmask = new bool[npixels];
    for ( int i = 0; i < npixels; ++i )
    {
        if ( data->at( i ) < 0.01 )
            bitmask[i] = true;
        else
            bitmask[i] = false;
    }

    dmax = 999999999.0f;

    // first pass
    for ( b = 0; b < nz; ++b )
    {
        for ( r = 0; r < ny; ++r )
        {
            for ( c = 0; c < nx; ++c )
            {
                //if (VPixel(src,b,r,c,VBit) == 1)
                if ( bitmask[b * ny * nx + r * nx + c] )
                {
                    out[b * ny * nx + r * nx + c] = 0;
                    continue;
                }

                srcpix = bitmask + b * ny * nx + r * nx + c;
                cc1 = c;
                while ( cc1 < nx && *srcpix++ == 0 )
                    cc1++;
                d1 = ( cc1 >= nx ? nx : ( cc1 - c ) );

                srcpix = bitmask + b * ny * nx + r * nx + c;
                cc2 = c;
                while ( cc2 >= 0 && *srcpix-- == 0 )
                    cc2--;
                d2 = ( cc2 <= 0 ? nx : ( c - cc2 ) );

                if ( d1 <= d2 )
                {
                    d = d1;
                    c0 = cc1;
                }
                else
                {
                    d = d2;
                    c0 = cc2;
                }
                out[b * ny * nx + r * nx + c] = (float) ( d * d );
            }
        }
    }

    // second pass
    for ( b = 0; b < nz; ++b )
    {
        for ( c = 0; c < nx; ++c )
        {
            for ( r = 0; r < ny; ++r )
                array[r] = (double) out[b * ny * nx + r * nx + c];

            for ( r = 0; r < ny; ++r )
            {
                if ( bitmask[b * ny * nx + r * nx + c] == 1 )
                    continue;

                dmin = dmax;
                r0 = r;
                g = sqrt( array[r] );
                istart = r - (int) g;
                if ( istart < 0 )
                    istart = 0;
                iend = r + (int) g + 1;
                if ( iend >= ny )
                    iend = ny;

                for ( rr = istart; rr < iend; rr++ )
                {
                    u = array[rr] + ( r - rr ) * ( r - rr );
                    if ( u < dmin )
                    {
                        dmin = u;
                        r0 = rr;
                    }
                }
                out[b * ny * nx + r * nx + c] = dmin;
            }
        }
    }

    // third pass

    for ( r = 0; r < ny; ++r )
    {
        for ( c = 0; c < nx; ++c )
        {
            for ( b = 0; b < nz; ++b )
                array[b] = (double) out[b * ny * nx + r * nx + c];

            for ( b = 0; b < nz; ++b )
            {
                if ( bitmask[b * ny * nx + r * nx + c] == 1 )
                    continue;

                dmin = dmax;
                b0 = b;

                g = sqrt( array[b] );
                istart = b - (int) g - 1;
                if ( istart < 0 )
                    istart = 0;
                iend = b + (int) g + 1;
                if ( iend >= nz )
                    iend = nz;

                for ( bb = istart; bb < iend; bb++ )
                {
                    u = array[bb] + ( b - bb ) * ( b - bb );
                    if ( u < dmin )
                    {
                        dmin = u;
                        b0 = bb;
                    }
                }
                out[b * ny * nx + r * nx + c] = dmin;
            }
        }
    }

    delete[] array;

    float max = 0;
    for ( i = 0; i < npixels; ++i )
    {
        out[i] = sqrt( (double) out[i] );
        if ( out[i] > max )
            max = out[i];
    }
    for ( i = 0; i < npixels; ++i )
    {
        out[i] = out[i] / max;

    }

    // filter with gauss
    // create the filter kernel
    double sigma = 3;

    int dim = (int) ( 3.0 * sigma + 1 );
    int n = 2 * dim + 1;
    double step = 1;

    float* kernel = new float[n];

    double sum = 0;
    double x = -(float) dim;

    double uu;
    for ( int i = 0; i < n; ++i )
    {
        uu = xxgauss( x, sigma );
        sum += uu;
        kernel[i] = uu;
        x += step;
    }

    /* normalize */
    for ( int i = 0; i < n; ++i )
    {
        uu = kernel[i];
        uu /= sum;
        kernel[i] = uu;
    }

    d = n / 2;
    float* float_pp;
    QVector<float> tmp( npixels );
    int c1, cc;

    for ( int i = 0; i < npixels; ++i )
    {
        tmp[i] = 0.0;
    }

    for ( b = 0; b < nz; ++b )
    {
        for ( r = 0; r < ny; ++r )
        {
            for ( c = d; c < nx - d; ++c )
            {

                float_pp = kernel;
                sum = 0;
                c0 = c - d;
                c1 = c + d;
                for ( cc = c0; cc <= c1; ++cc )
                {
                    x = out[b * ny * nx + r * nx + cc];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * ny * nx + r * nx + c] = sum;
            }
        }
    }
    int r1;
    for ( b = 0; b < nz; ++b )
    {
        for ( r = d; r < ny - d; ++r )
        {
            for ( c = 0; c < nx; ++c )
            {
                float_pp = kernel;
                sum = 0;
                r0 = r - d;
                r1 = r + d;
                for ( rr = r0; rr <= r1; rr++ )
                {
                    x = tmp[b * ny * nx + rr * nx + c];
                    sum += x * ( *float_pp++ );
                }
                out[b * ny * nx + r * nx + c] = sum;
            }
        }
    }
    int b1;
    for ( b = d; b < nz - d; ++b )
    {
        for ( r = 0; r < ny; ++r )
        {
            for ( c = 0; c < nx; ++c )
            {

                float_pp = kernel;
                sum = 0;
                b0 = b - d;
                b1 = b + d;
                for ( bb = b0; bb <= b1; bb++ )
                {
                    x = out[bb * ny * nx + r * nx + c];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * ny * nx + r * nx + c] = sum;
            }
        }
    }
    delete[] bitmask;
    delete[] kernel;

    out = tmp;

    DatasetScalar* dsOut = new DatasetScalar( QDir( "distance map" ), out, static_cast<DatasetScalar*>( ds )->getHeader() );

    QList<Dataset*> l;
    l.push_back( dsOut );
    return l;
}

QList<Dataset*> ScalarAlgos::gauss( Dataset* ds )
{
    QVector<float>* data = static_cast<DatasetScalar*>( ds )->getData();
    QVector<float> out( data->size() );

    for ( int i = 0; i < data->size(); ++i )
    {
        out[i] = data->at( i );
    }

    int b, r, c, bb, rr, r0, b0, c0;

    int nx = ds->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
    int ny = ds->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
    int nz = ds->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();
    int npixels = nz * ny * nx;

    // create the filter kernel
    double sigma = Models::getGlobal( Fn::Property::G_FILTER_SIZE ).toInt();

    int dim = (int) ( sigma + 1 );
    int n = 2 * dim + 1;
    double step = 1;

    float* kernel = new float[n];

    double sum = 0;
    double x = -(float) dim;

    double uu;
    for ( int i = 0; i < n; ++i )
    {
        uu = xxgauss( x, sigma );
        sum += uu;
        kernel[i] = uu;
        x += step;
    }

    /* normalize */
    for ( int i = 0; i < n; ++i )
    {
        uu = kernel[i];
        uu /= sum;
        kernel[i] = uu;
    }

    int d = n / 2;
    float* float_pp;
    QVector<float> tmp( npixels );
    int c1, cc;

    for ( int i = 0; i < npixels; ++i )
    {
        tmp[i] = 0.0;
    }

    for ( b = 0; b < nz; ++b )
    {
        for ( r = 0; r < ny; ++r )
        {
            for ( c = d; c < nx - d; ++c )
            {

                float_pp = kernel;
                sum = 0;
                c0 = c - d;
                c1 = c + d;
                for ( cc = c0; cc <= c1; ++cc )
                {
                    x = out[b * ny * nx + r * nx + cc];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * ny * nx + r * nx + c] = sum;
            }
        }
    }
    int r1;
    for ( b = 0; b < nz; ++b )
    {
        for ( r = d; r < ny - d; ++r )
        {
            for ( c = 0; c < nx; ++c )
            {
                float_pp = kernel;
                sum = 0;
                r0 = r - d;
                r1 = r + d;
                for ( rr = r0; rr <= r1; rr++ )
                {
                    x = tmp[b * ny * nx + rr * nx + c];
                    sum += x * ( *float_pp++ );
                }
                out[b * ny * nx + r * nx + c] = sum;
            }
        }
    }
    int b1;
    for ( b = d; b < nz - d; ++b )
    {
        for ( r = 0; r < ny; ++r )
        {
            for ( c = 0; c < nx; ++c )
            {

                float_pp = kernel;
                sum = 0;
                b0 = b - d;
                b1 = b + d;
                for ( bb = b0; bb <= b1; bb++ )
                {
                    x = out[bb * ny * nx + r * nx + c];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * ny * nx + r * nx + c] = sum;
            }
        }
    }

    delete[] kernel;

    out = tmp;

    DatasetScalar* dsOut = new DatasetScalar( QDir( "gauss" ), out, static_cast<DatasetScalar*>( ds )->getHeader() );

    QList<Dataset*> l;
    l.push_back( dsOut );
    return l;
}

double ScalarAlgos::xxgauss(double x, double sigma)
{
    double y, z, a = 2.506628273;
    z = x / sigma;
    y = exp((double) -z * z * 0.5) / (sigma * a);
    return y;
}

QList<Dataset*> ScalarAlgos::createNew( Dataset* ds )
{
    QVector<float>* data = static_cast<DatasetScalar*>( ds )->getData();
    QVector<float> out( data->size() );

    float max = static_cast<DatasetScalar*>( ds )->properties( "maingl" )->get( Fn::Property::D_MAX ).toFloat();
    out[0] = max;


    DatasetScalar* dsOut = new DatasetScalar( QDir( "new scalar dataset" ), out, static_cast<DatasetScalar*>( ds )->getHeader() );
    QList<Dataset*> l;
    l.push_back( dsOut );
    return l;
}

QList<Dataset*> ScalarAlgos::median( Dataset* ds )
{
    DatasetScalar* dss = static_cast<DatasetScalar*>( ds );

    QVector<float>* data = dss->getData();
    QVector<float> out( data->size() );

    int nx = ds->properties( "maingl" )->get( Fn::Property::D_NX ).toInt();
    int ny = ds->properties( "maingl" )->get( Fn::Property::D_NY ).toInt();
    int nz = ds->properties( "maingl" )->get( Fn::Property::D_NZ ).toInt();

    // create the filter kernel
    int dist = Models::getGlobal( Fn::Property::G_FILTER_SIZE ).toInt();

    for ( int z = 0; z < nz; ++z )
    {
        for ( int y = 0; y < ny; ++y )
        {
        for ( int x = 0; x < nx; ++x )
            {
                QList<int> neighs = dss->getNeighbourhoodXxX( x, y, z, dist );
                QList<float>values;
                for ( int i = 0; i < neighs.size(); ++i )
                {
                    values.push_back( data->at( neighs[i] ) );
                }
                qSort( values );
                out[dss->getId( x, y, z)] = values.at( values.size() / 2 );
            }
        }
    }

    DatasetScalar* dsOut = new DatasetScalar( QDir( "median" ), out, static_cast<DatasetScalar*>( ds )->getHeader() );

    QList<Dataset*> l;
    l.push_back( dsOut );
    return l;
}
