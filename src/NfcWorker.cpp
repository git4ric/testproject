/*
 * NfcWorker.cpp
 *
 *  Created on: 2012-08-25
 *      Author: user
 */
#include "NfcWorker.hpp"

#include <QDebug>
#include <QByteArray>
#include <bps/bps.h>
#include <nfc/nfc_bps.h>
#include <sys/time.h>

//#include <QtNfcSubset/qndefmessage.h>
//#include <QtNfcSubset/qndefrecord.h>

NfcWorker* NfcWorker::_instance;

/*
 * BPS_EVENT_TIMEOUT(-1)   == no timeout on BPS blocking waits
 * BPS_EVENT_TIMEOUT(3000) ==  3 seconds timeout on BPS blocking waits
 */

NfcWorker::NfcWorker(QObject *parent) :
		QObject(0), BPS_EVENT_TIMEOUT(-1), _failedToInitialize(false), _timeToDie(
				false){
}
NfcWorker::~NfcWorker() {
	qDebug() << "XXXX NfcWorker destructor";
	_timeToDie = true;
	_instance = 0;
	CHECK(nfc_unregister_tag_readerwriter());
}
NfcWorker* NfcWorker::getInstance() {
	if (_instance == 0) {
		_instance = new NfcWorker;
	}
	return _instance;
}
void NfcWorker::startReadEventLoop() {
	initialize();
	listen(0);
}

void NfcWorker::startWriteEventLoop(){
	initialize();
	listen(1);
}
/*
 * This method initialises the interface to BPS
 */
void NfcWorker::initialize() {

	qDebug() << "XXXX NfcWorker::initialize() starts...";

	int rc = BPS_FAILURE;

	qDebug() << ": bps_initialize() - entering";
	rc = bps_initialize();
	qDebug() << ": bps_initialize() - exited";

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: BPS failed to initialise. rc=" << rc;
		emit message(QString("BPS failed to initialise. rc=%1").arg(rc));
	} else {
		qDebug() << "XXXX BPS Initialised";
	}

	qDebug() << ": nfc_request_events() - entering";
	rc = nfc_request_events();
	qDebug() << ": nfc_request_events() - exited";

	if (rc) {
		_failedToInitialize = true;
		qDebug() << "XXXX Error: Failed to request NFC BPS events. rc=" << rc;
		emit message("Failed to request NFC BPS events");
		bps_shutdown();
	} else {
		qDebug() << "XXXX Registered for NFC BPS events OK";
		emit message("Registered for NFC BPS events OK");
		_interruptMutex.lock();
		_bpsInterruptDomain = bps_register_domain();

		if (_bpsInterruptDomain == -1) {
			qDebug() << "XXXX Failed to register an interrupt domain.";
			emit message("Failed to register an interrupt domain");
		}

		_bpsChannel = bps_channel_get_active();
		_interruptMutex.unlock();

	}
	qDebug() << "XXXX NfcWorker::initialize() ends...";
}
/*
 * @param doWhat: 0 for reading, 1 for Writing
 * This method encapsulated the main BPS event loop -- all the details needed to
 * set it in place have been done in the initialize() method
 */
void NfcWorker::listen(int doWhat) {
	qDebug() << "XXXX NfcWorker::listen() entered ...";

	int rc, domain;

	if (_failedToInitialize) {
		qDebug()
				<< "XXXX NfcWorker::listen() terminating due to initialisation failure";
		emit message("Terminating due to initialisation failure");
		return;
	}

	_timeToDie = false;

	qDebug() << "XXXX NfcWorker::listen() entering event loop";

	while (!_timeToDie) {
		bps_event_t *event;

		qDebug() << ": bps_get_event() - entering";
		rc = bps_get_event(&event, BPS_EVENT_TIMEOUT);
		qDebug() << ": bps_get_event() - exited";

		if (!rc) {
			if (event) {
				domain = bps_event_get_domain(event);
				if (nfc_get_domain() == domain) {
					//Un-implemented portion
					handleNfcEvent(event,doWhat);
				} else if (_bpsInterruptDomain == domain) {
					unsigned int code;
					code = bps_event_get_code(event);

					switch (code) {
					case NfcWorker::TERMINATE:
						_timeToDie = true;
						emit message("Got a custom BPS event to terminate");
						qDebug() << "XXXX Got a custom BPS event to terminate.";
						break;

					case NfcWorker::WAKEUP:
						emit message("Got a custom BPS event to wake up");
						qDebug() << "XXXX Got a custom BPS event to wake up.";
						break;

					default:
						emit message("Unrecognised custom BPS event");
						qDebug() << "XXXX Unrecognised custom BPS event.";
						break;
					}

				} else {
					emit message("BPS event didn't match a wanted domain");
					qDebug() << "XXXX BPS event didn't match a wanted domain";
				}
			} else {
				qDebug() << "XXXX No events from BPS on this wakeup";
			}
		} else {
			qDebug() << "XXXX NfcWorker::listen() bps_get_event rc=" << rc;
			emit message("Error getting BPS event");
			qDebug() << "XXXX Error getting BPS event.";
		}
	}

	//
	// NOTE: no need to call nfc_unregister_tag_readerwriter() since it gets
	//       called on our behalf automatically if needed.
	//
	qDebug() << ": nfc_stop_events() - entering";
	rc = nfc_stop_events();
	qDebug() << ": nfc_stop_events() - exited";

	qDebug() << ": bps_shutdown() - entering";
	bps_shutdown();
	qDebug() << ": bps_shutdown() - exited";

	qDebug() << "XXXX NfcWorker::listen() BPS shutdown. Exiting listen()";
}
/*
 * This method is used to inject a custom BPS event into the BPS event handler using
 * the channel and domain that we saved during the initialisation process.
 */
