#include "SLDataCoreL2.h"

#include <iostream>


#include "SLSession.h"
#include "SLLocation.h"
#include "SLActor.h"


bool g_bShowActorWeight = false;
bool g_bShowLocationWeight = false;
bool g_bShowActorDistance = false;
bool g_bPrintDistance = false;
bool g_bShowLocationSorting = false;



using namespace Eigen;
using namespace std;

extern int getStrIndex(const vector<string>& list, const string& str) noexcept;

/*
* merge record in the hierarchical clustering
*/
struct MergeRecord {
	int clusterA;			// id of the first member
	int clusterB;			// id of the 2nd member
	double similarity;		// similarity of the two cluster
	int newClusterID;		// new id of the cluster
	int size;				// number of element
	int toLeaf;				// length of the path to leaf
	int toRoot;				// length of the path to the root
};

/*
* Location clusters
*/
struct Cluster {
	std::set<int> members;
	bool active = true;
	int toLeaf = 0;
	int toRoot = 0;
};

/*
* update toRoot property of the records
* nLen: the length of the list of leaves
* nIndex: current update index
* nDis: current distance
*/
void update_toRoot(vector<MergeRecord>& records, int nLen, int nIndex, int nDis) {

	// update this node
	records[nIndex].toRoot = nDis;


	// update children
	if (records[nIndex].clusterA >= nLen)
	{
		update_toRoot(records, nLen, records[nIndex].clusterA - nLen, nDis + 1);
	}
	if (records[nIndex].clusterB >= nLen)
	{
		update_toRoot(records, nLen, records[nIndex].clusterB - nLen, nDis + 1);
	}
}

vector<MergeRecord> hierarchical_clustering_by_similarity(const MatrixXd& sim_matrix) {
	int n = sim_matrix.rows();
	vector<Cluster> clusters(2 * n - 1);
	MatrixXd sim = MatrixXd::Constant(2 * n - 1, 2 * n - 1, -1e9);
	vector<MergeRecord> merge_history;

	// 0.initialization
	for (int i = 0; i < n; ++i) {
		clusters[i].members.insert(i);
		for (int j = 0; j < n; ++j) {
			sim(i, j) = sim_matrix(i, j);
		}
	}
	/*
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++) {
			cout << sim_matrix(i, j) << "\t";
		}
		cout << "\n";
	}
	*/
	set<int> active_ids;
	for (int i = 0; i < n; ++i) active_ids.insert(i);
	int next_cluster_id = n;

	// 1.merging
	while (active_ids.size() > 1) {
		// 1.1.find pair with max similarity
		int max_i = -1, max_j = -1;
		double max_sim = -1e9;

		for (int i : active_ids) {
			for (int j : active_ids) {
				if (i < j && sim(i, j) > max_sim) {
					max_sim = sim(i, j);
					max_i = i;
					max_j = j;
				}
			}
		}

		// 1.2.merging
		Cluster& new_cluster = clusters[next_cluster_id];
		new_cluster.members.insert(clusters[max_i].members.begin(), clusters[max_i].members.end());
		new_cluster.members.insert(clusters[max_j].members.begin(), clusters[max_j].members.end());
		new_cluster.active = true;
		new_cluster.toLeaf = max(clusters[max_i].toLeaf, clusters[max_j].toLeaf) + 1;

		merge_history.push_back({ max_i
			, max_j
			, max_sim
			, next_cluster_id
			, static_cast<int>(new_cluster.members.size())
			, new_cluster.toLeaf });

		// 1.3.update the similarity between the new cluster and others, using the maximum
		for (int k : active_ids) {
			if (k == max_i || k == max_j) continue;
			double s1 = sim(max_i, k);
			double s2 = sim(max_j, k);
			sim(next_cluster_id, k) = sim(k, next_cluster_id) = min(s1, s2);
			//sim(next_cluster_id, k) = sim(k, next_cluster_id) = (s1 + s2) / 2.0;
		}

		// 1.4.update the states
		clusters[max_i].active = false;
		clusters[max_j].active = false;
		active_ids.erase(max_i);
		active_ids.erase(max_j);
		active_ids.insert(next_cluster_id);

		next_cluster_id++;
	}

	// 2.update the toRoot property
	update_toRoot(merge_history, n, merge_history.size() - 1, 0);

	return merge_history;
}

