#include"q.h"
//#include "conf.h"
#include<conio.h>
void MAIN_MENU() // creating the data base and printing welcome screen + menu
{
    printf("\t\t# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");
    printf("\t\t# _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ #\n");
    printf("\t\t#|         Welcome to the School database administration           |#\n");
    printf("\t\t#|                Here you can control your database               |#\n");
    printf("\t\t#|             You can Create and Edit Student Accounts            |#\n");
    printf("\t\t#|          Every Account has student information which is         |#\n");
    printf("\t\t#|            Name , ID , Address , Phone , and Birthdate          |#\n");
    printf("\t\t#|    also has Scores (Math Arabic English and Computer Science)   |#\n");
    printf("\t\t#|            You can SEARCH and Delete an Student Account         |#\n");
    printf("\t\t#|                 You Can Edit Every Student Scores               |#\n");
    printf("\t\t#| You also can print all students arranged to Scores or Names(A-Z)|#\n");
    printf("\t\t# ----------------------------------------------------------------- #\n");
    printf("\t\t# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #\n");

    School Q;
    char choice;
    char str[30]="1";
    CreateSchool(&Q);
    enum
    {
        APPEND='1',
        EDIT,
        SEARCH_ID,
        SEARCH_NAME,
        LIST,
        SIZE,
        DELETE,
        PRINT,
        EDIT_SCORE_,
        PRINT_SCORE_='A',
        RANK_STUDENT_,
        EXIT='C',
        PRINT_SCORE_lower='a',
        RANK_STUDENT_lower,
        EXIT_lower='c',
    };
    do{
        printf("\t\t\t\t# # # # # # # # # # # # # # # # # # # # #\n");
        printf("\t\t\t\tPlease Enter Choice:\n");
        printf("\t\t\t\t1-CREATE new Student ACC.\n\t\t\t\t2-EDIT Student\n\t\t\t\t3-SEARCH Student with ID\n\t\t\t\t4-SEARCH Student with Name\n\t\t\t\t5-LIST Students (A-Z)\n\t\t\t\t6-Number OF Students in database\n\t\t\t\t7-DELETE Student\n\t\t\t\t8-PRINT School\n\t\t\t\t9-EDIT ALL Comp.Sc Scores\n\t\t\t\tA-PRINT STUDENTS WITH SCORES\n\t\t\t\tB-RANK STUDENTs WITH Computer Sc SCOREs\n\t\t\t\tC-EXIT\n");
        printf("\t\t\t\t# # # # # # # # # # # # # # # # # # # # #\n");
        fflush(stdin);
        printf("\t\t\t\tyour input : ");
        choice=getche();
        printf("\n\t\t\t\t# # # # # # # # # # # # # # # # # # # # #\n");
    switch(choice)
    {
        case APPEND:
            NEW_STUDENT(&Q);
            break;
        case EDIT:
            printf("\nPlease enter ID of Student :");
            fgets(str,ID_LEN,stdin);
            STUDENT_EDIT(&Q,str);
            break;
        case SEARCH_ID:
            printf("\nPlease enter ID of Student :");
            fgets(str,ID_LEN,stdin);
            printf("\n\n======+++====== STUDENT SEARCH with ID =====++++======\n");
            STUDENT_SEARCH_ID(&Q,str);
            printf("======+++=====END OF STUDENT SEARCH with ID====++++======\n\n");
            break;
        case SEARCH_NAME:
            printf("\nPlease enter the Name of Student :");
            fgets(str,NAME_LEN,stdin);
            printf("\n\n======+++======STUDENT SEARCH with NAME=====++++======\n");
            STUDENT_SEARCH_NAME(&Q,str);
            printf("=====+++=====END OF STUDENT SEARCH with NAME=====+++=====\n\n");
            break;
        case LIST:
            STUDENT_LIST(&Q);
            break;
        case SIZE:
            SchoolSize(&Q);
            break;
        case DELETE:
            printf("\nPlease enter ID of Student :");
            fgets(str,ID_LEN,stdin);
            DELETE_STUDENT(&Q,str);
            break;
        case PRINT: //fun done//
            PRINT_SCHOOL(&Q);
            break;
        case EDIT_SCORE_:
            STUDENT_SCORE(&Q);
            break;
        case PRINT_SCORE_:
        case PRINT_SCORE_lower:
            PRINT_SCHOOL_SCORE(&Q);
            break;
        case RANK_STUDENT_:
        case RANK_STUDENT_lower:

            RANK_STUDENT(&Q);
            break;
        case EXIT:
        case EXIT_lower:
            break;
        default : printf("\t\twrong choice \n");
    }

    }while(choice!= EXIT);
}
void CreateSchool(School *pschool)
{
    pschool->front=NULL;
    pschool->rear=NULL;
    pschool->size=0;
}
void NEW_STUDENT(School *pschool)
{
    bool boolcomp; // used to store result of comparing 2 strings(input name,existed name)(input id,existed id)
    Student* pstudent=(Student*)malloc(sizeof(Student));
    if (pstudent==NULL)
    {
        printf("Heap Memory is FULL\n");
        return;
    }
    printf("\n======+++++======Create New Students Acc.======+++++======\n");
    printf("you have just created new student account\n");
    printf("please enter student data separated \"enter\":\n");
    printf("Student Name: ");
     while(1)
    {
        fgets(pstudent->name,NAME_LEN,stdin);
        boolcomp=STUDENT_SEARCH_NAME_SILENT(pschool,pstudent->name);// comparing the input name with all existing students names
        if (boolcomp==1)
        {
            printf("this NAME is Exist Please enter new NAME \n");

        }
        else break;
    }
    printf("date of birth DD/MM/YYYY: ");
    fgets(pstudent->birth,BIRTH_LEN,stdin);
    while(1)
    {
        printf("ID: ");
        fgets(pstudent->ID,ID_LEN,stdin);
        boolcomp=STUDENT_SEARCH_ID_SILENT(pschool,(pstudent->ID));//  comparing the input id with all existing students ids
        if (boolcomp==1)
        {
            printf("this ID is Exist Please enter new ID \n");

        }
        else break;
    }
    printf("Address: ");
    fgets(pstudent->address,ADDRESS_LEN,stdin);
    printf("Phone Number: ");
    fgets(pstudent->phone,PHONE_LEN,stdin);

    pstudent->pNext=NULL;

    if (isEmpty(pschool)) // case of empty queue
    {
        pschool->front=pstudent;
        pschool->rear=pstudent;
    }
    else     // not empty
    {
        pschool->rear->pNext=pstudent;
        pschool->rear=pstudent;

    }
    (pschool->size)++;
    pstudent->com_sc=0;   // assigning scores to 0 to avoid garbages
    pstudent->eng_sc=0;
    pstudent->ar_sc=0;
    pstudent->math_sc=0;
    printf("======+++======end of Creating New Students Acc.=====+++++======\n\n");
}
bool isEmpty(School *pschool)
{
    return ( pschool->front==NULL );
}
void STUDENT_EDIT(School *pschool,char id[ID_LEN])
{
    char temp_id[ID_LEN]="00";
    char temp_name[NAME_LEN]="0";
    bool boolcomp; // used to store result of comparing 2 strings(input id,existed id)
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    printf("\n======+++====== EDIT STUDENT =====++++======\n");
    Student *temp = STUDENT_SEARCH_ID(pschool,id); // the SEARCH function return pointer to the student node(Student data type) if student is found
    // if not found it return NULL
    if (temp==NULL) //the student not found//STUDENT_SEARCH()it self prints empty school or student not found
        return;
    // student is found
    /// the search function has just printed the student info now; ///
    printf("you are going to edit this student account:\n");
    printf("please enter student data separated \"enter\":\n");
    printf("Student Name: ");
    while(1) // search if the input name is existed
    {
        printf("Name: ");
        fgets(temp_name,ID_LEN,stdin);
        strcpy(temp->name,"0"); // assigning to 0 to avoid the case the new name = the old one (the search func would return the name is existed)
        boolcomp=STUDENT_SEARCH_NAME_SILENT(pschool,temp_name);////////// comparing input name and existed names
        if (boolcomp==1)
        {
            printf("this NAME is Exist Please enter new Name \n");

        }
        else
        {
            strcpy(temp->name,temp_name);
            break;
        }
    }
    printf("date of birth DD/MM/YYYY: ");
    fgets(temp->birth,BIRTH_LEN,stdin);
    while(1) // search if the input id is existed
    {
        printf("ID: ");
        fgets(temp_id,ID_LEN,stdin);
        strcpy(temp->ID,"0"); // assigning to 0 to avoid the case the new id = the old one (the search func would return the id is existed)
        boolcomp=STUDENT_SEARCH_ID_SILENT(pschool,temp_id);////////// comparing input id and existed ids
        if (boolcomp==1)
        {
            printf("this ID is Exist Please enter new ID \n");

        }
        else
        {
            strcpy(temp->ID,temp_id);
            break;
        }
    }
    printf("Address: ");
    fgets(temp->address,ADDRESS_LEN,stdin);
    printf("Phone Number: ");
    fgets(temp->phone,PHONE_LEN,stdin);
    printf("New Data of the Student:\nName:%sBirth:%sID:%sAddress:%sPhone Number:%s",temp->name,temp->birth,temp->ID,temp->address,temp->phone);
    printf("======+++====== END OF EDITing STUDENT =====+++=====\n\n");
}

