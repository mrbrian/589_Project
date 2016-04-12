#include "window.h"
#include "renderer.h"
#include <QSignalMapper>

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("CPSC589 Project: Populating Trees");

    // Create the main drawing object
    renderer = new Renderer();

    // Create the actions to be used by the menus
    createActions();

    // Create the menus
    // Setup the file menu
    mFileMenu = menuBar()->addMenu(tr("&File"));
    mFileMenu->addAction(mLoadHeightMapAction);
    mFileMenu->addAction(mLoadTreeAction);
    mFileMenu->addAction(mLoadTextureAction);
    mFileMenu->addAction(mResetModelsAction);
    mFileMenu->addAction(mResetViewAction);
    mFileMenu->addAction(mResetAllAction);
    mFileMenu->addAction(mClearAction);
    mFileMenu->addAction(mQuitAction);  // add quitting

    // Setup the draw menu
    mDrawMenu = menuBar()->addMenu(tr("&Draw"));
    mDrawMenu->addAction(mWireAction);
    mDrawMenu->addAction(mFaceAction);
    mDrawMenu->addAction(mTextureAction);

    mNormalsMenu = mDrawMenu->addMenu(tr("&Normals"));
    mNormalsMenu->addAction(mNormalFaceAction);
    mNormalsMenu->addAction(mNormalVertAction);
    mNormalsMenu->addAction(mShowNormalAction);

    mDrawMenu->addAction(mSelectModelAction);

    // Setup the draw menu
    mOptionMenu = menuBar()->addMenu(tr("&Options"));
    mOptionMenu->addAction(mFpsAction);
    mOptionMenu->addAction(mOrigAction);

    mSelectMenu = menuBar()->addMenu(tr("&Select"));
    mSelectMenu->addAction(mDeselectAction);
    mSelectMenu->addAction(mCycleAction);
    mSelectMenu->addAction(mSelectMesh);


    // Setup the application's widget collection
    QGridLayout * layout = new QGridLayout();

    QWidget * mainWidget = new QWidget();
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);

    // Add renderer
    layout->addWidget(renderer);
    renderer->setMinimumSize(600, 600);

    // Setup the mode label
    modeLabel = new QLabel(this);
    modeLabel->setMaximumHeight(40);

    layout->addWidget(modeLabel);
    //layout->setAlignment(modeLabel, Qt::AlignBottom);
    modeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    modeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    updateModelLabel();
}

