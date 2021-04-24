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
// Signed:_____________Michael Ameteku________________________ Date:______4/23/2021_______

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
			if (dir[i] != 0)
			{

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
		}

		int spaceLeft = 511 * 512 - usedBytes;

		printf("Total Bytes used is: %6d bytes\n", usedBytes);
		printf("Total Bytes left is: %6d bytes\n", spaceLeft);
	}
	else if (argc == 3)
	{
		//this is for printing out a file;
		if (*argv[1] == 'P')
		{
			char *fileName = argv[2];

			int foundName = 1;
			int startingIndex;
			int readable = 0;
			int fileSize = 0;

			for (i = 0; i < 512; i = i + 16)
			{
				if (dir[i] != 0)
				{
					for (j = 0; j < 8; j++)
					{
						if (dir[i + j] == fileName[j])
						{
							printf("Correct: %i %c\n", i + j, fileName[j]);
						}
						else if (dir[i + j] != fileName[j])
						{
							foundName = 0;
							printf("Not at this pos %i \n", i + j);
							break;
						}
						if (fileName[j] == '\0')
							break;
					}

					if (foundName == 1)
					{

						// Check if file is exe
						if ((dir[i + 8] == 't') || (dir[i + 8] == 'T'))
						{
							readable = 1;
							printf(".t\n");
							startingIndex = dir[i + 9];

							printf("gotten starting index: %i\n", startingIndex);
							fileSize = dir[i + 10] * 512;
						}
						else
						{
							printf("File is an exe, not readable\n");
							readable = -1;
						}
						printf("Done with getting file\n");
						break;
					}
					else
						foundName = 1;
				}
			}

			if (readable == 1)
			{
				const int MAX_SIZE = 12288;
				char file[12288];

				//loading the load map fron the starting sector
				fseek(floppy, startingIndex * 512, SEEK_SET);

				for (int a = 0; a < MAX_SIZE; a++)
				{
					//printf("passing characters in");
					file[a] = fgetc(floppy);
				}
				printf("Done passing characters in\n\n\nContent is:\n");

				for (int i = 0; i < MAX_SIZE; i++)
				{
					char letter = file[i];

					if (letter == '\0')
						break;

					printf("%c", letter);
				}
				printf("\n");
			}
		}

		else if (*argv[1] == 'M')
		{

			char *fileName = argv[2];
			char content[512];

			printf("Enter your text!!\n");
			scanf("%s", content);

			printf(" You wrote : %s \n", content);

			int foundName = 1;
			int freeDirectoryEntry;
			int freeMapSpace = -1;

			// checking if file name is already in use
			for (i = 0; i < 512; i = i + 16)
			{
				// finding free space
				if (dir[i] == 0)
				{
					printf("Space is empty\n");
					freeDirectoryEntry = i;
					break;
				}

				for (j = 0; j < 8; j++)
				{
					if (fileName[j] == '\0')
						break;

					foundName = 1;

					if (dir[i + j] == fileName[j])
					{
						printf("Character match: %i %c\n", i + j, fileName[j]);
					}
					else if (dir[i + j] != fileName[j])
					{
						foundName = 0;
						printf("Not a match %i \n", i + j);
						break;
					}
				}

				if (foundName == 1)
				{
					printf("file name is in use\n");
					break;
				}
			}

			if (foundName != 1)
			{

				printf("free space found at: %d\n", freeDirectoryEntry);

				//pushing the file directory name
				fseek(floppy, freeDirectoryEntry + 257 * 512, SEEK_SET);

				for (int a = 0; a < 8; a++)
				{
					if (fileName[a] == '\0')
						break;

					fputc(fileName[a], floppy);
				}

				// pushing file type on disk
				fseek(floppy, freeDirectoryEntry + 8 + 257 * 512, SEEK_SET);

				fputc('t', floppy);

				//finding a free space on map
				for (int i = 0; i < 512; i++)
				{
					if (map[i] == 0)
					{
						freeMapSpace = i;
						fseek(floppy, 256 * 512 + i, SEEK_SET);
						fputc(255, floppy);
						printf("Found free map space at sector: %d", freeMapSpace);
						break;
					}
				}

				if (freeMapSpace == -1)
				{
					printf("No more free space on map");
				}
				else
				{

					// updating starting position of file
					fseek(floppy, freeDirectoryEntry + 9 + 257 * 512, SEEK_SET);
					fputc(freeMapSpace, floppy);
					fputc(1, floppy);

					//writing content to file
					fseek(floppy, 512 * freeMapSpace, SEEK_SET);
					for (i = 0; i < 512; i++)
					{
						if (content[i] == '\0')
						{
							for (; i < 512; i++)
							{
								fputc(0, floppy);
							}
						}
						else
						{
							fputc(content[i], floppy);
						}
					}

					printf("Done\n");
				}
			}
		}

		else if (*argv[1] == 'D')
		{

			char *fileName = argv[2];

			int foundName = 1;
			int startingIndex;
			int directoryEntryIndex;
			int numberOfSectors;
			int fileSize = 0;

			for (i = 0; i < 512; i = i + 16)
			{
				if (dir[i] == 0)
				{
					printf("File is empty or file not found\n");
					break;
				}

				for (j = 0; j < 8; j++)
				{
					if (dir[i + j] == fileName[j])
					{
						printf("Correct: %i %c\n", i + j, fileName[j]);
					}
					else if (dir[i + j] != fileName[j])
					{
						foundName = 0;
						printf("Not at this pos %i \n", i + j);
						break;
					}
					if (fileName[j] == '\0')
						break;
				}

				if (foundName == 1)
				{
					directoryEntryIndex = i;

					//set first entry of directory to zero
					dir[directoryEntryIndex] = 0;

					//find starting sector set and number of sectors used.
					startingIndex = dir[directoryEntryIndex + 9];
					numberOfSectors = dir[directoryEntryIndex + 10];
					//set positions based on sector number to zero in maps
					for (i = 0; i < numberOfSectors; i++)
					{
						map[startingIndex + i] = 0;
					}

					//write directory and map to disk

					fseek(floppy, 256 * 512, SEEK_SET);

					for (i = 0; i < 512; i++)
					{
						fputc(map[i], floppy);
					}

					fseek(floppy, 257 * 512, SEEK_SET);

					for (i = 0; i < 512; i++)
					{
						fputc(dir[i], floppy);
					}

					//done
					printf("Done with deleting file\n");
					break;
				}
				else
					foundName = 1;
			}
		}
	}

	fclose(floppy);
}
