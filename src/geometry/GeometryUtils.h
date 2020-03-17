#pragma once

#include "cinder/gl/gl.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/TriMesh.h"
#include "cinder/DataSource.h"
#include <typeinfo>

namespace sitara {
	namespace ecs {
		namespace geometry {
			enum Primitive { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE, RECT, ROUNDEDRECT, CIRCLE, RING,
				WIRE_CAPSULE, WIRE_CONE, WIRE_CUBE, WIRE_CYLINDER, WIRE_ICOSAHEDRON, WIRE_SPHERE, WIRE_TORUS, WIRE_PLANE, WIRE_RECT, WIRE_ROUNDEDRECT, WIRE_CIRCLE,
				UNKNOWN, PRIMITIVE_COUNT };

			enum Quality { LOW, DEFAULT, HIGH };
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
				else if (typeid(ci::geom::WireCapsule) == typeid(source)) {
					return Primitive::WIRE_CAPSULE;
				}
				else if (typeid(ci::geom::WireCone) == typeid(source)) {
					return Primitive::WIRE_CONE;
				}
				else if (typeid(ci::geom::WireCube) == typeid(source)) {
					return Primitive::WIRE_CUBE;
				}
				else if (typeid(ci::geom::WireCylinder) == typeid(source)) {
					return Primitive::WIRE_CYLINDER;
				}
				else if (typeid(ci::geom::WireIcosahedron) == typeid(source)) {
					return Primitive::WIRE_ICOSAHEDRON;
				}
				else if (typeid(ci::geom::WireSphere) == typeid(source)) {
					return Primitive::WIRE_SPHERE;
				}
				else if (typeid(ci::geom::WireTorus) == typeid(source)) {
					return Primitive::WIRE_TORUS;
				}
				else if (typeid(ci::geom::WirePlane) == typeid(source)) {
					return Primitive::WIRE_PLANE;
				}
				else if (typeid(ci::geom::WireRect) == typeid(source)) {
					return Primitive::WIRE_RECT;
				}
				else if (typeid(ci::geom::WireRoundedRect) == typeid(source)) {
					return Primitive::WIRE_ROUNDEDRECT;
				}
				else if (typeid(ci::geom::WireCircle) == typeid(source)) {
					return Primitive::WIRE_CIRCLE;
				}
				else {
					return Primitive::UNKNOWN;
				}
			}

			static ci::geom::Capsule createCapsule(float radius, float length, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Capsule().radius(radius).length(length);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Capsule().radius(radius).length(length).subdivisionsAxis(6).subdivisionsHeight(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Capsule().radius(radius).length(length).subdivisionsAxis(60).subdivisionsHeight(20);
					break;
				}
			}

			static ci::geom::WireCapsule createWireCapsule(float radius, float length, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WireCapsule().radius(radius).length(length);
					break;
				case geometry::Quality::LOW:
					return ci::geom::WireCapsule().radius(radius).length(length).subdivisionsAxis(6).subdivisionsHeight(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WireCapsule().radius(radius).length(length).subdivisionsAxis(60).subdivisionsHeight(20);
					break;
				}
			}

			static ci::geom::Cone createCone(float radius, float height, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Cone().radius(radius).height(height);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Cone().radius(radius).height(height).subdivisionsAxis(6).subdivisionsHeight(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Cone().radius(radius).height(height).subdivisionsAxis(60).subdivisionsHeight(60);
					break;
				}
			}

			static ci::geom::WireCone createWireCone(float radius, float height, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WireCone().radius(radius).height(height);
					break;
				case geometry::Quality::LOW:
					return ci::geom::WireCone().radius(radius).height(height).subdivisionsAxis(6).subdivisionsHeight(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WireCone().radius(radius).height(height).subdivisionsAxis(60).subdivisionsHeight(60);
					break;
				}
			}

			static ci::geom::Cube createCube(ci::vec3 size, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Cube().size(size);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Cube().size(size).subdivisions(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Cube().size(size).subdivisions(10);
					break;
				}
			}

