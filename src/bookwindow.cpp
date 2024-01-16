// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "bookwindow.h"
#include "bookdelegate.h"
#include "initdb.h"

#include <QtSql>

BookWindow::BookWindow()
{
    download_and_simplify_ExcelFile();
    ui.setupUi(this);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                    this,
                    "Unable to load database",
                    "This demo needs the SQLITE driver"
                    );

    // Initialize the database:
    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    // Create the data model:
    model = new QSqlRelationalTableModel(ui.bookTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("books");

    // Remember the indexes of the columns:
    authorIdx = model->fieldIndex("author");
    genreIdx = model->fieldIndex("genre");

    // Set the relations to the other database tables:
    model->setRelation(authorIdx, QSqlRelation("authors", "id", "name"));
    model->setRelation(genreIdx, QSqlRelation("genres", "id", "name"));

    // Set the localized header captions:
    model->setHeaderData(authorIdx, Qt::Horizontal, tr("Author Name"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(model->fieldIndex("title"),
                         Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));
    model->setHeaderData(model->fieldIndex("rating"),
                         Qt::Horizontal, tr("Rating"));

    // Populate the model:
    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    // Set the model and hide the ID column:
    ui.bookTable->setModel(model);
    ui.bookTable->setItemDelegate(new BookDelegate(ui.bookTable));
    ui.bookTable->setColumnHidden(model->fieldIndex("id"), true);
    ui.bookTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Initialize the Author combo box:
    ui.authorEdit->setModel(model->relationModel(authorIdx));
    ui.authorEdit->setModelColumn(
                model->relationModel(authorIdx)->fieldIndex("name"));

    ui.genreEdit->setModel(model->relationModel(genreIdx));
    ui.genreEdit->setModelColumn(
                model->relationModel(genreIdx)->fieldIndex("name"));

    // Lock and prohibit resizing of the width of the rating column:
    ui.bookTable->horizontalHeader()->setSectionResizeMode(
                model->fieldIndex("rating"),
                QHeaderView::ResizeToContents);

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setItemDelegate(new BookDelegate(this));
    mapper->addMapping(ui.titleEdit, model->fieldIndex("title"));
    mapper->addMapping(ui.yearEdit, model->fieldIndex("year"));
    mapper->addMapping(ui.authorEdit, authorIdx);
    mapper->addMapping(ui.genreEdit, genreIdx);
    mapper->addMapping(ui.ratingEdit, model->fieldIndex("rating"));

    connect(ui.bookTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
            );

    ui.bookTable->setCurrentIndex(model->index(0, 0));
    ui.bookTable->selectRow(0);
    createMenuBar();
}

void BookWindow::download_and_simplify_ExcelFile()
{
    const wchar_t* url = L"https://www.nti.org/wp-content/uploads/2021/10/north_korea_missile_test_database.xlsx";
    const wchar_t* pathToExcelFile = getDestination();

    URLDownloadToFile(NULL, url, pathToExcelFile, 0, NULL);
    storeExcelFileData(wideToNarrow(pathToExcelFile));
}

wchar_t* BookWindow::getDestination() {
    // Get the size of the buffer needed for the current working directory
    DWORD buffer_size = GetCurrentDirectory(0, nullptr);

    // Allocate a buffer to store the current working directory
    wchar_t* destination = new wchar_t[buffer_size];

    // Get the current working directory and store it in the buffer
    GetCurrentDirectory(buffer_size, destination);

    // Go back 1 folder
    PathRemoveFileSpec(destination);

    // Append "\\temp\\excel.xlsx"
    PathAppend(destination, L"temp");
    PathAppend(destination, L"excel.xlsx");

    return destination;
}

char* BookWindow::wideToNarrow(const wchar_t* wideStr) {
    // Calculate the required size for the narrow string
    size_t size = wcstombs(nullptr, wideStr, 0);

    if (size == static_cast<size_t>(-1)) {
        // Conversion failed, handle the error as needed
        return nullptr;
    }

    // Allocate memory for the narrow string
    char* narrowStr = new char[size + 1];

    // Convert wide string to narrow string
    size_t result = wcstombs(narrowStr, wideStr, size);

    if (result == static_cast<size_t>(-1)) {
        // Conversion failed, handle the error as needed
        delete[] narrowStr;
        return nullptr;
    }

    // Null-terminate the narrow string
    narrowStr[size] = '\0';

    return narrowStr;
}

void BookWindow::storeExcelFileData(const char* pathToExcelFile)
{
    /*
    // Open the .xlsx file
    xlsxioreader xlsxioread;
    if ((xlsxioread = xlsxioread_open(pathToExcelFile)) == NULL) {
        throw std::runtime_error("Error opening .xlsx file");
    }
    // Define vectors to store data
    std::vector<std::string> date;
    std::vector<std::string> timeInUtc;
    std::vector<std::string> startingLocation_city;
    std::vector<std::string> startingLocation_latitude;
    std::vector<std::string> startingLocation_longitude;
    std::vector<std::string> landingLocation;
    std::vector<std::string> distanceTraveled;

    // Open the first sheet
    xlsxioreadersheet sheet;
    if ((sheet = xlsxioread_sheet_open(xlsxioread, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS)) != NULL) {
        char* cell_value;
        size_t row_count = 0;

        // Read each row
        while (xlsxioread_sheet_next_row(sheet)) {
            row_count++;

            // Only process rows 148 to 260
            if (row_count < 148) continue;
            if (row_count > 260) break;

            // Read each cell in the row
            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                date.push_back(cell_value);
                free(cell_value);
            }
            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                timeInUtc.push_back(cell_value);
                free(cell_value);
            }
            // Skip columns E to H
            for (int i = 0; i < 4; i++) xlsxioread_sheet_next_cell(sheet);

            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                startingLocation_city.push_back(cell_value);
                free(cell_value);
            }
            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                startingLocation_latitude.push_back(cell_value);
                free(cell_value);
            }
            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                startingLocation_longitude.push_back(cell_value);
                free(cell_value);
            }
            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                landingLocation.push_back(cell_value);
                free(cell_value);
            }
            // Skip column N
            xlsxioread_sheet_next_cell(sheet);

            if ((cell_value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
                distanceTraveled.push_back(cell_value);
                free(cell_value);
            }
        }
        // Close the sheet
        xlsxioread_sheet_close(sheet);
    }
    // Close the .xlsx reader
    xlsxioread_close(xlsxioread);
    */
}

void BookWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                "Error initializing database: " + err.text());
}

void BookWindow::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quit"), this);
    QAction *aboutAction = new QAction(tr("&About"), this);
    QAction *aboutQtAction = new QAction(tr("&About Qt"), this);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(aboutAction, &QAction::triggered, this, &BookWindow::about);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void BookWindow::about()
{
    QMessageBox::about(this, tr("About Books"),
            tr("<p>The <b>Books</b> example shows how to use Qt SQL classes "
               "with a model/view framework."));
}