MatrixXd compute_cluster_aware_similarity(int n, const vector<MergeRecord>& merge_history) {
	MatrixXd level_matrix = MatrixXd::Constant(n, n, -1);

	// 初始化：每个点单独为一类
	vector<set<int>> clusters(2 * n - 1);
	for (int i = 0; i < n; ++i)
		clusters[i].insert(i);

	// step 标记，越早聚类，层级越高（越近）
	int total_steps = merge_history.size();
	for (int step = 0; step < total_steps; ++step) {
		const auto& m = merge_history[step];
		int id = m.newClusterID;
		clusters[id].insert(clusters[m.clusterA].begin(), clusters[m.clusterA].end());
		clusters[id].insert(clusters[m.clusterB].begin(), clusters[m.clusterB].end());

		// 对新簇中所有两两元素对，若尚未记录，则记录本 step
		for (int a : clusters[id]) {
			for (int b : clusters[id]) {
				if (a < b && level_matrix(a, b) < 0) {
					level_matrix(a, b) = total_steps - step; // 越早 step 值越大
					level_matrix(b, a) = level_matrix(a, b); // 对称
				}
			}
		}
	}

	// 归一化到 [0, 1]
	double max_level = level_matrix.maxCoeff();
	MatrixXd sim_matrix(n, n);
	for (int i = 0; i < n; ++i) {
		sim_matrix(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double sim = level_matrix(i, j) / max_level;
			sim_matrix(i, j) = sim_matrix(j, i) = sim;
		}
	}
	return sim_matrix;
}

MatrixXd compute_cluster_aware_similarity_weighted(
	int n,
	const vector<MergeRecord>& merge_history,
	const MatrixXd& original_sim,
	double alpha = 0.7  // 聚类结构的权重
) {
	MatrixXd level_matrix = MatrixXd::Constant(n, n, -1);

	vector<set<int>> clusters(2 * n - 1);
	for (int i = 0; i < n; ++i)
		clusters[i].insert(i);

	int total_steps = merge_history.size();
	for (int step = 0; step < total_steps; ++step) {
		const auto& m = merge_history[step];
		int id = m.newClusterID;
		clusters[id].insert(clusters[m.clusterA].begin(), clusters[m.clusterA].end());
		clusters[id].insert(clusters[m.clusterB].begin(), clusters[m.clusterB].end());

		for (int a : clusters[id]) {
			for (int b : clusters[id]) {
				if (a < b && level_matrix(a, b) < 0) {
					level_matrix(a, b) = total_steps - step;
					level_matrix(b, a) = level_matrix(a, b);
				}
			}
		}
	}

	// 归一化
	double max_level = level_matrix.maxCoeff();
	MatrixXd level_sim = MatrixXd::Zero(n, n);
	for (int i = 0; i < n; ++i) {
		level_sim(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double norm_level = level_matrix(i, j) / max_level;
			level_sim(i, j) = level_sim(j, i) = norm_level;
		}
	}

	// 融合结构 + 原始相似度
	MatrixXd final_sim = MatrixXd::Zero(n, n);
	for (int i = 0; i < n; ++i) {
		final_sim(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double val = alpha * level_sim(i, j) + (1 - alpha) * original_sim(i, j);
			final_sim(i, j) = final_sim(j, i) = val;
		}
	}

	return final_sim;
}


