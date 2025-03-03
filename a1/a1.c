#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>


typedef struct SF_
{
	int MAGIC;
	int ind_M;
	int HEADER_SIZE;
	int VERSION;
	int ind_V;
	int NO_OF_SECTIONS;
	int ind_N;
	int SECT_TYPE[5000];
	int ind_S;
	long long int SECT_NAME[5000];
	unsigned int SECT_OFFSET[5000];
	long long int SECT_SIZE[5000];
} SF_file;

const int variant = 84889;

int revN(int a)
{
	int rez = 0;
	while (a > 0)
	{
		rez = rez * 256 + a % 256;
		a = a / 256;
	}
	return rez;
}

long long int revN2(long long int a)
{
	long long int rez = 0;
	while (a > 0)
	{
		rez = rez * 256 + a % 256;
		a = a / 256;
	}
	return rez;
}

int nameStartsWith(char *s, char *aux)
{
	if (aux == NULL)
	{
		return 1;
	}
	for (int i = 0; i < strlen(aux); i++)
	{
		if (s[i] != aux[i])
		{
			return 0;
		}
	}
	return 1;
}

void convToString(long long int number, char rez[7])
{
	number = revN2(number);
	int i=0;
	while(number != 0)
	{
		rez[i]=number%256;
		number /= 256;
		i++;
	}
	rez[6]='\0';

}

int testMagic(int magic)
{
	if(magic!=0x4f754763)
	{
		return 0;
	}
	return 1;
}

int testVersion(int version)
{
	if(version>=12 && version <=64)
	{
		return 1;
	}
	return 0;
}

int testNOSections(int nrOSections)
{
	if(nrOSections == 2 || (nrOSections >= 6 && nrOSections <= 14))
	{
		return 1;
	}
	return 0;
}

int testSectType(int section)
{
	if(section == 31 || section == 90 || section == 97 || section == 93)
	{
		return 1;
	}
	return 0;
}

int sizeGreater(struct stat statbuf, int value)
{
	// struct stat statbuf;
	//	lstat(path, &statbuf);

	if (statbuf.st_size > value)
	{
		return 1;
	}
	return 0;
}


SF_file readSF(int fd)
{
	SF_file file;
	 long int buff[20];
	 unsigned int buff2[20];
	read(fd, buff, 4);
	file.MAGIC = buff[0];
	buff[0] = 0;

	read(fd, buff, 2);
	file.HEADER_SIZE = buff[0];
	buff[0] = 0;

	read(fd, buff, 4);
	file.VERSION = buff[0];
	buff[0] = 0;

	read(fd, buff, 1);
	file.NO_OF_SECTIONS = buff[0];
	buff[0] = 0;

	for(int i=0;i<file.NO_OF_SECTIONS;i++)
	{	read(fd, buff, 6);
		file.SECT_NAME[i] = revN2(buff[0]);
		buff[0] = 0;

		read(fd, buff, 1);
		file.SECT_TYPE[i] = buff[0];
		buff[0] = 0;
		

		read(fd, buff, 4);
		file.SECT_OFFSET[i] = buff[0];
		buff[0] = 0;
		read(fd, buff2, 4);
		file.SECT_SIZE[i] = (buff2[0]);
	//	printf("!%d!",buff2[0]);
		buff2[0] = 0;
	}

	return file;
}



