/*
 * models.cpp
 *
 * Created on: Mar 18, 2013
 * @author schurade
 */

#include "models.h"

#include "datastore.h"
#include "globalpropertymodel.h"
#include "roimodel.h"
#include "roi.h"
#include "roiarea.h"
#include "vptr.h"

#include "datasets/dataset.h"
#include "../gui/widgets/roiwidget.h"

QAbstractItemModel* Models::m_dataModel = 0;
QAbstractItemModel* Models::m_globalModel = 0;
QAbstractItemModel* Models::m_roiModel = 0;
ROIWidget* Models::m_roiWidget = 0;
QMap<QString, Fn::Property>Models::m_propMap;

void Models::init()
{
    m_globalModel = new GlobalPropertyModel();
    m_dataModel = new DataStore();
    m_roiModel = new ROIModel();
    initPropMap();
}

QAbstractItemModel* Models::globalModel()
{
    return m_globalModel;
}

QAbstractItemModel* Models::dataModel()
{
    return m_dataModel;
}

QAbstractItemModel* Models::RoiModel()
{
    return m_roiModel;
}

QAbstractItemModel* Models::g()
{
    return m_globalModel;
}

QAbstractItemModel* Models::d()
{
    return m_dataModel;
}

QAbstractItemModel* Models::r()
{
    return m_roiModel;
}

void Models::setGlobal( int var, QVariant value )
{
    m_globalModel->setData( m_globalModel->index( var, 0 ), value );
}


void Models::setGlobal( Fn::Property var, QVariant value )
{
    m_globalModel->setData( m_globalModel->index( (int)var, 0 ), value );
}

QVariant Models::getGlobal( Fn::Property var )
{
    return m_globalModel->data( m_globalModel->index( (int)var, 0 ) );
}

QVariant Models::getGlobal( int var )
{
    return m_globalModel->data( m_globalModel->index( var, 0 ) );
}

ROI* Models::getRoi( int branch, int pos )
{
    ROI* roi = VPtr<ROI>::asPtr( m_roiModel->data( createRoiIndex( branch, pos, (int)Fn::Property::D_POINTER ), Qt::DisplayRole ) );
    return roi;
}

int Models::getBranchSize( int branch )
{
    return m_roiModel->rowCount( createRoiIndex( branch, 0, 0 ) );
}

void Models::setROIProp( int branch, int pos, int prop, QVariant value )
{
    m_roiModel->setData( createRoiIndex( branch, pos, prop ), value, Qt::DisplayRole );
}

void Models::setROIProp( int branch, int pos, Fn::Property prop, QVariant value )
{
    m_roiModel->setData( createRoiIndex( branch, pos, (int)prop ), value, Qt::DisplayRole );
}


QModelIndex Models::createRoiIndex( int branch, int pos, int column )
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
        parent = m_roiModel->index( branch, 0 );
    }
    return m_roiModel->index( row, column, parent );
}

Dataset* Models::getDataset( int id )
{
    return VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( id, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
}

QList<Dataset*> Models::getDatasets( Fn::DatasetType filter, bool ignoreInactive )
{
    QList<Dataset*>rl;
    int countDatasets = m_dataModel->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataModel->index( i, (int)Fn::Property::D_ACTIVE );
        if ( m_dataModel->data( index, Qt::DisplayRole ).toBool() || !ignoreInactive )
        {
            index = m_dataModel->index( i, (int)Fn::Property::D_TYPE );
            if ( m_dataModel->data( index, Qt::DisplayRole ).toInt() & (int)filter )
            {
                Dataset* ds = VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
                rl.push_back( ds );
            }
        }
    }
    return rl;
}

QList<Dataset*> Models::getDatasets( int filter, bool ignoreInactive )
{
    QList<Dataset*>rl;
    int countDatasets = m_dataModel->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = m_dataModel->index( i, (int)Fn::Property::D_ACTIVE );
        if ( m_dataModel->data( index, Qt::DisplayRole ).toBool() || !ignoreInactive )
        {
            index = m_dataModel->index( i, (int)Fn::Property::D_TYPE );
            if ( m_dataModel->data( index, Qt::DisplayRole ).toInt() & filter )
            {
                Dataset* ds = VPtr<Dataset>::asPtr( m_dataModel->data( m_dataModel->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
                rl.push_back( ds );
            }
        }
    }
    return rl;
}


void Models::setRoiWidget( ROIWidget* rw )
{
    m_roiWidget = rw;
}

void Models::addROIArea( ROIArea* roi )
{
    m_roiWidget->addROIArea( roi );
}

void Models::addDataset( Dataset* ds )
{
    Models::d()->setData( Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET ), VPtr<Dataset>::asQVariant( ds ), Qt::DisplayRole );
}



