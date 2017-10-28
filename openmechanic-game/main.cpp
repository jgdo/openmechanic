#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include "RigidBodyPartVisualizer.h"
#include "GamePhysics/GameWorld.h"

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>


#include <osgText/Text>


#include <sstream>


#include "GameVisualization.h"
#include "GameWorldVisualizer.h"

#include "GameEngine/ThreadedLocalGameEngine.h"

// class to handle events with a pick
class PickHandler : public osgGA::GUIEventHandler {
public:


    PickHandler(osgText::Text* updateText):
        _updateText(updateText) {}
    ~PickHandler() {}
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
    void setLabel(const std::string& name)
    {
        if (_updateText.get()) _updateText->setText(name);

    }
protected:
    osg::ref_ptr<osgText::Text>  _updateText;
};


bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::PUSH):
        {
            osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
            if (view) pick(view,ea);
            return false;
        }
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            if (ea.getKey()=='c')
            {
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
                event->setX((ea.getXmin()+ea.getXmax())*0.5);
                event->setY((ea.getYmin()+ea.getYmax())*0.5);
                if (view) pick(view,*event);
            }
            return false;
        }
        default:
            return false;
    }
}


void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
    osgUtil::LineSegmentIntersector::Intersections intersections;


    std::string gdlist="";


    if (view->computeIntersections(ea,intersections))
    {
        for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
            hitr != intersections.end();
            ++hitr)
        {
            std::ostringstream os;
            if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
            {
                // the geodes are identified by name.
                os<<"Object " <<hitr->nodePath.back()->getName() <<std::endl;
            }
            else if (hitr->drawable.valid())
            {
                os<<"Object "<<hitr->drawable->className()<<std::endl;
            }


            os<<"        local coords vertex("<< hitr->getLocalIntersectPoint()<<")"<<"  normal("<<hitr->getLocalIntersectNormal()<<")"<<std::endl;
            os<<"        world coords vertex("<< hitr->getWorldIntersectPoint()<<")"<<"  normal("<<hitr->getWorldIntersectNormal()<<")"<<std::endl;
            const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr->indexList;
            for(unsigned int i=0;i<vil.size();++i)
            {
                os<<"        vertex indices ["<<i<<"] = "<<vil[i]<<std::endl;
            }


            gdlist += os.str();
        }
    } else {
        gdlist = "no intersections";
    }
    setLabel(gdlist);
}


osg::Node* createScene() {
osg::ref_ptr<osg::Geode> node = new osg::Geode;
osg::ref_ptr<osg::ShapeDrawable> sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(500,300,0), 100));
node->addDrawable(sphere.get());
return node.release();
}


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


    // A geometry node for our HUD:
   osg::Geode* HUDGeode = new osg::Geode();
   // Text instance that wil show up in the HUD:
   osgText::Text* textOne = new osgText::Text();
   // Projection node for defining view frustrum for HUD:
   osg::Projection* HUDProjectionMatrix = new osg::Projection;

    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,1024,0,768));

    // For the HUD model view matrix use an identity matrix:
    osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
    HUDModelViewMatrix->setMatrix(osg::Matrix::identity());

    // Make sure the model view matrix is not affected by any transforms
    // above it in the scene graph:
    HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    // Add the HUD projection matrix as a child of the root node
    // and the HUD model view matrix as a child of the projection matrix
    // Anything under this node will be viewed using this projection matrix
    // and positioned with this model view matrix.
    gameViz.rootNode->addChild(HUDProjectionMatrix);
    HUDProjectionMatrix->addChild(HUDModelViewMatrix);

    // Add the Geometry node to contain HUD geometry as a child of the
           // HUD model view matrix.
           HUDModelViewMatrix->addChild( HUDGeode );


           // Create and set up a state set using the texture from above:
                  osg::StateSet* HUDStateSet = new osg::StateSet();
                  HUDGeode->setStateSet(HUDStateSet);
                  //HUDStateSet->
                   //  setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);

                  // For this state set, turn blending on (so alpha texture looks right)
                  HUDStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

                  // Disable depth testing so geometry is draw regardless of depth values
                  // of geometry already draw.
                  HUDStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
                  HUDStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

                  // Need to make sure this geometry is draw last. RenderBins are handled
                  // in numerical order so set bin number to 11
                  HUDStateSet->setRenderBinDetails( 11, "RenderBin");

                  // Add the text (Text class is derived from drawable) to the geode:
                  HUDGeode->addDrawable( textOne );

                  // Set up the parameters for the text we'll add to the HUD:
                  textOne->setCharacterSize(12.0f);
                  textOne->setFont("fonts/arial.ttf");
                  textOne->setText("Not so good");
                  textOne->setAxisAlignment(osgText::Text::SCREEN);
                  textOne->setPosition( osg::Vec3(0,700,-1.5) );
                  textOne->setColor( osg::Vec4(199, 77, 15, 1) );


    GameWorldVisualizer worldViz(&gameViz);

    ThreadedLocalGameEngine engine;

    std::vector<float> terrain;
    terrain.resize(100*100, 0);

    engine.startEngine(terrain, 100, terrain.size());

    osgViewer::Viewer viewer;

    viewer.setUpViewInWindow(0,0,1024,768);

   viewer.addEventHandler(new PickHandler(textOne));

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

