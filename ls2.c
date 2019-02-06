/*
 *ls2 would list all stats of all files under one directory
 * 
 * 
 * */

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define MODE_STR_SIZE 1 + 3 + 3 + 3 + 1
void do_ls(char *);
void do_stat(char *fname);
void show_stat_info(char *, struct stat*);
void mode_to_string(struct stat*, char *, int);

int main(int argc, char *argv[]) {
	if (argc == 1){
		do_ls(".");
	} else	{
		while (--argc) {
			do_ls(*(++argv));
		}
	}

	return 0;
}

void do_ls(char *dir_name) {
	DIR *dir_stream;
	struct dirent *dir_entry;

	if (!dir_name) {
		fprintf(stderr, "directory name is NULL");
		return;
	}

	dir_stream = opendir(dir_name);
	if (!dir_stream) {
		perror("do_ls can't open directory\n");
		return;
	}

	while (dir_entry = readdir(dir_stream)) {
		do_stat(dir_entry->d_name);
	}

	if (closedir(dir_stream) == -1) {
		perror("do_ls can't close directory\n");
	}
}

void do_stat(char *fname) {
	struct stat stat_info;
	struct passwd *pw = NULL;
	struct group *grp = NULL;
	struct tm *now;
	char modestr[MODE_STR_SIZE];
	char time_buf[64];

	if (!fname) {
		fprintf(stderr, "do_stat input fname is NULL\n");
		return;
	}

	if (stat(fname, &stat_info) == -1) {
		perror(fname);
		return;
	}

	mode_to_string(&stat_info, modestr, sizeof(modestr));
	printf("%s", modestr);
	printf("%4d", (int)(stat_info.st_nlink));

	pw = getpwuid(stat_info.st_uid);
	if (pw)
		printf("%10s", pw->pw_name);
	else
		printf("%10d", stat_info.st_uid);

	grp = getgrgid(stat_info.st_gid);
	if (grp)
		printf("%10s", grp->gr_name);
	else
		printf("%10d", stat_info.st_gid);

	printf("%8ld", stat_info.st_size);
/*
	now = localtime(&(stat_info.st_mtim.tv_sec));
	strftime(time_buf, sizeof(time_buf), "%m %d %H:%M:%S", now);
	printf("%s ", time_buf);
*/

	printf(" %.12s", 4+ctime(&(stat_info.st_mtim.tv_sec)));
	printf(" %s\n", fname);

}

void mode_to_string(struct stat *stat_ptr, char *modestr, int modestr_size) {
	if (!stat_ptr || !modestr || modestr_size < MODE_STR_SIZE) {
		fprintf(stderr, "mode_to_string input is NULL\n");
		return;
	}

	int mode = (int)stat_ptr->st_mode;
	strcpy(modestr, "----------");
	if (S_ISDIR(mode)) {
		modestr[0] = 'd';
	} else if (S_ISCHR(mode)) {
		modestr[0] = 'c';
	} else if (S_ISBLK(mode)) {
		modestr[0] = 'b';
	}

	if (mode & S_IRUSR)
		modestr[1] = 'r';
	if (mode & S_IWUSR)
		modestr[2] = 'w';
	if (mode & S_IXUSR)
		modestr[3] = 'x';

	if (mode & S_IRGRP)
		modestr[4] = 'r';
	if (mode & S_IWGRP)
		modestr[5] = 'w';
	if (mode & S_IWGRP)
		modestr[6] = 'x';

	if (mode & S_IROTH)
		modestr[7] = 'r';
	if (mode & S_IWOTH)
		modestr[8] = 'w';
	if (mode & S_IXOTH)
		modestr[9] = 'x';
}
