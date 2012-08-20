/*
 * MainScreen.hpp
 *
 *  Created on: 2012-08-18
 *      Author: user
 */

#ifndef MAINSCREEN_HPP_
#define MAINSCREEN_HPP_

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/ImageTracker>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ActionItem>
#include <bb/cascades/resources/ResourceTracker.h>
#include <QUrl>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/ActionItem>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/DockLayout>
#include <bb/cascades/DockLayoutProperties>
#include <bb/cascades/ImagePaint>
#include <bb/cascades/TitleBar>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
class MainScreen: public QObject
{
	Q_OBJECT
public:
	MainScreen();
	~MainScreen();
	void createScreen();
	bb::cascades::Page* loadImage();
	bb::cascades::Page* createSecondPage();
	void setMenu();
	bb::cascades::Page* getRoot();
	void addItems();
public slots:
	void onStateChanged(bb::cascades::ResourceTracker::State);
	void onClicked();
private:
	bb::cascades::NavigationPane *mNavPane;
	bb::cascades::Container* _container;
	bb::cascades::Page* _main_page;
	bb::cascades::ImageView* _main_image;
	bb::cascades::ImageTracker* _tracker;
	bb::cascades::ActionSet* _actionSet;
	bb::cascades::Page *secondPage;

};


#endif /* MAINSCREEN_HPP_ */
