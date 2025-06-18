//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _DLGGENVIRTUALTEXTURE_H_
#define _DLGGENVIRTUALTEXTURE_H_

#include <QDialog>
#include "ui_GenVirtualTexture.h"

class CDlgGenVirtualTexture : public QDialog
{
	Q_OBJECT

public:

	QString		m_strVTFile;		//	VT File name
	int			m_iVTexSize;
	int			m_iIndirTexSize;
	int			m_iVPageSize;
	int			m_iOriginTexSize;

public:

	CDlgGenVirtualTexture(QWidget* parent);
	virtual ~CDlgGenVirtualTexture();

protected:

	Ui_DlgGenVirtualTexture	ui;

protected:

	//	Re-calculate indirect map size
	void RecalcIndirectMapSize();

protected slots:

	void OnBrowseFile();
	void OnBrowseSrcTexFile();
	void OnVPageSizeChanged(int index);
	void OnOriTexSizeChanged(int index);
	void OnAddBorderToTex();
	virtual void accept() override;
};


#endif	//	_DLGGENVIRTUALTEXTURE_H_