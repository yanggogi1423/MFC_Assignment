
// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Assignment_4.h"
#include "ChildView.h"
#include <vector>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	srand(time(NULL));
	color = RGB(rand() % 256, rand() % 256, rand() % 256);
	type = 0;

	p = CPoint(0, 0);
	q = CPoint(0, 0);

	cnt = 0;

	isSel = false;
	isMore = false;
}

CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_DRAW_RECTANGLE, &CChildView::OnDrawRectangle)
	ON_COMMAND(ID_DRAW_CIRCLE, &CChildView::OnDrawCircle)
	ON_COMMAND(ID_ACTION_SELECT, &CChildView::OnActionSelect)
	ON_COMMAND(ID_ACTION_GROUP, &CChildView::OnActionGroup)
	ON_COMMAND(ID_ACTION_UNGROUP, &CChildView::OnActionUngroup)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_DRAW_CURVE, &CChildView::OnDrawCurve)
	ON_COMMAND(ID_DRAW_STAR, &CChildView::OnDrawStar)
	ON_COMMAND(ID_ALIGN_BRINGFRONT, &CChildView::OnAlignBringfront)
	ON_COMMAND(ID_ALIGN_BRINGBACK, &CChildView::OnAlignBringback)
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECTANGLE, &CChildView::OnUpdateDrawRectangle)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CIRCLE, &CChildView::OnUpdateDrawCircle)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CURVE, &CChildView::OnUpdateDrawCurve)
	ON_UPDATE_COMMAND_UI(ID_DRAW_STAR, &CChildView::OnUpdateDrawStar)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SELECT, &CChildView::OnUpdateActionSelect)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_ACTION_GROUP, &CChildView::OnUpdateActionGroup)
	ON_UPDATE_COMMAND_UI(ID_ACTION_UNGROUP, &CChildView::OnUpdateActionUngroup)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_BRINGBACK, &CChildView::OnUpdateAlignBringback)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_BRINGFRONT, &CChildView::OnUpdateAlignBringfront)
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rect;
	GetClientRect(&rect);

	//	Double Buffering
	CDC memDc;
	memDc.CreateCompatibleDC(&dc);
	CBitmap bitmap;

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDc.SelectObject(&bitmap);

	//	배경을 하얀색으로
	memDc.Rectangle(rect);
	memDc.SetBkMode(TRANSPARENT);

	list<MyShape*>::iterator it;

	for (it = shapeList.begin(); it != shapeList.end(); it++) {
		if ((*it)->isG) {
			printAll((*it), &memDc);
		}
		CBrush brush((*it)->c);
		memDc.SelectObject(&brush);
		CPen black(PS_SOLID, 3, RGB(0, 0, 0));
		memDc.SelectObject(&black);

		if ((*it)->type == 0) {
			memDc.Rectangle((*it)->box);
		}

		else if ((*it)->type == 1) {
			memDc.Ellipse((*it)->box);
		}

		else if ((*it)->type == 2) {	//	curve

			//	다운 캐스팅
			CPen pen(BS_SOLID, 3, (*it)->c);
			memDc.SelectObject(pen);

			vector<CPoint>::iterator cp;
			MyCurve* tmp = (MyCurve*)(*it);
			if (tmp->curve.size() != 0) {
				for (cp = tmp->curve.begin(); cp != tmp->curve.end() - 1; cp++) {
					memDc.MoveTo(*(cp));
					memDc.LineTo(*(cp + 1));
				}
			}
		}

		else if ((*it)->type == 3) {
			MyStar* tmp = (MyStar*)(*it);

			//	polygon을 이용해야 외곽선을 칠할 수 있음
			memDc.Polygon(tmp->starList, 10);
		}

		//	외곽선
		memDc.SelectStockObject(NULL_BRUSH);
		CPen selPen(PS_DOT, 1, RGB(255, 0, 0));
		memDc.SelectObject(selPen);

		if ((*it)->isSel) {
			memDc.Rectangle((*it)->selBox);
		}
	}

	//	현재 그린 것 그리기

	CBrush brush(color);
	memDc.SelectObject(&brush);
	CPen black(PS_SOLID, 3, RGB(0, 0, 0));
	memDc.SelectObject(&black);

	if (type == 0) {
		if (p != CPoint(0, 0) && q != CPoint(0, 0)) {
			memDc.Rectangle(p.x, p.y, q.x, q.y);
		}
	}
	else if (type == 1) {
		if (buffCircle.click != CPoint(0, 0) && buffCircle.center != CPoint(0, 0)) {
			memDc.Ellipse(buffCircle.box);
		}

	}
	else if (type == 2) {
		vector<CPoint>::iterator cp;

		CPen pen(PS_SOLID, 3, color);
		memDc.SelectObject(pen);

		//	혹시 모를 오류 방지를 위한 코드 -> vector가 공백일 때.
		if (curve_point.size() != 0) {
			for (cp = curve_point.begin(); cp != curve_point.end() - 1; cp++) {
				memDc.MoveTo(*(cp));
				memDc.LineTo(*(cp + 1));
			}
		}

	}
	else if (type == 3) {

		if (buffStar.click != CPoint(0, 0) && buffStar.center != CPoint(0, 0)) {
			memDc.Polygon(buffStar.starList, 10);
		}
	}

	//	select Region
	if (type == 5 && !isIn(lastPoint)) {
		if (selPoint != CPoint(0, 0) && selPoint2 != CPoint(0, 0)) {
			memDc.SelectStockObject(NULL_BRUSH);
			CPen selPen(PS_DOT, 1, RGB(0, 0, 0));
			memDc.SelectObject(selPen);

			CRect selRegion(selPoint, selPoint2);
			memDc.Rectangle(selRegion);
		}
	}


	CString str1, str2;

	
	
	memDc.SelectStockObject(NULL_BRUSH);

	str1.Format(_T("Number of Shapes : %d"), shapeList.size());
	memDc.TextOutW(5, 5, str1);

	//	선택된 개체 개수
	cnt = 0;
	if (shapeList.size() != 0) {
		for (it = shapeList.begin(); it != shapeList.end(); it++) {
			if ((*it)->isSel) cnt++;
		}
	}
	str2.Format(_T("Number of Selected : %d"), cnt);
	memDc.TextOutW(5, 25, str2);

	//	Double Buffuring
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDc, 0, 0, SRCCOPY);
}



