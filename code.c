#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <fcntl.h>
#include <mysql/mysql.h>
#include <time.h>
#include <unistd.h>

void download(char * url);
void connection(CURL * curl);
void vote(CURL * curl); 
int controlVote();
int sizeFile(char * fileName);
void saveBdd(int result);
void wait(int result);
int main(int argc, char *argv[]){
	CURL * curl = curl_easy_init();
	while(1){
		connection(curl);
		vote(curl);
		int result = controlVote();
		saveBdd(result);
		wait(result);
	}
	curl_easy_cleanup(curl);

	return 0;
}
 
void download (char * url){
    FILE * file = NULL;
    CURL * session = NULL;   
     
    file = fopen("outRpg.txt", "w");
     
    if (file != NULL)
    {
         
        session = curl_easy_init ();
        curl_easy_setopt (session, CURLOPT_URL, url);
         
        curl_easy_setopt (session,  CURLOPT_WRITEDATA, file);
        curl_easy_setopt (session,  CURLOPT_WRITEFUNCTION, fwrite);
         
        curl_easy_perform (session);
         
        fclose (file);
        curl_easy_cleanup (session);
         
    } 
}

void connection(CURL * curl){
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, "http://intuition-games.net/index.php?page=vote");
		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	
		
		FILE * file = fopen("result.html","w");	
		curl_easy_setopt (curl,  CURLOPT_WRITEDATA, file);
	        curl_easy_setopt (curl,  CURLOPT_WRITEFUNCTION, fwrite);
		 
		// Forcer cURL à utiliser un nouveau cookie de session
		curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1);
		curl_easy_setopt(curl, CURLOPT_POST, 1);

		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");
		
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "login=clemoland&passlog=moktar67&hidden=log&logon=");

		// Fichier dans lequel cURL va écrire les cookies
		// (pour y stocker les cookies de session)
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");

		curl_easy_perform(curl);

		//curl_easy_cleanup(curl);

		fclose(file);
	}
}

void vote(CURL * curl){
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, "http://intuition-games.net/index.php?page=vote");
		FILE * file = fopen("resultVote.html","w");	
		curl_easy_setopt (curl,  CURLOPT_WRITEDATA, file);
	        curl_easy_setopt (curl,  CURLOPT_WRITEFUNCTION, fwrite);
		 
		
		char url [] = "http://api.lifecraft.fr/rpg.php?ids=107827" ;
    		download (url);
		
		int taille = sizeFile("outRpg.txt");
		int fileRpg = open("outRpg.txt", O_RDONLY);
		char nbOut[taille];
		read(fileRpg,nbOut,taille);
    		close(fileRpg);
		char postField[taille+19];
		strcpy(postField,"out=");
		strcat(postField,nbOut);
		strcat(postField,"&vote=Confirmer");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postField);

		// Fichier dans lequel cURL va écrire les cookies
		// (pour y stocker les cookies de session)

		curl_easy_perform(curl);
		//curl_easy_cleanup(curl);
	}
}

int controlVote() {

	char * strSearch = "Félicitation, ton vote";
//	char * fileName = "resultVoteValide.html";
	char * fileName = "resultVote.html";
	FILE *f;
	char c;
	int found=0;
	int pos=0;
	int length;

	f = fopen(fileName,"r");
	if( f == NULL ){
		printf("Error chargement\n");
		return 0;
	}

	length=strlen(strSearch);

	while( fread(&c,1,1,f)>0 && !found ) {

		if( c==strSearch[pos] ) {
			pos++;
		} else {
		if(pos!=0) {
			// On doit rembobiner !
			fseek(f,-pos,SEEK_CUR);
			pos=0;
			}
		}

		found = (pos==length);

	}
	fclose(f);

	return found;
}

void saveBdd(int result){
  
	time_t timer;
	char tdy[26];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);
	strftime(tdy, 26, "%Y-%m-%d %H:%M:%S", tm_info);
		
	MYSQL mysql;
	mysql_init(&mysql);
	
	char query[70];
	sprintf(query,"INSERT INTO test (`date`,`result`) VALUES('%s','%d')",tdy,result);
	if(mysql_real_connect(&mysql,"137.74.194.232","remoteAcces","paozie","vote",0,NULL,0)){
 		mysql_query(&mysql, query);
        	mysql_close(&mysql);
        	printf("Connection succed ! \n");
   	}
	else	{
		printf("Une erreur s'est produite lors de la connexion à la BDD!\n");
	}
}

void wait(int result){
	int temps = 0;
	if(result){
		temps = 10800 + rand()%120;
	}
	else{
		temps = 600;
	}
	
	while(temps-300>=0){
		printf("temps restant : %d minutes\n",temps/60);
		temps = temps - 300;
		sleep(300);
	}
	printf("temps restant : %d minutes\n",temps/60);
	sleep(temps);

}
int sizeFile(char * fileName){
	int taille = 0;
	FILE * file = fopen(fileName,"r");
	if(file != NULL ){
		while(getc(file)!=EOF){
			taille++;
		}
	}
	return taille;
}
