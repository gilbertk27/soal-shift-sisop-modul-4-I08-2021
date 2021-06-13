//fusermount -u [direktori tujuan]
//gcc -Wall `pkg-config fuse --cflags` [file.c] -o [output] `pkg-config fuse --libs`
 
#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>

char directorypath[50] = "/home/xyncz/Downloads";

void writeW(char *text, char* path)
{
    char* info = "WARNING";
    char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/xyncz/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
}

void writeI(char *text, char* path)
{
    char* info = "INFO";
	char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/xyncz/SinSeiFS.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
    
}


char ext[100000] = "\0";
int id = 0;

char *encrypt(char* str, bool cek)
{
	int i, j, k = 0;
	char *ext = strrchr(str, '.');
	if(cek && ext != NULL) k = strlen(ext);
	for(i=0; i<strlen(str)-k; i++) {
		if(str[i] >= 'A' && str[i] <= 'Z') str[i] = 'Z' + 'A' - str[i];
		if(str[i] >= 'a' && str[i] <= 'z') str[i] = 'z' + 'a' - str[i];
	}
	return str;
}


void substring(char *s, char *sub, int p, int l) {
   int c = 0;
   while (c < l) 
   {
      sub[c] = s[p + c];
      c++;
   }
   sub[c] = '\0';
}

char *lastPart(char *str)
{
	if(!strcmp(str, "/")) return NULL;
	return strrchr(str, '/') + 1;
}

int check_ext(char* file)
{
	id = 0;
	while(id < strlen(file) && file[id] != '.') id++;
	memset(ext, 0, sizeof(ext));
	strcpy(ext, file + id);
	return id;
}

char *checkPath(char *str)
{
	bool encr;
	int start, id;
	encr = 0; start = 1; 
	id = strchr(str + start, '/') - str - 1;
	char curpos[1024];
	while(id < strlen(str))
	{
		strcpy(curpos, "");
		strncpy(curpos, str + start, id - start + 1);
		curpos[id - start + 1] = '\0';
		if(encr)
		{
			encrypt(curpos, 0);
			strncpy(str + start, curpos, id - start + 1);
		}
		if(!encr && strstr(str + start, "AtoZ_") == str + start) encr = 1;
		start = id + 2;
		id = strchr(str + start, '/') - str - 1;
	}
	id = strlen(str); id--;
	strncpy(curpos, str + start, id - start + 1);
	curpos[id - start + 1] = '\0';
	if(encr)
	{
		encrypt(curpos, 1);
		strncpy(str + start, curpos, id - start + 1);
	}
	return str;
}

char *mixPath(char *fin, char *str1, const char *str2)
{
	strcpy(fin, str1);
	if(!strcmp(str2, "/")) return fin;
	if(str2[0] != '/')
	{
		fin[strlen(fin) + 1] = '\0';
		fin[strlen(fin)] = '/';
	}
	sprintf(fin, "%s%s", fin, str2);
	return fin;
}

int encrFolder(char *str)
{
	int ans;
	char *fi = strtok(str, "/");
	ans = 0;
	while(fi)
	{
		char sub[1024];
		substring(fi, sub, 0, 6);
		if(!strcmp(sub, "AtoZ_")) ans |= 1;
		fi = strtok(NULL, "/");
	}
	return ans;
}

int encrFull(char *str)
{
	int ans;
	char *fi = strtok(str, "/");
	char *sc = strtok(NULL, "/");
	ans = 0;
	while(sc)
	{
		char sub[1024];
		substring(fi, sub, 0, 6);
		if(!strcmp(sub, "AtoZ_")) ans |= 1;
		fi = sc;
		sc = strtok(NULL, "/");
	}
	return ans;
}

void loopAllEnc1(char *str, int flag)
{
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir) return;
	
	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char path[2000000], name[1000000], newname[1000000];
        	mixPath(path, str, dp->d_name);
			strcpy(name, dp->d_name);
			if(flag == 1) mixPath(newname, str, encrypt(name, 1));
			//else if(flag == -1) mixPath(newname, str, decrypt(name, 1));
			if(dp->d_type == DT_REG) rename(path, newname);
			else if(dp->d_type == DT_DIR)
			{
				rename(path, newname);
				loopAllEnc1(newname, flag);
			}
        }
	}
}





