#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 1024
#define PORT_NO 2001
#define error(a,b) fprintf(stderr, a, b)

  /*Szükséges saját függvények függvények */

  void statusfogad (int fd, int *tomb1, int *tomb2);
  void rajzol (int tomb1[], int tomb2[]);
  int recvint(int fd);
  void sendint(int fd,int kuldott);

int
main (int argc, char *argv[])
{				// arg count, arg vector   


  /* Declarations */
  int fd;			// socket endpt  
  int flags;			// rcv flags
  struct sockaddr_in server;	// socket name (addr) of server 
  struct sockaddr_in client;	// socket name of client 
  int server_size;		// length of the socket addr. server 
  int client_size;		// length of the socket addr. client 
  int bytes;			// length of buffer 
  int rcvsize;			// received bytes
  int trnmsize;			// transmitted bytes
  int err;			// error code
  int ip;			// ip address
  char on;			// 
  char buffer[BUFSIZE + 1];	// datagram dat buffer area
  char server_addr[16];		// server address    

  /* Initialization */
  on = 1;
  flags = 0;
  server_size = sizeof server;
  client_size = sizeof client;
  sprintf (server_addr, "%s", argv[1]);
  ip = inet_addr (server_addr);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = ip;
  server.sin_port = htons (PORT_NO);
  

/*Játékos pozició tárolók deklarálása*/

  int jt1[2];
  int jt2[2];

  /*kockadobás tároló */

  int dobott;

  /*bábú tároló */

  int babu;

  /*Ki győzött */

  int win = 0;

  /*Én jövök-e */

  int status;

  /* Creating socket */
  fd = socket (AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    {
      error ("%s: Socket creation error.\n", argv[0]);
      exit (1);
    }

  /* Setting socket options */
  setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof on);
  setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof on);

  /* Connecting to the server */
  err = connect (fd, (struct sockaddr *) &server, server_size);
  if (err < 0)
    {
      error ("%s: Cannot connect to the server.\n", argv[0]);
      exit (2);
    }

  while (1)
    {
      /*Win fogadása ( nyert-e már vki */

      win=recvint(fd);

      /*Ha 3ast kapok ( azaz én nyertem ) */

      if (win == 3)
	{

	  /*Utolsó állás fogadása */

	  statusfogad(fd,&jt1,&jt2);

	  /*Állás felrajzolása */

	  rajzol (jt1, jt2);

	  printf ("Nyertél!\n");

	  /*lezárás kilépés */

	  close (fd);
	  exit (0);

	}
/*Ha 4ast kapok ( azaz nem én nyertem ) */
      if (win == 4)
	{
	  /*Utolsó állás fogadása */

	statusfogad(fd,&jt1,&jt2);

	  /*Állás felrajzolása */

	  rajzol (jt1, jt2);

	  printf ("Vesztettél sorry budy:(\n");

	  /*lezárás kilépés */
	  close (fd);
	  exit (0);
	}
	
	/*Jelenlegi állás fogadása*/
	
	statusfogad(fd,&jt1,&jt2);
	
      printf ("%d\n", jt1[0]);
      printf ("%d\n", jt1[1]);
      printf ("%d\n", jt2[0]);
      printf ("%d\n", jt2[1]);
      rajzol (jt1, jt2);

      /*status fogadása ( én jövök -e ) */

      status=recvint(fd);

      /* Ha status 1-es azaz én jövök */

      if (status == 1)
	{
	  /*Dobókocka fogadása */

	  dobott=recvint(fd);
	  
	  /*Adatok kiírása */

	  printf ("Dobott szám: %d\n", dobott);
	  printf ("Bábú választás:");

	  /*Választott bábú beolvasása */

	  scanf ("%d", &babu);

	  /*Helyes bábú megadásának ellenörzése csak 1,2,3-t fogadunk el */

	  while (!((babu > 0) && (babu < 4)))
	    {
	      printf ("Rossz választás válassz újat!\n");
	      scanf ("%d", &babu);
	    }

	  /* Bűbú elküldése a servernek */

	  sendint(fd, babu);
	}

      /*Ha nem én jövök( kiírás és kezdjük az elejéről */

      else
	{
	  printf ("Várakozás a másik játékosra\n");

	}
    }

/*lezárás*/

  close (fd);
  exit (0);

}

void
rajzol (int tomb1[], int tomb2[])
{

  int i, j;
  system ("clear");
  for (i = 0; i < 86; i++)
    printf ("#");
  printf ("\n");
  printf ("\t\t\t\t Ki nevet, a végén?\n");
  printf ("\t\t\t\t    Let's play!:)\n");
  printf ("\t\t\t\t     version 0.5\n");
  for (i = 0; i < 86; i++)
    printf ("#");
  printf ("\n");

  printf ("\n");
  printf ("\n");
  for (j = 0; j < 2; j++)
    {
      printf ("\tPlayer1 %d. bábú   ", j + 1);
      for (i = 1; i < 50; i++)
	{
	  if (i == tomb1[j])
	    printf ("T");
	  else
	    printf (".");
	}
      printf ("\n");
    }
  for (j = 0; j < 2; j++)
    {
      printf ("\tPlayer2 %d. bábú   ", j + 1);
      for (i = 1; i < 50; i++)
	{
	  if (i == tomb2[j])
	    printf ("T");
	  else
	    printf (".");
	}
      printf ("\n");
    }

  printf ("\n");
  printf ("\n");

  for (i = 0; i < 86; i++)
    printf ("#");
  printf ("\n");
  printf
    ("1 -> Lépés az 1. bábúval	      2 -> Lépés a 2. bábúval		    3 -> Felad\n");
  for (i = 0; i < 86; i++)
    printf ("#");
  printf ("\n");

}

void
statusfogad (int fd, int * tomb1, int * tomb2)
{

  tomb1[0]=recvint(fd);
  tomb1[1]=recvint(fd);
  tomb2[0]=recvint(fd);
  tomb2[1]=recvint(fd);
}

void sendint(int fd,int kuldott){
  int trnmsize;
  int ideig;
  
  ideig=htons(kuldott);
  
  trnmsize=send (fd, &ideig, sizeof (int), 0);
      if (trnmsize < 0)
	{
	  printf ("Cannot send data to the client.\n");
	  exit (6);
	}
}

int recvint(int fd){
  int trnmsize;
  int ideig;
  
  
  trnmsize=recv(fd, &ideig, sizeof (int), 0);
      if (trnmsize < 0)
	{
	  printf ("Cannot send data to the client.\n");
	  exit (6);
	}
	
  return (int) ntohs(ideig);
  
}
