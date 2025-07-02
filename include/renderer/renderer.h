#ifndef RENDERER_H
#define RENDERER_H

#include <standard.h>

class PinchContainer : public QLabel {

    Q_OBJECT

signals:
    void renderFinished(const QImage &image);  // ← Signal

public slots:
    void handleRendering(int pageNumber, QSize size, const QImage &image,
                         QPdfDocumentRenderOptions options, quint64 requestId);

public:

    PinchContainer(QString pdfPath);

    void displayPage(int page, double zoom);

    friend class Renderer;

private:

    int currentPage = 0;
    double zoomLevel = 1.0;

    QString pdfPath;

    QPdfDocumentRenderOptions options;
    QPdfDocument *pdf;
    QPdfPageRenderer *rend;
    QScrollArea *scrollArea;

    bool event(QEvent *event);

};


class Renderer : public QObject {

    Q_OBJECT

public:
    Renderer(QString path);


    QWidget *getParent() const {
        return parent;
    }

    QString getName() {
        return QFileInfo(pdfPath).baseName();
    }

private:

    QString pdfPath;
    QVBoxLayout *layout;
    PinchContainer *label;

    QWidget *parent;
    QVBoxLayout *viewerLayout;
    QToolBar *toolbar;

};

#endif // RENDERER_H
