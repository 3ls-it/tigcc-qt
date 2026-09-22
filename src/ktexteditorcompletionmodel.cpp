/*
 * @file    src/ktexteditorcompletionmodel.cpp
 * @brief   Implements the KTextEditor completion adapter.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KTextEditor/View>

#include "ktexteditorcompletionmodel.h"
#include "completiondatabackend.h"



KTextEditorCompletionModel::KTextEditorCompletionModel(
	const CompletionDataBackend *completionData,
	QObject *parent
)
	: KTextEditor::CodeCompletionModel(parent),
	  m_completionData(completionData),
	  m_matches()
{
}


void
KTextEditorCompletionModel::completionInvoked(
	KTextEditor::View *view,
	const KTextEditor::Range &range,
	InvocationType invocationType
)
{
	(void)invocationType;

	m_matches.clear();

	if (view == nullptr ||
		m_completionData == nullptr ||
		!m_completionData->isLoaded()) {
		setRowCount(
			0
		);

		return;
	}

	const QString prefix =
		view->document()->text(
			range
		);

	if (prefix.isEmpty()) {
		setRowCount(
			0
		);

		return;
	}

	m_matches =
		m_completionData->findCompletions(
			prefix
		);

	setRowCount(
		m_matches.size()
	);
} // End completionInvoked


QVariant
KTextEditorCompletionModel::data(
	const QModelIndex &index,
	int role
) const
{
	const QMap<int, QVariant> values =
		itemData(
			index
		);

	return values.value(
		role
	);
} // End data


QMap<int, QVariant>
KTextEditorCompletionModel::itemData(
	const QModelIndex &index
) const
{
	QMap<int, QVariant> result;

	if (!index.isValid() ||
		index.row() < 0 ||
		index.row() >= m_matches.size()) {
		return result;
	}

	const CompletionEntry &entry =
		m_matches.at(
			index.row()
		);

	switch (index.column()) {
		case KTextEditor::CodeCompletionModel::Prefix:
			result.insert(
				Qt::DisplayRole,
				entry.returnType
			);
			break;

		case KTextEditor::CodeCompletionModel::Name:
			result.insert(
				Qt::DisplayRole,
				entry.name
			);
			break;

		case KTextEditor::CodeCompletionModel::Arguments:
			result.insert(
				Qt::DisplayRole,
				entry.signature
			);
			break;

		case KTextEditor::CodeCompletionModel::Postfix:
			result.insert(
				Qt::DisplayRole,
				entry.header
			);
			break;

		default:
			break;
	}

	if (index.column() ==
		KTextEditor::CodeCompletionModel::Name &&
		!entry.description.isEmpty()) {
		result.insert(
			KTextEditor::CodeCompletionModel::ItemSelected,
			entry.description
		);
	}

	return result;
} // End itemData