int fileTest(const char *path)
{
	int fd = -1;
	int ind= -1;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{	close(fd);
		perror("Nu se poare deschide fisierul");
	}
	  long int buff[20];
	for(int i=0;i<20;i++)
	{
		buff[i]=0;
	}
	SF_file file;

	read(fd, buff, 4);
	file.MAGIC = revN(buff[0]);
	buff[0] = 0;
	if(!testMagic(file.MAGIC))
	{	
		close(fd);
		return 0;
	}
	
	read(fd, buff, 2);
	file.HEADER_SIZE = revN(buff[0]);
	buff[0] = 0;

	read(fd, buff, 4);
	file.VERSION = revN(buff[0]);
	buff[0] = 0;

	if(ind == -1 && !testVersion(file.VERSION))
	{
		close(fd);
		return 0;
	}

	read(fd, buff, 1);
	file.NO_OF_SECTIONS = revN(buff[0]);
	buff[0] = 0;

	
	if(ind == -1 && !testNOSections(file.NO_OF_SECTIONS))
	{
		close(fd);
		return 0;
	}
	for(int i=0;i<file.NO_OF_SECTIONS;i++)
	{	read(fd, buff, 6);
		file.SECT_NAME[i] = revN2(buff[0]);
		buff[0] = 0;

		read(fd, buff, 1);
		file.SECT_TYPE[i] = revN(buff[0]);
		buff[0] = 0;
	//	printf("%d\n",file.SECT_TYPE[i]);
		if(ind == -1 && !testSectType(file.SECT_TYPE[i]))
		{
			close(fd);
			return 0;
		}

		read(fd, buff, 4);
		file.SECT_OFFSET[i] = revN(buff[0]);
		buff[0] = 0;
		read(fd, buff, 4);
		file.SECT_SIZE[i] = buff[0];
	//	printf("!%d!",buff2[0]);
		buff[0] = 0;
	}

	//printf("ind=%d\n", ind);

	//return file;
	return 1;
	close(fd);
}



int findAllCond(const char *path)
{
	int fd = -1;
	fd = open(path, O_RDONLY);
	if (fd == -1 )
	{	close(fd);
		printf("ERROR\ninvalid file");
		return 0;
	}
	if(!fileTest(path))
	{	close(fd);
		return 0;
	}
	SF_file file = readSF(fd);
	
	int validSections = 0;

	for(int i=1; i <= file.NO_OF_SECTIONS ; i++)
	{

	//lseek(fd, file.SECT_OFFSET[i - 1],SEEK_SET);


		//char buff[1700000];
		char buff[501];
		
		lseek(fd, file.SECT_OFFSET[i - 1]+1,SEEK_SET);
		int a = 500;
		int lCt = 1;
		if(file.SECT_SIZE[i - 1]<500)
		{
			a = file.SECT_SIZE[i - 1];
		}
		long int rem = file.SECT_SIZE[i-1];
		do
		{
			rem = rem - a + 1;
			lseek(fd,-1,SEEK_CUR);
			
			read(fd, buff, a);
			buff[a] = 0;
			for(int i=0 ;i<a - 1; i++)
			{
				if(buff[i] == '\r' && buff[i+1] == '\n')
				{
					lCt++;
				}
			}
			if(rem<500)
			{
				a = rem;
			}
			else{
				a=500;
			}
			
		}while(lCt<=16 && rem - 1 >0);


		//int charCt = 0;
	//	printf("lCt=%d\n",lCt);
		if(lCt == 16)
		{
			validSections++;
		}
	////for(int i=0; i < file.NO_OF_SECTIONS ; i++)
	////{
		//char buff[100000];

	//	lseek(fd, file.SECT_OFFSET[i], SEEK_SET);
		//printf("noLines = %d\n",noOfLines(path, i));
		////if(noOfLines(path, i) == 16)
		////{
			////validSections ++;
		///}
		//int ct=1;
		//read(fd, buff, file.SECT_SIZE[i]);
		//buff[file.SECT_SIZE[i]] = 0;
		//char *p= (char *) memmem(buff,file.SECT_SIZE[i],"\x0d\x0a",2);
		//char *p = strstr(buff,"\x0d\x0a");
		//char buffprev = buff[0];
		//char buffprev=buff[0];
		//char a = '\x0a';
		//char d = '\x0d';
		//printf("%s", buff);
		
		
		//printf("ct = %d--",ct);
		//printf(" %d ,",noOfLines(path, i));
	
		//printf("\nvS = %d--\n",validSections);
	}
	close(fd);

	if(validSections >=2)
	{
		return 1;
	}
	return 0;
	
}
/*
int findAllCond(const char *path)
{
	int fd = -1;
	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		printf("ERROR\ninvalid file");
		return 0;
	}

	SF_file file = readSF(fd);
	int validSections = 0;

	for(int i=0; i < file.NO_OF_SECTIONS ; i++)
	{
		//printf("ok");
		char buff[550];
		validSections = 0;
		int lCt=1;
		int offFinal = lseek(fd, file.SECT_OFFSET[i]+ file.SECT_SIZE[i] , SEEK_SET);
		lseek(fd, file.SECT_OFFSET[i] + 1, SEEK_SET);

		int k = 500;
		           
		while(k == 500 && lCt<17)
		{
			
			if(offFinal - lseek(fd, 0, SEEK_CUR) +1>=500)
			{
				k=500;
			}
			else
			{
				k = offFinal - lseek(fd, 0, SEEK_CUR);
			}//printf("~%d~",k);

			lseek(fd, -1, SEEK_CUR);
			read(fd, buff, k);

			while(strstr(buff,"\x0d\x0a")!=0)
			{
				lCt ++;
				char *p = strstr(buff,"\x0d\x0a");
				strcpy(buff,p+2);
			}

		}

		if(lCt == 16)
		{
			validSections ++;
		}
		//printf("validSections = %d\n",validSections);
	}
	if(validSections >= 2)
	{
		return 1;
	}
	return 0;
}*/

