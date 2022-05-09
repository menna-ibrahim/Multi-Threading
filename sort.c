#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


//global variables
int sizeArray;
int *array;

//struct definition that will contain the start and the end of the range sent to the merge sort
struct Range{
int start;
int finish;
};

//merging of two arrays
void merge(int low,int mid, int high)
{
//define number of elements in the two auxiliary arrays
int n1=mid-low+1;
int n2=high-mid;

//declare two auxiliary arrays
int leftArray[n1];
int rightArray[n2];

//copy elements from the original array to the corresponding elements in the auxiliary arrays
for(int i=0;i<n1;i++)
leftArray[i]=array[low+i];

for(int i=0;i<n2;i++)
rightArray[i]=array[mid+i+1];

//put the elements in order from the smallest to the largest from the two auxiliary arrays back to the original array and continue till one auxiliary array is empty
int l=0,r=0,k=low;
while(l<n1 && r<n2)
{
if(leftArray[l]>rightArray[r])
array[k++]=rightArray[r++];
else
array[k++]=leftArray[l++];
}
 //copy the remaining elements from the non-empty auxiliary array in the same order back to the original array
while(r<n2)
array[k++]=rightArray[r++];
while(l<n1)
array[k++]=leftArray[l++];
}

void * mergeSort(void * arg)
{
//convert void pointer received in arg to struct of type Range defined earlier
struct Range * range=arg;
//save the struct values in local variables
int low= range->start;
int high=range->finish;
int mid;
if(low<high)
{
//calculate mid element index
mid=(low+high)/2;
//create two other structs of type Range
struct Range range1, range2;
//set the first range from the start (the start we have started from earlier in the call of the function) to the mid element (we calculated it in the last step)
range1.start=low;
range1.finish=mid;
//set the second range from the element after the mid element to the end (the end we have stopped at earlier in the call of the function)
range2.start=mid+1;
range2.finish=high;

//create two threads one for the first range and one for the second range
pthread_t th[2];
pthread_create(&th[0],NULL,&mergeSort,(void*)&range1);
pthread_create(&th[1],NULL,&mergeSort,(void*)&range2);

pthread_join(th[0],NULL);
pthread_join(th[1],NULL);

//merge both subarrays
merge(low,mid,high);

}
}



void printArray()
{
printf("size of array: %d\n",sizeArray);
for(int i=0;i<sizeArray;i++)
printf("%d ",array[i]);
printf("\n");
}

int main()
{
FILE * filep;
int maximumLength=200*128;
char * string=(char *)malloc(maximumLength);
char * filename=(char*) malloc(128);

printf("Please enter the file name: ");
START: gets(filename);
//opening input file
filep=fopen(filename,"r");
if(filep==NULL)
{printf("Trouble opening file with this name\n");
printf("Please make sure you enter the correct file name: ");
goto START;
}
else
{
printf("File opened successfully\n");

//reading size of array and allocating the array
fgets(string, maximumLength,filep);
sizeArray=atoi(string);
array=malloc(sizeof(int)*sizeArray);

//reading the array elements
fgets(string, maximumLength,filep);
array[0]=atoi(strtok(string," "));
for(int i=1;i<sizeArray;i++)
array[i]=atoi(strtok(NULL," "));

fclose(filep);

printf("Original Array:\n");
printArray();
//sending the initial range which is from 0 to size-1
struct Range initialRange;
initialRange.start=0;
initialRange.finish=sizeArray-1;
int begin=clock(); //start clock
mergeSort((void*)&initialRange);
int end=clock(); //stop clock
double time_spent=(double)(end-begin)/CLOCKS_PER_SEC; //calculate elapsed time
printf("Sorted Array:\n");
printArray();

//opening output file
filep=fopen("outputSort.txt","w");
if(filep==NULL)
printf("Trouble opening file with this name\n");
else
{
printf("Output file is ready\n");
fprintf(filep,"Sorted Array:\n");
fclose(filep);
filep=fopen("outputSort.txt","a");
for(int i=0;i<sizeArray;i++)
fprintf(filep,"%d ",array[i]);
fprintf(filep,"\ntime spent: %fs",time_spent);
}

fclose(filep);

//freeing allocated pointers
free(string);
free(filename);
free(array);

return 0;
}
}
