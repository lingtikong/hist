#include "stdlib.h"
#include "stdio.h"
#include "histogram.h"
#include <cstring>
#include "math.h"

#define MAXLINE 256
/* ---------------------------------------------------------------------- */

Histogram::Histogram(int *iflag, double *dflag)
{
  HasItem.clear();
  Item2Index.clear();
  Index2Item.clear();
  ItemCount.clear();
  HasNum.clear();
  HitSum.clear();
  HitSum2.clear();
  
  nitem = nuniq = 0;
  datatype = iflag[0];
  zeroflag = iflag[1];
  pbcflag  = iflag[2];
  stepsize = dflag[0];
  pstr     = dflag[1];
  pend     = dflag[2];
  prd      = pend - pstr;

  if (pbcflag && (stepsize > prd)) pbcflag = 0;
  if (pbcflag == 0) pstr = pend = 0.;
      
  if (datatype){
    halfstep = 0.5*stepsize;
    inv_step = 1./stepsize;
  }

return;
}

/* ---------------------------------------------------------------------- */

Histogram::~Histogram( )
{
  nitem = nuniq = 0;
  HasItem.clear();
  Item2Index.clear();
  Index2Item.clear();
  ItemCount.clear();
  HasNum.clear();
  HitSum.clear();
  HitSum2.clear();
}

/* ----------------------------------------------------------------------
   To add one item into the histogram calculation
------------------------------------------------------------------------- */

void Histogram::AddValue(const std::string itemstr)
{
  int id;
  HasItem[itemstr] = 1;
  if ( (int)HasItem.size() > nuniq ){
    id = ++nuniq;
    Item2Index[itemstr] = id;
    Index2Item[id] = itemstr;
    ItemCount[id] = 0;
  } else id = Item2Index[itemstr];

  ItemCount[id] ++;
  nitem ++;
}

/* ----------------------------------------------------------------------
   To add one item into the histogram calculation; numbers
------------------------------------------------------------------------- */

void Histogram::AddValue(double value)
{
  int id;
  if (pbcflag){
    double dr = value + halfstep - pstr;
    while (dr > prd) dr -= prd;
    while (dr < 0.)  dr += prd;
    id = dr * inv_step;
  } else id = (value + halfstep) * inv_step;
  HasNum[id] = 1;
  if ( (int)HasNum.size() > nuniq ){
    ++nuniq;
    ItemCount[id] = 0;
  }

  ItemCount[id]++;
  nitem ++;
}

/* ----------------------------------------------------------------------
   To add one item into the histogram calculation; number pairs
------------------------------------------------------------------------- */

void Histogram::AddValue(double pos, double value)
{
  int id;
  if (pbcflag){
    double dr = pos + halfstep - pstr;
    while (dr > prd) dr -= prd;
    while (dr < 0.)  dr += prd;
    id = dr * inv_step;
  } else id = (pos + halfstep) * inv_step;

  HasNum[id] = 1;
  if ( (int)HasNum.size() > nuniq ){
    ++nuniq;
    ItemCount[id] = 0;
    HitSum[id] = 0.;
    HitSum2[id] = 0.;
  }
  
  HitSum[id]  += value;
  HitSum2[id] += value*value;
  ItemCount[id]++;
  nitem ++;
}
/* ----------------------------------------------------------------------
   To output the resultant histogram
------------------------------------------------------------------------- */

void Histogram::Output(char *fname)
{
  char str[MAXLINE];
  double fac = 1./double(nitem>0?nitem:1);
  
  FILE *fp = fopen(fname, "w");
  if (fp == NULL){printf("\nError while opening file %s! Outputing stopped.\n", fname); return;}

  if (datatype == 0){ // string case
    fprintf(fp,"#index Item Counts weight\n");
    Sort();
    for (int id=1; id<= nuniq; id++){
      int num = ItemCount[id];
      fprintf(fp,"%d %s %d %lg\n", id, Index2Item[id].c_str(), num, double(num)*fac);
    }
  } else if (datatype == 1){ // number case

    fprintf(fp,"#index Item Counts weight\n");
    int ic = 0;
    std::map<int,int>::iterator it;
    int inext = ItemCount.begin()->first;
    for (it = ItemCount.begin(); it != ItemCount.end(); it++){
      int id = it->first, num = it->second;
      if (zeroflag){ // insert empty bins
        for (int ik=inext; ik<id; ik++) fprintf(fp,"%d %lg %d %lg\n", ++ic, double(ik)*stepsize+pstr, 0, 0.);
        inext = id+1;
      }
      fprintf(fp,"%d %lg %d %lg\n", ++ic, double(id)*stepsize+pstr, num, double(num)*fac);
    }
  } else if (datatype == 2){ // number pair case

    fprintf(fp,"#index position AveValue StdEr Counts weight\n");
    int ic = 0;
    double ave, stdv;
    std::map<int,int>::iterator it;
    int inext = ItemCount.begin()->first;
    for (it = ItemCount.begin(); it != ItemCount.end(); it++){
      int id = it->first, num = it->second;
      if (zeroflag){ // insert empty bins
        for (int ik = inext; ik<id; ik++)
          fprintf(fp,"%d %lg %lg %lg %d %lg\n", ++ic, double(ik)*stepsize+pstr, 0., 0., 0, 0.);
        inext = id+1;
      }
      ave = HitSum[id]/double(num);
      if (num > 2) stdv = sqrt((HitSum2[id]-double(num)*ave*ave)/double(num-1));
      else stdv = 0.;
      fprintf(fp,"%d %lg %lg %lg %d %lg\n", ++ic, double(id)*stepsize+pstr, ave, stdv, num, double(num)*fac);
    }
  }
  fclose(fp);
}

/* ----------------------------------------------------------------------
   To sort the histogram by descending order
------------------------------------------------------------------------- */

void Histogram::Sort()
{
  for (int i=1; i<nuniq; i++){
    for (int j=i+1; j<= nuniq; j++){
      if (ItemCount[j] > ItemCount[i]){
        int num = ItemCount[i];
        ItemCount[i] = ItemCount[j];
        ItemCount[j] = num;
        
        std::string stri = Index2Item[i];
        std::string strj = Index2Item[j];
        Index2Item[i] = strj;
        Index2Item[j] = stri;
        Item2Index[stri] = j;
        Item2Index[strj] = i;
      }
    }
  }
return;
}
