//===========================================================================
// PROJECT NAME :- COSTMISIED VIRTUAL FILE SYSTEM

// PROJECT DESCRIPTION :- IT IS RESEARCHED BASED PROJECT IN WHICH TRIED TO
// IMPLEMENT VIRTUAL FILE SYSTEM BY STUDYING FILE SYSTEM OF OPERATING SYSTEM.

//INPUT :- COMMANDS 

//OUTPUT :- ACCORIDNG TO INPUT COMMAND PERFORMED OPERATION

// AUTOR :- VAIBHAV A JAISINGPURE

// DATE :- 17/6/2021 

//============================================================================
// #DEFINE AND ALL INCLUDES

#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINODE 50 //MAximum number of inode

#define READ 1	//For read using 1
#define WRITE 2	//for write using 2

#define MAXFILESIZE 1024	//definining maximum file size of 1024 bytes

#define REGULAR 1 	//FOR REGULAR USING 1
#define SPECIAL 2 	//FOR SPECIAL FILE USING 2

#define START 0 //FOR START USED 0
#define CURRENT 1 //FOR CURRENT USED 1
#define END 2 	//FOR END USED 2

// ================================================================================
//------------------------------------------------------------
// DECLARAIGN STRUCTURE FOR FILE SYSTEMS
// --------------------------------------------------------

