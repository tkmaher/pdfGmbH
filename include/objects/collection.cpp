#include "collection.h"

Collection::Collection() {
    collection = new QTableWidget(0, 4);
    QStringList headers;
    headers << "Title" << "Author" << "Date Added" << "Path";
    collection->setHorizontalHeaderLabels(headers);
    collection->setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView* headerView = new QHeaderView(Qt::Horizontal);
    collection->verticalHeader()->setVisible(false);
    headerView->setStretchLastSection(true);
    collection->setHorizontalHeader(headerView);

    collection->setColumnWidth(0, 300);
    collection->setColumnWidth(1, 100);

}

void Collection::addRow(PDFobject *p) {
    QString path = p->getPath();
    if (items.contains(path))
        return;

    collection->insertRow(collection->rowCount());
    int row = collection->rowCount()-1;    
    items[path] = *p;
    getPDF(path); 
    collection->setItem(row, 0, new QTableWidgetItem(p->retrieve("Title")));
    collection->setItem(row, 1, new QTableWidgetItem(p->retrieve("Author")));
    collection->setItem(row, 2, new QTableWidgetItem(p->retrieve("Date Added")));
    collection->setItem(row, 3, new QTableWidgetItem(p->getPath()));

    if (p->getParent()->getType() == static_cast<int>(DocType::Unwritten))
        QFuture<void> future1 = QtConcurrent::run( PDFobject::parse, &items[path] );
}

void Collection::addRow(QString path) {
    if (items.contains(path))
        return;

    collection->insertRow(collection->rowCount());
    int row = collection->rowCount()-1;    
    items[path] = PDFobject(path);
    PDFobject *p = &items[path];
    collection->setItem(row, 0, new QTableWidgetItem(p->retrieve("Title")));
    collection->setItem(row, 1, new QTableWidgetItem(p->retrieve("Author")));
    collection->setItem(row, 2, new QTableWidgetItem(p->retrieve("Date Added")));
    collection->setItem(row, 3, new QTableWidgetItem(p->getPath()));
    // Start parsing the PDF in a separate thread
    QFuture<void> future1 = QtConcurrent::run( PDFobject::parse, &items[path] );
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