#include <standard.h>

QLabel *getPage(int page, QPdfDocument * pdf, QSizeF sz);

void displayPage(int page, QDockWidget * scrollArea, QPdfDocument * pdf);

void open(QScrollArea * scrollArea);