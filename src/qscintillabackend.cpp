/*
 * @file    src/qscintillabackend.cpp
 * @brief   
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>

#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QTabWidget>
#include <QColor>
#include <QFont>

#include "qscintillabackend.h"



QScintillaBackend::QScintillaBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_tabs(new QTabWidget(parent)),
	  m_editor(new QsciScintilla(m_tabs))
{
	m_tabs->setDocumentMode(
		true
	);

	m_tabs->setTabsClosable(
		false
	);

	m_tabs->setMovable(
		true
	);

	m_editor->setUtf8(
		true
	);

	auto *lexer =
		new QsciLexerCPP(
			m_editor
		);

	m_editor->setLexer(
		lexer
	);

	configureEditorAppearance(
		lexer
	);


	m_editor->setMarginType(
		0,
		QsciScintilla::NumberMargin
	);

	m_editor->setMarginLineNumbers(
		0,
		true
	);

	m_editor->setMarginWidth(
		0,
		40
	);

	m_editor->setCaretWidth(
		2
	);

	m_tabs->addTab(
		m_editor,
		QStringLiteral("QScintilla")
	);

	connect(
		m_editor,
		&QsciScintilla::textChanged,
		this,
		[this]() {
			emit modificationChanged(
				m_editor->isModified()
			);
		}
	);
} // End constructor


QWidget *
QScintillaBackend::widget()
{
	return m_tabs;
}


bool
QScintillaBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	const QFileInfo fileInfo(
		filePath
	);

	if (!fileInfo.exists() ||
		!fileInfo.isFile()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The file does not exist:\n%1"
				).arg(
					filePath
				);
		}

		return false;
	}

	QFile file(
		fileInfo.absoluteFilePath()
	);

	if (!file.open(
			QIODevice::ReadOnly
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	const QByteArray fileData =
		file.readAll();

	file.close();

	const QString fileText =
		QString::fromUtf8(
			fileData
		);

	/*
	 * Prevent the intermediate setText() operation
	 * from producing a spurious application-level
	 * modification notification.
	 */
	const QSignalBlocker signalBlocker(
		m_editor
	);

	m_editor->setText(
		fileText
	);

	m_editor->setModified(
		false
	);

	m_filePath =
		fileInfo.absoluteFilePath();

	emit currentFileChanged(
		m_filePath
	);

	emit modificationChanged(
		false
	);

	return true;
} // End openFile


bool
QScintillaBackend::saveCurrentFile(
	QString *errorMessage
)
{
	if (m_filePath.isEmpty()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No file is currently open."
				);
		}

		return false;
	}

	QSaveFile file(
		m_filePath
	);

	if (!file.open(
			QIODevice::WriteOnly
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	const QByteArray fileData =
		m_editor->text().toUtf8();

	const qint64 bytesWritten =
		file.write(
			fileData
		);

	if (bytesWritten != fileData.size()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	if (!file.commit()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	m_editor->setModified(
		false
	);

	emit modificationChanged(
		false
	);

	return true;
} // End saveCurrentFile


bool
QScintillaBackend::hasModifiedFiles() const
{
	return m_editor->isModified();
}


bool
QScintillaBackend::saveAllFiles(
	QString *errorMessage
)
{
	if (!m_editor->isModified()) {
		return true;
	}

	return saveCurrentFile(
		errorMessage
	);
}


bool
QScintillaBackend::discardAllChanges(
	QString *errorMessage
)
{
		Q_UNUSED(errorMessage);

	m_editor->setModified(
		false
	);

	emit modificationChanged(
		false
	);

	return true;
}


bool
QScintillaBackend::closeCurrentFile(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


bool
QScintillaBackend::closeAllFiles(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


QString
QScintillaBackend::currentFilePath() const
{
	return m_filePath;
}


bool
QScintillaBackend::isModified() const
{
	return m_editor->isModified();
}


void
QScintillaBackend::configureEditorAppearance(
		QsciLexerCPP *lexer
)
{
	if (lexer == nullptr ||
		m_editor == nullptr) {
		return;
	}

	// Default font size
	QFont editorFont =
		m_editor->font();

	editorFont.setPointSize(
		editorFont.pointSize() + 3
	);

	// Dark theme colours
	const QColor editorBackground(
			QStringLiteral("#121212")
	);

	const QColor editorForeground(
			QStringLiteral("#b2b2b2")
	);

	const QColor marginBackground(
			QStringLiteral("#31363b")
	);

	const QColor marginForeground(
			QStringLiteral("#767676")
	);

	const QColor keywordColor(
			QStringLiteral("#5bcdf3")
	);

	const QColor stringColor(
			QStringLiteral("#f5d676")
	);

	const QColor commentColor(
			QStringLiteral("#505050")
	);

	const QColor numberColor(
			QStringLiteral("#9775da")
	);

	const QColor preprocessorColor(
			QStringLiteral("#c22323")
	);

	const QColor operatorColor(
			QStringLiteral("#f8f8f8")
	);
	
	// Set font assignments
	m_editor->setMarginsFont(
		editorFont
	);

	lexer->setDefaultFont(
		editorFont
	);

	lexer->setDefaultPaper(
		editorBackground
	);

	const int cppStyles[] = {
		QsciLexerCPP::Default,
		QsciLexerCPP::Comment,
		QsciLexerCPP::CommentLine,
		QsciLexerCPP::CommentDoc,
		QsciLexerCPP::Number,
		QsciLexerCPP::Keyword,
		QsciLexerCPP::DoubleQuotedString,
		QsciLexerCPP::SingleQuotedString,
		QsciLexerCPP::Operator,
		QsciLexerCPP::Identifier,
		QsciLexerCPP::PreProcessor
	};

	for (const int style : cppStyles) {
		lexer->setFont(
			editorFont,
			style
		);

		lexer->setPaper(
			editorBackground,
			style
		);
	}

	// Set colour assignments
	m_editor->setMarginsBackgroundColor(
		marginBackground
	);

	m_editor->setMarginsForegroundColor(
		marginForeground
	);

	m_editor->setCaretForegroundColor(
		editorForeground
	);

	m_editor->setCaretWidth(
		4
	);

	lexer->setDefaultColor(
		editorForeground
	);

	lexer->setColor(
		commentColor,
		QsciLexerCPP::Comment
	);

	lexer->setColor(
		commentColor,
		QsciLexerCPP::CommentLine
	);

	lexer->setColor(
		commentColor,
		QsciLexerCPP::CommentDoc
	);

	lexer->setColor(
		numberColor,
		QsciLexerCPP::Number
	);

	lexer->setColor(
		keywordColor,
		QsciLexerCPP::Keyword
	);

	lexer->setColor(
		stringColor,
		QsciLexerCPP::DoubleQuotedString
	);

	lexer->setColor(
		stringColor,
		QsciLexerCPP::SingleQuotedString
	);

	lexer->setColor(
		operatorColor,
		QsciLexerCPP::Operator
	);

	lexer->setColor(
		editorForeground,
		QsciLexerCPP::Identifier
	);

	lexer->setColor(
		preprocessorColor,
		QsciLexerCPP::PreProcessor
	);
} // End confitureEditorAppearance

