#include "window.h"
#include "renderer.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC453: Assignment 0");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the tools menu
    // Use a group to allow toggle options
    mToolsMenu = menuBar()->addMenu(tr("&Tools"));

    // Setup the help menu
    mHelpMenu = menuBar()->addMenu(tr("&Help"));
    mHelpMenu->addAction(mLineHelpAction);  // add line help

    // Setup the quit button
    QPushButton * quitButton = new QPushButton("&Quit", this);
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    // Setup the application's widget collection
    QVBoxLayout * layout = new QVBoxLayout();

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(300, 300);

    // Add quit button
    layout->addWidget(quitButton);

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setShortcut(QKeySequence(Qt::Key_Q));
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // Displays line help
    mLineHelpAction = new QAction(tr("&Line Help"), this);
    mLineHelpAction->setStatusTip(tr("Displays line help"));
    connect(mLineHelpAction, SIGNAL(triggered()), this, SLOT(lineHelp()));

}

// destructor
Window::~Window()
{
    delete renderer;
}

// Display a help message for line objects
// Note: these could be consolidated into a helper function.
void Window::lineHelp()
{
    QMessageBox msgBox(this);
    msgBox.setText("Drawing a Line:\n"
                   "To draw a line, press the left mouse button to mark the beginning "
                   "of the line. Drag the mouse to the end of the line and release "
                   "the button.");
    msgBox.setWindowTitle(windowTitle());
    msgBox.exec();
}

