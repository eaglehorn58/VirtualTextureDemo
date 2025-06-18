#include "GradientSlider.h"
#include "GradientSliderPlugin.h"

#include <QtCore/QtPlugin>

GradientSliderPlugin::GradientSliderPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void GradientSliderPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool GradientSliderPlugin::isInitialized() const
{
	return initialized;
}

QWidget *GradientSliderPlugin::createWidget(QWidget *parent)
{
	return new GradientSlider(parent);
}

QString GradientSliderPlugin::name() const
{
	return "GradientSlider";
}

QString GradientSliderPlugin::group() const
{
	return "My Plugins";
}

QIcon GradientSliderPlugin::icon() const
{
	return QIcon();
}

QString GradientSliderPlugin::toolTip() const
{
	return QString();
}

QString GradientSliderPlugin::whatsThis() const
{
	return QString();
}

bool GradientSliderPlugin::isContainer() const
{
	return false;
}

QString GradientSliderPlugin::domXml() const
{
	return "<widget class=\"GradientSlider\" name=\"gradientSlider\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString GradientSliderPlugin::includeFile() const
{
	return "GradientSlider.h";
}
