#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define CONSTRSIZE 64




/*--------------------------------------DATA STRUCTURES AND GLOBAL VARIABLES------------------------------------------*/

//hash table node: each node contains one specific letter, two array of indexes representing respectively where was
//                 the letter found in the word to find, and where the letter is surely not positioned.
//                 it also has an int occ, which gives the exact occurrence of the specific letter of the node in the
//                 reference word
typedef struct constr{
    //char letter; //I do not need it!!!!
    int* found_in;
    int* not_in;
    int count_not_in;
    int tempCount;
    int count;
    bool isIn;
    bool found;
    int occ;
} constr;

//hash table array: since I know I have 64 elements, I can declare it here
constr* constrTab[CONSTRSIZE];

typedef struct node{
    bool printable;
    struct node* right_child;
    struct node* left_child;
    struct node* next;
    struct node* constr_next;
    char string[];
}node;

node* T = NULL;
node* first_constr = NULL;
node* last_constr = NULL;
node* last_next = NULL;

int dim = 0;
int printableDim = 0;
int dictionaryDim = 0;
/*------------------------------------------------CONSTR FUNCTIONS----------------------------------------------------*/

//hash function: receives in input a character and outputs the index position in which that character is stored
int hash_constr(char a) {
    int b = (int) a;
    if (b == 45) {
        return 0;
    } else if (b >= 48 && b <= 57) {
        return b - 47;
    } else if (b >= 65 && b <= 90) {
        return b - 54;
    } else if (b == 95) return b - 58;

    else if (b >= 97 && b <= 122) {
        return b - 59;
    } else return -1;

}

//initializes the constrTab
void createConstr(){
    //INITIALIZE THE TABLE
    for(int i = 0; i<CONSTRSIZE; i++) {
        constr* elem = malloc(sizeof(constr));
        elem -> found_in = malloc(sizeof(int));
        elem -> not_in = malloc(sizeof(int));
        for(int j = 0; j < dim;j++){
            elem -> found_in[j] = -1;
            elem -> not_in[j] = -1;
        }
        elem -> occ = 0;
        elem -> count_not_in = 0;
        elem -> isIn = true;
        elem -> found = false;
        elem -> tempCount = 0;
        elem -> count = 0;
        constrTab[i] = elem;
    }
}

//reset the constrTab
void resetConstraints(){
    for(int i=0; i < CONSTRSIZE; i++){
        constrTab[i] -> occ = 0;
        constrTab[i] -> count_not_in = 0;
        constrTab[i] -> isIn = true;
        constrTab[i] -> found = false;
        constrTab[i] -> tempCount = 0;
        constrTab[i] -> count = 0;
        for( int j = 0; j < dim; j++){
            constrTab[i] -> found_in[j] = -1;
            constrTab[i] -> not_in[j] = -1;
        }
    }

}


void setFoundIn(int index, int found_in_index){
    if (constrTab[index] -> found_in[found_in_index] == -1){
        constrTab[index] -> found_in[found_in_index] = 0;
    }
}

void setNotIn(int index, int not_in_index){
    if (constrTab[index] -> not_in[not_in_index] == -1)  constrTab[index] -> not_in[not_in_index] = 0;
}

void setDifferentPosition(int index, int not_in_index){
    if (constrTab[index] -> not_in[not_in_index] == -1)  constrTab[index] -> not_in[not_in_index] = 2;
}


