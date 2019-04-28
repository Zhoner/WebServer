#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

int main(int argc, char *argv[])
{
	if (argc!=4) {
		printf("USAGE:%s REGEX STRING #MATCHES\n", argv[0]);
		return (-1);
	}

	int rc;
	char errore[100];

	char* regex = argv[1];
	char* string = argv[2];
	int n_matches = atoi(argv[3]);

	regex_t reg;

	rc = regcomp(&reg, regex, REG_EXTENDED);
	if (rc) {
		regerror(rc, &reg, errore, 100);
		printf("Error in regcomp(): %s\n", errore);
		return (-2);
	}

	regmatch_t matches[n_matches];
	char* strings[n_matches];

	printf("regex:%s\n", regex);
	printf("string:%s\n", string);

	rc = regexec(&reg, string, n_matches, matches, 0);
	if (rc) {
		regerror(rc, &reg, errore, 100);
		printf("Error in regexec(): %s\n", errore);
		return (-2);
	}

	printf("   inizio\tfine\n");
	for (int i=0; i<n_matches; i++) {

		int inizio = matches[i].rm_so;
		int fine = matches[i].rm_eo;

		strings[i] = (char*) malloc (sizeof(char)* (fine - inizio + 1));
		strncpy(strings[i], string + inizio,  fine - inizio);
		strings[i][fine-inizio]='\0';

		printf("%d) %d\t\t%d\t\t\"%s\"\t%p\n", i, inizio, fine, strings[i], strings[i]);
		fflush(stdout);
	}

	regfree(&reg);
	
	for (int i=0; i<n_matches; i++) {
		printf("%d) %p\n", i, strings[i]);
		fflush(stdout);
		free(strings[i]);
	}
	
	return 0;
}