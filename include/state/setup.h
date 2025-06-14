#include <state/globals.h>

#include <ui/infopanel.h>

class State : public QObject {

public:
    State() {
        dockWidget = new QDockWidget("Library", window);
        dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        importFile = new QPushButton("Import file");
        importFolder = new QPushButton("Import folder");

        //collection panel
        window->setCentralWidget(collection.getCollection());

        QObject::connect(importFile, &QPushButton::clicked, [&]() {
            importOne();
        });

        QWidget *contentWidget = new QWidget();
        layoutLeft = new QVBoxLayout(contentWidget);
        layoutLeft->addWidget(importFile);
        contentWidget->setLayout(layoutLeft);
        dockWidget->setWidget(contentWidget);

        // create central table
        window->addDockWidget(Qt::RightDockWidgetArea, infoPanel.getInfoPanel());
        // left click behavior on cell
        QObject::connect(collection.getCollection(), &QTableWidget::cellClicked, this, &State::collectionClick);
        // right click behavior on cell
        collection.getCollection()->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(collection.getCollection(), &QTableWidget::customContextMenuRequested, this, &State::showContextMenu);

        // Add the dock widget to the main window
        window->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
        
        window->resize(800, 600);
        window->show();

        // instantiate user collection from cache file TODO uncomment
        appCache.readAll(collection);
    }

    ~State() {
        delete dockWidget;
        delete scrollArea;

        delete layoutLeft;
        delete importFile;
        delete importFolder;
    }

private:

    //for center area
    QDockWidget *dockWidget;
    QMainWindow *window = new QMainWindow();;
    QScrollArea *scrollArea;

    //for import/library area
    QVBoxLayout *layoutLeft;
    QPushButton *importFile;
    QPushButton *importFolder;

    Collection collection;
    InfoPanel infoPanel = InfoPanel(window);

    void collectionClick(int row, int column) {
        infoPanel.displayRowInfo(row, column, collection);
    }

    void showContextMenu(const QPoint& pos) {
        QTableWidget *tableWidget = collection.getCollection();

        QMenu menu;
        QAction *deleteAction = menu.addAction("Remove from collection");

        QAction *selectedAction = menu.exec(tableWidget->viewport()->mapToGlobal(pos));

        if (selectedAction == deleteAction) {
            int row = tableWidget->rowAt(pos.y());
            if (row >= 0) {
                // 3 = row of path in collection
                QString path = tableWidget->item(row, 3)->text();
                deleteOne(path);
                tableWidget->removeRow(row);
            }
        }
    }

    void importOne() {
        QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
        if (path.isEmpty()) {
            return;
        }

        collection.addRow(path);
        appCache.writeOne(collection.getMap()[path]);
    }

    void deleteOne(QString path) {
        if (!collection.getMap().contains(path)) {
            qDebug() << "Key out of bounds";
            return;
        }
        
        collection.getMap().remove(path);
        appCache.deleteOne(path);

    }


};