MatrixXd compute_cluster_aware_similarity_depth(int n, const vector<MergeRecord>& merge_history) {
	MatrixXd level_matrix = MatrixXd::Constant(n, n, -1);

	int nMax_depth = -1;

	// 初始化：每个点单独为一类
	vector<set<int>> clusters(2 * n - 1);
	for (int i = 0; i < n; ++i)
		clusters[i].insert(i);

	// step 标记，越早聚类，层级越高（越近）
	int total_steps = merge_history.size();
	for (int step = 0; step < total_steps; ++step) {
		const auto& m = merge_history[step];
		int id = m.newClusterID;
		clusters[id].insert(clusters[m.clusterA].begin(), clusters[m.clusterA].end());
		clusters[id].insert(clusters[m.clusterB].begin(), clusters[m.clusterB].end());

		// 对新簇中所有两两元素对，若尚未记录，则记录本 step
		for (int a : clusters[id]) {
			for (int b : clusters[id]) {
				if (a < b && level_matrix(a, b) < 0) {
					level_matrix(a, b) = m.toLeaf;
					level_matrix(b, a) = level_matrix(a, b); // 对称
					if (m.toLeaf > nMax_depth) nMax_depth = m.toLeaf;
				}
			}
		}
	}
	nMax_depth++;

	// 归一化到 [0, 1]
	double max_level = level_matrix.maxCoeff();
	MatrixXd sim_matrix(n, n);
	for (int i = 0; i < n; ++i) {
		sim_matrix(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double sim = (nMax_depth - level_matrix(i, j)) / nMax_depth;
			sim_matrix(i, j) = sim_matrix(j, i) = sim;
		}
	}
	return sim_matrix;
}

MatrixXd compute_cluster_aware_similarity_depth_weighted(
	int n,
	const vector<MergeRecord>& merge_history,
	const MatrixXd& original_sim,
	double alpha = 0.7  // 聚类结构的权重
) {
	MatrixXd level_matrix = MatrixXd::Constant(n, n, -1);

	int nMax_depth = -1;

	// 初始化：每个点单独为一类
	vector<set<int>> clusters(2 * n - 1);
	for (int i = 0; i < n; ++i)
		clusters[i].insert(i);

	// step 标记，越早聚类，层级越高（越近）
	int total_steps = merge_history.size();
	for (int step = 0; step < total_steps; ++step) {
		const auto& m = merge_history[step];
		int id = m.newClusterID;
		clusters[id].insert(clusters[m.clusterA].begin(), clusters[m.clusterA].end());
		clusters[id].insert(clusters[m.clusterB].begin(), clusters[m.clusterB].end());

		// 对新簇中所有两两元素对，若尚未记录，则记录本 step
		for (int a : clusters[id]) {
			for (int b : clusters[id]) {
				if (a < b && level_matrix(a, b) < 0) {
					level_matrix(a, b) = m.toLeaf;
					level_matrix(b, a) = level_matrix(a, b); // 对称
					if (m.toLeaf > nMax_depth) nMax_depth = m.toLeaf;
				}
			}
		}
	}
	nMax_depth++;
	cout << "Number of points: " << n << ";Depth: " << nMax_depth << endl;

	// 归一化到 [0, 1]
	double max_level = level_matrix.maxCoeff();
	MatrixXd level_sim(n, n);
	for (int i = 0; i < n; ++i) {
		level_sim(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double sim = (nMax_depth - level_matrix(i, j)) / nMax_depth;
			level_sim(i, j) = level_sim(j, i) = sim;
		}
	}

	// 融合结构 + 原始相似度
	MatrixXd final_sim = MatrixXd::Zero(n, n);
	for (int i = 0; i < n; ++i) {
		final_sim(i, i) = 1.0;
		for (int j = i + 1; j < n; ++j) {
			double val = alpha * level_sim(i, j) + (1 - alpha) * original_sim(i, j);
			final_sim(i, j) = final_sim(j, i) = val;
		}
	}

	return final_sim;
}

// using the correlation to generate distance and reture the maximum distance
// this distance is the 1.5 times max correlation minus the correlation.
// selft distance is set to 0
double FormalizeDistance(int n, MatrixXd& disOrg, MatrixXd& disResult) {

	// 1.calculate max distance
	double dbMaxDis = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (disOrg(i, j) > dbMaxDis) dbMaxDis = disOrg(i, j);
		}
	}
	cout << "Max distance: " << dbMaxDis << endl;
	double dbBaseDisFactor = 1.5;
	dbMaxDis *= dbBaseDisFactor;

	// 2.self distance set to 0
	for (int i = 0; i < n; i++)
	{
		disResult(i, i) = 0;
	}
	// 3.calculate distance
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			disResult(i, j) = disResult(j, i) = dbMaxDis - disOrg(i, j);
		}
	}
	return dbMaxDis;
}

