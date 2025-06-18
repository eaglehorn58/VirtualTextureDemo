#include "GradientSlider.h"
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>



class Vernier : public QWidget
{
	//Q_OBJECT
public:
	enum class EType
	{
		Min,
		Max
	};
	// Triangle w/h
	static const int s_iHeight = 16;
	static const int s_iHalfWidth = 8;

	Vernier(EType eType, GradientSlider *parent)
		: QWidget(parent)
		, m_pParent(parent)
		, m_eType(eType)
		, m_bPressed(false)
		, m_fPosX(0.0)
		, m_iLastGobalPosX(0)
		, m_iCurParentGeometryLeft(0)
		, m_iCurParentGeometryRight(0)
	{
		switch (m_eType)
		{
		case EType::Min:
		{
			m_qColor = Qt::black;
		}
		break;
		case EType::Max:
		{
			m_qColor = Qt::white;
		}
		break;
		default:
		{
			Q_ASSERT(0 && "illegal Vernier::EType");
		}
		break;
		}
		int w = s_iHalfWidth * 2;
		m_polygon.resize(3);
		m_polygon.setPoint(0, QPoint(s_iHalfWidth, 0));
		m_polygon.setPoint(1, QPoint(0, s_iHeight));
		m_polygon.setPoint(2, QPoint(w, s_iHeight));

		setFixedSize(w + 1, s_iHeight + 1);
	}

	~Vernier()
	{}

	void SetPosX(qreal fPosX)
	{
		m_fPosX = qBound(0.0, fPosX, 1.0);
		UpdateUI();
	}

	qreal GetPosX() const
	{
		Q_ASSERT(0.0 <= m_fPosX && m_fPosX <= 1.0);
		return m_fPosX;
	}

	void UpdateUI()
	{
		move(m_pParent->m_iLocalStartX + m_fPosX * m_pParent->GetGradientWidth() - s_iHalfWidth, m_pParent->height() - s_iHeight - 1);
	}

	EType GetType() const { return m_eType; }

protected:
	virtual void paintEvent(QPaintEvent * e) override
	{
		QPainter painter(this);
		painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.setBrush(m_qColor);
		painter.drawPolygon(m_polygon);
	}
	virtual void mousePressEvent(QMouseEvent * e) override
	{
		if (m_polygon.containsPoint(mapFromGlobal(e->globalPos()), Qt::OddEvenFill))
		{
			m_bPressed = true;
			m_iLastGobalPosX = e->globalPos().x();
			m_iCurParentGeometryLeft = m_pParent->m_iLocalStartX + m_pParent->mapToGlobal(m_pParent->rect().topLeft()).x();
			m_iCurParentGeometryRight = m_pParent->m_iLocalEndX + m_iCurParentGeometryLeft;

			raise();
		}
		else
		{
			lower();
			e->setAccepted(false);//!!!!
			//return QWidget::mousePressEvent(e);
		}
	}
	virtual void mouseMoveEvent(QMouseEvent * e) override
	{
		if (m_bPressed)
		{
			int iCurPosX = e->globalPos().x();

			auto PosChanged_ = [this, iCurPosX]()
			{
				m_iLastGobalPosX = iCurPosX;
				m_pParent->ValidateOtherVernier(this);
				m_pParent->emit_valuesChanged();
			};

			if (m_iCurParentGeometryLeft > iCurPosX)
			{
				SetPosX(0.0);
				PosChanged_();
			}
			else if (iCurPosX > m_iCurParentGeometryRight)
			{
				SetPosX(1.0);
				PosChanged_();
			}
			else
			{
				Q_ASSERT(m_iCurParentGeometryLeft <= iCurPosX && iCurPosX <= m_iCurParentGeometryRight);
				int iDeltaX = iCurPosX - m_iLastGobalPosX;
				if (iDeltaX != 0)
				{
					qreal fDeltaX = (qreal)iDeltaX / m_pParent->GetGradientWidth();
					SetPosX(m_fPosX + fDeltaX);
					PosChanged_();
				}
			}
		}
	}
	virtual void mouseReleaseEvent(QMouseEvent * e) override
	{
		m_bPressed = false;

		//m_pParent->emit_valuesChanged();
	}

protected:
	GradientSlider* const m_pParent;
	const EType m_eType;
	QPolygon m_polygon;
	QColor m_qColor;
	bool m_bPressed;
	qreal m_fPosX;
	int m_iLastGobalPosX;
	int m_iCurParentGeometryLeft;
	int m_iCurParentGeometryRight;
};

