#pragma once
#include <QWidget>

class QSpinBox;
class QComboBox;
class QPushButton;
class QTextEdit;
class QRadioButton;
class QCheckBox;
class QSlider;
class QLabel;

/// <summary>
/// class of the control widget
/// </summary>
class ControlWidget : public QWidget
{
	Q_OBJECT

// ===================== public operations ==========================
public:
	explicit ControlWidget(QWidget *parent = 0);
	/// <summary>
	/// set the start and end of the time
	/// </summary>
	/// <param name="nStart"></param>
	/// <param name="nEnd"></param>
	void SetTimeWindow(int nStart, int nEnd);
	/// <summary>
	/// set number of actors
	/// </summary>
	/// <param name="nActor"></param>
	void SetActor(int nActor);
	/// <summary>
	/// set number of locations
	/// </summary>
	/// <param name="nL"></param>
	void SetLocation(int nL);
	/// <summary>
	/// set the reading time range (read from the config file)
	/// </summary>
	/// <param name="nRange"></param>
	void SetTimeRange(int nRange);
	/// <summary>
	/// set the current tips
	/// </summary>
	/// <param name="strTips"></param>
	void SetTips(QString strTips);
	/// <summary>
	/// set the current session weight
	/// </summary>
	/// <param name="nW"></param>
	void SetSessionWeight(int nW);
	/// <summary>
	/// set the current location weight
	/// </summary>
	/// <param name="nW"></param>
	void SetLocationWeight(int nW);
	/// <summary>
	/// initialize the settings
	/// </summary>
	void InitializeSetting();

// ===================== private operations ==========================
private:
	void createWidgets();
	void createLayout();
	void createConnections();

// ===================== slots ==========================
private slots:
	void onTimeWindowChanged(int);
	void onActorChanged(int);
	void onTimeRangeChanged(int);
	void onWeightMethodChanged(int nMethod);
	void onSWChanged(int nW);
	void onLWChanged(int nW);
	void onOverviewDetailChanged(bool);
	void onFlowVisibilityChanged(int);
	void onStatVisibilityChanged(int);
	void onScaleLChanged(int);
	void onScaleAChanged(int);
	void onScaleOLChanged(int);
	void onScaleOAChanged(int);
	void onFocusATriggered();
	void onFocusLTriggered();

signals:
	/// <summary>
	/// the session weight changed
	/// </summary>
	void SWChanged(int nW);
	/// <summary>
	/// the location weight changed
	/// </summary>
	/// <param name="nW"></param>
	void LWChanged(int nW);
	/// <summary>
	/// showing the overview or detail changed
	/// </summary>
	/// <param name="bOverview"></param>
	void OverviewDetailChanged(bool bOverview);
	/// <summary>
	/// the flow visibility changed
	/// </summary>
	/// <param name="nState"></param>
	void FlowVisibilityChanged(int nState);
	/// <summary>
	/// show the statistics
	/// </summary>
	/// <param name="nState"></param>
	void StatVisibilityChanged(int nState);
	/// <summary>
	/// the location scale changed
	/// </summary>
	/// <param name="nState"></param>
	void LScaleChanged(int nState);
	/// <summary>
	/// the actor scale changed
	/// </summary>
	/// <param name="nState"></param>
	void AScaleChanged(int nState);
	/// <summary>
	/// the overview location scale changed
	/// </summary>
	/// <param name="nState"></param>
	void OLScaleChanged(int nState);
	/// <summary>
	/// the overview actor scale changed
	/// </summary>
	/// <param name="nState"></param>
	void OAScaleChanged(int nState);
	/// <summary>
	/// focus on the selected actors triggered
	/// </summary>
	void FocusATriggered();
	/// <summary>
	/// focus on the selected locations triggered
	/// </summary>
	void FocusLTriggered();
	/// <summary>
	/// the time window changed
	/// </summary>
	/// <param name="nStart"></param>
	/// <param name="nEnd"></param>
	void TimeWindowChanged(int nStart, int nEnd);
	/// <summary>
	/// the actor threshold changed
	/// </summary>
	/// <param name="nActor"></param>
	void ActorThresholdChanged(int nActor);
	/// <summary>
	/// the reading time range changed
	/// </summary>
	/// <param name="nRange"></param>
	void TimeRangeChanged(int nRange);
	/// <summary>
	/// the weight method changed
	/// </summary>
	/// <param name="nFunction"></param>
	void WeightMethodChanged(int nFunction);

// ===================== controls ==========================
private:	
	/// <summary>
	/// label showing the actor number
	/// </summary>
	QLabel* _lbANum;
	/// <summary>
	/// label showing the location number
	/// </summary>
	QLabel* _lbLNum;
	/// <summary>
	/// spinbox setting the start time
	/// </summary>
	QSpinBox* _sbST;
	/// <summary>
	/// spinbox setting the end time
	/// </summary>
	QSpinBox* _sbET;
	/// <summary>
	/// spinbox setting the actor threshold
	/// </summary>
	QSpinBox* _sbAThreshold;
	/// <summary>
	/// combobox selecting the weight method
	/// </summary>
	QComboBox* _cbboxWeights;
	/// <summary>
	/// spinbox setting the range to read of the data
	/// </summary>
	QSpinBox* _sbTRange;	
	/// <summary>
	/// text edit showing the tips of selections
	/// </summary>
	QTextEdit* _textTips;
	/// <summary>
	/// spinbox setting the weight of the selected Session
	/// </summary>
	QSpinBox* _sbSW;	
	/// <summary>
	/// spinbox setting the weight of the location of the selected Session
	/// </summary>
	QSpinBox* _sbLW;			
	/// <summary>
	/// radio button show overview
	/// </summary>
	QRadioButton* _radioOverview;
	/// <summary>
	/// radio button show detail
	/// </summary>
	QRadioButton* _radioDetail;
	/// <summary>
	/// checkbox set if show the flow
	/// </summary>
	QCheckBox* _checkShowFlow;	
	/// <summary>
	/// checkbox set if show the statistics
	/// </summary>
	QCheckBox* _checkShowStat;	
	/// <summary>
	/// slider control the scale of location projection
	/// </summary>
	QSlider* _sliderPjkScaleL;
	/// <summary>
	/// slider control the scale of actor projection
	/// </summary>
	QSlider* _sliderPjkScaleA;	
	/// <summary>
	/// slider control the scale of overview location projection
	/// </summary>
	QSlider* _sliderPjkScaleOL;	
	/// <summary>
	/// control the scale of overview actor projection
	/// </summary>
	QSlider* _sliderPjkScaleOA;	
	/// <summary>
	/// button trigger focus the selected actors
	/// </summary>
	QPushButton* _btnFocusAs;
	/// <summary>
	/// button trigger focus the selected locations
	/// </summary>
	QPushButton* _btnFocusLs;
};