// sort the list of strings according to the list of values
void SortStringByValue(vector<double>& listV, vector<string>& listS) {
	int n = listS.size();
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = i + 1; j < n; j++) {
			if (listV[i] > listV[j])
			{
				swap(listV[i], listV[j]);
				swap(listS[i], listS[j]);
			}
		}
	}
}

// keep the pcs stable
void KeepStable(vector<string> listIdsRef, vector<double> listPC0Ref, vector<double> listPC1Ref
	, vector<string>& listIds, vector<double>& listPC0, vector<double>& listPC1) {
	double dbOrg = 0;
	double dbSwap0 = 0;
	double dbSwap1 = 0;
	double dbSwap01 = 0;
	int nLen = listIds.size();
	for (int i = 0; i < nLen; i++)
	{
		int nIndex = getStrIndex(listIdsRef,listIds[i]);
		double dbPC0Ref = listPC0Ref[nIndex];
		double dbPC1Ref = listPC1Ref[nIndex];
		double dbPC0 = listPC0[i];
		double dbPC1 = listPC1[i];
		dbOrg += (abs(dbPC0Ref - dbPC0) + abs(dbPC1Ref - dbPC1));
		dbSwap0 += (abs(dbPC0Ref + dbPC0) + abs(dbPC1Ref - dbPC1));
		dbSwap1 += (abs(dbPC0Ref - dbPC0) + abs(dbPC1Ref + dbPC1));
		dbSwap01 += (abs(dbPC0Ref + dbPC0) + abs(dbPC1Ref + dbPC1));
	}
	cout << "stability:" << dbOrg << dbSwap0 << dbSwap1 << dbSwap01 << endl;
	if (dbOrg < dbSwap0 && dbOrg < dbSwap1 && dbOrg < dbSwap01)
	{
		// do nothing
	}
	else if (dbSwap0 < dbSwap1 && dbSwap0 < dbSwap01) {
		for (int i = 0; i < nLen; i++)
		{
			listPC0[i] = -listPC0[i];
		}
	}
	else if (dbSwap1 < dbSwap01) {
		for (int i = 0; i < nLen; i++)
		{
			listPC1[i] = -listPC1[i];
		}
	}
	else {
		for (int i = 0; i < nLen; i++)
		{
			listPC0[i] = -listPC0[i];
			listPC1[i] = -listPC1[i];
		}
	}
}


void SLDataCoreL2::PreVisualization(int nWeightMethod) {
	// 1.calculate weight
	setWeight();

	// 2.generate session weight，timestep weight and time projection
	generateSessionWeight();

	// 3.sort the locations
	sortLocations(nWeightMethod);

	// 4.sort actors
	SortActors();

	// 5.calculate location depth
	calculateLocationDepth();
}

