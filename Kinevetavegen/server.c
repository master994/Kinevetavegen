#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 1024		// buffer size
#define PORT_NO 2001		// port number
#define error(a,b) fprintf(stderr, a, b)	// error 'function'

/*Saját függvények deklalárása */

  void kijon (int player1, int player2, int servertime);
  void statuskuld (int fd, int tomb1[], int tomb2[]);
  int vegrehajt (int player, int babu, int lepes, int tomb1[], int tomb2[]);
  void sendint(int fd,int kuldott);
  int recvint(int fd);
  
int
main (int argc, char *argv[])
{				// arg count, arg vector 

/*Használt függvénye deklarálás*/


  
/* Random szám generáláshoz szükséges függvény meghívása*/

  srand (time (0));

  /* Declarations */
  int fd;			// socket endpt
  int jatekos1;			// socket endpt
  int jatekos2;			// socket endpt
  int flags;			// rcv flags
  struct sockaddr_in server;	// socket name (addr) of server
  struct sockaddr_in client1;	// socket name of client 1
  struct sockaddr_in client2;	// socket name of client 2
  int server_size;		// length of the socket addr. server
  int client1_size;		// length of the socket addr. client1
  int client2_size;		// length of the socket addr. clien2
  int bytes;			// length of buffer 
  int rcvsize;			// received bytes
  int trnmsize;			// transmitted bytes
  int err;			// error code
  int on;			// 

  /* Initialization */
  
  on = 1;
  flags = 0;
  bytes = BUFSIZE;
  server_size = sizeof server;
  client1_size = sizeof client1;
  client2_size = sizeof client2;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons (PORT_NO);

  /*Saját változók */

  /*servertime ebből tudjuk ki következik */

  int servertime = 0;

  /*dobókocka eredménye */

  int dobott;

  /*Első játékos 2 bábujának tárolása */

  int jt1[2];

  /*Második játékos 2 bábujának tárolása */
  int jt2[2];

  /*Melyik bábuval lép az aktuális játékos */

  int babu;

  /*Jelzőbit 0->senki sem nyert(nem adta fel) 1->Első játékos nyert 2->Második játékos nyert */

  int win = 0;

  
  /*Jatekos tomb alap állapot */

  jt1[0] = 1;
  jt1[1] = 1;
  jt2[0] = 1;
  jt2[1] = 1;

  printf ("**********************************\n");
  printf ("*                                *\n");
  printf ("*       Ki nevet, a végén?       *\n");
  printf ("*  Created by: Bodrogi Krisztián *\n");
  printf ("*                                *\n");
  printf ("*                                *\n");
  printf ("**********************************\n");
  printf ("\n");
  printf ("\n");
  printf ("\n");
  printf ("Game server starting...\n");
  printf ("\n");
  printf ("\n");
  printf ("\n");

  /* Creating socket */

  fd = socket (AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
    {
      error ("%s: Socket creation error\n", argv[0]);
      exit (1);
    }

  printf ("Socket:\t\tOK\n");

  /* Setting socket options */

  setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof on);
  setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &on, sizeof on);

  /* Binding socket */

  err = bind (fd, (struct sockaddr *) &server, server_size);
  if (err < 0)
    {
      error ("%s: Cannot bind to the socket\n", argv[0]);
      exit (2);
    }

  printf ("Port:\t\tOK\n");

  /* Listening */

  err = listen (fd, 10);
  if (err < 0)
    {
      error ("%s: Cannot listen to the socket\n", argv[0]);
      exit (3);
    }
  printf ("Várakozás a kliensekre!\n");

  /* Accepting connection request */

  jatekos1 = accept (fd, (struct sockaddr *) &client1, &client1_size);
  if (jatekos1 < 0)
    {
      error ("%s: Cannot accept on socket\n", argv[0]);
      exit (4);
    }

  printf ("Első játékos felcsatlakozott!\n");
  
