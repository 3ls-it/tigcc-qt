/*
 * @file    src/completiondatabackend.cpp
 * @brief   Implements the canonical completion-data backend.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <algorithm>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "completiondatabackend.h"



namespace
{

QString
normalizedCompletionName(
	const QString &name
)
{
	return name.trimmed().toCaseFolded();
}

}


CompletionDataBackend::CompletionDataBackend()
	: m_entries(),
	  m_loaded(false)
{
}


bool
CompletionDataBackend::load(
	const QString &filePath,
	QString *errorMessage
)
{
	QFile file(
		filePath
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

	const QByteArray data =
		file.readAll();

	file.close();

	QJsonParseError parseError;

	const QJsonDocument document =
		QJsonDocument::fromJson(
			data,
			&parseError
		);

	if (parseError.error !=
			QJsonParseError::NoError) {
		if (errorMessage != nullptr) {
			*errorMessage =
				parseError.errorString();
		}

		return false;
	}

	if (!document.isObject()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Completion data root must "
					"be a JSON object."
				);
		}

		return false;
	}

	const QJsonObject root =
		document.object();

	const int formatVersion =
		root.value(
			QStringLiteral("formatVersion")
		).toInt(
			-1
		);

	if (formatVersion != 1) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Unsupported completion-data "
					"format version: %1"
				).arg(
					formatVersion
				);
		}

		return false;
	}

	const QJsonValue entriesValue =
		root.value(
			QStringLiteral("entries")
		);

	if (!entriesValue.isArray()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Completion data must contain "
					"an entries array."
				);
		}

		return false;
	}

	const QJsonArray entriesArray =
		entriesValue.toArray();

	QList<CompletionEntry> loadedEntries;

	for (const QJsonValue &entryValue :
			entriesArray) {
		if (!entryValue.isObject()) {
			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"Each completion entry "
						"must be an object."
					);
			}

			return false;
		}

		const QJsonObject entryObject =
			entryValue.toObject();

		CompletionEntry entry;

		entry.name =
			entryObject.value(
				QStringLiteral("name")
			).toString();

		entry.kind =
			entryObject.value(
				QStringLiteral("kind")
			).toString();

		entry.signature =
			entryObject.value(
				QStringLiteral("signature")
			).toString();

		entry.returnType =
			entryObject.value(
				QStringLiteral("returnType")
			).toString();

		entry.description =
			entryObject.value(
				QStringLiteral("description")
			).toString();

		entry.header =
			entryObject.value(
				QStringLiteral("header")
			).toString();

		if (entry.name.isEmpty() ||
			entry.kind.isEmpty()) {
			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"Each completion entry must "
						"have a name and kind."
					);
			}

			return false;
		}

		const QJsonValue parametersValue =
			entryObject.value(
				QStringLiteral("parameters")
			);

		if (!parametersValue.isArray()) {
			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"Completion entry '%1' must "
						"contain a parameters array."
					).arg(
						entry.name
					);
			}

			return false;
		}

		const QJsonArray parametersArray =
			parametersValue.toArray();

		for (const QJsonValue &parameterValue :
				parametersArray) {
			if (!parameterValue.isObject()) {
				if (errorMessage != nullptr) {
					*errorMessage =
						QStringLiteral(
							"Parameters for '%1' "
							"must be objects."
						).arg(
							entry.name
						);
				}

				return false;
			}

			const QJsonObject parameterObject =
				parameterValue.toObject();

			CompletionParameter parameter;

			parameter.name =
				parameterObject.value(
					QStringLiteral("name")
				).toString();

			parameter.type =
				parameterObject.value(
					QStringLiteral("type")
				).toString();

			entry.parameters.append(
				parameter
			);
		}

		loadedEntries.append(
			entry
		);
	}

	std::sort(
		loadedEntries.begin(),
		loadedEntries.end(),
		[](const CompletionEntry &left,
			const CompletionEntry &right) {
			return normalizedCompletionName(
				left.name
			) < normalizedCompletionName(
				right.name
			);
		}
	);

	m_entries =
		loadedEntries;

	m_normalizedNames.clear();

	m_normalizedNames.reserve(
		m_entries.size()
	);

	for (const CompletionEntry &entry :
			m_entries) {
		m_normalizedNames.append(
			normalizedCompletionName(
				entry.name
			)
		);
	}

	m_loaded =
		true;

	return true;
} // End load


bool
CompletionDataBackend::isLoaded() const
{
	return m_loaded;
}


const QList<CompletionEntry> &
CompletionDataBackend::entries() const
{
	return m_entries;
}


QList<CompletionEntry>
CompletionDataBackend::findCompletions(
	const QString &prefix
) const
{
	QList<CompletionEntry> results;

	if (!m_loaded) {
		return results;
	}

	const QString normalizedPrefix =
		normalizedCompletionName(
			prefix
		);

	if (normalizedPrefix.isEmpty()) {
		return results;
	}

	const auto first =
		std::lower_bound(
			m_normalizedNames.cbegin(),
			m_normalizedNames.cend(),
			normalizedPrefix
		);

	const qsizetype firstIndex =
		std::distance(
			m_normalizedNames.cbegin(),
			first
		);

	for (qsizetype index = firstIndex;
			index < m_entries.size();
			++index) {
		if (!m_normalizedNames.at(
				index
			).startsWith(
				normalizedPrefix
			)) {
			break;
		}

		results.append(
			m_entries.at(
				index
			)
		);
	}

	return results;
} // End findCompletions