void SLDataCoreL2::setWeight() {

	vector<string> listSignificantActors = _listA;
	vector<string> listSignificantLocations = _listL;

	// 0.set initial weights for locations
	double dbInitialLocationW = 1.0;
	for (string strL : listSignificantLocations)
	{
		_mapL[strL]->SetW1(dbInitialLocationW);
	}

	// 1. interatively update
	double dbThreshold = 0.00001;
	if (listSignificantActors.size() > 0) {
		int nLoop = 0;
		while (++nLoop)
		{
			map<string, double> mapLastAW = GetAWMap();
			map<string, double> mapLastLW = GetLWMap();

			updateWeight(listSignificantActors, listSignificantLocations);
			double dbSumAWChange = 0;

			map<string, double> mapAW = GetAWMap();
			for (string strA : listSignificantActors)
			{
				dbSumAWChange += abs(mapLastAW[strA] - mapAW[strA]);
			}
			double dbSumLWChange = 0;
			map<string, double> mapLW = GetLWMap();
			for (string strL : listSignificantLocations)
			{
				dbSumLWChange += abs(mapLastLW[strL] - mapLW[strL]);
			}
			//qDebug() << "Change of Actor Weights: " << dbSumAWChange;
			//qDebug() << "Change of Location Weights: " << dbSumLWChange;
			if (dbSumAWChange < dbThreshold && dbSumLWChange < dbThreshold) {
				cout << "page rank iteration:" << nLoop << endl;
				break;
			}
			assert(nLoop < 100000);
		}
	}
	else {
		for (string strL : listSignificantLocations)
		{
			_mapL[strL]->SetW1(0);
		}
	}

	// 2.calculate maximum
	_dbMaxAW = 0;
	_dbMaxLW = 0;
	_dbMaxAWO = 1;
	_dbMaxLWO = 1;
	_dbMaxAW1 = 0;
	_dbMaxLW1 = 0;
	for (string strA : listSignificantActors)
	{
		if (GetAWO(strA) > _dbMaxAWO) _dbMaxAWO = GetAWO(strA);
		if (GetAW1(strA) > _dbMaxAW1) _dbMaxAW1 = GetAW1(strA);
		double dbAW = GetAW(strA);
		if (dbAW > _dbMaxAW) _dbMaxAW = dbAW;
	}
	for (string strL : listSignificantLocations)
	{
		if (GetLWO(strL) > _dbMaxLWO) _dbMaxLWO = GetLWO(strL);
		if (GetLW1(strL) > _dbMaxLW1) _dbMaxLW1 = GetLW1(strL);
		double dbLW = GetLW(strL);
		if (dbLW > _dbMaxLW) _dbMaxLW = dbLW;
	}


	// 3. output
	if (g_bShowActorWeight) {
		cout << "====================Actor Weights==================\n";
		double dbW = 0;
		for (string strA : listSignificantActors)
		{
			dbW += GetAW1(strA);
			cout << strA << ":\t" << GetAW1(strA) << endl;
		}
		cout << dbW << endl;
	}

	if (g_bShowLocationWeight) {
		cout << "====================Location Weights==================\n";
		double dbW = 0;
		for (string strL : listSignificantLocations)
		{
			cout << _mapL[strL]->GetDisplayName() << ":\t" << GetLW1(strL) << endl;
			dbW += GetLW1(strL);
		}
		cout << dbW << endl;
	}

	//generateSessionWeight();
}

void SLDataCoreL2::sortLocations(int nMethod) {

	if (_listA.size() <= 2) return;

	// 1.calculate distance matrix
	buildDistanceMatrix(nMethod);

	buildActorDistanceMatrix();

	// 2.sort
	sortMDS(_D_Location);

	// 3.set sequence
	int nSequence = 0;
	for (string strL : _listL) {
		_mapL[strL]->SetSequence(nSequence++);
	}
}

void SLDataCoreL2::generateSessionWeight() {
	// 0. Initialize containers with default values
	const int timeRange = getTimeRange();
	_listTimestepW.assign(timeRange, 0.0);
	_listTimeProjection.resize(timeRange);

	// Initialize time projection with sequential values (0,1,2,...)
	std::iota(_listTimeProjection.begin(), _listTimeProjection.end(), 0);

	// 1. Reset max session weight
	_dbMaxSW1 = 0.0;

	// 2. Calculate session weights and timestep weights
	for (const auto& strS : _listS) {
		auto& session = _mapS[strS];

		// Calculate location weight
		double dbLW = GetLW(session->GetL());

		// Calculate actor weight (sum of all actors' weights)
		double dbAW = 0.0;
		for (const auto& strA : session->GetActors()) {
			dbAW += GetAW(strA);
		}

		// Calculate session weight
		double dbW = dbAW * dbLW;

		// Update max session weight
		if (dbW > _dbMaxSW1) {
			_dbMaxSW1 = dbW;
		}

		// Store session weight and update timestep weight
		session->SetW1(dbW);
		_listTimestepW[session->GetT() - _nStart] += dbW;
	}

	// 3. Generate time projection
	for (int i = 1; i < timeRange; i++) {
		_listTimeProjection[i] = _listTimeProjection[i - 1] +
			(_listTimestepW[i - 1] > 0 ? 1 : 0);
	}
}

