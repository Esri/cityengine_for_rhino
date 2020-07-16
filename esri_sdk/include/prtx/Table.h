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

#ifndef SRC_PRTX_TABLE_H_
#define SRC_PRTX_TABLE_H_



#include "prtx/prtx.h"
#include "prtx/Content.h"

#include <vector>
#include <memory>
#include <string>

namespace prtx {


class Table;
typedef std::shared_ptr<Table> TablePtr;
typedef std::vector<TablePtr>  TablePtrVector;

class PRTX_EXPORTS_API Table : public Content {
public:
	virtual ~Table();

	virtual size_t getColumnsCount() const = 0;
	virtual size_t getRowsCount()    const = 0;
	virtual size_t getCellsCount()   const = 0;

	virtual std::shared_ptr<std::vector<std::shared_ptr<std::wstring>>> getCellsAsStrings() const = 0;
	virtual std::shared_ptr<std::vector<double>>                        getCellsAsFloats()  const = 0;

protected:
	Table();
};

}

#endif /* SRC_PRTX_TABLE_H_ */
