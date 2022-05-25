#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/TriMesh.h"
#include "cinder/DataSource.h"
#include <typeinfo>

namespace sitara {
	namespace ecs {
		namespace geometry {
			enum Primitive {
				BOX, CAPSULE, CONE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE,
				WIRE_BOX, WIRE_CAPSULE, WIRE_CONE, WIRE_CYLINDER, WIRE_ICOSAHEDRON, WIRE_SPHERE, WIRE_TORUS, WIRE_PLANE,
                RECT, ROUNDED_RECT, CIRCLE, RING, WIRE_RECT, WIRE_ROUNDED_RECT, WIRE_CIRCLE,
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
					return Primitive::BOX;
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
				else if (typeid(ci::geom::WireCapsule) == typeid(source)) {
					return Primitive::WIRE_CAPSULE;
				}
				else if (typeid(ci::geom::WireCone) == typeid(source)) {
					return Primitive::WIRE_CONE;
				}
				else if (typeid(ci::geom::WireCube) == typeid(source)) {
					return Primitive::WIRE_BOX;
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
				else if (typeid(ci::geom::WirePlane) == typeid(source)) {
					return Primitive::WIRE_PLANE;
				}
				else if (typeid(ci::geom::Rect) == typeid(source)) {
                    return Primitive::RECT;
                }
				else if (typeid(ci::geom::RoundedRect) == typeid(source)) {
                    return Primitive::ROUNDED_RECT;
                }
				else if (typeid(ci::geom::Circle) == typeid(source)) {
                    return Primitive::CIRCLE;
                }
				else if (typeid(ci::geom::Ring) == typeid(source)) {
                    return Primitive::RING;
                }
				else if (typeid(ci::geom::WireRect) == typeid(source)) {
                    return Primitive::WIRE_RECT;
                }
				else if (typeid(ci::geom::WireRoundedRect) == typeid(source)) {
                    return Primitive::WIRE_ROUNDED_RECT;
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
				default:
					return ci::geom::Capsule().radius(radius).length(length);
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
				default:
					return ci::geom::WireCapsule().radius(radius).length(length);
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
				default:
					return ci::geom::Cone().radius(radius).height(height);
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
				default:
					return ci::geom::WireCone().radius(radius).height(height);
				}
			}

			static ci::geom::Cube createBox(ci::vec3 halfSize, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Cube().size(2.0f * halfSize);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Cube().size(2.0f * halfSize).subdivisions(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Cube().size(2.0f * halfSize).subdivisions(10);
					break;
				default:
					return ci::geom::Cube().size(2.0f * halfSize);
					break;
				}
			}

			static ci::geom::WireCube createWireBox(ci::vec3 halfSize, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WireCube().size(2.0f * halfSize);
					break;
				case geometry::Quality::LOW:
					return ci::geom::WireCube().size(2.0f * halfSize).subdivisions(1);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WireCube().size(2.0f * halfSize).subdivisions(10);
					break;
				default:
					return ci::geom::WireCube().size(2.0f * halfSize);
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
				default:
					return ci::geom::Cylinder().radius(radius).height(height);
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
				default:
					return ci::geom::WireCylinder().radius(radius).height(height);
				}
			}

			static ci::geom::Icosahedron createIcosahedron() {
				// only one quality level of Icosahedron, so just return it
				return ci::geom::Icosahedron();
			}

			static ci::geom::WireIcosahedron createWireIcosahedron() {
				// only one quality level of WireIcosahedron, so just return it
				return ci::geom::WireIcosahedron();
			}

			static ci::geom::Plane createPlane(const ci::vec3& u, const ci::vec3& v, const ci::vec2& size, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Plane().axes(u, v).size(size).subdivisions(ci::vec2(10, 10));
					break;
				case geometry::Quality::LOW:
					return ci::geom::Plane().axes(u, v).size(size).subdivisions(ci::vec2(2, 2));
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Plane().axes(u, v).size(size).subdivisions(ci::vec2(100, 100));
					break;
				}
			}

