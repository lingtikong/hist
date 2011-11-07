#include "driver.h"
#include "stdlib.h"
#include "stdio.h"
#include <cstring>

#define MAXLINE 512

int main (int narg, char **arg)
{
  Driver *driver = new Driver(narg, arg);

  delete driver;
return 0;
}
