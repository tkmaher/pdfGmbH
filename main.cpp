#include "renderer/renderer.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;

    // Create a dock widget for the side panel
    QDockWidget *dockWidget = new QDockWidget("Library", &window);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create a widget to put inside the dock widget (e.g., a text edit)
    QTextEdit *textEdit = new QTextEdit();
    dockWidget->setWidget(textEdit);

    // Create a button
    QPushButton *button1 = new QPushButton("Import file");
    QPushButton *button2 = new QPushButton("Import folder");
    QObject::connect(button1, &QPushButton::clicked, [&]() {
        window.setCentralWidget(open());
    });
    // Create a widget to hold the combobox
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->addWidget(button1);
    contentWidget->setLayout(layout);
    dockWidget->setWidget(contentWidget);

    // Add the dock widget to the main window
    window.addDockWidget(Qt::LeftDockWidgetArea, dockWidget); //Dock to the left side
    

    window.resize(800, 600);
    window.show();

    return app.exec();
}
