// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QColor>
#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    //rule name
    ruleNameFormat.setFontWeight(QFont::Bold);
    ruleNameFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression(QStringLiteral("rule\\s+(\\S+)"));
    rule.format = ruleNameFormat;
    highlightingRules.append(rule);

    //$string
    stringFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\$\\S+"));
    rule.format = stringFormat;
    highlightingRules.append(rule);

    //Hex
    hexFormat.setForeground( Qt::darkRed );

    const QString funcPatterns[] = {
        QStringLiteral("\\{\\s*([A-F0-9\\s]+)\\s*\\}"),QStringLiteral("\\b0x[0-9A-Fa-f]+\\b")
    };
    for (const QString &pattern : funcPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = hexFormat;
        highlightingRules.append(rule);
    }
    QColor color;
    color.setNamedColor( "#2596be" );
    keywordFormat.setForeground(color);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\ball\\b"), QStringLiteral("\\band\\b"), QStringLiteral("\\bany\\b"), QStringLiteral("\\bascii\\b"), QStringLiteral("\\bat\\b"),
        QStringLiteral("\\bbase64\\b"), QStringLiteral("\\bbase64wide\\b"), QStringLiteral("\\bcondition"), QStringLiteral("\\bcontains\\b"), QStringLiteral("\\bendswith\\b"),
        QStringLiteral("\\bendswith\\b"), QStringLiteral("\\bentrypoint\\b"), QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfilesize\\b"), QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\biendswith\\b"), QStringLiteral("\\bicontains\\b"), QStringLiteral("\\bimport\\b"), QStringLiteral("\\bglobal\\b"), QStringLiteral("\\bfullword\\b"),
        QStringLiteral("\\bint16be\\b"), QStringLiteral("\\bint16\\b"), QStringLiteral("\\binclude\\b"), QStringLiteral("\\bin\\b"), QStringLiteral("\\biequals\\b"),
        QStringLiteral("\\bistartswith\\b"), QStringLiteral("\\bint8be\\b"), QStringLiteral("\\bint8\\b"), QStringLiteral("\\bint32be\\b"), QStringLiteral("\\bint32\\b"),
        QStringLiteral("\\bnot\\b"), QStringLiteral("\\bnone\\b"), QStringLiteral("\\bnocase\\b"), QStringLiteral("\\bmeta\\b"), QStringLiteral("\\bmatches\\b"),
        QStringLiteral("\\bstartswith\\b"), QStringLiteral("\\brule\\b"), QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bor\\b"), QStringLiteral("\\bof\\b"),
        QStringLiteral("\\buint16be\\b"), QStringLiteral("\\buint16\\b"), QStringLiteral("\\btrue\\b"), QStringLiteral("\\bthem\\b"), QStringLiteral("\\bstrings\\b"),
        QStringLiteral("\\bwide\\b"), QStringLiteral("\\buint8be\\b"), QStringLiteral("\\buint8\\b"), QStringLiteral("\\buint32be\\b"), QStringLiteral("\\buint32\\b"),
        QStringLiteral("\\bdefined\\b"), QStringLiteral("\\bxor\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
//! [0] //! [1]
    }
//! [1]

//! [3]
    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);
//! [3]

//! [4]
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [4]

//! [5]
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [5]

//! [6]
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
//! [11]