void findAll(const char *path, int s)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char path2[2048];
	struct stat statbuf;
	//printf("ok");
	dir = opendir(path);
	if (dir == NULL)
	{	closedir(dir);
		perror("ERROR\ninvalid directory path");
		return;
	}
	if (s == 0)
	{
		printf("SUCCESS\n");
		s++;
	}
	//printf("ok");
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			snprintf(path2, 2048, "%s/%s", path, entry->d_name);
			//	printf("ok1");
			if (lstat(path2, &statbuf) == 0)
			{
				//	printf("ok2");
				if (S_ISDIR(statbuf.st_mode))
				{
					findAll(path2, s);
				}
				else if(findAllCond(path2))
				{
					//	printf("ok3");
					printf("%s\n", path2);
				}
			}
		}
	}
	closedir(dir);
}

/*
int findAllCond(const char *path)
{
	int fd = -1;
	//printf("ok");
	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		printf("ERROR\ninvalid file");
		return 0;
	}

	long int buff[20];

	for(int i=0;i<20;i++)
	{
		buff[i]=0;
	}
	SF_file file;

	read(fd, buff, 4);
	file.MAGIC = revN(buff[0]);
	buff[0] = 0;

	read(fd, buff, 2);
	file.HEADER_SIZE = revN(buff[0]);
	buff[0] = 0;

	read(fd, buff, 4);
	file.VERSION = revN(buff[0]);
	buff[0] = 0;

	read(fd, buff, 1);
	file.NO_OF_SECTIONS = revN(buff[0]);
	buff[0] = 0;

	for(int i=0;i<file.NO_OF_SECTIONS;i++)
	{	read(fd, buff, 6);
		file.SECT_NAME[i] = revN2(buff[0]);
		buff[0] = 0;

		read(fd, buff, 1);
		file.SECT_TYPE[i] = revN(buff[0]);
		buff[0] = 0;


		read(fd, buff, 4);
		file.SECT_OFFSET[i] = revN(buff[0]);
		buff[0] = 0;
		read(fd, buff, 4);
		file.SECT_SIZE[i] = buff[0];
	//	printf("!%d!",buff2[0]);
		buff[0] = 0;
	}

	int morethan16=0;
	
	for(int i=0;i<file.NO_OF_SECTIONS ;i++)
	{
		int ct = 1;
		
		int j=20;
		char buff2[100];
		buff2[0]='\0';
		int k;
		if(file.SECT_SIZE[i]-20<0)
		{
			k=2;
		}
		else{
			k=20;
		}
		lseek(fd,file.SECT_OFFSET[i]+1,SEEK_SET);
		int ind=0;
		while(j<=file.SECT_SIZE[i]-20 || ind<2)
		{
			strcpy(buff2,"");
			lseek(fd,-1,SEEK_CUR);
			j --;
			read(fd,buff2,k);
			//int ind=1;
				
			while(strstr(buff2,"\x0d\x0a")!=0)
			{
				ct++;
				char *p=strstr(buff2,"\x0d\x0a");
				strcpy(buff2,p+2);
			}
			
			if(file.SECT_SIZE[i] -j >=20)
			{
				j +=20;
			}
			else
			{	
				k=file.SECT_SIZE[i] -j;
				ind++;
			}

		}
	//	ct +=2;
		printf("%d-",ct);
		if(ct == 16)
		{//	printf("%d,%d",ct,morethan16);
			morethan16++;
		}
	}	
	printf("\n%d\n",morethan16);
		if(morethan16 >=2)
		{
			return 1;
		}
		return 0;
}
*/



