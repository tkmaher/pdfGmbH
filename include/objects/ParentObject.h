#pragma once

#include <standard.h>
#include <nlohmann/json.hpp>
#include <regex>

using namespace std;

static int pagesToScan = 5;

const string user_agent = "pdfprojtest_tomaszkkmaher@gmail.com/1.0";
const string ISBNendpoint = "https://openlibrary.org/api/volumes/brief/isbn/"; 

enum DocType {
    Unwritten,
    Null,
    Book,
    Journal,
};

class ParentObject {
public:

    ParentObject();

    void fromJSON(const QJsonObject &json_in);

    QJsonObject qJSONify() const;

    void setData(const QString &key, const QString &value);

    void parseCategory(const QString &key1, const QString &key2, 
                        const QString &dataKey, const QJsonObject &data);

    const QList<QPair<QString, QString>> getData();

    const DocType getType() const;

    friend class PDFobject;

protected:
    
    DocType type;
    QList<QPair<QString, QString>> pData;

};

class BookObject : public ParentObject {

public:

    BookObject(string isbn_in);

private:

    static size_t writeCallback(char *contents, size_t size, size_t nmemb, string *output);

};