void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{

	if (!(nFlags & MK_SHIFT)||!isSel) {
		//	select shapeList 초기화
		isSel = false;
		list<MyShape*>::iterator it;
		for (it = shapeList.begin(); it != shapeList.end(); it++) {
			(*it)->isSel = false;
		}
	}

	else {
		isMore = true;
		if (!isIn(point)) {
			selPoint = point;
		}
	}

	if (type != 5) {
		color = RGB(rand() % 256, rand() % 256, rand() % 256);
		if (type == 0) {
			p = point;
		}

		else if (type == 1) {
			buffCircle.center = point;
		}

		else if (type == 2) {	//	초기화
			curve_point.clear();
		}

		else if (type == 3) {
			buffStar.center = point;
		}
		SetCapture();
	}

	else {
		selPoint = point;

		if ((nFlags & MK_SHIFT) && isIn(point)) isMore = false;
		if ((nFlags & MK_SHIFT) && !isIn(point)) isMore = true;

		list<MyShape*>::reverse_iterator it;
		for (it = shapeList.rbegin(); it != shapeList.rend(); it++) {

			if ((*it)->type == 0 || (*it)->type == 1) {
				CPoint TL = (*it)->invb;
				CPoint BR = (*it)->bor;

				//	너무 작은거 패스하기
				if (abs(TL.x - BR.x) <= 2 || abs(TL.y - BR.y) <= 2) continue;
				if (TL.x <= point.x && point.x <= BR.x) {
					if (TL.y <= point.y && point.y <= BR.y) {
						isSel = true;
						(*it)->isSel = true;

						Invalidate();
						break;
					}
					else {
						(*it)->isSel = false;
					}
				}
			}

			else if (((*it)->type == 2)) {
				vector<CPoint>::iterator pt;
				MyCurve* tmp = (MyCurve*)(*it);

				for (pt = tmp->curve.begin(); pt != tmp->curve.end(); pt++) {
					float dist = sqrt((point.x - (*pt).x) * (point.x - (*pt).x) + (point.y - (*pt).y) * (point.y - (*pt).y));

					if (dist < 7) {
						isSel = true;
						(*it)->isSel = true;

						Invalidate();
						break;
					}
				}
			}

			else if ((*it)->type == 3) {
				MyStar* tmp = (MyStar*)(*it);
				if (isStar(point, tmp->starList)) {
					isSel = true;
					(*it)->isSel = true;

					Invalidate();
					break;
				}
			}

			else if ((*it)->type == 4) {	//Group
				selParent(*it, point);
				if (isInAll(*it, point)) {
					isSel = true;
					(*it)->isSel = true;
					Invalidate();
					break;
				}

				
			}

			if (nFlags & MK_SHIFT) {
				if(!isSel) isMore = isSel;
			}

		}
	}

	lastPoint = point;

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON) {
		if (type != 5) {	//	Draw

			if (type == 0) {
				q = point;
				if (p.x > q.x) swap(p.x, q.x);
				if (p.y > q.y) swap(p.y, q.y);
			}

			else if (type == 1) {
				buffCircle.click = point;
				buffCircle.update();
			}

			else if (type == 2) {
				curve_point.push_back(point);
			}

			else if (type == 3) {
				buffStar.click = point;
				buffStar.update();
			}

			Invalidate();
		}

		else {	//	Select
			if (isIn(point) && isSel && !isMore) {
				list<MyShape*>::iterator it;
				for (it = shapeList.begin(); it != shapeList.end(); it++) {
					if ((*it)->isSel) {

						if ((*it)->group.size() != 0) {

							(*it)->moveAll(point - selPoint);
						}
						else {
							(*it)->move(point - selPoint);
						}
					}
				}

				selPoint = point;
				Invalidate();
			}
			else {	//	Select Region 확정
				selPoint2 = point;

				SetCapture();
				Invalidate();
			}
		}
	}

	lastPoint = point;

	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (type != 5) {

		//	동적할당을 활용한다.
		if (type == 0) {
			if (p != CPoint(0, 0) && q != CPoint(0, 0)) {
				shapeList.push_back(new MyRect(p, q, color));
			}
			p = CPoint(0, 0);
			q = CPoint(0, 0);
		}

		else if (type == 1) {
			if (buffCircle.click != CPoint(0, 0) && buffCircle.center != CPoint(0, 0)) {
				shapeList.push_back(new MyCircle(buffCircle.center, buffCircle.click, color));
			}

			buffCircle.reset();
		}

		else if (type == 2) {
			shapeList.push_back(new MyCurve(curve_point, color));

			//	재 초기화
			curve_point.clear();
		}

		else if (type == 3) {
			if (buffStar.click != CPoint(0, 0) && buffStar.center != CPoint(0, 0)) {
				shapeList.push_back(new MyStar(buffStar.center, buffStar.click, color));
			}

			buffStar.reset();
		}

		ReleaseCapture();
		Invalidate();
	}
	else {	//	Select Region 해제
		if (!isIn(point)) {
			if (selPoint != CPoint(0, 0) && selPoint2 != CPoint(0, 0)) {
				//	swap (어느 방향으로든 선택 가능)
				if (selPoint.x > selPoint2.x) swap(selPoint.x, selPoint2.x);
				if (selPoint.y > selPoint2.y) swap(selPoint.y, selPoint2.y);

				list<MyShape*>::iterator it;
				for (it = shapeList.begin(); it != shapeList.end(); it++) {
					CPoint i = (*it)->invb;
					CPoint b = (*it)->bor;

					//	Select Box 안에 들어오는 객체들 선택
					if (selPoint.x <= i.x && b.x <= selPoint2.x) {
						if (selPoint.y <= i.y && b.y <= selPoint2.y) {
							isSel = true;
							(*it)->isSel = true;
						}
					}

					Invalidate();
				}
			}
		}

		ReleaseCapture();

		//	Select Box 초기화
		selPoint = CPoint(0, 0);
		selPoint2 = CPoint(0, 0);
		lastPoint = point;
		Invalidate();
	}

	CWnd::OnLButtonUp(nFlags, point);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

