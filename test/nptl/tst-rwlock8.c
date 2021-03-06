/* Test program for timedout read/write lock functions.
   Copyright (C) 2000, 2003 Free Software Foundation, Inc.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2000.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#if defined(__GLIBC__) || defined(__UCLIBC__)
#define NWRITERS 15
#define WRITETRIES 10
#define NREADERS 15
#define READTRIES 15

#define DELAY   1000000

#ifndef INIT
# define INIT PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP
#endif

static pthread_rwlock_t lock = INIT;


static void *
writer_thread (void *nr)
{
  struct timespec delay;
  int n;

  delay.tv_sec = 0;
  delay.tv_nsec = DELAY;

  for (n = 0; n < WRITETRIES; ++n)
    {
      printf ("writer thread %ld tries again\n", (long int) nr);

      if (pthread_rwlock_wrlock (&lock) != 0)
	{
	  puts ("wrlock failed");
	  exit (1);
	}

      printf ("writer thread %ld succeeded\n", (long int) nr);

      nanosleep (&delay, NULL);

      if (pthread_rwlock_unlock (&lock) != 0)
	{
	  puts ("unlock for writer failed");
	  exit (1);
	}

      printf ("writer thread %ld released\n", (long int) nr);
    }

  return NULL;
}


static void *
reader_thread (void *nr)
{
  struct timespec delay;
  int n;

  delay.tv_sec = 0;
  delay.tv_nsec = DELAY;

  for (n = 0; n < READTRIES; ++n)
    {
      printf ("reader thread %ld tries again\n", (long int) nr);

      if (pthread_rwlock_rdlock (&lock) != 0)
	{
	  puts ("rdlock failed");
	  exit (1);
	}

      printf ("reader thread %ld succeeded\n", (long int) nr);

      nanosleep (&delay, NULL);

      if (pthread_rwlock_unlock (&lock) != 0)
	{
	  puts ("unlock for reader failed");
	  exit (1);
	}

      printf ("reader thread %ld released\n", (long int) nr);
    }

  return NULL;
}
#endif

static int
do_test (void)
{
#if defined(__GLIBC__) || defined(__UCLIBC__)
  pthread_t thwr[NWRITERS];
  pthread_t thrd[NREADERS];
  int n;
  void *res;

  /* Make standard error the same as standard output.  */
  dup2 (1, 2);

  /* Make sure we see all message, even those on stdout.  */
  setvbuf (stdout, NULL, _IONBF, 0);

  for (n = 0; n < NWRITERS; ++n)
    if (pthread_create (&thwr[n], NULL, writer_thread,
			(void *) (long int) n) != 0)
      {
	puts ("writer create failed");
	exit (1);
      }

  for (n = 0; n < NREADERS; ++n)
    if (pthread_create (&thrd[n], NULL, reader_thread,
			(void *) (long int) n) != 0)
      {
	puts ("reader create failed");
	exit (1);
      }

  /* Wait for all the threads.  */
  for (n = 0; n < NWRITERS; ++n)
    if (pthread_join (thwr[n], &res) != 0)
      {
	puts ("writer join failed");
	exit (1);
      }
  for (n = 0; n < NREADERS; ++n)
    if (pthread_join (thrd[n], &res) != 0)
      {
	puts ("reader join failed");
	exit (1);
      }

  return 0;
#else
  return 23;
#endif
}

#define TIMEOUT 30
#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
