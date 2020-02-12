#include <stdio.h>
#include <string.h>
#include <time.h>


//CONSTANTS
#define NAMESIZE 50
#define BOOKTITLE 100
#define BOOKSTORAGE 500
#define MAXSTUDENTS 200
#define SERIAL 6
#define FRONT 0

enum menuOpt {READ=1, SELL, DELETE_STUDENT, PRINT_REPORT, MAIN_EXIT};//for main menu
enum reportOpt {PAYMENT=1, SUBJ_COUNT, SUBJ_BOOK_COUNT, EXIT};//for report menu
enum check {YES, NO};//for new student check and book availability


//STRUCTURES
typedef struct{
	int ownerID; //last 4 digits
	char author[NAMESIZE];// first and last name
	int price;
	char subject[NAMESIZE];
	char title[BOOKTITLE];
	char serialNum[SERIAL];// unique identifier of book
	char date_received[26];
} bookData;

typedef struct{
	int idNum; //last 4 digits
	char name[NAMESIZE];// first and last name	
	int total_owed;// whole figures
} studentData;

typedef struct{
	studentData studList[MAXSTUDENTS];
	int count;
} student;

typedef struct{
	bookData bookList[BOOKSTORAGE];
	int count;
} book;


//FUNCTION PROTOTYPES
int displayMenu();

//FILE FUNCTIONS
int retrieveBooks(bookData[]);
int retrieveStudents(studentData[]);
void saveStudents(studentData[], int);
void saveBooks(bookData[], int);

//READ FUNCTIONS
int readBook(bookData[], int, int);
int addStudent(studentData[], int, int*);

//REMOVE FUNCTIONS
void deleteBook(book*, char[], student*);
void deleteStudent(student*, book*);

//BOOK SALE FUNCTIONS
void calcTotal(student*, bookData);
void displayBooks(char[], int *, int *, book*);
void makeSale(book*, student*);

//REPORT FUNCTIONS
void reportMain(book*, student*);
int displayReportMenu();
void sortBooks(bookData[], int);
void printTotalOwed(student*);
void displaySubjCount(bookData[], int);
void displaySubjBooks(bookData[], int);

//AUXILIARY FUNCTIONS
int readSubject(bookData[], int, bookData[], int, char[]);
void loadBooks(bookData[], book*);
int checkStudent(studentData[], int, int);
int findStudent(student*, int);

//QUEUE FUNCTIONS
int enqueue(bookData[], int, bookData);
bookData dequeue(bookData[], int*);
int isEmpty(int);
int isFull(int, int);


//MAIN FUNCTION
int main()
{
	student studRecord;
	book bookRecord;
	int choice, studID;
	
	//populating book and student record
	bookRecord.count=retrieveBooks(bookRecord.bookList);
	studRecord.count=retrieveStudents(studRecord.studList);

	choice=displayMenu();
	
	while(choice!=MAIN_EXIT){
		if(choice==SELL)
			makeSale(&bookRecord, &studRecord);
				
		else{
			if(choice==READ){				
				studRecord.count=addStudent(studRecord.studList, studRecord.count, &studID);

				//add new book to student record			
				bookRecord.count=readBook(bookRecord.bookList, bookRecord.count, studID);				
			}// end if
				
			else{
				if(choice==DELETE_STUDENT)
					deleteStudent(&studRecord, &bookRecord);				
				
				else{
					if(choice==PRINT_REPORT)
						reportMain(&bookRecord, &studRecord);													
				}// end else
			}//end else
		}//end else
		
		choice=displayMenu();
	}// end main while				
	
	saveBooks(bookRecord.bookList, bookRecord.count); 
	saveStudents(studRecord.studList, studRecord.count);
	return 0;
}//end main


/*name: displayMenu
return value: integer representing user's choice
parameter(s): none
purpose: presents a list of choices to the user*/

int displayMenu()
{
	int choice;

	printf("%i New book donation\n", READ);
	printf("%i Sell book\n", SELL);
	printf("%i Remove student\n", DELETE_STUDENT);
	printf("%i Generate report\n", PRINT_REPORT);
	printf("%i Exit\n\n", MAIN_EXIT);

	printf ("Enter choice\t");
 	scanf ("%i", &choice);
     
  	//validates choice entered
  	while(choice<READ || choice>MAIN_EXIT){
   		printf("Not valid. Re-enter choice\t");
      	scanf("%i", &choice);
   }//end while
     
   printf ("\n\n");
   return choice;
}// end displayMenu


