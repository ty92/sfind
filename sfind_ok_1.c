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
	{"gid", 1, 0, 'g'},
	{"help", 0, 0, 'h'}
};

//void do_find_argu(char *name, char *option[], struct find_options *x, struct stat *info, int n);
//char *find_result(char *name, struct find_options *x, struct stat *info, char opt[], int i);
void do_find_argu();
char *find_result();
find_file(char *option[], struct find_options *x, int n)
{
	DIR *dir_ptr;
	struct dirent *direntp;
	struct stat info;

	if( (dir_ptr = opendir(".")) == NULL)
		perror("opendir");

	else
	{
		while((direntp = readdir(dir_ptr)) != NULL)
		{
			if(strncmp(direntp->d_name,"..",2)) //不对".."进行判断
			{
				if(stat(direntp->d_name, &info) == -1)
					perror("stat");
				do_find_argu(direntp->d_name, option, x, &info, n);
			}
		}
		closedir(dir_ptr);
	}
}

void do_find_argu(char *name, char *option[], struct find_options *x, struct stat *info, int n)
{
	char *filename;
	int i=0, m=0;

	if(n==1)        //一个参数时
        {
                for(i=0; i<5; i++)
                        if(*option[i] != 0)
                        {
                                filename = find_result(name, x, info, option[i],i);
                                if(filename != NULL)
                                {
                                        printf("./%s\n",filename);
                                }
                        }
        }
        else {          //指定多个参数时
		while(i<5)
                {
                        if(*option[i] != 0) //选项的参数不为空，即就是有指定该选项，进行筛选
                        {
                                filename = find_result(name, x, info, option[i], i);
                                if(filename != NULL)
                                        m++;
                        }
                        i++;
                }
                i=0;    //读取下一个文件，i置0，重新开始进行循环
                if(m==n)        //一个文件筛选出的个数满足参数个数n，即该文件匹配所有选项，输出
                        printf("./%s\n",filename);
                m=0;
        }
}



char *find_result(char *name, struct find_options *x, struct stat *info, char opt[], int i)
{
	long filesize;
	char a;

	if(x->find_name && opt[0] != 0)
	{
		if(strcmp(name,opt) == 0)
		{
			return name;
		}
	}
	if(x->find_size && opt[1] != 0)
	{
		filesize = atoi(opt);
		if(info->st_size <= filesize )
		{
			return name;
		}
	}

	if(x->find_type && opt[2] != 0)
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

	if(x->find_uid && opt[3] != 0)
	{
		if(info->st_uid == atoi(opt))
		{
			return name;
		}
	}

	if(x->find_gid && opt[4] !=0)
	{
		if(info->st_gid == atoi(opt))
		{
			return name;
		}
	}
	return 0;
}

void usage(int status)
{
	if(status != EXIT_SUCCESS)
		fprintf(stderr,"Try `sfind --help' for more information.\n");
	else
	{
		printf("Usage: sfind OPTION...ARGUMENT...\n");
		printf("-n|-name   根据文件名查找文件\n");
		printf("-t|-type   file type(d,c,b,f)\n");
		printf("-s|-size   查找不大于size的文件\n");
		printf("-u|-uid    根据uid查找\n");
		printf("-g|-gid    根据gid查找\n");
	}
	exit(status);
}

int main(int argc, char *argv[])
{
	struct find_options x;
	int opt, n=0;
	int option_index = 0;

	find_option_init(&x);
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
			case 'u':
				x.find_uid = true;
				x.argu[3] = optarg;
				break;
			case 'g':
				x.find_gid = true;
				x.argu[4] = optarg;
				break;
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			default:
				usage(EXIT_FAILURE);
		}
		n++;
	}
	if(argc == 1) {
		fprintf(stderr,"missing operand\n");
		usage(EXIT_FAILURE);
	}
	else {
		find_file(x.argu, &x, n);
	}
}
