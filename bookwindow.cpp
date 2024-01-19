// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "bookwindow.h"
#include "bookdelegate.h"
#include "initdb.h"
#include "row.h"

BookWindow::BookWindow()
{
    // What I added: start
    downloadExcelFile_and_storeItsData();
    createTable_and_displayIt();
    // What I added: end

    /*
    ui.setupUi(this);

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
    */
    createMenuBar();
}

void BookWindow::downloadExcelFile_and_storeItsData()
{
    const wchar_t* url = L"https://www.nti.org/wp-content/uploads/2021/10/north_korea_missile_test_database.xlsx";

    wstring pathToExcelFile = pathToTempFolder();
    pathToExcelFile += L"\\input.xlsx";

    URLDownloadToFile(NULL, url, pathToExcelFile.c_str(), 0, NULL);
    storeExcelFileData(wideToNarrow(pathToExcelFile.c_str()));
}

wchar_t* BookWindow::pathToTempFolder() {
    // Get the size of the buffer needed for the current working directory
    DWORD buffer_size = GetCurrentDirectory(0, nullptr);

    // Allocate a buffer to store the current working directory
    wchar_t* destination = new wchar_t[buffer_size];

    // Get the current working directory and store it in the buffer
    GetCurrentDirectory(buffer_size, destination);

    // Go back 1 folder
    PathRemoveFileSpec(destination);

    // Append "\\temp"
    PathAppend(destination, L"temp");

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
    xlsxioreader xlsxioread = xlsxioread_open(pathToExcelFile);
    xlsxioreadersheet sheet = xlsxioread_sheet_open(xlsxioread, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);

    char* cellVal;
    int current_rowNum = 0;

    while (xlsxioread_sheet_next_row(sheet)) {
        if (current_rowNum >= 148) {
            string date;
            string timeInUtc;
            string startingLocation_city;
            string startingLocation_latitude;
            string startingLocation_longitude;
            string landingLocation;
            string distanceTraveled_km;

            for (char current_columnLetter = 'A'; current_columnLetter <= 'O'; current_columnLetter++) {
                cellVal = xlsxioread_sheet_next_cell(sheet);

                if (current_columnLetter == 'B') {
                    date = convertDate(cellVal);
                }
                else if (current_columnLetter == 'D') {
                    timeInUtc = convertTime(cellVal);
                }
                else if (current_columnLetter == 'I') {
                    startingLocation_city = cellVal;
                    startingLocation_city.erase(remove(startingLocation_city.begin(), startingLocation_city.end(), '\''), startingLocation_city.end());
                }
                else if (current_columnLetter == 'K') {
                    startingLocation_latitude = cellVal;
                }
                else if (current_columnLetter == 'L') {
                    startingLocation_longitude = cellVal;
                }
                else if (current_columnLetter == 'M') {
                    landingLocation = cellVal;
                }
                else if (current_columnLetter == 'O') {
                    distanceTraveled_km = cellVal;
                }
            }
            if(date != "Unknown" &&
                timeInUtc != "Unknown" &&
                startingLocation_city != "Unknown" &&
                startingLocation_latitude != "Unknown" &&
                startingLocation_longitude != "Unknown" &&
                landingLocation != "Unknown" &&
                distanceTraveled_km != "Unknown") {

                Row row;
                row.dateAndTime_inUtc = date + " " + timeInUtc;
                row.startingLocation_city = startingLocation_city;
                row.startingLocation_coordinates = startingLocation_latitude.substr(0, 5) + ", " + startingLocation_longitude.substr(0, 6);
                row.landingLocation = landingLocation;
                row.distanceTraveled_km = distanceTraveled_km.erase(distanceTraveled_km.length() - 3);;
                entireTable.push_back(row);
            }
        }
        current_rowNum++;
    }
    xlsxioread_sheet_close(sheet);
    xlsxioread_close(xlsxioread);
}

bool BookWindow::isLeapYear(const int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int BookWindow::daysInMonth(const int year, const int month) {
    vector<int> monthLengths = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return monthLengths[month - 1];
}

string BookWindow::convertDate(const char* excelDate) {
    int days = stoi(excelDate);
    int year = 1900;
    int month = 1;

    while (days > 365) {
        if (isLeapYear(year) && days == 366) {
            break;
        }
        else if (isLeapYear(year)) {
            days -= 366;
        }
        else {
            days -= 365;
        }
        year++;
    }
    while (days > daysInMonth(year, month)) {
        days -= daysInMonth(year, month);
        month++;
    }
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, days);
    return string(buffer);
}

string BookWindow::convertTime(const char* excelTime) {
    double fractionalDay = atof(excelTime);
    int totalSeconds = static_cast<int>(fractionalDay * 86400);  // Total seconds in a day
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    ostringstream oss;
    oss << setfill('0') << setw(2) << hours << ":"
        << setfill('0') << setw(2) << minutes << ":"
        << setfill('0') << setw(2) << seconds;
    return oss.str();
}

void BookWindow::createTable_and_displayIt() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString outputFilePath = QString::fromWCharArray(pathToTempFolder()) + "\\output.db";
    db.setDatabaseName(outputFilePath);
    db.open();
    QSqlQuery query;

    query.exec(R"(
        CREATE TABLE MissileLaunches (
            dateAndTime_inUtc DATETIME,
            startingLocation_city VARCHAR,
            startingLocation_coordinates VARCHAR,
            landingLocation VARCHAR,
            distanceTraveled_km INTEGER
        );
    )");
    for(const Row& row : entireTable) {
        string str = "INSERT INTO MissileLaunches (dateAndTime_inUtc, startingLocation_city, startingLocation_coordinates, landingLocation, distanceTraveled_km) "
                     "VALUES('" + row.dateAndTime_inUtc + "', '" + row.startingLocation_city + "', '" + row.startingLocation_coordinates + "', '" + row.landingLocation + "', '" + row.distanceTraveled_km + "')";
        QString qStr = QString::fromStdString(str);
        query.exec(qStr);
    }
    query.exec("SELECT * FROM MissileLaunches ORDER BY distanceTraveled_km DESC;");

    QSqlTableModel* model = new QSqlTableModel(this);
    model->setTable("MissileLaunches");
    model->select();

    QTableView* view = new QTableView;
    view->setModel(model);

    view->setColumnWidth(0, 150);
    view->setColumnWidth(1, 300);
    view->setColumnWidth(2, 175);
    view->setColumnWidth(3, 150);
    view->setColumnWidth(4, 125);

    for (int column = 0; column < model->columnCount(); ++column) {
        view->setItemDelegateForColumn(column, new MyCustomDelegate(view));
    }
    QMainWindow* mainWindow = new QMainWindow;
    mainWindow->setCentralWidget(view);
    mainWindow->showMaximized();
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