void Models::initPropMap()
{
    m_propMap.insert( "D_NAME", Fn::Property::D_NAME );
    m_propMap.insert( "D_DIM", Fn::Property::D_DIM );
    m_propMap.insert( "D_DATATYPE", Fn::Property::D_DATATYPE );
    m_propMap.insert( "D_SIZE", Fn::Property::D_SIZE );
    m_propMap.insert( "D_NX", Fn::Property::D_NX );
    m_propMap.insert( "D_NY", Fn::Property::D_NY );
    m_propMap.insert( "D_NZ", Fn::Property::D_NZ );
    m_propMap.insert( "D_DX", Fn::Property::D_DX );
    m_propMap.insert( "D_DY", Fn::Property::D_DY );
    m_propMap.insert( "D_DZ", Fn::Property::D_DZ );
    m_propMap.insert( "D_MIN", Fn::Property::D_MIN );
    m_propMap.insert( "D_MAX", Fn::Property::D_MAX );
    m_propMap.insert( "D_TYPE", Fn::Property::D_TYPE );
    m_propMap.insert( "D_CREATED_BY", Fn::Property::D_CREATED_BY );
    m_propMap.insert( "D_FILENAME", Fn::Property::D_FILENAME );
    m_propMap.insert( "D_SELECTED_MIN", Fn::Property::D_SELECTED_MIN );
    m_propMap.insert( "D_SELECTED_MAX", Fn::Property::D_SELECTED_MAX );
    m_propMap.insert( "D_LOWER_THRESHOLD", Fn::Property::D_LOWER_THRESHOLD );
    m_propMap.insert( "D_UPPER_THRESHOLD", Fn::Property::D_UPPER_THRESHOLD );
    m_propMap.insert( "D_COLORMAP", Fn::Property::D_COLORMAP );
    m_propMap.insert( "D_RENDER_COLORMAP", Fn::Property::D_RENDER_COLORMAP );
    m_propMap.insert( "D_COLORMAP_ORIENT", Fn::Property::D_COLORMAP_ORIENT );
    m_propMap.insert( "D_COLORMAP_X", Fn::Property::D_COLORMAP_X );
    m_propMap.insert( "D_COLORMAP_Y", Fn::Property::D_COLORMAP_Y );
    m_propMap.insert( "D_COLORMAP_DX", Fn::Property::D_COLORMAP_DX );
    m_propMap.insert( "D_COLORMAP_DY", Fn::Property::D_COLORMAP_DY );
    m_propMap.insert( "D_COLORMAP_TEXT_SIZE", Fn::Property::D_COLORMAP_TEXT_SIZE );
    m_propMap.insert( "D_COLORMAP_TEXT_COLOR", Fn::Property::D_COLORMAP_TEXT_COLOR );
    m_propMap.insert( "D_INTERPOLATION", Fn::Property::D_INTERPOLATION );
    m_propMap.insert( "D_ALPHA", Fn::Property::D_ALPHA );
    m_propMap.insert( "D_ACTIVE", Fn::Property::D_ACTIVE );
    m_propMap.insert( "D_ORDER", Fn::Property::D_ORDER );
    m_propMap.insert( "D_LOD", Fn::Property::D_LOD );
    m_propMap.insert( "D_SCALING", Fn::Property::D_SCALING );
    m_propMap.insert( "D_FA_THRESHOLD", Fn::Property::D_FA_THRESHOLD );
    m_propMap.insert( "D_EV_THRESHOLD", Fn::Property::D_EV_THRESHOLD );
    m_propMap.insert( "D_GAMMA", Fn::Property::D_GAMMA );
    m_propMap.insert( "D_OFFSET", Fn::Property::D_OFFSET );
    m_propMap.insert( "D_RENDER_SLICE", Fn::Property::D_RENDER_SLICE );
    m_propMap.insert( "D_RENDER_SAGITTAL", Fn::Property::D_RENDER_SAGITTAL );
    m_propMap.insert( "D_RENDER_CORONAL", Fn::Property::D_RENDER_CORONAL );
    m_propMap.insert( "D_RENDER_AXIAL", Fn::Property::D_RENDER_AXIAL );
    m_propMap.insert( "D_MINMAX_SCALING", Fn::Property::D_MINMAX_SCALING );
    m_propMap.insert( "D_HIDE_NEGATIVE_LOBES", Fn::Property::D_HIDE_NEGATIVE_LOBES );
    m_propMap.insert( "D_BVALUE", Fn::Property::D_BVALUE );
    m_propMap.insert( "D_TENSOR_RENDERMODE", Fn::Property::D_TENSOR_RENDERMODE );
    m_propMap.insert( "D_RENDER_FIRST", Fn::Property::D_RENDER_FIRST );
    m_propMap.insert( "D_RENDER_SECOND", Fn::Property::D_RENDER_SECOND );
    m_propMap.insert( "D_RENDER_THIRD", Fn::Property::D_RENDER_THIRD );
    m_propMap.insert( "D_RENDER_VECTORS_STICKS", Fn::Property::D_RENDER_VECTORS_STICKS );
    m_propMap.insert( "D_RENDER_VECTORS_STIPPLES", Fn::Property::D_RENDER_VECTORS_STIPPLES );
    m_propMap.insert( "D_ISO_VALUE", Fn::Property::D_ISO_VALUE );
    m_propMap.insert( "D_NUM_POINTS", Fn::Property::D_NUM_POINTS );
    m_propMap.insert( "D_NUM_LINES", Fn::Property::D_NUM_LINES );
    m_propMap.insert( "D_COLOR", Fn::Property::D_COLOR );
    m_propMap.insert( "D_COLORMODE", Fn::Property::D_COLORMODE );
    m_propMap.insert( "D_DATAMODE", Fn::Property::D_DATAMODE );
    m_propMap.insert( "D_PAINTMODE", Fn::Property::D_PAINTMODE );
    m_propMap.insert( "D_PAINTSIZE", Fn::Property::D_PAINTSIZE );
    m_propMap.insert( "D_PAINTCOLOR", Fn::Property::D_PAINTCOLOR );
    m_propMap.insert( "D_PAINTVALUE", Fn::Property::D_PAINTVALUE );
    m_propMap.insert( "D_PAINT_LINK_CURSOR", Fn::Property::D_PAINT_LINK_CURSOR );
    m_propMap.insert( "D_FIBER_RENDERMODE", Fn::Property::D_FIBER_RENDERMODE );
    m_propMap.insert( "D_FIBER_THICKNESS", Fn::Property::D_FIBER_THICKNESS );
    m_propMap.insert( "D_HAS_TEXTURE", Fn::Property::D_HAS_TEXTURE );
    m_propMap.insert( "D_SELECTED_TEXTURE", Fn::Property::D_SELECTED_TEXTURE );
    m_propMap.insert( "D_TEXTURE_GLUINT", Fn::Property::D_TEXTURE_GLUINT );
    m_propMap.insert( "D_DATASET_POINTER", Fn::Property::D_DATASET_POINTER );
    m_propMap.insert( "D_NEW_DATASET", Fn::Property::D_NEW_DATASET );
    m_propMap.insert( "D_DATASET_LIST", Fn::Property::D_DATASET_LIST );
    m_propMap.insert( "D_SURFACE", Fn::Property::D_SURFACE );
    m_propMap.insert( "D_THRESHOLD", Fn::Property::D_THRESHOLD );
    m_propMap.insert( "D_GLYPHSTYLE", Fn::Property::D_GLYPHSTYLE );
    m_propMap.insert( "D_GLYPHRADIUS", Fn::Property::D_GLYPHRADIUS );
    m_propMap.insert( "D_NORMALIZATION", Fn::Property::D_NORMALIZATION );
    m_propMap.insert( "D_PRIMSIZE", Fn::Property::D_PRIMSIZE );
    m_propMap.insert( "D_MINLENGTH", Fn::Property::D_MINLENGTH );
    m_propMap.insert( "D_DRAW_SURFACE", Fn::Property::D_DRAW_SURFACE );
    m_propMap.insert( "D_DRAW_GLYPHS", Fn::Property::D_DRAW_GLYPHS );
    m_propMap.insert( "D_SURFACE_GLYPH_GEOMETRY", Fn::Property::D_SURFACE_GLYPH_GEOMETRY );
    m_propMap.insert( "D_SURFACE_GLYPH_COLOR", Fn::Property::D_SURFACE_GLYPH_COLOR );
    m_propMap.insert( "D_RENDER_TARGET", Fn::Property::D_RENDER_TARGET );
    m_propMap.insert( "D_GLYPH_ROT_X", Fn::Property::D_GLYPH_ROT_X );
    m_propMap.insert( "D_GLYPH_ROT_Y", Fn::Property::D_GLYPH_ROT_Y );
    m_propMap.insert( "D_GLYPH_ROT_Z", Fn::Property::D_GLYPH_ROT_Z );
    m_propMap.insert( "D_GLYPH_ALPHA", Fn::Property::D_GLYPH_ALPHA );
    m_propMap.insert( "D_LITTLE_BRAIN_VISIBILITY", Fn::Property::D_LITTLE_BRAIN_VISIBILITY );
    m_propMap.insert( "D_IS_ATLAS", Fn::Property::D_IS_ATLAS );
    m_propMap.insert( "D_LOCK_PROPS", Fn::Property::D_LOCK_PROPS );
    m_propMap.insert( "D_AUTOPLAY", Fn::Property::D_AUTOPLAY );
    m_propMap.insert( "D_AUTOPLAY_INTERVAL", Fn::Property::D_AUTOPLAY_INTERVAL );
    m_propMap.insert( "D_MESH_CUT_LOWER_X", Fn::Property::D_MESH_CUT_LOWER_X );
    m_propMap.insert( "D_MESH_CUT_LOWER_Y", Fn::Property::D_MESH_CUT_LOWER_Y );
    m_propMap.insert( "D_MESH_CUT_LOWER_Z", Fn::Property::D_MESH_CUT_LOWER_Z );
    m_propMap.insert( "D_MESH_CUT_HIGHER_X", Fn::Property::D_MESH_CUT_HIGHER_X );
    m_propMap.insert( "D_MESH_CUT_HIGHER_Y", Fn::Property::D_MESH_CUT_HIGHER_Y );
    m_propMap.insert( "D_MESH_CUT_HIGHER_Z", Fn::Property::D_MESH_CUT_HIGHER_Z );
    m_propMap.insert( "D_FIBER_GROW_LENGTH", Fn::Property::D_FIBER_GROW_LENGTH );
    m_propMap.insert( "D_ADJUST_X", Fn::Property::D_ADJUST_X );
    m_propMap.insert( "D_ADJUST_Y", Fn::Property::D_ADJUST_Y );
    m_propMap.insert( "D_ADJUST_Z", Fn::Property::D_ADJUST_Z );
    m_propMap.insert( "D_GLYPHSET_PICKED_ID", Fn::Property::D_GLYPHSET_PICKED_ID );
    m_propMap.insert( "D_GLYPH_COLORMODE", Fn::Property::D_GLYPH_COLORMODE );
    m_propMap.insert( "D_GLYPH_ROTATION", Fn::Property::D_GLYPH_ROTATION );
    m_propMap.insert( "D_LIGHT_SWITCH", Fn::Property::D_LIGHT_SWITCH );
    m_propMap.insert( "D_LIGHT_AMBIENT", Fn::Property::D_LIGHT_AMBIENT );
    m_propMap.insert( "D_LIGHT_DIFFUSE", Fn::Property::D_LIGHT_DIFFUSE );
    m_propMap.insert( "D_LIGHT_SPECULAR", Fn::Property::D_LIGHT_SPECULAR );
    m_propMap.insert( "D_MATERIAL_AMBIENT", Fn::Property::D_MATERIAL_AMBIENT );
    m_propMap.insert( "D_MATERIAL_DIFFUSE", Fn::Property::D_MATERIAL_DIFFUSE );
    m_propMap.insert( "D_MATERIAL_SPECULAR", Fn::Property::D_MATERIAL_SPECULAR );
    m_propMap.insert( "D_MATERIAL_SHININESS", Fn::Property::D_MATERIAL_SHININESS );
    m_propMap.insert( "D_COPY_COLORS", Fn::Property::D_COPY_COLORS );
    m_propMap.insert( "D_RENDER_WIREFRAME", Fn::Property::D_RENDER_WIREFRAME );
    m_propMap.insert( "D_ROTATE_X", Fn::Property::D_ROTATE_X );
    m_propMap.insert( "D_ROTATE_Y", Fn::Property::D_ROTATE_Y );
    m_propMap.insert( "D_ROTATE_Z", Fn::Property::D_ROTATE_Z );
    m_propMap.insert( "D_SCALE_X", Fn::Property::D_SCALE_X );
    m_propMap.insert( "D_SCALE_Y", Fn::Property::D_SCALE_Y );
    m_propMap.insert( "D_SCALE_Z", Fn::Property::D_SCALE_Z );
    m_propMap.insert( "D_MESH_MAKE_PERMANENT", Fn::Property::D_MESH_MAKE_PERMANENT );
    m_propMap.insert( "D_MESH_NUM_VERTEX", Fn::Property::D_MESH_NUM_VERTEX );
    m_propMap.insert( "D_MESH_NUM_TRIANGLES", Fn::Property::D_MESH_NUM_TRIANGLES );
    m_propMap.insert( "D_TREE_SELECTED_CLUSTER", Fn::Property::D_TREE_SELECTED_CLUSTER );
    m_propMap.insert( "D_TREE_SELECTED_CLUSTER_COLOR", Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR );
    m_propMap.insert( "D_TREE_UNSELECTED_CLUSTER_COLOR", Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR );
    m_propMap.insert( "D_S_FORM", Fn::Property::D_S_FORM );
    m_propMap.insert( "D_Q_FORM", Fn::Property::D_Q_FORM );
    m_propMap.insert( "D_USE_TRANSFORM", Fn::Property::D_USE_TRANSFORM );
    m_propMap.insert( "D_TRANSFORM", Fn::Property::D_TRANSFORM );
    m_propMap.insert( "D_APPLY_TRANSFORM", Fn::Property::D_APPLY_TRANSFORM );
    m_propMap.insert( "D_INVERT_VERTEX_ORDER", Fn::Property::D_INVERT_VERTEX_ORDER );
    m_propMap.insert( "D_START_INDEX", Fn::Property::D_START_INDEX );
    m_propMap.insert( "D_END_INDEX", Fn::Property::D_END_INDEX );
    m_propMap.insert( "D_LEFT_RIGHT", Fn::Property::D_LEFT_RIGHT );
    m_propMap.insert( "D_TREE_COLOR_SELECTION", Fn::Property::D_TREE_COLOR_SELECTION );
    m_propMap.insert( "D_TREE_USER_CLUSTER_COLOR", Fn::Property::D_TREE_USER_CLUSTER_COLOR );
    m_propMap.insert( "D_TREE_SET_USER_CLUSTER_COLOR", Fn::Property::D_TREE_SET_USER_CLUSTER_COLOR );
    m_propMap.insert( "D_TREE_PARTITION_MODE", Fn::Property::D_TREE_PARTITION_MODE );
    m_propMap.insert( "D_TREE_APPLY_PARTITION_MODE", Fn::Property::D_TREE_APPLY_PARTITION_MODE );
    m_propMap.insert( "D_TREE_PARTITION_LEVEL", Fn::Property::D_TREE_PARTITION_LEVEL );
    m_propMap.insert( "D_TREE_PARTITION_SIZE", Fn::Property::D_TREE_PARTITION_SIZE );
    m_propMap.insert( "D_GLYPH_THRESHOLD_SIGN", Fn::Property::D_GLYPH_THRESHOLD_SIGN );
    m_propMap.insert( "D_LITTLE_BRAINS_COLORMODE", Fn::Property::D_LITTLE_BRAINS_COLORMODE );
    m_propMap.insert( "D_THRESHOLD_PERC", Fn::Property::D_THRESHOLD_PERC );
    m_propMap.insert( "D_FIBER_MORPH", Fn::Property::D_FIBER_MORPH );
    m_propMap.insert( "D_SHOW_PLANE_HANDLES", Fn::Property::D_SHOW_PLANE_HANDLES );
    m_propMap.insert( "D_HANDLE_COLOR", Fn::Property::D_HANDLE_COLOR );
    m_propMap.insert( "D_LINE_WIDTH", Fn::Property::D_LINE_WIDTH );
    m_propMap.insert( "D_STIPPLE_PROB_MASK", Fn::Property::D_STIPPLE_PROB_MASK );
    m_propMap.insert( "D_STIPPLE_THICKNESS", Fn::Property::D_STIPPLE_THICKNESS );
    m_propMap.insert( "D_STIPPLE_GLYPH_SIZE", Fn::Property::D_STIPPLE_GLYPH_SIZE );
    m_propMap.insert( "D_STIPPLE_SLICE_ORIENT", Fn::Property::D_STIPPLE_SLICE_ORIENT );
    m_propMap.insert( "D_GUIDE_X", Fn::Property::D_GUIDE_X );
    m_propMap.insert( "D_GUIDE_Y", Fn::Property::D_GUIDE_Y );
    m_propMap.insert( "D_GUIDE_Z", Fn::Property::D_GUIDE_Z );
    m_propMap.insert( "D_HANDLE_0", Fn::Property::D_HANDLE_0 );
    m_propMap.insert( "D_HANDLE_1", Fn::Property::D_HANDLE_1 );
    m_propMap.insert( "D_HANDLE_2", Fn::Property::D_HANDLE_2 );
    m_propMap.insert( "D_ISOLINE_STRIPES", Fn::Property::D_ISOLINE_STRIPES );
    m_propMap.insert( "D_ISOLINE_STRIPES_WIDTH", Fn::Property::D_ISOLINE_STRIPES_WIDTH );
    m_propMap.insert( "D_COLORMAP_LABEL", Fn::Property::D_COLORMAP_LABEL );
    m_propMap.insert( "G_FIRST", Fn::Property::G_FIRST );
    m_propMap.insert( "G_LOCK_WIDGETS", Fn::Property::G_LOCK_WIDGETS );
    m_propMap.insert( "G_RENDER_CROSSHAIRS", Fn::Property::G_RENDER_CROSSHAIRS );
    m_propMap.insert( "G_SAGITTAL", Fn::Property::G_SAGITTAL );
    m_propMap.insert( "G_CORONAL", Fn::Property::G_CORONAL );
    m_propMap.insert( "G_AXIAL", Fn::Property::G_AXIAL );
    m_propMap.insert( "G_CORONAL_AXIAL", Fn::Property::G_CORONAL_AXIAL );
    m_propMap.insert( "G_SAGITTAL_AXIAL", Fn::Property::G_SAGITTAL_AXIAL );
    m_propMap.insert( "G_SAGITTAL_CORONAL", Fn::Property::G_SAGITTAL_CORONAL );
    m_propMap.insert( "G_SHOW_SAGITTAL", Fn::Property::G_SHOW_SAGITTAL );
    m_propMap.insert( "G_SHOW_CORONAL", Fn::Property::G_SHOW_CORONAL );
    m_propMap.insert( "G_SHOW_AXIAL", Fn::Property::G_SHOW_AXIAL );
    m_propMap.insert( "G_SHOW_NAV_SLIDERS", Fn::Property::G_SHOW_NAV_SLIDERS );
    m_propMap.insert( "G_ZOOM", Fn::Property::G_ZOOM );
    m_propMap.insert( "G_MOVEX", Fn::Property::G_MOVEX );
    m_propMap.insert( "G_MOVEY", Fn::Property::G_MOVEY );
    m_propMap.insert( "G_CAMERA_TYPE", Fn::Property::G_CAMERA_TYPE );
    m_propMap.insert( "G_CAMERA_PROJECTION", Fn::Property::G_CAMERA_PROJECTION );
    m_propMap.insert( "G_CAMERA_POS_X", Fn::Property::G_CAMERA_POS_X );
    m_propMap.insert( "G_CAMERA_POS_Y", Fn::Property::G_CAMERA_POS_Y );
    m_propMap.insert( "G_CAMERA_POS_Z", Fn::Property::G_CAMERA_POS_Z );
    m_propMap.insert( "G_CAMERA_LOOKAT_X", Fn::Property::G_CAMERA_LOOKAT_X );
    m_propMap.insert( "G_CAMERA_LOOKAT_Y", Fn::Property::G_CAMERA_LOOKAT_Y );
    m_propMap.insert( "G_CAMERA_LOOKAT_Z", Fn::Property::G_CAMERA_LOOKAT_Z );
    m_propMap.insert( "G_CAMERA_UP_X", Fn::Property::G_CAMERA_UP_X );
    m_propMap.insert( "G_CAMERA_UP_Y", Fn::Property::G_CAMERA_UP_Y );
    m_propMap.insert( "G_CAMERA_UP_Z", Fn::Property::G_CAMERA_UP_Z );
    m_propMap.insert( "G_CAMERA_NEAR", Fn::Property::G_CAMERA_NEAR );
    m_propMap.insert( "G_CAMERA_FAR", Fn::Property::G_CAMERA_FAR );
    m_propMap.insert( "G_CAMERA_ANGLE", Fn::Property::G_CAMERA_ANGLE );
    m_propMap.insert( "G_SCREENSHOT_QUALITY", Fn::Property::G_SCREENSHOT_QUALITY );
    m_propMap.insert( "G_TRANSPARENCY", Fn::Property::G_TRANSPARENCY );
    m_propMap.insert( "G_LAST_PATH", Fn::Property::G_LAST_PATH );
    m_propMap.insert( "G_SCREENSHOT_PATH", Fn::Property::G_SCREENSHOT_PATH );
    m_propMap.insert( "G_OBJECT", Fn::Property::G_OBJECT );
    m_propMap.insert( "G_NEED_SHADER_UPDATE", Fn::Property::G_NEED_SHADER_UPDATE );
    m_propMap.insert( "G_BACKGROUND_COLOR_MAIN", Fn::Property::G_BACKGROUND_COLOR_MAIN );
    m_propMap.insert( "G_BACKGROUND_COLOR_MAIN2", Fn::Property::G_BACKGROUND_COLOR_MAIN2 );
    m_propMap.insert( "G_BACKGROUND_COLOR_COMBINED", Fn::Property::G_BACKGROUND_COLOR_COMBINED );
    m_propMap.insert( "G_BACKGROUND_COLOR_NAV1", Fn::Property::G_BACKGROUND_COLOR_NAV1 );
    m_propMap.insert( "G_BACKGROUND_COLOR_NAV2", Fn::Property::G_BACKGROUND_COLOR_NAV2 );
    m_propMap.insert( "G_BACKGROUND_COLOR_NAV3", Fn::Property::G_BACKGROUND_COLOR_NAV3 );
    m_propMap.insert( "G_CROSSHAIR_COLOR", Fn::Property::G_CROSSHAIR_COLOR );
    m_propMap.insert( "G_SCREENSHOT_WIDTH", Fn::Property::G_SCREENSHOT_WIDTH );
    m_propMap.insert( "G_SCREENSHOT_HEIGHT", Fn::Property::G_SCREENSHOT_HEIGHT );
    m_propMap.insert( "G_SCREENSHOT_KEEP_ASPECT", Fn::Property::G_SCREENSHOT_KEEP_ASPECT );
    m_propMap.insert( "G_SCREENSHOT_SIZE_RATIO", Fn::Property::G_SCREENSHOT_SIZE_RATIO );
    m_propMap.insert( "G_SCREENSHOT_SIZE_SELECTION", Fn::Property::G_SCREENSHOT_SIZE_SELECTION );
    m_propMap.insert( "G_SCREENSHOT_COPY_CURRENT", Fn::Property::G_SCREENSHOT_COPY_CURRENT );
    m_propMap.insert( "G_WIDTH_MAINGL", Fn::Property::G_WIDTH_MAINGL );
    m_propMap.insert( "G_HEIGHT_MAINGL", Fn::Property::G_HEIGHT_MAINGL );
    m_propMap.insert( "G_WIDTH_MAINGL2", Fn::Property::G_WIDTH_MAINGL2 );
    m_propMap.insert( "G_HEIGHT_MAINGL2", Fn::Property::G_HEIGHT_MAINGL2 );
    m_propMap.insert( "G_SCREENSHOT_DO_MAINGL", Fn::Property::G_SCREENSHOT_DO_MAINGL );
    m_propMap.insert( "G_SCREENSHOT_DO_MAINGL2", Fn::Property::G_SCREENSHOT_DO_MAINGL2 );
    m_propMap.insert( "G_SCREENSHOT_STEREOSCOPIC", Fn::Property::G_SCREENSHOT_STEREOSCOPIC );
    m_propMap.insert( "G_SCREENSHOT_PREFIX", Fn::Property::G_SCREENSHOT_PREFIX );
    m_propMap.insert( "G_SCREENSHOT_PREFIX2", Fn::Property::G_SCREENSHOT_PREFIX2 );
    m_propMap.insert( "G_SCREENSHOT_DIGITS", Fn::Property::G_SCREENSHOT_DIGITS );
    m_propMap.insert( "G_SCREENSHOT_CURRENT_NUMBER", Fn::Property::G_SCREENSHOT_CURRENT_NUMBER );
    m_propMap.insert( "G_ARCBALL_DISTANCE", Fn::Property::G_ARCBALL_DISTANCE );
    m_propMap.insert( "G_CAMERA_KEYBOARD_STEP", Fn::Property::G_CAMERA_KEYBOARD_STEP );
    m_propMap.insert( "G_CAMERA_FIXATE_Z", Fn::Property::G_CAMERA_FIXATE_Z );
    m_propMap.insert( "G_CAMERA_FULLCIRCLE_STEPS", Fn::Property::G_CAMERA_FULLCIRCLE_STEPS );
    m_propMap.insert( "G_LIGHT_SWITCH", Fn::Property::G_LIGHT_SWITCH );
    m_propMap.insert( "G_LIGHT_AMBIENT", Fn::Property::G_LIGHT_AMBIENT );
    m_propMap.insert( "G_LIGHT_DIFFUSE", Fn::Property::G_LIGHT_DIFFUSE );
    m_propMap.insert( "G_FILTER_SIZE", Fn::Property::G_FILTER_SIZE );
    m_propMap.insert( "G_DECIMATE_EPSILON", Fn::Property::G_DECIMATE_EPSILON );
    m_propMap.insert( "G_MIN_COMPONENT_SIZE", Fn::Property::G_MIN_COMPONENT_SIZE );
    m_propMap.insert( "G_RENDER_CROSSHAIRS_2", Fn::Property::G_RENDER_CROSSHAIRS_2 );
    m_propMap.insert( "G_UNSELECTED_FIBERS_GREY", Fn::Property::G_UNSELECTED_FIBERS_GREY );
    m_propMap.insert( "G_TRACT_TEX_RESOLUTION", Fn::Property::G_TRACT_TEX_RESOLUTION );
    m_propMap.insert( "G_TRACT_TEXT_SOURCE", Fn::Property::G_TRACT_TEXT_SOURCE );
    m_propMap.insert( "G_ISOLINE_STANDARD_COLOR", Fn::Property::G_ISOLINE_STANDARD_COLOR );
    m_propMap.insert( "G_MESH_TRANSPARENCY", Fn::Property::G_MESH_TRANSPARENCY );
    m_propMap.insert( "G_LAST", Fn::Property::G_LAST );
    m_propMap.insert( "D_X", Fn::Property::D_X );
    m_propMap.insert( "D_Y", Fn::Property::D_Y );
    m_propMap.insert( "D_Z", Fn::Property::D_Z );
    m_propMap.insert( "D_NEG", Fn::Property::D_NEG );
    m_propMap.insert( "D_RENDER", Fn::Property::D_RENDER );
    m_propMap.insert( "D_SHAPE", Fn::Property::D_SHAPE );
    m_propMap.insert( "D_STICK_TO_CROSSHAIR", Fn::Property::D_STICK_TO_CROSSHAIR );
    m_propMap.insert( "D_ID", Fn::Property::D_ID );
    m_propMap.insert( "D_PICK_ID", Fn::Property::D_PICK_ID );
    m_propMap.insert( "D_UPDATED", Fn::Property::D_UPDATED );
    m_propMap.insert( "D_POINTER", Fn::Property::D_POINTER );
    m_propMap.insert( "D_DESCRIPTION", Fn::Property::D_DESCRIPTION );
}

Fn::Property Models::s2p( QString s )
{
    if ( m_propMap.contains( s ) )
    {
        return m_propMap[s];
    }
    else
    {
        qCritical() << "Property " << s << " doesn't exist, exiting!";
        exit( 0 );
    }
}