/*name: retrieveBooks
return value: the number of current available books
parameter(s): an empty list of books
purpose: update book count and populates the book list from file*/

int retrieveBooks(bookData bookList[])
{
	int amt=0, readRes;
	FILE *bookFile;
	
	bookFile=fopen("Book_List.txt", "r");
	
	if (bookFile==NULL)
		printf("Book file not available\n\n");
		
	else{		
		//loads book information from file to an array
		readRes=fscanf(bookFile,"%[^\n]", bookList[amt].title); 
				
		while(readRes>0){
			fscanf(bookFile," %s", bookList[amt].serialNum);
			fscanf(bookFile," %[^\n]", bookList[amt].author);
			fscanf(bookFile," %[^\n]", bookList[amt].subject);
			fscanf(bookFile,"%i", &bookList[amt].price);
			fscanf(bookFile,"%i", &bookList[amt].ownerID);
			fscanf(bookFile," %[^\n]", bookList[amt].date_received);
						
			++amt;
			readRes=fscanf(bookFile," %[^\n]", bookList[amt].title);
		}// end while
		
		fclose(bookFile);
	}// end else
	
	return amt;
}// end retrieveBooks


/*name: retrieveStudents
return value: the number of students
parameter(s): an empty list of students
purpose: update student count and populates the student list from file*/

int retrieveStudents(studentData studList[])
{
	int amt=0, readRes;
	FILE *studFile;
	
	studFile=fopen("Student_List.txt", "r");
	
	if (studFile==NULL)
		printf("Student file not available\n\n");
	else{
		
		//loads student information from file to an array
		readRes=fscanf(studFile,"%i", &studList[amt].idNum);
		
		while(readRes>0){
			fscanf(studFile," %[^\n]", studList[amt].name);			
			fscanf(studFile," %i", &studList[amt].total_owed);
									
			++amt;
			readRes=fscanf(studFile," %i", &studList[amt].idNum);
		}// end while
		
		fclose(studFile);
	}// end else
	
	return amt;
}// end retrieveStudents


/*name: checkStudent
return value: integer value representing whether or not the student is new or returning
parameter(s): list of students, the number of students, the ID number of a student
purpose: checks if student is new*/

int checkStudent(studentData studList[], int sCount, int ID)
{
	int newStudent=YES, i;
	
	for(i=0; i<sCount; i++){
		
		//if the student exists in the current student list
		if(ID==studList[i].idNum){
			newStudent=NO;
			break;
		}// end if
	}// end for
	
	return newStudent;
}// end new student check

/*name: addStudent
return value: the number of students
parameter(s): list of students, the number of students, placeholder for studID (value to be entered in module)
purpose: information about new student is read*/

int addStudent(studentData studList[], int sCount, int* studID)
{	
	printf("Enter last 4 digits of student id\t");
	scanf("%i", studID);
	
	//validation for positive 4 digits
	while(*studID<0 || *studID>9999){
   		printf("Not valid. Re-enter student ID\t");
      	scanf("%i", studID);
	}//end while

	//checking existence of student				
	int studCheck=checkStudent(studList, sCount, *studID);
					
	if(studCheck==YES){
		//the student is new
		studList[sCount].idNum = *studID;
	
		fflush(stdin);
		printf("\nEnter name of student\t");
		scanf("%[^\n]", studList[sCount].name);	
		
		studList[sCount].total_owed=0; //total owed is initialised
		
		++sCount;
	}//end if
	
	printf("\n\n");
	return sCount;
}// end add new student


/*name: readBook
return value: the number of books
parameter(s): list of books, number of books, ID of owner of new book
purpose: reads information about new book*/

