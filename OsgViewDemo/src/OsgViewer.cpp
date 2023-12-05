#include "OsgViewer.h"
#include <osg/LineWidth>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <BRep_Tool.hxx>
#include <osg/Material>
#include <Poly.hxx>

void OsgViewer::showCoordinate(double xLength, double yLength, double zLength)
{
	Handle(Geom_TrimmedCurve) xCoordinate = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(xLength, 0, 0)).Value();
	Handle(Geom_TrimmedCurve) yCoordinate = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(0, yLength, 0)).Value();
	Handle(Geom_TrimmedCurve) zCoordinate = GC_MakeSegment(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, zLength)).Value();

	showCurveBase(xCoordinate, osg::Vec4(255, 0, 0, 1));
	showCurveBase(yCoordinate, osg::Vec4(0, 255, 0, 1));
	showCurveBase(zCoordinate, osg::Vec4(0, 0, 255, 1));
}

void OsgViewer::showCurveBase(Handle(Geom_Curve) curve, osg::Vec4 color)
{
	const TopoDS_Shape& edge = BRepBuilderAPI_MakeEdge(curve);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> curveGeom = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> curveVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec4Array> curveVerticesColors = new osg::Vec4Array();

	for (TopExp_Explorer aEdgeExplorer(edge, TopAbs_EDGE); aEdgeExplorer.More(); aEdgeExplorer.Next())
	{
		TopoDS_Edge myEdge = TopoDS::Edge(aEdgeExplorer.Current());

		auto adaptorCurve = BRepAdaptor_Curve(myEdge);
		auto tangDef = GCPnts_TangentialDeflection(adaptorCurve, 0.1, 0.1);
		for (size_t j = 0; j < tangDef.NbPoints(); j++) {
			auto vertex = tangDef.Value(j + 1);

			curveVertices->push_back(osg::Vec3(vertex.X(), vertex.Y(), vertex.Z()));
			curveVerticesColors->push_back(color);
		}
	}

	osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth(color.a());
	curveGeom->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
	curveGeom->setColorArray(curveVerticesColors);
	curveGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	curveGeom->setVertexArray(curveVertices);
	curveGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, curveVertices->size()));
	geode->addChild(curveGeom.get());
	root->addChild(geode.get());
}

void OsgViewer::showShape(TopoDS_Shape& shape)
{
	BRepMesh_IncrementalMesh mesh(shape, 0.1);

	osg::ref_ptr<osg::Vec3Array> allVertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> allNormals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> allColors = new osg::Vec4Array;
	allColors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	osg::ref_ptr<osg::Vec2Array> allUvs = new osg::Vec2Array();
	osg::ref_ptr<osg::DrawElementsUInt> allIndexs = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);

	TopLoc_Location aLoc;
	int maxIndex = 0;
	int newIndexStart = 0;
	for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next())
	{
		const TopoDS_Face& face = TopoDS::Face(explorer.Current());
		const Handle(Poly_Triangulation)& triangulation = BRep_Tool::Triangulation(face, aLoc);
		if (triangulation.IsNull()) continue;

		triangulation->ComputeNormals();
		NCollection_Array1<gp_Vec3f>& normals = triangulation->InternalNormals();
		Poly_ArrayOfNodes& nodes = triangulation->InternalNodes();
		Poly_ArrayOfUVNodes& uvNodes = triangulation->InternalUVNodes();

		for (Standard_Integer aNodeIter = nodes.Lower(); aNodeIter <= nodes.Upper(); ++aNodeIter)
		{
			gp_Pnt vertex = nodes.Value(aNodeIter);
			gp_Pnt2d uv = uvNodes.Value(aNodeIter);
			gp_Vec3f normalf = normals.Value(aNodeIter);
			gp_Dir normal = gp_Dir(normalf.x(), normalf.y(), normalf.z());

			if ((face.Orientation() == TopAbs_REVERSED))
			{
				normal.Reverse();
			}

			allVertices->push_back(osg::Vec3(vertex.X(), vertex.Y(), vertex.Z()));
			allNormals->push_back(osg::Vec3(normal.X(), normal.Y(), normal.Z()));
			allUvs->push_back(osg::Vec2(uv.X(), uv.Y()));

			// std::cout << "allVertices " << vertex.X() << " " << vertex.Y() << " " << vertex.Z() << std::endl;
			// std::cout << "allNormals " << normalf.x() << " " << normalf.y() << " " << normalf.z() << std::endl;
			// std::cout << "allUvs " << uv.X() << " " << uv.Y() << std::endl;
		}

		int triCount = triangulation->NbTriangles();
		for (int i = 1; i <= triCount; ++i)
		{
			Poly_Triangle aTriangle = triangulation->Triangle(i);
			int index1 = aTriangle.Value(1) - 1;
			int index2 = aTriangle.Value(2) - 1;
			int index3 = aTriangle.Value(3) - 1;
			if (index1 > maxIndex) maxIndex = index1;
			if (index2 > maxIndex) maxIndex = index2;
			if (index3 > maxIndex) maxIndex = index3;
			allIndexs->push_back(index1 + newIndexStart);
			allIndexs->push_back(index2 + newIndexStart);
			allIndexs->push_back(index3 + newIndexStart);

			// std::cout << " allIndexs " << index1 + newStart << " " << index2 + newStart << " " << index3 + newStart << std::endl;
		}
		newIndexStart += (maxIndex + 1);
	}
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

	geometry->setVertexArray(allVertices);
	geometry->setNormalArray(allNormals);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	geometry->setColorArray(allColors.get());
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->setTexCoordArray(0, allUvs);
	geometry->addPrimitiveSet(allIndexs);
	
	geode->addChild(geometry.get());
	root->addChild(geode.get());
}

OsgViewer* OsgViewer::instance()
{
	static OsgViewer m_instance;
	return &m_instance;
}

void OsgViewer::setRoot(osg::ref_ptr<osg::Group>& root)
{
	this->root = root;
}