//function that updates the constrTab parameters
void manageConstraints(char* word, const char* output){
    int temp;
    for(int i = 0; i< dim; i++){
        temp = hash_constr(word[i]);
        constrTab[temp] -> tempCount = 0;
    }
    for(int i=0; i<dim; i++){
        temp = hash_constr(word[i]);
        switch(output[i]){
            case '+':
                constrTab[temp] -> tempCount++;
                constrTab[temp] -> found = true;
                if(constrTab[temp] -> count <  constrTab[temp] -> tempCount)  constrTab[temp] -> count =  constrTab[temp] -> tempCount;
                setFoundIn(temp,i);
                for(int j = 0; j < CONSTRSIZE; j++) {
                    if (j != temp) {
                        setNotIn(j, i);
                    }
                }
                break;
            case '/':
                constrTab[temp] -> found = true;
                setNotIn(temp,i);
                constrTab[temp] -> count_not_in++;

                if (constrTab[temp] -> occ == 0){
                    constrTab[temp] -> isIn = false;
                }
                break;
            case '|':
                constrTab[temp] -> tempCount++;
                constrTab[temp] -> found = true;
                if(constrTab[temp] -> count <  constrTab[temp] -> tempCount)  constrTab[temp] -> count =  constrTab[temp] -> tempCount;
                setDifferentPosition(temp,i);
                break;
            default:
                exit(1);
        }
    }
}

//receives in input the constrTab index, returns the letter that matches that index
char hashInv(int i){
    if (i == 0) {
        return (char) 45;
    }
    else if (i > 0 && i <= 10) {
        return (char) (i + 47);

    } else if (i > 10 && i <= 36) {
        return (char) (i + 54);
    } else if (i == 37) return (char) (i + 58);

    else if (i > 37) {
        return (char) (i + 59);
    }
    return ',';
}

/*------------------------------------------------BS-TREE FUNCTIONS---------------------------------------------------*/

node* insertBST(node* curr, char* string){
//create a new node
    if (curr == NULL){

        node * temp;
        temp = malloc(sizeof(node)+dim+1);
        for(int i = 0; i < dim; i++) {
            temp -> string[i] = string[i];
        }

        temp -> left_child = NULL;
        temp -> right_child = NULL;
        if(last_next == NULL){
            last_next = temp;
        }

        else {
            last_next->next = temp;
            last_next = temp;
        }
        temp -> printable = true;
        if (last_constr == NULL) last_constr = temp;
        last_constr ->constr_next = temp;
        last_constr = temp;
        dictionaryDim++;
        printableDim++;
        return temp;
    }
    //still need to find where to insert a new node:
    //case 1: the string comes after the current node (I need to insert into the right)
    if( strcmp(string, curr -> string)<0){ //key< T
        curr -> left_child = insertBST( curr -> left_child, string);
    }
        //case 2: the string comes before the current node (I need to insert into the left)
    else if (strcmp(string, curr -> string)>0){
        curr -> right_child = insertBST( curr -> right_child, string);
    }
    return curr;
}


node* search(node* temp, char string[]){
    //found the element
    if (temp == NULL) return temp;
    if (strcmp(temp -> string,string)==0) return temp;
    // word comes after
    if (strcmp(temp -> string,string)<0) return search(temp -> right_child, string);
    // word comes before
    return search(temp -> left_child, string);
}

void resetBST(node* temp){
    first_constr = temp;
    while(temp != NULL){

        temp -> printable = true;
        temp -> constr_next = temp -> next;
        if(temp -> next == NULL) last_constr = temp;
        temp = temp-> next;
    }

    printableDim = dictionaryDim;
}

//in order print of the strings
void printFiltered(node* temp){
    if(temp!=NULL){
        printFiltered(temp -> left_child);
        if(temp -> printable == true) printf("%s\n", temp -> string);
        printFiltered(temp -> right_child);
    }
}

/*------------------------------------------------WORDLE FUNCTIONS----------------------------------------------------*/

//function that builds the output
void output(const char* string, char r[], char* out){
    bool constraints[strlen(r)];
    int checked[CONSTRSIZE];
    int i;
    //initialize the output and flag array
    for(i=0; i<strlen(r); i++){
        constraints[i] = false;
        out[i] = '/';
    }
    for(int k = 0; k<CONSTRSIZE; k++){
        checked[k] = constrTab[k] -> occ;
    }
    out[i] = '\0';

    //check constraints
    for(i = 0; i< strlen(r); i++){
        if(string[i] == r[i]){
            out[i] = '+';
            constraints[i] = true;
            checked[hash_constr(string[i])]--;
        }
    }
    for(i=0; i<strlen(r); i++){
        if(out[i]!='+'){

            for(int j=0; j<strlen(r); j++){
                if(!constraints[j] && string[i] == r[j] && checked[hash_constr(r[j])] != 0){
                    out[i] = '|';
                    checked[hash_constr(string[i])]--;
                    break;
                }

            }
        }
    }
    out[strlen(r)] = '\0';
}


