#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>


typedef struct {
   int id;
   sem_t *logSemIn;
   sem_t *logSemOut;
   sem_t *logSem13;
   sem_t *logSemGen2;
   pthread_mutex_t *lock;

}TH_STRUCT;

int ct,ind = 0;
void *function4(void *arg)
{
    TH_STRUCT *s = (TH_STRUCT *) arg;

    if(s->id != 13)
    {
    sem_wait(s->logSemIn);
    }
    else
    {
        sem_wait(s->logSem13);
    }
    info(BEGIN, 4, s->id);
    ct++;
     
    if(ct == 5)
    {
        sem_post(s->logSem13);
    }
    if(s->id != 13)
    {
        sem_wait(s->logSemOut);
    }

    info(END, 4, s->id);
    sem_post(s->logSemOut);
    ct--;

    sem_post(s->logSemIn);
    return NULL;
}



void *function5(void *arg)
{
    TH_STRUCT *s = (TH_STRUCT *) arg;
    if(s->id == 2)
    {
        sem_wait(s->logSemGen2);
    }
    info(BEGIN, 5, s->id);

    info(END, 5, s->id);
    if(s->id == 1)
    {
        sem_post(s->logSem13);
    }
    return NULL;
}


 void *function(void *arg)
{   
    TH_STRUCT *s = (TH_STRUCT *) arg;

    if(s->id == 4)
    {
        sem_wait(s->logSemIn);
    }
    if(s->id == 2)
    {
        sem_wait(s->logSem13);
    }
    info(BEGIN, 3, s->id);

    // printf("%d\n",s->id);
    if(s->id ==5)
    {
        sem_post(s->logSemIn);
       
    }
    
     if(s->id == 5)
    {
        sem_wait(s->logSemOut);
    }
    info(END,3, s->id);

    if(s->id == 4)
    {
        sem_post(s->logSemOut);
    }
    if(s->id == 2)
    {
        sem_post(s->logSemGen2);
    }
    return NULL;
 }

int main(){

    init(); 
    int status = 0;

    sem_t *logSemGen = NULL;
    logSemGen = sem_open("/Task5_log_semaphore", O_CREAT, 0644, 0);
    if(logSemGen == NULL) {
     perror("Could not aquire the semaphore");
  //  exit(0);
    }


    sem_t *logSemGen2 = NULL;
    logSemGen2 = sem_open("/Task5_2_log_semaphore", O_CREAT, 0644, 0);
    if(logSemGen2 == NULL) {
     perror("Could not aquire the semaphore");
  //  exit(0);
    }

    info(BEGIN, 1, 0);
    
    if(fork() == 0) //2
    {
        info(BEGIN, 2, 0);
        
        if(fork() == 0) //3
        {
            info(BEGIN, 3, 0);
            
            pthread_t tids[6];
            TH_STRUCT params[6];
            sem_t logSem1;
            sem_t logSem2;
         //   params[5].id = 5;

        //    pthread_create(&tids[5], NULL, function, &params[5]);       

             if(sem_init(&logSem1, 0, 0) != 0) 
            {
                perror("Could not init the semaphore");
                return -1;
            }  
            if(sem_init(&logSem2, 0, 0) != 0) 
            {
                perror("Could not init the semaphore");
                return -1;
            }     
            params[5].logSemIn = &logSem1;
            params[5].logSemOut = &logSem2;
            params[4].logSemIn = &logSem1;
            params[4].logSemOut = &logSem2;
            for(int i=5; i>=1; i--)
            {   

                    params[i].id = i;
                    //params[i].tids = tids;
                    
                    params[i].logSem13 = logSemGen;
                    params[i].logSemGen2 = logSemGen2;
                    pthread_create(&tids[i], NULL, function, &params[i]);
                  // printf("\n~%ld = %ld~\n",params[i].tids[i],tids[i]);
                    //info(BEGIN, 3, i);
                
            }
            
         //   params[4].id = 4;
         //   params[4].tids = tids;
         //   pthread_create(&tids[4], NULL, function, &params[4]);
            

           
            for(int i=1; i<=5; i++) 
            {
                pthread_join(tids[i], NULL);
            }
 
            info(END, 3, 0);
        }
        else if(fork() == 0) //5
        {
            info(BEGIN, 5, 0);

            pthread_t tids[5];
            TH_STRUCT params[5];
            sem_t logSem;
            sem_t logSem2;
            pthread_mutex_t lock;

            if(sem_init(&logSem, 0, 5) != 0) 
            {
                perror("Could not init the semaphore");
                return -1;
            }  
            if(sem_init(&logSem2, 0, 0) != 0) 
            {
                perror("Could not init the semaphore");
                return -1;
            }    
            for(int i=1; i<=4; i++)
            {
                params[i].id = i;
                params[i].lock = &lock;
                params[i].logSemIn = &logSem;
                params[i].logSemOut = &logSem2;
                params[i].logSem13 = logSemGen;
                params[i].logSemGen2 = logSemGen2;
                pthread_create(&tids[i], NULL, function5, &params[i]);
            }


            for(int i=1; i<=4; i++) 
            {
                pthread_join(tids[i], NULL);
            }
 

            if(fork() == 0)
            {
                info(BEGIN, 7, 0);

                if(fork() == 0)
                {
                    info(BEGIN, 8, 0);
                    
                    info(END, 8, 0);
                }
                wait(&status);
                info(END, 7, 0);
            }
            wait(&status);
            info(END, 5, 0);
        }
        else if(fork() == 0) //6
        {
            info(BEGIN, 6, 0);

            if(fork() == 0) //9
            {
                info(BEGIN, 9, 0);
                
                info(END, 9, 0);
            }
            wait(&status);
            info(END, 6, 0);
        }

        wait(&status);
        wait(&status);
        wait(&status);
        info(END, 2, 0);
    }
    else if(fork() == 0)
    {
        info(BEGIN, 4, 0);
        
        pthread_t tids[36];
        TH_STRUCT params[36];
        sem_t logSem;
        sem_t logSem2;
        sem_t logSem13;
        pthread_mutex_t lock;

         if(sem_init(&logSem, 0, 5) != 0) 
        {
                perror("Could not init the semaphore");
                return -1;
        }  
         if(sem_init(&logSem2, 0, 0) != 0) 
        {
                perror("Could not init the semaphore");
                return -1;
        } 
        if(sem_init(&logSem13, 0, 0) != 0) 
        {
                perror("Could not init the semaphore");
                return -1;
        } 
        for(int i=1; i<=35; i++)
        {
            params[i].id = i;
            params[i].lock = &lock;
            params[i].logSemIn = &logSem;
            params[i].logSemOut = &logSem2;
            params[i].logSem13 = &logSem13;
            pthread_create(&tids[i], NULL, function4, &params[i]);
        }


        for(int i=1; i<=35; i++) 
        {
                pthread_join(tids[i], NULL);
        }
 

        info(END, 4, 0);
    }

    wait(&status);
    wait(&status);

    info(END, 1, 0);
    return 0;

}
