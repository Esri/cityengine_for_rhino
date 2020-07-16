/*
  COPYRIGHT (c) 2012-2020 Esri R&D Center Zurich
  TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
  Unpublished material - all rights reserved under the
  Copyright Laws of the United States and applicable international
  laws, treaties, and conventions.

  For additional information, contact:
  Environmental Systems Research Institute, Inc.
  Attn: Contracts and Legal Services Department
  380 New York Street
  Redlands, California, 92373
  USA

  email: contracts@esri.com
*/

#ifndef PRTX_SHAPEITERATOR_H_
#define PRTX_SHAPEITERATOR_H_

#include "prtx/prtx.h"
#include "prtx/Shape.h"
#include "prtx/GenerateContext.h"

#include <memory>


namespace prtx {

/**
 * ShapeIterators are used to generate and iterate over a shape tree. ShapeIterator defines the interface and a number of
 * specializations provide actual implementations.
 * Choose the one matching your application.
 *
 * @sa DepthFirstIterator, BreadthFirstIterator, LeafIterator, InitialShapeIterator, SelectedShapesIterator, CGAPrintIterator,
 * CGAErrorIterator, CGAReportIterator
 */
class PRTX_EXPORTS_API ShapeIterator {
public:
	/**
	 * @returns a valid shared pointer to the next shape while there are any or a nullptr pointer if there are no more shapes.
	 */
	virtual ShapePtr getNext() = 0;

protected:
	virtual ~ShapeIterator() {}
	ShapeIterator() {}
};


class DepthFirstIterator;
typedef std::shared_ptr<DepthFirstIterator> DepthFirstIteratorPtr;

/**
 * Generates a shape tree and traverses it in depth-first manner.
 */
class PRTX_EXPORTS_API DepthFirstIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a DepthFirstIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static DepthFirstIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	DepthFirstIterator();

public:
	virtual ~DepthFirstIterator();
};


class BreadthFirstIterator;
typedef std::shared_ptr<BreadthFirstIterator> BreadthFirstIteratorPtr;

/**
 * Generates a shape tree and traverses it in breadth-first manner.
 */
class PRTX_EXPORTS_API BreadthFirstIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a BreadthFirstIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static BreadthFirstIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	BreadthFirstIterator();

public:
	virtual ~BreadthFirstIterator();
};


class LeafIterator;
typedef std::shared_ptr<LeafIterator> LeafIteratorPtr;

/**
 * Generates a shape tree and traverses its leaves. Note that NIL'ed shapes are not part of the leaves.
 */
class PRTX_EXPORTS_API LeafIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a LeafIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static LeafIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	LeafIterator();

public:
	virtual ~LeafIterator();
};


class InitialShapeIterator;
typedef std::shared_ptr<InitialShapeIterator> InitialShapeIteratorPtr;

/**
 * Sets up the shape tree's root shape and evaluates its attributes. The successors are not generated.
 * This iterator will iterate over exactly one shape and is designed for evaluation of the default values
 * of a cga rule file's attributes.
 */
class PRTX_EXPORTS_API InitialShapeIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a InitialShapeIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static InitialShapeIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	InitialShapeIterator();

public:
	virtual ~InitialShapeIterator();
};


class SelectedShapesIterator;
typedef std::shared_ptr<SelectedShapesIterator> SelectedShapesIteratorPtr;

/**
 * Generates a shape tree and traverses the selected set of shapes.
 * Shapes can be selected by their IDs.
 * Generating an InitialShape more than once is guaranteed to result the same shape tree with the same shape IDs
 * if the OcclusionSet was not changed in-between the prt::generate() calls and no internal multithreading is used.
 */
class PRTX_EXPORTS_API SelectedShapesIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a BreadthFirstIteratorPtr.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 * @param selectedShapes    IDs of the shapes to extract.
	 */
	static SelectedShapesIteratorPtr create(GenerateContext& context, size_t initialShapeIndex, const std::vector<int32_t>& selectedShapes);

protected:
	SelectedShapesIterator();

public:
	virtual ~SelectedShapesIterator();
};


class CGAPrintIterator;
typedef std::shared_ptr<CGAPrintIterator> CGAPrintIteratorPtr;

/**
 * Generates a shape tree and traverses all shapes which have output from the CGA print operations/functions.
 */
class PRTX_EXPORTS_API CGAPrintIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a InitialShapeIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static CGAPrintIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	CGAPrintIterator();

public:
	virtual ~CGAPrintIterator();
};


class CGAErrorIterator;
typedef std::shared_ptr<CGAErrorIterator> CGAErrorIteratorPtr;

/**
 * Generates a shape tree and traverses all shapes which have CGA errors.
 */
class PRTX_EXPORTS_API CGAErrorIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a InitialShapeIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static CGAErrorIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

	/**
	 * Gets errors and warnings encountered on reading external assets during generation.
	 * @param[out] errors The errors will be copied into this vector.
	 */
	virtual void getAssetsErrors(prtx::AssetErrorPtrVector& errors) = 0;

protected:
	CGAErrorIterator();

public:
	virtual ~CGAErrorIterator();
};


class CGAReportIterator;
typedef std::shared_ptr<CGAReportIterator> CGAReportIteratorPtr;

/**
 * Generates a shape tree and traverses all shapes which have output from the CGA report operations.
 */
class PRTX_EXPORTS_API CGAReportIterator : public ShapeIterator {
public:
	/**
	 * Creates an instance of a InitialShapeIterator.
	 * @param context           GenerateContext to use.
	 * @param initialShapeIndex Index of the InitialShape in the context to use as start shape.
	 */
	static CGAReportIteratorPtr create(GenerateContext& context, size_t initialShapeIndex);

protected:
	CGAReportIterator();

public:
	virtual ~CGAReportIterator();
};


} // namespace



#endif /* PRTX_SHAPEITERATOR_H_ */
