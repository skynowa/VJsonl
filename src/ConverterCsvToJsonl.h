/**
 * \file  ConverterCsvToJsonl.h
 * \brief Declares CSV-to-JSONL conversion facilities.
 */


#pragma once

#include "CsvReader.h"

//-------------------------------------------------------------------------------------------------
class ConverterCsvToJsonl final
{
public:
    static bool convert(const CsvData &csvData, QByteArray *outJsonl, QString *outError = nullptr);
};
//-------------------------------------------------------------------------------------------------
