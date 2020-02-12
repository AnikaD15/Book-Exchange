constant integer BOOKSTORAGE=500
constant integer MAXSTUDENTS=200
constant integer FRONT=0

enum menuOpt {READ=1, SELL, DELETE_STUDENT, PRINT_REPORT, MAIN_EXIT}
enum reportOpt {PAYMENT=1, SUBJ_COUNT, SUBJ_BOOK_COUNT, EXIT}
enum check {YES, NO}

record bookData
	integer ownerID
	string author
	integer price
	string subject
	string title
	string serialNum
	string date_received
end record

record studentData
	integer idNum 
	string name	
	integer total_owed;
end record

record student
	studentData studList[MAXSTUDENTS]
	integer count
end record

record book
	bookData bookList[BOOKSTORAGE]
	integer count
end record


main()

	student studRecord
	book bookRecord
	integer choice, studID
	
	bookRecord.count=retrieveBooks(bookRecord.bookList)
	studRecord.count=retrieveStudents(studRecord.studList)

	choice=displayMenu()
	
	while choice<>MAIN_EXIT do
		if choice=SELL then
			makeSale(&bookRecord, &studRecord)
				
		else
			if choice=READ then 			
				studRecord.count=addStudent(studRecord.studList, studRecord.count, &studID)							
				bookRecord.count=readBook(bookRecord.bookList, bookRecord.count, studID)			
				
			else
				if choice=DELETE_STUDENT then					
					deleteStudent(&studRecord, &bookRecord)				
				
				else
					if choice=PRINT_REPORT then
						reportMain(&bookRecord, &studRecord)
					end if													
				end if
			end if
		end if
		
		choice=displayMenu()
	end while 				
	
	saveBooks(bookRecord.bookList, bookRecord.count) 
	saveStudents(studRecord.studList, studRecord.count)
	return 0
end main


integer displayMenu()

	integer choice
	
	print main menu options
	
	print "Enter choice"
 	read choice
     
  	while choice<READ OR choice>MAIN_EXIT do
   		print "Not valid. Re-enter choice"
      		read choice
   	end while   
   
  	return choice
end displayMenu


integer retrieveBooks(bookData bookList[])

	integer amt
	amt=0
	
	open Book_List to read
		
	if Book_List is correctly opened then	
		while data exist in Book_List do
			read book List from file						
			amt=amt+1			
		end while
		
		close Book_List	
		
	else		
		print "Book file not available"		
	end if
	
	return amt
end retrieveBooks


integer retrieveStudents(studentData studList[])

	integer amt
	amt=0
	
	open Student_List to read
	
	if Student_List is correctly opened then
				
		while data exist in Student_List do
			read student List from file									
			amt=amt+1			
		end while
		
		close Student_List	
			
	else
		print "Student file not available"
	end if	
	
	return amt
end retrieveStudents


integer checkStudent(studentData studList[], integer sCount, integer ID)

	integer newStudent, i
	
	newStudent=YES
	
	for i=0 to sCount-1 do		
		if ID=studList[i].idNum then
			newStudent=NO
			i=sCount
		end if
	end for
	
	return newStudent
end checkStudent


integer addStudent(studentData studList[], integer sCount, integer* studID)

	integer studCheck

	print "Enter last 4 digits of student ID"
	read studID

	while *studID<0 OR *studID>9999 do
   		print "Not valid. Re-enter student ID"
      	read studID
	end while

	studCheck=checkStudent(studRecord.studList, studRecord.count, *studID)

	if studCheck=YES then	
		studList[sCount].idNum = *studID
	
		print "Enter name of student"
		read studList[sCount].name
	
		studList[sCount].total_owed=0
		
		sCount=sCount+1
	end if

	return sCount
end addStudent


integer readBook (bookData bookList[], integer bCount, integer ownerID)
		
	bookList[bCount].ownerID = ownerID
	
	Determine date received
   	Assign date to bookList[bCount].date_received	
	
	print "Enter serial number of book"
	read bookList[bCount].serialNum
		
	print "Enter book title"
	read bookList[bCount].title
			
	print "Enter first and last name of (first) author"
	read bookList[bCount].author
	
	print "Enter selling price"
	read bookList[bCount].price
	
	while bookList[bCount].price<=0 do
   		print "Not valid. Re-enter price"
      	read bookList[bCount].price    	
   	end while
		
	print "Enter subject"
	read bookList[bCount].subject
	
	bCount=bCount+1
	return bCount
end readBook


saveBooks(bookData bookList[], integer bCount)

	integer i
	
	open Book_List to write
	
	if Book_List is correctly opened then
	
		for i=0 to bCount-1 do
			write book list to file			
		end for
		
		close Book_List
		print "Books saved to file"		
		
	else	
		print "Book file not opened"		
	end if

end saveBooks


saveStudents(studentData studList[], integer sCount)
	
	integer i
	
	open Student_List to write
	
	if Student_List is correctly opened then
	
		for i=0 to sCount-1 do
			write student list to file			
		end for		
				
		close Student_List
		print "Students saved to file"
		
	else	
		print "Student file not opened"
	end if