int readBook (bookData bookList[], int bCount, int ownerID)
{		
	//the new book information is added to the book list
	bookList[bCount].ownerID = ownerID;
	
	//assign date received
	time_t curr_time;
	time(&curr_time);
   	strcpy(bookList[bCount].date_received, ctime(&curr_time));	  
	
	fflush(stdin);
	printf("Enter serial number of book\t");
	scanf("%s", bookList[bCount].serialNum);
	
	fflush(stdin);
	printf("Enter book title\t");
	scanf("%[^\n]", bookList[bCount].title);
		
	fflush(stdin);
	printf("Enter first and last name of (first) author\t");
	scanf("%[^\n]", bookList[bCount].author);
	
	fflush(stdin);
	printf("Enter selling price\t");
	scanf("%i", &bookList[bCount].price);
	
	//validates price entered
  	while(bookList[bCount].price<=0){
   		printf("Not valid. Re-enter price\t");
      	scanf("%i", &bookList[bCount].price);      	
   }//end while
	
	fflush(stdin);
	printf("Enter subject\t");
	scanf("%[^\n]", bookList[bCount].subject);
	
	++bCount;
		
	printf ("\n\n\n");
	return bCount;
}// end readBook


/*name: saveBooks
return value: none
parameter(s): list of books, number of books
purpose: rewrites the list of current books to book file*/

void saveBooks(bookData bookList[], int bCount)
{
	int i;
	FILE *bookFile;
	
	bookFile=fopen("Book_List.txt", "w");
	
	if(bookFile==NULL)
		printf("Book file not opened\n");
		
	else{	
		//write all book information to a file	
		for(i=0; i<bCount; i++){
			fprintf(bookFile,"%s\n", bookList[i].title);
			fprintf(bookFile,"%s\n", bookList[i].serialNum);
			fprintf(bookFile,"%s\n", bookList[i].author);
			fprintf(bookFile,"%s\n", bookList[i].subject);
			fprintf(bookFile,"%i\n", bookList[i].price);
			fprintf(bookFile,"%i\n", bookList[i].ownerID);
			fprintf(bookFile,"%s\n\n", bookList[i].date_received);
		}// end for
		
		fclose(bookFile);
		printf("Books saved to file\n");
	}// end else
}// end saveBooks


/*name: saveStudents
return value: none
parameter(s): list of students, number of students
purpose: rewrites the list of current students to student file*/

void saveStudents(studentData studList[], int sCount)
{
	FILE *studFile;
	int i;
	
	studFile=fopen("Student_List.txt", "w");
	
	if(studFile==NULL)
		printf("Student file not opened\n");
		
	else{	
		//write all student information to a file	
		for(i=0; i<sCount; i++){
			fprintf(studFile,"%i\n", studList[i].idNum);
			fprintf(studFile,"%s\n", studList[i].name);		
			fprintf(studFile,"%i\n\n", studList[i].total_owed);
		}//end for		
				
		fclose(studFile);
		printf("Students saved to file\n");
	}// end else
}// end saveStudents


/*name: makeSale
return value: none
parameter(s): list of books, number of books, list of students, number of students
purpose: available books are identified and the book and student lists are updated if book is sold*/

void makeSale(book* bookRecord, student* studRecord)
{
	char bookID[SERIAL], curr_choice[4], final_choice[4];
	int isAvailable, isSold, compVal;
		
	do{	
		displayBooks(bookID, &isAvailable, &isSold, bookRecord); 
		
		//if the desired book is available but a copy was not selected
		if(isAvailable==YES && isSold==NO){
			printf("Do you want any of these books?\t");
			scanf("%s", curr_choice);
			
			compVal=strcmpi(curr_choice, "yes");
			
			//if choice is "yes"
			if(compVal==0){
				isSold=YES;
				printf("Enter the ID of book chosen\t");
				scanf("%s", bookID);				
			}//end if		
		}// end if
	
		//if the desired book is available and a copy was chosen
		if (isAvailable==YES && isSold==YES){
			deleteBook(bookRecord, bookID, studRecord);
			printf("Book sold!\n\n");	
		}// end if	
		
		printf("Do you want any more books?\t");
		scanf("%s", final_choice);
			
		compVal=strcmpi(final_choice, "yes");							
	}while(compVal==0); //while the user wants more books		
				
	printf("\n\n\n");	
}// end sale of book


/*name: displayBooks
return value: none
parameter(s): list of books, number of books, the availability status of the book passed by reference,
the sale status of book passed by reference, placeholder for bookID(value to be entered in module)
purpose: available subject books are displayed*/

