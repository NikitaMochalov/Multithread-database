#include <iostream>
#include <queue>
#include <unistd.h>

// Хранилище
std::queue<char> storage;
// Вместимость
int capacity;
// Инициализация замков
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t write_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t read_mtx = PTHREAD_MUTEX_INITIALIZER;
// Количество потоков читателей и писателей
int nReaders;
int nWriters;
// Количество действий читателей и писателей
int readerPortion;
int writerPortion;
// Количество действий, выполненных за программу
int actionsCount = 0;
// Количество оставшихся действий читателей и писателей
int readingsLeft;
int writingsLeft;

void* reader(void* arg)
{
	for(int i = 0; i < readerPortion;)
	{
		usleep(rand()%1000000 + 100000);
		
		pthread_mutex_lock(&mutex); 
		
		if(!storage.empty()) {
			char elementToRead = storage.front();
			actionsCount++;
			i++;
			std::cout << "Прочитал:" << elementToRead << "   Элементов в очереди: " << storage.size()-1 << '\n';
			storage.pop();
			pthread_mutex_lock(&read_mtx); ////
			readingsLeft--;
			pthread_mutex_unlock(&read_mtx); ////
		}
		else if(writingsLeft == 0)
			i = readerPortion;
			
		pthread_mutex_unlock(&mutex); 
	}
	return 0;
}

void* writer(void* arg)
{
	for(int i = 0; i < writerPortion;)
	{
		usleep(rand()%1000000 + 100000);
		pthread_mutex_lock(&mutex);
		
		if (capacity > storage.size()) {
			pthread_mutex_lock(&write_mtx);
			
			storage.push(*(char*)arg);
			actionsCount++;
			i++;
			writingsLeft--;
			std::cout << "Записал: " << *(char*)arg << "   Элементов в очереди: " << storage.size() << '\n';
			
			pthread_mutex_unlock(&write_mtx);
		}
		else if(readingsLeft == 0) 
			i = writerPortion;
			
		pthread_mutex_unlock(&mutex);
		
		
	}
	return 0;
}

int main(int argc, char **argv)
{
	capacity = 60;
	printf("Количество писателей: ");
	std::cin >> nWriters;
	pthread_t writers[nWriters];
	
	printf("Количество читателей: ");
	std::cin >> nReaders;
	pthread_t readers[nReaders];
	
	printf("Порция писателей: ");
	std::cin >> writerPortion;
	writingsLeft = nWriters*writerPortion;
	
	printf("Порция читателей: ");
	std::cin >> readerPortion;
	readingsLeft = nReaders*readerPortion;
	
	printf("Вместимость : ");
	std::cin >> capacity;
	
	std::string keys = "ABCDEFGHIKLMNOPQRSTVXYZabcdefghiklmnopqrstvxyz0123456789";
	
	for(int i = 0; i < nWriters; i++)
	{
		pthread_create(&writers[i], NULL, writer, &keys[i]);
	}
	
	for(int i = 0; i < nReaders; i++)
	{
		pthread_create(&readers[i], NULL, reader, NULL);
	}
	
	for(int i = 0; i < nWriters; i++)
		pthread_join(writers[i], 0);
	
	for(int i = 0; i < nReaders; i++)
		pthread_join(readers[i], 0);
	
	printf("Действий совершено: %d", actionsCount);
	return 0;
}
