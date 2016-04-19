/**
 * sfind_test_1.c: 可支持选项:name、type、size、uid、gid;
 * 长短选项均支持即就是“-name”"-n"、"--name"；
 * 问题：使用-d -n时，不能过滤掉当前目录'.'
 **/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  //支持bool类型
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


/* struct for find option */
struct find_options{    
	bool find_name;
	bool find_size;
	bool find_type;
	bool find_gid;
	bool find_uid;
	char *argu[5];
};

/* find option init */
static void find_option_init(struct find_options *x)
{
	int i;
	x->find_name = false;
	x->find_size = false;
	x->find_type = false;
	x->find_gid = false;
	x->find_uid = false;

	for(i=0; i<5; i++)
		x->argu[i] = (char*)malloc(sizeof(char*));
}

static struct option long_options[] = {
	{"name", 1, 0, 'n'},
	{"type", 1, 0, 't'},
	{"size", 1, 0, 's'},
	{"uid", 1, 0, 'u'},
	{"gid", 1, 0, 'g'}
};

char *find_result(char *name, struct find_options *x, struct stat *info, char *opt);
find_file(char *option[], struct find_options *x, int n)
{
	DIR *dir_ptr;
	struct dirent *direntp;
	struct stat info;
	int i=0, m=0;
	char *filename;

	if( (dir_ptr = opendir(".")) == NULL)
		perror("opendir");

	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)
		{
			if(strncmp(direntp->d_name,"..",2))
			{
				if(stat(direntp->d_name, &info) == -1)
				perror("stat");
			
			if(n==1)	//一个参数时
			{
				for(i=0; i<5; i++)
					if(*option[i] != 0)
					{
						filename = find_result(direntp->d_name, x, &info, option[i]);		
						if(filename != NULL)
						{	
							printf("./%s\n",filename);
						}
					}
			}
			else {		//指定多个参数时
					while(i<5)
					{
						if(*option[i] != 0) //选项的参数不为空，即就是有指定该选项，进行筛选
						{
							filename = find_result(direntp->d_name, x, &info, option[i]);		
							if(filename != NULL)
								m++;
						}
						i++;
					}
					i=0;	//读取下一个文件，i置0，重新开始进行循环
					if(m==n)	//一个文件筛选出的个数满足参数个数n，即该文件匹配所有选项，输出
						printf("./%s\n",filename);
					m=0;
			}
			}
		}
		closedir(dir_ptr);
	}
}

char *find_result(char *name, struct find_options *x, struct stat *info, char *opt)
{
	long filesize;
	char a;

	if(x->find_name)
	{
		if(strcmp(name,opt) == 0)
		{
			return name;
		}
	}
	if(x->find_size)
	{
		filesize = atoi(opt);
		if(info->st_size <= filesize )
		{
			return name;
		}
	}

	if(x->find_type)
	{
		if(S_ISDIR(info->st_mode)) a = 'd';
		if(S_ISCHR(info->st_mode)) a = 'c';
		if(S_ISBLK(info->st_mode)) a = 'b';
		if(S_ISREG(info->st_mode)) a = 'f';

		if( strncmp(&a,opt,1) == 0 )
		{
			return name;
		}
	}

	if(x->find_uid)
	{
		if(info->st_uid == atoi(opt))
		{
			return name;
		}
	}

	if(x->find_uid )
	{
		if(info->st_gid == atoi(opt))
		{
			return name;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	struct find_options x;
	int opt, n=0;
	int option_index = 0;

	find_option_init(&x);
/*	while((opt = getopt(argc, argv, "n:s:t:g:u:")) != -1) {
		switch (opt) {
			case 'n':
				x.find_name = true;
				x.argu[0] = optarg;
				break;
			case 's':
				x.find_size = true;
				x.argu[1] = optarg;
				break;
			case 't':
				x.find_type = true;
				x.argu[2] = optarg;
				break;
			case 'g':
				x.find_gid = true;
				x.argu[3] = optarg;
				break;
			case 'u':
				x.find_uid = true;
				x.argu[4] = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s \n",argv[0]);
				exit(EXIT_FAILURE);
		}
		n++;
	}
*/
	while((opt = getopt_long_only(argc, argv, "n:t:s:u:g:", long_options, &option_index)) != -1) {
		switch (opt) {
			case 'n':
				x.find_name = true;
				x.argu[0] = optarg;
				break;
			case 's':
				x.find_size = true;
				x.argu[1] = optarg;
				break;
			case 't':
				x.find_type = true;
				x.argu[2] = optarg;
				break;
			case 'g':
				x.find_gid = true;
				x.argu[3] = optarg;
				break;
			case 'u':
				x.find_uid = true;
				x.argu[4] = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s \n",argv[0]);
				exit(EXIT_FAILURE);
		}
		n++;
	}
	find_file(x.argu, &x, n);
}
