/**
 * \file  ConverterCsvToJsonl.cpp
 * \brief Implements CSV-to-JSONL conversion facilities.
 */


#include "ConverterCsvToJsonl.h"

#include <QJsonDocument>
#include <QJsonObject>

//-------------------------------------------------------------------------------------------------
// Conversion
//-------------------------------------------------------------------------------------------------
bool
ConverterCsvToJsonl::convert(
    const CsvData &csvData,
    QByteArray    *outJsonl,
    QString       *outError
)
{
    if (outJsonl == nullptr) {
        if (outError != nullptr) {
            *outError = QStringLiteral("JSONL output is null");
        }

        return false;
    }

    outJsonl->clear();

    if (outError != nullptr) {
        outError->clear();
    }

    if (csvData.headers.isEmpty()) {
        if (outError != nullptr) {
            *outError = QStringLiteral("CSV data has no headers");
        }

        return false;
    }

    for (qsizetype rowIndex = 0; rowIndex < csvData.rows.size(); ++rowIndex) {
        const QStringList &row = csvData.rows.at(rowIndex);

        if (row.size() != csvData.headers.size()) {
            if (outError != nullptr) {
                *outError = QStringLiteral("CSV row %1 has an invalid field count").arg(rowIndex + 2);
            }

            outJsonl->clear();
            return false;
        }

        QJsonObject object;

        for (qsizetype column = 0; column < csvData.headers.size(); ++column) {
            object.insert(csvData.headers.at(column), row.at(column));
        }

        outJsonl->append(QJsonDocument(object).toJson(QJsonDocument::Compact));
        outJsonl->append('\n');
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
