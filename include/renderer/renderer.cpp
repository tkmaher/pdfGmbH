#include "renderer.h"

static const int areaWidth = 850;
static const int areaHeight = 900;
static const int pageInitWidth = 800;

Renderer::Renderer(QString path) {
    pdf = new QPdfDocument;
    pdf->load(path);

    container = new QWidget;
    layout = new QVBoxLayout(container);

    displayPage(0);
}

QLabel * Renderer::getPage(int page, QSizeF sz) {
    QImage image = pdf->render(page, sz.toSize(), QPdfDocumentRenderOptions());
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap::fromImage(image));
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    return label;
}

void Renderer::displayPage(int page) {

    QSizeF sz = pdf->pagePointSize(page);
    sz.setHeight(sz.height() * (double(pageInitWidth) / sz.width()));
    sz.setWidth(pageInitWidth);

    layout->addWidget(getPage(page, sz));

    container->setLayout(layout);
}
