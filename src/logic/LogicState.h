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

			void update(entityx::ComponentHandle<LogicState> handle) {
				if (mOnUpdateFn) {
					mOnUpdateFn(handle);
				}
			}

			void setEnterFn(std::function<void(entityx::ComponentHandle<LogicState>)> fn) {
				mOnEnterFn = fn;
			}

			void setUpdateFn(std::function<void(entityx::ComponentHandle<LogicState>)> fn) {
				mOnUpdateFn = fn;
			}

			void setExitFn(std::function<void(entityx::ComponentHandle<LogicState>)> fn) {
				mOnExitFn = fn;
			}

		private:
			void enter(entityx::ComponentHandle<LogicState> handle) {
				if (mOnEnterFn) {
					mOnEnterFn(handle);
				}
			}

			void exit(entityx::ComponentHandle<LogicState> handle) {
				if (mOnExitFn) {
					mOnExitFn(handle);
				}
			}

			int mStateId;
			std::function<void(entityx::ComponentHandle<LogicState>)> mOnEnterFn;
			std::function<void(entityx::ComponentHandle<LogicState>)> mOnUpdateFn;
			std::function<void(entityx::ComponentHandle<LogicState>)> mOnExitFn;

			friend class StateSystem;
		};
	}
}
