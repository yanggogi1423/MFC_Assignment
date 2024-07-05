
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once
using namespace std;
#include <vector>
#include <list>

//	cmath 헤더 사용 X
#define PI 3.141592

//	Class

class MyShape {
public:
	//	BORDER 박스 요소
	CRect box;
	CPoint invb, bor;

	//	선택 여부
	bool isSel;

	//	색상
	COLORREF c;

	//	도형 타입
	// type -> 0 : RECT, 1 : CIRCLE, 2 : CURVE, 3 : STAR, 5 : SELECT
	int type;

	//	선택박스 요소
	CRect selBox;
	CPoint topLeft;
	CPoint bottomRight;

	//	그룹 요소
	vector<MyShape*> group;
	bool isG;

	MyShape* parent;

	MyShape() {
		isSel = false;
		isG = false;

		type = 4;
		parent = NULL;
	}

	//	그룹 객체 전용 생성자
	MyShape(bool g, vector<MyShape*>G) {
		isG = g;
		isSel = true;
		if (isG == true) {
			group = G;
			setGroup();
			setSelBox();
			type = 4;
		}

		parent = NULL;
	}

	MyShape(CPoint i, CPoint b, COLORREF col) {
		invb = i;
		bor = b;
		isSel = false;
		c = col;
		box.SetRect(i, b);

		isG = false;
	}

	virtual void move(CPoint vec) {
		//	MyShape 객체를 생성해야 하기에 순수 가상함수로 생성하지 않음
	}

	void setSelBox() {
		int boxMargin = 5;

		if (1) {
			topLeft = CPoint(invb.x - boxMargin, invb.y - boxMargin);
			bottomRight = CPoint(bor.x + boxMargin, bor.y + boxMargin);

			selBox.SetRect(topLeft, bottomRight);
		}
	}

	void setGroup() {
		vector<MyShape*>::iterator it;
		int maxX = 0, maxY = 0, minX = INT_MAX, minY = INT_MAX;
		for (it = group.begin(); it != group.end(); it++) {
			//	부모를 자신으로 설정
			(*it)->parent = this;

			CPoint tmp1 = (*it)->invb;
			CPoint tmp2 = (*it)->bor;

			if (tmp2.x > maxX) {
				maxX = tmp2.x;
			}
			if (tmp2.y > maxY) {
				maxY = tmp2.y;
			}
			if (tmp1.x < minX) {
				minX = tmp1.x;
			}
			if (tmp1.y < minY) {
				minY = tmp1.y;
			}
		}
		bor = CPoint(maxX, maxY);
		invb = CPoint(minX, minY);
		box.SetRect(invb, bor);
	}

	void moveAll(CPoint vec) {
		vector<MyShape*>::iterator it;
		for (it = group.begin(); it != group.end(); it++) {
			if ((*it)->type == 4) {
				(*it)->moveAll(vec);
			}
			else {
				(*it)->move(vec);
			}
		}
		setGroup();
		setSelBox();
	}
};

class MyRect : public MyShape {
public:
	MyRect() {
		type = 0;
	}

	MyRect(CPoint i, CPoint b, COLORREF col) : MyShape(i, b, col) {
		type = 0;

		//	선택 박스 생성
		setSelBox();
	}

	void move(CPoint vec) {
		invb += vec;
		bor += vec;
		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();
	}

};

class MyCircle :public MyShape {
public:
	CPoint center, click;
	float radius;

	MyCircle() {
		type = 1;
	}

	MyCircle(CPoint c, CPoint b, COLORREF col) {

		type = 1;

		center = c;
		click = b;
		radius = calRad();
		find();

		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();

		this->c = col;
	}

	float calRad() {
		float n1 = (center.x - click.x) * (center.x - click.x);
		float n2 = (center.y - click.y) * (center.y - click.y);

		return sqrt(n1 + n2);
	}

	void update() {
		radius = calRad();
		find();

		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();
	}

	void find() {
		bor.x = center.x + radius;
		bor.y = center.y + radius;
		invb.x = center.x - radius;
		invb.y = center.y - radius;
	}

	void reset() {
		center = CPoint(0, 0);
		click = CPoint(0, 0);
		radius = calRad();
		find();
	}


	void move(CPoint vec) {
		invb += vec;
		bor += vec;
		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();
	}
};

class MyCurve : public MyShape {
public:
	vector<CPoint> curve;

	MyCurve() {
		type = 2;
	}

	MyCurve(vector<CPoint>& ls, COLORREF col) {
		//	vector 복사
		curve = ls;

		this->c = col;
		type = 2;

		//	선택 박스 생성
		setBox();
		setSelBox();
	}

	~MyCurve() {
		curve.clear();
	}