void SLDataCoreL2::calculateLocationDepth() {
	_nMaxDepth = 1;
	for (LocationMerge& merge : _listLocationMerge)
	{

		SLLocation* pLI = _mapL[merge._strL1];
		SLLocation* pLJ = _mapL[merge._strL2];

		// 1.calculate maximum depth and depth for the leaves
		int nNewDepth = merge._nToRoot + 1;
		if (find(_listL.begin(), _listL.end(), merge._strL1)!= _listL.end()) pLI->SetD2R(nNewDepth);
		if (find(_listL.begin(), _listL.end(), merge._strL2) != _listL.end()) pLJ->SetD2R(nNewDepth);

		_listLClusters.push_back(merge._strC);
		if (nNewDepth > _nMaxDepth)
		{
			_nMaxDepth = nNewDepth;
		}

		// 2. create the new Locations for the cluster
		SLLocation* pNewL = new SLLocation(merge._strC, pLI, pLJ,this);
		pNewL->SetD2R(merge._nToRoot);
		_mapL[merge._strC] = pNewL;
		pLI->SetCluster(merge._strC);
		pLJ->SetCluster(merge._strC);
		calculateLocationWidth(merge._strC);
	}
}

void SLDataCoreL2::updateWeight(const vector<string>& listActors, const vector<string>& listLocations) {
	
	double dbBaseW = 0;
	// 1.using location weights to update actor weights, location weights have been set all to 1.0
	double dbSumW = 0;
	for (string strA : listActors)
	{
		double dbAW = dbBaseW;
		for (SLSpan span : _mapA[strA]->GetSpans()) {
			string strL = span._strLocationId;
			double dbLW = GetLW(strL);
			double dbSW = _mapS[GenerateSessionID(span._nT, strL)]->GetWO();
			double dbW = dbLW * dbSW;
			dbAW += dbW;
		}
		_mapA[strA]->SetW1(dbAW);
		dbSumW += dbAW;
	}
	//	dbSumW /= listActors.size();
	for (string strA : listActors)
	{
		_mapA[strA]->SetW1(GetAW1(strA) / dbSumW);
	}

	// 2. using actor weights to update location weights
	dbSumW = 0;
	for (string strL : listLocations)
	{
		_mapL[strL]->SetW1(dbBaseW);
		dbSumW += dbBaseW;
	}
	for (string strA : listActors)
	{
		double dbAW = GetAW(strA);
		for (SLSpan span : _mapA[strA]->GetSpans()) {
			string strL = span._strLocationId;
			double dbSW = _mapS[GenerateSessionID(span._nT, strL)]->GetWO();
			double dbW = dbAW * dbSW;
			_mapL[strL]->SetW1(GetLW1(strL) + dbW);
			dbSumW += dbW;
		}
	}
	//dbSumW /= _listLocationIds.size();
	for (string strL : listLocations)
	{
		_mapL[strL]->SetW1(GetLW1(strL) / dbSumW);
	}

	
}

void SLDataCoreL2::sortMDS(MatrixXd D) {

	vector<string>& listLocations = _listL;
	_listLocationIdsPC1 = _listL;
	int n = listLocations.size();

	// 3.Compute metric MDS (embedding into a 2-dimensional space)	
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(D, 2);
	vector<double> listPC;
	vector<double> listPC1;

	for (int i = 0; i < n; i++)
	{
		listPC.push_back(X(0, i));
		listPC1.push_back(X(1, i));
	}
	if (_bInit)
	{
		KeepStable(_listIdsRef, _listPC0Ref, _listPC1Ref, listLocations, listPC, listPC1);
	}
	else {
		_listIdsRef = listLocations;
		_listPC0Ref = listPC;
		_listPC1Ref = listPC1;
		_bInit = true;
	}

	for (int i = 0; i < n; i++)
	{
		_mapL[listLocations[i]]->SetPC(0, X(0, i));
		_mapL[listLocations[i]]->SetPC(1, X(1, i));
	}

	SortStringByValue(listPC, listLocations);
	SortStringByValue(listPC1, _listLocationIdsPC1);

	// let the first location in bottom
	if (getStrIndex(listLocations,_strFirstLocation) < listLocations.size() / 2)
		std::reverse(listLocations.begin(), listLocations.end());
	/*
	if (g_bShowLocationSorting) {
		qDebug() << "Output sorted locations: ";
		for (int i = 0; i < n; i++)
		{
			qDebug() << _mapL[listLocations[i]].GetDisplayName() << "\t" << listPC[i];
		}
	}
	*/
}

