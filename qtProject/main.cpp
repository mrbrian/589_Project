/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Entry point for execution into the program
 */

#include "window.h"
#include <QApplication>
#include <QVector2D>
#include "tests.h"

int main(int argc, char *argv[])
{
    Tests t;

    QApplication a(argc, argv);
    Window w;
    w.show();

    return a.exec();
}
