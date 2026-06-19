/**
 * \file  CsvReader.cpp
 * \brief Implements CSV file parsing facilities.
 */


#include "CsvReader.h"

#include <QFile>
#include <QSet>

//-------------------------------------------------------------------------------------------------
bool CsvReader::readFile(const QString &fileName, CsvData *outData, QString *outError)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        if (outError != nullptr) {
            *outError = file.errorString();
        }

        return false;
    }

    return parse(file.readAll(), outData, outError);
}

//-------------------------------------------------------------------------------------------------
bool CsvReader::parse(const QByteArray &data, CsvData *outData, QString *outError)
{
    if (outData == nullptr) {
        if (outError != nullptr) {
            *outError = QStringLiteral("CSV output is null");
        }

        return false;
    }

    *outData = {};

    if (outError != nullptr) {
        outError->clear();
    }

    QString text = QString::fromUtf8(data);

    if (text.startsWith(QChar::ByteOrderMark)) {
        text.remove(0, 1);
    }

    QVector<QStringList> records;
    QStringList row;
    QString field;
    bool inQuotes = false;
    bool quoteClosed = false;
    bool fieldStarted = false;

    auto fail = [&](const QString &message) {
        if (outError != nullptr) {
            *outError = message;
        }

        return false;
    };

    auto finishField = [&] {
        row.push_back(field);
        field.clear();
        fieldStarted = false;
        quoteClosed = false;
    };

    auto finishRecord = [&] {
        finishField();
        records.push_back(row);
        row.clear();
    };

    for (qsizetype index = 0; index < text.size(); ++index) {
        const QChar character = text.at(index);

        if (inQuotes) {
            if (character == QLatin1Char('"')) {
                if (index + 1 < text.size() && text.at(index + 1) == QLatin1Char('"')) {
                    field += QLatin1Char('"');
                    ++index;
                } else {
                    inQuotes = false;
                    quoteClosed = true;
                }
            } else if (
                character == QLatin1Char('\r')
                && index + 1 < text.size()
                && text.at(index + 1) == QLatin1Char('\n')
            ) {
                field += QLatin1Char('\n');
                ++index;
            } else {
                field += character;
            }

            continue;
        }

        if (quoteClosed) {
            if (character == QLatin1Char(',')) {
                finishField();
                continue;
            }

            if (character == QLatin1Char('\r') || character == QLatin1Char('\n')) {
                finishRecord();

                if (
                    character == QLatin1Char('\r')
                    && index + 1 < text.size()
                    && text.at(index + 1) == QLatin1Char('\n')
                ) {
                    ++index;
                }

                continue;
            }

            return fail(QStringLiteral("Unexpected character after closing quote at position %1").arg(index + 1));
        }

        if (character == QLatin1Char('"')) {
            if (fieldStarted) {
                return fail(QStringLiteral("Unexpected quote at position %1").arg(index + 1));
            }

            inQuotes = true;
            fieldStarted = true;
        } else if (character == QLatin1Char(',')) {
            finishField();
        } else if (character == QLatin1Char('\r') || character == QLatin1Char('\n')) {
            finishRecord();

            if (
                character == QLatin1Char('\r')
                && index + 1 < text.size()
                && text.at(index + 1) == QLatin1Char('\n')
            ) {
                ++index;
            }
        } else {
            field += character;
            fieldStarted = true;
        }
    }

    if (inQuotes) {
        return fail(QStringLiteral("Unterminated quoted field"));
    }

    if (quoteClosed || fieldStarted || !row.isEmpty()) {
        finishRecord();
    }

    if (records.isEmpty()) {
        return fail(QStringLiteral("CSV file has no header row"));
    }

    outData->headers = records.takeFirst();
    QSet<QString> headerNames;

    for (QString &header : outData->headers) {
        header = header.trimmed();

        if (header.isEmpty()) {
            return fail(QStringLiteral("CSV header contains an empty column name"));
        }

        if (headerNames.contains(header)) {
            return fail(QStringLiteral("CSV header contains duplicate column '%1'").arg(header));
        }

        headerNames.insert(header);
    }

    for (qsizetype rowIndex = 0; rowIndex < records.size(); ++rowIndex) {
        QStringList values = records.at(rowIndex);

        if (values.size() > outData->headers.size()) {
            return fail(
                QStringLiteral("CSV row %1 has %2 fields; expected %3")
                    .arg(rowIndex + 2)
                    .arg(values.size())
                    .arg(outData->headers.size())
            );
        }

        while (values.size() < outData->headers.size()) {
            values.push_back(QString());
        }

        outData->rows.push_back(std::move(values));
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
