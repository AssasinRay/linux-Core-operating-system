#include "file_sys.h"

#define BLOCKSIZE 4096  //4KB per block
#define MAX_FILE_SIZE 1023*4  //1023 per inode, 4kb per data block
#define MAX_BUFFER_SIZE 40000
#define MAX_WRITE_BUFFER 8192
#define MAX_PROCESS 6

static boot_block_t* boot_block_addr;
static uint32_t multiboot_addr;
static uint32_t inode_number;
static uint32_t data_block_number;
static uint32_t inode_start_addr;
static uint32_t data_block_start_addr;

static uint8_t available_inodes[MAX_FILE];
static uint8_t available_datablock[MAX_DATA_BLOCK];
static void init_block_info();
static int32_t write_helper(int32_t fd, const void* buf, int32_t nbytes);
static char filename_buffer[MAX_PROCESS][MAX_FILE_NAME_LENGTH];


/**
	Function to initialize the file system
	@param pointer for the location of the file system
	@return  none
*/
void init_file_sys(uint32_t input)
{
	int i;
	boot_block_addr = (boot_block_t*) input;
	multiboot_addr = input;
	inode_number = boot_block_addr->inodes_N;
	data_block_number = boot_block_addr->data_blocks_D;
	inode_start_addr = input+BLOCKSIZE;
	data_block_start_addr =  inode_start_addr + inode_number*BLOCKSIZE;
	//printf(" file system initialized, inode number: %d, data block number: %d!\n", boot_block_addr->inodes_N,boot_block_addr->data_blocks_D);
	
	 // clean_up_hard_disk(101);
	if(sync_with_hard_disk() == 1)
	{
		// copy_data_to_hard_disk((char*) (input),0);
		// copy_data_from_hard_disk((char*) (input),0);

		for(i=0;i<inode_number+data_block_number+1;i++)
		{
			copy_data_to_hard_disk((char*) (input+i*BLOCKSIZE),i);
		}
		for(i=0;i<inode_number+data_block_number+1;i++)
		{
			copy_data_from_hard_disk((char*) (input+i*BLOCKSIZE),i);
		}
	}
	else
	{
		
		for(i=0;i<inode_number+data_block_number+1;i++)
		{
			copy_data_from_hard_disk((char*) (input+i*BLOCKSIZE),i);
		}
	}

	init_block_info();//EXTRA CREDIT
	


}


/**
	Function to read a file
	@param file descriptor
	@param buffer for reading
	@param bytes to read
	@return 0 if success, -1 if fails
*/

int32_t read_file (int32_t fd, void* buf, int32_t nbytes)
{
	int32_t bytes_read;
	fd_t* curr_fd = &(current_process->fd_array[fd]);


	if(curr_fd->flags.is_directory!=1)  //if not directory
		 {
		 	bytes_read = read_data(curr_fd->inode, curr_fd->file_pos, buf, nbytes);
		 	curr_fd->file_pos += bytes_read;
		 	return bytes_read;
		 }
	else //if to read is directory
	{
		if(strlen((int8_t*)(boot_block_addr->dir_entries[curr_fd->file_pos].filename))!=0)
		{
			// printf("the pid is %d\n",current_process->pid );
			// printf("the fd is %d file position is %d\n",fd,curr_fd->file_pos );
			curr_fd->file_pos ++;
			strncpy(buf,(void*)(boot_block_addr->dir_entries[curr_fd->file_pos].filename),
				strlen((char*)boot_block_addr->dir_entries[curr_fd->file_pos].filename));
			return strlen((char*)boot_block_addr->dir_entries[curr_fd->file_pos].filename);
		}
		else 
			return 0;
		
	}


}

/**
	Function to write a file
	@param file descriptor
	@param buffer for reading
	@param bytes to WRITE  ---MUST BE NO GREATER THAN 4096
	@return This function should alwasy fail because this system is read only
*/
int32_t write_file (int32_t fd, const void* buf, int32_t nbytes)
{
	//for extra credit
	if(nbytes>BLOCKSIZE)
	return -1;
	return write_helper(fd,buf,nbytes);


	// return -1;
}

