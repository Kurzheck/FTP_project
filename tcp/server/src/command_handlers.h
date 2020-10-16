#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "util.h"
#include "global.h"
#include "data_structure.h"

int USER_Handler(struct ThreadParam* data) {
    // anonymous
    if (strcmp(data->request.arg, "anonymous") == 0) {
        printf("USER anonymous received, connfd = %d\n", data->connfd);
        WriteResponse(data->connfd, );
        // TODO
    }
    else {
        
    }
    
};

int PASS_Handler(struct ThreadParam* data) {

};
