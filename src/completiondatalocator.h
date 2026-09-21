/*
 * @file    src/completiondatalocatore.h
 * @brief   
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_COMPLETIONDATALOCATOR_H
#define TIGCC_QT_COMPLETIONDATALOCATOR_H

#include <QString>



class CompletionDataLocator
{
public:
	static QString
	locate(
		QString *errorMessage = nullptr
	);
};

#endif // TIGCC_QT_COMPLETIONDATALOCATOR_H