			static ci::geom::WireCube createWireCube(ci::vec3 size, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WireCube().size(size);
					break;
				case geometry::Quality::LOW:
					return ci::geom::WireCube().size(size).subdivisions(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WireCube().size(size).subdivisions(10);
					break;
				}
			}

			static ci::geom::Cylinder createCylinder(float radius, float height, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Cylinder().radius(radius).height(height);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Cylinder().radius(radius).height(height).subdivisionsAxis(6).subdivisionsCap(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Cylinder().radius(radius).height(height).subdivisionsAxis(60).subdivisionsCap(10).subdivisionsHeight(20);
					break;
				}
			}

			static ci::geom::WireCylinder createWireCylinder(float radius, float height, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WireCylinder().radius(radius).height(height);
					break;
				case geometry::Quality::LOW:
					return ci::geom::WireCylinder().radius(radius).height(height).subdivisionsAxis(6);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WireCylinder().radius(radius).height(height).subdivisionsAxis(60).subdivisionsHeight(20);
					break;
				}
			}

			static ci::geom::Plane createPlane(const ci::vec3& u, const ci::vec3& v, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Plane().axes(u, v).subdivisions(ci::vec2(10, 10));
					break;
				case geometry::Quality::LOW:
					return ci::geom::Plane().axes(u, v).subdivisions(ci::vec2(2, 2));
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Plane().axes(u, v).subdivisions(ci::vec2(100, 100));
					break;
				}
			}

			static ci::geom::WirePlane createWirePlane(const ci::vec3& u, const ci::vec3& v, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WirePlane().axes(u, v).subdivisions(ci::vec2(10, 10));
					break;
				case geometry::Quality::LOW:
					return ci::geom::WirePlane().axes(u, v).subdivisions(ci::vec2(2, 2));
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WirePlane().axes(u, v).subdivisions(ci::vec2(100, 100));
					break;
				}
			}

			static ci::geom::Rect createRect(ci::vec2 p1, ci::vec2 p2) {
				// only one quality of Rectangle available
				return ci::geom::Rect(ci::Rectf(p1, p2));
			}

			static ci::geom::WireRect createWireRect(ci::vec2 p1, ci::vec2 p2) {
				// only one quality of Rectangle available
				return ci::geom::WireRect(ci::Rectf(p1, p1));
			}


			static ci::geom::Sphere createSphere(float radius, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Sphere().radius(radius);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Sphere().radius(radius).subdivisions(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Sphere().radius(radius).subdivisions(10);
					break;
				}
			}

			static ci::geom::WireSphere createWireSphere(float radius) {
				// only one quality level of WireSphere, so just return it
				return ci::geom::WireSphere().radius(radius);
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

			static ci::gl::GlslProgRef getWireframeShader() {
				#if ! defined( CINDER_GL_ES )
				ci::gl::GlslProg::Format format;
				try {
					format = ci::gl::GlslProg::Format()
						.vertex(ci::app::loadAsset("shaders//wireframe//wireframe.vert"))
						.geometry(ci::app::loadAsset("shaders//wireframe//wireframe.geom"))
						.fragment(ci::app::loadAsset("shaders//wireframe//wireframe.frag"));
				}
				catch (ci::Exception &exc) {
					std::printf("sitara::ecs::geometry ERROR | Error loading wireframe shader: %s\n", exc.what());
				}
				return ci::gl::GlslProg::create(format);
				#endif // ! defined( CINDER_GL_ES )
			}

			static ci::gl::GlslProgRef getPhongShader() {
				ci::gl::GlslProgRef shader;
				try {
					#if defined( CINDER_GL_ES )
					shader = ci::gl::GlslProg::create(ci::app::loadAsset("shaders/wireframe/phong_es2.vert"), ci::app::loadAsset("shaders/wireframe/phong_es2.frag"));
					#else
					shader = ci::gl::GlslProg::create(ci::app::loadAsset("shaders/wireframe/phong.vert"), ci::app::loadAsset("shaders/wireframe/phong.frag"));
					#endif
				}
				catch (ci::Exception &exc) {
					std::printf("sitara::ecs::geometry ERROR | Error loading phong shader: %s\n", exc.what());
				}
				return shader;
			}
		}
	}
}