void extractLine(const char *path, int sectionNr, int lineNr)
{
	int fd = -1;
	//printf("ok");
	fd = open(path, O_RDONLY);
	if (fd == -1)
	{	close(fd);
		printf("ERROR\ninvalid file");
		return;
	}


	SF_file file = readSF(fd);	
	//int size = lseek(fd, file.SECT_OFFSET[sectionNr - 1],SEEK_SET);
	//int sizeS = lseek(fd, file.SECT_OFFSET[sectionNr - 1], SEEK_SET);
	//lseek(fd,0,SEEK_SET);
	
	if(sectionNr>file.NO_OF_SECTIONS)
	{	close(fd);
		printf("ERROR\ninvalid section");
		return;
	}
	else 
	{
//		printf("ok1");
		//printf("ok");
	//int nrOfL = noOfLines(path,sectionNr - 1);
	//printf("%d",nrOfL);
	//if(nrOfL<lineNr || lineNr<=0)
	//{
	//	printf("ERROR\ninvalid line");
	//	return; 
	//}
	//else
	//{
		
		//lseek(fd, file.SECT_OFFSET[sectionNr - 1],SEEK_SET);


		char buff[501];
		long int sp=file.SECT_OFFSET[sectionNr- 1] + file.SECT_SIZE[sectionNr-1];
		
		lseek(fd, file.SECT_OFFSET[sectionNr- 1] + file.SECT_SIZE[sectionNr-1]-1,SEEK_SET);
		int a = 500;
		int rd = 0;
		int lCt = 1;
		int ind = 0;
		long int rem = file.SECT_SIZE[sectionNr-1];
		//lseek(fd, -a+1 ,SEEK_CUR);
		while(lCt < lineNr && rem > 1 && ind == 0)
		{
		if(rem<500)
			{
				a = rem;
			}
			else{
				a=500;
			}

			rem = rem - rd + 1;
			lseek(fd, -rd - a +1,SEEK_CUR);
			
			rd = read(fd, buff, a);
		//	printf("%s",buff);
			buff[a] = 0;
			for(int i=a - 1 ;i>0 && ind == 0; i--)
			{//printf("ok");
				if(buff[i] == '\n' && buff[i-1] == '\r')
				{	
					lCt++;
					if(lCt == lineNr)
					{
						sp = lseek(fd, -(a-1-i+1),SEEK_CUR); // a -1 -i + 1?
					//	printf("\n%ld\n",sp);
						ind = 1;
					}
				}
			}
			
			
		}
		if( lCt != lineNr)
		{
			close(fd);
			printf("ERROR\ninvalid line");
			printf("%d",lCt);
			printf("--%d",ind);
			return;
		}
		ind = 0;
		rem = sp - file.SECT_OFFSET[sectionNr -1];
	//	printf("rem=%d",sp);
		lseek(fd, sp, SEEK_SET);
		printf("SUCCESS\n");
		rd=0;
		while(ind == 0 && rem > 1)
		{
			if(rem<500)
			{
				a = rem;
			}
			else{
				a=500;
			}
			rem = rem - a + 1;
			lseek(fd, -rd - a ,SEEK_CUR);
			
			rd = read(fd, buff, a);

			//printf("rd=%d",rd);
			//printf("%s",buff);
			buff[a] = 0;
			for(int i=a - 1 ;i>=0 && ind == 0; i--)
			{
				
				if(i>0 && buff[i] == '\n' && buff[i-1] == '\r')
				{	
					lCt++;
					if(lCt == lineNr + 1)
					{
						ind = 1;
					}
				}
				else
				{
				//	printf("i=%d\n",i);			
				printf("%c",buff[i]);
				}
			}
			
		}

		/*for(int i = strlen(buff)-1 ;i>=1 && lCt<lineNr ;i--)
		{
			if(buff[i] == '\n' && buff[i-1] == '\r')
			{
				sp=i-2;
				lCt++;
			}
		}
		if(lCt < lineNr)
		{	close(fd);
			printf("ERROR\ninvalid line");
			return;
		}

		int ind = 1;
		printf("SUCCESS\n");
		for(int i = sp; i>=1 && ind ==1; i--)
		{
			if(buff[i] == '\n' && buff[i-1] == '\r')
			{
				ind = 0;
			}
			else
			{
			printf("%c",buff[i]);
			}
		}
		if(ind == 1)
		{
			printf("%c",buff[0]);
		}

		read(fd, buff, file.SECT_SIZE[sectionNr - 1]);
		buff[file.SECT_SIZE[sectionNr - 1]] = 0;
		//char *pprev = NULL;
		char *p = strstr(buff,"\r\n");
		
		while(p!=NULL && lCt <= nrOfL - lineNr)
		{
			lCt++;
			strcpy(buff, p+2);
			//printf("%s\n",buff);
			p=strstr(buff,"\r\n");
		}
		//printf("%s",buff);

		
		if(lineNr != 1)
		{	
			p=strstr(buff,"\r\n");
		int s = p-buff;
			buff[p-buff] = 0;
	
		for(int i = s-1; i>=0 ;i--)
		{
			printf("%c",buff[i]);
		}
		}
		else
		{
			for(int i = strlen(buff) - 1; i>=0 ;i--)
		{
			printf("%c",buff[i]);
		}
		}
		while(buff[k]!=0 && buff[k+1]!=0 )//&& (buff[k] != '\r' || buff[k+1] != '\n'))
		{
			k++;
		}printf("k=%d\n",k);
		buff[k] = 0;
		for(int i=k-1; i>=0; i--)
		{
			printf("%c",buff[i]);
		}
	}}*/
	//lseek(fd,3,SEEK_CUR);
	//read(fd,buff2,aux2);
	//printf("aux2=%d,ct=%d,lineNR=%d",aux2,ct,lineNr);

		//}
	}
	close(fd);
}


