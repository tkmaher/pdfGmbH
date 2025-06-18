#include <standard.h>
#include <objects/collection.h>

class MainWindow : public QMainWindow
{

public:
    MainWindow(Collection *c_in) : collection(c_in), QMainWindow(nullptr)
    {
        setAcceptDrops(true);
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