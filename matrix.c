#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


//global variables
int rows[2];  //number of rows of array1 is in rows[0] and array2 is in rows[1]
int columns[2]; //number of columns of array1 is in columns[0] and array2 is in columns[1]
int **array1;  //matrix 1
int **array2;  //matrix 2
int **productArray1; //product matrix from approach 1
int **productArray2; //product matrix from approach 2
int element=0; //keeping track of which element is calculated in 1st approach
int rowIndicator=0; //keeping track of which row is calculated in 2nd approach

//calculating the value of element in the product array in the 1st approach
void * multF1(void * arg)
{
int e1=element++;  //save index of the element we are calculating
int colN=e1%columns[1];  //calculate colN using the index received
int rowN=e1/columns[1];  //calculate rowN using the index received

productArray1[rowN][colN]=0; //initialize value of the element

//calculating the element value
for(int i=0;i<columns[0];i++)
{
productArray1[rowN][colN]+=array1[rowN][i]*array2[i][colN];
}

}

//calculating the value of each row in the product array in the 2nd approach
void * multF2(void *arg)
{
int r1=rowIndicator++;  //save the row number of the row we are calculating

for(int i=0;i<columns[1];i++) //repeat for each element in the row
{
productArray2[r1][i]=0; //initialize value of the element
//calculating the element value
for(int j=0;j<columns[0];j++)
{
productArray2[r1][i]+=array1[r1][j]*array2[j][i];
}
}
}

int main()
{
//local variables (necessary for reading input)
FILE * filep;
int maximumLength=200*128;
char * string=(char *)malloc(maximumLength);
char * filename=(char*) malloc(128);

printf("Please enter the file name: ");
START: gets(filename);
//opening input file
filep=fopen(filename,"r");
if(filep==NULL) //in case the file was not opened successfully
{printf("Trouble opening file with this name\n");
printf("Please make sure you enter the correct file name: ");
goto START;
}
else //file opened successfully
{printf("File opened successfully\n");

for(int k=0;k<2;k++) //loop for the two matrices
{
//reading first line of file
fgets(string, maximumLength,filep);
//saving the size of the matrix
rows[k]=atoi(strtok(string," "));
columns[k]=atoi(strtok(NULL," "));


//creating the matrix array
if(k==0) //first matrix code
{
//allocating the matrix
array1=malloc(sizeof(int *)*rows[k]);
for(int i=0;i<rows[k];i++)
array1[i]=malloc(sizeof(int)*columns[k]);

//filling the matrix from the text file
for(int i=0;i<rows[k];i++)
{
fgets(string, maximumLength,filep);
array1[i][0]=atoi(strtok(string," "));
for(int j=1; j<columns[k];j++)
{
array1[i][j]=atoi(strtok(NULL," "));
}
}
}

if(k==1) //second matrix code
{
//allocating the matrix
array2=malloc(sizeof(int *)*rows[k]);
for(int i=0;i<rows[k];i++)
array2[i]=malloc(sizeof(int)*columns[k]);

//filling the matrix from the text file
for(int i=0;i<rows[k];i++)
{
fgets(string, maximumLength,filep);
array2[i][0]=atoi(strtok(string," "));
for(int j=1; j<columns[k];j++)
{
array2[i][j]=atoi(strtok(NULL," "));
}
}
}
}
fclose(filep);
//allocating product arrays to start saving calculated values in them
productArray1=malloc(sizeof(int *)*rows[0]);
for(int i=0;i<rows[0];i++)
productArray1[i]=malloc(sizeof(int)*columns[1]);

productArray2=malloc(sizeof(int *)*rows[0]);
for(int i=0;i<rows[0];i++)
productArray2[i]=malloc(sizeof(int)*columns[1]);

//new number of elements saved in a variable
int newSize=rows[0]*columns[1];

//check to make sure the matrices can be multplied
if(columns[0]!=rows[1])
{
//opening output file
filep=fopen("output.txt","w");
if(filep==NULL)
printf("Trouble opening output file\n");
else
{
printf("Matrix can't be multplied\n");
fprintf(filep,"Matrix multplication is not possible\nPlease make sure that the number of columns of the first matrix equals the number of rows of the second matrix");
fclose(filep);
}

}
else
{

//1st approach
clock_t begin=clock();  //start the clock
pthread_t th[newSize];  //create threads as many as the number of elements
for(int i=0;i<newSize;i++)
{
pthread_create(&th[i],NULL, &multF1,NULL);
}
for(int i=0;i<newSize;i++)
{
pthread_join(th[i],NULL);
}
clock_t end=clock(); //end the clock
double time_spent=(double)(end-begin)/CLOCKS_PER_SEC; //calculate elapsed time

//2nd approach
clock_t begin2=clock(); //start the clock for 2nd approach
pthread_t th2[rows[0]]; //create threads as many as the number of rows
for(int i=0;i<rows[0];i++)
{
pthread_create(&th2[i],NULL, &multF2,NULL);
}
for(int i=0;i<rows[0];i++)
{
pthread_join(th2[i],NULL);
}
clock_t end2=clock(); //end the clock for 2nd approach
double time_spent2=(double)(end2-begin2)/CLOCKS_PER_SEC; //calculate elapsed time

//opening output file
filep=fopen("output.txt","w");
if(filep==NULL)
printf("Trouble opening output file\n");
else
{
printf("Output file is ready\n");
fprintf(filep,"1st Approach Product (By element)\n");
fclose(filep);
}
filep=fopen("output.txt","a");
for(int i=0;i<rows[0];i++)
{
for(int j=0;j<columns[1];j++)
fprintf(filep,"%d ",productArray1[i][j]);
fprintf(filep,"\n");
}
fprintf(filep,"time spent: %fs\n",time_spent);
fprintf(filep,"2nd Approach Product (By row)\n");
for(int i=0;i<rows[0];i++)
{
for(int j=0;j<columns[1];j++)
fprintf(filep,"%d ",productArray2[i][j]);
fprintf(filep,"\n");
}
fprintf(filep,"time spent: %fs\n",time_spent2);
fclose(filep);
}
//free all allocated variables
for(int i=0;i<rows[0];i++)
{
free(productArray1[i]);
free(productArray2[i]);
free(array1[i]);
}
for(int i=0;i<rows[1];i++)
free(array2[i]);

free(string);
free(filename);
free(array1);
free(array2);
free(productArray1);
free(productArray2);
return 0;
}
}
