#pragma once

#include "wrap.h"

#include <memory>

#define RHINOPRTAPI   __declspec(dllexport)

namespace RhinoPRT {
	struct IRhinoPRTAPI {
		virtual void initializeRhinoPRT() = 0;
		virtual void realeaseRhinoPRT() = 0;
		virtual bool isPRTInitialized() = 0;
	};

	struct RhinoPRTAPI : public IRhinoPRTAPI {
	public:
		// Inherited via IRhinoPRTAPI
		virtual void initializeRhinoPRT() override {
			if (!prtCtx)
				prtCtx.reset(new PRTContext(prt::LOG_DEBUG));
		}

		virtual void realeaseRhinoPRT() override {
			prtCtx.reset();
		}

		virtual bool isPRTInitialized() override {
			return (bool)prtCtx;
		}

	private:
		std::unique_ptr<PRTContext> prtCtx;
	};

	// Global PRT API
	RhinoPRTAPI* myPRTAPI = new RhinoPRT::RhinoPRTAPI();
}

extern "C" {
	RHINOPRTAPI RhinoPRT::IRhinoPRTAPI* GetRhinoPRT() {
		return RhinoPRT::myPRTAPI;
	}
}