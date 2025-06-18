#include <state/setup.h>

State::State() {
    window = new MainWindow(&collection);
    dockWidget = new QDockWidget("Library", window);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    importFile = new QPushButton("Import file");
    importFolder = new QPushButton("Import folder");

    //collection panel
    window->setCentralWidget(collection.getCollection());
    window->setAcceptDrops(true);
    

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
    // right click behavior on cell
    collection.getCollection()->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(collection.getCollection(), &QTableWidget::customContextMenuRequested, this, &State::showContextMenu);
    // arrow navigation behavior on cell
    QObject::connect(collection.getCollection(), &QTableWidget::currentCellChanged, this, &State::collectionNav);

    // Add the dock widget to the main window
    window->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    
    window->resize(800, 600);
    window->show();

    // instantiate user collection from cache file TODO uncomment
    appCache.readAll(collection);
}

State::~State() { /* probably unnecessary */ }

void State::collectionNav(int currentRow, int currentColumn, int previousRow, int previousColumn) {
    qDebug() << "Show row: " << currentRow;
    infoPanel.displayRowInfo(currentRow, currentColumn, collection);
}

void State::showContextMenu(const QPoint& pos) {
    QTableWidget *tableWidget = collection.getCollection();

    QMenu menu;
    QAction *deleteAction = menu.addAction("Remove from collection");
    QAction *finderAction = menu.addAction("Reveal in file explorer");

    QAction *selectedAction = menu.exec(tableWidget->viewport()->mapToGlobal(pos));

    if (selectedAction == deleteAction) {
        QList<QTableWidgetItem*> rows = tableWidget->selectedItems();
        tableWidget->clearSelection();
        QList<int> rowsToDelete;
        for (QTableWidgetItem* item : rows) {
            if (!rowsToDelete.contains(item->row()))
                rowsToDelete.append(item->row());
            
        }
        sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

        for (const auto &row : rowsToDelete) {
            QString path = tableWidget->item(row, 2)->text();
            tableWidget->removeRow(row);
            deleteOne(path);
        }
    } else if (selectedAction == finderAction) {
        int row = tableWidget->rowAt(pos.y());
        if (row >= 0) {
            // 2 = row of path in collection
            // TODO: replace this with global
            QString path = tableWidget->item(row, 2)->text();
            showInFolder(path);
        }
    }
}

void State::importOne() {
    //QMutexLocker locker{collection.getMutex()};
    QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
    if (path.isEmpty()) {
        return;
    }

    collection.addRow(path);
    appCache.writeOne(collection.getMap()[path]);
}

void State::deleteOne(QString path) {
    //QMutexLocker locker{collection.getMutex()};
    if (!collection.getMap().contains(path)) {
        qDebug() << "Key out of bounds";
        return;
    }
    
    collection.getMap().remove(path);
    appCache.deleteOne(path);

}

void State::showInFolder(const QString& path) {
    QFileInfo info(path);
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
}
