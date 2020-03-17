#pragma once

#include "cinder/gl/gl.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/TriMesh.h"
#include <typeinfo>

namespace sitara {
	namespace ecs {
		namespace geometry {
			enum Primitive { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE, RECT, ROUNDEDRECT, CIRCLE, RING, UNKNOWN, PRIMITIVE_COUNT };
			enum Quality { LOW, DEFAULT, HIGH };
			enum ViewMode { SHADED, WIREFRAME };
			enum TexturingMode { NONE, PROCEDURAL, SAMPLER };

			static Primitive checkGeometryType(const ci::geom::Source& source) {
				if (typeid(ci::geom::Capsule) == typeid(source)) {
					return Primitive::CAPSULE;
				}
				else if (typeid(ci::geom::Cone) == typeid(source)) {
					return Primitive::CONE;
				}
				else if (typeid(ci::geom::Cube) == typeid(source)) {
					return Primitive::CUBE;
				}
				else if (typeid(ci::geom::Cylinder) == typeid(source)) {
					return Primitive::CYLINDER;
				}
				else if (typeid(ci::geom::Helix) == typeid(source)) {
					return Primitive::HELIX;
				}
				else if (typeid(ci::geom::Icosahedron) == typeid(source)) {
					return Primitive::ICOSAHEDRON;
				}
				else if (typeid(ci::geom::Icosphere) == typeid(source)) {
					return Primitive::ICOSAHEDRON;
				}
				else if (typeid(ci::geom::Sphere) == typeid(source)) {
					return Primitive::SPHERE;
				}
				else if (typeid(ci::geom::Teapot) == typeid(source)) {
					return Primitive::TEAPOT;
				}
				else if (typeid(ci::geom::Torus) == typeid(source)) {
					return Primitive::TORUS;
				}
				else if (typeid(ci::geom::TorusKnot) == typeid(source)) {
					return Primitive::TORUSKNOT;
				}
				else if (typeid(ci::geom::Plane) == typeid(source)) {
					return Primitive::PLANE;
				}
				else if (typeid(ci::geom::Rect) == typeid(source)) {
					return Primitive::RECT;
				}
				else if (typeid(ci::geom::RoundedRect) == typeid(source)) {
					return Primitive::ROUNDEDRECT;
				}
				else if (typeid(ci::geom::Circle) == typeid(source)) {
					return Primitive::CIRCLE;
				}
				else if (typeid(ci::geom::Ring) == typeid(source)) {
					return Primitive::RING;
				}
				else {
					return Primitive::UNKNOWN;
				}
			}

