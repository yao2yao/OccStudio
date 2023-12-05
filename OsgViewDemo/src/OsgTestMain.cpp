#include "OsgViewer.h"
#include <osg/ref_ptr>
#include <osgViewer/Viewer>
#include <BRepPrimAPI_MakeBox.hxx>

int main()
{
	osg::ref_ptr<osg::Group> root = new osg::Group();
	OsgViewer::instance()->setRoot(root);
	OsgViewer::instance()->showCoordinate(500, 500, 500);

	TopoDS_Shape box = BRepPrimAPI_MakeBox(100, 200, 300);
	OsgViewer::instance()->showShape(box);

	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(50, 50, 1000, 800);
	viewer.getCamera()->setClearColor(osg::Vec4(0.94f, 0.94f, 0.94f, 1.0f));
	viewer.setSceneData(root.get());

	return viewer.run();
}