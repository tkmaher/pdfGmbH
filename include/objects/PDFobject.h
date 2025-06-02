#pragma once

#include <standard.h>
#include <nlohmann/json.hpp>
#include <regex>

using namespace std;

static int pagesToScan = 5;

const string user_agent = "pdfprojtest_tomaszkkmaher@gmail.com/1.0";
const string ISBNendpoint = "https://openlibrary.org/api/volumes/brief/isbn/"; 

enum DocType {
    Null,
    Book,
    Journal,
};

class ParentObject {
public:

    ParentObject() {
        type = DocType::Null;
    }

    DocType type;
};

class BookObject : public ParentObject {

public:

    BookObject(string isbn_in);

private:

    static size_t writeCallback(char *contents, size_t size, size_t nmemb, string *output);

    // title, subtitle, author list, series, series number, volume, volume count, edition, publisher, publish location, publish date, page count, language, isbn, subjects
    string title;
    string subtitle;
    vector<string> authors;
    vector<string> series;
    /*string volume;
    string volumeCount;
    string edition;*/
    vector<string> publishers;
    vector<string> pubLocs;
    string pubYear;
    vector<string> langs;
    string ISBN;
    vector<string> subjects;
};

class PDFobject {

public:

    PDFobject() {};

    PDFobject(QPdfDocument *pdf_in, QString path_in);

    void displayInfo(QTableWidget *tableWidget);

    QPdfDocument *getPDF() {
        return pdf;
    }

    QString retrieve(const QString &in) {
        for (auto &i : data) {
            if (i.first == in) 
                return i.second;
        }
        return "";
    }

    int getDataSize() {
        return data.size();
    }

    ~PDFobject () {
        delete pdf;
    }

    friend class Collection;

private:

    QPdfDocument *pdf = nullptr;
    QString path;

    QList<QPair<QString, QString>> data;

    ParentObject parent;

    const string getISBN(const string &text);

    void determineType(QPdfDocument *pdf);

};