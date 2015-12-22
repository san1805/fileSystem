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
