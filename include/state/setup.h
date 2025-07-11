#include <state/globals.h>
#include <ui/mainwindow.h>
#include <ui/infopanel.h>

class State : public QObject {

public:
    State();
    ~State();

private:

    //for center area
    QDockWidget *dockWidget;
    MainWindow *window;
    QScrollArea *scrollArea;

    //for import/library area
    QVBoxLayout *layoutLeft;
    QPushButton *importFile;
    QPushButton *importFolder;

    Collection collection;
    InfoPanel infoPanel = InfoPanel(window);

    void openPDF(int row, int column);

    void collectionNav(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void showContextMenu(const QPoint& pos);

    void importOne();

    void importMany();

    void deleteOne(QString path);

    void showInFolder(const QString& path);


};