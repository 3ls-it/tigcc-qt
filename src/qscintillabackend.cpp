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

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QIODevice>
#include <QLabel>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QTabBar>
#include <QTabWidget>

#include "qscintillabackend.h"



QScintillaBackend::QScintillaBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_tabs(new QTabWidget(parent)),
	  m_emptyState(nullptr),
	  m_fontPointSize(
		  QApplication::font().pointSize()
	  )
{
	if (m_fontPointSize <= 0) {
		m_fontPointSize = 12;
	}

	m_tabs->setDocumentMode(
		true
	);

	m_tabs->setTabsClosable(
		false
	);

	m_tabs->setMovable(
		true
	);

	m_emptyState =
		createEmptyStateWidget();

	m_tabs->addTab(
		m_emptyState,
		QStringLiteral("Welcome")
	);

	connect(
		m_tabs,
		&QTabWidget::currentChanged,
		this,
		[this](int index) {
			Q_UNUSED(index);

			emitCurrentDocumentState();
		}
	);

	connect(
		m_tabs,
		&QTabWidget::tabCloseRequested,
		this,
		[this](int index) {
			if (index < 0 ||
				index >= m_tabs->count()) {
				return;
			}

			m_tabs->setCurrentIndex(
				index
			);

			QString errorMessage;

			closeCurrentFile(
				&errorMessage
			);
		}
	);
} // End constructor


QWidget *
QScintillaBackend::widget()
{
	return m_tabs;
}


QScintillaBackend::DocumentEntry *
QScintillaBackend::currentDocumentEntry() const
{
	QWidget *currentWidget =
		m_tabs->currentWidget();

	if (currentWidget == nullptr) {
		return nullptr;
	}

	for (DocumentEntry *entry : m_documents) {
		if (entry->widget == currentWidget) {
			return entry;
		}
	}

	return nullptr;
}


QScintillaBackend::DocumentEntry *
QScintillaBackend::documentEntryForPath(
	const QString &filePath
) const
{
	const QString normalizedPath =
		QFileInfo(
			filePath
		).absoluteFilePath();

	for (DocumentEntry *entry : m_documents) {
		if (entry->filePath == normalizedPath) {
			return entry;
		}
	}

	return nullptr;
}


QLabel *
QScintillaBackend::createEmptyStateWidget()
{
	auto *emptyState =
		new QLabel(
			QStringLiteral(
				"No file is open.\n\n"
				"Create or open a project, then "
				"double-click a file in the "
				"project tree."
			),
			m_tabs
		);

	emptyState->setAlignment(
		Qt::AlignCenter
	);

	emptyState->setMinimumHeight(
		160
	);

	return emptyState;
}


void
QScintillaBackend::hideEmptyState()
{
	if (m_emptyState == nullptr) {
		return;
	}

	const int tabIndex =
		m_tabs->indexOf(
			m_emptyState
		);

	if (tabIndex >= 0) {
		m_tabs->removeTab(
			tabIndex
		);
	}

	m_emptyState->deleteLater();
	m_emptyState = nullptr;
}



