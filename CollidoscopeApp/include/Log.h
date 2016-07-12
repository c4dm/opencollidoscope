#pragma once 


/**
 * Utility function to log errors using the cinder::log library.
 * Errors are logged to collidoscope_error.log file. 
 *
 */ 
void logError( const std::string &errorMsg );


/**
 * Utility function to log info using the cinder::log library.
 * Errors are logged to the terminal. Used only for debugging.
 *
 */ 
void logInfo( const std::string &infoMsg );