// type -> 0 : RECT, 1 : CIRCLE, 2 : CURVE, 3 : STAR, 5 : SELECT

void CChildView::OnDrawRectangle()
{
	type = 0;
}


void CChildView::OnDrawCircle()
{
	type = 1;
}

void CChildView::OnDrawCurve()
{
	type = 2;
}

void CChildView::OnDrawStar()
{
	type = 3;
}

void CChildView::OnActionSelect()
{
	type = 5;
}

void CChildView::OnUpdateDrawRectangle(CCmdUI* pCmdUI)
{
	if (type == 0) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}


void CChildView::OnUpdateDrawCircle(CCmdUI* pCmdUI)
{
	if (type == 1) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}


void CChildView::OnUpdateDrawCurve(CCmdUI* pCmdUI)
{
	if (type == 2) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}


void CChildView::OnUpdateDrawStar(CCmdUI* pCmdUI)
{
	if (type == 3) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}


void CChildView::OnUpdateActionSelect(CCmdUI* pCmdUI)
{
	if (type == 5) pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);
}

void CChildView::OnUpdateAlignBringback(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(isSel);
}


void CChildView::OnUpdateAlignBringfront(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(isSel);
}

void CChildView::OnAlignBringfront()
{
	if (isSel) {
		list<MyShape*>::iterator it;

		for (it = shapeList.begin(); it != shapeList.end(); it++) {
			if ((*it)->isSel) {
				MyShape* tmp = (*it);
				shapeList.erase(it);
				shapeList.push_back(tmp);
				break;
			}
		}
		Invalidate();
	}
}


