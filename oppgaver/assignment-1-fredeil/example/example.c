// https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_73/apis/users_86.htm

#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>

pthread_rwlock_t       rwlock;

void *rdlockThread(void *arg)
{
  int rc;

  printf("Entered thread, getting read lock\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  printf("pthread_rwlock_rdlock()\n");
  printf("got the rwlock read lock\n");

  sleep(5);

  printf("unlock the read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  printf("pthread_rwlock_unlock()\n");
  printf("Secondary thread unlocked\n");
  return NULL;
}

void *wrlockThread(void *arg)
{
  int rc;

  printf("Entered thread, getting write lock\n");
  rc = pthread_rwlock_wrlock(&rwlock);
  printf("pthread_rwlock_wrlock()\n");

  printf("Got the rwlock write lock, now unlock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  printf("pthread_rwlock_unlock()\n");
  printf("Secondary thread unlocked\n");
  return NULL;
}

int main(int argc, char **argv)
{
  int                   rc=0;
  pthread_t             thread, thread1;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Main, initialize the read write lock\n");
  rc = pthread_rwlock_init(&rwlock, NULL);
  printf("pthread_rwlock_init()\n");

  printf("Main, grab a read lock\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  printf("pthread_rwlock_rdlock()\n",rc);

  printf("Main, grab the same read lock again\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  printf("pthread_rwlock_rdlock() second\n");

  printf("Main, create the read lock thread\n");
  rc = pthread_create(&thread, NULL, rdlockThread, NULL);
  printf("pthread_create\n");

  printf("Main - unlock the first read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  printf("pthread_rwlock_unlock()\n");

  printf("Main, create the write lock thread\n");
  rc = pthread_create(&thread1, NULL, wrlockThread, NULL);
  printf("pthread_create\n");

  sleep(5);
  printf("Main - unlock the second read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  printf("pthread_rwlock_unlock()\n");

  printf("Main, wait for the threads\n");
  rc = pthread_join(thread, NULL);
  printf("pthread_join\n");

  rc = pthread_join(thread1, NULL);
  printf("pthread_join\n");

  rc = pthread_rwlock_destroy(&rwlock);
  printf("pthread_rwlock_destroy()\n");

  printf("Main completed\n");
  return 0;
}
