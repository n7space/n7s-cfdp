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
	char directory_name[MAX_LISTING_FILE_NAME_SIZE];
	char directory_file_name[MAX_LISTING_FILE_NAME_SIZE];
};

enum MessageToUserType {
	DIRECTORY_LISTING_REQUEST = 0,
	DIRECTORY_LISTING_RESPONSE = 1
};

union message_to_user_union {
    struct directory_listing_request directory_listing_request;
    struct directory_listing_response directory_listing_response;
};

struct message_to_user {
    enum MessageToUserType message_to_user_type;
	union message_to_user_union message_to_user_union;
};

#endif