/* Accepting connection request */

  jatekos2 = accept (fd, (struct sockaddr *) &client2, &client2_size);
  if (jatekos2 < 0)
    {
      error ("%s: Cannot accept on socket\n", argv[0]);
      exit (4);
    }
  printf ("Második játékos felcsatlakozott!\n");

  /*Játék kezdődik */

  while (win == 0)
    {
      /*Nyert-e valaki küldése a klienseknek */
      
      sendint (jatekos1, win);
      sendint (jatekos2, win);

      /*Jelenlegi állapot elküldése */
 
      statuskuld (jatekos1, jt1, jt2);
      statuskuld (jatekos2, jt1, jt2);


      /* Döntés ki következik ki várakozzon ki léphet */


      kijon (jatekos1, jatekos2, servertime);

      /* Dobás a kockával */

      dobott = (rand () % 6) + 1;
      printf ("dobott szamm:%d\n", dobott);

      /*Első játékos körének implementálása */
      if (servertime == 0)
	{

	  /*Dobott érték küldése a kliensnek */

	  sendint (jatekos1, dobott);

	  /*Bábú fogadása */

	  babu=recvint(jatekos1);
	  
	  /*Allapot kiírás */

	  printf ("Jatekos 1 %d lepett a %d babuval\n", dobott, babu);

	  /* Lépés végrehajtása és játékállapot megállapítása( győzött-e vki ) */

	  win = vegrehajt (1, babu, dobott, jt1, jt2);

	  /*win értékének kiírása */

	  printf ("Win értéke %d\n", win);

	}

      /* Második körének implementálása */

      if (servertime == 1)
	{
	  /*Dobott érték küldése a kliensnek */

	  sendint(jatekos2, dobott);
	  

	  /*Bábú fogadása */

	  babu=recvint(jatekos2);

	  /*Allapot kiírás */

	  printf ("Jatekos 2 %d lepett a %d babuval\n", dobott, babu);

	  /* Lépés végrehajtása és játékállapot megállapítása( győzött-e vki ) */

	  win = vegrehajt (2, babu, dobott, jt1, jt2);

	  /*win értékének kiírása */

	  printf ("Win értéke %d\n", win);
	}

      if (win == 0)
	{
	  if (servertime == 0)
	    servertime = 1;
	  else
	    servertime = 0;
	}
    }

/*Első játékos győzelme esetén */

  if (win == 1)
    {
      /* Első játékos egy 3ast kap ez jelenti, hogy győzött második játékos kap egy 4est ez jelenti hogy veszett */

      win += 2;
      sendint(jatekos1, win);
      win += 1;
      sendint(jatekos2, win);

      /*Status küldése a végleges állásról */

      statuskuld (jatekos1, jt1, jt2);
      statuskuld (jatekos2, jt1, jt2);
    }

  if (win == 2)
    {

      /* Első játékos egy 4ast kap ez jelenti, hogy veszett második játékos kap egy 3est ez jelenti hogy győzött */

      win += 2;
      sendint(jatekos1, win);
      win -= 1;
      sendint(jatekos2, win);

      /*Status küldése a végleges állásról */

      statuskuld (jatekos1, jt1, jt2);
      statuskuld (jatekos2, jt1, jt2);
    }

  /*Lezárás */

  close (jatekos1);
  close (jatekos2);
  close (fd);
  exit (0);
}

void
statuskuld (int fd, int tomb1[], int tomb2[])
{
  
  /*4 db int kiküldése a jelenlegi állásról*/
  
sendint(fd,tomb1[0]);
sendint(fd,tomb1[1]);
sendint(fd,tomb2[0]);
sendint(fd,tomb2[1]);

    }

void
kijon (int player1, int player2, int servertime)
{

/*Server idő( kinek a köre megy éppen ) eldönti ki jön és küldi 2 integert
/*Ha a kliens 1-est kap azt jelenti ő jön
/*Ha a kliens 0-t kap azt jelenti várnia kell,a másik játékos következik*/

  int trnmsize;
  int a = 1;
  int b = 0;

/*Ha a server idő = 0 akkor 1. játékos következik 1.játékos 1est kap 2.játékos 0-át*/

  if (servertime == 0)
    {
      sendint(player1,a);
      sendint(player2,b);
    
    }

  /*Ha a server idő = 1 akkor 2. játékos következik 1.játékos 0t kap 2.játékos 1-et */

  else
    {
      sendint(player1, b);
      sendint(player2, a);
    }
}

