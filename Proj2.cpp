/*
 * FILE: planeTest.cpp
 * Tyler Ward
 * DATE: 11/12/2024
 */
#include "AirportAnimator.hpp"
#include <unistd.h>
#include <sys/sem.h>
#include <cstdlib>
#include <iostream>

#define RUNWAY 0
#define TOURSSEM 1
#define PASSENGERSEM 2

using namespace std;

int avaliablePassengers, totalTours, totalCompletedTours = 0;
int sem_id;

int initializeSet(int key,int initialValue){
  //creates semephore set with 4 semephores
  sem_id = semget(key, 4, 0);
  if (sem_id == -1){
    sem_id = semget(key, 4, IPC_CREAT|IPC_EXCL | 0666);
      if (sem_id ==-1){
        cerr << "semget() failed with errno = " << errno <<endl;
        exit(2);
      }
  }

  //initialize semephores with initial values
  for (int i = 0; i < 3; i++){
    if(semctl(sem_id, i, SETVAL, initialValue)==-1){
      cerr <<"semctl()failed during initialization with errno = "<<errno<<endl;
      exit(3);
    }
  }

  if(semctl(sem_id, PASSENGERSEM, SETVAL, avaliablePassengers) == -1){
    cerr <<"semctl()failed during PASSENGERSEM initalization with errno = "<<errno<<endl;
  }
  return sem_id;
}

void waitSemaphore(int semNum){
  struct sembuf sb = {semNum,-1,0};
  if (semop(sem_id, &sb, 1)== -1){
    cerr << "semop() failed with errno= " <<errno <<endl;
    exit(3);
  }
}

void signalSemaphore(int semNum){
  struct sembuf sb = {semNum,1,0};
  if (semop(sem_id, &sb, 1)== -1){
    cerr << "semop() failed with errno= " <<errno <<endl;
    exit(4);
  }
}

void* planeSym(void* arg){
  unsigned int planeNum = *(unsigned int*)arg;
  delete (unsigned int*)arg;

  for(; totalCompletedTours <= totalTours;){
  // Boarding passengers
    int boardedPassengers = 0;
    
    
    for (;boardedPassengers<10;){
      waitSemaphore(PASSENGERSEM);
      if(avaliablePassengers >= 1 && boardedPassengers < 10){
        --avaliablePassengers;
        ++boardedPassengers;
        signalSemaphore(PASSENGERSEM);
        sleep(rand()%3);
      
      //update pass count
      AirportAnimator::updatePassengers(planeNum, boardedPassengers);
      sleep(rand()%3);
      
      } else{
        signalSemaphore(PASSENGERSEM);
      } 
    }

    //taxi
    AirportAnimator::updateStatus(planeNum, "TAXI");
    AirportAnimator::taxiOut(planeNum);

    //take off
    waitSemaphore(RUNWAY);
    AirportAnimator::updateStatus(planeNum, "TKOFF");
    AirportAnimator::takeoff(planeNum);
    signalSemaphore(RUNWAY);

    //tour
    AirportAnimator::updateStatus(planeNum, "TOUR");
    sleep(5+rand()%41);
    AirportAnimator::updateStatus(planeNum, "LNDRQ");

    //land
    waitSemaphore(RUNWAY);
    AirportAnimator::updateStatus(planeNum, "LAND");
    AirportAnimator::land(planeNum);
    signalSemaphore(RUNWAY);

    //taxi
    AirportAnimator::updateStatus(planeNum, "TAXI");
    AirportAnimator::taxiIn(planeNum);

    //depln
    AirportAnimator::updateStatus(planeNum, "DEPLN");
    for(int passenger = 10; passenger> 0; --passenger){
      waitSemaphore(PASSENGERSEM);
      avaliablePassengers++;
      AirportAnimator::updatePassengers(planeNum, passenger-1);
      signalSemaphore(PASSENGERSEM);
      sleep(1);
    }
    AirportAnimator::updateStatus(planeNum, "BOARD");

    //update
    waitSemaphore(TOURSSEM);
    totalCompletedTours++;
    AirportAnimator::updateTours(totalCompletedTours);
    signalSemaphore(TOURSSEM);

  }
  AirportAnimator::updateStatus(planeNum, "DONE");
  return nullptr;
}

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    cerr <<"Usage: " <<argv[0] << "<totalPassengers> <totalTours>" << endl;
    return 1;
  }
  avaliablePassengers = atoi(argv[1]);
  totalTours = atoi(argv[2]);

  //initialize sem set 
  initializeSet(1234, 1);

  AirportAnimator::init();

  //creates 8 threads for the planes
  pthread_t planes[8];
  for (int i =0; i<8; ++i){
    unsigned int* planeNum = new unsigned int(i);
    pthread_create(&planes[i], nullptr, planeSym, planeNum);
  }

  for(int i=0; i<8;++i){
    pthread_join(planes[i], nullptr);
  }

  AirportAnimator::end();

  cout<<"Total tours: " <<totalCompletedTours<<endl;
  

  semctl(sem_id, 0, IPC_RMID); //removes sem set
  return 0;
}
