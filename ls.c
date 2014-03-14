#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char** argv) {

	 if (argc > 3) {
		char* msg = "Error: wrong number of arguments";
		write(2, msg, strlen(msg));
		_exit(0);
	}

	DIR *dir;
	struct dirent *dir_str;

	dir = opendir(".");
	if (!dir) {
		perror("DIR");
		exit(1);
	}

	while ( (dir_str = readdir(dir)) != NULL) {

		if (argc == 2) {
			if(dir_str->d_name[0] != '.') 
				printf("%s \n", dir_str->d_name);
		}
		
		if (argc == 3 && (strcmp(argv[2], "-L") == 0)) {
			struct stat dirstat;
			struct stat *dir_stat = &dirstat;
			if(dir_str->d_name[0] != '.') {
				if ( stat(dir_str->d_name, dir_stat) >= 0 )
					printf("%s %d %d \n", dir_str->d_name, dir_stat->st_size, dir_stat->st_ctime);
				else
					perror("STAT");
			}
		}
	}
	closedir(dir);	
	return(0);
}
