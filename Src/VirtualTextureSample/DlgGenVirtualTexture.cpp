//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "Global.h"
#include "DlgGenVirtualTexture.h"
#include "qvalidator.h"
#include "qfiledialog.h"
#include "DirectXTex.h"
#include "VTextureTypes.h"
#include "VTextureStream.h"
#include "VTextureGenerator.h"
#include "AFI.h"

struct TEX_SIZE
{
	const char*	szText;
	int			iTexSize;
};

const TEX_SIZE g_aTexSize[] =
{
	{ "32", 32 },
	{ "64", 64 },
	{ "128", 128 },
	{ "256", 256 },
	{ "512", 512 },
	{ "1024", 1024 },
	{ "2048", 2048 },
};

CDlgGenVirtualTexture::CDlgGenVirtualTexture(QWidget* parent) :
QDialog(parent),
m_iVTexSize(0),
m_iIndirTexSize(0),
m_iVPageSize(128),
m_iOriginTexSize(1024)
{
	ui.setupUi(this);

	this->setMinimumSize(this->size());
	this->setMaximumSize(this->size());

	ui.editVTexSize->setText(QString::number(m_iVTexSize));
	ui.editIndirMapSize->setText(QString::number(m_iIndirTexSize));

	const int iNumTexSize = sizeof(g_aTexSize) / sizeof(g_aTexSize[0]);
	for (int i = 0; i < iNumTexSize; i++)
	{
		ui.comboVPageSize->addItem(g_aTexSize[i].szText);
		ui.comboOriTexSize->addItem(g_aTexSize[i].szText);

		if (g_aTexSize[i].iTexSize == m_iVPageSize)
		{
			ui.comboVPageSize->setCurrentIndex(i);
		}

		if (g_aTexSize[i].iTexSize == m_iOriginTexSize)
		{
			ui.comboOriTexSize->setCurrentIndex(i);
		}
	}

	connect(ui.btnBrowse, SIGNAL(released()), this, SLOT(OnBrowseFile()));
	connect(ui.btnBrowseSrcTexFile, SIGNAL(released()), this, SLOT(OnBrowseSrcTexFile()));
	connect(ui.comboVPageSize, SIGNAL(currentIndexChanged(int)), this, SLOT(OnVPageSizeChanged(int)));
	connect(ui.comboOriTexSize, SIGNAL(currentIndexChanged(int)), this, SLOT(OnOriTexSizeChanged(int)));
	connect(ui.btnAddBorderToTex, SIGNAL(released()), this, SLOT(OnAddBorderToTex()));
}

CDlgGenVirtualTexture::~CDlgGenVirtualTexture()
{
}

void CDlgGenVirtualTexture::OnBrowseSrcTexFile()
{
	QString str = QFileDialog::getOpenFileName(this, tr("Load Texture File"), "", "DDS Files (*.dds)");
	ui.editSrcTexFile->setText(str);
}

void CDlgGenVirtualTexture::OnAddBorderToTex()
{
	MyAssert(m_iVPageSize);

	QString strSrcTexFile = ui.editSrcTexFile->text();
	if (!strSrcTexFile.length())
	{
		glb_MessageBox("Error", "Input a valid texture file!");
		return;
	}

	char szPreFile[MAX_PATH];
	strcpy(szPreFile, strSrcTexFile.toLocal8Bit());
	af_ChangeFileExt(szPreFile, MAX_PATH, "");

	CVTextureGenerator vtGen;
	vtGen.Init();
	
	char szFile[MAX_PATH];
	int iCount = 0;

	while (1)
	{
		if (!iCount)
		{
			strcpy(szFile, strSrcTexFile.toLocal8Bit());
		}
		else
		{
			sprintf(szFile, "%s%02d.dds", szPreFile, iCount);
		}

		if (!afx_IsFileExist(szFile))
		{
			glb_MessageBox("Succeed", "Total [%d] file processed!", iCount);
			break;
		}

		if (!vtGen.ProcessVT(szFile, m_iVPageSize, VTexture::PAGE_BORDER_SIZE))
		{
			glb_MessageBox("Error", "Failed to process file [%s]!", szFile);
			break;
		}

		iCount++;
	}

	vtGen.Release();
}

void CDlgGenVirtualTexture::OnBrowseFile()
{
	QString str = QFileDialog::getOpenFileName(this, tr("Load Texture File"), "", "DDS Files (*.dds)");
	QByteArray strBytes = str.toLocal8Bit();
	auint16 szWFile[MAX_PATH];
	a_GB2312ToUTF16(szWFile, strBytes.data(), MAX_PATH);

	//	Get dds file info
	DirectX::TexMetadata metadata;
	HRESULT hr = DirectX::GetMetadataFromDDSFile((LPCWSTR)szWFile, DirectX::DDS_FLAGS_FORCE_RGB, metadata);
	if (FAILED(hr))
	{
		glb_MessageBox("Error", "Failed to open file!");
		return;
	}

	if (metadata.width != metadata.height || !glb_Is2Power(metadata.width))
	{
		glb_MessageBox("Error", "A square texture is required and its width needs to be power of 2!");
		return;
	}

	m_strVTFile = str;
	ui.editFileName->setText(m_strVTFile);

	m_iVTexSize = metadata.width;
	ui.editVTexSize->setText(QString::number(m_iVTexSize));

	//	Re-calculate indirect map size
	RecalcIndirectMapSize();
}

void CDlgGenVirtualTexture::OnVPageSizeChanged(int index)
{
	m_iVPageSize = g_aTexSize[index].iTexSize;
	//	Re-calculate indirect map size
	RecalcIndirectMapSize();
}

void CDlgGenVirtualTexture::OnOriTexSizeChanged(int index)
{
	m_iOriginTexSize = g_aTexSize[index].iTexSize;
}

void CDlgGenVirtualTexture::RecalcIndirectMapSize()
{
	m_iIndirTexSize = m_iVTexSize / m_iVPageSize;
	ui.editIndirMapSize->setText(QString::number(m_iIndirTexSize));
}

void CDlgGenVirtualTexture::accept()
{
	//	Check project name
	if (!m_strVTFile.length())
	{
		glb_MessageBox("Error", "Input a valid texture file!");
		return;
	}

	MyAssert(m_iVTexSize && m_iVPageSize);

	QByteArray bytesFile = m_strVTFile.toLocal8Bit();

	if (!CVTextureStream::GenVirtualTexture(bytesFile.data(), m_iVTexSize, m_iVPageSize, m_iIndirTexSize, m_iOriginTexSize))
	{
		glb_MessageBox("Error", "Failed to generate virtual texture!");
		return;
	}

	glb_MessageBox("Succeed", "Virtual Texture was generated!");

	QDialog::accept();
}



