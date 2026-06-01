#include "ControlWidget.h"


#include <QSpinBox>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QSizePolicy>
#include <QLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>

#include "SLDefinition.h"


ControlWidget::ControlWidget(QWidget *parent)
: QWidget(parent)
{
	createWidgets();
	createLayout();
	createConnections();
	setFixedSize(minimumSizeHint());
}

void ControlWidget::InitializeSetting() {

	_sliderPjkScaleOL->setValue(10);
	_sliderPjkScaleOA->setValue(10);
	_sliderPjkScaleL->setValue(10);
	_sliderPjkScaleA->setValue(10);
	_checkShowStat->setChecked(false);
	_checkShowFlow->setChecked(false);
}

void ControlWidget::createWidgets()
{
// 	xSpinBox = new QSpinBox;
// 	xSpinBox->setRange(0, 1024);
// 	xSpinBox->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
// 
// 	ySpinBox = new QSpinBox;
// 	ySpinBox->setRange(0, 1024);
// 	ySpinBox->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

	_sbST = new QSpinBox;
	_sbST->setRange(0, 1000);
	_sbST->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

	_sbET = new QSpinBox;
	_sbET->setRange(0, 1000);
	_sbET->setAlignment(Qt::AlignVCenter | Qt::AlignRight);


	_sbAThreshold = new QSpinBox;
	_sbAThreshold->setRange(1, 5);
	_sbAThreshold->setValue(1);
	_sbAThreshold->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

	_lbANum = new QLabel("0");
	_lbLNum = new QLabel("0");


	_sbTRange = new QSpinBox;
	_sbTRange->setRange(0, 1000);
	_sbTRange->setAlignment(Qt::AlignVCenter | Qt::AlignRight);



	_cbboxWeights = new QComboBox;
	_cbboxWeights->addItem("AMD", WeightMethod_0);
	_cbboxWeights->addItem("Weight 1", WeightMethod_1);
	_cbboxWeights->addItem("APD", WeightMethod_2);
	_cbboxWeights->addItem("Weight 3", WeightMethod_3);
	_cbboxWeights->setCurrentIndex(0);



	_textTips = new QTextEdit();
	//_pTips->setText("Aslan's Camp");
	//_pTips->setDisabled(true);
	_textTips->setReadOnly(true);
	_textTips->setMaximumHeight(10000000);
	_textTips->setFixedHeight(430);
	//_pTips->setSizePolicy(QSizePolicy::Expanding);

	_sbSW = new QSpinBox;
	_sbSW->setRange(1, 10);
	_sbSW->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_sbSW->setEnabled(false);


	_sbLW = new QSpinBox;
	_sbLW->setRange(1, 10);
	_sbLW->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	_sbLW->setEnabled(false);



	_radioOverview = new QRadioButton();
	_radioOverview->setText("Overview");
	_radioOverview->setChecked(false);
	_radioDetail = new QRadioButton();
	_radioDetail->setText("Detail");
	_radioDetail->setChecked(true);

	_checkShowFlow = new QCheckBox();
	_checkShowFlow->setText("Show Flow");
	_checkShowFlow->setChecked(true);

	_checkShowStat = new QCheckBox();
	_checkShowStat->setText("Show Statistics");
	_checkShowStat->setChecked(true);

	_sliderPjkScaleL = new QSlider(Qt::Horizontal);
	_sliderPjkScaleL->setToolTip("Location Projection Scale");
	_sliderPjkScaleL->setRange(1, 10);
	_sliderPjkScaleL->setValue(1);


	_sliderPjkScaleA = new QSlider(Qt::Horizontal);
	_sliderPjkScaleA->setToolTip("Actor Projection Scale");
	_sliderPjkScaleA->setRange(1, 10);
	_sliderPjkScaleA->setValue(1);

	_sliderPjkScaleOL = new QSlider(Qt::Horizontal);
	_sliderPjkScaleOL->setToolTip("Overview Location Projection Scale");
	_sliderPjkScaleOL->setRange(1, 10);
	_sliderPjkScaleOL->setValue(1);


	_sliderPjkScaleOA = new QSlider(Qt::Horizontal);
	_sliderPjkScaleOA->setToolTip("Overview Actor Projection Scale");
	_sliderPjkScaleOA->setRange(1, 10);
	_sliderPjkScaleOA->setValue(1);


	_btnFocusAs = new QPushButton;
	_btnFocusAs->setText("Focus Actors");


	_btnFocusLs = new QPushButton;
	_btnFocusLs->setText("Focus Locations");

}

