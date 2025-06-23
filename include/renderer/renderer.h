#include <standard.h>

class Renderer {
public:
    Renderer(QString path);

    // Renders a page of the PDF document and returns a QLabel containing the rendered image.
    QLabel *getPage(int page, QSizeF sz);

    // Displays a specific page in a scroll area.
    void displayPage(int page);

    QWidget *getParent() const {
        return container;
    }

private:

    QDockWidget * scrollArea;
    QWidget *container;
    QVBoxLayout *layout;

    QPdfDocument *pdf;

};