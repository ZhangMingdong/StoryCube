#include "ActorWidget.h"


#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QTabWidget>
#include <QBarSet>
#include <QHorizontalBarSeries>
#include <QBarCategoryAxis>
#include <QApplication>

using namespace std;


ActorWidget::ActorWidget(QWidget *parent)
: QWidget(parent)
{
	// create the table view
	QTabWidget* tab = new QTabWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(tab);
	setLayout(layout);

	// create the first tab
	{
		m_chart = new QChart();
		m_chartView = new QChartView(m_chart, this);
		m_chart->legend()->setVisible(true);
		m_chart->legend()->setAlignment(Qt::AlignLeft);
		m_chartView->setRenderHint(QPainter::Antialiasing); 



		// 创建滚动区域
		QScrollArea* scrollArea = new QScrollArea(this);
		scrollArea->setWidgetResizable(false); // 重要：设置为false以保持图表原始大小
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);



		// 设置图表视图的最小尺寸，确保它不会被压缩
		//m_chartView->setMinimumSize(200, 4000); // 根据您的需求调整尺寸

		// 将图表视图添加到滚动区域
		scrollArea->setWidget(m_chartView);

		// 添加到tab
		tab->addTab(scrollArea, "Vis");

		//tab->addTab(m_chartView, "Vis");
	}

	// create the 2nd tab
	{
		QWidget* w2 = new QWidget();
		_pLayoutAdjust = new QGridLayout;
		QScrollArea* pSa = new QScrollArea();
		pSa->setWidgetResizable(true);
		pSa->setMinimumWidth(170);
		pSa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		pSa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		pSa->setWidget(w2);
		w2->setLayout(_pLayoutAdjust);
		tab->addTab(pSa, "Adj");
	}
	// create the 3rd tab
	{
		QWidget* w3 = new QWidget();
		_pLayoutWeight = new QGridLayout;
		QScrollArea* pSa = new QScrollArea();
		pSa->setWidgetResizable(true);
		pSa->setMinimumWidth(170);
		pSa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		pSa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		pSa->setWidget(w3);
		w3->setLayout(_pLayoutWeight);
		tab->addTab(pSa, "Num");
	}

}

void ActorWidget::generateBarCharts() {

	// clear old data;
	m_chart->removeAllSeries();
	if (m_axisY) {
		m_chart->removeAxis(m_axisY);
		delete m_axisY;
	}

	// create new data;
	QList<double> listW = _listW;
	QList<QString> listN = _listActorNames;
	if (listW.length() > _nLimit) listW.remove(0, listW.length() - _nLimit);
	if (listN.length() > _nLimit) listN.remove(0, listN.length() - _nLimit);

	QBarSet* set0 = new QBarSet("Weight");
	for (double dbW : listW)
	{
		*set0 << dbW;
	}

	QHorizontalBarSeries* series = new QHorizontalBarSeries();
	series->append(set0);
	m_chart->addSeries(series);
	m_chart->setAnimationOptions(QChart::SeriesAnimations);
	//![3]

	//![4]
	m_axisY = new QBarCategoryAxis();
	m_axisY->append(listN);
	m_chart->addAxis(m_axisY, Qt::AlignLeft);
	series->attachAxis(m_axisY);

	m_chart->legend()->setVisible(false);



	// 根据数据量计算需要的尺寸
	int seriesCount = _listActorNames.length();

	qDebug() << seriesCount;
	int calculatedWidth = 200;
	int calculatedHeight = seriesCount * 30 + 200;

	m_chartView->setMinimumSize(calculatedWidth, calculatedHeight);
	m_chartView->setFixedSize(calculatedWidth, calculatedHeight);

	// 立即更新
	m_chartView->update();
	QApplication::processEvents();
}

void ActorWidget::generateListView() {
	// for tab3
	while (_pLayoutWeight->count() != 0) // Check this first as warning issued if no items when calling takeAt(0).
	{
		QLayoutItem* forDeletion = _pLayoutWeight->takeAt(0);
		delete forDeletion->widget();
		delete forDeletion;
	}

	int nLine = _listActorNames.length() - 1;
	int nIndex = 0;
	for (QString strA : _listActorNames)
	{
		QLabel* label = new QLabel(strA);
		QLabel* weight = new QLabel(QString::number(_listW[nIndex]));
		_pLayoutWeight->addWidget(label, nLine, 0);
		_pLayoutWeight->addWidget(weight, nLine, 1);
		nLine--;
		nIndex++;
	}
}

