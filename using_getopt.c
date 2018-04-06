
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>


/* Extern declarations: */

extern  int     optind;
extern  char    *optarg;

int main(int argc, char** argv)
{
	char input;
	char *language = "C";	// default language name
	int size = 100;			// default size (or number of lines)

	/* parse command line options example

	   an appropriate usage message should be included

	   a sample command line is provided below:

					./prog_name -l Java -s 500

	   where l option is used to specify the name of the language used (char array)
	         s option is used to specify the size or the number of lines in the file (int)

	*/


	while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF)
	{
		switch ( input )
		{
			case 'f':
				if(strcmp(optarg, "testFirst1") == 0)
					language  = optarg;

				else if(strcmp(optarg, "testBest1") == 0)
					language  = optarg;

				else if(strcmp(optarg, "testWorst1") == 0)
					language  = optarg;

				else {
					// exit if optarg unknown
					fprintf(stderr, "Usage: -f <filename> -a <algorithm> -m
					<memory size> -q <quantum>\n");
					exit(1);
				}
 				break;
					// should not get here

			case 'a':
				if (strcmp(optarg, "first") == 0)
				else if (strcmp(optarg, "best") == 0)
				else if (strcmp(optarg, "worst") == 0)
				else {
					fprintf(stderr, "Usage: -f <filename> -a <algorithm> -m
					<memory size> -q <quantum>\n");
					exit(1);
				}
				break;

			case 'm':
				// set the value of size (int value) based on optarg
				if (optarg) size = atoi(optarg);
				else {
					fprintf(stderr, "Usage: -f <filename> -a <algorithm> -m
					<memory size> -q <quantum>\n");
					exit(1);
				}
				break;

			case 'q':
				if (optarg) quantum = atoi(optarg);
				else {
					fprintf(stderr, "Usage: -f <filename> -a <algorithm> -m
					<memory size> -q <quantum>\n");
					exit(1);
				}
				break;

			default:
				// do something
				break;
		}
	}

	// ideally, a usage statement should also be included

	// a debugging step is included to display the value of args read
	printf("language = %s size = %d\n", language, size);

	// once the input arguments have been processed, call other functions
	// after appropriate error checking

	return 0;
}
