#include <standard.h>
#include "infopanel.h"


InfoPanel::InfoPanel(QMainWindow *window) {
    infoPanel = new QDockWidget("Information", window);
    infoPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    infoPanel->setFixedWidth(200); 
    infoPanel->hide(); 
}

QDockWidget * InfoPanel::getInfoPanel() {
    return infoPanel;
}

void InfoPanel::displayRowInfo(int row, int column, Collection &collection) {
    QTableWidget *c = collection.getCollection();
    if (c->rowCount() == 0 || row < 0 || row >= c->rowCount()) {
        infoPanel->hide();
        return;
    }
    QString path = c->item(row, 3)->text();
    if (infoPanel->isHidden() || path != currentPath) {
        // TODO: error checking
        if (path != currentPath) {
            doc->load(path); // check to see if current pdf is already loaded?
            QSizeF sz = doc->pagePointSize(0);
            sz.setHeight(sz.height() * (double(200) / sz.width()));
            sz.setWidth(200);
            QImage image = doc->render(0, sz.toSize(), QPdfDocumentRenderOptions());
            imgLabel->setPixmap(QPixmap::fromImage(image));
            imgLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            layoutRight->addWidget(imgLabel);

            infoTable->setRowCount(collection.getPDF(path)->getDataSize());
            layoutRight->addWidget(infoTable);
            infoContent->setLayout(layoutRight);
            infoPanel->setWidget(infoContent);
            collection.getPDF(path)->displayInfo(infoTable);
            infoTable->resizeColumnsToContents();
        }

        infoPanel->show();
    } else if (path == currentPath) {
        infoPanel->hide();
    }

    currentPath = path;
}

InfoPanel::~InfoPanel() {
    // delete infoPanel;
    // delete infoTable;
    // delete imgLabel;
    // delete doc;
}