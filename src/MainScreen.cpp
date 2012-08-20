/*
 * MainScreen.cpp
 *
 *  Created on: 2012-08-18
 *      Author: user
 */
#include "MainScreen.hpp"
#include <bb/cascades/Application>

using namespace bb::cascades;

MainScreen::MainScreen(){
	TabbedPane *root = new TabbedPane;
	root->setShowTabsOnActionBar(true);

	Tab *firstTab = Tab::create().title("Test 1");
	 Page *firstTabContent = loadImage();
	 firstTab->setContent(firstTabContent);

	 Tab *secondTab = Tab::create()
	                  .title("Tab 2");
	 Page *secondTabContent = new Page;
	 secondTabContent->setContent(Label::create("This is tab 2."));
	 secondTab->setContent(secondTabContent);

	 Tab *thirdTab = Tab::create()
	                  .title("Tab 3");
	 Page *thirdTabContent = new Page;
	 thirdTabContent->setContent(Label::create("This is tab 3."));
	 thirdTab->setContent(thirdTabContent);

	 Tab *FourthTab = Tab::create()
	                  .title("Tab 4");
	 Page *FourthTabContent = new Page;
	 FourthTabContent->setContent(Label::create("This is tab 4."));
	 FourthTab->setContent(FourthTabContent);

	 Tab *fifthTab = Tab::create()
	 	                  .title("Tab 5");
	 	 Page *fifthTabContent = new Page;
	 	fifthTabContent->setContent(Label::create("This is tab 5. It should overflow..."));
	 	fifthTab->setContent(fifthTabContent);
	 root->add(firstTab);
	 root->add(secondTab);
	 root->add(thirdTab);
	 root->add(FourthTab);
	 root->add(fifthTab);
	 /*
    mNavPane = new NavigationPane();
    connect(mNavPane, SIGNAL(topChanged(bb::cascades::Page* )), this, SLOT(onTopChanged(bb::cascades::Page* )));

    Page *newPage = loadImage();

    secondPage = createSecondPage();

    mNavPane->push(newPage);
    mNavPane->push(secondPage);
	*/
   Application::setScene(root);


	//_main_page->setContent(_container);
}
MainScreen::~MainScreen(){

}
Page* MainScreen::getRoot(){
	return _main_page;
}
Page* MainScreen::loadImage(){

	 Page *page = new Page();

	    // The main app Container a DockLayout is used to align children.
	    Container *container = new Container();
	    DockLayout *layout= new DockLayout();
	    container->setLayout(layout);

	    TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title("Test Page");
	     page->setTitleBar(titleBar);

	    ImageView *backgroundImage = ImageView::create(
	            "assets//PRESTO_card.png").layoutProperties(
	            DockLayoutProperties::create().vertical(VerticalAlignment::Fill).horizontal(
	                    HorizontalAlignment::Fill));

	    container->add(backgroundImage);

	    page->setContent(container);

	    return page;
}
void MainScreen::setMenu(){
	Container* contextContainer = Container::create();
	_actionSet = ActionSet::create().title("Context menu").subtitle("Select an action.");
	addItems();
	contextContainer->addActionSet(_actionSet);
	_container->add(contextContainer);
}

Page* MainScreen::createSecondPage()
{
    Page *page = new Page();

    ActionItem* backAction = ActionItem::create();
    connect(backAction, SIGNAL(triggered()), mNavPane, SLOT(pop()));
    page->setPaneProperties(NavigationPaneProperties::create().backButton(backAction));

    TitleBar* titleBar = TitleBar::create().visibility(ChromeVisibility::Visible).title("Test Page");
    page->setTitleBar(titleBar);


    Container *content = new Container();

    ImagePaint paint(QUrl("asset://background"), RepeatPattern::XY);
    content->setBackground(paint);
    content->setLayout(DockLayout::create());

    // Adding the ContentContainer.
    page->setContent(content);

    return page;
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
