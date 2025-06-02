#include <objects/PDFobject.h>
#include <renderer/renderer.h>

class Collection {

public:

    Collection() {
        collection = new QTableWidget(0, 2);
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

    void addRow(QPdfDocument *pdf, QString path) {
        collection->insertRow(collection->rowCount());
        int row = collection->rowCount()-1;
        items.emplace_back(pdf, path);
        auto p = items.back();
        const QString title("Title");
        const QString author("Author");
        collection->setItem(row, 0, new QTableWidgetItem(p.retrieve(title)));
        collection->setItem(row, 1, new QTableWidgetItem(p.retrieve(author)));
    }

    QTableWidget * getCollection() {
        return collection;
    }

    PDFobject * getPDF(int index) {
        return &items[index];
    }

    ~Collection() {
        delete collection;
    }

private:

    std::vector<PDFobject> items;
    QTableWidget *collection;

};

class State : public QObject {

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

        QWidget *contentWidget = new QWidget();
        layoutLeft = new QVBoxLayout(contentWidget);
        layoutLeft->addWidget(importFile);
        contentWidget->setLayout(layoutLeft);
        dockWidget->setWidget(contentWidget);

        //info sidebar

        infoPanel = new QDockWidget("Information", &window);
        infoPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        infoPanel->setFixedWidth(200); // Set width
        infoPanel->hide(); // Initially hide the sidebar
        window.addDockWidget(Qt::RightDockWidgetArea, infoPanel);
        QObject::connect(collection.getCollection(), &QTableWidget::cellClicked, this, &State::displayRowInfo);

        // Add the dock widget to the main window
        window.addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        
        window.resize(800, 600);
        window.show();
    }

    ~State() {
        delete dockWidget;
        delete scrollArea;

        delete layoutLeft;
        delete importFile;
        delete importFolder;

        delete layoutRight;
        delete infoPanel;
        delete infoTable;
        delete infoContent;
    }

private:

    //for center area
    QDockWidget *dockWidget;
    QMainWindow window;
    QScrollArea *scrollArea;

    //for import/library area
    QVBoxLayout *layoutLeft;
    QPushButton *importFile;
    QPushButton *importFolder;

    Collection collection;

    // for info panel
    QVBoxLayout *layoutRight;
    QDockWidget *infoPanel;
    QTableWidget *infoTable;
    QWidget *infoContent;

    void displayRowInfo(int row, int column) {
        qDebug() << "Cell clicked at row:" << row << ", column:" << column; // Debugging
        if (infoPanel->isHidden()){
            infoPanel->show();
            infoTable = new QTableWidget(collection.getPDF(row)->getDataSize(), 2);
            infoContent = new QWidget();
            layoutRight = new QVBoxLayout(infoContent);
            layoutRight->addWidget(infoTable);
            infoContent->setLayout(layoutRight);
            infoPanel->setWidget(infoContent);
            collection.getPDF(row)->displayInfo(infoTable);
            infoTable->resizeColumnsToContents();


            //getPage(0, collection.getPDF(row)->getPDF(), QSizeF(200, 300));

        } else {
            infoPanel->hide();
        }
    }

    void importOne() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
        if (path.isEmpty()) {
            return;
        }

        QPdfDocument *pdf = new QPdfDocument;
        pdf->load(path);
        collection.addRow(pdf, path);
    }
};