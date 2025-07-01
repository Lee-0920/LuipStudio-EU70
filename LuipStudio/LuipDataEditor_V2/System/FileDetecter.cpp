/*
 * FileDetecter.cpp
 *
 *  Created on: 2016年12月23日
 *      Author: yongxi
 */

#include "FileDetecter.h"
#include "Setting/Environment.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <QString>

#ifdef    _CS_ARM_LINUX
#include <sys/stat.h>
#endif

using namespace Configuration;

namespace System
{
FileDetecter::FileDetecter()
{
    // TODO Auto-generated constructor stub

}

FileDetecter::~FileDetecter()
{
    // TODO Auto-generated destructor stub
}

int FileDetecter::findFileInDir(char *dir, char *fileName)
{
    int iFindResult = -1;

#ifdef    _CS_ARM_LINUX

    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return iFindResult;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode))
            continue;
        if(strstr(entry->d_name, fileName))
        {
            iFindResult = 0;
            break;
        }
    }
    chdir("..");
    closedir(dp);

    chdir(Environment::Instance()->GetAppPath().c_str());
#else
    (void)dir;
    (void)fileName;
#endif

    return iFindResult;

}

bool FileDetecter::UdiskDetect()
{
    if(findFileInDir((char *)DEV_DIR_NAME, (char *)USB_STORAGE_DEV_NAME) >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
}
