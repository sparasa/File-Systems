#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int cmpavaildescend(const void * ,const void *);
int cmpavailascend(const void * ,const void * );
void add();
long offset=0;
int count=0;

typedef struct { 
	int key; /* Record's key */ 
	long off; /* Record's offset in file */ } index_S;

index_S indexList[10000];

typedef struct 
{ 
int siz; /* Hole's size */ 
long off; /* Hole's offset in file */ 
}avail_S;

avail_S availList[10000];
int availCount=0;

int main(int argc, char* argv[]){
	char* s= malloc(10*sizeof(char));
	FILE *fp;
	fp=fopen(argv[2],"rb");
	
	if(fp!=NULL){
		//open index file to load previous indexes stored in file
		FILE *out; /* Output file stream */  
		out = fopen( "index.bin", "rb" ); 
		index_S buf;
		while(out!=NULL && fread(&buf,sizeof(index_S),1,out )){
			count++;
			indexList[count-1].key=buf.key;
			indexList[count-1].off=buf.off;		
		}
	
		//open avail file to load previous availability list stored from file.
		out=fopen("available.bin","rb");
		avail_S buff;
		while(out!=NULL && fread(&buff,sizeof(avail_S),1,out )){
			availCount++;
			availList[availCount-1].siz=buff.siz;
			availList[availCount-1].off=buff.off;		
		}
	}

	//loop different commands until the user presses "end"
	while(1)
	{
		scanf("%s",s);

		if(strcmp(s,"add")==0){
			add(argv[2],argv[1]);
		}
		else if(strcmp(s,"end")==0){
			//write indexfile into the disk.
			FILE *out; /* Output file stream */ 
			out = fopen( "index.bin", "wb" ); 
			fwrite( indexList, sizeof( index_S ), count, out );
			fclose( out );
			
			//sort the avail list before storing on disk.
			if(strcmp(argv[1],"--best-fit")==0){
				qsort(availList, availCount, sizeof(avail_S), cmpavailascend);
	}
			else if(strcmp(argv[1],"--worst-fit")==0){
				qsort(availList, availCount, sizeof(avail_S), cmpavaildescend);
	}
			//now push the array onto the disk.
			out = fopen("available.bin","wb");	
			fwrite( availList, sizeof(avail_S), availCount, out);
			fclose(out);
			
			//print index array data onto console
			int iterator;
			printf("Index:\n");
			for(iterator=0;iterator<count;iterator++){
				printf( "key=%d: offset=%ld\n", indexList[iterator].key, indexList[iterator].off );
			}

			//print availability array data onto console	
			int holeCount=0;
			int holeSize=0;
			printf("Availability:\n");
			for(iterator=0;iterator<availCount;iterator++){
				holeCount++;
				holeSize=holeSize+availList[iterator].siz;
				printf( "size=%d: offset=%ld\n", availList[iterator].siz, availList[iterator].off );
			}
			printf( "Number of holes: %d\n", holeCount );
			printf( "Hole space: %d\n", holeSize );
			break;
		}
		else if(strcmp(s,"find")==0){
			find(argv[2]);
		}
		else if(strcmp(s,"del")==0){
			delete(argv[2]);
		}
	}
	
}

//comparison function that sorts the given struct in ascend order of first eleme
//nt in struct
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//comparision function that sorts the given struct in ascending order of a value
// and ascending value of b if multiple a's are equal.
int cmpavailascend(const void * a,const void * b){
	avail_S * p = (avail_S *)a;
	avail_S * q = (avail_S *)b;
	int comparision = p->siz - q->siz;
	if(comparision!=0){
		return comparision;
	}
	else{
		return p->off - q->off;
	}
}

//comparision function that sorts the given struct in descending order of a value and ascending value of b if multiple a's are equal.
int cmpavaildescend(const void * a,const void * b){
	avail_S * p = (avail_S *)a;
	avail_S * q = (avail_S *)b;
	int comparision = q->siz - p->siz;
	if(comparision!=0){
		return comparision;
	}
	else{
		return p->off - q->off;
	}
}

