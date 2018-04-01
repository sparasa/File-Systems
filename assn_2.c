#include<stdio.h>
#include<stdlib.h>
void add();
int offset=0;
int count=0;
typedef struct { 
	int key; /* Record's key */ 
	long off; /* Record's offset in file */ } index_S;

int main(int argc, char* argv[]){
	index_S *indexList= malloc(1000*sizeof(index_S));
	char* s= malloc(10*sizeof(char));
	FILE *fp;
	fp = fopen( "student.db", "r+b" );
	int offset = lseek(fp, 0, SEEK_END);
	printf("offset: %d\n",offset);
	//add the record to the file.
	FILE *out; /* Output file stream */  
	out = fopen( "index.bin", "rb" ); 
	if(out!=NULL){
		printf("have data inside index file");
		fread( indexList, sizeof( index_S ), 1000, out ); 
		fclose(out);
	}
	int iterator;
	for(iterator=0;iterator<2;iterator++){
		printf("key: %d   ",indexList[iterator].key);
		printf("key: %d\n",indexList[iterator].off);
		}
	while(1)
	{
		gets(s);
		if(strcmp(s,"add")==0){
			printf("entered here\n");
			add(indexList);
		}
		else if(strcmp(s,"end")==0){
			FILE *out; /* Output file stream */ 
			out = fopen( "index.bin", "wb" ); 
			fwrite( indexList, sizeof( index_S ), 50, out );
			fclose( out );
			int iterator;
			for(iterator=0;iterator<2;iterator++){
				printf("key: %d   ",indexList[iterator].key);
				printf("key: %d\n",indexList[iterator].off);
			}
			break;
		}
	}
	
}

void add(index_S* indexList){
	printf("entered into add\n");
	int key;
	char* record = malloc(1000*sizeof(char));
	int iterator=0;
	FILE *fp;
	printf("input the record now\n");
	scanf("%d", &key);
	gets(record);	
	//get the length of string
	while(record[iterator]!='\0'){
		iterator++;
	}
	printf("%d",iterator);
	//iterator is length of record.
	fp = fopen( "student.db", "w+b" );
	fseek( fp, offset, SEEK_SET );
	fwrite(&iterator,sizeof(int),1, fp);
	int p=fwrite(record, sizeof(char),iterator,fp);
	indexList[count].key=key;
	indexList[count].off=offset;
	offset=offset+p+4;
	count++;
	//index must be choosed be choosed from availability list or append at the end.
}