//---------------------------------------------------------
// STRUCTURE FOR SUPERBLOCK IN FILE SYSTEM
typedef struct superblock
{
	int TotalInodes;
	int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;
// ------------------------------------------------------------
// STRUCTION FOR INODE OF LINKEDLIST FOR STORING DATA OF FILE

typedef struct inode
{
	char FileName[50];//WILL STORE NAME OF FILE
	int InodeNumber;//WILL STORE NUMBER OF INODE
	int FileSize;//SIZE ALLOCATED BY OS
	int FileActualSize;//ACTUAL SIZE OF FILE
	int FileType;//WILL STORE FILETYPE
	char *Buffer;//WILL TAKE DATA OF 1024 IN ONE BY ONE
	int LinkCount;//STORES NUMBER OF LINKAGE
	int ReferenceCount;//STORE HOW MANY REFERENCE OF FILES ARE
	int permission;//WILL STORE THE PERMISSION OF FILES 
	struct inode *next;//POINTER TO STORE THE ADDRESS OF NEXT INODE
}INODE,*PINODE,*PPINODE;
//------------------------------------------------------------------
//STRUCTURE FOR FILETABLE TP STORE PONTER OF INODE AND ITS INFO

typedef struct filetable
{
	int readoffset;// WILL STORE THE OFFSET OFREAD
	int writeoffset;//WILL STORE THE OFFSET OF WRITE
	int count;//WILL STORE THE NUMBER OF WORDS IN INPUT
	int mode;//WILL STORE THE IN WHICH MODE FLE SHOULD BE OPENDE
	PINODE ptrinode;//WILL STORE POINTER TO INODE
}FILETABLE,*PFILETABLE;
//-------------------------------------------------------------------
//STRUCTURE FOR UFDT ARRAY IN UAREA
typedef struct ufdt
{
	PFILETABLE ptrfiletable;//FOR STORING POINTER OF FILETABLE 
	//if want to add memebers in future need to add in here
}UFDT;
//======================================================================

UFDT UFDTArr[MAXINODE];//FOR STORING THE POINTER TO FILE TABLE

SUPERBLOCK SUPERBLOCKobj;//CREATING OBJECT OF SUPERBLOCK STRUCTURE
PINODE head = NULL;//GLOBAL MEMEBER 

//=========================================================================

//MANUAL PAGE FUNCTION
void man(char *name)
{
	if(name == NULL)return;
	if(stricmp(name,"create")==0)
	{
		printf("Description : Used to create new regular file\n");
		printf("Usage : create File_name Permission \n");
	}
	else if(stricmp(name,"read")==0)
	{
		printf("Description : Used to read data from regular fle\n");
		printf("Usage : read File_name No_of_bytes_to_read\n");
	}
	else if(stricmp(name,"write")==0)
	{
		printf("Description : Used to write into regular file\n");
		printf("Usage : write File_name\n After this enter the data you want to write in file\n");
	}
	else if(stricmp(name,"ls")==0)
	{
		printf("Description : Used to list all info of files\n");
		printf("Usage : ls\n");
	}
	else if(stricmp(name,"stst")==0)
	{
		printf("Description : Used to display info of file\n");
		printf("Usage : stat File_name\n");
	}
	else if(stricmp(name,"fstat")==0)
	{
		printf("Description : Used to display info of file\n");
		printf("Usage : stat File_Descriptor\n");
	}
	else if(stricmp(name,"turncate")==0)
	{
		printf("Description : Used to remove data from file\n");
		printf("Usage : turncate File_name\n");
	}
	else if(stricmp(name,"open")==0)
	{
		printf("Description :Used to open existing file\n");
		printf("Usage : open Fine_name mode\n");
	}
	else if(stricmp(name,"close")==0)
	{
		printf("Description : Used to close opned file\n");
		printf("Usage :open File_name \n");
	}
	else if(stricmp(name,"closeall")==0)
	{
		printf("Description : Used to close all opened files\n");
		printf("Usage :closeall\n");
	}
	else if(stricmp(name,"lseek")==0)
	{
		printf("Description : Used to change file offset\n");
		printf("Usage :lseek File_name ChangeInOffset StartPoint\n");
	}
	else if(stricmp(name,"rm")==0)
	{
		printf("Description : Used to delete the file\n");
		printf("Usage :rm File_name\n");
	}
	else
	{
		printf("ERROR : No manual entry available\n");
	}
}
//=========================================================================

//FUNCTION TO DISPLAY HELP 

void DisplayHelp() 
{ 
	printf("ls : To List out all files\n"); 
	printf("clear : To clear console\n");
	printf("open : To open the file\n");
	printf("close : To close the file\n");
	printf("closeall : To close all opened files\n");
	printf("read : To Read the contents from file\n");
	printf("write :To Write contents into file\n");
	printf("exit : To Terminate file system\n");
	printf("stat : To Display information of file using name\n");
	printf("fstat :To Display information of file using file descriptor\n");
	printf("truncate : To Remove all data from file\n");
	printf("rm : To Delet the file\n");
}
//---------------------------------------------------------------------------------------------------
//FUNCTION FOR FILE DISCRIPTORE
 
 int GetFDFromName(char *name)
 {
 	int i = 0;

 	while(i<50)
 	{
 		if(UFDTArr[i].ptrfiletable != NULL)
 		{
 			if(stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
 			{
 				break;
 			}
 		}
 		i++;
 	}
 	if(i==50)
 	{
 		return -1;
 	}
 	else
 	{
 		return i;
 	}
 }
 //---------------------------------------------------------------------------------------------------
 //FUNCTION FOR CHECK WEATHER THAT FILE IS ALREADY EXIST OR NOT

 // WILL RETURN NULL OR NON NULL VALUES

 PINODE Get_Inode(char *name)
 {
 	PINODE temp = head;
 	int i = 0;

 	if(name==NULL)
 	{
 		return NULL;
 	}
 	while(temp != NULL)
 	{
 		if(strcmp(name,temp->FileName)==0)
 		{
 			break;
 		}
 		temp = temp->next;
 	}
 	return temp;
 }
//---------------------------------------------------------------------------------------------------
//FUNCTION FOR CREATING DISK INODE LIST TABLE

void CreateDILB()
{
	int i = 1;
	PINODE newn = NULL;
	PINODE temp = head;

	while(i<=MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));

		newn->LinkCount = newn->ReferenceCount = 0;
		newn->FileType = newn->FileSize = 0;
		newn->Buffer = NULL;
		newn->next = NULL;
		newn->InodeNumber = i;

		if(temp == NULL)
		{
			head = newn;
			temp = head;
		}
		else
		{
			temp->next = newn;
			temp = temp->next;
		}
		i++;
	}
	printf("DILB created successfully\n");
}
//---------------------------------------------------------------------------------------------------
//FUNCTION TO INITIALISE SUPERBLOCK

void InitialiseSuperBlock()
{
	int i = 0;

	while(i<MAXINODE)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInodes = MAXINODE;
	SUPERBLOCKobj.FreeInodes = MAXINODE;
}
//---------------------------------------------------------------------------------------------------
//FUNCTION FOR CREATING NEW REGLAR FILE

