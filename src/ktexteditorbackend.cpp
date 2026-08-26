/*
 * @file    src/ktexteditorbackend.cpp
 * @brief   KTextEditor extends the EditorBackend
 *          interface to provide the editor component.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <QFileInfo>
#include <QStringList>
#include <QTabWidget>
#include <QUrl>
#include <QLabel>
#include <QMenu>

/* DEBUG
#include <QDebug>
#include <QScrollBar>
#include <QTimer>
#include <QAbstractScrollArea>
 */

#include "ktexteditorbackend.h"




KTextEditorBackend::KTextEditorBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_tabs(new QTabWidget(parent)),
	  m_emptyState(nullptr)
{
	m_tabs->setDocumentMode(true);
	m_tabs->setTabsClosable(true);
	m_tabs->setMovable(true);

	m_emptyState =
		createEmptyStateWidget();

	m_tabs->addTab(
		m_emptyState,
		QStringLiteral("Welcome")
	);

	m_emptyState->setAlignment(
		Qt::AlignCenter
	);

	m_emptyState->setMinimumHeight(
		160
	);

	connect(
		m_tabs,
		&QTabWidget::currentChanged,
		this,
		&KTextEditorBackend::handleCurrentTabChanged
	);

	connect(
		m_tabs,
		&QTabWidget::tabCloseRequested,
		this,
		&KTextEditorBackend::handleTabCloseRequested
	);
}


QWidget *
KTextEditorBackend::widget()
{
	return m_tabs;
}


KTextEditorBackend::DocumentEntry *
KTextEditorBackend::documentEntryForPath(
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


KTextEditorBackend::DocumentEntry *
KTextEditorBackend::currentDocumentEntry() const
{
	const int currentIndex =
		m_tabs->currentIndex();

	if (currentIndex < 0) {
		return nullptr;
	}

	for (DocumentEntry *entry : m_documents) {
		if (entry->tabIndex == currentIndex) {
			return entry;
		}
	}

	return nullptr;
}


bool
KTextEditorBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	const QString normalizedPath =
		QFileInfo(
			filePath
		).absoluteFilePath();

	DocumentEntry *existingEntry =
		documentEntryForPath(
			normalizedPath
		);

	if (existingEntry != nullptr) {
		m_tabs->setCurrentIndex(
			existingEntry->tabIndex
		);

		return true;
	}

	KTextEditor::Editor *editor =
		KTextEditor::Editor::instance();

	KTextEditor::Document *document =
		editor->createDocument(this);

	if (document == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The KTextEditor document could not be created."
				);
		}

		return false;
	}

	const QUrl fileUrl =
		QUrl::fromLocalFile(
			normalizedPath
		);

	if (!document->openUrl(fileUrl)) {
		document->deleteLater();

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor could not open the file."
				);
		}

		return false;
	}

	KTextEditor::View *view =
		document->createView(m_tabs);

	if (view == nullptr) {
		document->deleteLater();

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor view could not be created."
				);
		}

		return false;
	}

	configureView(
		view
	);

	//QWidget *editorWidget =
	//	view->editorWidget();
	QWidget *editorWidget =
		view->editorWidget();

	QWidget *viewWidget =
		editorWidget->parentWidget();

	if (editorWidget == nullptr ||
			viewWidget == nullptr) {
		document->deleteLater();

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor widget could not be obtained."
				);
		}

		return false;
	}

	const QString tabLabel =
		QFileInfo(
			normalizedPath
		).fileName();

	if (m_documents.isEmpty())
		hideEmptyState();

	const int tabIndex =
		m_tabs->addTab(
			viewWidget,
			tabLabel
		);

	auto *entry =
		new DocumentEntry{
			document,
			view,
			viewWidget,
			normalizedPath,
			tabIndex
		};

	m_documents.append(entry);

	connect(
		document,
		&KTextEditor::Document::modifiedChanged,
		this,
		&KTextEditorBackend::documentModifiedChanged
	);

	m_tabs->setCurrentIndex(
		tabIndex
	);

	emit currentFileChanged(
		normalizedPath
	);

	emit modificationChanged(
		document->isModified()
	);

	return true;
}// End openFile()


bool
KTextEditorBackend::saveCurrentFile(
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

	if (!entry->document->documentSave()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor could not save the file."
				);
		}

		return false;
	}

	emit modificationChanged(
		entry->document->isModified()
	);

	return true;
}


QString
KTextEditorBackend::currentFilePath() const
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		return QString();
	}

	return entry->filePath;
}


bool
KTextEditorBackend::isModified() const
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr) {
		return false;
	}

	return entry->document->isModified();
}


