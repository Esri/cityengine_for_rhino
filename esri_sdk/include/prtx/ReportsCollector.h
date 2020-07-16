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

#ifndef PRTX_REPORTSCOLLECTOR_H_
#define PRTX_REPORTSCOLLECTOR_H_

#include "prtx/prtx.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif

#include <memory>
#include <map>


namespace prtx {

class GenerateContext;

class Reports;
using ReportsPtr = std::shared_ptr<Reports>;

using ReportDomains = std::map<uint32_t, prtx::ReportsPtr>;

class ReportsAccumulator;
typedef std::shared_ptr<ReportsAccumulator> ReportsAccumulatorPtr;

/**
 * Interface for sorting the incoming reports into domains.
 */
class PRTX_EXPORTS_API ReportsAccumulator {
public:
	ReportsAccumulator()                                     = default;
	ReportsAccumulator(const ReportsAccumulator&)            = delete;
	ReportsAccumulator& operator=(const ReportsAccumulator&) = delete;

	/**
	 * Subclasses may override this function to change the way how reports are inserted into
	 * the report domain map. The default implementation inserts (or appends) the reports to
	 * the specified domain.
	 */
	virtual void add(
			ReportDomains& reportDomains,
			uint32_t domain,
			const prtx::ReportsPtr& reports
	) = 0;

	/**
	 * Subclasses may implement a post-process step to finalize the report domains.
	 * The default implementation does nothing.
	 */
	virtual void finalize(ReportDomains& reportDomains) const = 0;
};


/**
 * Simple concatenation of all reports, no summarizing, no report key/type clash resolution
 */
class PRTX_EXPORTS_API AppendingReportsAccumulator : public ReportsAccumulator {
public:
	static ReportsAccumulatorPtr create();
};


/**
 * Returns only the sum for all reports:
 * - sum: <key>
 *
 * For booleans the sum is computed by using the binary values.
 * For strings the sum is the most frequent value.
 */
class PRTX_EXPORTS_API SumReportsAccumulator : public AppendingReportsAccumulator {
public:
	static ReportsAccumulatorPtr create();
};


/**
 * Returns a summary for all reports:
 * - cardinality: <key>_n
 * - average: <key>_avg
 * - sum: <key>_sum
 * - minimum: <key>_min
 * - maximum: <key>_max
 *
 * For booleans the summary is computed by using the binary values.
 * For strings the sum and avg are the most frequent values while min/max are computed lexicographically.
 */
class PRTX_EXPORTS_API SummarizingReportsAccumulator : public AppendingReportsAccumulator {
public:
	static ReportsAccumulatorPtr create();
};


/**
 * This report accumulator only keeps the first report item added to a domain.
 */
class PRTX_EXPORTS_API WriteFirstReportsAccumulator : public ReportsAccumulator {
public:
	static ReportsAccumulatorPtr create();
};


class ReportingStrategy;
typedef std::shared_ptr<ReportingStrategy> ReportingStrategyPtr;

/**
 * Interface for a CGA reporting strategy. Used together with an ReportsAccumulator
 * to collect and sort CGA reports into domains.
 *
 * \sa ReportsAccumulator
 */
class PRTX_EXPORTS_API ReportingStrategy {
public:
	ReportingStrategy()                                    = default;
	ReportingStrategy(const ReportingStrategy&)            = delete;
	ReportingStrategy& operator=(const ReportingStrategy&) = delete;
	virtual ~ReportingStrategy() { }

	/**
	 * subclasses may throw std::domain_error if accumulation per domain is not supported
	 * @param
	 * @returns accumulated reports for one domain.
	 */
	virtual const prtx::ReportsPtr& getReports(uint32_t domain) const = 0;

	/**
	 * subclasses may throw std::domain_error if domain-less report accumulation is not supported
	 * @returns accumulated reports for all domains
	 */
	virtual const prtx::ReportsPtr& getReports() const = 0;
};


/**
 * Collects CGA reports by traversing the shape tree for each leaf shape from leaf to root, starting
 * at the "left"-most leaf shape. Each leaf shape ID is used as its report domain.
 */
class PRTX_EXPORTS_API LeafShapeReportingStrategy : public ReportingStrategy {
public:
	static ReportingStrategyPtr create(
			prtx::GenerateContext& context,
			size_t initialShapeIndex,
			ReportsAccumulatorPtr reportsAccumulator
	);
};


/**
 * Traverses the whole shape tree in a breadth-first manner and adds all reports to the accumulator
 * using a single domain.
 */
class PRTX_EXPORTS_API AllShapesReportingStrategy : public ReportingStrategy {
public:
	static ReportingStrategyPtr create(
			prtx::GenerateContext& context,
			size_t initialShapeIndex,
			ReportsAccumulatorPtr reportsAccumulator
	);
};


/**
 * Collects the reports for each shape with its ID as report domain.
 */
class PRTX_EXPORTS_API SingleShapeReportingStrategy : public ReportingStrategy {
public:
	static ReportingStrategyPtr create(
			prtx::GenerateContext& context,
			size_t initialShapeIndex,
			ReportsAccumulatorPtr reportsAccumulator
	);
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#endif
