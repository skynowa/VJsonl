/**
 * \file  Utils/Demangle.cpp
 * \brief Implements helpers for demangling C++ symbols in backtraces.
 */


#include "Utils/Demangle.h"

#include <QByteArray>
#include <QRegularExpression>

#include <cxxabi.h>
#include <cstdlib>

namespace
{

QString
demangleSymbol(
    const QString &symbol
)
{
    const QByteArray symbolBytes = symbol.toUtf8();
    int status = 0;
    char *demangled = abi::__cxa_demangle(symbolBytes.constData(), nullptr, nullptr, &status);

    if (status != 0 || demangled == nullptr) {
        std::free(demangled);
        return symbol;
    }

    const QString result = QString::fromUtf8(demangled);
    std::free(demangled);
    return result;
}

}

//-------------------------------------------------------------------------------------------------
namespace demangle_utils
{
QString
demangleSymbols(
    const QString &text
)
{
    static const QRegularExpression mangledSymbol(QStringLiteral(R"(_Z[A-Za-z0-9_]+)"));
    QString result;
    qsizetype lastEnd = 0;
    QRegularExpressionMatchIterator matches = mangledSymbol.globalMatch(text);

    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        const QString symbol = match.captured(0);
        const QString demangled = demangleSymbol(symbol);

        result += text.mid(lastEnd, match.capturedStart() - lastEnd);
        result += demangled;
        lastEnd = match.capturedEnd();
    }

    result += text.mid(lastEnd);
    return result;
}
}
//-------------------------------------------------------------------------------------------------
