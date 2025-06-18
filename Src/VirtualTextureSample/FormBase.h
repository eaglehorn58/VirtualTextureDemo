//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _FORMBASE_H_
#define _FORMBASE_H_

#include "qwidget.h"

//	Initialize slider controller
#define GLB_SLIDER_INIT(slider, val, max, max_range) \
	{ \
		int cur_pos = (int)(((float)(val) / (max)) * max_range + 0.1f); \
		a_Clamp(cur_pos, 0, max_range); \
		slider->setRange(0, max_range); \
		slider->setPageStep(max_range / 10); \
		slider->setSliderPosition(cur_pos); \
	}

//	update slider controller
#define GLB_SLIDER_UPDATE(slider, val, min_val, max_val, max_range) \
	{ \
		a_Clamp(val, min_val, max_val); \
		float delta = val - min_val; \
		int cur_pos = (int)((delta / max_val) * max_range + 0.1f); \
		slider->setSliderPosition(cur_pos); \
	}

////////////////////////////////////////////////////////////////////////////
//
//	class CFormBase
//
////////////////////////////////////////////////////////////////////////////

class CFormBase : public QWidget
{
	Q_OBJECT

public:

	CFormBase(QWidget* parent) : QWidget(parent) {}

public:

	//	Update UI
	virtual void UpdateUI() {}

protected:

protected:

};


#endif	//	_FORMBASE_H_