void displayBooks(char bookID[], int *isAvailable, int *isSold, book* bookRecord)
{ 
	int compVal, i, subjCount;
	char choice[4], bookTitle[BOOKTITLE], subject[NAMESIZE];
	bookData subjBks[bookRecord->count];	
	bookData bookItem; //stores book from queue
	
	*isAvailable=NO;
	*isSold=NO;

	fflush(stdin);
	printf("Enter the subject\t");
	scanf("%[^\n]", subject);	
			
	subjCount=readSubject(subjBks, subjCount, bookRecord->bookList, bookRecord->count, subject);
		
	if(subjCount==0)//if there are no books in that subject category
		printf("\nNo book in this subject category\n\n");
	
	else{
		fflush(stdin);
		printf("\nWhat is the name of book?\t");
		scanf("%[^\n]", bookTitle);

		while(isEmpty(subjCount)==0){
			bookItem=dequeue(subjBks, &subjCount);
			
			compVal=strcmpi(bookItem.title, bookTitle);
			
			//if book matches the book specified
			if(compVal==0){
				*isAvailable=YES;
				
				//displays the details about the matching book
				printf("\nTitle: %s\n", subjBks[i].title);
				printf("Book ID: %s\n", subjBks[i].serialNum);
				printf("Author: %s\n", subjBks[i].author);
				printf("Price: %i\n\n", subjBks[i].price);
								
				printf("Do you want this book?\t");
				scanf("%s", choice);
					
				compVal=strcmpi(choice, "yes");
				
				//if choice is "yes"
				if(compVal==0){
					*isSold=YES;
					strcpy(bookID, subjBks[i].serialNum);
					break;// stops showing books					
				}//end if
			}// end if
			
		}//end while		
		
		if(compVal!=0)//if the user never chose a book
			printf("No other %s found\n\n", bookTitle);		
	}//end else 
}// end displayBooks


/*name: readSubject
return value: number of books relevant to a particular subject
parameter(s): empty list of subject books, number of subject books, list of books, number of books, the subject of wanted book
purpose: populates subject book list with relevant books*/

int readSubject(bookData subjBks[], int subjCount, bookData bookList[], int bCount, char subject[])
{
	int i=0, compVal;
	subjCount=0;
	
	while(isFull(subjCount, bCount)==0 && i<bCount){
		compVal=strcmpi(bookList[i].subject, subject);
		
		//if the subject of the book matches the specified subject
		if(compVal==0)
			//adds book to subject list
			subjCount=enqueue(subjBks, subjCount, bookList[i]);
			
		++i;		
	}//end while	
	
	return subjCount;	
} // end readSubject
	

/*name: deleteBook
return value: none
parameter(s): list of students and books, number of books and students, ID of book sold
purpose: deletes book sold from array of current books*/

void deleteBook(book* bookRecord, char bookID[], student* studRecord)
{
	int compID, bookSold, i;	
		
	for(i=0; i<bookRecord->count; i++){
		compID=strcmpi(bookID, bookRecord->bookList[i].serialNum);
		
		//if the serial number of the sold book matches a book in current list
		if(compID==0){
			bookSold=i;//the postion in the book list aaray			
			break;
		}// end if
	}// end for
	
	--bookRecord->count;
	
	calcTotal(studRecord, bookRecord->bookList[bookSold]);
	
	//book sold is deleted from the book list
	for(i=bookSold; i<bookRecord->count; i++)
		bookRecord->bookList[i]=bookRecord->bookList[i+1];	
	
}// end delete book


/*name: calcTotal
return value: none
parameter(s): list of students, number of students, the information about the book sold
purpose: price of the book sold is added to the student owner's total amount owed*/

void calcTotal(student *studRecord, bookData bookSold){
	int i;
	
	for(i=0; i<studRecord->count; i++){		 
		if(bookSold.ownerID==studRecord->studList[i].idNum){ //owner of sold book is found
		
			//the total owed to student is increased
			studRecord->studList[i].total_owed+=bookSold.price; 
			break;
		}// end if
	}// end for
}// end calculate total owed


/*name: printTotalOwed
return value: none
parameter(s): list of students, number of students
purpose: list of students who are to be paid are placed in file*/