void CChildView::OnAlignBringback()
{
	if (isSel) {
		list<MyShape*>::iterator it;

		for (it = shapeList.begin(); it != shapeList.end(); it++) {
			if ((*it)->isSel) {
				MyShape* tmp = (*it);
				shapeList.erase(it);
				shapeList.push_front(tmp);
				break;
			}
		}
		Invalidate();
	}
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE && isSel) {
		list<MyShape*>::iterator it;

		for (it = shapeList.begin(); it != shapeList.end();) {
			if ((*it)->isSel) {
				shapeList.erase(it);
				it = shapeList.begin();
			}

			//	이렇게 하지 않으면 모든 원소를 지울 때 작동하지 않음.
			else {
				it++;
			}
			Invalidate();
		}

	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnActionGroup()
{
	list<MyShape*>::iterator it;
	vector<MyShape*> group;

	for (it = shapeList.begin(); it != shapeList.end();) {
		if ((*it)->isSel) {
			group.push_back(*it);
			shapeList.erase(it);
			it = shapeList.begin();
		}
		else {
			it++;
		}
	}

	shapeList.push_back(new MyShape(true, group));

	isMore = false;

	group.clear();
	Invalidate();
}

void CChildView::OnActionUngroup()
{
	list<MyShape*>::iterator it;
	for (it = shapeList.begin(); it != shapeList.end(); it++) {
		if ((*it)->isSel == true && (*it)->group.size() != 0) {
			vector<MyShape*>::iterator pt;
			for (pt = (*it)->group.begin(); pt != (*it)->group.end(); pt++) {
				(*pt)->isSel = true;
				shapeList.push_back(*pt);
			}

			(*it)->group.clear();
			shapeList.erase(it);
			Invalidate();
			break;
		}
	}
}

void CChildView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenuW(IDR_MAINFRAME);

	CMenu* pMenu = menu.GetSubMenu(4);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}


