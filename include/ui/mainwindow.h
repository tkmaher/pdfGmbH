#include <standard.h>
#include <objects/collection.h>
#include <renderer/renderer.h>

class MainWindow : public QMainWindow
{

public:
    MainWindow(Collection *c_in) : collection(c_in), QMainWindow(nullptr)
    {
        setAcceptDrops(true);

        QTabWidget *tabWidget = new QTabWidget(this);
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);
        connect(tabWidget, &QTabWidget::tabCloseRequested, tabWidget, &QTabWidget::removeTab);
        setCentralWidget(tabWidget);

        // Create Page 1
        QWidget *page1 = new QWidget();
        QVBoxLayout *layout1 = new QVBoxLayout(page1);
        layout1->addWidget(collection->getCollection());
        tabWidget->addTab(page1, "Collection");

        tabWidget->tabBar()->tabButton(0, QTabBar::LeftSide)->hide();
    }

    void openViewerTab(const QString &path, const QString &name)
    {
        QTabWidget *tabWidget = qobject_cast<QTabWidget *>(centralWidget());
        if (!tabWidget) return;

        // Check if the tab already exists
        for (int i = 0; i < tabWidget->count(); ++i) {
            if (tabWidget->tabText(i) == name) {
                tabWidget->setCurrentIndex(i);
                return;
            }
        }

        // Create a new tab for the PDF viewer
        renderers.append(std::make_unique<Renderer>(path));

        tabWidget->addTab(renderers.back()->getParent(), name);
        tabWidget->setCurrentWidget(renderers.back()->getParent());

        connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::handleTabCloseRequest);
    }

    virtual ~MainWindow() {};

protected:
    void dragEnterEvent(QDragEnterEvent *e) override
    {
        bool valid = false;
        const QMimeData *mimedata = e->mimeData();
        QList<QUrl> urllist = mimedata->urls();
        for (auto &url : urllist) {
            QFileInfo filename(url.toLocalFile());
            QMimeDatabase db;
            QMimeType mime = db.mimeTypeForFile(filename);
            if (fileSet.contains(mime.preferredSuffix())) {
                valid = true;
                break;
            }  
        }
        if (valid) { 
            e->acceptProposedAction();  
            collection->getCollection()->setStyleSheet("QTableWidget { background-color: lightgray; }");
        } else {
            e->ignore();
        }
    }

    void dragLeaveEvent(QDragLeaveEvent *e) override
    {
        collection->getCollection()->setStyleSheet("QTableWidget { background-color: white; }");
    }

    void dropEvent(QDropEvent *e) override
    {
        collection->getCollection()->setStyleSheet("QTableWidget { background-color: white; }");
        const QMimeData *mimedata = e->mimeData();
        qDebug() << tr("dropEvent");
        if (mimedata->hasUrls()) {
            QList<QUrl> urllist = mimedata->urls();
            for (auto &url : urllist) {
                QString filename = url.toLocalFile();
                QMimeDatabase db;
                QMimeType mime = db.mimeTypeForFile(filename);
                if (fileSet.contains(mime.preferredSuffix()))
                    collection->addRow(filename);
            }
        }
    }

private:

    void handleTabCloseRequest(int index) {
        QTabWidget *tabWidget = qobject_cast<QTabWidget *>(centralWidget());
        if (!tabWidget || index < 0 || index >= tabWidget->count()) return;

        QString name = tabWidget->tabText(index);
        for (int i = 0; i < renderers.size(); ++i) {
            if (renderers[i]->getName() == name) {
                renderers.removeAt(i);
                break;
            }
        }
        tabWidget->removeTab(index);
    }

    Collection *collection;

    // TODO: add more extensions
    QSet<QString> fileSet = {"pdf", "PDF", "Pdf"};
    QList<std::shared_ptr<Renderer>> renderers;

};