void printTotalOwed(student* studRecord)
{
	FILE *studFile;
	int i;
	char filename[NAMESIZE];
	
	time_t curr_time; //stores raw calendar time
	struct tm *date_info;
	char file_date[12];

	//determining the time of file
	time(&curr_time);
	date_info = localtime(&curr_time);
	
	strftime(file_date, 12, "%b%d,%Y", date_info);//date in format mmdd,yyyy 
   	sprintf(filename, "%s%s%s", "Student Payment List_", file_date, ".txt");//customised filename
	   	
	studFile=fopen(filename, "w");
	
	if(studFile==NULL)
		printf("File not opened\n\n\n");
		
	else{		
		//report headings
			fprintf(studFile, "STUDENT ID\t");
			fprintf(studFile, "NAME OF STUDENT\t");
			fprintf(studFile, "TOTAL OWED\n\n");
				
		for(i=0; i<studRecord->count; i++){
			
			//the students who are owed money are written to a file
			if(studRecord->studList[i].total_owed<=0)
				continue;			
			
			fprintf(studFile, "%i\t", studRecord->studList[i].idNum);
			fprintf(studFile, "%s\t", studRecord->studList[i].name);		
			fprintf(studFile, "Total owed: $%i\n", studRecord->studList[i].total_owed);

			studRecord->studList[i].total_owed=0; //resets total owed to zero 
		}// end for	
		
		fclose(studFile);
		printf("Payment information stored to file\n\n\n");
	}// end else	
}//end print money owed


/*name: deleteStudent
return value: none
parameter(s): list of students, number of students, list of books, number of books
purpose: deletes student from array of current students and updates the book record*/

void deleteStudent (student* studRecord, book *bookRecord)
{
	char bookID[SERIAL];
	int i;	
	int bookFound=NO;
	int studID;
		
	printf("Enter last four digits of student ID\t");
	scanf("%i", &studID);

	//validation
	while(studID<0 || studID>9999){
   		printf("Not valid. Re-enter student ID\t");
      	scanf("%i", &studID);
	}//end while

	//find student in array
	int studPosition=findStudent(studRecord, studID);
	
	if(studPosition!=-1){ //student exists	
		printf("Total owed to %s: $%i\n\n", studRecord->studList[studPosition].name, studRecord->studList[studPosition].total_owed);
		printf("Books owned by %s:\n", studRecord->studList[studPosition].name);
	
		for(i=0; i<bookRecord->count; i++){
			if(bookRecord->bookList[i].ownerID==studID){
				bookFound=YES;
				strcpy(bookID, bookRecord->bookList[i].serialNum);
			
				//name of book to be removed is displayed
				printf("%s\t", bookRecord->bookList[i].serialNum);
				printf("%s\n", bookRecord->bookList[i].title);

				//book is deleted from list
				deleteBook(bookRecord, bookID, studRecord);
			}//end if
		}//end for

		if(bookFound==NO)
			printf("None found!\n");		

		//student is deleted from list
		--studRecord->count;
		
		for(i=studPosition; i<studRecord->count; i++)
			studRecord->studList[i]=studRecord->studList[i+1];
	}//end if		
		
	printf("\n\n");
}//end deleteStudent


/*name: reportMain
return value: none
parameter(s): list of students, number of students, list of books, number of books
purpose: report function driver*/

void reportMain(book* bookRecord, student* studRecord)
{	
	bookData sorted_bookList[bookRecord->count];
	
	//populates sorted book list with original book list
	loadBooks(sorted_bookList, bookRecord);	
	
	//sorts the book list aphabetically						
	sortBooks(sorted_bookList, bookRecord->count);
					
	int reportChoice=displayReportMenu();					
					
	while(reportChoice!=EXIT){
		if(reportChoice==PAYMENT)
			printTotalOwed(studRecord);				
				
		else{
			if(reportChoice==SUBJ_COUNT)
				displaySubjCount(sorted_bookList, bookRecord->count);								
		
			else{
				if(reportChoice==SUBJ_BOOK_COUNT){		
					displaySubjBooks(sorted_bookList, bookRecord->count);
				}//end if							 		
			}//end else					
		}//end else	
						
		reportChoice=displayReportMenu();										
	}// end report while
}//end reportMain


/*name: findStudent
return value: position of student in array
parameter(s): list of students, number of students, ID of student to be removed from list
purpose: identifies the array index of the student to be removed from student list*/

int findStudent(student* studRecord, int studID)
{
	int i;
	int studPosition=-1; //assuming student does not exist
	
	//traverses list to find student positon index 
	for(i=0; i<studRecord->count; i++){
		if(studID==studRecord->studList[i].idNum){
			studPosition=i;
			break; //or i=studRecord->count
		}//end if			
	}//end for

	//student not found
	if(studPosition==-1)
		printf("\nStudent %i not found!\n", studID);
	
	return studPosition;
}//end findStudent


