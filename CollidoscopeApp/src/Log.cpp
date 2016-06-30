
#include "cinder/Log.h"

bool fileLoggerCreated = false;

void logError( const std::string &errorMsg )
{
    using namespace ci::log;

    if ( !fileLoggerCreated ){
        makeLogger<ci::log::LoggerFile>( "./collidoscope_error.log" );
        fileLoggerCreated = true;
    }

    LogManager *log = LogManager::instance();

    Metadata logMeta;
    logMeta.mLevel = LEVEL_ERROR;

    log->write( logMeta, errorMsg );

}

void logInfo( const std::string &infoMsg )
{
#ifdef _DEBUG
    using namespace ci::log;

    LogManager *log = LogManager::instance();

    Metadata logMeta;
    logMeta.mLevel = LEVEL_INFO;

    log->write( logMeta, infoMsg );
#endif
}