/**
	Function to open a file
	@param file name
	@return  0 if success, -1 if fails
*/
int32_t open_file (const uint8_t* filename)
{
	dentry_t mydir;
	return read_dentry_by_name (filename, &mydir);
}
/**
	Function to close a file
	@param file name
	@return 0 if success, -1 if fails
*/
int32_t close_file (int32_t fd)
{
	return 0;
}
/**
	Temporary function to test print out the directory
*/
int32_t test_read_directory(void)
{
	//printf("\n testing reading directory...\n");
	int i;
	for(i=0;i<63;i++)
	{	
		if(strlen((int8_t*)boot_block_addr->dir_entries[i].filename))
		{
			printf(" filename %d is %s inode is %d\n",i,boot_block_addr->dir_entries[i].filename,boot_block_addr->dir_entries[i].inode_number );
		}
	}
	return 0;
}

/**
	Temporary function to test read a file
*/

int32_t test_read_file(char* filename,uint32_t offset,uint32_t length,int32_t index)
{
	////printf("\n the file name is %s idx %d\n",filename,index );
	int i;
	dentry_t directory;
	uint32_t inode;
	uint8_t data[MAX_BUFFER_SIZE];
	int32_t return_value;
	

	if(index == -1)
	{
		if(read_dentry_by_name((uint8_t*)filename,&directory) == -1)
		{
			printf("\n Fail to find the file %s\n\n",filename);
			return -1;
		}
	}
	else
	{
		if(read_dentry_by_index(index,&directory) == -1)
		{
			printf("\n Fail to find a file %d\n\n",index);
			return -1;
		}
	}

	inode = directory.inode_number;
	if((return_value = read_data(inode,offset,data,length)) <0 )
	{
		printf("\n read data failed!!!!!\n\n");
		return -1;
	}
	printf(" the data read is %d\n",return_value);
	//data[return_value] = '\0';
	////printf("%s\n",data);
	for(i=0;i<return_value;i++)
		printf("%c",data[i] );
	//printf("\n");
	//printf("print finished\n");
	return 0;
	

}

