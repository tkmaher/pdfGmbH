#include "collection.h"

Collection::Collection() {
    collection = new QTableWidget(0, 4);
    QStringList headers;
    headers << "Title" << "Author" << "Subject" << "Keywords";
    collection->setHorizontalHeaderLabels(headers);
    collection->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView* headerView = new QHeaderView(Qt::Horizontal);
    collection->verticalHeader()->setVisible(false);
    headerView->setStretchLastSection(true);
    collection->setHorizontalHeader(headerView);

    collection->setColumnWidth(0, 300);
    collection->setColumnWidth(1, 100);

}

void Collection::addRow(PDFobject &pdf) {
    QString path = pdf.getPath();
    if (items.contains(path))
        return;

    collection->insertRow(collection->rowCount());
    int row = collection->rowCount()-1;    
    items[path] = pdf;
    getPDF(path); 
    collection->setItem(row, 0, new QTableWidgetItem(pdf.retrieve("Title")));
    collection->setItem(row, 1, new QTableWidgetItem(pdf.retrieve("Author")));
    collection->setItem(row, 2, new QTableWidgetItem(pdf.retrieve("Date Added")));
    collection->setItem(row, 3, new QTableWidgetItem(pdf.getPath()));
}

void Collection::addRow(QString path) {
    if (items.contains(path))
        return;

    collection->insertRow(collection->rowCount());
    int row = collection->rowCount()-1;    
    items[path] = PDFobject(path);
    auto p = items[path];
    collection->setItem(row, 0, new QTableWidgetItem(p.retrieve("Title")));
    collection->setItem(row, 1, new QTableWidgetItem(p.retrieve("Author")));
    collection->setItem(row, 2, new QTableWidgetItem(p.retrieve("Date Added")));
    collection->setItem(row, 3, new QTableWidgetItem(p.getPath()));
}

int Collection::getRowCount() {
    return collection->rowCount();
}

QMap<QString, PDFobject> & Collection::getMap() {
    return items;
}

QTableWidget * Collection::getCollection() const {
    return collection;
}

PDFobject * Collection::getPDF(QString &path) {
    if (!items.contains(path)) {
        qDebug() << "Key out of bounds";
        return nullptr;
    }
    return &items[path];
}

Collection::~Collection() {
    delete collection;
}