void ControlWidget::createLayout()
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	{
		QGroupBox* pG = new QGroupBox("Data");
		QFormLayout* pL = new QFormLayout;
		pG->setLayout(pL);
		pL->addRow(tr("Default:"), _sbTRange);

		pL->addRow(tr("Start:"), _sbST);
		pL->addRow(tr("End:"), _sbET);
		pL->addRow(tr("A Threshold:"), _sbAThreshold);
		//pL->addRow(tr("Locations:"), locationSpinBox);
		//pL->addRow(tr("L Depth:"), locationDepthSpinBox);
		pL->addRow(tr("Actors:"), _lbANum);
		pL->addRow(tr("Locations:"), _lbLNum);

		vLayout->addWidget(pG);
	}


	{
		QGroupBox* pG = new QGroupBox("Operation");


		QFormLayout* pL = new QFormLayout;
		pG->setLayout(pL);

		vLayout->addWidget(pG);

		pL->addRow(tr("Session Weight:"), _sbSW);
		pL->addRow(tr("Location Weight:"), _sbLW);
		pL->addRow(tr("Dis. Metric:"), _cbboxWeights);
//		pL->addRow(tr("Location Depth:"), _spinLD);
		{
			QGroupBox* group = new QGroupBox();
			QHBoxLayout* layout = new QHBoxLayout();
			group->setLayout(layout);
			layout->addWidget(_radioDetail);
			layout->addWidget(_radioOverview);
			pL->addRow(group);
		}
		pL->addRow(_checkShowFlow);
		pL->addRow(_checkShowStat);
		pL->addRow(_btnFocusAs);
		pL->addRow(_btnFocusLs);
		pL->addRow(tr("L Scale"), _sliderPjkScaleL);
		pL->addRow(tr("A Scale"), _sliderPjkScaleA);
		pL->addRow(tr("OL Scale"), _sliderPjkScaleOL);
		pL->addRow(tr("OA Scale"), _sliderPjkScaleOA);
		pL->addRow(_textTips);

	}


	setLayout(vLayout);
}

void ControlWidget::createConnections()
{
// 	connect(xSpinBox, SIGNAL(valueChanged(int)), this, SLOT(geometryChanged(int)));
// 	connect(ySpinBox, SIGNAL(valueChanged(int)), this, SLOT(geometryChanged(int)));
	connect(_sbST, SIGNAL(valueChanged(int)), this, SLOT(onTimeWindowChanged(int)));
	connect(_sbET, SIGNAL(valueChanged(int)), this, SLOT(onTimeWindowChanged(int)));
	connect(_sbAThreshold, SIGNAL(valueChanged(int)), this, SLOT(onActorChanged(int)));
	connect(_sbTRange, SIGNAL(valueChanged(int)), this, SLOT(onTimeRangeChanged(int)));
	connect(_cbboxWeights, SIGNAL(currentIndexChanged(int)), this, SLOT(onWeightMethodChanged(int)));



	connect(_sbSW, SIGNAL(valueChanged(int)), this, SLOT(onSWChanged(int)));
	connect(_sbLW, SIGNAL(valueChanged(int)), this, SLOT(onLWChanged(int)));



	connect(_radioOverview, SIGNAL(toggled(bool)), this, SLOT(onOverviewDetailChanged(bool)));
	connect(_checkShowFlow, SIGNAL(stateChanged(int)), this, SLOT(onFlowVisibilityChanged(int)));
	connect(_checkShowStat, SIGNAL(stateChanged(int)), this, SLOT(onStatVisibilityChanged(int)));
	connect(_sliderPjkScaleL, SIGNAL(valueChanged(int)), this, SLOT(onScaleLChanged(int)));
	connect(_sliderPjkScaleA, SIGNAL(valueChanged(int)), this, SLOT(onScaleAChanged(int)));
	connect(_sliderPjkScaleOL, SIGNAL(valueChanged(int)), this, SLOT(onScaleOLChanged(int)));
	connect(_sliderPjkScaleOA, SIGNAL(valueChanged(int)), this, SLOT(onScaleOAChanged(int)));


	connect(_btnFocusAs, SIGNAL(clicked()), this, SLOT(onFocusATriggered()));
	connect(_btnFocusLs, SIGNAL(clicked()), this, SLOT(onFocusLTriggered()));

}