end saveStudents


makeSale(book* bookRecord, student* studRecord)

	string bookID, curr_choice, final_choice
	integer isAvailable, isSold
		
	repeat			
		displayBooks(bookID, &isAvailable, &isSold, bookRecord) 		
	
		if isAvailable=YES AND isSold=NO then
			print "Do you want any of these books?"
			read curr_choice)			
				
			if curr_choice="yes" then
				isSold=YES
				print "Enter the ID of book chosen"
				read bookID				
			end if		
		end if
	
		if isAvailable=YES AND isSold=YES then
			deleteBook(bookRecord, bookID, studRecord)
			print "Book sold!"	
		end if	
		
		print "Do you want any more books?"
		read final_choice			
								
	until final_choice="no"				
	
end makeSale


displayBooks(string bookID[], integer *isAvailable, integer *isSold, book* bookRecord)
 
	string choice, bookTitle, subject
	bookData subjBks[bookRecord->count], bookItem
	integer subjCount, i
		
	*isAvailable=NO
	*isSold=NO

	print "Enter the subject"
	read subject		
			
	subjCount=readSubject(subjBks, subjCount, bookRecord->bookList, bookRecord->count, subject)
		
	if subjCount=0 then
		print "No book in this subject category"
	
	else		
		print "What is the name of book?"
		read bookTitle

		while isEmpty(subjCount)=0 do
			bookItem=dequeue(subjBks, &subjCount)
			
			if bookItem.title=bookTitle then
				*isAvailable=YES
				
				print book information
												
				print "Do you want this book?"
				read choice
								
				if choice="yes" then
					*isSold=YES
					bookID=subjBks[i].serialNum
					subjCount=0				
				end if
			end if
			
		end while		
		
		if choice<>"yes" then
			print "No other ",bookTitle, " found"
		end if
				
	end if
end displayBooks


integer readSubject(bookData subjBks[], integer subjCount, bookData bookList[], integer bCount, string subject[])

	integer i 
	subjCount=0
	
	i=0
	while isFull(subjCount, bCount)=0 OR i<bCount do			
		if bookList[i].subject=subject then			
			subjCount=enqueue(subjBks, subjCount, bookList[i])
		end if
			
		i=i+1	
	end while	
	
	return subjCount	
end readSubject
	

deleteBook(book* bookRecord, string bookID[], student* studRecord)

	integer bookSold, i	
		
	for i=0 to bookRecord->count-1 do		
		if bookID=bookRecord->bookList[i].serialNum then
			bookSold=i			
			i=bookRecord->count
		end if
	end for
	
	bookRecord->count=bookRecord->count-1
	
	calcTotal(studRecord, bookRecord->bookList[bookSold])
	
	for i=bookSold to bookRecord->count-1 do
		bookRecord->bookList[i]=bookRecord->bookList[i+1]
	end for	
	
end deleteBook


calcTotal(student *studRecord, bookData bookSold)
	integer i
	
	for i=0 to studRecord->count-1 do		 
		if bookSold.ownerID=studRecord->studList[i].idNum then		
			studRecord->studList[i].total_owed+=bookSold.price 
			i=studRecord->count
		end if
	end for
end calcTotal


printTotalOwed(student studList[], integer sCount)
	integer i
	
	open Money_Owed to write
	
	if Money_Owed is correctly opened then
		
		for i=0 to sCount-1 do		
			if studList[i].total_owed<=0 then
				write student to file		
				write "Total owed: $", studList[i].total_owed
				studList[i].total_owed=0 
			end if			
		end for
		
		close Money_Owed		
		
	else
		print "Payment file not opened"
	end if
	
end printTotalOwed


deleteStudent (student* studRecord, book *bookRecord)

	string bookID
	integer i, studPosition, bookFound, studID
	
	bookFound=NO

	print "Enter last four digits of student ID"
	read studID
	
	while studID<0 OR studID>9999 do
   		print "Not valid. Re-enter student ID"
      		read studID
	end while

	studPosition=findStudent(studRecord, studID)
	
	print "Total owed to student:", studRecord->studList[studPosition].total_owed
	print "Books owned by ", studRecord->studList[studPosition].name)
	
	for i=0 to bookRecord->count-1 do
		if bookRecord->bookList[i].ownerID=studID then
			bookFound=YES
			bookID=bookRecord->bookList[i].serialNum			
			
			print bookRecord->bookList[i].serialNum
			print bookRecord->bookList[i].title

			deleteBook(bookRecord, bookID, studRecord)
		end if
	end for

	if bookFound=NO then
		print "None found!"
	end if
	
	studRecord->count=studRecord->count-1
	
	for i=studPosition to studRecord->count-1 do
		studRecord->studList[i]=studRecord->studList[i+1]
	end for	
	
end deleteStudent


