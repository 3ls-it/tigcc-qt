/*
 * @file    src/ktexteditorcompletionmodel.h
 * @brief   KTextEditor completion adapter for TIGCC completion data.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_KTEXTEDITORCOMPLETIONMODEL_H
#define TIGCC_QT_KTEXTEDITORCOMPLETIONMODEL_H

#include <KTextEditor/CodeCompletionModel>

#include <QList>
#include <QMap>
#include <QVariant>

#include "completionentry.h"




class CompletionDataBackend;

namespace KTextEditor
{
	class View;
	class Range;
}

class KTextEditorCompletionModel :
	public KTextEditor::CodeCompletionModel
{
	Q_OBJECT

public:
	explicit KTextEditorCompletionModel(
		const CompletionDataBackend *completionData,
		QObject *parent = nullptr
	);

	void
	completionInvoked(
		KTextEditor::View *view,
		const KTextEditor::Range &range,
		InvocationType invocationType
	) override;

	QVariant
	data(
		const QModelIndex &index,
		int role = Qt::DisplayRole
	) const override;

	QMap<int, QVariant>
	itemData(
		const QModelIndex &index
	) const override;

private:
	const CompletionDataBackend *m_completionData;
	QList<CompletionEntry> m_matches;
};

#endif // TIGCC_QT_KTEXTEDITORCOMPLETIONMODEL_H