void
KTextEditorBackend::handleCurrentTabChanged(
    int index
)
{
	Q_UNUSED(index);

	emitCurrentDocumentState();
}


void
KTextEditorBackend::documentModifiedChanged(
    KTextEditor::Document *document
)
{
	DocumentEntry *entry =
		currentDocumentEntry();

	if (entry == nullptr ||
		entry->document != document) {
		return;
	}

	emitCurrentDocumentState();
}


void
KTextEditorBackend::emitCurrentDocumentState()
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
		entry->document->isModified()
	);
}


void
KTextEditorBackend::handleTabCloseRequested(
    int index
)
{
	if (m_emptyState != nullptr &&
		m_tabs->widget(index) == m_emptyState) {
		return;
	}

	if (index < 0 ||
		index >= m_tabs->count()) {
		return;
	}

	DocumentEntry *entry = nullptr;

	for (DocumentEntry *candidate : m_documents) {
		if (candidate->tabIndex == index) {
			entry = candidate;
			break;
		}
	}

	if (entry == nullptr) {
		return;
	}

	if (entry->document->isModified()) {
		emit editorError(
			QStringLiteral(
				"The file has unsaved changes."
			)
		);

		return;
	}

	closeCurrentFile();
}


bool
KTextEditorBackend::closeCurrentFile(
    QString *errorMessage
)
{
	const int index =
		m_tabs->currentIndex();

	if (index < 0) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No editor document is open."
				);
		}

		return false;
	}

	DocumentEntry *entry = nullptr;

	for (DocumentEntry *candidate : m_documents) {
		if (candidate->tabIndex == index) {
			entry = candidate;
			break;
		}
	}

	if (entry == nullptr) {
		return false;
	}

	if (entry->document->isModified()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The file has unsaved changes."
				);
		}

		return false;
	}

	m_documents.removeOne(entry);

	m_tabs->removeTab(index);

	entry->document->deleteLater();
	entry->widget->deleteLater();

	delete entry;

	if (m_documents.isEmpty())
		showEmptyState();

	for (int position = 0;
		 position < m_documents.size();
		 ++position) {
		m_documents.at(position)->tabIndex =
			m_tabs->indexOf(
				m_documents.at(position)->widget
			);
	}

	emitCurrentDocumentState();

	return true;
}


bool
KTextEditorBackend::closeAllFiles(
    QString *errorMessage
)
{
	for (DocumentEntry *entry : m_documents) {
		if (entry->document->isModified()) {
			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"One or more files have unsaved changes."
					);
			}

			return false;
		}
	}

	while (!m_documents.isEmpty()) {
		m_tabs->setCurrentIndex(0);

		if (!closeCurrentFile(errorMessage)) {
			return false;
		}
	}

	return true;
}


void
KTextEditorBackend::hideEmptyState()
{
	if (m_emptyState == nullptr) {
		return;
	}

	const int index =
		m_tabs->indexOf(
			m_emptyState
		);

	if (index >= 0) {
		m_tabs->removeTab(
			index
		);
	}

	m_emptyState->deleteLater();
	m_emptyState = nullptr;
}


void
KTextEditorBackend::showEmptyState()
{
	if (m_emptyState != nullptr)
		return;

	m_emptyState =
		createEmptyStateWidget();

	m_emptyState->setAlignment(
		Qt::AlignCenter
	);

	m_emptyState->setMinimumHeight(
		160
	);

	m_tabs->addTab(
		m_emptyState,
		QStringLiteral("Welcome")
	);

	m_tabs->setCurrentWidget(
		m_emptyState
	);
}


QLabel *
KTextEditorBackend::createEmptyStateWidget()
{
	auto *emptyState =
		new QLabel(
			QStringLiteral(
				"No file is open.\n\n"
				"Double-click a source, header, "
				"or GNU Assembly file in the "
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
KTextEditorBackend::configureView(
    KTextEditor::View *view
)
{
	if (view == nullptr) {
		return;
	}

	QMenu *contextMenu =
		view->defaultContextMenu();

	if (contextMenu != nullptr) {
		view->setContextMenu(
			contextMenu
		);
	}

	const QStringList configKeys =
		view->configKeys();

	const QString minimapKey =
		QStringLiteral(
			"scrollbar-minimap"
		);

	const QString previewKey =
		QStringLiteral(
			"scrollbar-preview"
		);

	if (configKeys.contains(
			minimapKey
		)) {
		view->setConfigValue(
			minimapKey,
			true
		);
	}

	if (configKeys.contains(
			previewKey
		)) {
		view->setConfigValue(
			previewKey,
			true
		);
	}
}

