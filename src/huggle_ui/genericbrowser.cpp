//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#include "genericbrowser.hpp"
#include <huggle_core/exception.hpp>
#include <huggle_core/generic.hpp>
#include <huggle_core/localization.hpp>
#include <huggle_core/syslog.hpp>
#include <huggle_core/configuration.hpp>
#include <huggle_core/wikipage.hpp>
#include <huggle_core/wikisite.hpp>
#include <huggle_core/wikiedit.hpp>
#include <huggle_core/resources.hpp>

using namespace Huggle;

GenericBrowser::GenericBrowser(QWidget *parent) : QFrame(parent)
{
    this->CurrentPage = _l("browser-none");
}

GenericBrowser::~GenericBrowser()
{
}

QString GenericBrowser::CurrentPageName()
{
    return this->CurrentPage;
}

void GenericBrowser::DisplayPreFormattedPage(WikiPage *page)
{
    if (page == nullptr)
    {
        throw new Huggle::NullPointerException("WikiPage *page", BOOST_CURRENT_FUNCTION);
    }
    this->DisplayPage(QString(Configuration::GetProjectScriptURL(page->Site) + "index.php?title=" + page->PageName + "&action=render"));
    this->CurrentPage = page->PageName;
}

void GenericBrowser::DisplayPreFormattedPage(QString url)
{
    url += "&action=render";
    this->DisplayPage(url);
    //this->CurrentPage = this->ui->webView->title();
}

QString GenericBrowser::GetShortcut()
{
    QString incoming = Resources::HtmlIncoming;
    if (!Configuration::HuggleConfiguration->Shortcuts.contains("main-mytalk"))
        throw new Huggle::Exception("key", BOOST_CURRENT_FUNCTION);
    Shortcut sh = Configuration::HuggleConfiguration->Shortcuts["main-mytalk"];
    incoming.replace("$shortcut", sh.QAccel);
    return incoming;
}

static QString Extras(WikiEdit *e)
{
    QString result = "";
    if (e->MetaLabels.count() || e->Tags.count())
    {
        result += "<br><small><b>Meta information -</b> ";
        if (e->MetaLabels.count() > 0)
        {
            foreach (QString label, e->MetaLabels.keys())
            {
                result += "<b>" + label + ":</b> " + e->MetaLabels[label] + " ";
            }
        }
        if (e->Tags.count() > 0)
        {
            result += "<b>Tags:</b> ";
            foreach (QString tag, e->Tags)
            {
                result += tag + ", ";
            }
            if (result.endsWith(", "))
                result = result.mid(0, result.size() - 2);
        }
        result += "</small>";
    }
    return result;
}

static QString GenerateEditSumm(WikiEdit *edit)
{
    QString prefix = "<b>" + _l("summary") + ":</b> ";
    if (edit->IsRangeOfEdits())
    {
        return _l("summary-edit-range", QString::number(edit->RevID), edit->DiffTo);
    }
    if (edit->Summary.isEmpty())
    {
        if (hcfg->UserConfig->SummaryMode)
            return prefix + _l("browser-miss-summ");
        else
            return prefix + "<font color=red> " + _l("browser-miss-summ") + "</font>";
    } else
    {
        if (hcfg->UserConfig->SummaryMode)
            return prefix + "<font color=red> " + Generic::HtmlEncode(edit->Summary) + "</font>";
        else
            return prefix + "<font> " + Generic::HtmlEncode(edit->Summary) + "</font>";
    }
}

void GenericBrowser::DisplayDiff(WikiEdit *edit)
{
    this->CurrentEdit = edit;
    if (edit == nullptr)
        throw new Huggle::NullPointerException("WikiEdit *edit", BOOST_CURRENT_FUNCTION);
    if (edit->Page == nullptr)
        throw new Huggle::NullPointerException("*edit->Page", BOOST_CURRENT_FUNCTION);
    if (edit->NewPage && !edit->Page->Contents.size())
    {
        this->RenderHtml(_l("browser-load"));
        this->DisplayPreFormattedPage(edit->Page);
        return;
    } else if (edit->NewPage)
    {
        this->DisplayNewPageEdit(edit);
        return;
    }
    if (!edit->DiffText.length())
    {
        Huggle::Syslog::HuggleLogs->WarningLog("unable to retrieve diff for edit " + edit->Page->PageName + " fallback to web rendering");
        this->RenderHtml(_l("browser-load"));
        if (edit->DiffTo != "prev")
        {
            this->DisplayPage(QString(Configuration::GetProjectScriptURL(edit->Page->Site) + "index.php?title=" + edit->Page->PageName + "&diff="
                                        + QString::number(edit->Diff) + "&oldid=" + edit->DiffTo + "&action=render"));
        } else
        {
            this->DisplayPage(QString(Configuration::GetProjectScriptURL(edit->Page->Site) + "index.php?title=" + edit->Page->PageName + "&diff="
                                        + QString::number(edit->Diff) + "&action=render"));
        }
        return;
    }
    QString HTML = Resources::GetHtmlHeader();
    if (Configuration::HuggleConfiguration->NewMessage)
    {
        // we display a notification that user received a new message
        HTML += this->GetShortcut();
    }
    HTML += Resources::DiffHeader + "<tr><td colspan=2>";
    if (Configuration::HuggleConfiguration->UserConfig->DisplayTitle)
    {
        HTML += "<p><font size=20px>" + Generic::HtmlEncode(edit->Page->PageName) + "</font></p>";
    }
    QString Summary = GenerateEditSumm(edit);
    QString size;
    if (edit->SizeIsKnown)
    {
        if (edit->GetSize() > 0)
            size = "<font color=green>+" + QString::number(edit->GetSize()) + "</font>";
        else if (edit->GetSize() == 0)
            size = "<font color=blue>" + QString::number(edit->GetSize()) + "</font>";
        else
            size = "<font color=\"red\">" + QString::number(edit->GetSize()) + "</font>";
    } else
    {
        size = "<font color=red>Unknown</font>";
    }
    Summary += "<b> Size change: " + size + "</b>";
    HTML += Summary + Extras(edit) + "</td></tr>" + edit->DiffText +
            Resources::DiffFooter + Resources::HtmlFooter;
    this->RenderHtml(HTML);
}

void GenericBrowser::DisplayNewPageEdit(WikiEdit *edit)
{
    if (!edit)
        throw new Huggle::NullPointerException("WikiEdit *edit", BOOST_CURRENT_FUNCTION);

    this->CurrentEdit = edit;
    QString HTML = Resources::GetHtmlHeader();
    if (Configuration::HuggleConfiguration->NewMessage)
    {
        // we display a notification that user received a new message
        HTML += this->GetShortcut();
    }
    if (Configuration::HuggleConfiguration->UserConfig->DisplayTitle)
    {
        HTML += "<p><font size=20px>" + Generic::HtmlEncode(edit->Page->PageName) + "</font></p>";
    }
    QString Summary = GenerateEditSumm(edit);
    HTML += Summary + Extras(edit) + "<br>" + edit->Page->Contents + Resources::HtmlFooter;
    this->RenderHtml(HTML);
}

