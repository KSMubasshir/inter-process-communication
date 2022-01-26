#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include<iostream>
#include<string.h>

using namespace std;

//semaphore to control sleep and wake up
sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;
sem_t empty3;
sem_t full3;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
pthread_mutex_t lockConsole;

queue<string> q1,q2,q3;




void init_semaphore()
{
	sem_init(&empty1,0,5);
	sem_init(&full1,0,0);
	sem_init(&empty2,0,5);
	sem_init(&full2,0,0);
	sem_init(&empty3,0,5);
	sem_init(&full3,0,0);
	pthread_mutex_init(&lock1,0);
	pthread_mutex_init(&lock2,0);
	pthread_mutex_init(&lock3,0);
	pthread_mutex_init(&lockConsole,0);
}

void printq(queue<string> q1,queue<string> q2,queue<string> q3)
{
	queue<string> temp1 = q1;
	queue<string> temp2 = q2;
	queue<string> temp3 = q3;
	
	cout<<"q1 :";
	while (!temp1.empty())
	{
		cout << temp1.front() << " ";
		temp1.pop();
	}
	cout << endl;

	cout<<"q2 :";
	while (!temp2.empty())
	{
		cout << temp2.front() << " ";
		temp2.pop();
	}
	cout << endl;

	cout<<"q3 :";
	while (!temp3.empty())
	{
		cout << temp3.front() << " ";
		temp3.pop();
	}
	cout << endl << endl;
}

void * chefX(void * arg)
{	
	printf("%s\n",(char*)arg);
	int i;
	while(1)
	{
		pthread_mutex_lock(&lockConsole);
		printf("ChefX trying to produce Chocolate Cake\n");
		pthread_mutex_unlock(&lockConsole);

		sem_wait(&empty1);
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lockConsole);		
		sleep(1);
		q1.push("Chocolate");

		
		printf("ChefX produced Chocolate Cake\n");
		printq(q1,q2,q3);
		pthread_mutex_unlock(&lockConsole);

		pthread_mutex_unlock(&lock1);		
		sem_post(&full1);
		
		
		
	}
}


void * chefY(void * arg)
{	
	printf("%s\n",(char*)arg);
	int i;
	while(1)
	{
		pthread_mutex_lock(&lockConsole);
		printf("ChefY trying to produce Vanilla Cake\n");
		pthread_mutex_unlock(&lockConsole);		
		
		sem_wait(&empty1);
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lockConsole);
		sleep(1);
		q1.push("Vanilla");

		
		printf("ChefY produced Vanilla Cake\n");
		printq(q1,q2,q3);
		pthread_mutex_unlock(&lockConsole);

		pthread_mutex_unlock(&lock1);
		sem_post(&full1);
		
		
	}
}


void * chefZ(void * arg)
{	
	printf("%s\n",(char*)arg);
	int i;
	while(1)
	{
		pthread_mutex_lock(&lockConsole);
		printf("ChefZ trying to take Cake from q1\n");
		pthread_mutex_unlock(&lockConsole);				
				
		sem_wait(&full1);
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lockConsole);
		sleep(1);
		string item=q1.front();
		q1.pop();

		
		cout<<"ChefZ consumed "<<item<<" Cake"<<endl;
		printq(q1,q2,q3);
		pthread_mutex_unlock(&lockConsole);

		pthread_mutex_unlock(&lock1);
		sem_post(&empty1);
		
		

			

		if(item=="Vanilla"){
				
			pthread_mutex_lock(&lockConsole);
			printf("ChefZ trying to put Vanilla Cake into q2\n");
			pthread_mutex_unlock(&lockConsole);
			
			sem_wait(&empty3);
			pthread_mutex_lock(&lock2);
			pthread_mutex_lock(&lockConsole);
			sleep(1);
			q2.push(item);

			
			printf("ChefZ produced Vanilla Cake\n");
			printq(q1,q2,q3);
			pthread_mutex_unlock(&lockConsole);			
			
			pthread_mutex_unlock(&lock2);
			sem_post(&full3);

			
		}
		else{
			
			pthread_mutex_lock(&lockConsole);
			printf("ChefZ trying to put Chocolate Cake into q3\n");
			pthread_mutex_unlock(&lockConsole);
			
			sem_wait(&empty2);
			pthread_mutex_lock(&lock3);
			pthread_mutex_lock(&lockConsole);
			sleep(1);
			q3.push(item);

			
			printf("ChefZ produced Chocolate Cake\n");
			printq(q1,q2,q3);
			pthread_mutex_unlock(&lockConsole);			
			
			pthread_mutex_unlock(&lock3);
			sem_post(&full2);
			
			
		}
	}
}

void * waiter1(void * arg)
{
	printf("%s\n",(char*)arg);
	int i;
	while(1)
	{	
		pthread_mutex_lock(&lockConsole);
		printf("Waiter1 trying to take Chocolate Cake from q3\n");
		pthread_mutex_unlock(&lockConsole);		
		
		sem_wait(&full2);
 		pthread_mutex_lock(&lock2);
		pthread_mutex_lock(&lockConsole);
		sleep(1);
		
		string item = q3.front();
		q3.pop();

		
		printf("Waiter1 consumed Chocolate Cake\n");
		printq(q1,q2,q3);
		pthread_mutex_unlock(&lockConsole);
		
		pthread_mutex_unlock(&lock2);
		sem_post(&empty2);

		
	}
}

void * waiter2(void * arg)
{
	printf("%s\n",(char*)arg);
	int i;
	while(1)
	{	
		pthread_mutex_lock(&lockConsole);
		printf("Waiter2 trying to take Vanilla Cake from q2\n");
		pthread_mutex_unlock(&lockConsole);		
		
		sem_wait(&full3);
		pthread_mutex_lock(&lock3);
		pthread_mutex_lock(&lockConsole);
 		
		sleep(1);

		string item = q2.front();
		q2.pop();

		
		printf("Waiter2 consumed Vanilla Cake\n");
		printq(q1,q2,q3);
		pthread_mutex_unlock(&lockConsole);
		
		pthread_mutex_unlock(&lock3);
		sem_post(&empty3);

		
	}
}



int main(void)
{	
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;
	
	init_semaphore();
	
	char * message1 = "i am chefX";
	char * message2 = "i am chefY";
	char * message3 = "i am chefZ";
	char * message4 = "i am waiter1";
	char * message5 = "i am waiter2";	
	
	pthread_create(&thread1,NULL,chefX,(void*)message1 );
	pthread_create(&thread2,NULL,chefY,(void*)message2 );
	pthread_create(&thread3,NULL,chefZ,(void*)message3 );
	pthread_create(&thread4,NULL,waiter1,(void*)message4 );
	pthread_create(&thread5,NULL,waiter2,(void*)message5 );
	
	
	while(1);
	return 0;
}