void CChildView::OnUpdateActionGroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(cnt > 1);
}


void CChildView::OnUpdateActionUngroup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(isSel);

	list<MyShape*>::iterator it;
	for (it = shapeList.begin(); it != shapeList.end(); it++) {
		if ((*it)->isSel == true && (*it)->group.size() != 0) {
			pCmdUI->Enable(true);
			break;
		}
	}

}

bool CChildView::isIn(CPoint point) {
	list<MyShape*>::iterator it;
	bool rst = false;
	for (it = shapeList.begin(); it != shapeList.end(); it++) {
		CPoint TL = (*it)->invb;
		CPoint BR = (*it)->bor;
		if ((*it)->type == 0 || (*it)->type == 1) {
			if (TL.x <= point.x && point.x <= BR.x) {
				if (TL.y <= point.y && point.y <= BR.y) {
					if ((*it)->isSel) {
						rst = true;
						break;
					}
				}
			}
		}
		else if ((*it)->type == 2) {	//	곡선
			MyCurve* tmp = (MyCurve*)(*it);
			vector<CPoint>::iterator pt;
			for (pt = tmp->curve.begin(); pt != tmp->curve.end(); pt++) {
				float dist = sqrt((point.x - (*pt).x) * (point.x - (*pt).x) + (point.y - (*pt).y) * (point.y - (*pt).y));

				if (dist < 30 && (*it)->isSel) {
					rst = true;
					break;
				}
			}
		}
		else if ((*it)->type == 3) {
			MyStar* tmp = (MyStar*)(*it);
			if (isStar(point, tmp->starList)) {
				rst = true;
				break;
			}
		}

		else if ((*it)->type == 4) {
			if ((*it)->isSel) {
				rst = isInAll(*it, point);
				if (rst) {
					break;
				}
			}
		}
	}
	return rst;
}

bool CChildView::isInAll(MyShape* tmp, CPoint point) {
	if (tmp->type == 4 && tmp->group.size() == 0) return true;
	vector<MyShape*>::iterator it;

	bool rst = false;
	for (it = tmp->group.begin(); it != tmp->group.end(); it++) {
		CPoint TL = (*it)->invb;
		CPoint BR = (*it)->bor;
		if ((*it)->type == 0 || (*it)->type == 1) {
			if (TL.x <= point.x && point.x <= BR.x) {
				if (TL.y <= point.y && point.y <= BR.y) {
					if ((*it)->isSel) {
						rst = true;
						break;
					}
				}
			}
		}
		else if ((*it)->type == 2) {	//	곡선
			MyCurve* tmp = (MyCurve*)(*it);
			vector<CPoint>::iterator pt;
			for (pt = tmp->curve.begin(); pt != tmp->curve.end(); pt++) {
				float dist = sqrt((point.x - (*pt).x) * (point.x - (*pt).x) + (point.y - (*pt).y) * (point.y - (*pt).y));

				if (dist < 30 && (*it)->isSel) {
					rst = true;
					break;
				}
			}
		}
		else if ((*it)->type == 3) {
			MyStar* tmp = (MyStar*)(*it);
			if (isStar(point, tmp->starList)) {
				rst = true;
				break;
			}
		}

		else if ((*it)->type == 4) {
			if ((*it)->isSel) {
				rst = isInAll(*it, point);
				if (rst) {
					break;
				}
			}
		}
	}

	return rst;
}

bool CChildView::isLeft(CPoint p, CPoint q, CPoint r) {
	float ax = q.x - p.x;
	float ay = q.y - p.y;
	float bx = r.x - p.x;
	float by = r.y - p.y;

	float cross = ax * by - ay * bx;

	if (cross >= 0) return false;
	return true;
}

