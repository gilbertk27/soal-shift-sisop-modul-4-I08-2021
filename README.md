# soal-shift-sisop-modul-4-I08-2021


Before doing the problems, we first create FUSE <i>filesystem</i> by impelenting <i>system call</i> needed for the problem to be done. Here's some of the information:
```C
static const struct fuse_operations _oper = {
    .getattr	= xmp_getattr,
    .access	= xmp_access,
    .readlink	= xmp_readlink,
    .readdir	= xmp_readdir,
    .mkdir	= xmp_mkdir,
    .symlink	= xmp_symlink,
    .unlink	= xmp_unlink,
    .rmdir	= xmp_rmdir,
    .rename	= xmp_rename,
    .link	= xmp_link,
    .chmod	= xmp_chmod,
    .chown	= xmp_chown,
    .truncate	= xmp_truncate,
    .utimens	= xmp_utimens,
    .open	= xmp_open,
    .create 	= xmp_create,
    .read	= xmp_read,
    .write	= xmp_write,
    .statfs	= xmp_statfs,
};
```

All these <i>system call</i> is only to call the <i>system call</i> Linux by changing the <b><i>path</i></b> inputted <i>system call</i> will become <b><i>path</i></b> within the <i>mount</i>-<i>point</i>, which is `dirpath` variable.
  - `xmp_getattr()`: to get <b><i>stat</i></b> from <b><i>path</i></b> inputted.
  - `xmp_access()`: to access <b><i>path</i></b> inputted.
  - `xmp_readlink()`: to read <i>symbolic link</i> from <b><i>path</i></b>.
  - `xmp_mkdir()`: to create directory from <b><i>path</i></b>.
  - `xmp_symlink`: to create <i>symbolic link</i> from <b><i>path</i></b>.
  - `xmp_unlink()`: to erase a <i>file</i> from <b><i>path</i></b>.
  - `xmp_rmdir()`: to erase directory from <b><i>path</i></b>.
  - `xmp_rename()`: to me-<i>rename</i> from <b><i>path</i></b> awal menjadi <b><i>path</i></b> tujuan.
  - `xmp_link()`: to create <i>hard</i>-<i>link</i> from <b><i>path</i></b>.
  - `xmp_chmod()`: to change mode from <b><i>path</i></b>.
  - `xmp_chown()`: to change ownershop (user and <i>group</i>) from <b><i>path</i></b>.
  - `xmp_truncate()`: to do <i>truncate</i> (clean or reduce <i>size</i>) from <b><i>path</i></b>.
  - `xmp_utimens()`: to change status <i>time</i> from <b><i>path</i></b>.
  - `xmp_open()`: to open <b><i>path</i></b>.
  - `xmp_create()`: to create <b><i>path</i></b> from the mode inputted.
  - `xmp_read()`: to read content from <b><i>path</i></b>.
  - `xmp_write()`: to write (<i>writing</i>) into <b><i>path</i></b>.
  - `xmp_statfs()`: to do <b><i>stat</i></b> to <i>file</i> inputted.

```C
char directorypath[50] = "/home/xyncz/Downloads";
```

Later, when <i>filesystem</i> being <i>mounted</i>, will have <i>root</i> from the `directorypath` . Then, <i>filesystem</i> will be editable to do operation according to user's will to answer Shift problem.


## PROBLEM 4 ##

To make it easier to monitor activities on their filesystem, Sin and Sei created a log system with the following specifications.

####4a. The system log that will be created is named “SinSeiFS.log” in the user's home directory (/home/[user]/SinSeiFS.log). This system log maintains a list of system call commands that have been executed on the filesystem..

####4b. Because Sin and Sei like tidiness, the logs that are made will be divided into two levels,INFO and WARNING.

####4c. For the WARNING level log, it is used to log the rmdir and unlink syscalls.

####4d. The rest will be recorded at the INFO level.

####4e. The format for logging is: [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]


We have to create a log that records what this program does. That's why we created a 2 Write function (for INFO & Warning) that will notify information and warnings and then store them in SinSeiFS.log

````
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
````	


````
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
````

`xmp_truncate function`
	
````
For the warning, we also need to add the Warning level on the rmdir dan truncate. Beside that, all the other system call can use INFO level.
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
````

`xmp_rmdir function`

````
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
````
	
And there is a special function for warning that it differs only in char * info.

#### Result Image:
| ![](/No4.png) |
|:--:| 
| *No 4 result* |

