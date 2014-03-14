
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

struct argum {
	char** this_argums;
	int count;
};

void cpFilesToDir(struct argum arg) {
	int i;
	for (i = 2; i < arg.count - 1; ++i) {
		int fd_1 = open(arg.this_argums[i], O_RDONLY);
		if (fd_1 < 0) {
			perror("File error(1)");
			_exit(0);
		}		
		int fd_2 = open(arg.this_argums[arg.count - 1], O_WRONLY|O_TRUNC|O_CREAT, 0666);
		if (fd_1 < 0) {
			perror("File error(2)");
			close(fd_1);
			_exit(0);
		}
		int n;
		void* buf = malloc(500);
		if (buf == NULL) {
			perror("Memory error");
			close(fd_1);
			close(fd_2);
			_exit(0);
		}
		while(n = read(fd_1, buf, 100) > 0) {
			int param = write(fd_2, buf, n);
			if (param <= 0) {
				perror("Write error");
				close(fd_1);
				close(fd_2);
				free(buf);
				_exit(0);
			}
		}
		if (n < 0) {
			perror("Read error");
			close(fd_1);
			close(fd_2);
			free(buf);
			_exit(0);
		}
		close(fd_1);
		close(fd_2);
		free(buf);		
	}
}

void cpDirsToDir(struct argum arg) {
	int i;
	for (i = 3; i < arg.count - 1; ++i) {
		char pathName[PATH_MAX + 1];
		strncpy( pathName, arg.this_argums[arg.count - 1], PATH_MAX );
		strncat( pathName, "/", PATH_MAX );
       	strncat( pathName, arg.this_argums[i], PATH_MAX );
       	DIR *dir_to;		
       	if ( ( dir_to = opendir(pathName) ) == NULL) { //If dir does not exist
       		if ( ( mkdir(pathName, 0777) ) < 0) {
				perror("MK_DIR");
			}
			else {
				DIR *dir;
				dir = opendir(arg.this_argums[i]);
				struct dirent *dir_str;
				while ( (dir_str = readdir(dir)) != NULL) {
					if (dir_str->d_name[0] != '.') {
						if (dir_str->d_type & S_IFDIR) { // If dir
							char pathNameFrom[PATH_MAX];
							strncpy( pathNameFrom, arg.this_argums[i], PATH_MAX );
							strncat( pathNameFrom, "/", PATH_MAX );
		   					strncat( pathNameFrom, dir_str->d_name, PATH_MAX );	
							char* this_argum[5] = {"", "", "", pathNameFrom, pathName};
							struct argum temp = {.this_argums=this_argum, .count=5};
							cpDirsToDir(temp);
						}
						if (dir_str->d_type & S_IFREG) { // If file
							char pathNameFrom[PATH_MAX];
							strncpy( pathNameFrom, arg.this_argums[i], PATH_MAX );
							strncat( pathNameFrom, "/", PATH_MAX );
		   					strncat( pathNameFrom, dir_str->d_name, PATH_MAX );	
							int fd_1 = open(pathNameFrom, O_RDONLY);
							if (fd_1 < 0) {
								perror("File error(1)");
								_exit(0);
							}		
							int fd_2 = open(pathName, O_WRONLY|O_TRUNC|O_CREAT, 0666);
							if (fd_1 < 0) {
								perror("File error(2)");
								close(fd_1);
								_exit(0);
							}
							int n;
							void* buf = malloc(500);
							if (buf == NULL) {
								perror("Memory error");
								close(fd_1);
								close(fd_2);
								_exit(0);
							}
							while( ( n = read(fd_1, buf, 500) ) > 0) {
								int param = write(fd_2, buf, n);
								if (param <= 0) {
									perror("Write error");
									close(fd_1);
									close(fd_2);
									free(buf);
									_exit(0);
								}
							}
							if (n < 0) {
								perror("Read error");
								close(fd_1);
								close(fd_2);
								free(buf);
								_exit(0);
							}
							close(fd_1);
							close(fd_2);
							free(buf);		
						}
					}
				}
			}
   		}
       	else { //If dir already exists
			DIR *dir;
			dir = opendir(arg.this_argums[i]);
			struct dirent *dir_str;
			while ( (dir_str = readdir(dir)) != NULL) {
				if (dir_str->d_type & S_IFDIR) { // If dir
					char pathNameFrom[PATH_MAX];
					strncpy( pathNameFrom, arg.this_argums[i], PATH_MAX );
					strncat( pathNameFrom, "/", PATH_MAX );
					strncat( pathNameFrom, dir_str->d_name, PATH_MAX );	
					char* this_argum[5] = {"", "", "", pathNameFrom, pathName};
					struct argum temp = {.this_argums=this_argum, .count=5};
					cpDirsToDir(temp);
				}
				if (dir_str->d_type & S_IFREG) { // If file
					char pathNameFrom[PATH_MAX];
					strncpy( pathNameFrom, arg.this_argums[i], PATH_MAX );
					strncat( pathNameFrom, "/", PATH_MAX );
					strncat( pathNameFrom, dir_str->d_name, PATH_MAX );	
					char* this_argum[4] = {"", "", pathNameFrom, pathName};
					struct argum temp = {.this_argums=this_argum, .count=4};
					cpDirsToDir(temp);
				}
			}				       		
       	}
	}
}


int main(int argc, char** argv) {

	struct argum beg = {.this_argums=argv, .count=argc};
	
	if (argc <= 3) {
		char* msg = "Error: wrong number of arguments";
		write(2, msg, strlen(msg));
		_exit(0);
	}

	if (strcmp(argv[2], "-R") == 0) {
		cpDirsToDir(beg);	
	}
	else {
		cpFilesToDir(beg);
	}
	return(0);
}