int CreateFile(char *name,int permission)
{
	int i = 0;
	PINODE temp =  head;

	if((name == NULL) || (permission == 0) || (permission>3))
	{
		return -1;
	}

	if(SUPERBLOCKobj.FreeInodes == 0)
	{
		return -2;
	}

	if(Get_Inode(name) != NULL)
	{
		return -3;
	}

	(SUPERBLOCKobj.FreeInodes)--;// DECREASING THE  NO OF FREE INODES

	while(temp != NULL)
	{
		if(temp->FileType == 0)
			break;
		temp = temp->next;
	}

	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		{
			break;
		}
		i++;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable == NULL)
	{
		return -4;
	}

	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = permission;
	UFDTArr[i].ptrfiletable->readoffset = 0;
	UFDTArr[i].ptrfiletable->writeoffset = 0;

	UFDTArr[i].ptrfiletable->ptrinode = temp;

	strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);

	UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
	UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);

	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,MAXFILESIZE);
	//used to fill a block of memory with a particular value
	return i;
}
//---------------------------------------------------------------------------------------------------
// FUNCTION FOR REMOVING FILE

int rm_file(char* name)
{
	int fd = 0;

	fd = GetFDFromName(name);

	if(fd == -1)
		return -1;

	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
		free(UFDTArr[fd].ptrfiletable);
	}

	UFDTArr[fd].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInodes)++;
}
//---------------------------------------------------------------------------------------------
//FUNCTION FOR READING OF FILE

int ReadFile(int fd,char *arr,int isize)
{
	int read_size = 0; 

	if(UFDTArr[fd].ptrfiletable == NULL)
		return -1;

	if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
	{
		return -2;
	}

	if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ+WRITE)
	{
		return -2;
	}
	if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
	{
		return -3;
	}
	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
	{
		return -4;
	}

	read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);

	if(read_size < isize)
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);
		UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
	}
	else
	{
		strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),isize);
		(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + isize;
	}
	return isize;
}
//--------------------------------------------------------------------------------------------------------------------------
//FUNCTION FOR WRITING FILE 

int WriteFile(int fd,char *arr,int isize)
{
	if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) && (UFDTArr[fd].ptrfiletable->mode != READ+WRITE))
	{
		return -1;
	}
	if(((UFDTArr[fd].ptrfiletable->ptrinode->permission) !=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ+WRITE)) 
	{
		return -1;
	}
	if((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFILESIZE)
	{
		return -2;
	}
	 if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
	 {
	 	return -3;
	 }

	strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

	(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset )+ isize; 

	(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

	return isize;
}
//---------------------------------------------------------------------------------------------------------------------------
//FUNCTION USED TO OPEN FILE
int OpenFile(char *name,int mode)
{
	int i = 0;
	PINODE temp = NULL;

	if(name == NULL || mode <= 0)
		return -1;

	temp = Get_Inode(name);

	if(temp == NULL)
		return -2;

	if(temp->permission < mode)
		return -3;

	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
			break;

		i++;
	}

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable == NULL)
		return -1;

	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = mode;

	if(mode == READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	}
	else if(mode == READ)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
	}
	else if(mode == WRITE)
	{
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	}

	UFDTArr[i].ptrfiletable->ptrinode = temp;

	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

	return i;
}
//----------------------------------------------------------------------------------------------------------
//WILL CLOSE BY IT NAME GIVEN AS INPUT

// void CloseFileByName(int fd)
// {
// 	UFDTArr[fd].ptrfiletable->readoffset = 0;
// 	UFDTArr[fd].ptrfiletable->writeoffset = 0;

// 	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
// }
//----------------------------------------------------------------------------------------------------

int CloseFileByName(char *name)
{
	int i = 0;
	i = GetFDFromName(name);

	if(i == -1)
	{
		return -1;
	}

	UFDTArr[i].ptrfiletable->readoffset = 0;
	UFDTArr[i].ptrfiletable->writeoffset = 0;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

	return 0;
}
//-----------------------------------------------------------------------------------------------
//FUNCTION FOR CLOSING ALL FILES

void CloseAllFile()
{
	int i = 0;
	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable != NULL)
		{
			UFDTArr[i].ptrfiletable->readoffset = 0;
			UFDTArr[i].ptrfiletable->writeoffset = 0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
			break;
		}
		i++;
	}
}
//-------------------------------------------------------------------------------------------------------------------
//FUNCTION FOR LSEEK OF FILE

