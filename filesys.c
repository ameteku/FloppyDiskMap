//ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:_____________________________________ Date:_____________

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>

int main(int argc, char *argv[])
{
	int i, j, size, noSecs, startPos;

	//open the floppy image
	FILE *floppy;
	floppy = fopen("floppya.img", "r+");
	if (floppy == 0)
	{
		printf("floppya.img not found\n");
		return 0;
	}

	//load the disk map from sector 256
	char map[512];
	fseek(floppy, 512 * 256, SEEK_SET);
	for (i = 0; i < 512; i++)
		map[i] = fgetc(floppy);

	//load the directory from sector 257
	char dir[512];
	fseek(floppy, 512 * 257, SEEK_SET);
	for (i = 0; i < 512; i++)
		dir[i] = fgetc(floppy);

	if (argc < 2)
	{
		printf("Not right command\n");
		return 0;
	}

	// listing all files on disk
	if (argc == 2 && *argv[1] == 'L')
	{
		// print directory
		printf("\nDisk directory:\n");
		printf("Name   Length\n");
		int usedBytes = 0;
		for (i = 0; i < 512; i = i + 16)
		{
			if (dir[i] == 0)
				break;
			for (j = 0; j < 8; j++)
			{
				if (dir[i + j] != 0)
					printf("%c", dir[i + j]);
			}
			if ((dir[i + 8] == 't') || (dir[i + 8] == 'T'))
				printf(".t");
			else
				printf(".x");
			int currentBytes = 512 * dir[i + 10];
			usedBytes += currentBytes;
			printf("%6d bytes\n", currentBytes);
		}

		int spaceLeft = 511 * 512 - usedBytes;

		printf("Total Bytes used is: %6d bytes\n", usedBytes);
		printf("Total Bytes left is: %6d bytes\n", spaceLeft);
	}
	else if (argc == 3)
	{
		if (*argv[1] == 'P')
		{
			char *fileName = argv[2];

			int foundName = 1;
			int startingIndex;
			int readable = 0;

			for (i = 0; i < 512; i = i + 16)
			{
				if (dir[i] == 0)
				{
					printf("File is empty\n");
					break;
				}

				for (j = 0; j < 4; j++)
				{
					if (dir[i + j] == fileName[j])
					{
						printf("Correct: %i \n", i);
					}
					else if (dir[i + j] != fileName[j])
					{
						foundName = 0;
						printf("Not at this pos %i \n", i);
						break;
					}
				}

				if (foundName == 1)
				{
					startingIndex = i;
					// Check if file is exe
					if ((dir[i + 8] == 't') || (dir[i + 8] == 'T')) {
						readable= 1;
						printf(".t\n");
					}
						
					else
						printf("File is an exe, not readable\n");

					break;
				}
				else
					foundName = 1;
			}

			printf("Name found at : %i", startingIndex);
			if(readable ==1) {
				char file[12288] ;
				for()
			}
		}
	}

	// //print disk map
	// printf("Disk usage map:\n");
	// printf("      0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
	// printf("     --------------------------------\n");
	// for (i=0; i<16; i++) {
	// 	switch(i) {
	// 		case 15: printf("0xF_ "); break;
	// 		case 14: printf("0xE_ "); break;
	// 		case 13: printf("0xD_ "); break;
	// 		case 12: printf("0xC_ "); break;
	// 		case 11: printf("0xB_ "); break;
	// 		case 10: printf("0xA_ "); break;
	// 		default: printf("0x%d_ ", i); break;
	// 	}
	// 	for (j=0; j<16; j++) {
	// 		if (map[16*i+j]==-1) printf(" X"); else printf(" .");
	// 	}
	// 	printf("\n");
	// }

	/*
	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
*/
	fclose(floppy);
}
