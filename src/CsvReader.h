/**
 * \file  CsvReader.h
 * \brief Declares CSV file parsing facilities.
 */


#pragma once

#include <QByteArray>
#include <QStringList>
#include <QVector>

//-------------------------------------------------------------------------------------------------
struct CsvData final
{
    QStringList          headers;
    QVector<QStringList> rows;
};

//-------------------------------------------------------------------------------------------------
class CsvReader final
{
public:
    static constexpr qsizetype maxInputBytes = 64 * 1024 * 1024;
    static constexpr qsizetype maxRows = 500000;
    static constexpr qsizetype maxFieldCharacters = 1024 * 1024;

    // CSV loading and parsing
    static bool readFile(const QString &fileName, CsvData *outData, QString *outError = nullptr);
    static bool parse(const QByteArray &data, CsvData *outData, QString *outError = nullptr);
};
//-------------------------------------------------------------------------------------------------