void
QScintillaBackend::showEmptyState()
{
	if (m_emptyState != nullptr) {
		return;
	}

	m_emptyState =
		createEmptyStateWidget();

	m_tabs->addTab(
		m_emptyState,
		QStringLiteral("Welcome")
	);

	m_tabs->setCurrentWidget(
		m_emptyState
	);

	m_tabs->setTabsClosable(
		false
	);
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

	const QString normalizedPath =
		fileInfo.absoluteFilePath();

	DocumentEntry *existingEntry =
		documentEntryForPath(
			normalizedPath
		);

	if (existingEntry != nullptr) {
		m_tabs->setCurrentIndex(
			existingEntry->tabIndex
		);

		emitCurrentDocumentState();

		return true;
	}

	QFile file(
		normalizedPath
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

	auto *editor =
		new QsciScintilla(
			m_tabs
		);

	editor->setUtf8(
		true
	);

	auto *lexer =
		new QsciLexerCPP(
			editor
		);

	editor->setLexer(
		lexer
	);

	configureEditorAppearance(
		lexer,
		editor
	);

	{
		const QSignalBlocker signalBlocker(
			editor
		);

		editor->setText(
			fileText
		);

		editor->setModified(
			false
		);
	}

	const QString tabLabel =
		fileInfo.fileName();

	if (m_documents.isEmpty()) {
		hideEmptyState();
	}

	const int tabIndex =
		m_tabs->addTab(
			editor,
			tabLabel
		);

	m_tabs->setTabsClosable(
		true
	);

	setTabCloseToolTip(
		tabIndex
	);

	auto *entry =
		new DocumentEntry{
			editor,
			lexer,
			editor,
			normalizedPath,
			tabIndex
		};

	m_documents.append(
		entry
	);

	connect(
		editor,
		&QsciScintilla::textChanged,
		this,
		[this, entry]() {
			updateTabTitle(
				entry
			);

			if (currentDocumentEntry() == entry) {
				emit modificationChanged(
					entry->editor->isModified()
				);
			}
		}
	);

	m_tabs->setCurrentIndex(
		tabIndex
	);

	emitCurrentDocumentState();

	return true;
} // End openFile


bool
QScintillaBackend::saveDocumentEntry(
	DocumentEntry *entry,
	QString *errorMessage
)
{
	if (entry == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No editor document is available."
				);
		}

		return false;
	}

	QSaveFile file(
		entry->filePath
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
		entry->editor->text().toUtf8();

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

	entry->editor->setModified(
		false
	);

	updateTabTitle(
		entry
	);

	return true;
} // End saveDocumentEntry


bool
QScintillaBackend::saveCurrentFile(
	QString *errorMessage
)
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No file is currently open."
				);
		}

		return false;
	}

	if (!saveDocumentEntry(
			entry,
			errorMessage
		)) {
		return false;
	}

	emitCurrentDocumentState();

	return true;
} // End saveCurrentFile


bool
QScintillaBackend::hasModifiedFiles() const
{
	for (DocumentEntry *entry : m_documents) {
		if (entry->editor->isModified()) {
			return true;
		}
	}

	return false;
}


bool
QScintillaBackend::saveAllFiles(
	QString *errorMessage
)
{
	for (DocumentEntry *entry : m_documents) {
		if (!entry->editor->isModified()) {
			continue;
		}

		if (!saveDocumentEntry(
				entry,
				errorMessage
			)) {
			return false;
		}
	}

	emitCurrentDocumentState();

	return true;
} // End saveAllFiles


bool
QScintillaBackend::discardAllChanges(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	for (DocumentEntry *entry : m_documents) {
		entry->editor->setModified(
			false
		);

		updateTabTitle(
			entry
		);
	}

	emitCurrentDocumentState();

	return true;
} // End discardAllChanges


bool
QScintillaBackend::closeCurrentFile(
	QString *errorMessage
)
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No editor document is open."
				);
		}

		return false;
	}

	if (entry->editor->isModified()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The current file has unsaved changes."
				);
		}

		return false;
	}

	const int tabIndex =
		m_tabs->indexOf(
			entry->widget
		);

	if (tabIndex < 0) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor tab could not be found."
				);
		}

		return false;
	}

	m_documents.removeOne(
		entry
	);

	m_tabs->removeTab(
		tabIndex
	);

	entry->widget->deleteLater();

	delete entry;

	for (DocumentEntry *remaining :
			m_documents) {
		remaining->tabIndex =
			m_tabs->indexOf(
				remaining->widget
			);
	}

	if (m_documents.isEmpty()) {
		showEmptyState();
	} else {
		m_tabs->setTabsClosable(
			true
		);
	}

	emitCurrentDocumentState();

	return true;
} // End closeCurrentFile