Student* STUDENT_SEARCH_ID(School *pschool,char id[ID_LEN])
{
    if(isEmpty(pschool))
    {
        printf("the school is empty\n");
        return NULL;
    }
    Student *temp=pschool->front;
    int idEqualitity=stricmp(temp->ID,id); // it
    while(1)
    {
        if (!idEqualitity)
        {
            printf("Student is found with ID:%s",id);
            PRINT_STUDENT(temp);
            break;
        }
        temp=temp->pNext;
        if (temp==NULL) break;
        idEqualitity=stricmp(temp->ID,id);
    }
    if (temp==NULL)
    {
        printf("Student is not found with ID:%s",id);
        return NULL;
    }
    else
        return temp;
}
Student* STUDENT_SEARCH_NAME(School *pschool,char NAME[NAME_LEN]) // search for student
{
    if(isEmpty(pschool))
    {
        printf("the school is empty\n");
        return NULL;
    }
    Student *temp=pschool->front;
    int idEqualitity=stricmp(temp->name,NAME);
    while(1)
    {
        if (!idEqualitity)
        {
            printf("Student is found with Name:%s",NAME);
            PRINT_STUDENT(temp);
            break;
        }
        temp=temp->pNext;
        if (temp==NULL) break;
        idEqualitity=stricmp(temp->name,NAME);
    }
    if (temp==NULL)
    {
        printf("Student is not found with Name:%s",NAME);
        return NULL;
    }
    else
        return temp;

}

