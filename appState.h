#include "renderer/renderer.h"

class PDFobject {

public:

private:

};

class Collection {

public:

    Collection() {
        collection = new QTableWidget(0, 4);
        QStringList headers;
        headers << "Title" << "Author" << "Subject" << "Keywords";
        collection->setHorizontalHeaderLabels(headers);
        collection->setSelectionBehavior(QAbstractItemView::SelectRows);

        QHeaderView* headerView = new QHeaderView(Qt::Horizontal);
        headerView->setStretchLastSection(true);
        collection->setHorizontalHeader(headerView);

        collection->setColumnWidth(0, 300);
        collection->setColumnWidth(1, 100);
        collection->setColumnWidth(2, 80);
        collection->setColumnWidth(3, 300);

    }

    void addRow(QPdfDocument *pdf) {
        collection->insertRow(collection->rowCount());
        int row = collection->rowCount()-1;
        // TODO: abstract this and add api calls
        collection->setItem(row, 0, new QTableWidgetItem(pdf->metaData(QPdfDocument::MetaDataField::Title).toString()));
        collection->setItem(row, 1, new QTableWidgetItem(pdf->metaData(QPdfDocument::MetaDataField::Author).toString()));
        collection->setItem(row, 2, new QTableWidgetItem(pdf->metaData(QPdfDocument::MetaDataField::Subject).toString()));
        collection->setItem(row, 3, new QTableWidgetItem(pdf->metaData(QPdfDocument::MetaDataField::Keywords).toString()));
    }

    QTableWidget * getCollection() {
        return collection;
    }

    ~Collection() {
        delete collection;
    }

private:

    QTableWidget *collection;

};

class State {

public:
    State() {
        dockWidget = new QDockWidget("Library", &window);
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        importFile = new QPushButton("Import file");
        importFolder = new QPushButton("Import folder");

        //collection panel
        window.setCentralWidget(collection.getCollection());

        QObject::connect(importFile, &QPushButton::clicked, [&]() {
            importOne();
        });

        contentWidget = new QWidget();
        layout = new QVBoxLayout(contentWidget);
        layout->addWidget(importFile);
        contentWidget->setLayout(layout);
        dockWidget->setWidget(contentWidget);

        // Add the dock widget to the main window
        window.addDockWidget(Qt::LeftDockWidgetArea, dockWidget); //Dock to the left side
        
        window.resize(800, 600);
        window.show();
    }
private:

    QDockWidget *dockWidget;
    QMainWindow window;
    QScrollArea *scrollArea;
    QWidget *contentWidget;
    QVBoxLayout *layout;

    QPushButton *importFile;
    QPushButton *importFolder;

    Collection collection;

    void importOne() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
        if (path.isEmpty()) {
            return;
        }

        QPdfDocument *pdf = new QPdfDocument;
        pdf->load(path);
        collection.addRow(pdf);
    }
};