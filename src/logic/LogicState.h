#pragma once

#include "entityx/Entity.h"

namespace sitara {
	namespace ecs {
		struct LogicState {
		public:
			LogicState(int stateId = 0) : mStateId(stateId), mOnEnterFn(nullptr), mOnUpdateFn(nullptr), mOnExitFn(nullptr) {
			}

			const int getState() {
				return mStateId;
			}

			void setState(int stateId) {
				exit();
				mStateId = stateId;
				enter();
			}

			void update() {
				if (mOnUpdateFn) {
					mOnUpdateFn();
				}
			}

			void setEnterFn(std::function<void()> fn) {
				mOnEnterFn = fn;
			}

			void setUpdateFn(std::function<void()> fn) {
				mOnUpdateFn = fn;
			}

			void setExitFn(std::function<void()> fn) {
				mOnExitFn = fn;
			}

		private:
			void enter() {
				if (mOnEnterFn) {
					mOnEnterFn();
				}
			}

			void exit() {
				if (mOnExitFn) {
					mOnExitFn();
				}
			}

			int mStateId;
			std::function<void()> mOnEnterFn;
			std::function<void()> mOnUpdateFn;
			std::function<void()> mOnExitFn;
		};
	}
}
