/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "IRhinoCallbacks.h"

#include "prtx/EncodePreparator.h"
#include "prtx/Encoder.h"
#include "prtx/EncoderFactory.h"
#include "prtx/Singleton.h"
#include "prtx/prtx.h"

#include "prt/Callbacks.h"

#include <string>

// forward declare some classes to reduce header inclusion
namespace prtx {
class GenerateContext;
}

class RhinoEncoder : public prtx::GeometryEncoder {
public:
	static const std::wstring ID;
	static const std::wstring NAME;
	static const std::wstring DESCRIPTION;

	using prtx::GeometryEncoder::GeometryEncoder;

	RhinoEncoder(const RhinoEncoder&) = delete;
	RhinoEncoder(RhinoEncoder&&) = delete;
	RhinoEncoder& operator=(RhinoEncoder&) = delete;
	virtual ~RhinoEncoder() = default;

	virtual void init(prtx::GenerateContext& context) override;
	virtual void encode(prtx::GenerateContext& context, size_t initialShapeIndex) override;
	virtual void finish(prtx::GenerateContext& context) override;

private:
	prtx::DefaultNamePreparator mNamePreparator;
	prtx::EncodePreparatorPtr mEncodePreparator;

	void convertGeometry(const prtx::InitialShape& initialShape,
	                     const prtx::EncodePreparator::InstanceVector& instances, IRhinoCallbacks* cb);
};

class RhinoEncoderFactory : public prtx::EncoderFactory, public prtx::Singleton<RhinoEncoderFactory> {
public:
	static RhinoEncoderFactory* createInstance();

	RhinoEncoderFactory(const prt::EncoderInfo* info) : prtx::EncoderFactory(info) {}
	RhinoEncoderFactory(const RhinoEncoderFactory&) = delete;
	RhinoEncoderFactory(RhinoEncoderFactory&&) = delete;
	RhinoEncoderFactory& operator=(RhinoEncoderFactory&) = delete;
	virtual ~RhinoEncoderFactory() = default;

	virtual RhinoEncoder* create(const prt::AttributeMap* defaultOptions, prt::Callbacks* callbacks) const override {
		return new RhinoEncoder(getID(), defaultOptions, callbacks);
	}
};