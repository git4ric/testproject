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


class MainScreen: public QObject
{
	Q_OBJECT
public:
	MainScreen();
	~MainScreen();
	void createScreen();
	void loadImage();
	void setMenu();
	bb::cascades::Page* getRoot();
	void addItems();
public slots:
	void onStateChanged(bb::cascades::ResourceTracker::State);
	void onClicked();
private:
	bb::cascades::Container* _container;
	bb::cascades::Page* _main_page;
	bb::cascades::ImageView* _main_image;
	bb::cascades::ImageTracker* _tracker;
	bb::cascades::ActionSet* _actionSet;
};


#endif /* MAINSCREEN_HPP_ */
