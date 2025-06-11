#pragma once

class Cache;

#include <objects/ParentObject.h>
#include <standard.h>
#include <nlohmann/json.hpp>
#include <regex>

using namespace std;

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

    static void parse(PDFobject *obj);

    QString path;

    QList<QPair<QString, QString>> data;

    ParentObject parent;

};