int
vegrehajt (int player, int babu, int lepes, int tomb1[], int tomb2[])
{

/*Ha az első játékos az első bábúval lép 
1. Megnézi, hogy jelenlegi állása + a lépése nem nagyobb-e mint 49 -> csak 49 mező van a pályán ha nagyobb a 49 mezőre állítja
2. Megnézi, hogy van-e ütéshelyzet ( azaz bármely bábú áll-e ezen a mezőn) és ha van az ott lévő bábúkat 1es mezőre állítja*/

  if (player == 1)
    {
      if (babu == 1)
	{
	  if ((tomb1[0] + lepes) > 49)
	    tomb1[0] = 49;
	  else
	    {
	      tomb1[0] += lepes;
	      if (tomb1[1] == tomb1[0] && tomb1[0] != 49)
		tomb1[1] = 1;
	      if (tomb2[0] == tomb1[0] && tomb1[0] != 49)
		tomb2[0] = 1;
	      if (tomb2[1] == tomb1[0] && tomb1[0] != 49)
		tomb2[1] = 1;
	    }
	}
	
/*Ha az első játékos az második bábúval lép 
1. Megnézi, hogy jelenlegi állása + a lépése nem nagyobb-e mint 49 -> csak 49 mező van a pályán ha nagyobb a 49 mezőre állítja
2. Megnézi, hogy van-e ütéshelyzet ( azaz bármely bábú áll-e ezen a mezőn) és ha van az ott lévő bábúkat 1es mezőre állítja*/
      if (babu == 2)
	{
	  if ((tomb1[1] + lepes) > 49)
	    tomb1[1] = 49;
	  else
	    {
	      tomb1[1] += lepes;
	      if (tomb1[0] == tomb1[1] && tomb1[1] != 49)
		tomb1[0] = 1;
	      if (tomb2[0] == tomb1[1] && tomb1[1] != 49)
		tomb2[0] = 1;
	      if (tomb2[1] == tomb1[1] && tomb1[1] != 49)
		tomb2[1] = 1;
	    }
	}
    }
/*Ha az második játékos az első bábúval lép 
1. Megnézi, hogy jelenlegi állása + a lépése nem nagyobb-e mint 49 -> csak 49 mező van a pályán ha nagyobb a 49 mezőre állítja
2. Megnézi, hogy van-e ütéshelyzet ( azaz bármely bábú áll-e ezen a mezőn) és ha van az ott lévő bábúkat 1es mezőre állítja*/
  if (player == 2)
    {
      if (babu == 1)
	{
	  if ((tomb2[0] + lepes) > 49)
	    tomb2[0] = 49;
	  else
	    {
	      tomb2[0] += lepes;
	      if (tomb1[0] == tomb2[0] && tomb2[0] != 49)
		tomb1[0] = 1;
	      if (tomb1[1] == tomb2[0] && tomb2[0] != 49)
		tomb1[1] = 1;
	      if (tomb2[1] == tomb2[0] && tomb2[0] != 49)
		tomb2[1] = 1;
	    }
	}
	
/*Ha az második játékos az második bábúval lép 
1. Megnézi, hogy jelenlegi állása + a lépése nem nagyobb-e mint 49 -> csak 49 mező van a pályán ha nagyobb a 49 mezőre állítja
2. Megnézi, hogy van-e ütéshelyzet ( azaz bármely bábú áll-e ezen a mezőn) és ha van az ott lévő bábúkat 1es mezőre állítja*/

      if (babu == 2)
	{
	  if ((tomb2[1] + lepes) > 49)
	    tomb2[1] = 49;
	  else
	    {
	      tomb2[1] += lepes;
	      if (tomb1[0] == tomb2[1] && tomb2[1] != 49)
		tomb1[0] = 1;
	      if (tomb1[1] == tomb2[1] && tomb2[1] != 49)
		tomb1[1] = 1;
	      if (tomb2[0] == tomb2[1] && tomb2[1] != 49)
		tomb2[0] = 1;
	    }
	}
    }

/* Nyert-e vizsgálat:
/* 1. Ha valakinek mind2 bábúja a 49es mezőn áll akkor nyert!
/* 2. Ha valaki feladom(3as babú) adott meg akkor vesztett a másik nyert*/
/* 3. Egyébként 0-t adunk vissza azaz senki sem nyert!*/

  if ((tomb1[0] == 49) && (tomb1[1] == 49))
    return 1;
  if ((tomb2[0] == 49) && (tomb2[1] == 49))
    return 2;
  if (player == 1 && babu == 3)
    return 2;
  if (player == 2 && babu == 3)
    return 1;
  else
    return 0;

}
/* Integer küldés*/

/*Sendint függvény megfelelő formában küldi tovább a paraméterül adott intet*/

void sendint(int fd,int kuldott)
{
  int trnmsize;
  int ideig=kuldott;
  
  ideig=htons(kuldott);
  
  trnmsize=send (fd, &ideig, sizeof (int), 0);
      if (trnmsize < 0)
	{
	  printf ("Cannot send data to the client.\n");
	  exit (6);
	}
}

/*Recvint függvény megfelelő formában fogadja a megadott socketen egy int-et és megfelelő formátumba konvertálja*/

int recvint(int fd)
{
  int trnmsize;
  int ideig;
  

  
  trnmsize=recv (fd, &ideig, sizeof (int), 0);
      if (trnmsize < 0)
	{
	  printf ("Cannot send data to the client.\n");
	  exit (6);
	}
	
  return (int) htons(ideig);
  
}