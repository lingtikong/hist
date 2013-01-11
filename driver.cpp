#include "stdlib.h"
#include "stdio.h"
#include <cstring>
#include "math.h"
#include "driver.h"
#include "histogram.h"

#define MAXLINE 512
#define ZERO    1.e-14

/* ----------------------------------------------------------------------
   Driver, to read all files and throw data into the histogram calculator
------------------------------------------------------------------------- */
Driver::Driver(int narg, char **arg)
{
  int flag = 0;
  int zeroflag = 0, pairflag = 0;
  int pbcflag = 0, vflag = 0, sflag = 0;
  double stepsize = 0.;
  double pstr=0., pend=0.;
  int iarg = 1, ikey=1, ivalue=1;

  char *fout = NULL;

  while (iarg < narg){
    if (strcmp(arg[iarg],"-w") == 0){ // key is string
      flag &= (~FloatKey);

    } else if (strcmp(arg[iarg],"-f") == 0){ // key is number
      flag |= FloatKey;

    } else if (strcmp(arg[iarg],"-s") == 0){ // stepsize if key is number
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      stepsize = atof(arg[iarg]); sflag = 1;
      if (fabs(stepsize) < ZERO) {DispHelp(arg[0]); return;}

    } else if ( strcmp(arg[iarg],"-k")==0 ){ // key column
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      ikey = atoi(arg[iarg]);

    } else if ( strcmp(arg[iarg],"-v")==0 ){ // value column; for string and number, ivalue = ikey
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      ivalue = atoi(arg[iarg]); vflag = 1;

    } else if ( strcmp(arg[iarg],"-p")==0 ){ // periodic boundary condition for number and number pairs
      flag |= PBC4Key;
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      pstr = atof(arg[iarg]);
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      pend = atof(arg[iarg]);

    } else if ( strcmp(arg[iarg],"-z")==0 ){ // add empty bins as zero
      flag |= ZeroPad;

    } else if (strcmp(arg[iarg],"-h")==0){
      DispHelp(arg[0]); return;

    } else if (strcmp(arg[iarg],"-o")==0){
      if (++iarg >= narg) {DispHelp(arg[0]); return;}
      if (fout) delete []fout;
      fout = new char [strlen(arg[iarg])+1];
      strcpy(fout, arg[iarg]);

    } else break;

    iarg++;
  }

  if (!vflag) ivalue = ikey;
  if (ikey != ivalue) flag |= PairData;

  if ((flag & FloatKey) && (!sflag)){DispHelp(arg[0]); return;}

  if (iarg >= narg){DispHelp(arg[0]); return;}
  if (pend < pstr) flag &= (~PBC4Key);

  if (fout == NULL){
    fout = new char [9];
    strcpy(fout, "hist.dat");
  }

  FILE *fp;
  char str[MAXLINE];
  std::string item;
  double key, value;

  double dflag[3];
  dflag[0] = stepsize; dflag[1] = pstr; dflag[2] = pend;

  Histogram *hist = new Histogram(flag, &dflag[0]);

  // to read all files
  while (iarg < narg){
    fp = fopen(arg[iarg], "r");
    if (fp == NULL){++iarg; continue;}
   
    // to read the current file
    fgets(str, MAXLINE, fp);
    while ( ! feof(fp) ){
      char *ptr = strchr(str,'#');   if ( ptr ) *ptr = '\0';
      ptr = strtok(str," \t\n\r\f"); if ( ptr == NULL) continue;

      int hit = 0, n = 1;
      while ( (ptr != NULL) && (hit != 3)){
        if (n == ikey){
          item.assign(ptr);
          key = atof(ptr);
          hit |= 1;
        }
        if (n == ivalue){
          value = atof(ptr);
          hit |= 2;
        }
        n++;

        ptr = strtok(NULL," \t\n\r\f");
      }

      if (hit == 3){ 
        if (flag & FloatKey){
          if (flag & PairData) hist->AddValue(key, value);
          else hist->AddValue(value);
        } else {
          if (flag & PairData) hist->AddValue(item, value);
          else hist->AddValue(item);
        }
      }

      fgets(str, MAXLINE, fp);
    }
    fclose(fp);
    ++iarg;
  }

  // output histogram
  hist->Output(fout);
  delete hist;

return;
}

Driver::~Driver()
{
}

/* ----------------------------------------------------------------------
   To display the help info
------------------------------------------------------------------------- */
void Driver::DispHelp(const char * cmd)
{
  printf("\n%s: Program to calculate the histogram of one column of data in one or many files.\n", cmd);
  printf("\nUsage: %s [option] file1 [file2 file3 ...]\n", cmd);
  printf("------------------------------------------------------------------------------------------------\n");
  printf("  Options:\n");
  printf("    -w            : Indicating that the key data are strings; default.\n");
  printf("    -f            : Indicating that the key data are numbers;\n");
  printf("    -s step       : To define the stepsize if key data are numbers; must be set if `-f` is set.\n");
  printf("    -k key-col    : To specify the column number of the key, by default it is the first column.\n");
  printf("    -v value-col  : To specify the column number of the value, by default: same as key-col.\n");
  printf("    -z            : To specify that empty bins should be output as zero, works only if `-f` is set;\n");
  printf("                    default: not set.\n");
  printf("    -p pstr pend  : To specify that the key column has periodic boundary condition, and \n");
  printf("                    the range of the period is given by the following arguments; works only if `-f` is set.\n");
  printf("    -o outfile    : To define the output filename; default: hist.dat\n");
  printf("    -h            : To display this help info.\n");
  printf("\n  Command line parameters:\n");
  printf("    file1         : the file which contains the data to be analysed, one and at least one\n");
  printf("                    or several files might be supplied.\n");
  printf("------------------------------------------------------------------------------------------------\n\n");
return;
}
