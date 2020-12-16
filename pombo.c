#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// tamanho da mochila(buffer) 
#define N 20
int contPostIt=0;

sem_t mutex; // Semaforo binario para garantir exclusao mutua na regiao critica
sem_t n_vagas; // Semaforo para controlar as posicoes vazias no buffer
sem_t cheio; // Semaforo para controlar as posicoes preenchidas no buffer

int mochila[N]; // Armazena os dados produzidos ou consumidos
int proxPosCheia; // Proxima posicao cheia
int proxPosVazia; // Proxima posicao vazia

// Controla a quantidade de dados presentes no buffer
// Prototipos das funcoes para o produtor e consumidor
void *usuario(void *);
void *pombo(void *);

int main(void){
	int tam; // tamanho do vetor que irá conter os usuários
	
	printf("\n Digite a quantidade de usuários =>  ");
	scanf("%d",&tam);
    printf("\n");

	// Mensagens alearótias
	srand(time(NULL));

	// inicializando  as variáveis
	proxPosCheia = 0;
	proxPosVazia = 0;

	// Inicializa os semaforos
	sem_init(&mutex, 0 , 1);
	sem_init(&n_vagas, 0, N);
	sem_init(&cheio, 0, 0);

	// indentificadores das treads usurario e pompo
	pthread_t thdU[tam]; // criação de um vetor de usuários
 	pthread_t thdP;

	// Inicializa os semaforos
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

int produz_item( ){
	int val;
	val = rand() % 100;
	printf("\nUsuario escrevendo msg... ");

	return val;
}

//Metodo que a realiza a insercao do dado no buffer
void ColaPostIt( int val ){
	if( contPostIt < N ){
		mochila[proxPosVazia] = val;
		/* A utilizacao da divisao em modulo implementa um comportamento
		* circular da utilizacao do buffer, ou seja, quando o contador
		* chegar no valor de N (N % N = 0) o valor da variavel voltara
		* ao inicio do buffer.
		*/
		proxPosVazia = ( proxPosVazia + 1 ) % N;
	}
}

void dorme_aleatotio( int item ){
	sleep( item%3 );
}

int contt = 0;
void *usuario( void *p_arg ){
	//printf("contt= %d\n", contt);
	//contt++;
	int item;
	//int pid = getpid();
	register int i=0;

	while(1){
		item = produz_item( );
		
		sem_wait( &n_vagas );
		sem_wait( &mutex );

		dorme_aleatotio(item);

		ColaPostIt( item );
		contPostIt++;

		//printf(" Espaço na Mochila => %d\n", (N - contPostIt) + 1);

		if( contPostIt == N ){
			printf("####### Mochila Cheia!! #######\n");	
			sem_post( &cheio );			
		}

		sem_post( &mutex );
	}

	pthread_exit( NULL );
}


void leva_mochila_ate_B_e_volta( int item ){
	sleep( item%3 );
}

// Metodo que realiza a retirada do dado do buffer
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
		sem_wait( &cheio );
		sem_wait( &mutex );

		leva_mochila_ate_B_e_volta( item );

		contPostIt = 0;

		for ( int i = 0; i < N; i++ ){
			printf("\nAdic. msg: %d", i);
			item = remove_item();
			sem_post( &n_vagas );
		}

		sem_post( &mutex );
	}
	pthread_exit( NULL );
}
