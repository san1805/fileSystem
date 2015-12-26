#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

//открытие файла
 
static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;
 
    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;
 
    return 0;
}

static struct fuse_operations operations =  // структура нами операций
{
    .getattr    = myfs_getattr,// атрибуты файла
    .readdir    = myfs_readdir,
    .open       = myfs_open,
    .read       = myfs_read,
    .write      = myfs_write,  //пишем данные в открытый файл.
    .mkdir      = myfs_mkdir,
    .mknod      = myfs_mknod,
    .rename     = myfs_rename,
    .rmdir      = myfs_rmdir, // удалить директорию
    .unlink     = myfs_unlink, // удалить файл
};