bool
QScintillaBackend::closeAllFiles(
	QString *errorMessage
)
{
	for (DocumentEntry *entry :
			m_documents) {
		if (entry->editor->isModified()) {
			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"One or more files have "
						"unsaved changes."
					);
			}

			return false;
		}
	}

	while (!m_documents.isEmpty()) {
		m_tabs->setCurrentIndex(
			0
		);

		if (!closeCurrentFile(
				errorMessage
			)) {
			return false;
		}
	}

	if (m_emptyState == nullptr) {
		showEmptyState();
	}

	emitCurrentDocumentState();

	return true;
} // End closeAllFiles
 

QString
QScintillaBackend::currentFilePath() const
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		return QString();
	}

	return entry->filePath;
}


bool
QScintillaBackend::isModified() const
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		return false;
	}

	return entry->editor->isModified();
}


void
QScintillaBackend::configureEditorAppearance(
		QsciLexerCPP *lexer,
		QsciScintilla *editor
)
{
	if (lexer == nullptr ||
		editor == nullptr) {
		return;
	}

	// Default font size
	QFont editorFont =
		editor->font();

	editorFont.setPointSize(
		m_fontPointSize
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
	editor->setMarginsFont(
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
	editor->setMarginsBackgroundColor(
		marginBackground
	);

	editor->setMarginsForegroundColor(
		marginForeground
	);

	editor->setCaretForegroundColor(
		editorForeground
	);

	editor->setCaretWidth(
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
} // End configureEditorAppearance



void
QScintillaBackend::updateTabTitle(
	DocumentEntry *entry
)
{
	if (entry == nullptr) {
		return;
	}

	const int tabIndex =
		m_tabs->indexOf(
			entry->widget
		);

	if (tabIndex < 0) {
		return;
	}

	QString title =
		QFileInfo(
			entry->filePath
		).fileName();

	if (entry->editor->isModified()) {
		title.append(
			QStringLiteral(" *")
		);
	}

	m_tabs->setTabText(
		tabIndex,
		title
	);

	entry->tabIndex =
		tabIndex;
} // End updateTabTiltle


void
QScintillaBackend::emitCurrentDocumentState()
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		emit currentFileChanged(
			QString()
		);

		emit modificationChanged(
			false
		);

		return;
	}

	emit currentFileChanged(
		entry->filePath
	);

	emit modificationChanged(
		entry->editor->isModified()
	);
}


int
QScintillaBackend::fontPointSize() const
{
	return m_fontPointSize;
}


bool
QScintillaBackend::setFontPointSize(
	int pointSize,
	QString *errorMessage
)
{
	if (pointSize < 6 ||
		pointSize > 36) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Font size must be between "
					"6 and 36 points."
				);
		}

		return false;
	}

	if (pointSize == m_fontPointSize) {
		return true;
	}

	m_fontPointSize =
		pointSize;

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

	for (DocumentEntry *entry : m_documents) {
		QFont editorFont =
			entry->editor->font();

		editorFont.setPointSize(
			m_fontPointSize
		);

		for (const int style : cppStyles) {
			entry->lexer->setFont(
				editorFont,
				style
			);
		}

		entry->editor->setMarginsFont(
			editorFont
		);
	}

	emit fontPointSizeChanged(
		m_fontPointSize
	);

	return true;
} // End setFontPointSize


void
QScintillaBackend::setTabCloseToolTip(
	int tabIndex
)
{
	if (tabIndex < 0 ||
		tabIndex >= m_tabs->count()) {
		return;
	}

	QTabBar *tabBar =
		m_tabs->tabBar();

	if (tabBar == nullptr) {
		return;
	}

	QWidget *closeButton =
		tabBar->tabButton(
			tabIndex,
			QTabBar::RightSide
		);

	if (closeButton == nullptr) {
		closeButton =
			tabBar->tabButton(
				tabIndex,
				QTabBar::LeftSide
			);
	}

	if (closeButton != nullptr) {
		closeButton->setToolTip(
			QStringLiteral("Close")
		);
	}
}