void NfcWorker::interruptBpsWaitLoop(unsigned int code) {
	int rc = 0;
	if (!_timeToDie) {
		qDebug() << "XXXX Trying to interrupt...";

		_interruptMutex.lock();
		bps_event_t *interruptEvent;

		rc = 0;
		qDebug() << ": bps_event_create() - entering";
		rc = bps_event_create(&interruptEvent, _bpsInterruptDomain, code, 0, 0);
		qDebug() << ": bps_event_create() - exited";

		if (rc) {
			qDebug() << "XXXX Unable to create a BPS custom event";
		} else {
			qDebug() << "XXXX Created a BPS custom event";
		}

		rc = 0;
		qDebug() << ": bps_channel_push_event() - entering";
		rc = bps_channel_push_event(_bpsChannel, interruptEvent);
		qDebug() << ": bps_channel_push_event() - exited";

		if (rc) {
			qDebug() << "XXXX Unable to push custom event onto BPS channel";
		} else {
			qDebug() << "XXXX Pushed custom event onto BPS channel";
		}

		_interruptMutex.unlock();
	} else {
		qDebug()
				<< "XXXX No need to interrupt NfcWorker since already closing down";
	}
}
void NfcWorker::checkReturnCode(int rc, int line, const char *file,
		const char *func) {
	if (rc != BPS_SUCCESS) {
		qDebug() << "XXXX Error code " << rc << " in function " << func
				<< " on line " << line << " in " << file;
	}
}
/*
 * handles what to do with nfc event- Read or Write
*/
void NfcWorker::handleNfcEvent(bps_event_t *event,int doWhat) {
	emit message("Handling an NFC event");
	qDebug() << "XXXX Handling an NFC event";
	qDebug() << "XXXX Handling an NFC event in READ_NDEF_TAG state";

	// register for NDEF tags(4)
	CHECK(nfc_register_tag_readerwriter(4));

	if(doWhat == 0){
		handleNfcReadNdefTagEvent(event);
	}
	if(doWhat == 1){
		handleNfcWriteUriTagEvent(event);
	}
}

/*
 * This method processed an NFC Event when we're intending to read an NDEF Tag
*/
void NfcWorker::handleNfcReadNdefTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	nfc_event_t *nfcEvent;

	qDebug()
			<< "XXXX NfcWorker::handleNfcReadNdefTagEvent - processing event code "
			<< code;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug()
				<< "XXXX NfcWorker::handleNfcReadNdefTagEvent - NDEF read event";

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		qDebug()
				<< "XXXX NfcWorker::handleNfcReadNdefTagEvent - got NFC event object";

		nfc_target_t* target;
		CHECK(nfc_get_target(nfcEvent, &target));
		qDebug() << "XXXX NfcWorker::handleNfcReadNdefTagEvent - got target";

		int ndefMsgCount = 0;
		CHECK(nfc_get_ndef_message_count(target, &ndefMsgCount));
		qDebug()
				<< "XXXX NfcWorker::handleNfcReadNdefTagEvent - target message count="
				<< ndefMsgCount;

		for (int ndefMsgIndex = 0; ndefMsgIndex < ndefMsgCount;
				++ndefMsgIndex) {

			// Now processing an an NDEF message

		//	qDebug() << "XXXX Processing NDEF Message index: " << ndefMsgIndex;

		//	nfc_ndef_message_t *ndefMessage;
		//	CHECK(nfc_get_ndef_message(target, ndefMsgIndex, &ndefMessage));

			//TODO
			// Parse the ndef message!!
			CHECK(nfc_destroy_target(target));
		}
	} else {
		qDebug()
				<< "XXXX NfcWorker::handleNfcReadNdefTagEvent - NFC BPS event that we didn't register for: "
				<< code;
	}
}
void NfcWorker::handleNfcWriteUriTagEvent(bps_event_t *event) {
	uint16_t code = bps_event_get_code(event);
	qDebug()
			<< "XXXX NfcWorker::handleWriteUriTagEvent - processing event code "
			<< code;

	nfc_event_t *nfcEvent;
	nfc_target_t* target;
	nfc_ndef_message_t* myNdefMessage;

	if (NFC_TAG_READWRITE_EVENT == code) {
		qDebug()
				<< "XXXX NfcWorker::handleWriteUriTagEvent - Target Read Write Event";

		CHECK(nfc_get_nfc_event(event, &nfcEvent));
		CHECK(nfc_get_target(nfcEvent, &target));

		qDebug()
				<< "XXXX NfcWorker::handleWriteUriTagEvent - Preparing to write URI: ";

//--------- Try using C APIs----------------------------
		QString uri("www.google.ca");
		nfc_ndef_record_t* record = 0;
		int len = uri.length();
		uchar_t payload[len + 1];
		memcpy(&payload[0], uri.toUtf8().constData(), len);
		CHECK(nfc_create_ndef_record(NDEF_TNF_WELL_KNOWN, "U", payload, len + 1, 0, &record));
		CHECK(nfc_create_ndef_message(&myNdefMessage));
		CHECK(nfc_add_ndef_record(myNdefMessage,record));
		qDebug() << "Before Writing";

		bool status;
		status = true;
		CHECK(nfc_tag_is_writable(target,&status));

		CHECK(nfc_write_ndef_message_to_tag(target,myNdefMessage, true));
		qDebug() << "After Writing";
		CHECK(nfc_delete_ndef_message(myNdefMessage, true));
		CHECK(nfc_destroy_target(target));
	} else {
		qDebug()
				<< "XXXX NfcWorker::handleWriteUriTagEvent - NFC BPS event that we didn't register for: "
				<< code;
	}
}





