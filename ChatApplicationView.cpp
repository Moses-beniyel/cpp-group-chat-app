
// ChatApplicationView.cpp : implementation of the CChatApplicationView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ChatApplication.h"
#endif

#include "ChatApplicationDoc.h"
#include "ChatApplicationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatApplicationView

IMPLEMENT_DYNCREATE(CChatApplicationView, CView)

BEGIN_MESSAGE_MAP(CChatApplicationView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChatApplicationView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CChatApplicationView construction/destruction

CChatApplicationView::CChatApplicationView() noexcept
{
	// TODO: add construction code here

}

CChatApplicationView::~CChatApplicationView()
{
}

BOOL CChatApplicationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CChatApplicationView drawing

void CChatApplicationView::OnDraw(CDC* /*pDC*/)
{
	CChatApplicationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CChatApplicationView printing


void CChatApplicationView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CChatApplicationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CChatApplicationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CChatApplicationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CChatApplicationView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CChatApplicationView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CChatApplicationView diagnostics

#ifdef _DEBUG
void CChatApplicationView::AssertValid() const
{
	CView::AssertValid();
}

void CChatApplicationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChatApplicationDoc* CChatApplicationView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChatApplicationDoc)));
	return (CChatApplicationDoc*)m_pDocument;
}
#endif //_DEBUG


// CChatApplicationView message handlers
