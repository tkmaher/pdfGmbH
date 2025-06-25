#include "renderer.h"
#include <QDebug>

Renderer::Renderer(QString path)
{
    pdfPath = path;

    // Setup render space
    scrollArea = new QScrollArea;
    container = new QWidget;
    layout = new QVBoxLayout(container);
    label = new QLabel(container);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(label);
    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidgetResizable(true);

    options.setRenderFlags(QPdfDocumentRenderOptions::RenderFlag::ImageAliased);
    options.setRenderFlags(QPdfDocumentRenderOptions::RenderFlag::TextAliased);

    pdf = new QPdfDocument;
    if (pdf->load(pdfPath) != QPdfDocument::Error::None) {
        qDebug() << "Failed to load PDF document from" << pdfPath;
        label->setText("Failed to load PDF document.");
        return;
    }

    rend = new QPdfPageRenderer;
    rend->setDocument(pdf);

    // Connect renderFinished signal to update the label on the GUI thread
    connect(this, &Renderer::renderFinished, this, [&](const QImage &img) {
        QPixmap pixmap = QPixmap::fromImage(img);
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);
        label->resize(pixmap.size());
        qDebug() << "Image size:" << img.size();
        qDebug() << "Label size after setPixmap:" << label->size();

        qDebug() << "Rendering finished for page" << currentPage;
    });

    // Connect the renderer's signal to our handler
    connect(rend, &QPdfPageRenderer::pageRendered, this, &Renderer::handleRendering);

    // Setup toolbar and viewer layout
    parent = new QWidget;
    viewerLayout = new QVBoxLayout(parent);
    toolbar = new QToolBar("PDF Viewer", parent);

    QAction* zoomPlus = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ZoomIn), "Zoom In", parent);
    connect(zoomPlus, &QAction::triggered, this, [=]() {
        zoomLevel = std::max(10.0, zoomLevel + 0.1);
        displayPage(currentPage, zoomLevel);
    });

    QAction* zoomMinus = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ZoomOut), "Zoom Out", parent);
    connect(zoomMinus, &QAction::triggered, this, [=]() {
        zoomLevel = std::max(0.1, zoomLevel - 0.1);
        displayPage(currentPage, zoomLevel);
    });


    toolbar->addAction(zoomPlus);
    toolbar->addAction(zoomMinus);

    viewerLayout->addWidget(toolbar);
    viewerLayout->addWidget(scrollArea);

    // Render initial page
    displayPage(0, 1.0);

}

void Renderer::displayPage(int page, double zoom)
{
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

void Renderer::handleRendering(int pageNumber, QSize, const QImage &image,
                               QPdfDocumentRenderOptions, quint64 requestId) {
    Q_UNUSED(pageNumber)
    Q_UNUSED(requestId)

    if (!image.isNull())
        emit renderFinished(image);
}