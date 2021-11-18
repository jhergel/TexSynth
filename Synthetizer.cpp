#include "Synthetizer.h"

using namespace std;

template <class T> struct VectorNd {
	std::vector<T> m_value;//float or double. IDC
	v2i pos;
	VectorNd() {
		pos = v2i(-1);
	}
	int size() {
		return m_value.size();
	}
	void pushback(T val) {
		m_value.push_back(val);
	}
	//Square distance actually, but sqrt not required here.
	float computeDistance(VectorNd<T>& queryValue) {
		sl_assert(queryValue.m_value.size() == m_value.size());
		float res = 0;
		for (int i = 0; i < queryValue.size(); i++) {
			res += (queryValue[i] - m_value[i]) * (queryValue[i] - m_value[i]);
		}
		return res;

	}
	T& operator[](int i) {
		return m_value[i];
	}
	VectorNd<T> operator+(VectorNd<T>& right) {
		VectorNd<T> res;
		for (int i = 0; i < m_value.size(); i++) {
			res.pushback(m_value[i] + right[i]);
		}
		return res;
	}

	VectorNd<T> operator-(VectorNd<T>& right) {
		VectorNd<T> res;
		for (int i = 0; i < m_value.size(); i++) {
			res.pushback(m_value[i] - right[i]);
		}
		return res;
	}
};
class Node {
	Node* m_leftSon;
	Node* m_rightSon;
	VectorNd<float> m_value;
public:
	Node() {
		m_leftSon = nullptr;
		m_rightSon = nullptr;
	}
	Node*& leftSon() {
		return m_leftSon;
	}
	Node*& rightSon() {
		return m_rightSon;
	}

	VectorNd<float>& Value() {
		return m_value;
	}
};

//to simplify the computation of the distance between two pixels
//RN, weight is 0 or 1, and we cannot go crazy, because the kernel isn't taken into account in the distance...
//it is a TODO BTW :=)
void computeVectorNd(ImageRGB_Ptr source, Kernel kernel, v2i posInSource, VectorNd<float>& val) {
	float res = 0;
	v2i kCenter = kernel.center;
	for (int i = 0; i < kernel.shape->h(); i++) {
		for (int j = 0; j < kernel.shape->w(); j++) {
			v2i posS = posInSource + (-kCenter + v2i(i, j));
			float weight = kernel.shape->pixel(i, j)[0];
			v3b sVal = source->pixel(posS[0] % source->w(), posS[1] % source->h());
			if (weight > 0) {
				val.pushback((float)sVal[0]);
				val.pushback((float)sVal[1]);
				val.pushback((float)sVal[2]);
			}
		}
	}
	val.pos = posInSource;
}


//for perturbation
void randomVector(int size, VectorNd<float>& res) {
	for (int i = 0; i < size; i++) {
		res.pushback((float)rand() / RAND_MAX);
	}
}

//mean value for the tree
//can I replace this by the median? with the random perturbation sure Not.
void computeMeanPoint(std::vector<VectorNd<float>>& values, VectorNd<float>& meanPoint) {
	//compute the mean point
	meanPoint.m_value.resize(values[0].size());
	for (int i = 0; i < meanPoint.size(); i++) {
		meanPoint[i] = 0;
		for (int j = 0; j < values.size(); j++) {
			meanPoint[i] += values[j][i];
		}
		meanPoint[i] = meanPoint[i] / values.size();
	}
}

//compute the TSVQ: the recursion
void computeTree_recurse(std::vector<VectorNd<float>>& values, VectorNd<float>& meanPoint, Node* node) {
	VectorNd<float> perturbator;
	randomVector(meanPoint.size(), perturbator);

	
	auto leftPert = meanPoint + perturbator;
	auto rightPert = meanPoint - perturbator;

	node->Value() = meanPoint;

	if (values.size() == 1) {
		node->Value() = values[0];//i will be a leaf
		return;
	}
	std::vector<VectorNd<float>> right;
	std::vector<VectorNd<float>> left;

	Node* leftNode = new Node();
	Node* righNode = new Node();
	//anything that is closest to the left perturbated point go to left,
	//same stuff for right
	for (int i = 0; i < values.size(); i++) {
		if (values[i].computeDistance(leftPert) < values[i].computeDistance(rightPert)) {

			left.push_back(values[i]);
		}
		else {
			right.push_back(values[i]);
		}
	}

	VectorNd<float> leftMean;
	VectorNd<float> righMean;
	//if no values on one side, that is sad, I mean performance wise
	if (left.size() > 0) {
		node->leftSon() = leftNode;
		computeMeanPoint(left, leftMean);
		computeTree_recurse(left, leftMean, leftNode);
	}
	if (right.size() > 0) {
		node->rightSon() = righNode;
		computeMeanPoint(right, righMean);
		computeTree_recurse(right, righMean, righNode);
	}
}