//delete a record given the key of the record.
void delete(char * studentfile)
{
	int key;
	scanf("%d",&key);
	//sort the indexList because binary search requires sorted array. 
	qsort(indexList, count, sizeof(index_S), cmpfunc);
	int keys=binarysearch(key,0,count);
	if(keys!=-1){ //if the key value is found then enter this loop 
		long off=indexList[keys].off;
		FILE *ft;
		int recsize=0;
		//get the avail size and push the required data into avail array
		ft=fopen(studentfile,"r+b");
		fseek(ft,off,SEEK_SET);
		fread(&recsize,sizeof(int),1,ft);
		availList[availCount].siz=recsize+4;
		availList[availCount].off=off;
		availCount++;
		//swap remove the key from index array
		long temp=indexList[count-1].off;
		int temp1=indexList[count-1].key;
		indexList[keys].off=temp;
		indexList[keys].key=temp1;
		indexList[count-1].off=0;
		indexList[count-1].key=0;
		qsort(indexList, count-1, sizeof(index_S), cmpfunc);
		count--;
	}	
	else{
		printf("No record with SID=%d exists\n",key);
	}	
}

//given the key find and return the record if found.
void find(char * studentfile){
	int key;
	scanf("%d",&key);
	//sort the index array to do binary search.
	qsort(indexList, count, sizeof(index_S), cmpfunc);
	int off=binarysearch(key,0,count-1);
	if(off!=-1){
		FILE *fl; 
		int rec_siz;
		fl= fopen(studentfile, "r+b" );
		char *buf=malloc(100*sizeof(char));
		//move to offset position, find the record size and then store
		//the record in buf array.
		fseek( fl, indexList[off].off, SEEK_SET ); 
		fread( &rec_siz, sizeof( int ), 1, fl ); 
		buf = malloc( rec_siz ); 
		fread( buf, rec_siz, 1, fl );
		printf("%s\n",buf);	
	}	
	else{
		printf("No record with SID=%d exists\n",key);
	}
}

//searches a struct based on key and return the index of the array if found.
int binarysearch(int target,int low, int high)
{
	if(low<=high){
		if(indexList[(low+high)/2].key==target){
			return (low+high)/2;
		}
		else if(target<indexList[(low+high)/2].key){
			binarysearch(target,low,((low+high)/2-1));
		}
		else if(target>indexList[(low+high)/2].key){
			binarysearch(target,((low+high)/2+1),high);
		}
	}
	else{
		return -1;
	}	
}


void add(char * studentfile,char * fit){
	int key;
	int flag=0;
	char* record = malloc(1000*sizeof(char));
	int len;
	FILE *fp;
	
	//open the file in read mode if its already present.
	fp=fopen(studentfile,"r+b");
	if(fp==NULL){
		fp=fopen(studentfile,"w+b");
	}
	
	scanf("%d", &key);
	scanf("%s",record);

	//sort the avaiable list according to fit
	if(strcmp(fit,"--best-fit")==0){
		qsort(availList, availCount, sizeof(avail_S), cmpavailascend);
	}
	else if(strcmp(fit,"--worst-fit")==0){
		qsort(availList, availCount, sizeof(avail_S), cmpavaildescend);
	}

	//check whether key is already in indexArray
	qsort(indexList, count, sizeof(index_S), cmpfunc);
	int off=binarysearch(key,0,count-1);
	if(off!=-1){
		printf("Record with SID=%d exists\n",indexList[off].key);
	}	
	else{
		
		//get the length of string
		len=strlen(record);

		//check whether the record has anyhole to fit-in
		int iterator=0;
		for(;iterator<availCount;iterator++){
			if(availList[iterator].siz>=len+4){
				flag=1;
				offset=availList[iterator].off;
				availList[iterator].siz=availList[iterator].siz-len-4;
				availList[iterator].off=availList[iterator].off+len+4;
				iterator++;
				break;
			}
		}

		//update the avialability list here
		if(iterator<availCount){
			//printf("entered into this check\n");
			int tempsize = availList[iterator-1].siz;
			long tempoff = availList[iterator-1].off;
			while(iterator<availCount){
				availList[iterator-1].siz=availList[iterator].siz;
				availList[iterator-1].off=availList[iterator].off;
				iterator++;
			}
			availList[availCount-1].siz=tempsize;
			if(tempsize==0){ 
				availList[availCount-1].off=0;
				availCount--;
			}
			else{
				availList[availCount-1].off=tempoff;
			}
		}
		
		//if hole is not found offset to end of the file.
		if(flag==0){
			fp=fopen(studentfile,"a+b");
			fseek(fp,0,SEEK_END);
			offset=ftell(fp);
		}

		//store the given record in the offset choosen above.
		fseek( fp, offset, SEEK_SET );
		fwrite(&len,sizeof(int),1, fp);
		fseek(fp, offset+4,SEEK_SET);
		fwrite(record, len,sizeof(char),fp);
		indexList[count].key=key;
		indexList[count].off=offset;
		offset=offset+len+4;
		count++;
		fclose(fp);
	}
}
