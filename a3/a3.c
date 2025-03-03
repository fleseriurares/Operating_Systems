#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include<sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>


#define RESP_PIPE "RESP_PIPE_84889"
#define REQ_PIPE "REQ_PIPE_84889"


void build_int(unsigned int *test_offset, char *data,int index)
{
    *test_offset = 0;
    *test_offset = data[index] & 0x000000FF;
    *test_offset += (data[index+1] & 0x000000FF) << 8;
    *test_offset += (data[index+2] & 0x000000FF) << 16;
    *test_offset += (data[index+3] & 0x000000FF) << 24;
                
}

int get_section_number(char *data, int logical_address, int *block_start)
{
    unsigned int no_sect;
    build_int(&no_sect,data,10);
    int var = logical_address / 1024;
    int cont = 0;
    for(int i = 1; i <= no_sect; i++)
    {
        int index_size = 11 + (i - 1) * 15 + 11;   
        unsigned int size = 0;
      //  printf("snmb %d : size %d\n",i,size);

        build_int(&size,data,index_size);
        printf("snmb %d : size %d\ncont: %d\n",i,size, cont);
        (*block_start) = cont;
        cont += (size/1024);
        if(size % 1024 != 0)
        {
            cont ++;
        }
        if(cont > var)
        {   
            return i;
        }
    }   
    return 10000;
}