GradientSlider::GradientSlider(QWidget *parent)
	: QWidget(parent)
	, m_iLocalStartX(0)
	, m_iLocalEndX(0)
	, m_iLocalStartY(0)
	, m_iLocalEndY(0)
	//, m_iFixedHeight(64)
{
	//setFixedHeight(m_iFixedHeight);

	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);

	m_pVernierMin = new Vernier(Vernier::EType::Min, this);
	m_pVernierMax = new Vernier(Vernier::EType::Max, this);
	m_pVernierMin->SetPosX(0.0);
	m_pVernierMax->SetPosX(1.0);
}

GradientSlider::~GradientSlider()
{
}

//QSize GradientSlider::minimumSizeHint() const
//{
//	return QSize(10, m_iFixedHeight);
//}
//
//QSize GradientSlider::sizeHint() const
//{
//	return QSize(10000, m_iFixedHeight);
//}

void GradientSlider::paintEvent(QPaintEvent * /*e*/)
{
	QPainter painter(this);

	painter.setPen(Qt::NoPen);
	painter.setBrush(parentWidget()->palette().background().color());
	painter.drawRect(this->rect());

	const QRect rect(QPoint(m_iLocalStartX, m_iLocalStartY), QPoint(m_iLocalEndX - 1, m_iLocalEndY - 1));
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin)); //border
	QLinearGradient linearGradient(rect.topLeft(), rect.topRight());
	linearGradient.setColorAt(0.0, Qt::black);
	linearGradient.setColorAt(1.0, Qt::black);
	linearGradient.setColorAt(GetMin(), Qt::white);
	linearGradient.setColorAt(GetMax(), Qt::white);
	painter.setBrush(linearGradient);
	painter.drawRect(rect);

	update();
}

void GradientSlider::resizeEvent(QResizeEvent * /*e*/)
{
	m_iLocalStartX = Vernier::s_iHalfWidth;
	m_iLocalEndX = this->width() - Vernier::s_iHalfWidth;
	m_iLocalStartY = 2;
	m_iLocalEndY = this->height() - Vernier::s_iHeight;

	m_pVernierMin->UpdateUI();
	m_pVernierMax->UpdateUI();
}

void GradientSlider::ValidateOtherVernier(const Vernier* pCurVernier)
{
	Q_ASSERT(pCurVernier);
	switch (pCurVernier->GetType())
	{
	case Vernier::EType::Min:
	{
		Q_ASSERT(pCurVernier == m_pVernierMin);
		qreal fMin = GetMin();
		if (GetMax() < fMin)
		{
			m_pVernierMax->SetPosX(fMin);
		}
	}
	break;
	case Vernier::EType::Max:
	{
		Q_ASSERT(pCurVernier == m_pVernierMax);
		qreal fMax = GetMax();
		if (GetMin() > fMax)
		{
			m_pVernierMin->SetPosX(fMax);
		}
	}
	break;
	default:
	{
		Q_ASSERT(0 && "illegal Vernier::EType");
	}
	break;
	}
}

void GradientSlider::emit_valuesChanged()
{
	emit valuesChanged(GetMin(), GetMax());
}

qreal GradientSlider::GetMin() const
{
	return m_pVernierMin->GetPosX();
}
qreal GradientSlider::GetMax() const
{
	return m_pVernierMax->GetPosX();
}

void GradientSlider::SetMin(qreal min)
{
	m_pVernierMin->SetPosX(min);
}
void GradientSlider::SetMax(qreal max)
{
	m_pVernierMax->SetPosX(max);
}

