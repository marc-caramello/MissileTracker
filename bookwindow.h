#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <shlwapi.h>
#include <xlsxio_read.h>
#include <QtSql>
#include "mycustomdelegate.h"
#include "row.h"
#include "ui_bookwindow.h"

#pragma comment(lib, "urlmon.lib")

using namespace std;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window();
private:
    vector<Row> entireTable;

    // Put in helper class
    wchar_t* pathToTempFolder();
    char* wideToNarrow(const wchar_t* wideStr);

    // Put in separate class
    void setUp_temp_folder();

    // Put in separate class
    void downloadExcelFile_and_storeItsData();
    void storeExcelFileData(const char* destination);
    bool isLeapYear(const int year);
    int daysInMonth(const int year, const int month);
    string convertDate(const char* excelDate);
    string convertTime(const char* excelTime);

    // Put in separate class
    void createTable_and_displayIt();
};

#endif