void ActorWidget::SetActors(vector<string> listActors, map<string, double> mapWs, map<string, double> mapWOs) {

	int nLen = listActors.size();

	// sort
	for (int i = 0; i < nLen - 1; i++)
	{
		for (int j = i + 1; j < nLen; j++)
		{
			if (mapWs[listActors[i]] > mapWs[listActors[j]]) {
				string strT = listActors[i];
				listActors[i] = listActors[j];
				listActors[j] = strT;
			}
		}
	}

	// transform to QString
	_listActorNames.clear();
	std::transform(
		listActors.begin(), listActors.end(),
		std::back_inserter(_listActorNames),
		[](const std::string& s) { return QString::fromStdString(s); }
	);

	//for (int i = 0; i < nLen; i++) qDebug() << _listActorNames[i];

	//double dbW = 1.0 / nLen;
	_listW.clear();
	_listWO.clear();
	for (size_t i = 0; i < nLen; i++)
	{
		_listW.append(mapWs[listActors[i]]);
		_listWO.append(mapWOs[listActors[i]]);
	}

	// ====================for tab1=========================
	generateBarCharts();


	// ====================for tab2=========================

	// clear old widgets
	_listSpins.clear();
	_listChecks.clear();
	while (_pLayoutAdjust->count() != 0) // Check this first as warning issued if no items when calling takeAt(0).
	{
		QLayoutItem* forDeletion = _pLayoutAdjust->takeAt(0);
		delete forDeletion->widget();
		delete forDeletion;
	}

	// createWidget
	{
		QPushButton* pBtn = new QPushButton("Select All");
		connect(pBtn, SIGNAL(clicked()), this, SLOT(selectAll()));
		_pLayoutAdjust->addWidget(pBtn, 0, 0);
	}
	{
		QPushButton* pBtn = new QPushButton("Clear All");
		connect(pBtn, SIGNAL(clicked()), this, SLOT(unselectAll()));
		_pLayoutAdjust->addWidget(pBtn, 1, 0);
	}
	int nLine = 2;
	for (int i = _listActorNames.length() - 2; i >= 0; i--)
	{
		QString strA = _listActorNames[i];
		// for tab2
		{
			QDoubleSpinBox* spinA = new QDoubleSpinBox;
			//QLabel* label = new QLabel(strA);
			QCheckBox* label = new QCheckBox(strA); label->setChecked(true);
			spinA->setRange(1, 10);
			spinA->setValue(mapWOs[strA.toStdString()]);
			spinA->setSingleStep(1);
			spinA->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			_pLayoutAdjust->addWidget(label, nLine, 0);
			_pLayoutAdjust->addWidget(spinA, nLine, 1);

			connect(spinA, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double d) {weightChanged(strA, d); });
			connect(label, QOverload<int>::of(&QCheckBox::stateChanged), [=](int s) {actorChecked(strA, s);	});

			_listSpins.append(spinA);
			_listChecks.append(label);
		}

		nLine++;
	}
	// ====================~for tab2=========================
	generateListView();
	updateGeometry();

}

void ActorWidget::unselectAll() {
	blockSignals(true);
	for (int i = 0; i < _listChecks.length(); i++)
	{
		_listChecks[i]->setChecked(false);
	}
	blockSignals(false);

	emit UnselectAllActors();
}

void ActorWidget::selectAll() {
	blockSignals(true);
	for (int i = 0; i < _listChecks.length(); i++)
	{
		_listChecks[i]->setChecked(true);
	}
	blockSignals(false);

	emit SelectAllActors();
}

void ActorWidget::weightChanged(QString strA, double dbValue) {	
//	qDebug()<<"ActorWidget::weightChanged: " << strA << ": " << dbValue;
	emit UpdateActorWeights(strA, dbValue);
}

void ActorWidget::actorChecked(QString strA, int nState) {
//	qDebug() << "ActorWidget::actorChecked: " << strA << ": " << nState;
	emit UpdateActorCheck(strA, nState);
}

void ActorWidget::ResetWeights(QMap<QString, double> mapWs) {

	int nLen = _listActorNames.length();
	// sort
	for (int i = 0; i < nLen - 1; i++)
	{
		for (int j = i + 1; j < nLen; j++)
		{
			if (mapWs[_listActorNames[i]] > mapWs[_listActorNames[j]]) {
				QString strT = _listActorNames[i];
				_listActorNames[i] = _listActorNames[j];
				_listActorNames[j] = strT;
			}
		}
	}
	//double dbW = 1.0 / nLen;
	_listW.clear();
	for (size_t i = 0; i < nLen; i++)
	{
		_listW.append(mapWs[_listActorNames[i]]);
	}

	// ====================for tab1=========================
	generateBarCharts();

	// for tab3
	generateListView();
}

void ActorWidget::ResetWeights(map<string, double> mapWs) {

	int nLen = _listActorNames.length();
	// sort
	for (int i = 0; i < nLen - 1; i++)
	{
		for (int j = i + 1; j < nLen; j++)
		{
			if (mapWs[_listActorNames[i].toStdString()] > mapWs[_listActorNames[j].toStdString()]) {
				QString strT = _listActorNames[i];
				_listActorNames[i] = _listActorNames[j];
				_listActorNames[j] = strT;
			}
		}
	}
	//double dbW = 1.0 / nLen;
	_listW.clear();
	for (size_t i = 0; i < nLen; i++)
	{
		_listW.append(mapWs[_listActorNames[i].toStdString()]);
	}

	// ====================for tab1=========================
	generateBarCharts();

	// for tab3
	generateListView();
}