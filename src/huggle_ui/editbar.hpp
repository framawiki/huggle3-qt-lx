//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef EDITBAR_HPP
#define EDITBAR_HPP

#include <huggle_core/definitions.hpp>
#include <QDockWidget>
#include <QList>
#include <QTimer>
#include <huggle_core/edittype.hpp>

namespace Ui
{
    class EditBar;
}

namespace Huggle
{
    class WikiEdit;
    class WikiPage;
    class WikiPageHistoryItem;
    class UserInfoFormHistoryItem;
    class WikiUser;
    class EditBarItem;

    /*!
     * \brief The EditBar widget can be used to render various edit information of pages, like
     *        the time and username of the last edit and display them as icons
     */
    class HUGGLE_EX_UI EditBar : public QDockWidget
    {
            Q_OBJECT
        public: 
	    /*!
	     * \brief EditBar Creates a new instance of the EditBar class
	     * \param parent The parent widget in which the EditBar is contained
	     */
            explicit EditBar(QWidget *parent = 0);
            ~EditBar();
            //! Removes all pages and user contributions in the EditBar
            void RemoveAll();
            //! Refresh page history and user information in the EditBar
            void Refresh();
            //! Reloads page history from the EditBar
            void RefreshPage();
            //! Reloads all user contributions in the EditBar
            void RefreshUser();
        private slots:
            void on_pushButton_2_clicked();
            void on_pushButton_clicked();
            void on_pushButton_4_clicked();
            void on_pushButton_3_clicked();
            void OnReload();
            void on_EditBar_visibilityChanged(bool visible);
        private:
            void InsertEdit(WikiPageHistoryItem *page, int RowId);
            void InsertUser(UserInfoFormHistoryItem *user);
            void MovePage(int size);
            void ClearUser();
            void ClearPage();
            void MoveUser(int size);
            bool needsRefresh = false;
            int PageSX;
            int UserSX;
            QTimer timer;
            QList<EditBarItem*> Items;
            Ui::EditBar *ui;
    };
}

#endif // EDITBAR_HPP
