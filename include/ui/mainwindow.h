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
        setCentralWidget(tabWidget);

        // Create Page 1
        QWidget *page1 = new QWidget();
        QVBoxLayout *layout1 = new QVBoxLayout(page1);
        layout1->addWidget(collection->getCollection());
        tabWidget->addTab(page1, "Tab 1");


    }

    void openViewerTab(const QString &path)
    {
        QTabWidget *tabWidget = qobject_cast<QTabWidget *>(centralWidget());
        if (!tabWidget) return;

        // Check if the tab already exists
        for (int i = 0; i < tabWidget->count(); ++i) {
            if (tabWidget->tabText(i) == path) {
                tabWidget->setCurrentIndex(i);
                return;
            }
        }

        // Create a new tab for the PDF viewer
        QWidget *viewerTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(viewerTab);
        Renderer r(path);
        layout->addWidget(r.getParent());
        tabWidget->addTab(viewerTab, path);
        tabWidget->setCurrentWidget(viewerTab);
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

    Collection *collection;

    // TODO: add more extensions
    QSet<QString> fileSet = {"pdf", "PDF", "Pdf"};


};