#include "bookwindow.h"

Window::Window()
{
    setUp_temp_folder();
    downloadExcelFile_and_storeItsData();
    createTable_and_displayIt();
}

void Window::setUp_temp_folder()
{
    char* pathTo_temp_folder = wideToNarrow(pathToTempFolder());

    if (filesystem::exists(pathTo_temp_folder) && filesystem::is_directory(pathTo_temp_folder)) {
        filesystem::remove_all(pathTo_temp_folder);
    }
    filesystem::create_directory(pathTo_temp_folder);
}

void Window::downloadExcelFile_and_storeItsData()
{
    const wchar_t* url = L"https://www.nti.org/wp-content/uploads/2021/10/north_korea_missile_test_database.xlsx";

    wstring pathToExcelFile = pathToTempFolder();
    pathToExcelFile += L"\\input.xlsx";

    URLDownloadToFile(NULL, url, pathToExcelFile.c_str(), 0, NULL);
    storeExcelFileData(wideToNarrow(pathToExcelFile.c_str()));
}

wchar_t* Window::pathToTempFolder() {
    DWORD buffer_size = GetCurrentDirectory(0, nullptr);
    wchar_t* pathTo_temp_folder = new wchar_t[buffer_size];
    GetCurrentDirectory(buffer_size, pathTo_temp_folder);

    // Go back 1 folder (right now it is in the "build" folder), append "\\temp", then return its value
    PathRemoveFileSpec(pathTo_temp_folder);
    PathAppend(pathTo_temp_folder, L"temp");
    return pathTo_temp_folder;
}

char* Window::wideToNarrow(const wchar_t* wideStr) {
    size_t size = wcstombs(nullptr, wideStr, 0);
    char* narrowStr = new char[size + 1];
    wcstombs(narrowStr, wideStr, size);
    narrowStr[size] = '\0';
    return narrowStr;
}

void Window::storeExcelFileData(const char* pathToExcelFile)
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

bool Window::isLeapYear(const int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Window::daysInMonth(const int year, const int month) {
    vector<int> monthLengths = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return monthLengths[month - 1];
}

string Window::convertDate(const char* excelDate) {
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

string Window::convertTime(const char* excelTime) {
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

void Window::createTable_and_displayIt() {
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
    this->setCentralWidget(view);
    this->showMaximized();
}
