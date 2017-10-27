#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include "RigidBodyPartVisualizer.h"
#include "GamePhysics/GameWorld.h"

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

#include "GameVisualization.h"
#include "GameWorldVisualizer.h"

#include "GameEngine/ThreadedLocalGameEngine.h"


int main( int argc, char** argv )
{


    // asyncContainer.postRequest(world.myIndex.asReadIndex(), &ServerInterface<GameWorldData>::addBlockAsNewBody, BLOCK_WOOD, btVector3(0, 0, 1), btQuaternion(0,0,0), btQuaternion(0,0,0));

    // world.addBlockAsNewBody(world.myIndex, BLOCK_WOOD, btVector3(0, 0, 1), btQuaternion(0,0,0), btQuaternion(0,0,0));

    // (world.*(&GameWorld::addBlockAsNewBody))(DataWriteIndex<D>{iter}, params...); }

    // RigidBodyPartVisualizer viz(nullptr);

    /*
	
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
    shape3->setColor( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); */

    /*
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

    */

    GameVisualization gameViz;
    gameViz.rootNode = new osg::Group;
   // gameViz.rootNode->addChild( transformation1.get() );
   // gameViz.rootNode->addChild( transformation2.get() );

    GameWorldVisualizer worldViz(&gameViz);

    ThreadedLocalGameEngine engine;

    std::vector<float> terrain;
    terrain.resize(100*100, 0);

    engine.startEngine(terrain, 100, terrain.size());

    osgViewer::Viewer viewer;
    viewer.setSceneData( gameViz.rootNode.get() );

    if (!viewer.getCameraManipulator() && viewer.getCamera()->getAllowEventFocus())
       {
            viewer.setCameraManipulator(new osgGA::TrackballManipulator());
       }

       viewer.setReleaseContextAtEndOfFrameHint(false);

       viewer.realize();

       osg::Vec3d eye( 1000.0, 1000.0, 0.0 );
        osg::Vec3d center( 0.0, 0.0, 0.0 );
        osg::Vec3d up( 0.0, 0.0, 1.0 );


           viewer.getCamera()->setViewMatrixAsLookAt( eye, center, up );

    while(!viewer.done()) {
        viewer.frame();


        auto lastEvents = engine.getGameWorld()->getLastEventQueue();

        for(auto& event: lastEvents->getChangeEvents()) {
            event->tryDispatchSelf(&worldViz);
        }

        for(auto& state: lastEvents->getStateData()) {
            state.second->tryDispatchSelf(&worldViz);
        }
    }
}
