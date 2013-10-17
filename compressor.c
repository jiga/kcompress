/* 
 * kcompress.c - Loadable Kernel Module for adding file compressing systemcall
 * Authors:
 * 	Jignesh Kumar Patel <Jignesh.Patel@umkc.edu>
 * 	Mitulkumar Patel    <mvpt33@umkc.edu>
 * 	Harshil Parikh      <happ67@umkc.edu>
 */

#include<linux/unistd.h>
#include<stdio.h>
#include<unistd.h>

#define __NR_sys_kcompress 252
_syscall4(int,sys_kcompress,char*,in,char*,out,int,f,int,l);

void showUsage();

int main(int argc, char* argv[])
{
	int flag = 0;
	int level =5; /* default compression level */
	char* type = NULL;
	char *inputFile = NULL;
	char *outputFile = NULL;
	
	int opt;
	int opterr = 0;

	while((opt = getopt(argc,argv,"i:o:t:l:cx")) != -1)
	{
		switch(opt)
		{
			case 'c':
				flag = 1;
				break;
			case 'x':
				flag = 2;
				break;
			case 'l':
				level = atoi(optarg);
				break;
			case 't':
				type = optarg;
				break;
			case 'i':
				inputFile = optarg;
				break;
			case 'o':
				outputFile = optarg;
				break;
			default:
				showUsage();
				exit(1);
		}
	}
	
	/* do error checking */
	if(inputFile == NULL || outputFile == NULL || type == NULL || flag ==0)
	{
		showUsage();
		exit(1);
	}
	if(strncmp(type,"lzo",3)==0)
	{
		flag +=2; 
	}
	else if(strncmp(type,"zlib",4)==0)
	{
		//no change in flags
	}
	else
	{
		showUsage();
		exit(1);
	}

	/* invoke the system call with arguments 
	 * the system call checks for all errors
	 */	
	if(sys_kcompress(inputFile,outputFile,flag,level)<0)
		perror("sys_kcompress");
	
  return 0;
}

void showUsage()
{

	fprintf(stderr," Usage:\n\tcompressor <option> [LEVEL] -t <library> -i <input> -o <output>\n");
	fprintf(stderr,"\t\t <option>:\n");
	fprintf(stderr,"\t\t\t -c : to compress\n");
	fprintf(stderr,"\t\t\t -x : to decompress\n");
	fprintf(stderr,"\t\t [LEVEL]: optional\n");
	fprintf(stderr,"\t\t\t -l<value> : value between 0 to 9\n");
	fprintf(stderr,"\t\t <library>:\n");
	fprintf(stderr,"\t\t\t zlib : for using ZLIB library\n");
	fprintf(stderr,"\t\t\t lzo  : for using LZO library\n");
}
