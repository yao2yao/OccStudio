
#include <osg/Geode>
#include <osg/Geometry>
#include <Geom_Curve.hxx>
#include <TopoDS_Shape.hxx>

class OsgViewer
{
public:
	
	static OsgViewer* instance();
	   	 
	void showCoordinate(double xLength, double yLength, double zLength);
	void showCurveBase(Handle(Geom_Curve) curve, osg::Vec4 color = osg::Vec4(0, 0, 0, 1));
	void showShape(TopoDS_Shape& shape);
	void setRoot(osg::ref_ptr<osg::Group>& root);

private:
	osg::ref_ptr<osg::Group> root;
};