void DELETE_STUDENT(School *pschool,char id[ID_LEN]) //search and delete specific student using ID
{

    if(isEmpty(pschool))
    {
        printf("the school is empty\n");
        return ;
    }
    int idEquality=stricmp(pschool->front->ID,id); // compare ids. it returns 0 if they are identical
    printf("\n====+++====DELETING STUDENT ACC.===+++====\n");
    if (pschool->front==pschool->rear && !idEquality) // case of one student in the school
        {
            PRINT_STUDENT(pschool->front);
            free(pschool->front);
            pschool->front=NULL;
            pschool->rear=NULL;
            (pschool->size)--;
            printf("Student is deleted with ID:%s",id);
            printf("====+++====end of DELETING ===+++====\n\n");
        }
    else                        // more than one student, cases: the found student is in (start,end or middle)
    {
        Student *tempCur=pschool->front;
        Student *tempPrev=NULL;
        idEquality=stricmp(tempCur->ID,id);
        while(1)                // SEARCHING for the student
        {
            if (!idEquality)
            {
                break;
            }
            tempPrev=tempCur;
            tempCur=tempCur->pNext;
            if(tempCur==NULL) break;
            idEquality=stricmp(tempCur->ID,id);
        }
        if (tempCur==NULL) //                   student is not exist
        {
            printf("Student is not found with ID:%s",id);
            printf("====+++====END OF DELETING===+++====\n\n");
            return;
        }
        // student is exist

        if(pschool->front==tempCur) // (found student exist at the start) front points to current student (tempCur)
        {
            PRINT_STUDENT(pschool->front); // printing the student info before deleting
            tempCur=pschool->front->pNext;
            free(pschool->front);
            pschool->front=tempCur;
        }
        else if (pschool->rear==tempCur) // (found student exist at the end) rear points to current student (tempCur)
        {
            PRINT_STUDENT(pschool->rear);   // printing the student info before deleting
            tempPrev->pNext=NULL;
            pschool->rear=tempPrev;
            free(tempCur);


        }
        else                            // (found student exist in middle)(non rear neither front points to current student
        {
            PRINT_STUDENT(tempCur); // printing the student info before deleting
            tempPrev->pNext=tempCur->pNext;
            free(tempCur);

        }
        (pschool->size)--;
        printf("Student is deleted with ID:%s",id);
        printf("====+++====END OF DELETING===+++====\n\n");
    }
}
void STUDENT_LIST(School *pschool) // listing database by names (A-Z)
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    printf("\n======+++====== LISTING STUDENTS A-Z =====+++=====\n");
    if(pschool->front==pschool->rear) // case of one student in database
    {
        printf("there is only one student\n");
        printf("======+++====== END LISTING STUDENTS =====+++=====\n\n");
        return;
    }
    //more than one element
    Student *tempCur;
    Student *tempPrev; // to compare prev with the current (cuz this is single linked list no pPrev option)
    Student *holder=(Student*)malloc(sizeof(Student)); /// to hold variable of Student data type after deleting it
    if (holder==NULL)
    {
        printf("Heap Memory is FULL\n");
        return;
    }

    int flag;
    for (int i=0;i< pschool->size ; i++ )
    {
        tempCur= pschool->front->pNext;
        tempPrev=pschool->front;
        flag=0; // used to check if the elements are arranged before reaching size-1
        int j=0; // for numbering all students for the user
        while(1)
        {
            j++;
            if (stricmp(tempPrev->name,tempCur->name)>0)
            {
                flag=1; //the elements are not arranged yet;
                tempPrev->pNext=tempCur->pNext;                    //// some attention please
                tempCur->pNext=tempCur;
                *holder=*tempPrev;
                *tempPrev=*tempCur;
                *tempCur=*holder;
            }
            tempPrev=tempCur;
            tempCur=tempCur->pNext;
            if (tempCur==NULL) break;
        }
        if (flag==0) break; //elements are arranged before reaching size-1
    }
    free(holder);
    PRINT_SCHOOL(pschool);
    printf("======+++====== END LISTING STUDENTS =====+++=====\n\n");
}
void PRINT_STUDENT(Student *pstudent) // print single student info
{
    printf("\n======= Student DATA ========\n");
    printf("Name:%sBirth:%sID:%sAddress:%sPhone Number:%s",pstudent->name,pstudent->birth,pstudent->ID,pstudent->address,pstudent->phone);
    printf("=====end of student data=====\n\n");
}
void PRINT_SCHOOL(School *pschool) // print multi PRINT_STUDENT() until reaching NULL
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    int i=0;
    Student *temp=pschool->front;
    printf("\n===+++++===PRINT SCHOOL===+++++===\n");
    while (1)
    {
        i++;
        printf("(%d) ",i);
        PRINT_STUDENT(temp);
        temp=temp->pNext;
        if (temp==NULL) break;
    }
    printf("==++++==END OF PRINTING SCHOOL==++++==\n\n");
}
void STUDENT_SCORE(School *pschool) // editing every score of every student
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    printf("\n=====+++++=====EDIT Scores of Students=====+++++=====\n");
    Student *temp=pschool->front;
    while(temp!=NULL)
    {
    printf("Student Name=%sID=%sComp.Science Score= ",temp->name,temp->ID);
    scanf("%d",&(temp->com_sc));
    printf("\nArabic Score= ");
    scanf("%d",&(temp->ar_sc));
    printf("\nEnlgish Score= ");
    scanf("%d",&(temp->eng_sc));
    printf("\nMath Score= ");
    scanf("%d",&(temp->math_sc));
    temp=temp->pNext;
    printf("_____________________\n");
    }
    printf("===+++===end of EDITING Scores of Students===+++===\n\n");
}
void PRINT_STUDENT_SCORE(Student *pstudent) // print single student scores
{
    printf("\n=====Student DATA======\nName:%sID:%s",pstudent->name,pstudent->ID);
    printf("Comp.Science=%d Arabic=%d English=%d Math=%d\n",pstudent->com_sc,pstudent->ar_sc,pstudent->eng_sc,pstudent->math_sc);
    printf("=====end of student scores=====\n\n");
}
void PRINT_SCHOOL_SCORE(School *pschool) // printing student scores with the current arrangement (multi PRINT_STUDENT_SCORE())
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    int i=0; // used to print count for every student
    Student *temp=pschool->front;
    printf("\n===++++===PRINT SCHOOL SCORES===++++===\n\n");
    while (1)
    {
        i++;
        printf("(%d) ",i);
        PRINT_STUDENT_SCORE(temp);
        temp=temp->pNext;
        if (temp==NULL) break;
    }
    printf("==++++==END OF PRINTING SCHOOL SCORES ==++++==\n\n");
}
void RANK_STUDENT(School *pschool) // ranking all data base with students computer sc. scores
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    printf("\n====+++====RANKING STUDENTS with Comp. Sc. Scores===++++====\n");
    Student *tempCur;
    Student *tempPrev;
    Student *holder=(Student*)malloc(sizeof(Student)); /// to hold variable of Student data type after deleting it
    if (holder==NULL)
    {
        printf("Heap Memory is FULL\n");
        return;
    }
    if (pschool->front==pschool->rear)// case of one student in database
    {
        printf("There is Only one Student\n");
        printf("====+++====END OF RANKING STUDENTS===++++====\n\n");
        return;
    }
    else      // case more than one student
        {
        int flag;
        for (int i=0;i< pschool->size ; i++ )
            {
            tempCur= pschool->front->pNext;
            tempPrev=pschool->front;
            flag=0;
            while(1)
            {
                if (tempPrev->com_sc<tempCur->com_sc)
                {
                    flag=1;
                    tempPrev->pNext=tempCur->pNext;                    //// some attention please
                    tempCur->pNext=tempCur;
                    *holder=*tempPrev;
                    *tempPrev=*tempCur;
                    *tempCur=*holder;
                }
                tempPrev=tempCur;
                tempCur=tempCur->pNext;
                if (tempCur==NULL) break;
            }
            if (flag==0) break;
            }
            printf("====+++====RANKING STUDENTS has been done===++++====\n\n");
        }
}

