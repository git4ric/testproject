/*
 * NfcManager.cpp
 *
 *  Created on: 2012-08-25
 *      Author: user
 */
#ifndef NFCMANAGER_HPP_
#define NFCMANAGER_HPP_

#include <bb/cascades/Application>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QThread>
#include <QFuture>
#include <QFutureWatcher>
#include <Qtconcurrentrun.h>
#include "NfcWorker.hpp"

class NfcManager: public QObject
{
	Q_OBJECT

private:
	NfcManager();

	static NfcManager* _instance;
	NfcWorker* _workerInstance;
	QThread * _bpsThread;

	QFuture<void> *_future;
	QFutureWatcher<void> *_watcher;

public:
	virtual ~NfcManager();
	static NfcManager* getInstance();
	void startEventProcessing();
	void stopNfcWorker();
	enum {
		READ = 1,
		WRITE = 2
	}mode_type;

signals:
	void nfcManagerStopped();

public slots:
	void workerStopped();
};

#endif /* NFCMANAGER_HPP_ */