/**
	Function to read directory entry by name
	@param file name
	@param pass back a dentry
	@return 0 if success, -1 if fails
*/

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	int i;
	if(current_process!=NULL)
	{
		for(i=0;i<MAX_FILE_NAME_LENGTH;i++)
		filename_buffer[current_process->pid][i] = ' ';
		strncpy(filename_buffer[current_process->pid], (int8_t*) fname,MAX_FILE_NAME_LENGTH);//the max file name is 32
		
	}

	
	if (fname == NULL)	//if fname is empty
	{
		dentry = NULL;
		return -1;
	}
	else
	{
		for(i = 0; i < MAX_FILE_SIZE; i++)  //traverse the file_directory_array  MAX FILE IS 63
		{
			int filename_length = strlen((int8_t*)((boot_block_addr->dir_entries[i]).filename)); 
			int input_length = strlen((int8_t*)fname);
			//printf(" i%d filename is %s\n",i,boot_block_addr->dir_entries[i].filename );
			if(filename_length == input_length && strncmp((int8_t*)fname,(int8_t*)(boot_block_addr->dir_entries[i]).filename,filename_length) == 0 && filename_length>0)	//if filename is found exactly the same
			{
				
				dentry->file_type = (boot_block_addr->dir_entries[i]).file_type;
				dentry->inode_number = (boot_block_addr->dir_entries[i]).inode_number;  //copy the data the pointer passed in 
				return 0;
			}
		}
		
		//for extra credit

		dentry = NULL;//if does not find 
		return -1;

		

	}
	return -1;

}

	
/**
	Function to read directory entry by index
	@param index number
	@param dentry is the directory entry pass back
	@return 0 if success, -1 if fails
*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	if (index >= MAX_FILE)	//requested number is greater or equal to 63, this functon should fail
	{
		dentry = NULL;
		return -1;
	}
	else
	{
		//dentry_t* found_dentry = &(boot_block_addr->dir_entries[index]);
		//printf(" filename is %s\n",boot_block_addr->dir_entries[index].filename );
		//strncpy((int8_t*)(boot_block_addr->dir_entries[index].filename), (int8_t*)(dentry->filename),32);//the max file name is 32
		dentry->file_type = (boot_block_addr->dir_entries[index]).file_type;
		dentry->inode_number = (boot_block_addr->dir_entries[index]).inode_number;
		return 0;
	}

}

/**
	Function to read data
	@param inode number
	@param memory offset
	@param buffer to pass back the data
	@param the length expect to read
	@return 0 if success, -1 if fails
*/

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	
	uint32_t byte_to_read;
	uint32_t byte_already_read;
	uint32_t file_length_in_byte;
	uint32_t cur_idx;
	uint32_t cur_data_block;
	uint32_t position_in_data_block;


	if(inode >= inode_number) //if the inode is greater than the inode given in the super block, this call should fail
		{
			//printf(" ERROR inode number is too big!\n");
			return -1;
		}
	inode_t* node_to_find;
	node_to_find = (inode_t*) (inode_start_addr+inode*BLOCKSIZE); //find the inode addr for the given inode idx
	file_length_in_byte = node_to_find->inode_length;
	//printf(" the file size is %d",file_length_in_byte);
	if(offset >= file_length_in_byte) //if invalid offset
	 	{
	 		//printf(" NOTHING TO READ\n");
	 		return 0;
	 	}
	
	byte_to_read = length;
	if(node_to_find->inode_length <= offset+length)
	 	byte_to_read = node_to_find->inode_length - offset;

	 cur_idx = offset/BLOCKSIZE;  //get the data block index according to the offset info
	 //printf("the cur index is %d\n",cur_idx);
	 if(offset<BLOCKSIZE)
	 	cur_idx = 0;

	 byte_already_read = 0; 	//already read 0 byte

	 cur_data_block = node_to_find->inode_data[cur_idx];  //the data block corresponding to the cur idx
	 position_in_data_block = offset%BLOCKSIZE;			 //the location of data in the data block
	 ////printf("offset %d curidx %d\n",offset,cur_idx );
	////printf("the data block no %d, the offset in data block is %d\n",cur_data_block, position_in_data_block);
	////printf("the byte to read is %d\n",byte_to_read);
	 while(byte_to_read>byte_already_read)		//there are more bytes to read
	 {
	 	//for debug
	 	if(byte_already_read >= file_length_in_byte) 	//this should not exceed file length
	 	{
	 		printf("reading byte exceed the file size\n");
	 		return -2;
	 	}

	 	if(cur_data_block >= data_block_number)  //should not read data block out of range
	 		return -1;

	 	memcpy(buf+byte_already_read,
	 		(uint8_t*) (data_block_start_addr+cur_data_block*BLOCKSIZE+position_in_data_block),sizeof(int8_t));	
	 	//copy the data from tmp buffer to the buffer

	 	position_in_data_block++;		//increase the offset in the data block
	 	byte_already_read++;			//increase the byte already read

	 	if(position_in_data_block == BLOCKSIZE)		//already finished reading one data block, move to the next
	 	{
	 		cur_idx++;					//go to the next data block
	 		cur_data_block = node_to_find->inode_data[cur_idx];		//find the data block according to the index
	 		position_in_data_block = 0;		//should read file starting from offset 0
	 		
	 		if(cur_idx > 1023)		//for debug: if exceed the max data block number, something happen
	 			{
	 				//FOR DEBUG
	 				printf("accessing invalid inode data number\n");
	 				return -2;
	 			}
	 	}

	 }
	 return byte_already_read;	//return the bytes read in total
}

/**
	Function FILE LOADER
	Fill the file into the physical memory
	@return  none
*/

int32_t file_loader(const uint8_t* filename, uint8_t* buf, uint32_t nbytes)
{
	dentry_t  mydir; //MAY NEED SOME MODIFIES FOR WRITING (EXTRA CREDIT)
	if(read_dentry_by_name(filename,&mydir) !=0)                     return -1;
	if(read_data(mydir.inode_number,0,buf,nbytes) < 0)              return -1;

	return 0;
}

/**
	Function for extra credit
	To find out the available inode blocks and data blocks
	@return  none
*/
static void init_block_info()
{
	int i,j;
	
	for(i =0; i < MAX_DATA_BLOCK; i++)
		available_datablock[i] = 0;

	//traverse the dir and mark 
	for(i = 0; i<MAX_FILE;i++)
	{
		if(strlen((int8_t*)boot_block_addr->dir_entries[i].filename)) //if the file name is not 0
			{
				uint32_t inode = boot_block_addr->dir_entries[i].inode_number;
				// printf("the inode is %d\n",inode );
				uint32_t file_length_in_byte;
				uint32_t data_block_count;
				available_inodes[inode] = 1;

				inode_t* node_to_find;
				node_to_find = (inode_t*) (inode_start_addr+inode*BLOCKSIZE); //find the inode addr for the given inode idx
				file_length_in_byte = node_to_find->inode_length;
				data_block_count = file_length_in_byte/BLOCKSIZE+1;

				for(j=0;j<data_block_count;j++)
				{
					uint32_t data_block;
					data_block = node_to_find->inode_data[j];
					available_datablock[data_block] = 1;
				}

			}
		else
			available_inodes[i] = 0;
	}

}

