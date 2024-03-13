//SO IS1 212B LAB11
//Jacek Jacewicz
//jj53675@zut.edu.pl
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
double WYNIK = 0;
HANDLE mutex;
typedef struct {
	int start;
	int ile;
	int retval;
	float *tab;
} ThreadData;

DWORD WINAPI thread(LPVOID data) {
	ThreadData* tdata = (ThreadData*)data;
	double suma = 0;
	DWORD self = GetCurrentThreadId();
	printf("Thread #%d size= %d\n", self,tdata->ile);
	for (int i = 0; i < tdata->ile; i++)
	{
		//printf("dodaje %f do funkcji\n",tdata->tab[tdata->start+i]);
		suma += tdata->tab[tdata->start + i];
	}
	//muteks
	WaitForSingleObject(mutex, INFINITE);
	WYNIK += suma;
	ReleaseMutex(mutex);
	//mutekks
	printf("Thread %d sum= %f\n", self,suma);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc == 3)
	{
		//sprawdzanie czy argumenty sa liczba i intem
		if (atoi(argv[1]) % 1 != 0 || atoi(argv[2]) % 1 != 0)
		{
			fprintf(stderr, "Argument 1 lub 2 nie jest liczba calkowita(jej reszta z dzielenia przez 1 nie jest rowna 0");
		}
		for (int i = 0; i < strlen(argv[1]); i++)
		{
			if (!isdigit(argv[1][i]))
			{
				fprintf(stderr, "Argument 1 nie jest liczba\n");
				return 1;
			}

		}
		for (int i = 0; i < strlen(argv[2]); i++)
		{
			if (!isdigit(argv[2][i]))
			{
				fprintf(stderr, "Argument 2 nie jest liczba\n");
				return 1;
			}

		}
		if (atoi(argv[1]) < 0 || atoi(argv[1]) > 1000000)
		{
			fprintf(stderr, "Argument 1 <1 lub >1000000");
			return 1;
		}
		unsigned int min;
		if (atoi(argv[1]) < 100)
			min = atoi(argv[1]);
		else
			min = 100;
		if (atoi(argv[2]) < 0 || atoi(argv[2]) > atoi(argv[1]))
		{
			fprintf(stderr, "Argument 2 <1 lub >1000000");
			return 1;
		}
		//koniec warunkow argumentu
		int n = atoi(argv[1]), w = atoi(argv[2]);

		//struct timespec start, stop;
		float tablica[n];
		srand(0);
		for (int i = 0; i < n; i++)
		{
			tablica[i] = 1000. * rand() / RAND_MAX;
			//printf("Wyloosoowano: %f\n", tablica[i]);
		}
		mutex = CreateMutex(NULL, FALSE, NULL);

		HANDLE threads[w];
		DWORD threadsids[w];
		ThreadData data[w];
		if (n % w == 0)
		{ 
			int ile = n / w;
			for (int i = 0; i < w; i++)
			{
				data[i].start = i * ile ;
				data[i].ile = ile;
				data[i].tab = tablica;
				//for (int j = 0; j < ile; j++)
				//	printf("tablica: %f\n", data[i].tab[data[i].start +j]);
			}
		}
		else
		{
			int reszta = n % w;
			int ile = n / w;
			for (int i = 0; i < w - 1; i++)
			{
				data[i].start = i * ile;
				data[i].ile = ile;
				data[i].tab = tablica;
				//for (int j = 0; j < ile; j++)
				//	printf("%f\n", data[i].tab[data[i].start + j]);
			}
			data[w - 1].start = (w - 1) * ile;
			data[w - 1].ile = ile + reszta;
			data[w - 1].tab = tablica;
			//for (int j = 0; j < data[w-1].ile; j++)
			//	printf("%f\n", data[w-1].tab[data[w-1].start + j]);

		}

		//liczenie czasu
		LARGE_INTEGER freq,start, stop;
		double czas;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);
		for (int i = 0; i < w; i++) {
			//printf("ile:%d start:%d\n",data[i].ile,data[i].start);
			threads[i] = CreateThread(NULL, 0, thread, data + i, 0, threadsids + i);
		}

		WaitForMultipleObjects(w, threads, 1, INFINITE);
		QueryPerformanceCounter(&stop);
		czas = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000000;

		for (int i = 0; i < w; i++)
			CloseHandle(threads[i]);

		printf("w/Threads:  sum = %f \ttime = %.2fms\n", WYNIK,czas);
		WYNIK = 0;

		QueryPerformanceCounter(&start);
		for (int i = 0; i < n; i++)
		{
			WYNIK += tablica[i];
		}
		QueryPerformanceCounter(&stop);
		czas = (double)(stop.QuadPart - start.QuadPart) / freq.QuadPart * 1000000;
		printf("wo/Threads: sum = %f \ttime = %.2fms\n", WYNIK, czas);
		return 0;
		//clock_gettime(CLOCK_MONOTONIC, &stop);
		//printf("W/threads:\tPi=%.20f\tczas=%.5f\n", WYNIK, (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / 1000000000.0);
		//double wynik2 = 3;
		//clock_gettime(CLOCK_MONOTONIC, &start);
		//for (int i = 1; i < n; i++)
		//	wynik2 = wynik2 + ciag(i);
		//clock_gettime(CLOCK_MONOTONIC, &stop);	
		//printf("Wo/threads:\tPi=%.20f\tczas=%.5f\n", wynik2, (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / 1000000000.0);
		//return 0;

	}
	else
	{
		fprintf(stderr, "blad argumentu\n");
		return 1;
	}

}