int LseekFile(int fd,int size,int from)
{
	if((fd<=0)||(from>2))
		return -1;

	if(UFDTArr[fd].ptrfiletable == NULL)
		return -1;

	if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
	{
		if(from == CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
			{
				return -1;
			}
			if(((UFDTArr[fd].ptrfiletable->readoffset) + size)<0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;

		}
		else if(from == START)
		{
			if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
				return -1;

			if(size < 0)
				return -1;

			(UFDTArr[fd].ptrfiletable->readoffset)==size;
		}
		else if(from == END)
		{
			if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
				return -1;

			if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
				return -1;

			(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
		}
	}

}
//----------------------------------------------------------------------------------------------------------------------------
//lsiting of all files in dictionary

void ls_file()
{
	int i = 0;
	PINODE temp = head;

	if(SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		printf("ERRRO : There are no files\n");
		return;
	}

	printf("\nFile Name\tInode Number\tFile size\tLink Count\n");
	printf("------------------------------------------------------\n");

	while(temp != NULL)
	{
		if(temp->FileType != 0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount );
		}
		temp = temp->next;
	}
	printf("------------------------------------------------------\n");

}
//---------------------------------------------------------------------------------------------

int fstat_file(int fd)
{
	PINODE temp = head;
	int i = 0;

	if(fd < 0)
	{
		return -1;
	}
	if(UFDTArr[fd].ptrfiletable == NULL)
	{
		return -2;
	}

	temp = UFDTArr[fd].ptrfiletable->ptrinode;

	printf("\n-------Statistical Information about file---------\n");
	printf("File Name : %s\n",temp->FileName );
	printf("Inode Number %d\n",temp->InodeNumber);
	printf("File size %d\n",temp->FileSize );
	printf("Actual fie size %d\n",temp->ReferenceCount);
	printf("Link count : %d\n",temp->LinkCount);
	printf("Reference count : %d\n",temp->ReferenceCount); 

	if(temp->permission == 1)
		printf("File Permission : Read only\n");

	else if(temp->permission == 2)
		printf("File Permission : Write\n");

	else if(temp->permission == 3)
		printf("File Permission : Read & Write\n");

	printf("------------------------------------------------------\n\n");

	return 0;
}

int stat_file(char *name)
{
	PINODE temp = head;
	int i = 0;

	if(name==NULL)
	{
		return -1;
	}
	while(temp!= NULL)
	{
		if(strcmp(name,temp->FileName) == 0)
			break;
		temp = temp->next;
	}

	if(temp == NULL)
		return -2;

	printf("\n---------Statistical Information about file----------\n");
	printf("File name : %s\n",temp->FileName);
	printf("Inode Number %d\n",temp->InodeNumber);
	printf("File size : %d\n",temp->FileSize);
  	printf("Actual File size : %d\n",temp->FileActualSize);
	printf("Link count : %d\n",temp->LinkCount);
	printf("Reference count : %d\n",temp->ReferenceCount);

	if(temp->permission == 1)
		printf("File Permission : Read only\n"); 

	else if(temp->permission == 2)
		printf("File Permission : Write\n");
	else if(temp->permission == 3)
		printf("File Permission : Read & Write\n");

	printf("------------------------------------------------------\n\n");

	return 0;
}
//------------------------------------------------------------------------------------------
//THIS FUNCTION USED TO CLEAN THE DATA RETURN IN THE FILE

int truncate_file(char* name)
{
	int fd = GetFDFromName(name);
	if(fd == -1)
	{
		return -1;
	}
	memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);

	UFDTArr[fd].ptrfiletable->readoffset = 0;
	UFDTArr[fd].ptrfiletable->writeoffset = 0;
	UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
}