/**
	HELPER FUNCTION FOR EXTRA CREDIT
	@param file descriptor
	@param buffer for reading
	@param bytes to read
	@return BYTES OF DATA WRITETEN SUCCESFULLY OR -1 IF FAILED 
*/

static int32_t write_helper(int32_t fd, const void* buf, int32_t nbytes)
{

	uint32_t file_length_in_byte;
	uint32_t data_block_count;
	int i;
	for(i = 0; i < MAX_FILE_SIZE; i++)  //traverse the file_directory_array  MAX FILE IS 63
		{
			int filename_length = strlen((int8_t*)((boot_block_addr->dir_entries[i]).filename)); 
			int input_length = strlen((int8_t*)filename_buffer[current_process->pid]);
			//printf(" i%d filename is %s\n",i,boot_block_addr->dir_entries[i].filename );
			if(filename_length == input_length && strncmp((int8_t*)filename_buffer[current_process->pid],(int8_t*)(boot_block_addr->dir_entries[i]).filename,filename_length) == 0 && filename_length>0)	//if filename is found exactly the same
			{
				
				return -1;
			}
		}


	fd_t* curr_fd = &(current_process->fd_array[fd]);
	//cannot write a directory...
	if(curr_fd->flags.is_directory==1)   return -1;//if not directory

	//check if the first , find a free inode block
	if(curr_fd->inode > MAX_FILE)
 	{
 		for(i=0;i<MAX_FILE;i++)
 			if(available_inodes[i] == 0)
 			{
 				available_inodes[i] = 1; //mark it as occpied
 				curr_fd->inode = i; 
 				 // printf("       inode found is %d\n",i );
 				break;
 			}
 	}

 		// IF does not find, create a new file
	for(i=0;i<MAX_FILE;i++)//first find a new empty dentry
		{
			int filename_length = strlen((int8_t*)((boot_block_addr->dir_entries[i]).filename)); 
			if(filename_length == 0)//an empty dentry found!
			{
				(boot_block_addr->dir_entries[i]).inode_number = curr_fd->inode; //initialize with invalid number
				strncpy((int8_t*)(boot_block_addr->dir_entries[i].filename), filename_buffer[current_process->pid],32);//the max file name is 32
				(boot_block_addr->dir_entries[i]).file_type = DENTRY_FILE;
				copy_data_to_hard_disk((char*)multiboot_addr,0);
				
				 // printf("       entry found is %d\n",i );
				break;
			}

		}

 	if(curr_fd->file_pos % BLOCKSIZE == 0) //SHOULD ALLOCATE NEW DATA BLOCK
 	{
 		// printf("                       here\n");
 		for(i = 0; i < MAX_DATA_BLOCK;i++)
 		{
 			if(available_datablock[i] == 0)
 			{
 				available_datablock[i] = 1;
 				inode_t* node_to_find;
				node_to_find = (inode_t*) (inode_start_addr+curr_fd->inode*BLOCKSIZE); //find the inode addr for the given inode idx
				node_to_find->inode_length += nbytes;
				file_length_in_byte = node_to_find->inode_length;
				data_block_count = file_length_in_byte/BLOCKSIZE;
				node_to_find->inode_data[data_block_count] = i; //the data block is this!
				memcpy( (uint8_t*) (data_block_start_addr+i*BLOCKSIZE),buf,nbytes);	//copy the whole thing!
				// printf("datablock found is %d \n",curr_fd->inode );
 		// printf("the inode is %d\n", curr_fd->inode);
				copy_data_to_hard_disk((char*) (multiboot_addr+(1+inode_number+i)*BLOCKSIZE),1+inode_number+i);
				curr_fd->file_pos+=nbytes;
				 // printf("       data block found is %d\n",i );
				break;

 			}
 		}
 	}
 	copy_data_to_hard_disk((char*) (multiboot_addr+(1+curr_fd->inode)*BLOCKSIZE),1+curr_fd->inode);
// test_read_directory();
 	return nbytes;
		
}

