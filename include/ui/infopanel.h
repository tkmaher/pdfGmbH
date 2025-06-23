#include <standard.h>
#include <objects/collection.h>

class InfoPanel {

public:

    InfoPanel(QWidget *window);

    QDockWidget *getInfoPanel();

    ~InfoPanel();

    void displayRowInfo(int row, int column, Collection &collection);


private:

    QDockWidget *infoPanel;
    QTableWidget *infoTable = new QTableWidget(0, 2);;
    QWidget *infoContent = new QWidget();
    QVBoxLayout *layoutRight = new QVBoxLayout(infoContent);
    QLabel *imgLabel = new QLabel;
    QPdfDocument *doc = new QPdfDocument;
    QString currentPath = "";

};