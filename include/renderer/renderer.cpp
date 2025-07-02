#include "renderer.h"
#include <QDebug>

bool PinchContainer::event(QEvent *event) {
    // TODO: recenter on mouse location

    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);
        if (QPinchGesture *pinch = static_cast<QPinchGesture*>(gestureEvent->gesture(Qt::PinchGesture))) {
            if (pinch->state() == Qt::GestureUpdated) {
                qreal scaleFactor = pinch->scaleFactor();
                QPixmap originalPixmap = pixmap(); // Get the image from the QLabel
                QSize newSize = originalPixmap.size() * scaleFactor;
                displayPage(currentPage, zoomLevel * scaleFactor);
                auto n = QCursor::pos();
                move(n.x() - newSize.width() / 2, n.y() - newSize.height() / 2);
            }
        }
    }
    return QWidget::event(event);
}

PinchContainer::PinchContainer(QString path) : QLabel(), pdfPath(path) {
    setMouseTracking(true);

    options.setRenderFlags(QPdfDocumentRenderOptions::RenderFlag::ImageAliased);
    options.setRenderFlags(QPdfDocumentRenderOptions::RenderFlag::TextAliased);

    connect(this, &PinchContainer::renderFinished, this, [&](const QImage &img) {
        QPixmap pixmap = QPixmap::fromImage(img);
        setPixmap(pixmap);
        setAlignment(Qt::AlignCenter);
        resize(pixmap.size());
        qDebug() << "Image size:" << img.size();
        qDebug() << "Label size after setPixmap:" << size();
    });

    scrollArea = new QScrollArea;
    scrollArea->setWidget(this);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidgetResizable(true);

    pdf = new QPdfDocument;
    if (pdf->load(pdfPath) != QPdfDocument::Error::None) {
        qDebug() << "Failed to load PDF document from" << pdfPath;
        setText("Failed to load PDF document.");
        return;
    }

    rend = new QPdfPageRenderer;
    rend->setDocument(pdf);
    connect(rend, &QPdfPageRenderer::pageRendered, this, &PinchContainer::handleRendering);
}

void PinchContainer::displayPage(int page, double zoom) {
    if (!pdf) return;

    zoomLevel = zoom;
    currentPage = page;

    QSize viewportSize = scrollArea->viewport()->size();
    int targetWidth = static_cast<int>(viewportSize.width() * zoom);
    QSizeF pageSize = pdf->pagePointSize(page);
    double scaleFactor = targetWidth / pageSize.width();
    QSize targetSize = (pageSize * scaleFactor).toSize();
    rend->requestPage(page, targetSize, options);
}

void PinchContainer::handleRendering(int pageNumber, QSize, const QImage &image,
                               QPdfDocumentRenderOptions, quint64 requestId) {
    Q_UNUSED(pageNumber)
    Q_UNUSED(requestId)

    if (!image.isNull())
        emit renderFinished(image);
}

Renderer::Renderer(QString path)
{
    // Setup render space
    label = new PinchContainer(path);
    layout = new QVBoxLayout(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //layout->addWidget(label);
    label->setLayout(layout);
    label->setAttribute(Qt::WA_AcceptTouchEvents);
    label->grabGesture(Qt::PinchGesture);
        

    // Setup toolbar and viewer layout
    parent = new QWidget;
    viewerLayout = new QVBoxLayout(parent);
    toolbar = new QToolBar("PDF Viewer", parent);

    QAction* zoomPlus = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ZoomIn), "Zoom In", parent);
    connect(zoomPlus, &QAction::triggered, this, [=]() {
        label->displayPage(label->currentPage, std::min(10.0, label->zoomLevel + 0.1));
    });

    QAction* zoomMinus = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ZoomOut), "Zoom Out", parent);
    connect(zoomMinus, &QAction::triggered, this, [=]() {
        label->displayPage(label->currentPage, std::max(0.1, label->zoomLevel - 0.1));
    });


    toolbar->addAction(zoomPlus);
    toolbar->addAction(zoomMinus);

    viewerLayout->addWidget(toolbar);
    viewerLayout->addWidget(label->scrollArea);

    // Render initial page
    label->displayPage(label->currentPage, 1.0);
}