			static std::shared_ptr<ci::geom::Source> createSource(geometry::Primitive primitive, geometry::Quality quality) {
				switch (primitive) {
				case CAPSULE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Capsule().radius(mCapsuleRadius).length(mCapsuleLength), geom::WireCapsule().radius(mCapsuleRadius).length(mCapsuleLength)); break;
					case LOW:		loadGeomSource(geom::Capsule().radius(mCapsuleRadius).length(mCapsuleLength).subdivisionsAxis(6).subdivisionsHeight(1), geom::WireCapsule().radius(mCapsuleRadius).length(mCapsuleLength)); break;
					case HIGH:		loadGeomSource(geom::Capsule().radius(mCapsuleRadius).length(mCapsuleLength).subdivisionsAxis(60).subdivisionsHeight(20), geom::WireCapsule().radius(mCapsuleRadius).length(mCapsuleLength)); break;
					}
					break;
				case CONE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Cone().ratio(mConeRatio), geom::WireCone()); break;
					case LOW:		loadGeomSource(geom::Cone().ratio(mConeRatio).subdivisionsAxis(6).subdivisionsHeight(1), geom::WireCone()); break;
					case HIGH:		loadGeomSource(geom::Cone().ratio(mConeRatio).subdivisionsAxis(60).subdivisionsHeight(60), geom::WireCone()); break;
					}
					break;
				case CUBE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Cube(), geom::WireCube()); break;
					case LOW:		loadGeomSource(geom::Cube().subdivisions(1), geom::WireCube()); break;
					case HIGH:		loadGeomSource(geom::Cube().subdivisions(10), geom::WireCube()); break;
					}
					break;
				case CYLINDER:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Cylinder(), geom::WireCylinder()); break;
					case LOW:		loadGeomSource(geom::Cylinder().subdivisionsAxis(6).subdivisionsCap(1), geom::WireCylinder()); break;
					case HIGH:		loadGeomSource(geom::Cylinder().subdivisionsAxis(60).subdivisionsHeight(20).subdivisionsCap(10), geom::WireCylinder()); break;
					}
					break;
				case HELIX:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Helix().ratio(mHelixRatio).coils(mHelixCoils).twist(mHelixTwist, mHelixOffset), geom::WireCube()); break;
					case LOW:		loadGeomSource(geom::Helix().ratio(mHelixRatio).coils(mHelixCoils).twist(mHelixTwist, mHelixOffset).subdivisionsAxis(12).subdivisionsHeight(6), geom::WireCube()); break;
					case HIGH:		loadGeomSource(geom::Helix().ratio(mHelixRatio).coils(mHelixCoils).twist(mHelixTwist, mHelixOffset).subdivisionsAxis(60).subdivisionsHeight(60), geom::WireCube()); break;
					}
					break;
				case ICOSAHEDRON:
					loadGeomSource(geom::Icosahedron(), geom::WireIcosahedron());
					break;
				case ICOSPHERE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Icosphere(), geom::WireSphere()); break;
					case LOW:		loadGeomSource(geom::Icosphere().subdivisions(1), geom::WireSphere()); break;
					case HIGH:		loadGeomSource(geom::Icosphere().subdivisions(5), geom::WireSphere()); break;
					}
					break;
				case SPHERE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Sphere(), geom::WireSphere()); break;
					case LOW:		loadGeomSource(geom::Sphere().subdivisions(6), geom::WireSphere()); break;
					case HIGH:		loadGeomSource(geom::Sphere().subdivisions(60), geom::WireSphere()); break;
					}
					break;
				case TEAPOT:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Teapot(), geom::WireCube()); break;
					case LOW:		loadGeomSource(geom::Teapot().subdivisions(2), geom::WireCube()); break;
					case HIGH:		loadGeomSource(geom::Teapot().subdivisions(12), geom::WireCube()); break;
					}
					break;
				case TORUS:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Torus().twist(mTorusTwist, mTorusOffset).ratio(mTorusRatio), geom::WireTorus().ratio(mTorusRatio)); break;
					case LOW:		loadGeomSource(geom::Torus().twist(mTorusTwist, mTorusOffset).ratio(mTorusRatio).subdivisionsAxis(12).subdivisionsHeight(6), geom::WireTorus().ratio(mTorusRatio)); break;
					case HIGH:		loadGeomSource(geom::Torus().twist(mTorusTwist, mTorusOffset).ratio(mTorusRatio).subdivisionsAxis(60).subdivisionsHeight(60), geom::WireTorus().ratio(mTorusRatio)); break;
					}
					break;
				case TORUSKNOT:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::TorusKnot().parameters(mTorusKnotP, mTorusKnotQ).radius(mTorusKnotRadius).scale(mTorusKnotScale), geom::WireCube()); break;
					case LOW:		loadGeomSource(geom::TorusKnot().parameters(mTorusKnotP, mTorusKnotQ).radius(mTorusKnotRadius).scale(mTorusKnotScale).subdivisionsAxis(6).subdivisionsHeight(64), geom::WireCube()); break;
					case HIGH:		loadGeomSource(geom::TorusKnot().parameters(mTorusKnotP, mTorusKnotQ).radius(mTorusKnotRadius).scale(mTorusKnotScale).subdivisionsAxis(32).subdivisionsHeight(1024), geom::WireCube()); break;
					}
					break;
				case PLANE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Plane().subdivisions(ivec2(10, 10)), geom::WirePlane().subdivisions(ivec2(10, 10))); break;
					case LOW:		loadGeomSource(geom::Plane().subdivisions(ivec2(2, 2)), geom::WirePlane().subdivisions(ivec2(2, 2))); break;
					case HIGH:		loadGeomSource(geom::Plane().subdivisions(ivec2(100, 100)), geom::WirePlane().subdivisions(ivec2(100, 100))); break;
					}
					break;

				case RECT:
					loadGeomSource(geom::Rect(), geom::WireRect()); break;
					break;
				case ROUNDEDRECT:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::RoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(3), geom::WireRoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(3)); break;
					case LOW:		loadGeomSource(geom::RoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(1), geom::WireRoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(1)); break;
					case HIGH:		loadGeomSource(geom::RoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(9), geom::WireRoundedRect().cornerRadius(mRoundedRectRadius).cornerSubdivisions(9)); break;
					}
					break;
				case CIRCLE:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Circle().subdivisions(24), geom::WireCircle().subdivisions(24)); break;
					case LOW:		loadGeomSource(geom::Circle().subdivisions(8), geom::WireCircle().subdivisions(8)); break;
					case HIGH:		loadGeomSource(geom::Circle().subdivisions(120), geom::WireCircle().subdivisions(120)); break;
					}
					break;
				case RING:
					switch (quality) {
					case DEFAULT:	loadGeomSource(geom::Ring().width(mRingWidth).subdivisions(24), geom::WireCircle().subdivisions(24).radius(1 + 0.5f * mRingWidth)); break;
					case LOW:		loadGeomSource(geom::Ring().width(mRingWidth).subdivisions(8), geom::WireCircle().subdivisions(8).radius(1 + 0.5f * mRingWidth)); break;
					case HIGH:		loadGeomSource(geom::Ring().width(mRingWidth).subdivisions(120), geom::WireCircle().subdivisions(120).radius(1 + 0.5f * mRingWidth)); break;
					}
					break;
				}
			}

			static ci::TriMesh getMesh(const ci::geom::Source& source) {
				ci::TriMesh::Format fmt = ci::TriMesh::Format().positions().normals().texCoords().tangents();
				/*
				if (source.getAvailableAttribs().count(geom::COLOR) > 0) {
					fmt.colors();
				}
				*/

				return ci::TriMesh(source, fmt);
			}

			static ci::AxisAlignedBox getBoundingBox(const ci::geom::Source& source) {
				return getMesh(source).calcBoundingBox();
			}

			static ci::AxisAlignedBox getBoundingBox(const ci::TriMesh& mesh) {
				return mesh.calcBoundingBox();
			}
		}
	}
}