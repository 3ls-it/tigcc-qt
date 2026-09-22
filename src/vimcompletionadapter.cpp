/*
 * @file    src/vimcompletionadapter.cpp
 * @brief   Generates Vim completion data for one Vim session.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QFile>
#include <QIODevice>
#include <QStringList>

#include "completiondatabackend.h"
#include "completionentry.h"
#include "vimcompletionadapter.h"



namespace
{

QString
escapeVimString(
	const QString &value
)
{
	QString escaped =
		value;

	/*
	 * The generated script uses single-quoted Vim strings.
	 * Vim represents a literal single quote by doubling it.
	 */
	escaped.replace(
		QStringLiteral("'"),
		QStringLiteral("''")
	);

	/*
	 * Keep generated Vimscript physically one-line per
	 * dictionary entry. This is primarily for robustness
	 * with descriptions containing newlines.
	 */
	escaped.replace(
		QStringLiteral("\r"),
		QStringLiteral("\\r")
	);

	escaped.replace(
		QStringLiteral("\n"),
		QStringLiteral("\\n")
	);

	return escaped;
}


QChar
completionKindCharacter(
	const QString &kind
)
{
	const QString normalizedKind =
		kind.trimmed().toLower();

	if (normalizedKind == QStringLiteral("function")) {
		return QChar('f');
	}

	if (normalizedKind == QStringLiteral("variable")) {
		return QChar('v');
	}

	if (normalizedKind == QStringLiteral("type")) {
		return QChar('t');
	}

	if (normalizedKind == QStringLiteral("enum")) {
		return QChar('e');
	}

	if (normalizedKind == QStringLiteral("macro")) {
		return QChar('d');
	}

	if (normalizedKind == QStringLiteral("constant")) {
		return QChar('c');
	}

	if (normalizedKind == QStringLiteral("keyword")) {
		return QChar('k');
	}

	return QChar('i');
}


QString
menuText(
	const CompletionEntry &entry
)
{
	QStringList parts;

	if (!entry.kind.isEmpty()) {
		parts.append(
			QStringLiteral("[") +
			entry.kind +
			QStringLiteral("]")
		);
	}

	if (!entry.header.isEmpty()) {
		parts.append(
			entry.header
		);
	}

	return parts.join(
		QStringLiteral(" ")
	);
}


QString
infoText(
	const CompletionEntry &entry
)
{
	QStringList parts;

	if (!entry.signature.isEmpty()) {
		parts.append(
			entry.signature
		);
	}

	if (!entry.returnType.isEmpty()) {
		parts.append(
			QStringLiteral(
				"returns %1"
			).arg(
				entry.returnType
			)
		);
	}

	if (!entry.description.isEmpty()) {
		parts.append(
			entry.description
		);
	}

	return parts.join(
		QStringLiteral("\n")
	);
}

QChar
completionKind(
	const QString &kind
)
{
	const QString normalized =
		kind.trimmed().toLower();

	if (normalized ==
		QStringLiteral("function")) {
		return QChar('f');
	}

	if (normalized ==
		QStringLiteral("variable")) {
		return QChar('v');
	}

	if (normalized ==
		QStringLiteral("type")) {
		return QChar('t');
	}

	if (normalized ==
		QStringLiteral("enum")) {
		return QChar('e');
	}

	if (normalized ==
		QStringLiteral("macro")) {
		return QChar('d');
	}

	if (normalized ==
		QStringLiteral("constant")) {
		return QChar('c');
	}

	if (normalized ==
		QStringLiteral("keyword")) {
		return QChar('k');
	}

	return QChar('i');
}

QString
completionMenu(
	const CompletionEntry &entry
)
{
	QStringList fields;

	if (!entry.kind.isEmpty()) {
		fields.append(
			QStringLiteral("[%1]")
				.arg(
					entry.kind
				)
		);
	}

	if (!entry.header.isEmpty()) {
		fields.append(
			entry.header
		);
	}

	return fields.join(
		QStringLiteral(" ")
	);
}

