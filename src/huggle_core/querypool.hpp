//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef QUERYPOOL_HPP
#define QUERYPOOL_HPP

#include "definitions.hpp"

#include <QList>

#define HUGGLE_QP_APPEND(id) if (Huggle::QueryPool::HugglePool)\
           { Huggle::QueryPool::HugglePool->AppendQuery(id); }

namespace Huggle
{
    class ApiQuery;
    class EditQuery;
    class Message;
    class ProcessList;
    class WikiEdit;
    class Query;

    //! Pool of all queries that are monitored by huggle itself

    //! Not every query is contained here, only these that are managed by something
    //! it's generally a good idea to insert every query to this pool
    class HUGGLE_EX_CORE QueryPool
    {
        public:
            static QueryPool *HugglePool;
            QueryPool();
            ~QueryPool();

            /*!
             * \brief Insert a query to internal list of running queries, so that they can be watched
             * This will insert it to a process list in main form
             * \param item Query that is about to be inserted to list of running queries
             */
            void AppendQuery(Query* item);
            void CheckQueries();
            //! Check the edit summary and similar in order to
            //! determine several edit attributes etc
            void PreProcessEdit(WikiEdit *edit);
            //! Perform more expensive tasks to finalize
            //! edit processing
            void PostProcessEdit(WikiEdit *edit);
            int RunningQueriesGetCount();
            int GetRunningEditingQueries();
#ifdef HUGGLE_METRICS
            //! Return average response time, or -1 in case it's unknown.
            qint64 GetAverageExecutionTime();
#endif
            //! List of all messages that are being sent
            QList<Message*> Messages;
            //! Pending changes
            QList<EditQuery*> PendingMods;
            //! We need to store some recent reverts for wiki provider so that we can backward decide if edit
            //! was reverted before we parse it
            QList<WikiEdit*> RevertBuffer;
            //! This is a list of all edits that are being processed by some way
            //! whole list needs to be checked and probed everytime once a while
            QList<WikiEdit*> ProcessingEdits;
            QList<WikiEdit*> UncheckedReverts;
            QList<ApiQuery*> PendingWatches;
        private:
#ifdef HUGGLE_METRICS
            void registerQueryPerfTime(Query *item);
            QList<qint64> performanceInfo;
#endif
            //! List of all running queries
            QList<Query*> runningQueries;
    };
}

#endif // QUERYPOOL_HPP
