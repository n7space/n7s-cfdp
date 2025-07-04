#ifndef MESSAGES_TO_USER_H
#define MESSAGES_TO_USER_H

#include "constants.h"

enum ListingResponseCode {
	LISTING_SUCCESSFUL = 0,
	LISTING_UNSUCCESSFUL = 1
};

struct directory_listing_request {
	char directory_name[MAX_LISTING_FILE_NAME_SIZE];
	char directory_file_name[MAX_LISTING_FILE_NAME_SIZE];
};

struct directory_listing_response {
    enum ListingResponseCode listing_response_code;
	char source_filename[MAX_LISTING_FILE_NAME_SIZE];
	char destination_filename[MAX_LISTING_FILE_NAME_SIZE];
};

union message_to_user {
    struct directory_listing_request directory_listing_request;
    struct directory_listing_response directory_listing_response;
};

#endif