// helper function for creating actions
void Window::createActions()
{
    // Quits the application
    mQuitAction = new QAction(tr("&Quit"), this);
    mQuitAction->setStatusTip(tr("Quits the application"));
    connect(mQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // file loading menu group
    mLoadGroup = new QActionGroup(this);
    connect(mLoadGroup, SIGNAL(triggered(QAction *)), this, SLOT(load(QAction *)));

    // open height map
    mLoadHeightMapAction = new QAction(tr("&Open Height Map"), this);
    mLoadHeightMapAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    mLoadHeightMapAction->setStatusTip(tr("Loads a heightMap"));
    mLoadGroup->addAction(mLoadHeightMapAction);

    // make tree
    mLoadTreeAction = new QAction(tr("Create Tree"), this);
    mLoadTreeAction->setStatusTip(tr("Create a tree"));
    mLoadGroup->addAction(mLoadTreeAction);

    // open texture
    mLoadTextureAction = new QAction(tr("&Load Texture"), this);
    mLoadTextureAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    mLoadTextureAction->setStatusTip(tr("Loads a texture"));
    mLoadGroup->addAction(mLoadTextureAction);

    // reset models
    mResetModelsAction = new QAction(tr("&Reset Models"), this);
    mResetModelsAction->setStatusTip(tr("Reset models to their default positions"));
    connect(mResetModelsAction, SIGNAL(triggered()), renderer, SLOT(resetModels()));

    // reset view
    mResetViewAction = new QAction(tr("&Reset View"), this);
    mResetViewAction->setStatusTip(tr("Reset camera view"));
    connect(mResetViewAction, SIGNAL(triggered()), renderer, SLOT(resetView()));

    // reset all
    mResetAllAction = new QAction(tr("&Reset All"), this);
    mResetAllAction->setStatusTip(tr("Resets models and camera"));
    connect(mResetAllAction, SIGNAL(triggered()), renderer, SLOT(resetAll()));

    // clear models
    mClearAction = new QAction(tr("&Clear"), this);
    mClearAction->setShortcut(QKeySequence(Qt::Key_C));
    mClearAction->setStatusTip(tr("Clears all models"));
    connect(mClearAction, SIGNAL(triggered()), this, SLOT(clearModels()));

    // option mode menu group
    mOptionGroup = new QActionGroup(this);
    connect(mOptionGroup, SIGNAL(triggered(QAction *)), this, SLOT(setControlMode(QAction *)));
    mOptionGroup->setExclusive(true);

    // Sets drawing mode to wireframe
    mFpsAction = new QAction(tr("&FPS"), this);
    mFpsAction->setStatusTip(tr("FPS controls"));
    mFpsAction->setCheckable(true);
    mFpsAction->setChecked(true);
    mOptionGroup->addAction(mFpsAction);

    mOrigAction = new QAction(tr("&Original"), this);
    mOrigAction->setStatusTip(tr("Original controls"));
    mOrigAction->setCheckable(true);
    mOptionGroup->setExclusive(true);
    mOptionGroup->addAction(mOrigAction);

    // draw mode menu group
    mDrawGroup = new QActionGroup(this);
    connect(mDrawGroup, SIGNAL(triggered(QAction *)), this, SLOT(setDrawMode(QAction *)));
    mDrawGroup->setExclusive(true);

    // Sets drawing mode to wireframe
    mWireAction = new QAction(tr("&Wireframe"), this);
    mWireAction->setStatusTip(tr("Wireframe mode"));
    mDrawGroup->addAction(mWireAction);

    // face drawing mode
    mFaceAction = new QAction(tr("&Face"), this);
    mFaceAction->setStatusTip(tr("Face mode"));
    mDrawGroup->addAction(mFaceAction);

    // textured drawing mode
    mTextureAction = new QAction(tr("&Textured"), this);
    mTextureAction->setStatusTip(tr("Textured mode"));
    mDrawGroup->addAction(mTextureAction);

    // select model mode
    mSelectModelAction = new QAction(tr("&Select Model"), this);
    mSelectModelAction->setStatusTip(tr("Select model"));
    mSelectModelAction->setEnabled(false);

    mSelectGroup = new QActionGroup(this);
    connect(mSelectGroup, SIGNAL(triggered(QAction *)), this, SLOT(selectModel(QAction *)));

    mDeselectAction = new QAction(tr("&Deselect"), this);
    mDeselectAction->setStatusTip(tr("Select model"));
    connect(mDeselectAction, SIGNAL(triggered()), renderer, SLOT(deselectModel()));
    connect(mDeselectAction, SIGNAL(triggered()), this, SLOT(updateModelLabel()));  // also trigger label update

    mCycleAction = new QAction(tr("&Cycle"), this);
    mCycleAction->setShortcut(QKeySequence(Qt::Key_Plus));
    mCycleAction->setStatusTip(tr("Select model"));
    connect(mCycleAction, SIGNAL(triggered()), renderer, SLOT(cycleModel()));
    connect(mCycleAction, SIGNAL(triggered()), this, SLOT(updateModelLabel()));     // also trigger label update


    mSelectMesh = new QAction(tr("&Select Mesh"), this);
    mSelectMesh->setShortcut(QKeySequence(Qt::Key_M));
    mSelectMesh->setStatusTip(tr("Select Control Points on the Mesh"));
    connect(mSelectMesh, SIGNAL(triggered()), renderer, SLOT(selectMesh()));

    // set draw menu items to exclusive
    mWireAction->setCheckable(true);
    mFaceAction->setCheckable(true);
    mTextureAction->setCheckable(true);
    mFaceAction->setChecked(true);      // face mode on by default

    // normals nested menu
    mNormalsGroup = new QActionGroup(this);
    connect(mNormalsGroup, SIGNAL(triggered(QAction *)), this, SLOT(setNormalMode(QAction *)));

    // face normal mode
    mNormalFaceAction = new QAction(tr("&Face"), this);
    mNormalFaceAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    mNormalFaceAction->setStatusTip(tr("Face normals mode"));
    mNormalsGroup->addAction(mNormalFaceAction);

    // vertex normal mode
    mNormalVertAction = new QAction(tr("&Vertex"), this);
    mNormalVertAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    mNormalVertAction->setStatusTip(tr("Vertex normals mode"));
    mNormalsGroup->addAction(mNormalVertAction);

    // display normals mode
    mShowNormalAction = new QAction(tr("&Normals"), this);
    mShowNormalAction->setShortcut(QKeySequence(Qt::Key_N));
    mShowNormalAction->setStatusTip(tr("Show normal indicators"));
    connect(mShowNormalAction, SIGNAL(triggered()), renderer, SLOT(toggleNormals()));
    mShowNormalAction->setCheckable(true);

    // set normals menu items to exclusive
    mNormalFaceAction->setCheckable(true);
    mNormalVertAction->setCheckable(true);
    mNormalVertAction->setChecked(true);
}

// destructor
Window::~Window()
{
    delete renderer;
}

void Window::updateModelLabel()
{
    QString draw = renderer->getDrawMode();
    QString normal = renderer->getNormalMode();
    QString model = renderer->getSelectedModel();
    modeLabel->setText(draw + normal + model);
}

// Sets the draw mode
void Window::setDrawMode(QAction * action)
{
    if (action == mWireAction)
    {
        renderer->setDrawMode(Renderer::WIRE);
    }
    else if (action == mFaceAction)
    {
        renderer->setDrawMode(Renderer::FACES);
    }
    else
    {
        renderer->setDrawMode(Renderer::TEXTURED);
    }
    updateModelLabel();
}

// Sets the control mode
void Window::setControlMode(QAction * action)
{
    if (action == mFpsAction)
    {
        renderer->setControlMode(Renderer::FPS);
    }
    else
    {
        renderer->setControlMode(Renderer::ORIG);
    }
}

// Set the normal mode
void Window::setNormalMode(QAction * action)
{
    if (action == mNormalVertAction)
    {
        renderer->setNormalMode(Renderer::VERTEX);
    }
    else
    {
        renderer->setNormalMode(Renderer::SURFACE);
    }
    updateModelLabel();
}

void Window::addToSelectMenu(QString name)
{
    QAction *menuAction;

    int shortcutKey = mSelectActions.size();
    menuAction = new QAction(name, this);

    if (shortcutKey <= 8)           // prevent any weird shortcut keys
        menuAction->setShortcut(QKeySequence(Qt::Key_1 + shortcutKey));
    menuAction->setStatusTip(tr("Select model"));
    mSelectGroup->addAction(menuAction);
    mSelectMenu->addAction(menuAction);
    mSelectActions.push_back(menuAction);
}

void Window::clearModels()
{
    renderer->clearModels();

    while (!mSelectActions.empty())     // remove all select model menu items
    {
        std::vector<QAction*>::iterator it = mSelectActions.end();
        it--;
        QAction *action = (*it);
        mSelectMenu->removeAction(action);
        delete((*it));                      // free memory
        mSelectActions.erase(it);
    }
    updateModelLabel();
}

void Window::selectModel(QAction * action)
{
    int idx = 0;
    for(std::vector<QAction*>::iterator it = mSelectActions.begin(); it != mSelectActions.end(); ++it)
    {
        if (action == *it)
            break;
        idx++;
    }
    renderer->selectModel(idx);
    updateModelLabel();
}

void Window::load(QAction * action)
{
    if(action == mLoadTreeAction)
    {
        Tree t = Tree(1.5, 1, 0.04);

        ObjModel *obj = t.getObjModel(0.5, 0.1, 0.01);
        Model *m_model = new Model(obj, NULL);   // NULL = no parent
        renderer->setModel(obj);
    }

    if(action == mLoadHeightMapAction)
    {

        {
            //LoadModel
            QString filename = QFileDialog::getOpenFileName(this, tr("Open Texture"), "./", tr("Obj Files (*.png *.xpm *.jpg)"), 0, QFileDialog::DontUseNativeDialog);

            if (filename == "")
                return;

            QImage *i = new QImage();
            i->load(filename);

            Terrain *terrain = renderer->createTerrain(i);

            ObjModel *obj = terrain->getObjModel();
            Model *m_model = new Model(obj, NULL);   // NULL = no parent
            renderer->setModel(obj);
        }
    }

    if (action == mLoadTextureAction)
    {
        //LoadModel
        QString filename = QFileDialog::getOpenFileName(this, tr("Open Texture"), "./", tr("Obj Files (*.png *.xpm *.jpg)"), 0, QFileDialog::DontUseNativeDialog);

        if (filename == "")
            return;

        QImage *i = new QImage();
        i->load(filename);

        renderer->setModelTexture(i);
        setDrawMode(mTextureAction);
    }
    renderer->setKeyReleased(Qt::Key_Control);  // turn these off since if we used ctrl key to open it and let go during the loading
    renderer->setKeyReleased(Qt::Key_Shift);    // menu, the program wont detect that
    updateModelLabel();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    renderer->setKeyPressed(event->key());
    updateModelLabel();
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    renderer->setKeyReleased(event->key());
    updateModelLabel();
}
