/*
 * NfcWorker.hpp
 *
 *  Created on: 2012-08-25
 *      Author: user
 */

#ifndef NFCWORKER_HPP_
#define NFCWORKER_HPP_

#include <QByteArray>
#include <bps/bps.h>

#define CHECK(rc) NfcWorker::checkReturnCode((rc), __LINE__, __FILE__, __PRETTY_FUNCTION__)
#include <QtCore/QObject>
#include <QMutex>
#include <QVariantMap>
#include <bb/cascades/QmlDocument>
#include <bps/event.h>
#include <nfc/nfc.h>
#include <nfc/nfc_bps.h>

class NfcWorker: public QObject
{
	Q_OBJECT
public:
	virtual ~NfcWorker();
	enum CustomEventType {
			TERMINATE, WAKEUP
	};

signals:
		void message(const QVariant &message);
public slots:
	void startReadEventLoop();
	void startWriteEventLoop();
	void initialize();
	void listen(int);
	void interruptBpsWaitLoop(unsigned int);

public:
	static NfcWorker* getInstance();
private:
	NfcWorker(QObject *parent = 0);
	void handleNfcEvent(bps_event_t*,int);
	static void checkReturnCode(int , int , const char*,const char* );
	void handleNfcReadNdefTagEvent(bps_event_t*);
	void handleNfcWriteUriTagEvent(bps_event_t *event);
	static NfcWorker* _instance;
	const int BPS_EVENT_TIMEOUT;
	int _bpsInterruptDomain;
	int _bpsChannel;
	bool _failedToInitialize;
	bool _timeToDie;
	QMutex _interruptMutex;
};

#endif /* NFCWORKER_HPP_ */



