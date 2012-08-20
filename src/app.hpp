#ifndef APP_H
#define APP_H

#include <QObject>
#include "MainScreen.hpp"
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/NavigationPaneProperties>

/*!
 * @brief Application GUI object
 */
class App : public QObject
{
    Q_OBJECT
public:
    App();
    ~App();
private:
    MainScreen* _main_screen;

    // A navigation pane is used to navigate between the list and the actual recipes.


};

#endif // ifndef APP_H
