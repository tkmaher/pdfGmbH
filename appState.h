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

    void addRow(QString path) {
        collection->insertRow(collection->rowCount());
        int row = collection->rowCount()-1;
        items.emplace_back(path);
        auto p = items.back();
        const QString title("Title");
        const QString author("Author");
        collection->setItem(row, 0, new QTableWidgetItem(p.retrieve(title)));
        collection->setItem(row, 1, new QTableWidgetItem(p.retrieve(author)));
    }

    int getRowCount() {
        return collection->rowCount();
    }

    QTableWidget * getCollection() {
        return collection;
    }

    PDFobject * getPDF(int index) {
        if (index < 0 || index >= items.size()) {
            qDebug() << "Index out of bounds";
            return nullptr;
        }
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
    QDockWidget *infoPanel;
    QTableWidget *infoTable = new QTableWidget(0, 2);;
    QWidget *infoContent = new QWidget();
    QVBoxLayout *layoutRight = new QVBoxLayout(infoContent);
    QLabel *imgLabel = new QLabel;
    QPdfDocument *doc = new QPdfDocument;

    void displayRowInfo(int row, int column) {
        if (infoPanel->isHidden()){

            // TODO: error checking
            QString path = collection.getPDF(row)->getPath();
            doc->load(path); // check to see if current pdf is already loaded?
            QImage image = doc->render(0, QSize(200, 300), QPdfDocumentRenderOptions());
            imgLabel->setPixmap(QPixmap::fromImage(image));
            imgLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            layoutRight->addWidget(imgLabel);

            infoPanel->show();
            infoTable->setRowCount(collection.getPDF(row)->getDataSize());
            layoutRight->addWidget(infoTable);
            infoContent->setLayout(layoutRight);
            infoPanel->setWidget(infoContent);
            collection.getPDF(row)->displayInfo(infoTable);
            infoTable->resizeColumnsToContents();

        } else {
            infoPanel->hide();
        }
    }

    void importOne() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
        if (path.isEmpty()) {
            return;
        }

        collection.addRow(path);

    }
};