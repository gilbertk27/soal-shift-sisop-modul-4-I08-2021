# soal-shift-sisop-modul-4-I08-2021
 
All files will be encrypted using Caesar Cipher with the key:
if : 
- A directory is created with the prefix “encv1_”, then the directory will be an encrypted directory using the v1 encryption method.
- A directory is renamed with the prefix "encv1_", then the directory will be an encrypted directory using the v1 encryption method.
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

