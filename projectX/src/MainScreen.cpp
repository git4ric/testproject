/*
 * MainScreen.cpp
 *
 *  Created on: 2012-08-18
 *      Author: user
 */
#include "MainScreen.hpp"

using namespace bb::cascades;

MainScreen::MainScreen(){
	_main_page = Page::create();
	_container = Container::create();
	_main_image = new ImageView();

	Button* loadMenu = Button::create();
	loadMenu->setText("Long press here");
	QObject::connect(loadMenu,SIGNAL(clicked()),this,SLOT(onClicked()));
	loadImage();

	_main_page->setContent(_container);
}
MainScreen::~MainScreen(){

}
Page* MainScreen::getRoot(){
	return _main_page;
}
void MainScreen::loadImage(){
	_tracker = new ImageTracker(QUrl("assets/PRESTO_card.png"));
	QObject::connect(_tracker,SIGNAL(stateChanged(bb::cascades::ResourceTracker::State)),this,SLOT(onStateChanged(bb::cascades::ResourceTracker::State)));
}
void MainScreen::setMenu(){
	Container* contextContainer = Container::create();
	_actionSet = ActionSet::create().title("Context menu").subtitle("Select an action.");
	addItems();
	contextContainer->addActionSet(_actionSet);
	_container->add(contextContainer);
}
void MainScreen::onStateChanged(bb::cascades::ResourceTracker::State state){
	if(state == ImageTracker::Loaded)
	{
		_main_image->setImage(_tracker->image());
		_main_image->setPreferredSize(768,1280);
		_container->add(_main_image);
	}
}
void MainScreen::onClicked(){
	setMenu();
}
void MainScreen::addItems(){
	ActionItem* action1 = ActionItem::create().title("Tap!");
	ActionItem* action2 = ActionItem::create().title("Reload");
	ActionItem* action3 = ActionItem::create().title("Help");
	_actionSet->add(action1);
	_actionSet->add(action2);
	_actionSet->add(action3);
}
