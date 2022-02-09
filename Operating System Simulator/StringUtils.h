// Pre-compiler directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// header files
#include <stdbool.h>
#include <stdio.h> // file operations
#include <stdlib.h> // dynamic memory operations
#include "StandardConstants.h"

//extern const int MAX_STR_LEN;
//extern const int STD_STR_LEN;
//extern const int SUBSTRING_NOT_FOUND;
//extern const int STR_EQ;
//extern const char SPACE;
//extern const char COLON;
//extern const char SEMICOLON;
//extern const char PERIOD;
//extern const char COMMA;
//extern const char NULL_CHAR;
extern const char LEFT_PAREN;
extern const char RIGHT_PAREN;
//extern const Boolean IGNORE_LEADING_WS;
extern const Boolean ACCEPT_LEADING_WS;
//extern const Boolean NO_ERR;

// function prototypes
int compareString( const char *oneStr, const char *otherStr );
void concatenateString( char *destStr, const char *sourceStr );
void copyString( char *destStr, const char *sourceStr );
int findSubString( const char *testStr, const char *searchSubStr );
Boolean getStringConstrained( 
							FILE *inStream,
							bool clearLeadingNonPrintable,
							bool clearLeadingSpace,
							bool stopAtNonPrintable,
							char delimeter,
							char *capturedString
						);
int getStringLength( const char *testStr );
Boolean getStringToDelimeter( FILE *inStream, char delimeter, char *capturedString );
Boolean getStringToLineEnd(
							FILE *inStream,
							char *capturedString );
void getSubString (char *destStr, const char *sourceStr,
                                      int startIndex, int endIndex );
void setStrToLowerCase( char *destStr, const char *sourceStr );
char toLowerCase( char testChar );


#endif   // STRING_UTILS_H