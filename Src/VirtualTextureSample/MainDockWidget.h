//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _MAINDOCKWIDGET_H_
#define _MAINDOCKWIDGET_H_

#include "qdockwidget.h"

class QTabWidget;
class CTabPageWidget;

class CMainDockWidget : public QDockWidget
{
	Q_OBJECT

public:

	//	Pages
	enum
	{
		PAGE_GENERAL = 0,
		NUM_PAGE,
	};

public:

	CMainDockWidget(QWidget *parent, Qt::WindowFlags flags);

public:

	//	Update UI
	//	bAllPages: true, force to update all pages; otherwise false means only update current active page
	void UpdateUI(bool bAllPages=false);

protected:

	QTabWidget*			m_pTab;		//	Tab widget
	CTabPageWidget*		m_aPages[NUM_PAGE];

protected:

protected slots:

	void OnCurrentPageChanged(int index);
};

#endif // _MAINDOCKWIDGET_H_