/*name: loadBooks
return value: none
parameter(s): empty copy of book list, list of books, number of books 
purpose: populates the book list to be sorted*/

void loadBooks(bookData sorted_bookList[], book* bookRecord)
{
	int i;
	
	for(i=0; i<bookRecord->count; i++)
		sorted_bookList[i]=bookRecord->bookList[i];
		
}//end loadBooks


/*name: sortBooks
return value: none
parameter(s): copy of book list, number of books
purpose: sorts the book list alphabetically by subject and then book title*/

void sortBooks(bookData sorted_bookList[], int bCount){
	int i;// for passes
	int j;//for comparisions
	int max_passes=bCount-1;
	bookData temp;//temparory holder
	int first_listed;//stores the position of book to be listed first in unsorted section

	//sorts by subject
	for(i=0; i<max_passes; i++){
		first_listed=i; //asumption

		for(j=i+1; j<bCount; j++){			
			if(strcmpi(sorted_bookList[first_listed].subject, sorted_bookList[j].subject)>0)// the first book comes after alphabetically
				first_listed=j;					
		}//end for (comparion)

		//if the swap was made
		if(i!=first_listed){ //if the book order needs to be changed
			temp=sorted_bookList[i];
			sorted_bookList[i]=sorted_bookList[first_listed];
			sorted_bookList[first_listed]=temp;
		}//end if		
	}//end for (pass control)
	
	//sorts by title
	for(i=0; i<max_passes; i++){
		first_listed=i; //asumption

		for(j=i+1; j<bCount; j++){			
			if(strcmpi(sorted_bookList[first_listed].title, sorted_bookList[j].title)>0 && strcmpi(sorted_bookList[first_listed].subject, sorted_bookList[j].subject)>=0)// the first book comes after alphabetically
				first_listed=j;					
		}//end for (comparion)

		//if the swap was made
		if(i!=first_listed){ //if the book order needs to be changed
			temp=sorted_bookList[i];
			sorted_bookList[i]=sorted_bookList[first_listed];
			sorted_bookList[first_listed]=temp;
		}//end if		
	}//end for (pass control)
	
	//sorting test
	/*for(i=0; i<bCount; i++)
		printf("%s %s\n", sorted_bookList[i].subject, sorted_bookList[i].title);*/
	
}//end sortBooks


/*name: displayReportMenu
return value: integer representing user's report choice
parameter(s): none
purpose: presents a list of report choices to the user*/

int displayReportMenu()
{
	int choice;	

	printf("%i Print the list of students to be paid\n", PAYMENT);
	printf("%i Print the number of available books under each subject category\n", SUBJ_COUNT);
	printf("%i Print the name and quantity of available books in a chosen subject category\n", SUBJ_BOOK_COUNT);
	printf("%i Exit\n\n", EXIT);

	printf ("Enter choice\t");
 	scanf ("%i", &choice);
     
  	//validates choice entered
  	while(choice<PAYMENT || choice>EXIT){
   		printf("Not valid. Re-enter choice\t");
      	scanf("%i", &choice);
   }//end while
     
   printf ("\n\n");
   return choice;
}// end displayReportMenu


/*name: displaySubjCount
return value: none
parameter(s): sorted book list, number of books
purpose: displays each book subject category and number of books under the categories*/

void displaySubjCount(bookData sorted_bookList[], int bCount)
{
	if(bCount>0){
		int i=0, subjCount;
		bookData subjBks[bCount];
		FILE *bookFile;
		char filename[NAMESIZE];
		
		time_t curr_time; //stores raw calendar time
		struct tm *date_info;
		char file_date[12];

		//determining the time of file
		time(&curr_time);
		date_info = localtime(&curr_time);
	
		strftime(file_date, 12, "%b%d,%Y", date_info);//date in format mmdd,yyyy   	   		
   		sprintf(filename, "%s%s%s", "Subject Count List_", file_date, ".txt");//customised filename with date
	
		bookFile=fopen(filename, "w"); 

		if(bookFile==NULL)
			printf("File not opened\n\n\n");
		
		else{	
			//report headings
			fprintf(bookFile, "SUBJECT\t");
			fprintf(bookFile, "NUMBER OF BOOKS\n");
	
			while(i<bCount){
				fprintf(bookFile, "%s\t", sorted_bookList[i].subject);			
				
				subjCount=readSubject(subjBks, subjCount, sorted_bookList, bCount, sorted_bookList[i].subject);						
				fprintf(bookFile, "%i\n", subjCount);
		
				//the book with a diferent subject is found
				i+=subjCount; 					
			}//end while	

			fclose(bookFile);
			printf("Subject quantity report is available\n\n\n");							
		}//end else	
			
	}//end if
	
	else
		printf("No books in list!\n\n\n");
				
}///end displaySubjCount


