//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "hw.hpp"
#include <huggle_core/configuration.hpp>
#include <huggle_core/exception.hpp>
#include <huggle_core/generic.hpp>
#include <huggle_core/localization.hpp>
#include <huggle_core/syslog.hpp>
#include <QStringList>
#include <QWidget>
#include <QFile>

using namespace Huggle;

HW::HW(QString window_name, QWidget *widget, QWidget *parent) : QDialog(parent)
{
    this->HW_Name = window_name;
    this->HW_Geometry = Configuration::GetConfigurationPath() + this->HW_Name + "_geometry";
    this->HW_Widget = widget;
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

HW::~HW()
{
    if (!Huggle::Configuration::HuggleConfiguration)
        return;
    // Check if we want to store the layout to file
    if (Huggle::Configuration::HuggleConfiguration->SystemConfig_SaveLayout == false)
        return;
    QFile *layout = new QFile(this->HW_Geometry);
    if (!layout->open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        HUGGLE_ERROR("Unable to write geometry to a config file for " + this->HW_Name + " @" + BOOST_CURRENT_FUNCTION);
    } else
    {
        layout->write(this->HW_Widget->saveGeometry());
    }
    layout->close();
    delete layout;
}

void HW::RestoreWindow()
{
    if (QFile().exists(this->HW_Geometry))
    {
        QFile *layout = new QFile(this->HW_Geometry);
        if (!layout->open(QIODevice::ReadOnly))
            Syslog::HuggleLogs->ErrorLog(_l("main-config-geom-fail", this->HW_Name));
        else if (!this->HW_Widget->restoreGeometry(layout->readAll()))
            HUGGLE_DEBUG1("Failed to restore layout of " + this->HW_Name);
        layout->close();
        delete layout;
    }
}


