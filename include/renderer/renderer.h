#ifndef RENDERER_H
#define RENDERER_H

#include <standard.h>

class Renderer : public QObject {

    Q_OBJECT

public:
    Renderer(QString path);
    

    // Displays a specific page in a scroll area.
    void displayPage(int page, double zoom);


    QWidget *getParent() const {
        return parent;
    }

    QString getName() {
        return QFileInfo(pdfPath).baseName();
    }

signals:
    void renderFinished(const QImage &image);  // ← Signal

private slots:
    void handleRendering(int pageNumber, QSize size, const QImage &image,
                         QPdfDocumentRenderOptions options, quint64 requestId);

private:

    QString pdfPath;
    QWidget *container;
    QVBoxLayout *layout;
    QLabel *label;
    QScrollArea *scrollArea;

    QPdfDocument *pdf;
    QPdfPageRenderer *rend;
    QPdfDocumentRenderOptions options;

    QWidget *parent;
    QVBoxLayout *viewerLayout;
    QToolBar *toolbar;

    int currentPage = 0;
    double zoomLevel = 1.0;
};

#endif // RENDERER_H