reportMain(book* bookRecord, student* studRecord)
	
	bookData sorted_bookList[bookRecord->count]
	
	loadBooks(sorted_bookList, bookRecord)						
	sortBooks(sorted_bookList, bookRecord->count)
					
	integer reportChoice
	reportChoice=displayReportMenu()										
					
	while reportChoice<>EXIT do
		if reportChoice=PAYMENT then
			printTotalOwed(studRecord)						
				
		else
			if reportChoice=SUBJ_COUNT then
				displaySubjCount(sorted_bookList, bookRecord->count)								
		
			else
				if reportChoice=SUBJ_BOOK_COUNT then					
					displaySubjBooks(sorted_bookList, bookRecord->count)
				end if							 		
			end if					
		end if	
						
		reportChoice=displayReportMenu()										
	end while 
end reportMain


integer findStudent(student* studRecord, integer studID)

	integer i, studPosition

	studPosition=-1	
	
	for i=0 to studRecord->count-1 do
		if studID=studRecord->studList[i].idNum then
			studPosition=i
			i=studRecord->count
		end if			
	end for

	if studPosition=-1 then
		print "Student ", studID, " not found!"
	end if
	
	return studPosition
end findStudent


loadBooks(bookData sorted_bookList[], book* bookRecord)

	integer i
	
	for i=0 to bookRecord->count-1 do
		sorted_bookList[i]=bookRecord->bookList[i]
	end for
		
end loadBooks


sortBooks(bookData sorted_bookList[], integer bCount)
	integer i, j, max_passes, first_listed
	bookData temp
		
	max_passes=bCount-1

	for i=0 to max_passes-1 do
		first_listed=i 

		for j=i+1 to bCount-1 do			
			if sorted_bookList[first_listed].subject>sorted_bookList[j].subject then
				first_listed=j	
			end if				
		end for 
		
		if i<>first_listed then
			temp=sorted_bookList[i]
			sorted_bookList[i]=sorted_bookList[first_listed]
			sorted_bookList[first_listed]=temp
		end if		
	end for	
	
	for i=0 to max_passes-1 do
		first_listed=i 

		for j=i+1 to bCount-1 do			
			if sorted_bookList[first_listed].title>sorted_bookList[j].title AND sorted_bookList[first_listed].subject>=sorted_bookList[j].subject then
				first_listed=j	
			end if				
		end for 
		
		if i<>first_listed then
			temp=sorted_bookList[i]
			sorted_bookList[i]=sorted_bookList[first_listed]
			sorted_bookList[first_listed]=temp
		end if		
	end for	
	
end sortBooks


integer displayReportMenu()

	integer choice	

	print report options

	print "Enter choice"
 	read choice
     
  	while choice<PAYMENT OR choice>EXIT do
   		print "Not valid. Re-enter choice"
      	read choice
   	end while     
   
   	return choice
end displayReportMenu


displaySubjCount(bookData sorted_bookList[], integer bCount)

	if bCount>0 then 
		integer subjCount, i
		bookData subjBks[bCount]
		
		Determine current time and date
		Attach date to created filename
	
		open Subject_List to write
	
		if Subject_List is correctly opened then	
			write headings to file
		
			i=0
			while i<bCount do
				write book subject to file			
				
				subjCount=readSubject(subjBks, subjCount, sorted_bookList, bCount, sorted_bookList[i].subject)						
				write subjCount to file
		
				i=subjCount+i 					
			end while	

			close Subject_List
			print "Subject quantity report is available"
		
		else	
			print "File not opened"							
		end if	
		
	else 
		print "No books in list!"
	end if
		
end displaySubjCount


displaySubjBooks(bookData sorted_bookList[], integer bCount)

	bookData subjBks[bCount]
	integer subjCount, bookCount, i	
	string subject

	print "Choose the subject category"
	read subject

	subjCount=readSubject(subjBks, subjCount, sorted_bookList, bCount, subject)

	if subjCount<>0 then	

		Determine current time and date
		Attach date to created filename
	
		open Subject_List to write
	
		if Subject_List is correctly opened then	
			write headings to file		
		
			i=0
			while i<subjCount do
				write book subject and title to file
		
				bookCount=1		
				while subjBks[i].title=subjBks[i++].title do
					bookCount=bookCount+1
				end while
			
				write book quantitity to file
			end while
			
			close Subject_List
			print subject," book list report is available" 
				
		else	
			print "File not opened"							
		end if
	
	else
		print subject, " book list does not exist"			
	end if		
	
end displaySubjBooks


integer enqueue(bookData subjBks[], integer subjCount, bookData bookItem)
		
	subjBks[subjCount]=bookItem
	subjCount=subjCount+1
	
	return subjCount
end enqueue
	

bookData dequeue(bookData subjBks[], integer* subjCount)

	bookData bookItem
	bookItem=subjBks[FRONT]
	
	integer i
	(*subjCount)=(*subjCount)-1
	
	for i=FRONT to (*subjCount)-1 do
		subjBks[i]=subjBks[i+1]
	end for
	
	return bookItem	
end dequeue


integer isFull(integer rear, integer size)
	return rear=size
end isFull


integer isEmpty(integer rear)
	return rear=FRONT
end isEmpty


	
	
		
		
	
		
	