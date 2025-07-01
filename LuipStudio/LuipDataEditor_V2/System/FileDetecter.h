/*
 * FileDetecter.h
 *
 *  Created on: 2016年12月23日
 *      Author: yongxi
 */

#ifndef SYSTEM_FILEDETECTER_H_
#define SYSTEM_FILEDETECTER_H_
#include <string.h>


#define DEV_DIR_NAME    "/dev"
#define USB_STORAGE_DEV_NAME    "sd"
/*
 *
 */
namespace System
{
class FileDetecter
{
public:
    FileDetecter();
    virtual ~FileDetecter();

    int findFileInDir(char *dir, char *fileName);
    bool UdiskDetect();
};
}
#endif /* SYSTEM_FILEDETECTER_H_ */