int main()
{
    int fd1 = -1, fd2 = -2;
   
    int size;
    char sir[250];
    unsigned int offset_read = 0;
    unsigned int no_of_bytes_read = 0;
    unsigned int section_no = 0;
    unsigned int offset_SF = 0;
    unsigned int no_of_bytes_SF = 0;
    unsigned int no_of_bytes_logical = 0;
    unsigned int logical_offset = 0;
    //verif
     int fd_ver = -1;
    fd_ver = open("fisier.txt",O_WRONLY);
    if(fd_ver == -1)
    {
       // printf("of");
    }
        char c_ver[10];
     
    unlink(RESP_PIPE);
    if(mkfifo(RESP_PIPE, 0600) != 0) {
        perror("ERROR\ncannot create the response pipe");
        return -1;
    }

    fd2 = open(REQ_PIPE, O_RDONLY);
    if(fd2 == -1)
    {
        perror("ERROR\ncannot open the request pipe");
        return -1;
    }

    fd1 = open(RESP_PIPE, O_WRONLY);
    if(fd1 == -1)
    {
        perror("ERROR\ncannot open the response pipe");
        return -1;
    }

    char s[250];
  //  scanf("%s",s);
    strcpy(s,"START!\0");

    if(write(fd1, &s, strlen(s) ) != -1)
    {
        if(!strcmp(s,"START!"))
        {
            //printf("SUCCESS");
          //  write(fd_ver,&s,strlen(s) );
        }
    }

    //pt verificare
  

    char c;
             int shmFd;
        char *data = NULL;
    while(1 == 1)
    {
        //get request

        c = '0';
        int i = 0;
        unsigned int nr_octeti ;
        read(fd2,&c,1);
        c_ver[0] = c;
        write(fd_ver,c_ver,1);
        while(c != '!')
        {   
            s[i] = c;
            read(fd2,&c,1);
            c_ver[0] = c;
            write(fd_ver,c_ver,1);
            i++;
        }
        //write(fd_ver,"=",1);
        s[i] = '\0';
        

        if(!strcmp(s,"VARIANT"))
        {
            write(fd1,"VARIANT!",8);
            write(fd1,"VALUE!",6);
            int var = 84889;
            write(fd1,&var,sizeof(var));
           // close(fd1);
           // close(fd2);
           // unlink(RESP_PIPE);
           return 0;
        }
        else if(!strcmp(s,"CREATE_SHM"))
        {
           
            read(fd2,&nr_octeti,sizeof(nr_octeti));
            write(fd1,"CREATE_SHM!",11);
            shmFd = shm_open("/EeQcHf", O_CREAT | O_RDWR, 0644);
            if(shmFd<0)
            {
                write(fd1,"ERROR!",6);
                return -1;
            }
            ftruncate(shmFd, (nr_octeti) * sizeof(char));
            //feedback
             write(fd1,"SUCCESS!",8);
            
        }
        else if(!strcmp(s,"WRITE_TO_SHM"))
        {
            unsigned int offset, value;
            read(fd2,&offset,sizeof(offset));
            read(fd2,&value,sizeof(value));
            write(fd1,"WRITE_TO_SHM!",13);

            if(offset < nr_octeti/4) //!
            {
                lseek(shmFd,offset,SEEK_SET);
                write(shmFd,&value,sizeof(unsigned int));
            }
            else
            {
                write(fd1,"ERROR!",6);
                return -1;
            }
             write(fd1,"SUCCESS!",8);
        }
        else if(!strcmp(s,"MAP_FILE"))
        {
            //citire nume
            char ch;
            int j = 0;

            write(fd1,"MAP_FILE!",9);
            read(fd2,&ch,1);
            while(ch != '!')
            {   
                sir[j] = ch;
                read(fd2,&ch,1);
                j++;
            }
            sir[j]='\0';
            int fd_fis = -1;
            fd_fis = open(sir, O_RDONLY);
            
            if(fd_fis == -1)
            {
                write(fd1,"ERROR!",6);
                return -1;
            }

            
             size = lseek(fd_fis,0,SEEK_END);
            lseek(fd_fis,0,SEEK_SET);
            data = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd_fis, 0);
            if(data == (void*)-1)
            {
                //printf("Could not map file");
                close(fd_fis);
                write(fd1,"ERROR!",6);
                return 1;
             }
             else
             {
                write(fd1,"SUCCESS!",8);
             }

        }
        else if(!strcmp(s,"READ_FROM_FILE_OFFSET"))
        {
             
            read(fd2,&offset_read,sizeof(offset_read));
            read(fd2,&no_of_bytes_read,sizeof(no_of_bytes_read));
            write(fd1,"READ_FROM_FILE_OFFSET!",22);
                          

            if(data == (void*)-1 || (offset_read + no_of_bytes_read) >= size || (shmFd < 0))
            {
                write(fd1,"ERROR!",6);
            }
            else
            {
                 
                for(int j = offset_read;j<offset_read + no_of_bytes_read;j++)
                {
                    write(shmFd,&data[j],1);
                }
                 write(fd1,"SUCCESS!",8);
            }
        }
        else if(!strcmp(s,"READ_FROM_FILE_SECTION"))
        {
            section_no = 0;
            offset_SF = 0;
            no_of_bytes_SF = 0;
            read(fd2,&section_no,sizeof(section_no));
            read(fd2,&offset_SF,sizeof(offset_SF));
            read(fd2,&no_of_bytes_SF,sizeof(no_of_bytes_SF));

            unsigned int no_sect = 0;
            unsigned int offset_sect = 0;
            unsigned int size_sect = 0;
            int index_no_sect = 10;
            int index_offset = 11 +(section_no - 1) * 15 + 7;
            int index_size = 11 + (section_no - 1) * 15 + 11;
            build_int(&no_sect,data,index_no_sect);
            build_int(&offset_sect,data,index_offset);
            build_int(&size_sect,data,index_size);
            
            offset_sect += offset_SF ;
            if(section_no < 1 || section_no > no_sect || size_sect < offset_SF + no_of_bytes_SF || size_sect > size)
            {
                write(fd1,"READ_FROM_FILE_SECTION!",23);
                write(fd1,"ERROR!",6);            
                }
            else
            {
                lseek(shmFd,0,SEEK_SET);
                for(int i = 0; i< no_of_bytes_SF; i++)
                {
                    //printf("%c",data[offset_sect]);
                    write(shmFd,&data[offset_sect],sizeof(char));
                    offset_sect ++;
                }

                write(fd1,"READ_FROM_FILE_SECTION!",23);
                write(fd1,"SUCCESS!",8);
                
            }
        }
        else if(!strcmp(s,"READ_FROM_LOGICAL_SPACE_OFFSET"))
        {
            read(fd2,&logical_offset,sizeof(logical_offset));
            read(fd2,&no_of_bytes_logical,sizeof(no_of_bytes_logical));

            int block_start = 0;      
            int section_number = get_section_number(data, logical_offset, &block_start);
            int section_offset2 = logical_offset - (block_start * 1024); // offsetul din cadrul sectiunii
            unsigned int no_of_sections = 0;
            unsigned int offset_sect = 0;
            build_int(&no_of_sections,data,10);
            build_int(&offset_sect,data,11 +(section_number - 1) * 15 + 7);
            offset_sect += section_offset2;
            printf("section_nmb = %d", section_number);
            unsigned int section_size = 0;
            build_int(&section_size,data,11 +(section_number - 1) * 15 + 11);
            printf("!section_offset2 = %d!\n",section_offset2);
            printf("offset_sect = %d\n", offset_sect);
            printf("sect_size = %d\n",section_size);
            if(section_number > no_of_sections || (section_offset2 + no_of_bytes_logical) >=section_size)
            {
                printf("off");
                write(fd1,"READ_FROM_LOGICAL_SPACE_OFFSET!",31);
                write(fd1,"ERROR!",6); 
            }
            else
            {
                
                lseek(shmFd,0,SEEK_SET);
                printf("\n:\n");
                for(int i = 0; i< no_of_bytes_logical; i++)
                {   
                 //   printf("%c", data[offset_sect]);
                    write(shmFd,&data[offset_sect],sizeof(char));
                    offset_sect ++;
                }
                printf("\n");
                write(fd1,"READ_FROM_LOGICAL_SPACE_OFFSET!",31);
                write(fd1,"SUCCESS!",8);
            }    
        }
        else if(!strcmp(s,"EXIT")) 
        {
            close(fd1);
            close(fd2);
            unlink(RESP_PIPE);
           return 0;
        }
    
        }

}