#pragma once

#include <QtWidgets/QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT GradientSlider : public QWidget
{
	Q_OBJECT
		friend class Vernier;
public:
	GradientSlider(QWidget *parent = Q_NULLPTR);
	~GradientSlider();

	//virtual QSize minimumSizeHint() const override;
	//virtual QSize sizeHint() const override;

	qreal GetMin() const;
	qreal GetMax() const;
	void SetMin(qreal min);
	void SetMax(qreal max);

signals:
	void valuesChanged(qreal min, qreal max);

protected:
	int GetGradientWidth() const { return m_iLocalEndX - m_iLocalStartX; }
	void ValidateOtherVernier(const Vernier* pCurVernier);
	void emit_valuesChanged();

	virtual void paintEvent(QPaintEvent * e) override;
	virtual void resizeEvent(QResizeEvent * e) override;

protected:
	int m_iLocalStartX;
	int m_iLocalEndX;
	int m_iLocalStartY;
	int m_iLocalEndY;

	Vernier* m_pVernierMin;
	Vernier* m_pVernierMax;

	//const int m_iFixedHeight;
};

