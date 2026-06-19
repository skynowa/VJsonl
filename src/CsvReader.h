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
    static bool readFile(const QString &fileName, CsvData *outData, QString *outError = nullptr);
    static bool parse(const QByteArray &data, CsvData *outData, QString *outError = nullptr);
};
//-------------------------------------------------------------------------------------------------
