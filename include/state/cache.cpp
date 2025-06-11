#include "cache.h"
#include <objects/collection.h>

void Cache::readAll(Collection &collection) {
    QMutexLocker locker{&cmutex};
    QJsonObject obj = stateJSON["collection"].toObject();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        PDFobject pdfobj(it->toObject());
        collection.addRow(&pdfobj);
    }
    qDebug() << "Cache read successfully";
}

void Cache::writeOne(PDFobject &pdfobj) {
    QMutexLocker locker{&cmutex};
    QJsonObject obj = stateJSON["collection"].toObject();
    obj[pdfobj.getPath()] = pdfobj.qJSONify();
    stateJSON["collection"] = obj;
    writeToFile();
    qDebug() << "Successfully wrote PDFobject to JSON: " << pdfobj.getPath();
}

void Cache::writeAll(Collection &collection) {
    QMutexLocker locker{&cmutex};
    QJsonObject obj = stateJSON["collection"].toObject();
    for (auto &i : collection.getMap())
        obj[i.getPath()] = (i.qJSONify());
    stateJSON["collection"] = obj;
    writeToFile();
}

void Cache::writeToFile() {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing.";
        return; // Or handle the error appropriately
    }

    file.write(QJsonDocument(stateJSON).toJson());
    file.close();

    qDebug() << "State saved to file:" << fileName;
}