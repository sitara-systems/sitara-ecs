#pragma once

namespace sitara {
	namespace ecs {
		/*
		 * We standardize units on the MKS system of units (meters - kilograms - seconds); functions to translate to imperial units
		 * of length are here mostly for convenience.  Try to stay in MKS whenever possible!
		 *
		 * Also note that Bullet Physics' 
		 */

		class Units {
		public:
			static Units& getInstance(float pixelsPerMeter = 100.0f) {
				static Units instance(pixelsPerMeter);
				return instance;
			}

			Units(Units const&) = delete;             // Copy construct
			Units(Units&&) = delete;                  // Move construct
			Units& operator=(Units const&) = delete;  // Copy assign
			Units& operator=(Units &&) = delete;      // Move assign

			void setPixelsPerMeter(float ppm) {
				mPixelsPerMeter = ppm;
				mPixelsPerFoot = ppm / mFeetPerMeter;
			}

			float getPixelsFromMeters(float meters) {
				return checkLengthUnits(meters) * mPixelsPerMeter;
			}

			ci::vec3 getPixelsFromMeters(ci::vec3 meters) {
				return ci::vec3(checkLengthUnits(meters.x) * mPixelsPerMeter,
					checkLengthUnits(meters.y) * mPixelsPerMeter,
					checkLengthUnits(meters.z) * mPixelsPerMeter
					);
			}

			ci::vec2 getPixelsFromMeters(ci::vec2 meters) {
				return ci::vec2(checkLengthUnits(meters.x) * mPixelsPerMeter,
					checkLengthUnits(meters.y) * mPixelsPerMeter
				);
			}

			float getMeters(float pixels) {
				return pixels / mPixelsPerMeter;
			}

			void setPixelsPerCentimeter(float ppcm) {
				mPixelsPerMeter = ppcm * 100.0f;
				mPixelsPerFoot = 100.0f * ppcm / mFeetPerMeter;
			}

			float getPixelsFromCentimeters(float centimeters) {
				return checkLengthUnits(centimeters / 100.0f) * mPixelsPerMeter;
			}

			ci::vec2 getPixelsFromCentimeters(ci::vec2 centimeters) {
				return centimeters / 100.0f * mPixelsPerMeter;
			}

			ci::vec3 getPixelsFromCentimeters(ci::vec3 centimeters) {
				return centimeters / 100.0f * mPixelsPerMeter;
			}

			float getCentimeters(float pixels) {
				return pixels / (mPixelsPerMeter / 100.0f);
			}

			ci::vec2 getCentimeters(ci::vec2 pixels) {
				return pixels / (mPixelsPerMeter / 100.0f);
			}

			ci::vec3 getCentimeters(ci::vec3 pixels) {
				return pixels / (mPixelsPerMeter / 100.0f);
			}

			void setPixelsPerFoot(float ppf) {
				mPixelsPerFoot = ppf;
				mPixelsPerMeter = ppf * mFeetPerMeter;
			}

			float getPixelsFromFeet(float feet) {
				return checkLengthUnits(feet / mFeetPerMeter) * mPixelsPerFoot;
			}

			float getFeet(float pixels) {
				return pixels / mPixelsPerFoot;
			}

			void setPixelsPerInch(float ppi) {
				mPixelsPerMeter = ppi * 12.0f * mFeetPerMeter;
				mPixelsPerFoot = ppi * 12.0f;
			}

			float getPixelsFromInches(float inches) {
				return checkLengthUnits((inches / 12.0f) / mFeetPerMeter) * (mPixelsPerFoot / 12.0f);
			}

			float getInches(float pixels) {
				return pixels / (mPixelsPerFoot / 12.0f);
			}

		private:
			Units(float pixelsPerMeter) {
				setPixelsPerMeter(pixelsPerMeter);
			};

			float checkLengthUnits(float meters) {
				if (meters < 0.2 && meters != 0) {
					//std::printf("sitara::ecs::geometry WARNING | Bullet Physics may misbehave with units smaller than 20 cm; please scale your world size up.\n");
				}
				return meters;
			}

			float mFeetPerMeter = 3.28f;
			float mPixelsPerMeter = 100.0f;
			float mPixelsPerFoot = 30.48f;
		};
	}
}