//function that manages the list of words that can be printed
void filter() {
    node *previous;
    node *node;
    char temp;
    int tempCount;

    for (int i = 0; i < CONSTRSIZE; i++) {
        temp = hashInv(i);
        //if I updated the constraints of temp letter at least one time
        if (constrTab[i]->found) {
            int count = 0;
            int flag;
            previous = NULL;
            node = first_constr;

            while (node != NULL) {

                //case 1: I don't have the letter in j position
                flag = 0;
                tempCount = constrTab[i]->count;
                for (int j = 0; j < dim; j++) {
                    if ((node->string[j] == temp && ((constrTab[i] -> not_in[j] != -1 || constrTab[i] -> isIn == false))) || (constrTab[i] -> found_in[j] == 0 && node->string[j] != temp )) {
                        printableDim--;
                        node->printable = false;
                        if (count == 0) {
                            first_constr = first_constr->constr_next;
                            count = -1;
                            flag = 1;
                            break;
                        } else {
                            previous->constr_next = node->constr_next;
                            node = previous;
                            flag = 1;
                            break;
                        }
                    }
                    //count occurrences
                    if (node->string[j] == temp &&
                        (constrTab[i]->found_in[j] == 0 || (constrTab[i]->not_in[j] != 0)) && constrTab[i]->count > 0)
                        tempCount--;
                }
                //case 2
                if (constrTab[i]->count > 0 && flag != 1) {

                    //temp is not in the word (the temporary occurrences didn't change)
                    if (tempCount == constrTab[i]->count) {
                        printableDim--;
                        node->printable = false;
                        if (count == 0) {
                            first_constr = first_constr->constr_next;
                            count = -1;
                        } else {
                            previous->constr_next = node->constr_next;
                            node = previous;
                        }
                        flag = 2;
                    }
                    //I found the exact number of occurrences and doesn't match in the word
                    if (flag !=2 && (tempCount != 0 && constrTab[i]->occ == constrTab[i]->count && constrTab[i]->count_not_in > 0)) {
                        printableDim--;
                        node->printable = false;
                        if (count == 0) {
                            first_constr = first_constr->constr_next;
                            count = -1;
                        } else {
                            previous->constr_next = node->constr_next;
                            node = previous;
                        }
                        flag = 2;
                    }
                    //the word has fewer occurrences of temp than r
                    if (flag !=2 && tempCount > 0 && constrTab[i]->occ >= constrTab[i]->count ) {
                        printableDim--;
                        node->printable = false;

                        if (count == 0) {
                            first_constr = first_constr->constr_next;
                            count = -1;
                        } else {
                            previous->constr_next = node->constr_next;
                            node = previous;
                        }
                    }
                }
                count++;
                previous = node;
                if(node -> constr_next == NULL) last_constr = node;
                node = node->constr_next;
            }


        }
    }


}