void headerTest(const char *path)
{
	int fd = -1;
	int ind= -1;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
	close(fd);
		perror("Nu se poare deschide fisierul");
	}
	  long int buff[20];
	 unsigned int buff2[20];
	for(int i=0;i<20;i++)
	{
		buff[i]=0;
	}
	SF_file file;

	read(fd, buff, 4);
	file.MAGIC = revN(buff[0]);
	buff[0] = 0;
	if(!testMagic(file.MAGIC))
	{
		ind=0;
	}
	
	read(fd, buff, 2);
	file.HEADER_SIZE = revN(buff[0]);
	buff[0] = 0;

	read(fd, buff, 4);
	file.VERSION = revN(buff[0]);
	buff[0] = 0;

	if(ind == -1 && !testVersion(file.VERSION))
	{
		ind=1;
	}

	read(fd, buff, 1);
	file.NO_OF_SECTIONS = revN(buff[0]);
	buff[0] = 0;

	if(ind == -1 && !testNOSections(file.NO_OF_SECTIONS))
	{
		ind=2;
	}
//	unsigned char buff2[10];
	for(int i=0;i<file.NO_OF_SECTIONS;i++)
	{	read(fd, buff, 6);
		file.SECT_NAME[i] = revN2(buff[0]);
		buff[0] = 0;

		read(fd, buff, 1);
		file.SECT_TYPE[i] = revN(buff[0]);
		buff[0] = 0;
		if(ind == -1 && !testSectType(file.SECT_TYPE[i]))
		{
			ind=3;
		}

		read(fd, buff, 4);
		file.SECT_OFFSET[i] = revN(buff[0]);
		buff[0] = 0;
read(fd, buff2, 4);
		file.SECT_SIZE[i] = buff2[0];
	//	printf("!%d!",buff2[0]);
		buff2[0] = 0;
	}

	//printf("ind=%d\n", ind);

	if(ind == -1)
	{
		printf("SUCCESS\n");
		printf("version=%d\n", file.VERSION);
		printf("nr_sections=%d\n", file.NO_OF_SECTIONS);
		for(int i=0; i<file.NO_OF_SECTIONS; i++)
		{
			char buffer[7];
			memset(buffer,0,7);
			convToString( file.SECT_NAME[i],buffer);
		//	printf("-%llx-", file.SECT_NAME[i]);
		//	printf("%d=",snprintf(buffer,6,"%lx",file.SECT_NAME[i]));
			buffer[6]='\0';
		//	printf("%s",buffer);
			printf("section%d: %s %d %lld\n", i+1, buffer, file.SECT_TYPE[i], file.SECT_SIZE[i]);
		}
	}
	else
	{	
		printf("ERROR\n");
		switch (ind)
		{
		case 0:
			printf("wrong magic");
			break;
		case 1:
			printf("wrong version");
			break;
		case 2:
			printf("wrong sect_nr");
			break;
		case 3:
			printf("wrong sect_types");
			break;
		default:
			break;
		}
	}
	close(fd);

	//printf("%x, %x, %x, %x, %lx, %x, %lx, %lx", file.MAGIC, file.HEADER_SIZE, file.VERSION, file.NO_OF_SECTIONS,file.SECT_NAME[0],file.SECT_TYPE[0],file.SECT_OFFSET[0], file.SECT_SIZE[0]);
}

