/**
 * 该sfind可以使用-n、-s、-t等参数，每次只能使用一个参数；
 * 不能任意组合
 **/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


struct find_options{
	bool find_name;
	bool find_size;
	bool find_type;
	bool find_gid;
	bool find_uid;
	char *argu;
};

static void find_option_init(struct find_options *x)
{
	x->find_name = false;
	x->find_size = false;
	x->find_type = false;
	x->find_gid = false;
	x->find_uid = false;
	x->argu = NULL;
}

find_file(char *option, struct find_options *x)
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
			if(stat(direntp->d_name, &info) == -1)
				perror("stat");

			if(x->find_name)
			{
				if(strcmp(direntp->d_name,option) == 0)
					printf("./%s\n",direntp->d_name);
			}
			if(x->find_size)
				get_stat_size(direntp->d_name, &info, option);
			if(x->find_type)
				get_stat_type(direntp->d_name, &info, option);
			if(x->find_uid)
			{
				if(info.st_uid == atoi(option))
					printf("./%s\n",direntp->d_name);
			}
			if(x->find_gid)
			{
				if(info.st_gid == atoi(option))
					printf("./%s\n",direntp->d_name);
			}
		}
		closedir(dir_ptr);
	}
}

get_stat_size(char *name, struct stat *info, char *opt)
{
	long filesize;
	filesize = atoi(opt);
		
	if(info->st_size <= filesize )
		printf("./%s\n",name);
}

get_stat_type(char *name, struct stat *info, char *opt)
{
        char a;

        if(S_ISDIR(info->st_mode)) a = 'd';
        if(S_ISCHR(info->st_mode)) a = 'c';
        if(S_ISBLK(info->st_mode)) a = 'b';
        if(S_ISREG(info->st_mode)) a = 'f';

        if( strncmp(&a,opt,1) == 0 )
              printf("./%s\n",name);
}

int main(int argc, char *argv[])
{
	struct find_options x;
	int opt, n=0;
	int option_index = 0;

	find_option_init(&x);
	static struct option long_options[] = {
		{"name", 1, 0, 'n'},
		{"type", 1, 0, 't'},
		{"size", 1, 0, 's'},
		{"uid", 1, 0, 'u'},
		{"gid", 1, 0, 'g'}
	};

	while((opt = getopt_long_only(argc, argv, "n:s:t:g:u:",long_options,&option_index)) != -1) {
		switch (opt) {
			case 'n':
				x.find_name = true;
				x.argu = optarg;
				break;
			case 's':
				x.find_size = true;
				x.argu = optarg;
				break;
			case 't':
				x.find_type = true;
				x.argu = optarg;
				break;
			case 'g':
				x.find_gid = true;
				x.argu = optarg;
				break;
			case 'u':
				x.find_uid = true;
				x.argu = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s \n",argv[0]);
				exit(EXIT_FAILURE);
		}
		n++;
		find_file(x.argu, &x);
	}
//	printf("n=%d\n",n);
}