//function to read one line of the code, also it tells when to stop the program and starts a match (maybe I should split it)
    int readLine(char *buffer, char *r, int *attempts) {
        char c;
        c = (char) getchar_unlocked();
        int count = 0;


//THE INPUT IS A COMMAND
        if (c == '+') {
            c = (char) getchar_unlocked();
            //start a new game command
            if (c == 'n') {
                resetConstraints();
                //read the reference word
                while (c != '\n' && count < dim) {
                    c = (char) getchar_unlocked();
                }
                c = (char) getchar_unlocked();
                while (c != '\n' && count < dim) {
                    r[count] = c;
                    constrTab[hash_constr(c)]->occ++;
                    count++;
                    c = (char) getchar_unlocked();
                }
                if (count != 0) {
                    r[count] = '\0';
                }

                //read the maximum attempts
                if(scanf("%d", &*attempts)){}
                getchar_unlocked();
                return 0;
            }
                //read print "eligible" words command
            else if (c == 's') {
                while (c != '\n') {
                    c = (char) getchar_unlocked();
                }
                return 4;
            } else {
                for (int i = 0; i < 9; i++) { //da capire se si può fare meglio
                    getchar();
                }
                //read insert new words command
                if (getchar_unlocked() == 'i') {
                    while (c != '\n') {
                        c = (char) getchar();
                    }
                    return -1;
                }
                    //read stop inserting new words command
                else {
                    while (c != '\n') {
                        c = (char) getchar_unlocked();
                    }
                    //TO INSERT:checkConstraints
                    filter();
                    return 2;
                }
            }
        }

        //the game is ended
        if (c == EOF) return 99;

//THE INPUT IS A STRING THAT NEEDS TO BE ADDED INTO THE HASH TABLE
        while (c != '\n' && count < dim) {
            buffer[count] = c;
            count++;
            c = (char) getchar_unlocked();
        }
        if (count != 0) {
            buffer[count] = '\0';
        }
        return 3;
    }

/*------------------------------------------------------MAIN----------------------------------------------------------*/
    int main() {
    node *temp = NULL;
    //initialize flags
    bool playing = false, memorize = true;
    unsigned int phase, count = 0, flag = 0;
    int attempts = 0;

    //memorize the words length
    if(scanf("%d", &dim)){}
    getchar_unlocked();
    char r[dim], out[dim], buffer[dim];

    //initialize constraint table
    createConstr();

    //read the first word or command
    phase = readLine(buffer, r, &attempts);

    while (phase != 4) {
        //GIOCO
        if (strlen(buffer) > 0) {
            //phase = gamePhase(r,buffer,&attempts);
            switch (phase) {
                case 0: //nuova partita (già memorizzati r e attempts)
                    resetBST(T);
                    playing = true;
                    memorize = false;
                    count = 0;
                    break;
                case 1: //inserisci inizio
                    playing = false;
                    memorize = true;
                    break;
                case 2: //inserisci fine
                    //filter(hashTab, T, (int) dim);
                    playing = true;
                    memorize = false;
                    filter();
                    break;
                case 3: //ho letto una parola
                    if (playing == true && count <= attempts &&
                        memorize == false) {//la parola che ho letto è da comparare a quella di riferimento
                        temp = search(T, buffer);

                        if (temp == NULL) {
                            printf("not_exists\n");
                            count --;
                        } else {
                            output(buffer, r, out);
                            manageConstraints(buffer, out);
                            filter();
                            for (int k = 0; k < strlen(r); k++) {
                                if (out[k] != '+') {
                                    flag = 1;
                                    break;
                                }
                            }
                            if (flag == 0) {
                                printf("ok\n");
                                resetConstraints();
                                playing = false;
                                memorize = false;
                                break;
                            }
                            printf("%s\n%d\n", out, printableDim);
                            //printFiltered(T);
                            flag = 0;
                            //break;
                        }
                        count++;
                    }
                    if (count == attempts && memorize == false) { //ho terminato i tentativi a disposizione
                        playing = false;
                        //memorize = false;
                        printf("ko\n");
                        resetConstraints();
                        count = 0;
                    }
                    if (memorize == true) { //devo inserire la parola
                        T = insertBST(T, buffer);
                    }
                    break;
                case 4://STAMPA FILTRATE
                    //printf("stampa filtrate\n");
                    printFiltered(T);
                    break;

                default:
                    printf("problema nel gioco");
                    return 1;
            }
        }
        phase = readLine(buffer, r, &attempts);
    }
    return 0;
}
