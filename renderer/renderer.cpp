#include "renderer.h"

static const int areaWidth = 850;
static const int areaHeight = 900;

QScrollArea * open() {
    QString path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "", "PDF Files (*.pdf)");
    if (path.isEmpty()) {
        return nullptr;
    }

    QPdfDocument *pdf = new QPdfDocument;
    pdf->load(path);

    QScrollArea *scrollArea = new QScrollArea;
    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);

    for (int i = 0; i < pdf->pageCount(); ++i) {

        QSizeF sz = pdf->pagePointSize(i);
        sz.setHeight(sz.height() * (areaWidth / sz.width()));
        sz.setWidth(areaWidth);

        QImage image = pdf->render(i, sz.toSize(), QPdfDocumentRenderOptions());
        QLabel *label = new QLabel;
        label->setPixmap(QPixmap::fromImage(image));
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        layout->addWidget(label);
    }

    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    scrollArea->resize(850, 900);
    scrollArea->show();

    return scrollArea;
}