void SLDataCoreL2::buildActorDistanceMatrix() {
	const double dbMaxDis = 1;
	const vector<string>& listLocations = _listL;

	// 1. Build distance matrix between actors
	const int n = _listA.size();
	Eigen::MatrixXd mxDis = MatrixXd(n, n);  // D(i,j) means distance of merging j to i

	for (int i = 0; i < n; i++) {
		mxDis(i, i) = 0;
		for (int j = i+1; j < n; j++) {

			double dbActorDis = 0;
			const SLActor* pA1 = _mapA[_listA[i]];
			const SLActor* pA2 = _mapA[_listA[j]];

			// Collect unique time points from both actors using std::set
			std::set<int> setTimes;
			for (const SLSpan& span : pA1->GetSpans()) {
				setTimes.insert(span._nT);
			}
			for (const SLSpan& span : pA2->GetSpans()) {
				setTimes.insert(span._nT);
			}

			// Calculate pairwise distances
			int nIndex0 = 0;
			int nIndex1 = 0;
			const auto& spans1 = pA1->GetSpans();
			const auto& spans2 = pA2->GetSpans();

			while (nIndex0 < spans1.size() && nIndex1 < spans2.size()) {
				if (spans1[nIndex0]._nT < spans2[nIndex1]._nT) {
					dbActorDis += dbMaxDis;
					nIndex0++;
				}
				else if (spans1[nIndex0]._nT > spans2[nIndex1]._nT) {
					dbActorDis += dbMaxDis;
					nIndex1++;
				}
				else {
					const string& strL0 = spans1[nIndex0]._strLocationId;
					const string& strL1 = spans2[nIndex1]._strLocationId;
					int nIndexL0 = getStrIndex(listLocations, strL0);
					int nIndexL1 = getStrIndex(listLocations, strL1);
					dbActorDis += _D_Location(nIndexL0, nIndexL1);
					nIndex0++;
					nIndex1++;
				}
			}

			// Add remaining spans
			while (nIndex0 < spans1.size()) {
				dbActorDis += dbMaxDis;
				nIndex0++;
			}
			while (nIndex1 < spans2.size()) {
				dbActorDis += dbMaxDis;
				nIndex1++;
			}

			// Store symmetric distance
			mxDis(i, j) = mxDis(j, i) = dbActorDis / setTimes.size();
		}
	}

	// 2. Compute metric MDS (embedding into 2D space)
	const MatrixXd X = mathtoolbox::ComputeClassicalMds(mxDis, 2);
	vector<double> listPC;
	vector<double> listPC1;

	for (int i = 0; i < n; i++) {
		listPC.push_back(X(0, i));
		listPC1.push_back(X(1, i));
		_mapA[_listA[i]]->SetPC(0, X(0, i));
		_mapA[_listA[i]]->SetPC(1, X(1, i));
	}

	_D_Actor = mxDis;
}