void encrypt1(char *str, int flag)
{
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	loopAllEnc1(str, flag);
}






static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	res = lstat(checkPath(fpath), stbuf);
	writeI("ls", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	res = access(checkPath(fpath), mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	res = readlink(checkPath(fpath), buf, size - 1);
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res = 0;
	
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(checkPath(fpath));
	if (dp == NULL) return -errno;
	
	int flag = encrFolder(fpath);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;		
		st.st_mode = de->d_type << 12;
		char names[1000000];
		strcpy(names, de->d_name);
		if(flag == 1)
		{
			//if(de->d_type == DT_REG) decrypt(names, 1);
			//else 
			//if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) decrypt(names, 0);
			res = (filler(buf, names, &st, 0));
			if(res!=0) break;
		}
		else
		{
			res = (filler(buf, names, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
    	writeI("CD", fpath);
	return 0;
}


static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	
	int res;

	res = mkdir(checkPath(fpath), mode);
	if (res == -1) return -errno;
	
   	char cek_substr[1024];
    	if(lastPart(fpath) == 0) return 0;
    	char filesPath[1000000];
    	strcpy(filesPath, lastPart(fpath));
    	substring(filesPath, cek_substr, 0, 6);
	if(strcmp(cek_substr, "AtoZ_") == 0) //folder encrypt1
	{
		encrypt1(fpath, 1);	
	}
	writeW("mkdir", fpath);
	return 0;
}




static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	checkPath(fpath);
	int res;
	
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	} else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	if (res == -1) return -errno;
	
    	writeI("create", fpath);
	return 0;
}




static int xmp_rmdir(const char *path)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = rmdir(checkPath(fpath));
    writeW("rmdir", fpath);
	if (res == -1) return -errno;
	return 0;
}




static int xmp_unlink(const char *path)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = unlink(checkPath(fpath));
    	writeW("remove", fpath);
	if (res == -1) return -errno;
	return 0;
}



static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{    
    char ffrom[1000];
	mixPath(ffrom, directorypath, from);
	
    char fto[1000];
	mixPath(fto, directorypath, to);
	
	int res;

	res = rename(checkPath(ffrom), checkPath(fto));
	
	if (res == -1)
		return -errno;
	
	int fromm = 0, too = 0;
	char cek_substr[1024], cek2[1024];
   	if(lastPart(ffrom) == 0) return 0;
    	char filesPath[1000000];
    	strcpy(filesPath, lastPart(ffrom));
    	substring(filesPath, cek_substr, 0, 6);
	if(strcmp(cek_substr, "AtoZ_") == 0) //folder encrypt1
	{
		fromm = 1;
	}
	
    	if(lastPart(fto) == 0) return 0;
    	strcpy(filesPath, lastPart(fto));
    	substring(filesPath, cek_substr, 0, 6);
	if(strcmp(cek2, "AtoZ_") == 0) //folder decrypt1
	{
		too = 1;
	}
	
	if(fromm == 0 && too == 1) encrypt1(fto, 1);
	else if(fromm == 1 && too != 1) encrypt1(fto, -1);
	writeI("move", ffrom);

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = chmod(checkPath(fpath), mode);
	writeW("chmod", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = lchown(checkPath(fpath), uid, gid);
	writeW("chown", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = truncate(checkPath(fpath), size);
    	writeW("truncate", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(checkPath(fpath), tv);
    	writeI("utimens", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = open(checkPath(fpath), fi->flags);
    	writeI("open", fpath);
	if (res == -1) return -errno;
	close(res);
	return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int fd = 0;
	int res = 0;

	(void) fi;
	fd = open(checkPath(fpath), O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;
	close(fd);
	
    	writeI("cat", fpath);
	return res;
}



static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int fd;
	int res;

	(void) fi;
	fd = open(checkPath(fpath), O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

    	writeW("write", fpath);
	close(fd);
	return res;
}



static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
	int res;

	res = statvfs(checkPath(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}



static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) 
{
	char fpath[1000];
	mixPath(fpath, directorypath, path);
    	(void) fi;

    	int res;
    	res = creat(checkPath(fpath), mode);
    	if(res == -1) return -errno;
	
    	writeI("create", fpath);
    	close(res);
    	return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
	return 0;
}


static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
