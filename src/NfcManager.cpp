/*
 * NfcManager.cpp
 *
 *  Created on: 2012-08-25
 *      Author: user
 */
#include "NfcManager.hpp"
#include "NfcWorker.hpp"

NfcManager* NfcManager::_instance;

NfcManager::NfcManager() :
		_workerInstance(0), _bpsThread(0), _future(0), _watcher(0) {
	qDebug() << "XXXX Constructing NfcManager";
	qDebug() << "XXXX Done constructing NfcManager";
}

NfcManager::~NfcManager() {
	qDebug() << "XXXX NcfManager destructor";

	_instance = 0;

	stopNfcWorker();

	if (_future) {
		qDebug() << "XXXX NcfManager deleting QtConcurrent";
		delete _future;
		_future = 0;
	}

	if (_watcher) {
		qDebug() << "XXXX NcfManager deleting Watcher";
		delete _watcher;
		_watcher = 0;
	}

	if (_workerInstance) {
		qDebug() << "XXXX NcfManager deleting Worker";
		delete _workerInstance;
		_workerInstance = 0;
	}
}
NfcManager* NfcManager::getInstance() {
	if (_instance == 0) {
		_instance = new NfcManager;
	}
	return _instance;
}
void NfcManager::startEventProcessing() {
	qDebug() << "XXXX starting event processing...";
	_future = new QFuture<void>;
	_watcher = new QFutureWatcher<void>;
	_workerInstance = NfcWorker::getInstance();
	if(mode_type == READ){
		*_future = QtConcurrent::run(_workerInstance, &NfcWorker::startReadEventLoop);
	}
	if(mode_type == WRITE){
		*_future = QtConcurrent::run(_workerInstance, &NfcWorker::startWriteEventLoop);
	}
	_watcher->setFuture(*_future);
	QObject::connect(_watcher, SIGNAL(finished()), this, SLOT(workerStopped()));
	qDebug() << "XXXX startEventProcessing - event loop started";
}
void NfcManager::stopNfcWorker() {
	qDebug() << "XXXX About to request NfcWorker to stop";
	if (_workerInstance)
		_workerInstance->interruptBpsWaitLoop(NfcWorker::TERMINATE);
}
void NfcManager::workerStopped() {
	qDebug() << "XXXX NfcManager::workerStopped entered";
	emit nfcManagerStopped();
}





