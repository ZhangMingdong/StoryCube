#pragma once
#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QChartView>

class QDoubleSpinBox;
class QFormLayout;
class QGridLayout;
class QBarCategoryAxis;
class QCheckBox;

class ActorWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ActorWidget(QWidget *parent = 0);
private:
	const int _nLimit = 100;					// displayed actor limit
private:
	QList<QString> _listActorNames;			// actor list
	QList<double> _listW;					// weight list
	QList<double> _listWO;					// original weight list

	// tab1
	QChart* m_chart;						// chart of tab1
	QChartView* m_chartView;				// chartview of tab1
	QBarCategoryAxis* m_axisY = NULL;		// axis of chart in tab1

	// tab2
	QGridLayout* _pLayoutAdjust;			// layout of tab2
	QGridLayout* _pLayoutWeight;			// layout of tab3
	QList<QDoubleSpinBox*> _listSpins;		// list of Spins of tab2
	QList<QCheckBox*> _listChecks;			// list of checkbox of tab2

public:
	/*
	* set Actors for the widget
	*/
	void SetActors(std::vector<std::string> listActors, std::map<std::string, double> mapWs, std::map<std::string, double> mapWOs);

	/*
	* reset the generated weight
	* called when:
	*	location weight modifed
	*	session weight modifed
	*	actor weight modifed
	*	actor checked state changed
	*/
	void ResetWeights(const QMap<QString, double> mapWs);
	void ResetWeights(const std::map<std::string, double> mapWs);
private:
	/*
	* called when the value of the DoubleSpinBox changed
	*/
	void weightChanged(QString strA, double dbValue);

	/*
	* called when the actor is checked
	*/
	void actorChecked(QString strA, int nState);

	// generate the barcharts of the actor weight
	void generateBarCharts();

	// generate the actor list view
	void generateListView();

private slots:
	void selectAll();
	void unselectAll();
signals:
	// change actor weight
	void UpdateActorWeights(QString strA, double dbW);

	// check actor check state
	void UpdateActorCheck(QString strA, int nState);

	void KeepOnlyOneActor(QString strA);

	void UnselectAllActors();

	void SelectAllActors();

};
