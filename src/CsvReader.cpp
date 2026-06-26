/**
 * \file  CsvReader.cpp
 * \brief Implements CSV file parsing facilities.
 */


#include "CsvReader.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>

//-------------------------------------------------------------------------------------------------
// CSV loading and parsing
//-------------------------------------------------------------------------------------------------
bool
CsvReader::readFile(
    const QString &fileName,
    CsvData       *outData,
    QString       *outError
)
{
    QFile file(fileName);
    const qint64 fileSize = QFileInfo(fileName).size();

    if (fileSize > maxInputBytes) {
        if (outError != nullptr) {
            *outError = QStringLiteral("CSV file is too large: %1 bytes; limit is %2 bytes")
                .arg(fileSize)
                .arg(maxInputBytes);
        }

        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        if (outError != nullptr) {
            *outError = file.errorString();
        }

        return false;
    }

    const QByteArray data = file.readAll();

    if (file.error() != QFileDevice::NoError) {
        if (outError != nullptr) {
            *outError = file.errorString();
        }

        return false;
    }

    return parse(data, outData, outError);
}

//-------------------------------------------------------------------------------------------------
bool
CsvReader::parse(
    const QByteArray &data,
    CsvData          *outData,
    QString          *outError
)
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

    if (text.size() > maxInputBytes) {
        return fail(QStringLiteral("CSV input is too large: %1 characters; limit is %2 characters")
            .arg(text.size())
            .arg(maxInputBytes));
    }

    if (text.startsWith(QChar::ByteOrderMark)) {
        text.remove(0, 1);
    }

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
        const auto character = text.at(index);

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

        if (field.size() > maxFieldCharacters) {
            return fail(QStringLiteral("CSV field at position %1 is too large").arg(index + 1));
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

    if (records.size() > maxRows + 1) {
        return fail(QStringLiteral("CSV row count %1 exceeds limit %2")
            .arg(records.size() - 1)
            .arg(maxRows));
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