void listDir(const char *path, char *nameS, int value)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char path2[512];
	dir = opendir(path);
	struct stat statbuf;
	if (dir == NULL)
	{	closedir(dir);
		perror("ERROR\nNu se poate deschide");
		return;
	}
	printf("SUCCESS\n");
	while ((entry = readdir(dir)) != NULL)
	{

		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			snprintf(path2, 512, "%s/%s", path, entry->d_name);
			if (lstat(path2, &statbuf) == 0)
			{
				if (nameStartsWith(entry->d_name, nameS))
				{
					if (value != -1)
					{
						if (sizeGreater(statbuf, value))
							if (S_ISREG(statbuf.st_mode))
							{
								printf("%s\n", path2);
							}
					}
					else
					{
						printf("%s\n", path2);
					}
				}
			}
		}
	}
	closedir(dir);
	return;
}



void listRec(const char *path, char *nameS, int value, int s)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char path2[512];
	struct stat statbuf;

	dir = opendir(path);
	if (dir == NULL)
	{	closedir(dir);
		perror("ERROR\nNu se poate deschide");
		return;
	}
	if (s == 0)
	{
		printf("SUCCESS\n");
		s++;
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			snprintf(path2, 512, "%s/%s", path, entry->d_name);

			if (lstat(path2, &statbuf) == 0)
			{
				if (nameStartsWith(entry->d_name, nameS))
				{
					if (value != -1)
					{
						if (sizeGreater(statbuf, value))
							if (S_ISREG(statbuf.st_mode))
							{
								printf("%s\n", path2);
							}
					}
					else
					{
						printf("%s\n", path2);
					}
				}
				if (S_ISDIR(statbuf.st_mode))
				{
					listRec(path2, nameS, value, s);
				}
			}
		}
	}
	closedir(dir);
}





