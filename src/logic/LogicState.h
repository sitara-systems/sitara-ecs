#pragma once

#include "entityx/Entity.h"

namespace sitara {
	namespace ecs {
		struct LogicState {
		public:
			LogicState(int stateId = 0) : mStateId(stateId) {
			}

			int getState() {
				return mStateId;
			}

			void setEnterFn(std::function<void()> fn) {
				mOnEnterFn = fn;
			}

			void enter() {
				mOnEnterFn();
			}

			void setUpdateFn(std::function<void()> fn) {
				mOnUpdateFn = fn;
			}

			void update() {
				mOnUpdateFn();
			}

			void setExitFn(std::function<void()> fn) {
				mOnExitFn = fn;
			}

			void exit() {
				mOnExitFn();
			}

		private:
			int mStateId;
			std::function<void()> mOnEnterFn;
			std::function<void()> mOnUpdateFn;
			std::function<void()> mOnExitFn;
		};
	}
}
