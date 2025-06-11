#pragma once

class Collection;
class PDFobject;

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QTableWidget>
#include <QDebug>

class Cache  {

public:

    Cache() {
        QFile file(fileName);
        if (file.exists()) {
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                return; // Or handle the error appropriately
            }
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            stateJSON = doc.object();
            file.close();
        } else {
            stateJSON = QJsonObject();
            stateJSON["collection"] = QJsonObject();
        }
    }

    void readAll(Collection &collection);

    void writeOne(PDFobject &pdfobj);

    void writeAll(Collection &collection);

    void writeToFile();

private:

    QMutex cmutex;
    
    QString fileName = "cache.json";
    QJsonObject stateJSON;

};