#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>

int main( int argc, char** argv )
{
    osg::ref_ptr<osg::ShapeDrawable> shape1 = new osg::ShapeDrawable;
    shape1->setShape( new osg::Box(osg::Vec3(-3.0f, 0.0f, 0.0f),
                                   2.0f, 2.0f, 1.0f) );
    osg::ref_ptr<osg::ShapeDrawable> shape2 = new osg::ShapeDrawable;
    shape2->setShape( new osg::Sphere(osg::Vec3(3.0f, 0.0f, 0.0f),
                                      1.0f) );
    shape2->setColor( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
    osg::ref_ptr<osg::ShapeDrawable> shape3 = new osg::ShapeDrawable;
    shape3->setShape( new osg::Cone(osg::Vec3(0.0f, 0.0f, 0.0f),
                                    1.0f, 1.0f) );
    shape3->setColor( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );

    osg::ref_ptr<osg::Geode> figures = new osg::Geode;
    figures->addDrawable( shape1.get() );
    figures->addDrawable( shape2.get() );
    figures->addDrawable( shape3.get() );


    osg::ref_ptr<osg::MatrixTransform> transformation1 = new
            osg::MatrixTransform;
    transformation1->setMatrix(osg::Matrix::translate(-5.0f,0.0f,0.0f));
    transformation1->addChild( figures.get() );
    osg::ref_ptr<osg::MatrixTransform> transformation2 = new
            osg::MatrixTransform;
    transformation2->setMatrix(osg::Matrix::translate(5.0f,0.0f,0.0f));
    transformation2->addChild( figures.get() );

    osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
    pm->setMode(osg::PolygonMode::FRONT_AND_BACK,
                osg::PolygonMode::LINE);
    transformation1->getOrCreateStateSet()->setAttribute( pm.get() );

    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( transformation1.get() );
    root->addChild( transformation2.get() );

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}