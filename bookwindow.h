// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QtWidgets>
#include <QtSql>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cwchar>
#include <iomanip>
#include <iostream>
#include <shlwapi.h>
#include <sstream>
#include <string>
#include <urlmon.h>
#include <vector>
#include <windows.h>
#include <xlsxio_read.h>
#include <QStyledItemDelegate>

#include "row.h"
#include "ui_bookwindow.h"

#pragma comment(lib, "urlmon.lib")

using namespace std;

class MyCustomDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem centeredOption(option);
        centeredOption.displayAlignment = Qt::AlignCenter;
        QStyledItemDelegate::paint(painter, centeredOption, index);
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        // Disable editing by returning nullptr
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return nullptr;
    }
};

class BookWindow: public QMainWindow
{
    Q_OBJECT
public:
    BookWindow();

private slots:
    void about();

private:
    vector<Row> entireTable;

    void downloadExcelFile_and_storeItsData();
    wchar_t* pathToTempFolder();
    char* wideToNarrow(const wchar_t* wideStr);
    void storeExcelFileData(const char* destination);
    bool isLeapYear(const int year);
    int daysInMonth(const int year, const int month);
    string convertDate(const char* excelDate);
    string convertTime(const char* excelTime);
    void createTable_and_displayIt();

    QSqlRelationalTableModel *model = nullptr;
    int authorIdx = 0, genreIdx = 0;
};

#endif