int main(int argc, char **argv)
{
	//printf("ok");
	const char comenzi[] = "variant,list,parse,extract,findall";
	const char optiuni[] = "recursive";
	// const char filtre[] = "size_greater,name_starts_with";
	char my_command[10][30]; // 0- comanda, 1- optiunea, 2- filtru, 3 -filtru, >=4 - argumente
	for (int i = 0; i < 10; i++)
	{
		strcpy(my_command[i], "0");
	}
	int k = 4;
	for (int i = 1; i < argc; i++)
	{

		if (strstr(comenzi, argv[i]))
		{
			strcpy(my_command[0], argv[i]);
		}
		else if (strstr(optiuni, argv[i]))
		{
			strcpy(my_command[1], argv[i]);
		}
		else if (strstr(argv[i], "name_starts_with"))
		{
			strcpy(my_command[3], argv[i]);
		}
		else if (strstr(argv[i], "size_greater"))
		{
			strcpy(my_command[2], argv[i]);
		}
		else if(strstr(argv[i], "section"))
		{
			strcpy(my_command[2], argv[i]);
		}
		else if (strstr(argv[i], "line"))
		{
			strcpy(my_command[3], argv[i]);
		}
		else
		{

			strcpy(my_command[k], argv[i]);
			k++;
		}
	}

	if (my_command[0] != NULL)
	{
		if (!strcmp(my_command[0], "variant"))
		{
			printf("%d", variant);
		}
		else if (!strcmp(my_command[0], "list"))
		{
			char *startW = NULL;
			int sz = -1;
			if (strcmp(my_command[2], "0"))
			{
				char *aux = strchr(my_command[2], '=');
				aux += 1;
				sscanf(aux, "%d", &sz);
			}

			if (strcmp(my_command[3], "0"))
			{
				// printf("ok");
				startW = strchr(my_command[3], '=');
				startW++;
			}
			if (!strcmp(my_command[1], "recursive")) // avem o singura optinue pentru comanda "list"
			{
				char *path = strchr(my_command[4], '=');
				path += 1;
				listRec(path, startW, sz, 0);
			}
			else
			{ // printf("%s",my_command[4]);
				if (strstr(my_command[4], "path="))
				{
					char *path = strchr(my_command[4], '=');
					path += 1;
					//	printf("~%d~",sz);
					listDir(path, startW, sz);
				}
			}
		}
		else if (!strcmp(my_command[0], "parse"))
		{
			if (strstr(my_command[4], "path="))
			{
				char *path = strchr(my_command[4], '=');
				path += 1;
				headerTest(path);
			}
		}
		else if (!strcmp(my_command[0], "extract"))
		{	
			
			int lineNr = -1, sectionNr = -1;

			if (strcmp(my_command[2], "0"))
			{
				char *aux = strchr(my_command[2], '=');
				aux += 1;
				sscanf(aux, "%d", &sectionNr);
			}

			if (strcmp(my_command[3], "0"))
			{
				char *aux = strchr(my_command[3], '=');
				aux += 1;
				sscanf(aux, "%d", &lineNr);
				
			}

			if (strstr(my_command[4], "path="))
			{
				char *path = strchr(my_command[4], '=');
				path += 1;
				//printf("path=%s",path);
				//printf("--%d--",lineNr);
				extractLine(path,sectionNr,lineNr);
			}
		}
		else if (!strcmp(my_command[0], "findall"))
		{
			if (strstr(my_command[4], "path="))
			{
				char *path = strchr(my_command[4], '=');
				path += 1;
				findAll(path,0);
			}
		}

	}
	return 0;
}