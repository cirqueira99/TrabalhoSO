#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// tamanho da mochila(buffer) 
#define N 20
int contPostIt=0;

sem_t mutex; // Semaforo binario para garantir exclusao mutua na regiao critica
sem_t n_vagas; // Semaforo para controlar as posicoes vazias no mochila
sem_t cheio; // Semaforo para controlar as posicoes preenchidas no mochila

int mochila[N]; // Armazena os dados produzidos ou consumidos
int proxPosCheia; // Proxima posicao cheia
int proxPosVazia; // Proxima posicao vazia

// Prototipos das funcoes para o produtor e consumidor
void *usuario(void *);
void *pombo(void *);

int main(void){
	int tam; // tamanho do vetor que irá conter os usuários
	
	printf("\n Digite a quantidade de usuários =>  ");
	scanf("%d",&tam);
    printf("\n");

	//Define a semente da funcao geradora de numeros aleatorios
	srand(time(NULL));

	// Inicializando  as variáveis
	proxPosCheia = 0;
	proxPosVazia = 0;

	// Inicializa os semaforos
	sem_init(&mutex, 0 , 1);
	sem_init(&n_vagas, 0, N);
	sem_init(&cheio, 0, 0);

	// Indentificadores das threads usurario e pompo
	pthread_t thdU[tam]; // criação de um vetor de usuários
 	pthread_t thdP;

	// Inicializa as threads
	for (int i = 0; i < tam; ++i){
		pthread_create(&thdU[i], 0, (void *) usuario, NULL); // semáforos usuários
    }

	pthread_create(&thdP, 0, (void *) pombo, NULL); // semáforo pombo

	// Bloqueiam a thread principal ate que as threads indicadas por thdU[] e thdP terminem.
	for (int i = 0; i < tam; ++i)
		pthread_join(thdU[i],0);
	pthread_join(thdP,0);

	printf("\n");

	exit(0);	
}

// Metodo que produz item
int produz_item( ){
	int val;
	val = rand() % 100;
	printf("\nUsuario escrevendo msg... ");

	return val;
}

//Metodo que a realiza a insercao do dado na mochila
void ColaPostIt( int val ){
	if( contPostIt < N ){
		mochila[proxPosVazia] = val;
		/* A utilizacao da divisao em modulo implementa um comportamento
		* circular da utilizacao da mochila, ou seja, quando o contador
		* chegar no valor de N (N % N = 0) o valor da variavel voltara
		* ao inicio da mochila.
		*/
		proxPosVazia = ( proxPosVazia + 1 ) % N;
	}
}

// Metodo que faz usuario dormir
void dorme_aleatotio( int item ){
	sleep( item%3 );
}

void *usuario( void *p_arg ){
	int item;

	register int i=0;

	while(1){
		item = produz_item( );
		
		// É dado down em n_vagas e mutex
		sem_wait( &n_vagas );
		sem_wait( &mutex );

		// Faz usuário dormir
		dorme_aleatotio(item);

		// Coloca item na mochila
		ColaPostIt( item );
		contPostIt++;

		if( contPostIt == N ){
			printf("####### Mochila Cheia!! #######\n");
			
			// É dado up em heio	
			sem_post( &cheio );			
		}
		// É dado up em mutex
		sem_post( &mutex );
	}

	pthread_exit( NULL );
}

// Metodo que faz pombo levar a mochila(dormir)
void leva_mochila_ate_B_e_volta( int item ){
	sleep( item%3 );
}

// Metodo que realiza a retirada do dado da mochila
int remove_item(){
	int val;
	
	if( contPostIt > 0 ){
		val = mochila[proxPosCheia];
		proxPosCheia = ( proxPosCheia + 1 ) % N;
	}

	return val;
}

void *pombo( void *p_arg ){
	int item;
	register int i=0;

	while(1){
		// É dado down em cheio e mutex
		sem_wait( &cheio );
		sem_wait( &mutex );

		// Faz pombo dormir
		leva_mochila_ate_B_e_volta( item );
		contPostIt = 0;
		
		for ( int i = 0; i < N; i++ ){
			printf("\nAdic. msg: %d", i);
			item = remove_item();
			
			// É dado up em n_vagas
			sem_post( &n_vagas );
		}
		
		// É dado up em mutex
		sem_post( &mutex );
	}
	pthread_exit( NULL );
}
