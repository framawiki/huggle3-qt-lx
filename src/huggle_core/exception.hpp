//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "definitions.hpp"
#include <QString>

//////////////////////////////////////////////////////////////////////////
// Breakpad init
//////////////////////////////////////////////////////////////////////////
#ifndef DISABLE_BREAKPAD
    #ifdef __linux__
        //linux code goes here
    #define HUGGLE_BREAKPAD 0
    #include "client/linux/handler/exception_handler.h"
    #elif _WIN32
        // windows code goes here
    #define HUGGLE_BREAKPAD 1
    // This fixes the qdatetime bug which produces error with compiler on windows
    #define NOMINMAX
    // Ensure that NOMINMAX is there before including this file
    #include "client/windows/handler/exception_handler.h"
    #endif
#endif
//////////////////////////////////////////////////////////////////////////
// remaining code must be surrounded with directives
//////////////////////////////////////////////////////////////////////////

#define HUGGLE_STACK 80

namespace Huggle
{
    //! Every exception raised by huggle is defined by this class
    class HUGGLE_EX_CORE Exception
    {
        public:
            /*!
             * \brief ThrowSoftException Soft exceptions that crashes the application only in debugging mode
             *This can be used in case you want to throw exception only when debugging the application because
             *the exception itself is not critical enough to crash whole application to regular users
             * \param Text
             * \param Source
             */
            static void ThrowSoftException(QString Text, QString Source);
            static QString GetCurrentStackTrace();
            static void InitBreakpad();
            static void ExitBreakpad();

            //! Error code
            int ErrorCode;
            //! Source
            QString Source;
            //! Reason for crash
            QString Message;
            QString StackTrace;
            //! ctor
            Exception(QString text, bool is_recoverable = true);
            Exception(QString text, QString source, bool is_recoverable = true);
            Exception(QString text, const char *source);
            virtual ~Exception();
            bool IsRecoverable() const;
        private:
            void construct(QString text, QString source, bool is_recoverable);
#ifdef HUGGLE_BREAKPAD
#if HUGGLE_BREAKPAD == 0
            static google_breakpad::MinidumpDescriptor *GoogleBP_descriptor;
#endif
            static google_breakpad::ExceptionHandler   *GoogleBP_handler;
#endif
            bool isRecoverable;
    };

    class HUGGLE_EX_CORE NullPointerException : public Exception
    {
        public:
            NullPointerException(QString name, QString source);
    };
}

#endif // EXCEPTION_H