bool CChildView::isStar(CPoint in, CPoint pt[]) {
	bool rst = false;
	for (int i = 1; i < 11; i += 2) {	//	겉에 있는 삼각형 5개
		if (!isLeft(pt[(i + 1) % 10], pt[(i + 2) % 10], in)) {
			if (!isLeft(pt[(i + 2) % 10], pt[(i + 3) % 10], in)) {
				if (!isLeft(pt[(i + 3) % 10], pt[(i + 1) % 10], in)) {
					rst = true;
				}
			}
		}
	}

	if (!rst) {	//	내부 확인
		rst = true;
		for (int i = 1; i < 11; i += 2) {
			if (isLeft(pt[(i + 1) % 10], pt[(i + 2) % 10], in)) {
				rst = false;
			}
		}
	}

	return rst;
}

void CChildView::printAll(MyShape* G, CDC* memDc) {
	if (G->type == 4 && G->group.size() == 0) return;
	vector<MyShape*>::iterator it;

	for (it = G->group.begin(); it != G->group.end(); it++) {
		//	재귀함수
		if ((*it)->isG) {
			printAll((*it), memDc);
		}
		CBrush brush((*it)->c);
		memDc->SelectObject(&brush);
		CPen black(PS_SOLID, 3, RGB(0, 0, 0));
		memDc->SelectObject(&black);

		if ((*it)->type == 0) {
			memDc->Rectangle((*it)->box);
		}

		else if ((*it)->type == 1) {
			memDc->Ellipse((*it)->box);
		}

		else if ((*it)->type == 2) {	//	curve

			//	다운 캐스팅
			CPen pen(BS_SOLID, 3, (*it)->c);
			memDc->SelectObject(pen);

			vector<CPoint>::iterator cp;
			MyCurve* tmp = (MyCurve*)(*it);

			for (cp = tmp->curve.begin(); cp != tmp->curve.end() - 1; cp++) {
				memDc->MoveTo(*(cp));
				memDc->LineTo(*(cp + 1));
			}
		}

		else if ((*it)->type == 3) {
			MyStar* tmp = (MyStar*)(*it);

			//	polygon을 이용해야 외곽선을 칠할 수 있음
			memDc->Polygon(tmp->starList, 10);
		}

		//	외곽선
		memDc->SelectStockObject(NULL_BRUSH);
		CPen selPen(PS_DOT, 1, RGB(255, 0, 0));
		memDc->SelectObject(selPen);

		if ((*it)->isSel && (*it)->parent == NULL) {
			memDc->Rectangle((*it)->selBox);
		}
	}
}

bool CChildView::selParent(MyShape* tmp, CPoint point) {
	if (tmp->type == 4 && tmp->group.size() == 0) return false;

	vector<MyShape*>::reverse_iterator it;

	for (it = tmp->group.rbegin(); it != tmp->group.rend(); it++) {

		if ((*it)->type == 0 || (*it)->type == 1) {
			CPoint TL = (*it)->invb;
			CPoint BR = (*it)->bor;
			//	너무 작은거 패스하기
			if (abs(TL.x - BR.x) <= 3 || abs(TL.y - BR.y) <= 3) continue;
			if (TL.x <= point.x && point.x <= BR.x) {
				if (TL.y <= point.y && point.y <= BR.y) {
					isSel = true;
					(*it)->parent->isSel = true;
				}
			}
		}

		else if (((*it)->type == 2)) {
			vector<CPoint>::iterator pt;
			MyCurve* tmp = (MyCurve*)(*it);

			for (pt = tmp->curve.begin(); pt != tmp->curve.end(); pt++) {
				float dist = sqrt((point.x - (*pt).x) * (point.x - (*pt).x) + (point.y - (*pt).y) * (point.y - (*pt).y));

				if (dist < 7) {
					isSel = true;
					(*it)->parent->isSel = true;
				}
			}
		}

		else if ((*it)->type == 3) {
			MyStar* tmp = (MyStar*)(*it);
			if (isStar(point, tmp->starList)) {
				isSel = true;
				(*it)->parent->isSel = true;
			}
		}

		else if ((*it)->type == 4) {	//Group
			(*it)->parent->isSel = selParent(*it, point);
		}
	}

	return isSel;
}

