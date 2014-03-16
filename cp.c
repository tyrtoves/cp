
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

void cpFilesToDir(char *path_from, char *path_to ) {
	ssize_t read_bytes;
	char buf[500];
	int fd1 = open( path_from, O_RDONLY );
	int fd2 = open( path_to, O_WRONLY | O_CREAT | O_TRUNC, 0777 );

	if ( fd1 < 0 || fd2 < 0 ) 
	{
		perror("Open error");
		return;
	}

	while (( read_bytes = read( fd1, &buf, 500 )) > 0 )
	{
		if ( write( fd2, &buf, read_bytes) < 0 )
		{
			perror("Writing error");
			return;
		}
	}
	
	close(fd1);
	close(fd2);
	
}

void cpDirsToDir(char *path_from, char *path_to ) {
	
	DIR *dir_from, *dir_to;
	struct dirent *cur_dir;
	struct stat inform;
	char newPathFrom[ PATH_MAX + 1];
	char newPathTo[ PATH_MAX + 1];
		       
	dir_from = opendir( path_from );

	if ( !dir_from ) {
	 	perror("PATH_FROM");
	  	return;	
	}

	mkdir( path_to, 0777 );
	
	while ( ( cur_dir = readdir( dir_from ) ) != NULL ) {	
	 	if ( cur_dir->d_name[0] == '.')
			continue;
		
		strncpy( newPathFrom, path_from, PATH_MAX );
		strncat( newPathFrom, "/", PATH_MAX );
		strncat( newPathFrom, cur_dir -> d_name, PATH_MAX );
	
	   	strncpy( newPathTo, path_to, PATH_MAX);
   		strncat( newPathTo, "/", PATH_MAX );
	    strncat( newPathTo, cur_dir -> d_name, PATH_MAX );

	    if ( stat( newPathFrom, &inform ) < 0 ){
	    	 perror("STAT");
		}

	    if (inform.st_mode & S_IFREG)
	    	cpFilesToDir( newPathFrom, newPathTo );

		if (inform.st_mode & S_IFDIR)
			cpDirsToDir( newPathFrom, newPathTo );		
	}
	closedir( dir_from );
}

int main(int argc, char** argv) {

	
	if (argc < 3) {
		char* msg = "Error: wrong number of arguments";
		write(2, msg, strlen(msg));
		_exit(0);
	}

	if (strcmp(argv[1], "-R") == 0) {
		int i = 0;
		for (i = 2; i < argc - 1; ++i) {
			cpDirsToDir( argv[i], argv[argc - 1] );
		}
	}
	else {
		int i = 0;
		for (i = 1; i < argc - 1; ++i) {
			cpFilesToDir( argv[i], argv[argc - 1] );
		}
	}
	return 0;
}
