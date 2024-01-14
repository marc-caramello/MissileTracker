// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QtWidgets>
#include <QtSql>
#include <cstdlib>
#include <cwchar>
#include <shlwapi.h>
#include <urlmon.h>
#include <windows.h>

#include <xlsxio_read.h>
#include <string>
#include <vector>

#include "ui_bookwindow.h"

#pragma comment(lib, "urlmon.lib")

class BookWindow: public QMainWindow
{
    Q_OBJECT
public:
    BookWindow();

private slots:
    void about();

private:
    void download_and_simplify_ExcelFile();
    wchar_t* getDestination();
    char* wideToNarrow(const wchar_t* wideStr);
    void storeExcelFileData(const char* destination);

    void showError(const QSqlError &err);
    Ui::BookWindow ui;
    QSqlRelationalTableModel *model = nullptr;
    int authorIdx = 0, genreIdx = 0;

    void createMenuBar();
};

#endif