void SchoolSize(School *pschool) // #OF STUDENTS
{
    if (isEmpty(pschool))
    {
        printf("The Data Base is Empty\n");
        return;
    }
    printf("Number OF Students = %d\n",pschool->size);
}

bool STUDENT_SEARCH_ID_SILENT(School *pschool,char id[ID_LEN]) // like STUDENT_SEARCH_ID but with no print
{
    if(isEmpty(pschool))
    {
        return 0;
    }
    Student *temp=pschool->front;
    int idEquality=stricmp(temp->ID,id); // it
    while(1)
    {
        if (!idEquality)
        {
            break;
        }
        temp=temp->pNext;
        if (temp==NULL) break;
        idEquality=stricmp(temp->ID,id);
    }
    if (temp==NULL)
    {
        return 0;
    }
    else
        return 1;
}
bool STUDENT_SEARCH_NAME_SILENT(School *pschool,char NAME[NAME_LEN]) // like STUDENT_SEARCH_NAME but with no print
{
    if(isEmpty(pschool))
    {
        return 0;
    }
    Student *temp=pschool->front;
    int idEquality=stricmp(temp->name,NAME); // it
    while(1)
    {
        if (!idEquality)
        {
            break;
        }
        temp=temp->pNext;
        if (temp==NULL) break;
        idEquality=stricmp(temp->name,NAME);
    }
    if (temp==NULL)
    {
        return 0;
    }
    return 1;

}
