#pragma once

#include <objects/PDFobject.h>

class Collection {

public:

    Collection();

    void addRow(PDFobject &pdf);

    void addRow(QString path);

    int getRowCount();

    QMap<QString, PDFobject> & getMap();

    QTableWidget * getCollection() const;

    PDFobject * getPDF(QString &path);

    ~Collection();

private:

    QMap<QString, PDFobject> items;
    QTableWidget *collection;

};