void SLDataCoreL2::buildDistanceMatrix(int nWeightMethod) {
	const vector<string>& listActors = _listA;
	const vector<string>& listLocations = _listL;
	const int n = listLocations.size();
	if (n < 2) return;

	// 0. Initialize moving matrix with zeros
	MatrixXd D_moving0 = MatrixXd::Zero(n, n);

	// Precompute location indices for faster lookup
	std::unordered_map<string, int> locationIndices;
	for (int i = 0; i < n; ++i) {
		locationIndices[listLocations[i]] = i;
	}

	// 1. Calculate actor moving distance
	for (const string& nameActor : listActors) {
		const SLActor* pA = _mapA[nameActor];
		const auto& spans = pA->GetSpans();
		const int nLen = spans.size();
		if (nLen < 2) continue;

		const double actorWeight = GetAW(nameActor);

		for (int i = 0; i < nLen - 1; ++i) {
			const auto& span_i = spans[i];
			auto it = locationIndices.find(span_i._strLocationId);
			if (it == locationIndices.end()) continue;
			const int nI0 = it->second;

			switch (nWeightMethod) {
			case WeightMethod_0: {
				const int j = i + 1;
				const auto& span_j = spans[j];
				it = locationIndices.find(span_j._strLocationId);
				if (it != locationIndices.end() && it->second != nI0) {
					const int nI1 = it->second;
					D_moving0(nI0, nI1) = D_moving0(nI1, nI0) = D_moving0(nI0, nI1) + actorWeight;
				}
				break;
			}
			case WeightMethod_1: {
				const int j = i + 1;
				const auto& span_j = spans[j];
				it = locationIndices.find(span_j._strLocationId);
				if (it != locationIndices.end() && it->second != nI0 && i > 0) {
					const int nI1 = it->second;
					const double timeFactor = spans[i]._nT - spans[i - 1]._nT;
					const double weightedDis = actorWeight * timeFactor;
					D_moving0(nI0, nI1) = D_moving0(nI1, nI0) = D_moving0(nI0, nI1) + weightedDis;
				}
				break;
			}
			case WeightMethod_2: {
				for (int j = i + 1; j < nLen; ++j) {
					const auto& span_j = spans[j];
					it = locationIndices.find(span_j._strLocationId);
					if (it != locationIndices.end() && it->second != nI0) {
						const int nI1 = it->second;
						const double weightedDis = actorWeight / (j - i);
						D_moving0(nI0, nI1) = D_moving0(nI1, nI0) = D_moving0(nI0, nI1) + weightedDis;
					}
				}
				break;
			}
			case WeightMethod_3: {
				if (i == 0) continue;
				for (int j = i + 1; j < nLen; ++j) {
					const auto& span_j = spans[j];
					it = locationIndices.find(span_j._strLocationId);
					if (it != locationIndices.end() && it->second != nI0) {
						const int nI1 = it->second;
						const double timeFactor = spans[i]._nT - spans[i - 1]._nT;
						const double weightedDis = (actorWeight * timeFactor) / (j - i);
						D_moving0(nI0, nI1) = D_moving0(nI1, nI0) = D_moving0(nI0, nI1) + weightedDis;
					}
				}
				break;
			}
			default:
				break;
			}
		}
	}

	// 2. Formalize the distance
	vector<MergeRecord> listMergeRecord = hierarchical_clustering_by_similarity(D_moving0);
	MatrixXd sim_new = compute_cluster_aware_similarity_depth_weighted(
		D_moving0.rows(), listMergeRecord, D_moving0, 0);

	// Store merge records
	_listLocationMerge.clear();
	_listLocationMerge.reserve(listMergeRecord.size());
	for (const MergeRecord& mr : listMergeRecord) {
		LocationMerge lm;
		lm._strL1 = (mr.clusterA < n) ? _listL[mr.clusterA] : "C" + to_string(mr.clusterA);
		lm._strL2 = (mr.clusterB < n) ? _listL[mr.clusterB] : "C" + to_string(mr.clusterB);
		lm._strC = "C" + to_string(mr.newClusterID);
		lm._nToRoot = mr.toRoot;
		_listLocationMerge.push_back(lm);

		SLLocation newL(lm._strC);
		newL.SetDisplayName(lm._strC);
		newL.SetD2R(mr.toRoot);
	}

	// Compute final distance matrix
	MatrixXd D(n, n);
	double dbMaxDis = FormalizeDistance(n, sim_new, D);
	cout << "Max Distance: " << dbMaxDis << endl;
	_D_Location = D;
}