int main()
{
	char *ptr = NULL;
	int ret = 0,fd = 0,count = 1;
	char command[4][80],str[80],arr[1024];

	InitialiseSuperBlock();

	CreateDILB();

	while(count !=  0)
	{
		fflush(stdin);
		strcpy_s(str,"");

		printf("\nVirtual file System :>");

		fgets(str,80,stdin);

		count  = sscanf(str," %s %s %s %s",command[0],command[1],command[2],command[3]);


		switch(count)
		{
			case 1:
				if(stricmp(command[0],"ls")==0)
				{
					ls_file();
				}
				else if(stricmp(command[0],"closeall")==0)
				{
					CloseAllFile();
					printf("All files are closed successfully\n");
					continue;
				}
				else if(stricmp(command[0],"clear")==0)
				{
					system("cls");
					continue;
				}
				else if(stricmp(command[0],"help")==0)
				{
					DisplayHelp();
					continue;
				}
				else if(stricmp(command[0],"exit")==0)
				{
					printf("Terminating the Virtual File System\n");
					break;
				}
				else
				{
					printf("\nError : Command Not Found !! \n");
					continue;
				}
			
			case 2:
				if(stricmp(command[0],"stat")==0)
				{
					ret = stat_file(command[1]);
					if(ret == -1)
					{
						printf("Error : Incorrect parameter\n");
					}
					if(ret == -2)
					{
						printf("Error : There is no such file\n");
					}
					continue;
				}
				else if(stricmp(command[0],"fstat")==0)
				{
					ret = fstat_file(atoi(command[1]));
					if(ret == -1)
					{
						printf("Error : Incorrect parameter\n");
					}
					if(ret == -2)
					{
						printf("Error : There is no such file\n");
					}
					continue;
				}
				else if(stricmp(command[0],"close")==0)
				{
					ret = CloseFileByName(command[1]);
					if(ret == -1)
					{
						printf("Error :There is no such file\n");
					}
					continue;
				}
				else if(stricmp(command[0],"rm")==0)
				{
					ret = rm_file(command[1]);
					if(ret == -1)
					{
						printf("Error : There is no such file\n");
					}
					continue;

				}
				else if(stricmp(command[0],"man")==0)
				{
					man(command[1]);
				}
				else if(stricmp(command[0],"write")==0)
				{
					fd = GetFDFromName(command[1]);
					if(fd==-1)
					{
						printf("Error : Incorrect parameter\n");
						continue;
					}
					printf("Enter the data : \n");
					scanf("%[^\n]",arr);

					ret = strlen(arr);
					if(ret == 0)
					{
						printf("Error : Incorrect parameter\n");
						continue;
					}
					ret = WriteFile(fd,arr,ret);
					if(ret==-1)
					{
						printf("Error : Incorrect parameter\n");
						continue;
					}
					if(ret==-2)
					{
						printf("Error : There is no sufficient memory to write\n");
					}
					if(ret==-3)
					{
						printf("Error : It is not regular file\n");

					}

				}
				else if(stricmp(command[0],"truncate")==0)
				{
					ret = truncate_file(command[1]);
					if(ret == -1)
					{
						printf("Error : Incorrect parameter\n");
					}
				}
				else
				{
					printf("\n ERROR : Command Not Foundf \n");
					continue;
				}
			
			case 3:
				if(stricmp(command[0],"create")==0)
				{
					ret = CreateFile(command[1],atoi(command[2]));
					if(ret >= 0)
					{
						printf("File is successfully created with file descriptor : %d \n",ret );
					}
					if(ret==-1)
					{
						printf("Error : Incorrect parameter\n");
					}
					if(ret==-2)
					{
						printf("Error : There is no inode\n");
					}
					if(ret == -3)
					{
						printf("Error : File already exists\n");
					}
					if(ret == -4)
					{
						printf("Error : Memoey allocation failure\n");
					}
					continue;
				}
				else if(stricmp(command[0],"read")==0)
				{
					fd = GetFDFromName(command[1]);
					if(fd == -1)
					{
						printf("Error : Incorrect parameter\n");
						continue;
					}
					ptr = (char*)malloc(sizeof(atoi(command[2]))+1);
					if(ptr == NULL)
					{
						printf("Error : memory Allocation failure\n");
					}
					ret = ReadFile(fd,ptr,atoi(command[2]));
					if(ret==-1)
					{
						printf("Error : File not existing\n");
					}
					if(ret == -2)
					{
						printf("Error : Permission Denied\n");
					}
					if(ret==-3)
						printf("Error : Reacher the end of file\n");
					if(ret==-4)
						printf("Error : It is not regular file\n");
					if(ret==0)
						printf("Error : File empty\n");
					if(ret > 0)
					{
						WriteFile(2,ptr,ret);
					}
					continue;
				}
				else if(stricmp(command[0],"open")==0)
				{
					ret = OpenFile(command[1],atoi(command[2]));
					if(ret >= 0)
						printf("File is successfully opened with file descriptor : %d \n",ret );
					if(ret==-1)
						printf("Error : Incorrect parameter\n");
					if(ret==-2)
						printf("Error : File not present\n");
					if(ret==-3)
						printf("Error : Permission Denied\n");
					continue;
				}
				else
				{    
					printf("\nERROR : Command not found !!!\n");   
				}
			case 4:
				if(stricmp(command[0],"lseek")==0)
				{
					fd = GetFDFromName(command[1]);
					if(fd==-1)
					{
						printf("Error : Incorrect parameter\n");
						continue;
					}
					ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
					if(ret==-1)
					{
						printf("Error : Unable to perform lseek\n");
					}
				}
				else
				{
					printf("\nError : Command not Found\n");
					continue;
				}
			// default:
			// {
			// 	printf("\nERROR : Command not found\n");
			// 	continue;
			// }
		}

	}
	return 0;
}