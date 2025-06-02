#include "renderer.h"

static const int areaWidth = 850;
static const int areaHeight = 900;
static const int pageInitWidth = 800;

QLabel *getPage(int page, QPdfDocument * pdf, QSizeF sz) {
    QImage image = pdf->render(page, sz.toSize(), QPdfDocumentRenderOptions());
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap::fromImage(image));
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    return label;
}

void displayPage(int page, QDockWidget * scrollArea, QPdfDocument * pdf) {
    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);

    QSizeF sz = pdf->pagePointSize(page);
    sz.setHeight(sz.height() * (double(pageInitWidth) / sz.width()));
    sz.setWidth(pageInitWidth);

    layout->addWidget(getPage(page, pdf, sz));

    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->resize(areaWidth, areaHeight);
    scrollArea->show();
}

void open(QScrollArea * scrollArea) {
    QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
    if (path.isEmpty()) {
        return;
    }

    QPdfDocument *pdf = new QPdfDocument;
    pdf->load(path);
}
