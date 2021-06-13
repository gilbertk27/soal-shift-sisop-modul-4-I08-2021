# soal-shift-sisop-modul-4-I08-2021


- A directory is created with the prefix “AtoZ_”, then the directory will be an encrypted directory using the v1 encryption method.
- A directory is renamed with the prefix "AtoZ_", then the directory will be an encrypted directory using the v1 encryption method.
- After that, if an encrypted directory is renamed to unencrypted, the contents of that directory will be decrypted.

Therefore, we create a command in xmp_mkdir which will send all the files to the encryption function :
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
	
Then do the command to enter into the encryption function
	if(strcmp(cek_substr, "AtoZ_") == 0) //folder encrypt1
	{
		encrypt1(fpath, 1);	
	}

After that we modify xmp_rename. Where later, if previously there was the word "AtoZ_" in a folder and then the folder was renamed to no such word, it will be entered into the decrypt function. If the situation is reversed, it will be entered into the encrypt function. The contents of xmp_rename which signifies the previous statement are as follows:
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
where :
- fromm is the name of the folder before rename, if there is AtoZ_ it will be marked with 1.
- too is the name of the folder after the rename, if there is AtoZ_ it will be marked with 1.

The alterations will occur when the two contradict state and encrypt function if the parameters are both worth -1 then will decrypt

- Encrypt 
		char *encrypt(char* str, bool cek)
		{
		int i, j, k = 0;
		char *ext = strrchr(str, '.');
		if(cek && ext != NULL) k = strlen(ext);
		for(i = 0; i < strlen(str) - k; i++)
		{
				if(str[i]>='A'&&str[i]<='Z')
					str[i] ='Z'+'A'-str[i];
				if(str[i]>='a'&&str[i]<='z')
					str[i]='z'+'a'-str[i];
		}
		return str;
		}

- Decrypt
		char *encrypt(char* str, bool cek)
		{
		int i, j, k = 0;
		char *ext = strrchr(str, '.');
		if(cek && ext != NULL) k = strlen(ext);
		for(i = 0; i < strlen(str) - k; i++)
		{
				if(str[i]>='A'&&str[i]<='Z')
					str[i] ='Z'+'A'-str[i];
				if(str[i]>='a'&&str[i]<='z')
					str[i]='z'+'a'-str[i];
		}
		return str;
		}

After that, we have to create a log that records what this program does. That's why we created a Write function that will notify information and warnings and then store them in fs.log
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

And there is a special function for warning that it differs only in char * info.
