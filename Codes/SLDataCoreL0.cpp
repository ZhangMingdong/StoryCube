#include "SLDataCoreL0.h"


#include <QXmlStreamReader>

#include "SLSession.h"
#include "SLLocation.h"
#include "SLActor.h"

using namespace std;


void skipUnknownElement(QXmlStreamReader& reader)
{
	reader.readNext();
	while (!reader.atEnd()) {
		if (reader.isEndElement()) {
			reader.readNext();
			break;
		}

		if (reader.isStartElement()) {
			skipUnknownElement(reader);
		}
		else {
			reader.readNext();
		}
	}
}

SLDataCoreL0::~SLDataCoreL0() {
	// 遍历 map，释放每个 SLActor* 指向的内存
	for (auto& pair : _mapA) {
		delete pair.second;  // 释放指针指向的对象
	}
	for (auto& pair : _mapL) {
		delete pair.second;  // 释放指针指向的对象
	}
	for (auto& pair : _mapS) {
		delete pair.second;  // 释放指针指向的对象
	}
}

void SLDataCoreL0::ReadStory(QXmlStreamReader& reader, int nTimeRange)
{
	_nTimeRange = nTimeRange;
	reader.readNext();					// skip <Story>
	while (!reader.atEnd()) {
		//	qDebug() << reader.name();
		if (reader.isEndElement()) {	//reach </Story>
			reader.readNext();
			break;
		}
		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Actors")) {
				readActors(reader);
			}
			else if (reader.name() == QString::fromStdString("LocationNames")) {
				readLocationNames(reader);
			}
			else if (reader.name() == QString::fromStdString("Sessions")) {
				readSessions(reader);
			}
			else {
				skipUnknownElement(reader);
			}
		}
		else {
			reader.readNext();			// skip space
		}
	}
}

void SLDataCoreL0::readLocationNames(QXmlStreamReader& reader) {

	reader.readNext();					// skip <LocationNames>
	while (!reader.atEnd()) {
		//qDebug() << reader.name();
		if (reader.isEndElement()) {	//reach </LocationNames>
			reader.readNext();
			break;
		}

		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Location")) {
				string lName = reader.attributes().value("ID").toString().toStdString();
				string lDisplayName = reader.attributes().value("Name").toString().toStdString();

				if (find(_listL.begin(),_listL.end(), lName)==_listL.end()) {
					_mapL[lName] = new SLLocation(lName);
					_listL.push_back(lName);
				}
				_mapL[lName]->SetDisplayName(lDisplayName);

				reader.readNext();	// skip <Location>
				//qDebug() << reader.name();
				reader.readNext();	// skip </Location>
				//qDebug() << reader.name();
			}
		}
		else {
			reader.readNext();			// skip space
		}
	}
}

void SLDataCoreL0::readSessions(QXmlStreamReader& reader) {

	reader.readNext();					// skip <Sessions>
	while (!reader.atEnd()) {
		//qDebug() << reader.name();
		if (reader.isEndElement()) {	//reach </Sessions>
			reader.readNext();
			break;
		}

		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Session")) {
				int nStart = reader.attributes().value("Start").toInt();
				string strL = reader.attributes().value("Location").toString().toStdString();
				string strDsp = reader.attributes().value("dsp").toString().toStdString();
				SLSession* session = new SLSession(nStart, strL, strDsp);
				if (session->GetT() <= _nTimeRange)
				{
					string strSessionId = GenerateSessionID(nStart, strL);
					_mapS[strSessionId] = session;
					_listS.push_back(strSessionId);
				}

				reader.readNext();	// skip <Session>
				reader.readNext();	// skip </Session>
			}
		}
		else {
			reader.readNext();			// skip space
		}
	}
}

void SLDataCoreL0::readActors(QXmlStreamReader& reader) {

	reader.readNext();					// skip <Actors>
	while (!reader.atEnd()) {
		if (reader.isEndElement()) {	//reach </Actors>
			reader.readNext();
			break;
		}

		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Actor")) {
				readActor(reader);
			}
		}
		else {
			reader.readNext();			// skip space
		}
	}
}

void SLDataCoreL0::readActor(QXmlStreamReader& reader) {
	string strName = reader.attributes().value("Name").toString().toStdString();
	SLActor* pA = new SLActor(strName);

	int nR = reader.attributes().value("R").toInt();
	int nG = reader.attributes().value("G").toInt();
	int nB = reader.attributes().value("B").toInt();
	if (nR > 0 || nG > 0 || nB > 0) {
		pA->SetColor(nR, nG, nB);
	}


	reader.readNext();					// skip <Actor>
	while (!reader.atEnd()) {
		//	qDebug() << reader.name();
		if (reader.isEndElement()) {	//reach </Actor>
			reader.readNext();
			break;
		}

		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Span")) {
				int nStart = reader.attributes().value("Start").toInt();
				int nEnd = reader.attributes().value("End").toInt();
				string strSession = reader.attributes().value("Location").toString().toStdString();
				bool bDefault = reader.attributes().value("Default").toString() == QString::fromStdString("true");
				bool bDeath = reader.attributes().value("Death").toString() == QString::fromStdString("true");

				if (nEnd != nStart + 1) {
					//qDebug() << "error: span more than one time step";
					//qDebug() << "=====\t" << strName;
					//qDebug() << "=====\t" << nStart;
					//qDebug() << "=====\t" << nEnd;
				}
				if (nStart <= _nTimeRange) {
					if (nStart < _nStart) _nStart = nStart;
					if (nStart >= _nEnd) _nEnd = nStart;
					pA->AddSpan(strSession, nStart, bDeath);
				}
				reader.readNext();	// skip <Span>
				reader.readNext();	// skip </Span>
			}
		}
		else {
			reader.readNext();			// skip space
		}
	}
	if (pA->IsValid()) {
		_mapA[pA->GetName()] = pA;
		_listA.push_back(pA->GetName());
	}
	else delete pA;
}