QString
completionInfo(
	const CompletionEntry &entry
)
{
	QStringList fields;

	if (!entry.signature.isEmpty()) {
		fields.append(
			entry.signature
		);
	}

	if (!entry.returnType.isEmpty()) {
		fields.append(
			QStringLiteral(
				"returns %1"
			).arg(
				entry.returnType
			)
		);
	}

	if (!entry.description.isEmpty()) {
		fields.append(
			entry.description
		);
	}

	return fields.join(
		QStringLiteral("\n")
	);
}

}



bool
VimCompletionAdapter::writeCompletionScript(
	const CompletionDataBackend *completionData,
	const QString &scriptPath,
	QString *errorMessage
)
{
	if (completionData == nullptr ||
		!completionData->isLoaded()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Completion data is not loaded."
				);
		}

		return false;
	}

	if (scriptPath.isEmpty()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No Vim completion-script path "
					"was provided."
				);
		}

		return false;
	}

	QFile file(
		scriptPath
	);

	if (!file.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate |
			QIODevice::Text
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	auto writeLine =
		[&file](const QString &line) {
			file.write(
				line.toUtf8()
			);

			file.write(
				"\n"
			);
		};

	//
	writeLine(
		QStringLiteral(
			"let g:tigcc_qt_completion_items = []"
		)
	);

	for (const CompletionEntry &entry :
			completionData->entries()) {
		const QString word =
			escapeVimString(
				entry.name
			);

		const QString menu =
			escapeVimString(
				completionMenu(
					entry
				)
			);

		const QString info =
			escapeVimString(
				completionInfo(
					entry
				)
			);

		const QString kind =
			escapeVimString(
				QString(
					completionKind(
						entry.kind
					)
				)
			);

		writeLine(
			QStringLiteral(
				"call add("
				"g:tigcc_qt_completion_items, "
				"{'word': '%1', "
				"'abbr': '%1', "
				"'menu': '%2', "
				"'kind': '%3', "
				"'info': '%4'})"
			).arg(
				word,
				menu,
				kind,
				info
			)
		);
	}
	//

	writeLine(
		QStringLiteral(
			"function! TigccQtComplete(findstart, base) abort"
		)
	);

	writeLine(
		QStringLiteral(
			"  if a:findstart"
		)
	);

	writeLine(
		QStringLiteral(
			"    let line = getline('.')"
		)
	);

	writeLine(
		QStringLiteral(
			"    let start = col('.') - 1"
		)
	);

	writeLine(
		QStringLiteral(
			"    while start > 0 && "
			"line[start - 1] =~# '\\a'"
		)
	);

	writeLine(
		QStringLiteral(
			"      let start -= 1"
		)
	);

	writeLine(
		QStringLiteral(
			"    endwhile"
		)
	);

	writeLine(
		QStringLiteral(
			"    return start"
		)
	);

	writeLine(
		QStringLiteral(
			"  endif"
		)
	);

	writeLine(
		QStringLiteral(
			"  let matches = []"
		)
	);

	writeLine(
		QStringLiteral(
			"  for item in g:tigcc_qt_completion_items"
		)
	);

	writeLine(
		QStringLiteral(
			"    if item.word =~? '^' . "
			"escape(a:base, '\\')"
		)
	);

	writeLine(
		QStringLiteral(
			"      call add(matches, item)"
		)
	);

	writeLine(
		QStringLiteral(
			"    endif"
		)
	);

	writeLine(
		QStringLiteral(
			"  endfor"
		)
	);

	writeLine(
		QStringLiteral(
			"  return matches"
		)
	);

	writeLine(
		QStringLiteral(
			"endfunction"
		)
	);

	writeLine(
		QStringLiteral(
			"setlocal completefunc=TigccQtComplete"
		)
	);

	writeLine(
		QStringLiteral(
			"set completeopt+=menuone"
		)
	);

	if (file.error() !=
		QFile::NoError) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		file.close();

		return false;
	}

	file.close();

	return true;
} // End writeCompletionScript
