/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Window - Fundamental GUI for interacting within the scene
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QFileDialog>
#include <QLabel>
#include "objModel.h"
#include "revsurface.h"

class Renderer;

class Window : public QMainWindow
{
    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Window(QWidget *parent = 0);

    // destructor
    ~Window();


private slots:
    void load(QAction * action);
    void setDrawMode(QAction *action);
    void setNormalMode(QAction *action);
    void clearModels();
    void selectModel(QAction * action);
    void updateModelLabel();

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QAction * mQuitAction;

    QAction * mResetModelsAction;
    QAction * mResetViewAction;
    QAction * mResetAllAction;
    QAction * mClearAction;

    QActionGroup * mLoadGroup;
    QAction * mLoadHeightMapAction;
    QAction * mLoadModelAction;
    QAction * mLoadTextureAction;

    QActionGroup * mDrawGroup;
    QMenu * mDrawMenu;
    QAction * mWireAction;
    QAction * mFaceAction;
    QAction * mTextureAction;
    QAction * mNormalFaceAction;
    QAction * mNormalVertAction;
    QAction * mShowNormalAction;
    QAction * mSelectModelAction;

    QMenu * mNormalsMenu;
    QActionGroup * mNormalsGroup;

    QMenu * mSelectMenu;
    QActionGroup * mSelectGroup;
    QAction * mDeselectAction;
    QAction * mCycleAction;
    QAction * mSelectMesh;

    vector<QAction*> mSelectActions;

    QLabel *modeLabel;

    // helper function for creating actions
    void createActions();
    void addToSelectMenu(QString name);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // WINDOW_H