/*name: displaySubjBooks
return value: none
parameter(s): sorted book list, number of books
purpose: displays the names and quantities  of books under a specific subject category*/

void displaySubjBooks(bookData sorted_bookList[], int bCount)
{
	bookData subjBks[bCount];
	int bookCount, subjCount, i=0;
	char subject[NAMESIZE], filename[NAMESIZE];
	FILE *bookFile;
	
	//choose subject
	fflush(stdin);
	printf("Choose the subject category\t");
	scanf("%[^\n]", subject);

	subjCount=readSubject(subjBks, subjCount, sorted_bookList, bCount, subject);

	//subject is available
	if(subjCount!=0){
		time_t curr_time; //stores raw calendar time
		struct tm *date_info;
		char file_date[12];

		//determining the time of file
		time(&curr_time);
		date_info = localtime(&curr_time);
	
		strftime(file_date, 12, "%b%d,%Y", date_info);//date in format mmdd,yyyy	
   		sprintf(filename, "%s%s%s%s", subject, " List_", file_date, ".txt"); //customised filename with date   
   		
		bookFile=fopen(filename, "w");
	
		if(bookFile==NULL)
			printf("File not opened\n\n\n");
		
		else{			
			//report headings
			fprintf(bookFile, "SUBJECT\t");
			fprintf(bookFile, "NAME OF BOOK\t");
			fprintf(bookFile, "QUANTITY\n\n");	
		
			//count identical books
			while(i<subjCount){
				fprintf(bookFile, "%s\t", subjBks[i].subject);
				fprintf(bookFile, "%s\t", subjBks[i].title);
		
				bookCount=1; //there is already one book with that title
		
				//determines if there are other books with the same title
				while(strcmpi(subjBks[i].title, subjBks[i++].title)==0)
					++bookCount;
									
				fprintf(bookFile, "%i\n", bookCount);		
			}//end while
		
			fclose(bookFile);
			printf("\n%s book list report is available\n\n\n", subject);
		}// end else
	}//end if		 
		
	//non-exisitng subject
	else
		printf("\n%s book list does not exist\n\n\n", subject);			

}//end displaySubjBooks


/*name: enqueue
return value: number of books relevant to a particular subject
parameter(s): current list of subject books, number of subject books, book to be added to subject list
purpose: adds book with matching subject to the current subject book list and updates the number of books in the list*/

int enqueue(bookData subjBks[], int subjCount, bookData bookItem)
{
	//isFull is used to regulate the array population outside this function
	
	subjBks[subjCount]=bookItem;
	++subjCount;
	
	return subjCount;
}//end enqueue
	

/*name: dequeue
return value: next book to be displayed to user
parameter(s): current list of subject books, number of subject books
purpose: removes the book from the subject book list and updates the number of books in the list*/

bookData dequeue(bookData subjBks[], int* subjCount)
{
	bookData bookItem;	
	
	//isEmpty is used to control the book display outside this function

	bookItem=subjBks[0];//0 is the front of the queue
	
	int i;
	--(*subjCount);
	
	for(i=FRONT; i<(*subjCount); i++)
		subjBks[i]=subjBks[i+1];
	
	return bookItem;	
}//end dequeue


/*name: isFull
return value: result of comparison between the current size and maximum size of the subject book list
parameter(s): number of subject books, maximum number of books in list
purpose: compares the current size and maximum size of the subject book list to determine if the list is full*/

int isFull(int rear, int size)
{
	return rear==size;
}//end isFull


/*name: isEmpty
return value: result of comparison between the current size and 0
parameter(s): number of subject books
purpose: compares the current size and queue front index to determine if the list is empty*/

int isEmpty(int rear)
{
	return rear==FRONT;
}//end isEmpty


	
	
		
		
	
		
	