	void setBox() {

		vector<CPoint>::iterator it;

		int maxX = 0, maxY = 0, minX = INT_MAX, minY = INT_MAX;

		for (it = curve.begin(); it != curve.end(); it++) {
			int x = (*it).x;
			int y = (*it).y;

			if (x > maxX) {
				maxX = x;
			}
			if (y > maxY) {
				maxY = y;
			}
			if (x < minX) {
				minX = x;
			}
			if (y < minY) {
				minY = y;
			}
		}

		bor = CPoint(maxX, maxY);
		invb = CPoint(minX, minY);
	}

	void move(CPoint vec) {
		vector<CPoint>::iterator it;

		for (it = curve.begin(); it != curve.end(); it++) {
			(*it) += vec;
		}

		invb += vec;
		bor += vec;
		box.SetRect(invb, bor);

		setBox();
		//	선택 박스 생성
		setSelBox();
	}
};

class MyStar : public MyShape {	//	Circle과 비슷
public:
	CPoint center, click;
	float radius;
	float radius2;

	CPoint starList[10];

	MyStar() {
		type = 3;
	}

	MyStar(CPoint c, CPoint b, COLORREF col) {

		type = 3;

		center = c;
		click = b;
		radius = calRad();
		radius2 = radius * 0.4;

		//	별들의 꼭짓점 설정
		setPoint();

		find();

		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();

		this->c = col;
	}

	float calRad() {
		float n1 = (center.x - click.x) * (center.x - click.x);
		float n2 = (center.y - click.y) * (center.y - click.y);

		return sqrt(n1 + n2);
	}

	void update() {
		radius = calRad();
		radius2 = radius * 0.4;

		setPoint();


		find();

		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();
	}

	void find() {

		int maxX = 0, maxY = 0, minX = INT_MAX, minY = INT_MAX;

		for (int i = 0; i < 10; i++) {
			int x = starList[i].x;
			int y = starList[i].y;

			if (x > maxX) {
				maxX = x;
			}
			if (y > maxY) {
				maxY = y;
			}
			if (x < minX) {
				minX = x;
			}
			if (y < minY) {
				minY = y;
			}
		}

		bor = CPoint(maxX, maxY);
		invb = CPoint(minX, minY);
	}

	void setPoint() {
		for (int i = 0; i < 10; i++) {
			if (i % 2 == 1) {
				float x = (radius)*cos(i * PI * 72 / 360.f + PI * 36 / 360.f) + center.x;
				float y = (radius)*sin(i * PI * 72 / 360.f + PI * 36 / 360.f) + center.y;
				starList[i] = CPoint(x, y);
			}

			else {	//inner
				float x = (radius2)*cos(i * PI * 72 / 360.f + PI * 36 / 360.f) + center.x;
				float y = (radius2)*sin(i * PI * 72 / 360.f + PI * 36 / 360.f) + center.y;
				starList[i] = CPoint(x, y);
			}
		}
	}

	void reset() {
		center = CPoint(0, 0);
		click = CPoint(0, 0);
		radius = calRad();
		radius2 = calRad() * 0.4f;
		find();
	}

	void move(CPoint vec) {

		for (int i = 0; i < 10; i++) {
			starList[i] += vec;
		}
		invb += vec;
		bor += vec;
		box.SetRect(invb, bor);

		//	선택 박스 생성
		setSelBox();
	}

};

// CChildView 창

class CChildView : public CWnd
{
	// 생성입니다.
public:
	CChildView();

	// 특성입니다.
public:
	COLORREF color;

	//	push_front를 사용하기 위해 list 사용
	list<MyShape*> shapeList;

	//	Rect 전용
	CPoint p, q;

	//	Circle 전용
	MyCircle buffCircle;

	//	Curve 전용
	vector<CPoint> curve_point;

	//	Star 전용
	MyStar buffStar;

	//	type
	int type;

	//	선택 박스
	bool isSel;

	CPoint selPoint;
	CPoint selPoint2;
	CPoint lastPoint;

	//	선택된 도형 개수
	int cnt;
	
	//	선택된 도형이 있을 때, 드래그
	bool isMore;

public:

	// 재정의입니다.
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDrawRectangle();
	afx_msg void OnDrawCircle();
	afx_msg void OnActionSelect();
	afx_msg void OnActionGroup();
	afx_msg void OnActionUngroup();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDrawCurve();
	afx_msg void OnDrawStar();
	afx_msg void OnAlignBringfront();
	afx_msg void OnAlignBringback();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateDrawRectangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawCircle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawCurve(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawStar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionSelect(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateActionGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActionUngroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignBringback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignBringfront(CCmdUI* pCmdUI);

	bool isInAll(MyShape* tmp, CPoint point);
	bool isIn(CPoint point);
	bool isLeft(CPoint p, CPoint q, CPoint r);
	bool isStar(CPoint in, CPoint pt[]);
	void printAll(MyShape* G, CDC* memDc);
	bool selParent(MyShape* tmp, CPoint point);
};

