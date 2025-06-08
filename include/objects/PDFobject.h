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

    void fromJSON(const QJsonObject &json_in) {
        pData.clear();
        type = static_cast<DocType>(json_in["type"].toInt());
        if (type == DocType::Null)
            return;
        QJsonArray dataArr = json_in["data"].toArray();
        for (const QJsonValue &value : dataArr) {
            QStringList keyValue = value.toString().split('\t');
            if (keyValue.size() == 2)
                setData(keyValue[0], keyValue[1]);
            else if (keyValue.size() == 1)
                setData(keyValue[0], "");
        }
    }

    QJsonObject qJSONify() const {
        QJsonObject obj;
        obj["type"] = static_cast<int>(type);

        QJsonArray dataArr;
        for (const QPair<QString, QString> &j : pData)
            dataArr.append(j.first + '\t' + j.second);
        obj["data"] = dataArr;

        return obj;
    }

    void setData(const QString &key, const QString &value) {
        for (auto &i : pData) {
            if (i.first == key) {
                i.second = value;
                return;
            }
        }
        pData.append(QPair<QString, QString>(key, value));
    }

    void parseCategory(const QString &key1, const QString &key2, 
                        const QString &dataKey, const nlohmann::json &data);

    DocType type;

protected:
    
    QList<QPair<QString, QString>> pData;

};

class BookObject : public ParentObject {

public:

    BookObject(string isbn_in);

private:

    static size_t writeCallback(char *contents, size_t size, size_t nmemb, string *output);

};

class PDFobject {

public:

    PDFobject() {};

    PDFobject(QString path_in);

    PDFobject(QJsonObject json_in);

    void displayInfo(QTableWidget *tableWidget);

    const ParentObject * getParent() {
        return &parent;
    }

    const QString getPath() {
        return path;
    }

    const QString retrieve(const QString &key) {
        for (auto &i : data)
            if (i.first == key) 
                return i.second;
        return "";
    }

    void setData(const QString &key, const QString &value) {
        for (auto &i : data) {
            if (i.first == key) {
                i.second = value;
                return;
            }
        }
        data.append(QPair<QString, QString>(key, value));
    }

    const int getDataSize() {
        return data.size();
    }

    const QList<QPair<QString, QString>> getData() {
        return data;
    }

    QJsonObject qJSONify() {
        QJsonObject obj;
        obj["path"] = path;
        QJsonArray dataArr;
        for (const QPair<QString, QString> &j : data)
            dataArr.append(j.first + '\t' + j.second);
        obj["data"] = dataArr;
        obj["parent"] = parent.qJSONify(); 
        return obj;
    }

    ~PDFobject () {}

    const string getISBN(const string &text);

    void determineType(QPdfDocument *pdf);

    friend class Collection;

private:

    static void handleParsing(PDFobject *obj, QPdfDocument *pdf) {
        obj->determineType(pdf);
        //appCache.writeOne(*obj);
    }

    QString path;

    QList<QPair<QString, QString>> data;

    ParentObject parent;

};