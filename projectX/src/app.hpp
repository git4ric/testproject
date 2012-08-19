#ifndef APP_H
#define APP_H

#include <QObject>
#include "MainScreen.hpp"
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
};

#endif // ifndef APP_H
