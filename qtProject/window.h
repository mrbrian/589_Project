/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 0
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
    // Display a message box providing assistance with the line drawing function.
    void lineHelp();

private:
    // Main widget for drawing
    Renderer *renderer;

    // Menu items and actions
    QMenu * mFileMenu;
    QAction * mQuitAction;

    QMenu * mToolsMenu;

    QMenu * mHelpMenu;
    QAction * mLineHelpAction;

    // helper function for creating actions
    void createActions();

};

#endif // WINDOW_H
