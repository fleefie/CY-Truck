#include "process.h"
#include "FIFO.h"



DataLine* init_ReadLine(FILE* fFile){
  if(fFile == NULL){
    printf("%s: pFile isn't properly given", __func__);
    exit(4);
  }

  // Set the cursor to the start af the file
  rewind(fFile);

  // Get rid of the first line
  while(fgetc(fFile) != '\n');

  DataLine* pNew = NULL;
  if((pNew = malloc(sizeof(DataLine))) == NULL){
    printf("%s: pNew allocation didn't work", __func__);
    exit(5);
  }

  pNew->route_ID = 0;
  pNew->step_ID = 0;
  pNew->town_A[0] = '\0';
  pNew->town_B[0] = '\0';
  pNew->distance = 0.0;
  pNew->name[0] = '\0';

  return pNew;
}

int readLine(FILE* fFile, DataLine* pLine){
  if(fFile == NULL){
      printf("%s: fFile isn't properly given", __func__);
      exit(4);
  }
  if(pLine == NULL){
      printf("%s: pLine isn't properly given", __func__);
      exit(4);
  }
  
  char c = 0;       // caracter being read
  char string[100]; // column being read
  int i = 0;        // 'c' caracter position in 'string'
  int column = 0;

  while(c != '\n' && c != EOF){
    c = getc(fFile);
      
    if(c == ';' || c == '\n' || c == EOF){
    string[i] = '\0';
    
    switch (column) {
    case ROUTE_ID:
        pLine->route_ID = atol(string);
        break;
    case STEP_ID:
        pLine->step_ID = atoi(string);
        break;
    case TOWN_A:
        sprintf(pLine->town_A, "%s", string);
        break;
    case TOWN_B:
        sprintf(pLine->town_B, "%s", string);
        break;
    case DISTANCE:
        pLine->distance = atof(string);
        break;
    case NAME:
        sprintf(pLine->name, "%s", string);
        break;
    }

    if(c == EOF){
      string[i] = c; // A little complicated way to know when we trully hit the EOF
    }
      
    column++;
    i=-1;
    }
    else{
      string[i] = c;
    }

    i++;
    if(i >= 100){
      printf("%s: The number of caracter of the column %d execced 100", __func__, column++);
      exit(6);
    }
  }

  return (string[0] != EOF); // Part of a little complicated way to know when we trully hit the EOF
}

// Holy crap finally some good code
void S_Init(FILE* fData){
  if (fData == NULL) {
    exit(122);
  }
  AvlRoute* pRoute = NULL;
  DataLine* pLine = init_ReadLine(fData);
  int *h = 0;
  AvlRoute* pNew = malloc(sizeof(AvlRoute));
  if (checkPtr(pNew)) exit (1);

  // 
  // PASS 1 : FILL THE AVL
  //
  int DEBUG=0;
  while (readLine(fData, pLine)) {
    // read the current line
    AvlRoute* pTemp = isInAvlRoute(pRoute, pLine->route_ID);

    // Insert the data into the tree
    if (pRoute == NULL) {
      pRoute = createAvlRoute(pLine->route_ID);
      pRoute->nSteps++;
      pRoute->distTot = pLine->distance;
      pRoute->distMax = pLine->distance;
      pRoute->distMax = pLine->distance;
    } else if (pTemp == NULL) {
      pRoute = addAvlRoute(pRoute, pLine->route_ID, pNew);
      pNew->nSteps++;
      pNew->distTot = pLine->distance;
      pNew->distMax = pLine->distance;
      pNew->distMin = pLine->distance;
    } else {
      pTemp->nSteps++;
      pTemp->distTot += pLine->distance;
      if ( pTemp->distMax < pLine->distance ) {
        pTemp->distMax += pLine->distance;
      }
      if ( pTemp->distMin > pLine->distance ) {
        pTemp->distMin = pLine->distance;
      }
    }
    printf("DEBUG %i\n", ++DEBUG);
    printf("nSteps: %i, distTot: %f, distMax: %f, distMin: %f\n", pRoute->nSteps, pRoute->distTot, pRoute->distMax, pRoute->distMin);
  }
  //
  // PASS 2 : COMPUTE AVERAGES AND PRINT
  //
  S_Process(pRoute);
}

void S_Process(AvlRoute* pRoute) {
  if ( pRoute == NULL ) exit (200);
  // LEFT CHILD
  printf("DEBUG : SENT TO LEFT CHILD\n");
  if ( pRoute->pL != NULL ) S_Process(pRoute->pL);
  
  // ROOT
  printf("DEBUG : PROCESSING ROOT\n");
  printf("%ld %f %f %f\n", pRoute->id, pRoute->distMin, pRoute->distTot / pRoute->nSteps, pRoute->distMax);

  // RIGHT CHILD
  printf("DEBUG : SENT TO RIGHT CHILD\n");
  if ( !checkPtr(pRoute->pR) ) S_Process(pRoute->pR);
}

// RIPBOZO PACKWATCH REST IN PISS YOU WONT BE MISSED
// AWK REPLACED YOU YOU USELESS PILE OF HACKY CRAP
/*AvlDriver *D2(FILE *fData) {
  if (fData == NULL) {
    exit(122);
  }
  AvlDriver *pDriver = NULL;
  DataLine* pLine =  init_ReadLine(fData);
  int *h = 0;
  AvlDriver* pNew = malloc(sizeof(AvlDriver*));
  if (checkPtr(pNew)) exit (1);
  while(readLine(fData, pLine)){
    char * copy = malloc(sizeof(char)*32);
    copy = strcpy(copy, pLine->name);
    AvlDriver *pTemp = isInAvlDriver(pDriver,copy);
    if(pDriver == NULL){
      //printf("DEBUG1\n");
      pDriver = createAvlDriver(copy);
      pDriver->totalDist = pLine->distance;
      // printf("%s,%f\n",pDriver->name,pDriver->totalDist);
      //printf("DEBUG2\n");
    } else if (pTemp == NULL){
      //printf("DEBUG3\n");
      pDriver = addAvlDriver(pDriver, copy, pNew);
      //printf("%s\n", pDriver->pL->name);
      //printf("DEBUG10\n");
      // TODO FIX THIS THIS IS AWFUL AND INEFFICIENT
      // pTemp = isInAvlDriver(pDriver, copy);
      //printf("DEBUG11\n");
      if(checkPtr(pNew)) exit (69);
      pNew->totalDist = pLine->distance;
      //printf("DEBUG12\n");
      // printf("%s,%f\n", pTemp->name, pTemp->totalDist);
      //printf("DEBUG4\n");
    } else {
        //printf("DEBUG5\n");
        pTemp->totalDist += pLine->distance;
        // printf("%s,%f\n",pTemp->name,pTemp->totalDist);
        //printf("existe deja\n");
        //printf("DEBUG6\n");
    }
  }
  infixe(pDriver);
  free(pLine);
}
*/