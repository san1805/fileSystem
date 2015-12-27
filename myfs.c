#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

//ссылка на узел
typedef struct Node* Link;
//тип узел
typedef struct Node
{
    //имя
    char* name;
    //содержание
    char* content;
    //ссылка на ребенка
    Link child;
	Link parent;
} Node;

//первый узел всей ФС
static Link root;

static int myfs_getattr(const char *path, struct stat *stbuf)
{
    //копируем 0 в первые sizeof(struct stat) буфера
    //заполняем нулями буфер
    memset(stbuf, 0, sizeof(struct stat));
	Link node=seekNode(root, path);
	if (node == 0) return -ENOENT;
    //если в переменной path-"/"
    if (node->content == 0) {
        //если установлен бит S_IFDIR, то в структуру пишем, что это директория
        stbuf->st_mode = S_IFDIR | 0666;
        //количество жестких ссылок
        stbuf->st_nlink = 2;
    }
    //иначе, если в path путь к файлу hello
    else 
	{
        //если установлен бит S_IFREG, то в структуру пишем, что это файл
        stbuf->st_mode = S_IFREG | 0666;
        //количество жестких ссылок
        stbuf->st_nlink = 1;
        //общий размер в байтах
        stbuf->st_size = strlen(node->content);
    }
    return 0;
}

//создаем узел
static int myfs_mknod(const char* path, mode_t mode, dev_t dev)
{
	char** array = split(path);
	int i=0;
	while(array[i] != 0) i++;
	Link node=createNode(array[i-1], "");
	Link par=seekNode(root, path);
	addNode(par, node);
	
	return 0; 
}

//создаём папку
static int myfs_mkdir(const char* path, mode_t mode)
{   
    char** array = split(path);
    int i = 0; 
    while(array[ct] != 0) i++;
    Link node = createNode(array[i-1], 0);
    Link par = seekNode(root, path);
    addNode(par, node);
	
    return 0;       
}

//открытие файла
static int myfs_open(const char *path, struct fuse_file_info *fi)
{
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;
 
    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;
 
    return 0;
}

//чтение из файла
static int myfs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
	Link node=seekNode(root, path);
	size_t len;
	len=srtlen(node->content);
	if (offset<len)
	{
		if (offset + size > len)
            size = len - offset;
		memcpy(buf, node->content + offset, size);
	} else  size = 0;
	return size;
}

//запись в файл
static int my_write(const char *path, const char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
	Link node=seekNode(root, path);
	size_t len;
	len=srtlen(buf);
	if (offset<len)
	{
		//надо изменить размер content
		if (offset + size > len)
            size = len - offset;
		memcpy(node->content, buf, size);
	} else  size = 0;
	return size;
}

//удаление файла(узла)
static int myfs_unlink(char* path) 
{
    Link node = seekNode(tree, path);
    deleteNode(node);
    return 0;
}

static int flush(const char* path, struct fuse_file_info* fi) 
{
	return 0;
}


static struct fuse_operations myfs_oper;

int main(int argc, char *argv[]) // структура операций
{
    myfs_oper.getattr    = myfs_getattr,// атрибуты файла
    myfs_oper.readdir    = myfs_readdir,
    myfs_oper.open       = myfs_open,
    myfs_oper.read       = myfs_read,
    myfs_oper.write      = myfs_write,  //пишем данные в открытый файл.
    myfs_oper.mkdir      = myfs_mkdir,
    myfs_oper.mknod      = myfs_mknod,
    myfs_oper.rename     = myfs_rename,
    myfs_oper.rmdir      = myfs_rmdir, // удалить директорию
    myfs_oper.unlink     = myfs_unlink, // удалить файл

    return fuse_main(argc, argv, &myfs_oper, 0);
}

//разбиваем путь на массив
char** split(char* path)
{  
    char** array;
    //=1 когда корневая директория
    if (strlen(path)>1)
    {
        //количество файлов/папок внутри пути (фактически количество элементов массива)
        int size=0;
      
        while(path[i] != 0)
        {
            if (path[i] == '/') size++; 
        }
        //выделяем память (+2, тк добавили первый символ "/" и последний "0")
        array = (char**)malloc(sizeof(char*)*(count + 2));
        array[count + 1] = 0;
        array[0] = "/";
        //индекс элемента массива указателей
        int n=1;
        //индекс символа пути
        int i=1;

        while (path[i] != 0)
        {
            int с=1; //число символов имени узла
			int j=i;
			
			//считаем число символов имени узла
			while((path[j] != '/') && (path[j] != 0))
			{
				c++;
				j++;
				i++;
			}
			if (path[i] != 0) i++; //вышли из предыдущего цикла
			//выделяем память яцейки=размеру с
			array[n]=(char*)malloc(sizeof(char)*c);
			//в конец имени узла добавляем отделяющий null символ
			array[n][c - 1] = 0;
			n++;
        }
		
		n=1;
		i=1;
		while (path[i]!=0)
		{
			int j = i;
			int tmp = i; 
			//заполняем массив именами узлов
			while((path[j] != '/') && (path[j] != 0))
			{
				array[n][j - tmp] = path[j];
				j++;
				i++;
			}
			if (path[i] != 0) i++;
			n++; 
		}
    }
	else
	{
		array = (char**)malloc(sizeof(char*)*2);
		array[0] = "/";
		array[1] = 0;
	}
	return array;
}

//ищем последний узел по заданному пути
Link seekNode (Link root, char* path)
{
	char** splitted = split(path);
	int count=0;//число узлов
	while(splited[i++] != 0) count++;
	
	//если корнень
	if (count == 1) return root;
	
	Link node=root;
	Link nodeseek=root;
	int i=0;
	int j=0;
	
	while(splitted[i]!=0)
	{
		if (nodeseek!=0)
		{
			if (strcmp(nodeseek->name, splited[i]) == 0)
		    {
			    node=nodeseek;
				j++;
		    }
			if (nodeseek->child!=0)
		        nodeseek=nodeseek->child;
		}
		i++;
	}
	if (j==0)
		return 0;
	if (strcmp(node->name,splited[count-1]) != 0) 
		return 0;
	return node;
}

//создаем узел
Link createNode(char* name, char* content) 
{
	Link node = (Link)malloc(sizeof(Node));
	node->name = name;
	node->content = content;
	node->child=0;
	node->parent=0;
	return node;
}

//добавляем узел
void addNode(Link par, Link node)
{
	par->child=node;
	node->parent=par;
}

//удаляем узел
void deleteNode(Link node)
{
	Link p=node->parent;
	Link ch=node->child;
	p->child=ch;
	ch->parent=p;
	node->parent=0;
	node->child=0;
}