//compute the TSVQ
Node* computeTree(ImageRGB_Ptr source, Kernel& kernel) {

	Node* node = new Node();

	std::vector<VectorNd<float>> values;
	//fill the values
	for (int i = 0; i < source->h(); i++) {
		for (int j = 0; j < source->w(); j++) {
			VectorNd<float> vec;
			computeVectorNd(source, kernel, v2i(i, j), vec);
			values.push_back(vec);
		}
	}
	VectorNd<float> mp;
	computeMeanPoint(values, mp);
	computeTree_recurse(values, mp, node);
	return node;
}


//look for some good candidates. I can miss THE best, but who cares. As long as it looks good.
//fill the candidates vector
void findClosestWithTree_recurse(Node* tree, VectorNd<float>& currentPixel,	std::vector<Node*>& candidates,float tresh) {
	Node* currentNode = tree;
	tresh = tresh * tresh;
	//Am i a leaf?
	if (currentNode->leftSon() != nullptr || currentNode->rightSon() != nullptr) {
		if (currentNode->leftSon() == nullptr) {
			findClosestWithTree_recurse(currentNode->rightSon(), currentPixel, candidates,tresh);
		}
		else if (currentNode->rightSon() == nullptr) {
			findClosestWithTree_recurse(currentNode->leftSon(), currentPixel, candidates, tresh);
		}
		else if (currentNode->leftSon()->Value().computeDistance(currentPixel) < tresh *
			currentNode->rightSon()->Value().computeDistance(currentPixel)) {
			findClosestWithTree_recurse(currentNode->leftSon(), currentPixel, candidates, tresh);
		}
		else if (currentNode->rightSon()->Value().computeDistance(currentPixel) < tresh *
			currentNode->leftSon()->Value().computeDistance(currentPixel)) {
			findClosestWithTree_recurse(currentNode->rightSon(), currentPixel, candidates, tresh);
		}
		else {//both could be good
			findClosestWithTree_recurse(currentNode->rightSon(), currentPixel, candidates, tresh);
			findClosestWithTree_recurse(currentNode->leftSon(), currentPixel, candidates, tresh);
		}
		return;
	}
	//i'm a leaf: if i'm here, i'm a good candidate
	v2i pos = currentNode->Value().pos;
	candidates.push_back(currentNode);
}

//find THE best in the candidates list
v2i findClosestWithTree(Node* tree, VectorNd<float>& currentPixel) {
	std::vector<Node* > candidates;
	findClosestWithTree_recurse(tree, currentPixel, candidates,0.95);
	float distanceMin = std::numeric_limits<float>::max();
	v2i closestPixel;
	//cerr << candidates.size() << endl;;
	for (int i = 0; i < candidates.size(); i++) {
		auto vec2 = candidates[i]->Value();
		float c = currentPixel.computeDistance(vec2);
		if (c < distanceMin) {
			distanceMin = c;
			closestPixel = candidates[i]->Value().pos;
		}
	}
	return closestPixel;
}


//compute the tree and synthetise the image
ImageRGB_Ptr SynthetizerTSVQ::Synthetize(ImageRGB_Ptr source, int sizeU, int sizeV)
{
	auto res = ImageRGB_Ptr(new ImageRGB(sizeU, sizeV));
	//a stoopid check
	sl_assert(sizeU > source->w() && sizeV > source->h());//Am i doing a bigger image?
	for (int i = 0; i < sizeU; i++) {
		for (int j = 0; j < sizeV; j++) {
			res->pixel(i, j) = v3b((unsigned char)rand(), (unsigned char)rand(), (unsigned char)rand());
		}
	}
	Node* tree = computeTree(source, m_kernel);

	ImageBool1_Ptr isAlreadySynth = ImageBool1_Ptr(new ImageBool1(sizeU, sizeV, false));
	for (int i = 0; i < sizeU; i++) {
		cerr << i << endl;
		for (int j = 0; j < sizeV; j++) {
			v2i closestPixel;
			VectorNd<float> vec;
			//this was too damn slow
			//computeVectorNd(res, m_kernel, v2i(i, j), vec);
			//float distanceMin = std::numeric_limits<float>::max();
			/*for (int k = 0; k < source->w(); k++) {
				for (int l = 0; l < source->h(); l++) {
					VectorNd<float> vec2;
					computeVectorNd(source, m_kernel, v2i(k, l), vec2);
					float c = vec.computeDistance(vec2);
					if (c < distanceMin) {
						distanceMin = c;
						closestPixel = v2i(k, l);
					}
				}
			}*/
			VectorNd<float> currentPixel;
			computeVectorNd(res, m_kernel, v2i(i, j), currentPixel);
			v2i pi = findClosestWithTree(tree, currentPixel);
			res->pixel(i, j) = source->pixel(pi[0], pi[1]);

		}
	}
	return res;
}