void ControlWidget::SetTimeWindow(int nStart, int nEnd) {
	blockSignals(true);
	_sbST->setRange(nStart,nEnd);
	_sbST->setValue(nStart);
	_sbET->setRange(nStart, nEnd);
	_sbET->setValue(nEnd);
	blockSignals(false);
}

void ControlWidget::SetTimeRange(int nRange) {
	blockSignals(true);
	_sbTRange->setValue(nRange);
	blockSignals(false);
}



void ControlWidget::SetActor(int nActor) {
	_lbANum->setText(QString::number(nActor));
}
void ControlWidget::SetLocation(int nL) {
	_lbLNum->setText(QString::number(nL));
}

void ControlWidget::onTimeWindowChanged(int){
	emit TimeWindowChanged(_sbST->value(), _sbET->value());
}

void ControlWidget::onActorChanged(int) {
	qDebug() << "void ControlWidget::actorChanged(int) {"<< _sbAThreshold->value();
	emit ActorThresholdChanged(_sbAThreshold->value());
}

void ControlWidget::onTimeRangeChanged(int) {
	emit TimeRangeChanged(_sbTRange->value());
}

void ControlWidget::onWeightMethodChanged(int nMethod)
{
	emit WeightMethodChanged(nMethod);
}



void ControlWidget::onSWChanged(int nW) {
	emit SWChanged(nW);
}
void ControlWidget::onLWChanged(int nW) {
	emit LWChanged(nW);
}


void ControlWidget::SetTips(QString strTips) {
//	qDebug() << "void ControlWidget::SetTips(QString strTips) {";
	_textTips->setText(strTips);
}


void ControlWidget::onOverviewDetailChanged(bool bOverview) {
//	qDebug() << "void ControlWidget::onOverviewDetail() {"<< bOverview;
	emit OverviewDetailChanged(bOverview);
}

void ControlWidget::onFlowVisibilityChanged(int nState) {
//	qDebug() << "void ControlWidget::onFlowVisibilityChanged(int nState) {" << nState;
	emit FlowVisibilityChanged(nState);
}

void ControlWidget::onStatVisibilityChanged(int nState) {
	emit StatVisibilityChanged(nState);
}

void ControlWidget::onScaleLChanged(int nValue) {
	emit LScaleChanged(nValue);
}

void ControlWidget::onScaleAChanged(int nValue) {
	emit AScaleChanged(nValue);
}

void ControlWidget::onScaleOLChanged(int nValue) {
	emit OLScaleChanged(nValue);
}

void ControlWidget::onScaleOAChanged(int nValue) {
	emit OAScaleChanged(nValue);
}

void ControlWidget::SetSessionWeight(int nW) {
	if (nW>0)
	{
		_sbSW->setValue(nW);
		_sbSW->setEnabled(true);
	}
	else {
		_sbSW->setEnabled(false);
	}
}

void ControlWidget::SetLocationWeight(int nW) {
	if (nW > 0)
	{
		_sbLW->setValue(nW);
		_sbLW->setEnabled(true);
	}
	else {
		_sbLW->setEnabled(false);
	}
}



void ControlWidget::onFocusATriggered() {
	emit FocusATriggered();
}

void ControlWidget::onFocusLTriggered() {
	emit FocusLTriggered();
}