			static ci::geom::WirePlane createWirePlane(const ci::vec3& u, const ci::vec3& v, const ci::vec2& size, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::WirePlane().axes(u, v).size(size).subdivisions(ci::vec2(10, 10));
					break;
				case geometry::Quality::LOW:
					return ci::geom::WirePlane().axes(u, v).size(size).subdivisions(ci::vec2(2, 2));
					break;
				case geometry::Quality::HIGH:
					return ci::geom::WirePlane().axes(u, v).size(size).subdivisions(ci::vec2(100, 100));
					break;
				}
			}

			static ci::geom::Sphere createSphere(float radius, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
				case geometry::Quality::DEFAULT:
					return ci::geom::Sphere().radius(radius);
					break;
				case geometry::Quality::LOW:
					return ci::geom::Sphere().radius(radius).subdivisions(6);
					break;
				case geometry::Quality::HIGH:
					return ci::geom::Sphere().radius(radius).subdivisions(60);
					break;
				default:
					return ci::geom::Sphere().radius(radius);
				}
			}

			static ci::geom::WireSphere createWireSphere(float radius) {
				// only one quality level of WireSphere, so just return it
				return ci::geom::WireSphere().radius(radius);
			}

			static ci::geom::Teapot createTeapot(float radius, geometry::Quality quality = geometry::Quality::DEFAULT) {
				switch (quality) {
					case DEFAULT:
						return ci::geom::Teapot();
						break;
					case LOW:
						return ci::geom::Teapot().subdivisions(2);
						break;
					case HIGH:
						return ci::geom::Teapot().subdivisions(12);
						break;
					default:
						return ci::geom::Teapot();
				}
			}

			static ci::geom::Rect createRect(ci::vec2 dims,
                                                         geometry::Quality quality = geometry::Quality::DEFAULT) {
                ci::Rectf r(0, 0, dims.x, dims.y);
                return ci::geom::Rect(r);
            }

			static ci::geom::RoundedRect createRoundedRect(ci::vec2 dims, float cornerRadius, 
                                                geometry::Quality quality = geometry::Quality::DEFAULT) {
                ci::Rectf r(0, 0, dims.x, dims.y);
                return ci::geom::RoundedRect(r, cornerRadius);
            }

			static ci::geom::Circle createCircle(float radius, geometry::Quality quality = geometry::Quality::DEFAULT) {
                switch (quality) {
                    case DEFAULT:
                        return ci::geom::Circle().subdivisions(24).radius(radius);
                        break;
                    case LOW:
                        return ci::geom::Circle().subdivisions(8).radius(radius);
                        break;
                    case HIGH:
                        return ci::geom::Circle().subdivisions(120).radius(radius);
                        break;
                    default:
                        return ci::geom::Circle().subdivisions(24).radius(radius);
                }
            }

			static ci::geom::Ring createRing(float ringWidth, geometry::Quality quality = geometry::Quality::DEFAULT) {
                switch (quality) {
                    case DEFAULT:
                        return ci::geom::Ring().subdivisions(24).width(ringWidth);
                        break;
                    case LOW:
                        return ci::geom::Ring().subdivisions(8).width(ringWidth);
                        break;
                    case HIGH:
                        return ci::geom::Ring().subdivisions(120).width(ringWidth);
                        break;
                    default:
                        return ci::geom::Ring().subdivisions(24).width(ringWidth);
                }
            }

			static ci::geom::WireRect createWireRect(ci::vec2 dims, geometry::Quality quality = geometry::Quality::DEFAULT) {
                ci::Rectf r(0, 0, dims.x, dims.y);
                return ci::geom::WireRect(r);
            }

            static ci::geom::WireRoundedRect createWireRoundedRect(ci::vec2 dims,
                                                           float cornerRadius,
                                                           geometry::Quality quality = geometry::Quality::DEFAULT) {
                ci::Rectf r(0, 0, dims.x, dims.y);
                return ci::geom::WireRoundedRect(r, cornerRadius);
            }

            static ci::geom::WireCircle createWireCircle(float radius, geometry::Quality quality = geometry::Quality::DEFAULT) {
                switch (quality) {
                    case DEFAULT:
                        return ci::geom::WireCircle().subdivisions(24).radius(radius);
                        break;
                    case LOW:
                        return ci::geom::WireCircle().subdivisions(8).radius(radius);
                        break;
                    case HIGH:
                        return ci::geom::WireCircle().subdivisions(120).radius(radius);
                        break;
                    default:
                        return ci::geom::WireCircle().subdivisions(24).radius(radius);
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